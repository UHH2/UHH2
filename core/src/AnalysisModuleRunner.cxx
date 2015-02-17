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


class MetaDataInputTree {
public:
    explicit MetaDataInputTree(TTree * tree_): tree(tree_), intree(tree){
        o_nevents = intree.open_branch<int64_t>("nevents");
        o_data = intree.open_branch("data");
        cout << "Setup metadata input tree with name '" << string(tree->GetName()) << "' holding metadata of type '" << demangle(o_data->type().name()) << "'" << endl;
    }
    
    const shared_ptr<obj> & data() const {
        return o_data;
    }
    
    // index is the event index
    // update metadata (including calling the callbacks(!))
    void update_metadata(int64_t index, SFrameContext * ctx);
    
private:
    TTree * tree; // belongs to input file
    InTree intree;
    
    shared_ptr<obj> o_nevents, o_data;
    
    int64_t valid_from = 0, valid_to = 0; // current data is valid for event tree indices [valid_from, valid_to)
    
    int64_t current_md_entry = -1; // entry number of metadata tree of current data
};

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
    friend class ::MetaDataInputTree;
    
    SFrameContext(AnalysisModuleRunner & base, const SInputData& sin, GenericEventStructure & es);

    virtual void put(const std::string & path, TH1 * t) override;

    ~SFrameContext();

    // this should be called from SCycleBase::BeginInputFile. It takes care
    // of setting up the input addresses into the event.
    void begin_input_file(Event & event);
    void begin_event(Event & event);
    void setup_output(Event & event); // should be called after processing the first event which is written to the output
    
    void write_metadata_trees(); // call TTree::Write for all metadata trees; is called just before closing output file
    void init_metadata_output(); // create all metadata trees and branches; is called just before writing the first event to the output

    std::string event_treename;
    
    TTree * outtree = 0; // output tree. Can be 0 in case no output should be written.
    
    void do_declare_event_input_handle(const std::type_info & ti, const std::string & bname, const GenericEvent::RawHandle & handle);
    void do_declare_event_output_handle(const std::type_info & ti, const std::string & bname, const GenericEvent::RawHandle & handle);

private:

    // event tree i/o
    // note that the declare_input does nothing but saving the parameters for later; the actual setup of
    // input branches is done in begin_input_file.
    virtual void do_declare_event_input(const std::type_info & ti, const std::string & bname, const std::string & mname) override;
    virtual void do_declare_event_output(const std::type_info & ti, const std::string & bname, const std::string & mname) override;

    TDirectory * get_tmpdir();

    AnalysisModuleRunner & base;

    TTree * input_tree = 0; // input event tree

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
    struct metadata_output_handler {
        TTree * event_outtree;
        
        TTree * tree; // belongs to root output file / directory
        std::unique_ptr<OutTree> outtree;
        
        std::shared_ptr<obj> o_data;
        std::shared_ptr<obj> o_nevents;
        
        int64_t last_nentries = 0;
        
        void write_current_metadata(){
            int64_t current_nentries = event_outtree->GetEntries();
            int64_t valid_for = current_nentries - last_nentries;
            if(valid_for == 0) return;
            o_nevents->assign<int64_t>(valid_for);
            last_nentries = current_nentries;
            tree->Fill();
        }
    };

    std::map<std::string, metadata_output_handler> metadata_out; // metadata name to info
    
    struct metadata_input_info {
        const type_info & type;
        bool found = true;
        
        explicit metadata_input_info(const std::type_info & ti_): type(ti_){
        }
    };
    
    // per input file:
    std::map<std::string, std::unique_ptr<MetaDataInputTree>> metadata_input_trees;
    // dataset-global, to check consistency across input files:
    std::map<std::string, metadata_input_info> metadata_input_infos;
};

}


