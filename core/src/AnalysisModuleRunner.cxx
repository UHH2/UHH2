#ifndef CMSSW

#include "UHH2/core/include/AnalysisModuleRunner.h"
#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/Utils.h"
#include "UHH2/core/include/registry.h"
#include "UHH2/core/include/root-utils.h"

#include "TH1.h"
#include "TXMLNode.h"
#include "TXMLAttr.h"
#include "TTree.h"

#include <stdexcept>
#include <memory>

using namespace std;
using namespace uhh2;

namespace{

void write_type_info(ostream & out, TClass * class_, EDataType type_){
    if(class_){
        out << class_->GetName();
        out << "(c++ name: '" << demangle(class_->GetTypeInfo()->name()) << "')";
    }
    else{
        auto tn = TDataType::GetTypeName(type_);
        if(tn == string("")){
            out << "(type not known to ROOT!)";
        }
        else{
            out << tn;
        }
    }
}

}

namespace uhh2 {


/** \brief The SFrame specific implementation for the Context
 * 
 * See the Context class for a description of the methods.
 * 
 * This class is intended to be used from a SCycle; see AnalysisModuleRunner for
 * an example use:
 *  - create a new instance of SFrameContext in SCycle::BeginInputData and set the dataset_type and dataset_version
 *    according to the current dataset
 *  - call SFrameContext::begin_input_file from SCycle::BeginInputFile to make sure that input addresses are set up correctly.
 */
class SFrameContext: public uhh2::Context{
public:
    SFrameContext(SCycleBase & base, const SInputData& sin, GenericEventStructure & es);
    
    virtual void put(const std::string & path, TH1 * t) override;
    
    ~SFrameContext();
    
    // this should be called from SCycleBase::BeginInputFile. It takes care
    // of setting up the input addresses into the event.
    void begin_input_file(Event & event);
    void begin_event(Event & event);
    void setup_output(Event & event); // should be called after processing the first event

private:
    
    // event tree i/o
    // note that the declare_input does nothing but saving the parameters for later; the actual setup of
    // input branches is done in begin_input_file.
    virtual void do_declare_event_input(const std::type_info & ti, const std::string & bname, const std::string & mname) override;
    virtual void do_declare_event_output(const std::type_info & ti, const std::string & bname, const std::string & mname) override;
    
    // other tree output:
    //virtual void do_declare_output(const std::string & treename, const std::string & branchname, const void * addr, const type_info & ti) override;
    //virtual void write_entry(const std::string & treename) override;
    
    TDirectory * get_tmpdir();
    
    SCycleBase & base;
    std::string event_treename;
    
    // input (event) tree stuff:
    TTree * input_tree;
    // input event branch name to branch info:
    struct branchinfo {
        TBranch * branch;
        void * addr; // memory belongs to branchinfo, not managed by Event
        boost::optional<std::function<void (void*)> > eraser; // eraser for addr
        const type_info & ti; // this is always a non-pointer type.
        GenericEvent::RawHandle handle;
        
        branchinfo(TBranch * branch_, const type_info & ti_, const GenericEvent::RawHandle & handle_): branch(branch_), addr(0), ti(ti_), handle(handle_){} // eraser is None
        ~branchinfo(){
            if(eraser && addr){
                (*eraser)(addr);
            }
        }
    };
    std::map<std::string, branchinfo> event_input_bname2bi;
    std::map<std::string, branchinfo> event_output_bname2bi;   
    
    // output tree stuff:
    // pointers to the objects of the output tree(s)
    std::list<void*> ptrs;
    std::map<std::string, TTree*> output_trees;
};

}


