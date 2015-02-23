#include "UHH2/core/include/root-utils.h"
#include "UHH2/core/include/Utils.h"

#include "TDataType.h"
#include "TEmulatedCollectionProxy.h"

#include <cassert>
#include <stdexcept>
#include <sstream>

using namespace std;
using namespace uhh2;

namespace {
    
    
// see TTree.cxx from ROOT.
char DataTypeToChar(EDataType datatype){
    switch(datatype) {
        case kChar_t:     return 'B';
        case kUChar_t:    return 'b';
        case kBool_t:     return 'O';
        case kShort_t:    return 'S';
        case kUShort_t:   return 's';
        case kCounter:
        case kInt_t:      return 'I';
        case kUInt_t:     return 'i';
        case kDouble_t:
        case kDouble32_t: return 'D';
        case kFloat_t:
        case kFloat16_t:  return 'F';
        case kLong_t:     return sizeof(Long_t) == sizeof(Long64_t) ? 'L' : 'I';
        case kULong_t:    return sizeof(ULong_t) == sizeof(ULong64_t) ? 'l' : 'i';
        case kchar:       return 0; // unsupported
        case kLong64_t:   return 'L';
        case kULong64_t:  return 'l';

        case kCharStar:   return 'C';
        case kBits:       return 0; //unsupported

        case kOther_t:
        case kNoType_t:
        default:
            return 0;
    }
}

namespace detail {
bool is_class(const std::type_info & ti){
    return TBuffer::GetClass(ti) != nullptr;
}
}


void * allocate_type(const std::type_info & ti, std::function<void (void*)> & deallocator){
    TClass * class_ = TBuffer::GetClass(ti);
    EDataType dt = TDataType::GetType(ti);
    void * result;
    if(class_){
        result = class_->New();
        deallocator = class_->GetDelete();
    }
    else{
        if(dt == kOther_t || dt == kNoType_t){
            throw runtime_error("allocate_type with type called which is not known to ROOT");
        }
        result = new uint64_t(0); // largest primitive type
        deallocator = [](void * addr) {delete reinterpret_cast<uint64_t*>(addr);};
    }
    return result;
}

void write_type_info(ostream & out, const std::type_info & ti) {
    out << "'" << demangle(ti.name()) << "'";
    TClass * class_ = TBuffer::GetClass(ti);
    EDataType dt = TDataType::GetType(ti);
    if (class_) {
        out << " (ROOT class name: '" << class_->GetName() << "')";
    }
    else {
        auto tn = TDataType::GetTypeName(dt);
        if (tn == string("")) {
            out << " (type not known to ROOT!)";
        }
        else {
            out << " (ROOT name: '" << tn << "')";
        }
    }
}


}

   
TBranch * uhh2::tree_branch(TTree * tree, const std::string & bname, void * addr, void ** addraddr, const std::type_info & ti){
    TBranch * b = tree->GetBranch(bname.c_str());
    if(b != 0){
        throw runtime_error("tree_branch: Branch '" + bname + "' in tree '" + tree->GetName() + "' already exists!");
    }
    if(bname.empty()){
        throw invalid_argument("tree_branch: tried to create empty branchname in tree '" + string(tree->GetName()) + "'");
    }
    //emulate
    // template<typename T>
    // outtree->Branch(name, (T*)addr);
    // which expands to
    // outtree->BranchImpRef(name, TBuffer::GetClass(ti), TDataType::GetType(ti), const_cast<void*>(addr), 32000, 99),
    // so look there to understand how this code here works.
    TClass * class_ = TBuffer::GetClass(ti);
    if(class_){
        assert(addr != nullptr);
        TClass * actualclass =  class_->GetActualClass(addr);
        assert(actualclass!=0);
        assert(class_ == actualclass);
        assert(dynamic_cast<TEmulatedCollectionProxy*>(actualclass->GetCollectionProxy())==0); // if this fails, it means T is a STL without compiled STL dictionary
        // I'd like to do now:
        //  outtree->BronchExec(name, actualClass->GetName(), addr, *** kFALSE ****, bufsize, splitlevel);
        // but root doesn't let me, so work around by passing a pointer-to-pointer after all:
        return tree->Bronch(bname.c_str(), class_->GetName(), addraddr);
    }
    else{
        EDataType dt = TDataType::GetType(ti);
        if(dt==kOther_t or dt==kNoType_t) throw invalid_argument("tree_branch: unknown type");
        char c = DataTypeToChar(dt);
        assert(c!=0);
        return tree->Branch(bname.c_str(), addr, (bname + '/' + c).c_str());
    }
}


