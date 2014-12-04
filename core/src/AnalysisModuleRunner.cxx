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
    
// other tree output:
/*
void SFrameContext::do_declare_output(const string & treename, const string & name, const void * caddr, const type_info & ti){
    auto it = output_trees.find(treename);
    TTree * tree;
    if(it==output_trees.end()){
        tree = base.GetOutputMetadataTree(treename.c_str());
        assert(tree);
        output_trees[treename] = tree;
    }
    else{
        tree = it->second;
    }
    void * addr = const_cast<void*>(caddr);
    ptrs.push_back(addr);
    tree_branch(tree, name, addr, &ptrs.back(), ti);
}


void SFrameContext::write_entry(const string & treename){
    auto it = output_trees.find(treename);
    if(it==output_trees.end()){
        throw runtime_error("called write_output for tree '" + treename + "' which has not been declared previously");
    }
    it->second->Fill();
}*/


AnalysisModuleRunner::AnalysisModuleRunner(){
    m_runid_triggernames = -1;
}


AnalysisModuleRunner::~AnalysisModuleRunner(){}

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
                dummyConfigVars[name] = "";
                DeclareProperty(name, dummyConfigVars[name]);
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
        if(dummyConfigVars.find(it->first) == dummyConfigVars.end()){
            dummyConfigVars[it->first] = "";
            DeclareProperty(it->first, dummyConfigVars[it->first]);
        }
    }
    SCycleBase::SetConfig(config);
}

void AnalysisModuleRunner::BeginInputData( const SInputData& in ) throw( SError ){
    // reset the triggernames output info:
    m_output_triggerNames_runid = -1;
    m_output_triggerNames.clear();
    
    
    // 1. general setup of user config:
    ges.reset(new GenericEventStructure);
    context.reset(new SFrameContext(*this, in, *ges));
    
    m_JetCollection = context->get("JetCollection", "");
    m_GenJetCollection = context->get("GenJetCollection", "");
    m_ElectronCollection = context->get("ElectronCollection", "");
    m_MuonCollection = context->get("MuonCollection", "");
    m_TauCollection = context->get("TauCollection", "");
    m_PhotonCollection = context->get("PhotonCollection", "");
    m_PrimaryVertexCollection = context->get("PrimaryVertexCollection", "");
    m_METName = context->get("METName", "");
    m_TopJetCollection = context->get("TopJetCollection", "");
    m_GenTopJetCollection = context->get("GenTopJetCollection", "");
    m_GenParticleCollection = context->get("GenParticleCollection", "");
    
    m_readCommonInfo = string2bool(context->get("readCommonInfo", "true"));
    m_readTrigger = string2bool(context->get("readTrigger", "true"));
    
    string module_classname = context->get("AnalysisModule");
    
    // read in gen info if and only if the type is Monte-Carlo:
    m_addGenInfo = context->get("dataset_type") == "MC";
    
    // 2. now construct user module, which could add event contest to ges
    analysis.reset(AnalysisModuleRegistry::build(module_classname, *context).release());
    event.reset(new Event(*ges));
    
    // 3. after the event content is defined, setup the output. setup_output does that for the
    // 'direct' event content, i.e. the bare pointers defined directly in Event,
    // while context->setup_output does it for content stored via the GenericEvent mechanisms.
    setup_output();
    context->setup_output(*event);
}