SFrameContext::SFrameContext(SCycleBase & base_, const SInputData& sin, GenericEventStructure & es_): Context(es_), base(base_) {
    // populate settings:
    vector<pair<string, string> > props = base.GetConfig().GetProperties();
    for(vector<pair<string, string> >::const_iterator it = props.begin(); it != props.end(); ++it){
        set(it->first, it->second);
    }
    set("dataset_type", sin.GetType().Data());
    set("dataset_version", sin.GetVersion().Data());
    set("dataset_lumi", double2string(sin.GetTotalLumi()));
    // we need exactly one input event tree:
    bool found_input_tree = false;
    const map< Int_t, vector<STree> > & tt2trees = sin.GetTrees(); // map of treetype to vector of trees
    for(map< Int_t, vector<STree> >::const_iterator it=tt2trees.begin(); it!=tt2trees.end(); ++it){
        const vector<STree> & trees = it->second;
        for(vector<STree>::const_iterator tree = trees.begin(); tree != trees.end(); ++tree){
            if((tree->type & STree::EVENT_TREE) && (tree->type & STree::INPUT_TREE)){
                if(found_input_tree){
                    throw runtime_error("more than one input event tree");
                }
                found_input_tree = true;
                event_treename = tree->treeName.Data();
            }
        }
    }
    if(!found_input_tree){
        throw runtime_error("no input event tree");
    }
}

    
TDirectory * SFrameContext::get_tmpdir(){
    // see SCycleBaseHist::GetTempDir (which is private ...):
    static TDirectory* tempdir = 0;

    if(!tempdir){
        gROOT->cd();
        tempdir = gROOT->mkdir( "SFrameTempDir" );
        if(!tempdir){
            throw runtime_error( "SFrameTempDir could not be created" );
        }
    }
    return tempdir;
}

void SFrameContext::put(const string & path, TH1 * t){
    // strip leading and/or trailing slash from path.
    string trimmed_path = path;
    trim(trimmed_path, "/");
    
    string fullname = trimmed_path + '/' + t->GetName();
    
    // see implementation of template SCycleBaseHist::Book with inFile=false (which is the default).
    // instead of calling t->Clone, try to call SetDirectory. This is not a method of TObject,
    // so for now, make sure it works for the common cases of a histogram and tree:
    TDirectory * tempdir = get_tmpdir();
    tempdir->cd();
    t->SetDirectory(tempdir);
    SCycleOutput* out = new SCycleOutput(t, fullname.c_str(), trimmed_path.c_str());
    TList * outlist = base.GetHistOutput();
    outlist->AddLast(out);
    gROOT->cd();
}

void SFrameContext::begin_input_file(Event & event){
    input_tree = base.GetInputTree(event_treename.c_str());
    if(input_tree==0){
        throw runtime_error("Could not find input tree '" + event_treename + "'");
    }
    for(auto & name_bi : event_input_bname2bi){
        const string & bname = name_bi.first;
        branchinfo & bi = name_bi.second;
        TBranch * branch = input_tree->GetBranch(bname.c_str());
        if(branch==0){
            throw runtime_error("Could not find branch '" + bname + "' in tree '" + event_treename + "'");
        }
        // now do SetBranchAddress, using address-to-address in case of a class, and simple address otherwise.
        TClass * branch_class = 0;
        EDataType branch_dtype = kNoType_t;
        int res = branch->GetExpectedType(branch_class, branch_dtype);
        if(res > 0){
            throw runtime_error("input branch '" + bname + "': error reading type information");
        }
        
        // get address type:
        TClass * address_class = TBuffer::GetClass(bi.ti);
        EDataType address_dtype = kNoType_t;
        TDataType * address_tdatatype = TDataType::GetDataType(TDataType::GetType(bi.ti));
        if(address_tdatatype){
            address_dtype = EDataType(address_tdatatype->GetType());
        }
        
        // compare if they match; note that it's an error if the class or data type has not been found:
        if((branch_class && address_class) && ((branch_class == address_class) || (branch_class->GetTypeInfo() == address_class->GetTypeInfo()))){
            if(bi.addr == 0){
                bi.addr = branch_class->New();
                bi.eraser = branch_class->GetDelete();
                event.set_unmanaged(bi.ti, bi.handle, bi.addr);
            }
            branch->SetAddress(&bi.addr);
        }
        else if(branch_dtype != kNoType_t && (branch_dtype == address_dtype)){
            if(bi.addr == 0){
                bi.addr = new uint64_t; // largest primitive type
                bi.eraser = [](void * addr){delete reinterpret_cast<uint64_t*>(addr);};
                event.set_unmanaged(bi.ti, bi.handle, bi.addr);
            }
            branch->SetAddress(bi.addr);
        }
        else{ // this is an error:
            stringstream ss;
            ss << "Type error for branch '" << bname << "': branch holds type '";
            write_type_info(ss, branch_class, branch_dtype);
            ss << "', but the type given in declare_input was '";
            write_type_info(ss, address_class, address_dtype);
            ss << "'";
            throw runtime_error(ss.str());
        }
        bi.branch = branch;
    }
}


