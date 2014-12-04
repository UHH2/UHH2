#include "UHH2/core/include/root-utils.h"

#include "TDataType.h"
#include "TEmulatedCollectionProxy.h"

#include <cassert>
#include <stdexcept>

using namespace std;

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
        case kLong_t:     return 0; // unsupported
        case kULong_t:    return 0; // unsupported?
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

}

   
void uhh2::tree_branch(TTree * tree, const std::string & bname, void * addr, void ** addraddr, const std::type_info & ti){
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
        TClass * actualclass =  class_->GetActualClass(addr);
        assert(actualclass!=0);
        assert((class_ == actualclass) || actualclass->InheritsFrom(class_));
        assert(dynamic_cast<TEmulatedCollectionProxy*>(actualclass->GetCollectionProxy())==0); // if this fails, it means T is a STL without compiled STL dictionary
        // I'd like to do now:
        //  outtree->BronchExec(name, actualClass->GetName(), addr, *** kFALSE ****, bufsize, splitlevel);
        // but root doesn't let me, so work around by passing a pointer-to-pointer after all:
        tree->Bronch(bname.c_str(), actualclass->GetName(), addraddr);
    }
    else{
        EDataType dt = TDataType::GetType(ti);
        if(dt==kOther_t or dt==kNoType_t) throw invalid_argument("unknown class");
        char c = DataTypeToChar(dt);
        assert(c!=0);
        tree->Branch(bname.c_str(), addr, (bname + '/' + c).c_str());
    }
}