void AnalysisModuleRunner::FillTriggerNames(){
    if(!m_readTrigger) return;
    if(event->run == m_runid_triggernames) return;
    
    //fill list of trigger names.
    // Maybe the event we just read was the first of the given run,
    // so it has the trigger name information already filled:
    if(!m_input_triggerNames->empty()) {
        m_triggerNames = *m_input_triggerNames;
        event->set_triggernames(*m_input_triggerNames);
        m_runid_triggernames = event->run;
    }
    else{
      m_logger << INFO << "No trigger table found for this event (need: run " << event->run << "; have: run " << m_runid_triggernames << ") -> start trigger search on all input events" << SLogger::endmsg;
      TTree* tmp_tree = GetInputTree("AnalysisTree");
      TBranch * run_branch = tmp_tree->GetBranch("run");
      TBranch * triggerNames_branch = tmp_tree->GetBranch("triggerNames");
      assert(run_branch);
      assert(triggerNames_branch);
      auto old_run_address = run_branch->GetAddress();
      int runid = -2;
      run_branch->SetAddress(&runid);
      int N_ent= tmp_tree->GetEntriesFast();
      //search for some event in the input event tree with the same runid which has the trigger filled. Can be before or after ...
      for(int i=0; i<N_ent; ++i){
        run_branch->GetEntry(i);
        if(runid != event->run){
            continue;
        }
        triggerNames_branch->GetEntry(i);
        if(m_input_triggerNames->empty()){
            continue;
        }
        m_triggerNames = *m_input_triggerNames;
        event->set_triggernames(m_triggerNames);
        m_runid_triggernames = event->run;
        break;
      }
      // reset address (should be event->run)
      run_branch->SetAddress(old_run_address);
      
    }
    if(m_runid_triggernames != event->run){
        m_logger << ERROR << "Trigger search was NOT succesful!!!" << SLogger::endmsg;
        throw runtime_error("triggerNames not found");
    }
}

void AnalysisModuleRunner::BeginInputFile( const SInputData& ) throw( SError ){
    event->clear();
    m_input_triggerNames = 0;
    if(m_ElectronCollection.size()>0) ConnectVariable( "AnalysisTree", m_ElectronCollection.c_str(), event->electrons);
    if(m_MuonCollection.size()>0) ConnectVariable( "AnalysisTree", m_MuonCollection.c_str(), event->muons);
    if(m_TauCollection.size()>0) ConnectVariable( "AnalysisTree", m_TauCollection.c_str(), event->taus);
    if(m_JetCollection.size()>0) ConnectVariable( "AnalysisTree", m_JetCollection.c_str(), event->jets);
    if(m_PhotonCollection.size()>0) ConnectVariable( "AnalysisTree", m_PhotonCollection.c_str(), event->photons);
    if(m_METName.size()>0) ConnectVariable( "AnalysisTree", m_METName.c_str(), event->met);
    if(m_PrimaryVertexCollection.size()>0) ConnectVariable( "AnalysisTree", m_PrimaryVertexCollection.c_str() , event->pvs);
    if(m_TopJetCollection.size()>0) ConnectVariable( "AnalysisTree", m_TopJetCollection.c_str(), event->topjets);
    
    if(m_addGenInfo){
        if(m_GenJetCollection.size()>0) ConnectVariable( "AnalysisTree", m_GenJetCollection.c_str(), event->genjets);
        if(m_GenTopJetCollection.size()>0) ConnectVariable( "AnalysisTree", m_GenTopJetCollection.c_str() , event->gentopjets);
        if(m_GenParticleCollection.size()>0) ConnectVariable( "AnalysisTree", m_GenParticleCollection.c_str() , event->genparticles);
        if(m_readCommonInfo) ConnectVariable( "AnalysisTree", "genInfo" , event->genInfo);
    }
    
    ConnectVariable( "AnalysisTree", "run" , event->run);
    ConnectVariable( "AnalysisTree", "luminosityBlock" , event->luminosityBlock);
    ConnectVariable( "AnalysisTree" ,"event" ,event->event);
    ConnectVariable( "AnalysisTree" ,"isRealData", event->isRealData);
    
    if(m_readTrigger){
        ConnectVariable( "AnalysisTree", "triggerResults", event->get_triggerResults());
        ConnectVariable( "AnalysisTree", "triggerNames", m_input_triggerNames);
    }

    if(m_readCommonInfo){
        ConnectVariable( "AnalysisTree", "rho" , event->rho);
        ConnectVariable( "AnalysisTree" ,"beamspot_x0", event->beamspot_x0);
        ConnectVariable( "AnalysisTree" ,"beamspot_y0", event->beamspot_y0);
        ConnectVariable( "AnalysisTree" ,"beamspot_z0", event->beamspot_z0);
    }
    
    context->begin_input_file(*event);
}