void SFrameContext::begin_event(Event & event){
    int ientry = input_tree->GetReadEntry();
    assert(ientry >= 0);
    for(const auto & name_bi : event_input_bname2bi){
        name_bi.second.branch->GetEntry(ientry);
        event.set_validity(name_bi.second.ti, name_bi.second.handle, true);
    }
    
}

void SFrameContext::setup_output(Event & event){
    TTree * outtree = base.GetOutputTree(event_treename.c_str());
    assert(outtree);
    for(const auto & name_bi : event_output_bname2bi){
        void * addr = event.get(name_bi.second.ti, name_bi.second.handle, false);
        ptrs.push_back(addr);
        tree_branch(outtree, name_bi.first, addr, &ptrs.back(), name_bi.second.ti);
    }
}

// event tree i/o
void SFrameContext::do_declare_event_input(const type_info & ti, const string & bname, const string & mname){
    auto handle = ges.get_raw_handle(ti, mname);
    event_input_bname2bi.insert(make_pair(bname, branchinfo(0, ti, handle)));
}

void SFrameContext::do_declare_event_output(const type_info & ti, const string & bname, const string & mname){
    auto handle = ges.get_raw_handle(ti, mname);
    event_output_bname2bi.insert(make_pair(bname, branchinfo(0, ti, handle)));
}

SFrameContext::~SFrameContext(){}

class AnalysisModuleRunner::AnalysisModuleRunnerImpl {
friend class AnalysisModuleRunner;
public:
    AnalysisModuleRunnerImpl(){}
    
private:
    
    std::string m_JetCollection, m_GenJetCollection, m_ElectronCollection, m_MuonCollection, 
      m_TauCollection, m_PhotonCollection, m_PrimaryVertexCollection, m_METName, m_TopJetCollection, m_GenTopJetCollection,
      m_GenParticleCollection, m_GenInfoName;
    bool m_readCommonInfo, m_addGenInfo, m_readTrigger;
    
    // trigger name stuff
    int last_runid_in; // last run id for which trigger names in uhh2::Event has been updated
    int last_runid_out; // last runid for which trigger names have been written to output 
    std::map<int, std::vector<std::string>> runid_to_triggernames; // for current input file
    
    bool setup_output_done;
    bool first_event_inputfile;
    
    std::auto_ptr<GenericEventStructure> ges;
    std::auto_ptr<Event> event;
    
    //std::list<void*> output_ptrs;
    
    std::unique_ptr<SFrameContext> context;
       
    // the actual analysis module to run:
    std::unique_ptr<AnalysisModule> analysis;
    
    std::map<std::string, std::string> dummyConfigVars;  
    
    
    // event content handles:
    Event::Handle<int> h_run, h_lumi, h_event;
    Event::Handle<float> h_rho, h_bsx, h_bsy, h_bsz;
    Event::Handle<bool> h_isRealData;
    
    Event::Handle<std::vector<PrimaryVertex>> h_pvs;
    Event::Handle<std::vector<Electron>> h_electrons;
    Event::Handle<std::vector<Muon>> h_muons;
    Event::Handle<std::vector<Tau>> h_taus;
    Event::Handle<std::vector<Photon>> h_photons;
    Event::Handle<std::vector<Jet>> h_jets;
    Event::Handle<std::vector<TopJet>> h_topjets;
    Event::Handle<MET> h_met;
    
    Event::Handle<GenInfo> h_genInfo;
    Event::Handle<std::vector<GenTopJet>> h_gentopjets;
    Event::Handle<std::vector<GenParticle>> h_genparticles;
    Event::Handle<std::vector<Particle>> h_genjets;
    
    Event::Handle<std::vector<bool>> h_triggerResults;
    Event::Handle<std::vector<std::string>> h_triggerNames; // only used for output
};