void MetaDataInputTree::update_metadata(int64_t index, SFrameContext * ctx){
    // update metadata from input file if necessary
    if(valid_from <= index && index < valid_to) return;
    
    // need to update. Tell others:
    ctx->notify_callbacks_before(o_data->address());
    const int64_t nentries = tree->GetEntries();
    if(index < valid_from){
        current_md_entry = -1;
        valid_from = valid_to = 0;
    }
    do{
        ++current_md_entry;
        if(current_md_entry == nentries){
            throw runtime_error("inconsistent metadata: metadata does not cover all events");
        }
        tree->GetEntry(current_md_entry);
        valid_from = valid_to;
        valid_to += o_nevents->get<int64_t>();
    }while(!(valid_from <= index && index < valid_to));
    ctx->notify_callbacks_after(o_data->address());
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

TDirectory * SFrameContext::get_tmpdir() {
    // see SCycleBaseHist::GetTempDir (which is private ...):
    static TDirectory* tempdir = 0;
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


void SFrameContext::write_metadata_trees(){
    if(!outtree) return;
    // see SCycleNTupleBase::SaveOutputTrees
    bool isProof = base.GetNTupleInput()->FindObject("PROOF_OUTPUTFILE"); //  SFrame::ProofOutputName = "PROOF_OUTPUTFILE"
    TDirectory * dir_before = gDirectory;
    for(auto & m : metadata_out){
        auto & info = m.second;
        if(info.tree){
            info.write_current_metadata();
            if(info.tree->GetEntries() || !isProof){
                TDirectory * dir = info.tree->GetDirectory();
                if(dir)  dir->cd();
                info.tree->Write();
                info.tree->AutoSave();
            }
            info.tree->SetDirectory(0);
            delete info.tree;
        }
    }
    gDirectory = dir_before;
}

void SFrameContext::init_metadata_output(){
    assert(outtree);
    visit_metadata_for_output([this](const string & name, const shared_ptr<obj> & object) -> void {
        auto it = metadata_out.insert(make_pair(name, metadata_output_handler()));
        auto & info = it.first->second;
        info.event_outtree = outtree;
        auto dir = outtree->GetDirectory();
        if(dir){
            dir->cd();
        }
        string treename = "meta_" + name;
        info.tree = new TTree(treename.c_str(), treename.c_str()); // belongs to output dir
        // put output TTree in the same directory as the event tree:
        info.tree->SetDirectory(dir);
        info.outtree.reset(new OutTree(info.tree));
        info.o_data = object;
        info.o_nevents = obj::create<int64_t>(0);
        info.outtree->create_branch("data", info.o_data);
        info.outtree->create_branch("nevents", info.o_nevents);
        // install callback to be called *before* a metadata change, to ensure *old* value is written:
        register_metadata_callback(object->type(), name, [&info](const void *){ info.write_current_metadata(); }, metadata_source_policy::handle_then_infile, true);
    });
}

void SFrameContext::begin_input_file(Event & event) {
    input_tree = base.GetInputTree(event_treename.c_str());
    if (input_tree == 0) {
        throw runtime_error("Could not find input tree '" + event_treename + "'");
    }
    for (auto & name_bi : event_input_bname2bi) {
        const string & bname = name_bi.first;
        branchinfo & bi = *name_bi.second;
        TBranch * branch = input_tree->GetBranch(bname.c_str());
        if (branch == 0) {
            throw runtime_error("Could not find branch '" + bname + "' in tree '" + event_treename + "'");
        }
        connect_input_branch(branch, bi.ti, &bi.addr, bi.eraser);
        EventAccess_::set_unmanaged(event, bi.ti, bi.handle, bi.addr);
        bi.branch = branch;
    }
    // discover metadata trees:
    metadata_input_trees.clear();
    for(auto & mi : metadata_input_infos){
        mi.second.found = false;
    }
    auto dir = input_tree->GetDirectory();
    if(!dir){
        throw invalid_argument("input tree has no TDirectory");
    }
    TIter next(dir->GetListOfKeys());
    while(TObject * obj = next()){
        auto key = dynamic_cast<TKey*>(obj);
        assert(key);
        if(std::string(key->GetClassName()) == "TTree"){
            auto treename = std::string(key->GetName());
            if(treename.find("meta_") != 0) continue;
            std::string metaname = treename.substr(strlen("meta_"));
            auto tree = dynamic_cast<TTree*>(key->ReadObj());
            assert(tree);
            auto new_metadata_it = metadata_input_trees.insert(make_pair(metaname, make_unique<MetaDataInputTree>(tree)));
            assert(new_metadata_it.second); // should be a new entry
            const MetaDataInputTree & new_md_tree = *new_metadata_it.first->second;
            // look whether we know about it already:
            auto it = metadata_input_infos.find(metaname);
            if(it != metadata_input_infos.end()){
                // yes: do type checking:
                if(it->second.type != new_md_tree.data()->type()){
                    throw runtime_error("Found inconsistent type for metadata '" + metaname + "'");
                }
            }
            else{
                // no: save to trigger a consistency check in next input file ...
                metadata_input_infos.insert(make_pair(metaname, metadata_input_info(new_md_tree.data()->type())));
                // ... and announce the found metadata via the Context base class:
                put_metadata(metaname, true, new_md_tree.data(), true);
            }
        }
    }
    
    for(auto & mi : metadata_input_infos){
        if(!mi.second.found){
            throw runtime_error("Did not find metadata of name '" + mi.first + "' in input file, although it has been there in previous files of this dataset.");
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
    // update metadata:
    for(const auto & md : metadata_input_trees){
        md.second->update_metadata(ientry, this);
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
}

void SFrameContext::do_declare_event_input(const type_info & ti, const string & bname, const string & mname) {
    auto handle = ges.get_raw_handle(ti, mname);
    do_declare_event_input_handle(ti, bname, handle);
}

void SFrameContext::do_declare_event_output(const type_info & ti, const string & bname, const string & mname) {
    auto handle = ges.get_raw_handle(ti, mname);
    do_declare_event_output_handle(ti, bname, handle);
}

void SFrameContext::do_declare_event_input_handle(const type_info & ti, const string & bname, const GenericEvent::RawHandle & handle) {
    event_input_bname2bi.insert(make_pair(bname, make_unique<branchinfo>(ti, handle)));
}

void SFrameContext::do_declare_event_output_handle(const type_info & ti, const string & bname, const GenericEvent::RawHandle & handle) {
    event_output_bname2bi.insert(make_pair(bname, make_unique<branchinfo>(ti, handle)));
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
    bool additional_branches_setup = false;

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
void AnalysisModuleRunner::Initialize(TXMLNode* node) throw (SError) {
    TXMLNode* nodes = node->GetChildren();
    while (nodes != 0) {
        if (!nodes->HasChildren()) {
            nodes = nodes->GetNextNode();
            continue;
        }
        if (nodes->GetNodeName() == string("UserConfig")) {
            TXMLNode* userNode = nodes->GetChildren();
            while (userNode != 0) {
                if (!userNode->HasAttributes()
                        || (userNode->GetNodeName() != TString("Item"))) {
                    userNode = userNode->GetNextNode();
                    continue;
                }

                std::string name, stringValue;
                TListIter userAttributes(userNode->GetAttributes());
                TXMLAttr* attribute;
                while ((attribute = dynamic_cast<TXMLAttr*>(userAttributes()))
                        != 0) {
                    if (attribute->GetName() == string("Name"))
                        name = attribute->GetValue();
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
        eh->setup_met(context->get("METName", ""));

        bool is_mc = context->get("dataset_type") == "MC";
        if (is_mc) {
            eh->setup_genInfo(context->get("GenInfoName", "genInfo"));
            eh->setup_genjets(context->get("GenJetCollection", ""));
            eh->setup_gentopjets(context->get("GenTopJetCollection", ""));
            eh->setup_genparticles(context->get("GenParticleCollection", ""));
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
    
    // 2. read additional branches from input:
    additional_branches = split(context->get("additionalBranches", ""));
    additional_branches_setup = false;

    // 3. now construct user module, which could add event contest to ges
    string module_classname = context->get("AnalysisModule");
    analysis = AnalysisModuleRegistry::build(module_classname, *context);

    event.reset(new Event(*ges));
    if(eh){
        eh->set_event(event.get());
    }
    setup_output_done = false;
}

void AnalysisModuleRunner::BeginInputData(const SInputData& in) throw (SError) {
    pimpl->begin_input_data(*this, in);
}

void AnalysisModuleRunner::BeginInputFile(const SInputData&) throw (SError) {
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
        if (runb == 0 || tnb == 0) {
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
    if(!pimpl->additional_branches_setup && !pimpl->additional_branches.empty()){
        TTree* intree = GetInputTree(pimpl->context->event_treename.c_str());
        if (!intree){
            throw runtime_error("Did not find input tree '" + pimpl->context->event_treename + "' in input file");
        }
        for(const auto & bname : pimpl->additional_branches){
            auto branch = intree->GetBranch(bname.c_str());
            if(!branch){
                throw runtime_error("While setting up additional branches: did not find branch '" + bname + "' in input tree");
            }
            const auto & ti = InTree::get_type(branch);
             m_logger << VERBOSE  << "Setting up additional branch '" << bname << "';  with (auto-detected) type: " << demangle(ti.name()) << SLogger::endmsg;
            // add it to the list of branches the context using the same mechanism as declare_event_input / declare_event_output.
            // Note that the call to get_raw_handle might create a new data member in event.
            auto handle = EventAccess_::get_raw_handle(*pimpl->event, ti, bname);
            pimpl->context->do_declare_event_input_handle(ti, bname, handle);
            pimpl->context->do_declare_event_output_handle(ti, bname, handle);
        }
        pimpl->additional_branches_setup = true;
    }
    
    pimpl->context->begin_input_file(*pimpl->event);
}

void AnalysisModuleRunner::ExecuteEvent(const SInputData&, Double_t w) throw (SError) {
    // read in the event from the input tree:
    pimpl->context->begin_event(*pimpl->event);
    // copy to Event members and setup trigger:
    if(!pimpl->m_userEventFormat){
        pimpl->eh->event_read();
    }

    uhh2::Event & event = *pimpl->event;
    
    // setup weight depending on the "use_sframe_weight" configuration option:
    if(pimpl->use_sframe_weight){
        event.weight = w;
    }
    else{
        event.weight = 1.0;
    }

    bool keep = pimpl->analysis->process(event);

    if (!keep) {
        throw SError(SError::SkipEvent);
    }

    if(!pimpl->m_userEventFormat){
        pimpl->eh->event_write();
    }

    if (!pimpl->setup_output_done) {
        pimpl->context->setup_output(*pimpl->event);
        if(pimpl->context->outtree){
            pimpl->context->init_metadata_output();
        }
        pimpl->setup_output_done = true;
    }
}

void AnalysisModuleRunner::CloseOutputFile() throw( SError ){
    // ideally, we want 'our' metadata output trees handled by SFrame. Unfortunately this is not foreseen.
    // So try to emulate behavior of SFrame, i.e. as is SCycleBaseNTuple::SaveOutputTrees would also save our metadata trees.
    // SaveOutputTrees cannot be overridden directly (it is not virtual), but SaveOutputTrees is only
    // called from CloseOutputFile, so we override that method here to call the save at the right point.
    pimpl->context->write_metadata_trees();
    SCycleBase::CloseOutputFile();
}

void AnalysisModuleRunner::EndMasterInputData(const SInputData &) throw (SError) {
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
        if (cf == 0 or cf_raw == 0 or cf->GetNbinsX() != cf_raw->GetNbinsX()) {
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