template<typename T>
void AnalysisModuleRunner::tree_branch(TTree * tree, const std::string & bname, T * addr){
    if(bname.empty()) return;
    output_ptrs.push_back(addr);
    uhh2::tree_branch(tree, bname, addr, &output_ptrs.back(), typeid(T));
}

void AnalysisModuleRunner::setup_output(){
    output_ptrs.clear();
    TTree * outtree = 0;
    try{
      outtree = GetOutputTree("AnalysisTree");
    }
    catch(const SError &){
         // if output tree is not found, we don't write anything.
        return;
    }
    assert(outtree!=0);

    // We want to write everything we have.
    // a) For all pointers in the Event container, this means we want to write everyhing that is not 0. Note 
    //    that this rule covers the case of writing recoHyps which are not present int the input but were created by an
    //    AnalysisModule 'on-the-fly'
    // b) For all "plain" data, use the configuration to see whether it was read and only write what has been read.

    
    // a.:
    tree_branch(outtree, m_ElectronCollection, event->electrons);
    tree_branch(outtree, m_MuonCollection, event->muons);
    tree_branch(outtree, m_TauCollection, event->taus);
    tree_branch(outtree, m_JetCollection, event->jets);
    tree_branch(outtree, m_PhotonCollection, event->photons);
    tree_branch(outtree, m_METName, event->met);
    tree_branch(outtree, m_PrimaryVertexCollection , event->pvs);
    tree_branch(outtree, m_TopJetCollection, event->topjets);
    if(m_addGenInfo){
        tree_branch(outtree, m_GenJetCollection, event->genjets);
        tree_branch(outtree, m_GenTopJetCollection, event->gentopjets);
        tree_branch(outtree, m_GenParticleCollection, event->genparticles);
        if(m_readCommonInfo){
            tree_branch(outtree, "genInfo", event->genInfo);
        }
    }
    
    // trigger names is special: use our own member variable for that:
    if(m_readTrigger){
        tree_branch(outtree, "triggerResults", event->get_triggerResults());
        tree_branch(outtree, "triggerNames", &m_output_triggerNames);
    }

    // b.:
    // these are always read:
    tree_branch(outtree, "run", &event->run);
    tree_branch(outtree, "luminosityBlock", &event->luminosityBlock);
    tree_branch(outtree, "event", &event->event);
    tree_branch(outtree, "isRealData", &event->isRealData);
    
    if(m_readCommonInfo){
        tree_branch(outtree, "rho", &event->rho);
        tree_branch(outtree, "beamspot_x0", &event->beamspot_x0);
        tree_branch(outtree, "beamspot_y0", &event->beamspot_y0);
        tree_branch(outtree, "beamspot_z0", &event->beamspot_z0);
    }
}



void AnalysisModuleRunner::ExecuteEvent( const SInputData&, Double_t w) throw( SError ){
    event->invalidate_all();
    // read in the event from the input tree:
    context->begin_event(*event);
    
    // search for trigger names in input tree:
    FillTriggerNames();

    // now call the user module:
    event->weight = w;
    bool keep = analysis->process(*event);
    
    if(!keep){
        throw SError(SError::SkipEvent);
    }
    
    // make sure list of trigger names is filled in the output for each runid once:
    if(m_output_triggerNames_runid != event->run){
        m_output_triggerNames_runid = event->run;
        m_output_triggerNames = m_triggerNames;
    }
    else{
        // we already output the name list for that runid, so write empty it for this event:
        m_output_triggerNames.clear();
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

ClassImp(AnalysisModuleRunner);

#endif