AnalysisModuleRunner::AnalysisModuleRunner(){
    pimpl.reset(new AnalysisModuleRunnerImpl());
}


AnalysisModuleRunner::~AnalysisModuleRunner(){
}

// I want to have a list of ALL settings the user gives in the xml config file, without
// having to declare them at an early point. One dirty way to achieve this is
// to override the initlizie routine here and call DeclareProperty for every
// property appearing in the xml file, before actually calling the original Initialize routine.
//
// Note that in proof mode, Initialize is only called on the master, where the configuration object is constructed
// and then distributed to the other workers.
void AnalysisModuleRunner::Initialize( TXMLNode* node ) throw( SError ){
    TXMLNode* nodes = node->GetChildren();
    while( nodes != 0 ) {
        if(!nodes->HasChildren()){
            nodes = nodes->GetNextNode();
            continue;
        }
        if(nodes->GetNodeName() == string("UserConfig")) {
            TXMLNode* userNode = nodes->GetChildren();
            while( userNode != 0 ) {
                if( ! userNode->HasAttributes() || ( userNode->GetNodeName() != TString( "Item" ) ) ) {
                    userNode = userNode->GetNextNode();
                    continue;
                }

                std::string name, stringValue;
                TListIter userAttributes( userNode->GetAttributes() );
                TXMLAttr* attribute;
                while( (attribute = dynamic_cast< TXMLAttr* >( userAttributes() ) ) != 0 ) {
                    if(attribute->GetName() == string("Name"))  name = attribute->GetValue();
                }
                //cout << "got '" << name << "'" << endl;
                pimpl->dummyConfigVars[name] = "";
                DeclareProperty(name, pimpl->dummyConfigVars[name]);
                userNode = userNode->GetNextNode();
            }
        }
        nodes = nodes->GetNextNode();
    }
   
    // now after having declared all properties, call the original Initialize routine:
    SCycleBase::Initialize(node);
}


// This method is called in proof mode to set the configuration according to config.
// The default implementation in SFrame will complain a lot about non-declared variables
// on the workers, so re-implement this to declare all settings before calling the original routine ...
void AnalysisModuleRunner::SetConfig(const SCycleConfig& config){
    const SCycleConfig::property_type & props = config.GetProperties();
    for(SCycleConfig::property_type::const_iterator it = props.begin(); it != props.end(); ++it){
        if(pimpl->dummyConfigVars.find(it->first) == pimpl->dummyConfigVars.end()){
            pimpl->dummyConfigVars[it->first] = "";
            DeclareProperty(it->first, pimpl->dummyConfigVars[it->first]);
        }
    }
    SCycleBase::SetConfig(config);
}


namespace {
    
template<typename T>
Event::Handle<T> declare_in_out(const std::string & name, Context & ctx){
    auto result = ctx.declare_event_input<T>(name);
    ctx.declare_event_output<T>(name);
    return result;
}

}