// *** OutTree ***
void OutTree::create_branch(const std::string & name, const std::shared_ptr<obj> & value){
    if(name.empty()){
        throw invalid_argument("OutTree Branch name is empty");
    }
    const auto & ti = value->type();
    TClass * class_ = TBuffer::GetClass(ti);
    char root_branch_type = '\0';
    if(class_){
        assert(dynamic_cast<TEmulatedCollectionProxy*>(class_->GetCollectionProxy())==0); // if this fails, it means T is a STL without compiled STL dictionary
        tree->Bronch(name.c_str(), class_->GetName(), const_cast<void**>(value->address_of_address()));
    }
    else{
        EDataType dt = TDataType::GetType(ti);
        if(dt==kOther_t or dt==kNoType_t) throw invalid_argument("tree_branch: unknown type");
        root_branch_type = DataTypeToChar(dt);
        assert(root_branch_type!=0);
        tree->Branch(name.c_str(), value->address(), (name + '/' + root_branch_type).c_str());
    }
    branches.emplace_back(name, root_branch_type, class_, value);// TODO: needed?
}

// *** InTree ***

const std::type_info & InTree::get_type(TBranch * branch){
    assert(branch != nullptr);
    TClass * branch_class = nullptr;
    EDataType branch_dtype = kNoType_t;
    int res = branch->GetExpectedType(branch_class, branch_dtype);
    if (res > 0) {
        throw runtime_error("input branch '" + string(branch->GetName()) + "': error calling GetExpectedType");
    }
    if(branch_class){
        return *branch_class->GetTypeInfo();
    }
    else{
        // normalize branch type to use explicit sizes for some types:
        if(branch_dtype == kLong_t){
            if(sizeof(Long_t) == sizeof(Long64_t)){
                branch_dtype = kLong64_t;
            }
            else{
                assert(sizeof(Long_t) == sizeof(Int_t));
                branch_dtype = kInt_t;
            }
        }
        else if(branch_dtype == kULong_t){
            if(sizeof(ULong_t) == sizeof(ULong64_t)){
                branch_dtype = kULong64_t;
            }
            else{
                assert(sizeof(Long_t) == sizeof(Int_t));
                branch_dtype = kUInt_t;
            }
        }
        switch(branch_dtype){
            case kChar_t:     return typeid(Char_t);
            case kUChar_t:    return typeid(UChar_t);
            case kBool_t:     return typeid(Bool_t);
            case kShort_t:    return typeid(Short_t);
            case kUShort_t:   return typeid(UShort_t);
            case kCounter:
            case kInt_t:      return typeid(Int_t);
            case kUInt_t:     return typeid(UInt_t);
            case kDouble_t:
            case kDouble32_t: return typeid(Double_t);
            case kFloat_t:
            case kFloat16_t:  return typeid(Float_t);
            case kLong64_t:   return typeid(int64_t);
            case kULong64_t:  return typeid(uint64_t);
            
            case kLong_t:     // cannot happen as of above normalization
            case kULong_t:    assert(false);

            case kOther_t:
            case kNoType_t:
            default: throw runtime_error("unknown non-class type for branch '" + string(branch->GetName()) + "' / type not known to ROOT");
        }
    }
}

std::shared_ptr<obj> InTree::open_branch(const std::type_info & ti, const std::string & bname){
    std::function<void (void*)> eraser;
    void * ptr = allocate_type(ti, eraser);
    auto result = obj::create(ti, ptr, std::move(eraser));
    connect_branch(bname, result);
    return result;
}

void InTree::visit_branches(const std::function<void (const std::string &, const std::type_info &)> & visitor){
    TIter next(tree->GetListOfBranches());
    while(TObject * obj = next()){
        auto branch = dynamic_cast<TBranch*>(obj);
        assert(branch);
        visitor(branch->GetName(), get_type(branch));
    }
}

void InTree::connect_branch(const std::string & bname, const std::shared_ptr<obj> & value){
    TBranch * branch = tree->GetBranch(bname.c_str());
    if(branch == nullptr){
        throw runtime_error("InTree::connect: did not find branch '" + bname + "'");
    }
    const std::type_info & branch_type = get_type(branch);
    if(value->type() != branch_type){
        stringstream ss;
        ss << "InTree: Type error for branch '" << bname << "': branch holds type ";
        write_type_info(ss, branch_type);
        ss << ", but tried to read into object of type ";
        write_type_info(ss, value->type());
        ss << "";
        throw runtime_error(ss.str());
    }
    if(detail::is_class(branch_type)){
        branch->SetAddress(value->address_of_address());
    }
    else{
        branch->SetAddress(value->address());
    }
    objects.push_back(value);
}


std::shared_ptr<obj> InTree::open_branch(const std::string & bname){
    TBranch * branch = tree->GetBranch(bname.c_str());
    if(branch == nullptr){
        throw runtime_error("InTree::connect: did not find branch '" + bname + "'");
    }
    const std::type_info & branch_type = get_type(branch);
    return open_branch(branch_type, bname);
}

