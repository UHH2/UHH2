#ifndef CMSSW

#include "UHH2/core/include/AnalysisModuleRunner.h"
#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/EventHelper.h"
#include "UHH2/core/include/Utils.h"
#include "UHH2/core/include/registry.h"
#include "UHH2/core/include/root-utils.h"

#include "TH1.h"
#include "TXMLNode.h"
#include "TXMLAttr.h"
#include "TTree.h"
#include "TBranchElement.h"
#include "TClonesArray.h"

#include <stdexcept>
#include <memory>
#include <iostream>

using namespace std;
using namespace uhh2;

using uhh2::detail::EventHelper;

namespace {

void write_type_info(ostream & out, TClass * class_, EDataType type_) {
    if (class_) {
        out << class_->GetName();
        out << "(c++ name: '" << demangle(class_->GetTypeInfo()->name()) << "')";
    }
    else {
        auto tn = TDataType::GetTypeName(type_);
        if (tn == string("")) {
            out << "(type not known to ROOT!)";
        }
        else {
            out << tn;
        }
    }
}

EDataType normalize(EDataType dt){
    if(sizeof(Long_t) == sizeof(Long64_t)){
        switch(dt){
            case kLong_t: return kLong64_t;
            case kULong_t: return kULong64_t;
            default: return dt;
        }
    }
    else{
        assert(sizeof(Long_t) == sizeof(Int_t));
        switch(dt){
	    case kLong_t: return kInt_t;
            case kULong_t: return kUInt_t;
            default: return dt;
        }
    }
}

// addraddr is a pointer-to-pointer to an object of type T, given by ti = typeid(T). T is never of pointer type.
// So addr := *addraddr is a pointer to T.
// addraddr must not be nullptr.
// If addr is a nullptr, memory will be allocated and eraser is set to a function which can erase the memory allocated here again. eraser has to be called
// like as 'eraser(addr)' after reading all input from the branch.
// If addr is non-null, addr is assumed to point to pre-allocated memory and is left unchanged; eraser is not set in this case.
void connect_input_branch(TBranch * branch, const std::type_info & ti, void ** addraddr, std::function<void (void *)> & eraser){
    assert(branch != nullptr);
    assert(addraddr != nullptr);
    void *& addr = *addraddr;
    // now do SetBranchAddress, using address-to-address in case of a class, and simple address otherwise.
    TClass * branch_class = 0;
    EDataType branch_dtype = kNoType_t;
    int res = branch->GetExpectedType(branch_class, branch_dtype);
    branch_dtype = normalize(branch_dtype);
    if (res > 0) {
        throw runtime_error("input branch '" + string(branch->GetName()) + "': error reading type information");
    }

    // get address type:
    TClass * address_class = TBuffer::GetClass(ti);
    EDataType address_dtype = kNoType_t;
    TDataType * address_tdatatype = TDataType::GetDataType(TDataType::GetType(ti));
    if (address_tdatatype) {
      address_dtype = normalize(EDataType(address_tdatatype->GetType()));
    }
    if(branch_dtype == kLong64_t && address_dtype==kInt_t)
      address_dtype=kLong64_t;

    // compare if they match; note that it's an error if the class or data type has not been found:
    if ((branch_class && address_class) && ((branch_class == address_class) || (branch_class->GetTypeInfo() == address_class->GetTypeInfo()))) {
        if (addr == 0) {
            addr = branch_class->New();
            eraser = branch_class->GetDelete();
            // TClonesArray needs special treatment, as the TClonesArray object needs to know the underlying class it should store:
            bool is_clones = branch_class == TClonesArray::Class();
            if(is_clones){
                auto br = dynamic_cast<TBranchElement*>(branch);
                assert(br);
                auto tca = reinterpret_cast<TClonesArray*>(addr);
                // note: have to initialize TClonesArray with large enough size, e.g. 100. Using
                // smaller sizes (at least <~10) segfaults root in TClonesArray::ExpandCreateFast called
                // upon reading input data by TTree.
                tca->SetClass(br->GetClonesName(), 100);
            }
        }
        branch->SetAddress(addraddr);
    }
    else if (branch_dtype != kNoType_t && (branch_dtype == address_dtype)) {
        if (addr == 0) {
            addr = new uint64_t; // largest primitive type
            eraser = [](void * addr) {delete reinterpret_cast<uint64_t*>(addr);};
        }
        branch->SetAddress(addr);
    }

    else { // this is an error:
        stringstream ss;
        ss << "Type error for branch '" << branch->GetName() << "': branch holds type ";
        write_type_info(ss, branch_class, branch_dtype);
        ss << ", but tried to connect to variable of type ";
        write_type_info(ss, address_class, address_dtype);
        throw runtime_error(ss.str());
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
class SFrameContext: public uhh2::Context {
public:

    SFrameContext(AnalysisModuleRunner & base, const SInputData& sin, GenericEventStructure & es);

    virtual void put(const std::string & path, TH1 * t) override;

    ~SFrameContext();

    // this should be called from SCycleBase::BeginInputFile. It takes care
    // of setting up the input addresses into the event.
    void begin_input_file(Event & event);
    void begin_event(Event & event);
    void setup_output(Event & event); // should be called after processing the first event which is written to the output
    void check_output(Event & event); // checks if all objetcs written to the output have been assigned a correct value during the process routine; called at the end of each event

    void write_metadata_tree();
    void first_input_file(); // called for the first input file of the dataset, in addition to begin_input_file, but with no event (yet)

    std::string event_treename;

    TTree * outtree = nullptr; // output tree. Can be 0 in case no output should be written.

    void do_declare_event_input_handle(const std::type_info & ti, const std::string & bname, const GenericEvent::RawHandle & handle);
    void do_declare_event_output_handle(const std::type_info & ti, const std::string & bname, const GenericEvent::RawHandle & handle);

private:

    // event tree i/o
    // note that the declare_input does nothing but saving the parameters for later; the actual setup of
    // input branches is done in begin_input_file.
    virtual void do_declare_event_input(const std::type_info & ti, const std::string & bname, const std::string & mname) override;
    virtual void do_declare_event_output(const std::type_info & ti, const std::string & bname, const std::string & mname) override;
    virtual void do_undeclare_event_output(const std::string & branch_name) override;
    virtual void do_undeclare_all_event_output() override;

    // read metadata from a tree "uhh2_meta" in dir into data. Returns whether the tree
    // was found or not. Also checks consistency opf all entries in case there is more than one entry
    // in the metadata tree (in case of merged output).
    static optional<map<string, string>> read_metadata(TDirectory * dir);

    TDirectory * get_tmpdir();

    AnalysisModuleRunner & base;

    TTree * input_tree = nullptr; // input event tree

    // IMPORTANT: branchinfo must not move in memory
    // to not invalidate the pointer to addr, which ROOT trees need.
    // Therefore, copy and move constructor are deleted to prevent that.
    struct branchinfo {
        TBranch * branch;
        void * addr;
        std::function<void(void*)> eraser; // eraser for addr, in case it belongs to branchinfo
        const type_info & ti; // this is always a non-pointer type.
        GenericEvent::RawHandle handle;

        branchinfo(const branchinfo &) = delete;
        branchinfo(branchinfo && other) = delete;
        branchinfo & operator=(const branchinfo &) = delete;
        branchinfo & operator=(branchinfo && other) = delete;

        branchinfo(const type_info & ti_, const GenericEvent::RawHandle & handle_) :
                branch(nullptr), addr(nullptr), ti(ti_), handle(handle_) {}

        ~branchinfo() {
            if (eraser && addr) {
                eraser(addr);
            }
        }
    };
    std::map<std::string, std::unique_ptr<branchinfo>> event_input_bname2bi;
    std::map<std::string, std::unique_ptr<branchinfo>> event_output_bname2bi;


    // metadata handling
    TTree * metadata_tree; // for output
    optional<map<string, string>> metadata; // from input. none if input has no metadata
};

}

SFrameContext::SFrameContext(AnalysisModuleRunner & base_, const SInputData& sin, GenericEventStructure & es_) :
        Context(es_), base(base_) {
    for (const auto & prop : base.GetConfig().GetProperties()) {
        set(prop.first, prop.second);
    }
    set("dataset_type", sin.GetType().Data());
    set("dataset_version", sin.GetVersion().Data());
    set("dataset_lumi", double2string(sin.GetTotalLumi()));
    set("target_lumi", double2string(base_.GetConfig().GetTargetLumi()));
    set("n_events_total", double2string(sin.GetEventsTotal())); // GetEventsTotal returns a long..
    // we need exactly one input event tree:
    bool found_input_tree = false;
    for (const auto & type_trees : sin.GetTrees()) {
        for (const STree & tree : type_trees.second) {
            if ((tree.type & STree::EVENT_TREE) && (tree.type & STree::INPUT_TREE)) {
                if (found_input_tree) {
                    throw runtime_error("more than one input event tree");
                }
                found_input_tree = true;
                event_treename = tree.treeName.Data();
            }
        }
    }
    if (!found_input_tree) {
        throw runtime_error("no input event tree");
    }
}

optional<map<string, string>> SFrameContext::read_metadata(TDirectory * dir){
    TTree * meta_intree = dynamic_cast<TTree*>(dir->Get("uhh2_meta"));
    if(meta_intree == nullptr){
        return std::nullopt;
    }
    else{
        string data;
        string * pdata = &data;
        meta_intree->SetBranchAddress("data", &pdata);
        int n = meta_intree->GetEntries();
        map<string, string> result;
        for(int i=0; i<n; ++i){
            meta_intree->GetEntry(i);
            map<string, string> result_i;
            // parse metadata into result_i:
            size_t p = 0;
            while (true){
                size_t p_equal = data.find("===", p);
                if(p_equal == string::npos){
                    break;
                }
                size_t p_value_start = p_equal + 3;
                size_t p_endline = data.find('\n', p_value_start);
                if(p_endline == string::npos){
                    break;
                }
                auto entry = make_pair(data.substr(p, p_equal - p), data.substr(p_value_start, p_endline - p_value_start));
                result_i.insert(move(entry));
                p = p_endline + 1;
            }
            if(i==0){
                result = move(result_i);
            }
            else{
                // check consistency:
                if(result_i.size() != result.size() || !equal(result.begin(), result.end(), result_i.begin())){
                    throw runtime_error("metadata tree has more than 1 entry, and entries are not consistent with each other");
                }
            }
        }
        return result;
    }
}


TDirectory * SFrameContext::get_tmpdir() {
    // see SCycleBaseHist::GetTempDir (which is private ...):
    static TDirectory* tempdir = nullptr;
    if (!tempdir) {
        gROOT->cd();
        tempdir = gROOT->mkdir("SFrameTempDir");
        if (!tempdir) {
            throw runtime_error("SFrameTempDir could not be created");
        }
    }
    return tempdir;
}

void SFrameContext::put(const string & path, TH1 * t) {
    // strip leading and/or trailing slash from path.
    string trimmed_path = path;
    trim(trimmed_path, "/");

    string fullname = trimmed_path + '/' + t->GetName();

    // see implementation of template SCycleBaseHist::Book with inFile=false (which is the default).
    // instead of calling t->Clone, use SetDirectory:
    TDirectory * tempdir = get_tmpdir();
    tempdir->cd();
    t->SetDirectory(tempdir);
    SCycleOutput* out = new SCycleOutput(t, fullname.c_str(), trimmed_path.c_str());
    TList * outlist = base.GetHistOutput();
    outlist->AddLast(out);
    gROOT->cd();
}

void SFrameContext::write_metadata_tree(){
    if(!outtree) return;
    // see SCycleNTupleBase::SaveOutputTrees
    bool isProof = base.GetNTupleInput()->FindObject("PROOF_OUTPUTFILE"); // SFrame::ProofOutputName = "PROOF_OUTPUTFILE"
    TDirectory * dir_before = gDirectory;
    if(metadata_tree){
        if(metadata_tree->GetEntries() || !isProof){
            TDirectory * dir = metadata_tree->GetDirectory();
            if(dir) dir->cd();
            metadata_tree->Write();
            metadata_tree->AutoSave();
        }
        metadata_tree->SetDirectory(nullptr);
    }
    delete metadata_tree;
    gDirectory = dir_before;
}

void SFrameContext::first_input_file() {
    input_tree = base.GetInputTree(event_treename.c_str());
    if (input_tree == nullptr) {
        throw runtime_error("Could not find input tree '" + event_treename + "'");
    }
    auto dir = input_tree->GetDirectory();
    std::map<std::string, std::string> data;
    metadata = read_metadata(dir);
    if(metadata){
        // set all keys:
        for(const auto & kv : *metadata){
            set("meta_" + kv.first, kv.second);
        }
    }
}

void SFrameContext::begin_input_file(Event & event) {
    input_tree = base.GetInputTree(event_treename.c_str());
    if (input_tree == nullptr) {
        throw runtime_error("Could not find input tree '" + event_treename + "'");
    }
    for (auto & name_bi : event_input_bname2bi) {
        const string & bname = name_bi.first;
        branchinfo & bi = *name_bi.second;
        TBranch * branch = input_tree->GetBranch(bname.c_str());
        if (branch == nullptr) {
            throw runtime_error("Could not find branch '" + bname + "' in tree '" + event_treename + "'");
        }
        connect_input_branch(branch, bi.ti, &bi.addr, bi.eraser);
        EventAccess_::set_unmanaged(event, bi.ti, bi.handle, bi.addr);
        bi.branch = branch;
    }

    // check consistency of file metadata with previous metadata.
    auto dir = input_tree->GetDirectory();
    assert(dir);
    auto thisfile_metadata = read_metadata(dir);
    if(static_cast<bool>(thisfile_metadata) != static_cast<bool>(metadata)){
        throw runtime_error("Inconsistent metadata presence in input files");
    }
    if(thisfile_metadata){
        if(metadata->size() != thisfile_metadata->size() || !std::equal(thisfile_metadata->begin(), thisfile_metadata->end(), metadata->begin())){
            throw runtime_error("Inconsistent metadata in input files");
        }
    }
}

void SFrameContext::begin_event(Event & event) {
    EventAccess_::invalidate_all(event);
    auto ientry = input_tree->GetReadEntry();
    assert(ientry >= 0);
    for (const auto & name_bi : event_input_bname2bi) {
        name_bi.second->branch->GetEntry(ientry);
        EventAccess_::set_validity(event, name_bi.second->ti, name_bi.second->handle, true);
    }
}

void SFrameContext::check_output(Event & event) {

    for (auto & name_bi : event_output_bname2bi) {
        auto & bi = *name_bi.second;
	bi.addr = EventAccess_::get(event, bi.ti, bi.handle, true, false);
	if(!bi.addr){
	  throw runtime_error("check_output failed with invalid pointer to " + name_bi.first);
	}
    }
}

void SFrameContext::setup_output(Event & event) {
    assert(!outtree); // prevent calling it twice
    try {
        outtree = base.GetOutputTree(event_treename.c_str());
    }
    catch (const SError &) {
        // no OutputTree defined -> no output is written.
        return;
    }
    assert(outtree);
    for (auto & name_bi : event_output_bname2bi) {
        auto & bi = *name_bi.second;
        bi.addr = EventAccess_::get(event, bi.ti, bi.handle, false, false);
        tree_branch(outtree, name_bi.first, bi.addr, &bi.addr, bi.ti);
    }
    // write metadata as string to the output.
    // Unfortunately, writing a single string is not straight-forward in ROOT,
    // at least not covering all the different cases (PROOF mode, local mode, etc.).
    // Therefore, write the string into a TTree.
    string metadata_string; // all metadata in a single string
    auto settings = get_all();
    for(const auto & kv : settings){
        const auto & key = kv.first;
        const auto & value = kv.second;
        if(!key.find("meta_") == 0) continue;
        metadata_string += key.substr(5) + "===" + value + '\n';
    }
    auto dir = outtree->GetDirectory();
    if(dir){
        dir->cd();
    }
    metadata_tree = new TTree("uhh2_meta", "uhh2_meta");
    metadata_tree->SetDirectory(dir);
    void * pstring = &metadata_string;
    tree_branch(metadata_tree, "data", pstring, &pstring, typeid(string));
    metadata_tree->Fill();
}

void SFrameContext::do_declare_event_input(const type_info & ti, const string & bname, const string & mname) {
    auto handle = ges.get_raw_handle(ti, mname);
    do_declare_event_input_handle(ti, bname, handle);
}

void SFrameContext::do_declare_event_output(const type_info & ti, const string & bname, const string & mname) {
    auto handle = ges.get_raw_handle(ti, mname);
    do_declare_event_output_handle(ti, bname, handle);
}

void SFrameContext::do_undeclare_event_output(const std::string & branch_name) {
    if (!event_output_bname2bi.count(branch_name)) {
        throw runtime_error("Can not undeclare event output '" + branch_name + "' as it has not been declared before.");
    }
    event_output_bname2bi.erase(branch_name);
};

void SFrameContext::do_undeclare_all_event_output() {
    event_output_bname2bi.clear();
};


void SFrameContext::do_declare_event_input_handle(const type_info & ti, const string & bname, const GenericEvent::RawHandle & handle) {
  event_input_bname2bi.insert(make_pair(bname, uhh2::make_unique<branchinfo>(ti, handle)));
}

void SFrameContext::do_declare_event_output_handle(const type_info & ti, const string & bname, const GenericEvent::RawHandle & handle) {
  event_output_bname2bi.insert(make_pair(bname, uhh2::make_unique<branchinfo>(ti, handle)));
}

SFrameContext::~SFrameContext() {
}

class AnalysisModuleRunner::AnalysisModuleRunnerImpl {
    friend class AnalysisModuleRunner;
    public:
    AnalysisModuleRunnerImpl() {
    }

    void begin_input_data(AnalysisModuleRunner & base, const SInputData& in);

private:
    bool m_readTrigger;
    bool m_userEventFormat;

    bool setup_output_done;

    bool use_sframe_weight;

    std::vector<std::string> additional_branches;
    bool first_file = true;

    // per-dataset information, in the order of construction:
    std::unique_ptr<GenericEventStructure> ges;
    std::unique_ptr<SFrameContext> context;
    std::unique_ptr<EventHelper> eh;
    std::unique_ptr<Event> event;

    // the actual analysis module to run:
    std::unique_ptr<AnalysisModule> analysis;

    std::map<std::string, std::string> dummyConfigVars;

};

AnalysisModuleRunner::AnalysisModuleRunner() {
    pimpl.reset(new AnalysisModuleRunnerImpl());
}

AnalysisModuleRunner::~AnalysisModuleRunner() {
}

// I want to have a list of ALL settings the user gives in the xml config file, without
// having to declare them at an early point. One dirty way to achieve this is
// to override the initialize routine here and call DeclareProperty for every
// property appearing in the xml file, before actually calling the original Initialize routine.
//
// Note that in proof mode, Initialize is only called on the master, where the configuration object is constructed
// and then distributed to the other workers.
void AnalysisModuleRunner::Initialize(TXMLNode* node) {
    TXMLNode* nodes = node->GetChildren();
    while (nodes != nullptr) {
        if (!nodes->HasChildren()) {
            nodes = nodes->GetNextNode();
            continue;
        }
        if (nodes->GetNodeName() == string("UserConfig")) {
            TXMLNode* userNode = nodes->GetChildren();
            while (userNode != nullptr) {
                if (!userNode->HasAttributes()
                        || (userNode->GetNodeName() != TString("Item"))) {
                    userNode = userNode->GetNextNode();
                    continue;
                }

                std::string name, stringValue;
                TListIter userAttributes(userNode->GetAttributes());
                TXMLAttr* attribute;
                while ((attribute = dynamic_cast<TXMLAttr*>(userAttributes())) != nullptr) {
                    if (attribute->GetName() == string("Name"))
                        name = attribute->GetValue();
                }
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
void AnalysisModuleRunner::SetConfig(const SCycleConfig& config) {
    const SCycleConfig::property_type & props = config.GetProperties();
    for (SCycleConfig::property_type::const_iterator it = props.begin();
            it != props.end(); ++it) {
        if (pimpl->dummyConfigVars.find(it->first)
                == pimpl->dummyConfigVars.end()) {
            pimpl->dummyConfigVars[it->first] = "";
            DeclareProperty(it->first, pimpl->dummyConfigVars[it->first]);
        }
    }
    SCycleBase::SetConfig(config);
}

void AnalysisModuleRunner::AnalysisModuleRunnerImpl::begin_input_data(AnalysisModuleRunner & base, const SInputData& in) {
    ges.reset(new GenericEventStructure);
    context.reset(new SFrameContext(base, in, *ges));

    // 1. setup common event data members / branches:
    m_userEventFormat = string2bool(context->get("userEventFormat", "false"));

    if(!m_userEventFormat){

        eh.reset(new EventHelper(*context));

        eh->setup_pvs(context->get("PrimaryVertexCollection", ""));
        eh->setup_electrons(context->get("ElectronCollection", ""));
        eh->setup_muons(context->get("MuonCollection", ""));
        eh->setup_taus(context->get("TauCollection", ""));
        eh->setup_photons(context->get("PhotonCollection", ""));
        eh->setup_jets(context->get("JetCollection", ""));
        eh->setup_topjets(context->get("TopJetCollection", ""));
        eh->setup_toppuppijets(context->get("TopPuppiJetCollection", ""));
        eh->setup_met(context->get("METName", ""));
        eh->setup_pfparticles(context->get("PFParticleCollection", ""));
        eh->setup_L1EG_seeds(context->get("L1EGseedsCollection", ""));
	      eh->setup_L1J_seeds(context->get("L1JseedsCollection", ""));
        eh->setup_L1M_seeds(context->get("L1MseedsCollection", ""));
        eh->setup_L1EtS_seeds(context->get("L1EtSseedsCollection", ""));

        bool is_mc = context->get("dataset_type") == "MC";
        if (is_mc) {
	  try{
            eh->setup_genInfo(context->get("GenInfoName", "genInfo"));}
	  catch(const std::runtime_error& error){
	    std::cout<<"Problem with genInfo in AnalysisModuleRunner.cxx"<<std::endl;
	    std::cout<<error.what();
	  }
	  try{
            eh->setup_genjets(context->get("GenJetCollection", ""));}
	  catch(const std::runtime_error& error){
	    std::cout<<"Problem with genJetCollection in AnalysisModuleRunner.cxx"<<std::endl;
	    std::cout<<error.what();
	  }
	  eh->setup_gentopjets(context->get("GenTopJetCollection", ""));
	  try{
            eh->setup_genparticles(context->get("GenParticleCollection", ""));}
	  catch(const std::runtime_error& error){
	    std::cout<<"Problem with genParticleCollection in AnalysisModuleRunner.cxx"<<std::endl;
	    std::cout<<error.what();
	  }
	    eh->setup_genmet(context->get("genMETName", ""));
        }

        m_readTrigger = string2bool(context->get("readTrigger", "true"));
        if (m_readTrigger) {
            eh->setup_trigger();
        }

        use_sframe_weight = string2bool(context->get("use_sframe_weight", "true"));
    }

    else{
        m_readTrigger = false;
    }

    // 2. prepare reading additional branches from input:
    additional_branches = split(context->get("additionalBranches", ""));
    first_file = true;
    setup_output_done = false;
}

void AnalysisModuleRunner::BeginInputData(const SInputData& in) {
    pimpl->begin_input_data(*this, in);
}

void AnalysisModuleRunner::BeginInputFile(const SInputData&) {
    // fill trigger names map:
    if (pimpl->m_readTrigger) {
        std::map<int, std::vector<std::string>> run2triggernames;
        // find triggernames for all runs in this file:
        TTree* intree = GetInputTree(pimpl->context->event_treename.c_str());
        if (!intree){
            throw runtime_error("Did not find input tree '" + pimpl->context->event_treename + "' in input file");
        }
        std::vector<std::string> trigger_names;
        std::vector<std::string> *ptrigger_names = &trigger_names;
        int runid = 0;
        TBranch* runb = intree->GetBranch("run");
        TBranch* tnb = intree->GetBranch("triggerNames");
        auto oldaddr_runb = runb->GetAddress();
        auto oldaddr_tnb = tnb->GetAddress();
        if (!runb || !tnb) {
            throw runtime_error("did not find branches for setting up trigger names");
        }
        intree->SetBranchAddress("run", &runid, &runb);
        intree->SetBranchAddress("triggerNames", &ptrigger_names, &tnb);
        int nentries = intree->GetEntries();
        int last_runid = -1;
        for (int i = 0; i < nentries; ++i) {
            runb->GetEntry(i);
            if (last_runid == runid)
                continue;
            assert(runid >= 1);
            tnb->GetEntry(i);
            if (!trigger_names.empty()) {
                last_runid = runid;
                run2triggernames[runid] = trigger_names;
            }
        }
        runb->SetAddress(oldaddr_runb);
        tnb->SetAddress(oldaddr_tnb);
        pimpl->eh->set_infile_triggernames(move(run2triggernames));
    }

    // setup additional branches, if not done yet:
    if(pimpl->first_file && !pimpl->additional_branches.empty()){
        TTree* intree = GetInputTree(pimpl->context->event_treename.c_str());
        if (!intree){
            throw runtime_error("Did not find input tree '" + pimpl->context->event_treename + "' in input file");
        }
        for(const auto & bname : pimpl->additional_branches){
            auto branch = intree->GetBranch(bname.c_str());
            if(!branch){
  	        m_logger  << WARNING << "While setting up additional branches: did not find branch '" << bname << "' in input tree" << SLogger::endmsg;
		continue;
            }
            const auto & ti = InTree::get_type(branch);
            m_logger << VERBOSE  << "Setting up additional branch '" << bname << "';  with (auto-detected) type: " << demangle(ti.name()) << SLogger::endmsg;
            // add it to the context using the same mechanism as declare_event_input / declare_event_output.
            // Note that the call to get_raw_handle might create a new data member in event.
            auto handle = pimpl->ges->get_raw_handle(ti, bname);
            pimpl->context->do_declare_event_input_handle(ti, bname, handle);
            pimpl->context->do_declare_event_output_handle(ti, bname, handle);
        }
    }

    // In case this is the first file in the dataset: construct AnalysisModule now.
    // This is done now (and not earlier, e.g. in BeginInputData) because only now we know
    // the metadata and the additional branches (the latter is needed to fix the event structure).
    if(pimpl->first_file){
        pimpl->context->first_input_file();
        string module_classname = pimpl->context->get("AnalysisModule");
        pimpl->analysis = AnalysisModuleRegistry::build(module_classname, *pimpl->context);
        pimpl->event.reset(new Event(*pimpl->ges));
        if(pimpl->eh){
            pimpl->eh->set_event(pimpl->event.get());
        }
        pimpl->first_file = false;
    }

    pimpl->context->begin_input_file(*pimpl->event);
}

void AnalysisModuleRunner::ExecuteEvent(const SInputData&, Double_t w) {
    // read in the event from the input tree:
    pimpl->context->begin_event(*pimpl->event);
    // copy to Event members and setup trigger:
    if(!pimpl->m_userEventFormat){
        pimpl->eh->event_read();
    }

    uhh2::Event & event = *pimpl->event;

    // setup weight depending on the "use_sframe_weight" configuration option:
    if(pimpl->use_sframe_weight && !event.isRealData){
        event.weight = w;
    }
    else{
        event.weight = 1.0;
    }
    try{
    if(event.genInfo){
        // Use first weight as the central weight, all others are actually
        // variations e.g. scale, parton shower
        event.weight = event.genInfo->weights().at(0);
    }
    }
    catch(const std::runtime_error& error){
	    std::cout<<"Problem with genInfo in AnalysisModuleRunner.cxx"<<std::endl;
	    std::cout<<error.what();
    }
    bool keep = pimpl->analysis->process(event);

    if (!keep) {
        throw SError(SError::SkipEvent);
    }

    pimpl->context->check_output(*pimpl->event);

    if(!pimpl->m_userEventFormat){
        pimpl->eh->event_write();
    }

    if (!pimpl->setup_output_done) {
        pimpl->context->setup_output(*pimpl->event);
        pimpl->setup_output_done = true;
    }
}

void AnalysisModuleRunner::CloseOutputFile(){
    // ideally, we want 'our' metadata output trees handled by SFrame. Unfortunately this is not foreseen.
    // So try to emulate behavior of SFrame, i.e. as is SCycleBaseNTuple::SaveOutputTrees would also save our metadata trees.
    // SaveOutputTrees cannot be overridden directly (it is not virtual), but SaveOutputTrees is only
    // called from CloseOutputFile, so we override that method here to call the save at the right point.
    pimpl->context->write_metadata_tree();
    SCycleBase::CloseOutputFile();
}

void AnalysisModuleRunner::EndMasterInputData(const SInputData &) {
    // Allow analysis module to finish up
    pimpl->analysis->endInputData();

    TList * l = GetHistOutput();
    TIter next(l);
    TObject * obj;

    // populate cutflows:
    map<string, pair<TH1D*, TH1D*> > cutflows; // first: weighted, second: unweighted
    while ((obj = next())) {
        string name(obj->GetName());
        if (name.find("cf_") != 0)
            continue;
        SCycleOutput * out = dynamic_cast<SCycleOutput*>(obj);
        if (!out)
            continue;
        obj = out->GetObject();
        TH1D * cutflow = dynamic_cast<TH1D*>(obj);
        if (!cutflow)
            continue;
        // get the selection name by cutting off "cf_" at the start:
        string sel_name = name.substr(3);
        // cut off "_raw in the end:"
        bool raw = false;
        if (name.size() > 4 and name.find("_raw") == name.size() - 4) {
            sel_name = sel_name.substr(0, sel_name.size() - 4);
            raw = true;
        }
        if (raw) {
            cutflows[sel_name].second = cutflow;
        }
        else {
            cutflows[sel_name].first = cutflow;
        }
    }

    // print them:
    for (map<string, pair<TH1D*, TH1D*> >::const_iterator it = cutflows.begin();
            it != cutflows.end(); ++it) {
        TH1D * cf = it->second.first;
        TH1D * cf_raw = it->second.second;
        if (!cf or !cf_raw or cf->GetNbinsX() != cf_raw->GetNbinsX()) {
            m_logger << WARNING
                    << " did not find all cutflows (or inconsistent cutflows)"
                    << endl;
            continue;
        }
        cout << endl << "Cutflow for selection '" << it->first << "':" << endl;
        vector<string> headers;
        headers.push_back("Selection");
        headers.push_back("N_raw");
        headers.push_back("N_weighted");
        TableOutput out(headers);
        TAxis * xax = cf->GetXaxis();
        for (int ibin = 1; ibin <= cf->GetNbinsX(); ++ibin) {
            vector<string> row;
            row.push_back(xax->GetBinLabel(ibin));
            row.push_back(to_string(cf_raw->GetBinContent(ibin)));
            row.push_back(to_string(cf->GetBinContent(ibin)));
            out.add_row(row);
        }
        out.print(cout);
    }
}

ClassImp(uhh2::AnalysisModuleRunner);

#endif