void AnalysisModuleRunner::BeginInputData( const SInputData& in ) throw( SError ){
    // reset the triggernames output info:
    pimpl->last_runid_in = -1;
    pimpl->last_runid_out = -1;
    pimpl->setup_output_done = false;
    
    // 1. general setup of user config:
    pimpl->ges.reset(new GenericEventStructure);
    pimpl->context.reset(new SFrameContext(*this, in, *pimpl->ges));
    
    pimpl->m_PrimaryVertexCollection = pimpl->context->get("PrimaryVertexCollection", "");
    pimpl->m_ElectronCollection = pimpl->context->get("ElectronCollection", "");
    pimpl->m_MuonCollection = pimpl->context->get("MuonCollection", "");
    pimpl->m_TauCollection = pimpl->context->get("TauCollection", "");
    pimpl->m_PhotonCollection = pimpl->context->get("PhotonCollection", "");
    pimpl->m_JetCollection = pimpl->context->get("JetCollection", "");
    pimpl->m_TopJetCollection = pimpl->context->get("TopJetCollection", "");
    pimpl->m_METName = pimpl->context->get("METName", "");
    
    pimpl->m_GenInfoName = pimpl->context->get("GenInfoName", "genInfo");
    pimpl->m_GenJetCollection = pimpl->context->get("GenJetCollection", "");
    pimpl->m_GenTopJetCollection = pimpl->context->get("GenTopJetCollection", "");
    pimpl->m_GenParticleCollection = pimpl->context->get("GenParticleCollection", "");
    
    pimpl->m_readCommonInfo = string2bool(pimpl->context->get("readCommonInfo", "true"));
    pimpl->m_readTrigger = string2bool(pimpl->context->get("readTrigger", "true"));
    
    string module_classname = pimpl->context->get("AnalysisModule");
    
    // read in gen info if and only if the type is Monte-Carlo:
    pimpl->m_addGenInfo = pimpl->context->get("dataset_type") == "MC";
    
    // 2. now construct user module, which could add event contest to ges
    pimpl->analysis = AnalysisModuleRegistry::build(module_classname, *pimpl->context);
    
    // 3. setup standrad output. To avoid double coding, use the context mechanism
    pimpl->h_run = declare_in_out<int>("run", *pimpl->context);
    pimpl->h_lumi = declare_in_out<int>("luminosityBlock", *pimpl->context);
    pimpl->h_event = declare_in_out<int>("event", *pimpl->context);
    pimpl->h_rho = declare_in_out<float>("rho", *pimpl->context);
    pimpl->h_isRealData = declare_in_out<bool>("isRealData", *pimpl->context);
    pimpl->h_bsx = declare_in_out<float>("beamspot_x0", *pimpl->context);
    pimpl->h_bsy = declare_in_out<float>("beamspot_y0", *pimpl->context);
    pimpl->h_bsz = declare_in_out<float>("beamspot_z0", *pimpl->context);
    
    if(!pimpl->m_PrimaryVertexCollection.empty()) pimpl->h_pvs = declare_in_out<vector<PrimaryVertex>>(pimpl->m_PrimaryVertexCollection, *pimpl->context);
    if(!pimpl->m_ElectronCollection.empty()) pimpl->h_electrons = declare_in_out<vector<Electron>>(pimpl->m_ElectronCollection, *pimpl->context);
    if(!pimpl->m_MuonCollection.empty()) pimpl->h_muons = declare_in_out<vector<Muon>>(pimpl->m_MuonCollection, *pimpl->context);
    if(!pimpl->m_TauCollection.empty()) pimpl->h_taus = declare_in_out<vector<Tau>>(pimpl->m_TauCollection, *pimpl->context);
    if(!pimpl->m_PhotonCollection.empty()) pimpl->h_photons = declare_in_out<vector<Photon>>(pimpl->m_PhotonCollection, *pimpl->context);
    if(!pimpl->m_JetCollection.empty()) pimpl->h_jets = declare_in_out<vector<Jet>>(pimpl->m_JetCollection, *pimpl->context);
    if(!pimpl->m_TopJetCollection.empty()) pimpl->h_topjets = declare_in_out<vector<TopJet>>(pimpl->m_TopJetCollection, *pimpl->context);
    if(!pimpl->m_METName.empty()) pimpl->h_met = declare_in_out<MET>(pimpl->m_METName, *pimpl->context);
    
    if(pimpl->m_addGenInfo){
        if(!pimpl->m_GenInfoName.empty()) pimpl->h_genInfo = declare_in_out<GenInfo>(pimpl->m_GenInfoName, *pimpl->context);
        if(!pimpl->m_GenTopJetCollection.empty()) pimpl->h_gentopjets = declare_in_out<vector<GenTopJet>>(pimpl->m_GenTopJetCollection, *pimpl->context);
        if(!pimpl->m_GenParticleCollection.empty()) pimpl->h_genparticles = declare_in_out<vector<GenParticle>>(pimpl->m_GenParticleCollection, *pimpl->context);
        if(!pimpl->m_GenJetCollection.empty()) pimpl->h_genjets = declare_in_out<vector<Particle>>(pimpl->m_GenJetCollection, *pimpl->context);
    }
    
    if(pimpl->m_readTrigger){
        pimpl->h_triggerResults = declare_in_out<std::vector<bool>>("triggerResults", *pimpl->context);
        pimpl->h_triggerNames = declare_in_out<std::vector<std::string>>("triggerNames", *pimpl->context);
    }
    
    pimpl->event.reset(new Event(*pimpl->ges));
}


void AnalysisModuleRunner::BeginInputFile( const SInputData& ) throw( SError ){
    pimpl->event->clear();    
    pimpl->context->begin_input_file(*pimpl->event);
    
    pimpl->first_event_inputfile = true;
    
    // fill trigger names map:
    if(pimpl->m_readTrigger){
        // find triggernames for all runs in this file:
        TTree* intree = GetInputTree("AnalysisTree");
        if(!intree) throw runtime_error("Did not find AnalysisTree in input file");
        std::vector<std::string> trigger_names;
        std::vector<std::string> *ptrigger_names = &trigger_names;
        int runid = 0;
        TBranch* runb = intree->GetBranch("run");
        TBranch* tnb = intree->GetBranch("triggerNames");
        auto oldaddr_runb = runb->GetAddress();
        auto oldaddr_tnb = tnb->GetAddress();
        if(runb==0 || tnb==0){
            throw runtime_error("did not find branches for setting up trigger names");
        }
        intree->SetBranchAddress("run", &runid, &runb);
        intree->SetBranchAddress("triggerNames", &ptrigger_names, &tnb);
        int nentries = intree->GetEntries();
        int last_runid = -1;
        for(int i=0; i<nentries; ++i){
            runb->GetEntry(i);
            if(last_runid == runid) continue;
            last_runid = runid;
            assert(runid >= 1);
            tnb->GetEntry(i);
            if(!trigger_names.empty()){
                pimpl->runid_to_triggernames[runid] = trigger_names;
            }
        }
        runb->SetAddress(oldaddr_runb);
        tnb->SetAddress(oldaddr_tnb);
    }
    
}


void AnalysisModuleRunner::ExecuteEvent( const SInputData&, Double_t w) throw( SError ){
    uhh2::Event & event = *pimpl->event;
    
    event.invalidate_all();
    // read in the event from the input tree:
    pimpl->context->begin_event(event);
    
    // transfer data to usual members:
    event.run = event.get(pimpl->h_run);
    event.luminosityBlock = event.get(pimpl->h_lumi);
    event.event = event.get(pimpl->h_event);
    event.rho = event.get(pimpl->h_rho);
    event.isRealData = event.get(pimpl->h_isRealData);
    event.beamspot_x0 = event.get(pimpl->h_bsx);
    event.beamspot_y0 = event.get(pimpl->h_bsy);
    event.beamspot_z0 = event.get(pimpl->h_bsz);
    
    if(pimpl->m_readTrigger){
        // fix triggerNames in case of a run change.
        if(pimpl->last_runid_in != event.run){
            auto it = pimpl->runid_to_triggernames.find(event.run);
            if(it==pimpl->runid_to_triggernames.end()){
                throw runtime_error("Did not find trigger names for current run");
            }
            event.set_triggernames(it->second);
        }
        pimpl->last_runid_in = event.run;
    }
    
    if(pimpl->first_event_inputfile){
        pimpl->first_event_inputfile = false;
        if(!pimpl->m_PrimaryVertexCollection.empty()) event.pvs = &event.get(pimpl->h_pvs);
        if(!pimpl->m_ElectronCollection.empty()) event.electrons = &event.get(pimpl->h_electrons);
        if(!pimpl->m_MuonCollection.empty()) event.muons = &event.get(pimpl->h_muons);
        if(!pimpl->m_TauCollection.empty())  event.taus = &event.get(pimpl->h_taus);
        if(!pimpl->m_PhotonCollection.empty())  event.photons = &event.get(pimpl->h_photons);
        if(!pimpl->m_JetCollection.empty()) event.jets = &event.get(pimpl->h_jets);
        if(!pimpl->m_TopJetCollection.empty())  event.topjets = &event.get(pimpl->h_topjets);
        if(!pimpl->m_METName.empty()) event.met = &event.get(pimpl->h_met);
        if(pimpl->m_addGenInfo){
            if(!pimpl->m_GenInfoName.empty()) event.genInfo = &event.get(pimpl->h_genInfo);
            if(!pimpl->m_GenTopJetCollection.empty()) event.gentopjets = &event.get(pimpl->h_gentopjets);
            if(!pimpl->m_GenParticleCollection.empty()) event.genparticles = & event.get(pimpl->h_genparticles);
            if(!pimpl->m_GenJetCollection.empty()) event.genjets = &event.get(pimpl->h_genjets);
        }
        if(pimpl->m_readTrigger){
            event.get_triggerResults() = &event.get(pimpl->h_triggerResults);
        }
    }

    // now call the user module:
    event.weight = w;
    bool keep = pimpl->analysis->process(event);
    
    if(!keep){
        throw SError(SError::SkipEvent);
    }
    
    if(!pimpl->setup_output_done){
        //setup_output();
        pimpl->context->setup_output(*pimpl->event);
        pimpl->setup_output_done = true;
    }
    
    // now setup output triggernames:
    if(pimpl->m_readTrigger){
        if(pimpl->last_runid_out != event.run){
            auto it = pimpl->runid_to_triggernames.find(event.run);
            if(it==pimpl->runid_to_triggernames.end()){
                throw runtime_error("Did not find trigger names for current runid");
            }
            event.set(pimpl->h_triggerNames, it->second);
            pimpl->last_runid_out = event.run;
        }
        else{
            event.set(pimpl->h_triggerNames, vector<string>());
        }
    }
}

namespace{
    // double to string; 
    string d2s(double d){
        char s[20];
        snprintf(s, 20, "%.6g", d);
        return s;
    };
    
    //long uint to string:
    string ul2s(unsigned long i){
        char s[20];
        snprintf(s, 20, "%lu", i);
        return s;
    };
}

void AnalysisModuleRunner::EndMasterInputData(const SInputData & d) throw (SError){
    TList * l = GetHistOutput();
    TIter next(l);
    TObject * obj;
    
    // populate cutflows:
    map<string, pair<TH1D*, TH1D*> > cutflows; // first: weighted, second: unweighted
    while((obj = next())){
        string name(obj->GetName());
        if(name.find("cf_") != 0) continue;
        SCycleOutput * out = dynamic_cast<SCycleOutput*>(obj);
        if(!out) continue;
        obj = out->GetObject();
        TH1D * cutflow = dynamic_cast<TH1D*>(obj);
        if(!cutflow)continue;
        // get the selection name by cutting off "cf_" at the start:
        string sel_name = name.substr(3);
        // cut off "_raw in the end:"
        bool raw = false;
        if(name.size() > 4 and name.find("_raw")==name.size()-4){
            sel_name = sel_name.substr(0, sel_name.size()-4);
            raw = true;
        }
        if(raw){
            cutflows[sel_name].second = cutflow;
        }
        else{
            cutflows[sel_name].first = cutflow;
        }
    }
    
    // print them:
    for(map<string, pair<TH1D*, TH1D*> >::const_iterator it = cutflows.begin(); it!= cutflows.end(); ++it){
        TH1D * cf = it->second.first;
        TH1D * cf_raw = it->second.second;
        if(cf==0 or cf_raw == 0 or cf->GetNbinsX() != cf_raw->GetNbinsX()){
            m_logger << WARNING << " did not find all cutflows (or inconsistent cutflows)" << endl;
            continue;
        }
        cout << endl << "Cutflow for selection '" << it->first << "':" << endl;
        vector<string> headers;
        headers.push_back("Selection");
        headers.push_back("N_raw");
        headers.push_back("N_weighted");
        TableOutput out(headers);
        TAxis * xax = cf->GetXaxis();
        for(int ibin=1; ibin<=cf->GetNbinsX(); ++ibin){
            vector<string> row;
            row.push_back(xax->GetBinLabel(ibin));
            row.push_back(ul2s(cf_raw->GetBinContent(ibin)));
            row.push_back(d2s(cf->GetBinContent(ibin)));
            out.add_row(row);
        }
        out.print(cout);
    }
}

ClassImp(uhh2::AnalysisModuleRunner);

#endif
