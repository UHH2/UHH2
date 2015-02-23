#pragma once

#include "TTree.h"
#include "obj.h"

#include <string>
#include <typeinfo>
#include <list>
#include <functional>
#include <cassert>
#include <memory>

namespace uhh2{
    
// Use TTree::Branch with types only known at runtime: given a pointer to T, setup a branch.
// Note that while this is possible with templates, it's not if the type is only known at runtime (like here), so
// for this case, we have to so some painful stuff by reading and re-writing root code.
// Fails with a runtime_error if a branch of that name already exists.
TBranch * tree_branch(TTree * tree, const std::string & bname, void * addr, void ** addraddr, const std::type_info & ti);


/** \brief Wrapper class for creating branches in a TTree with types only known at runtime
 * 
 * Uses tree_branch internally, but stores the addraddr also.
 */
class OutTree {
public:
    // tree_ has to outlive the OutTree
    explicit OutTree(TTree * tree_): tree(tree_){}
    
    OutTree(const OutTree &) = delete;
    OutTree(OutTree &&) = default;
    
    void create_branch(const std::string & name, const std::shared_ptr<obj> & value);
    
private:
    TTree * tree;
    
    struct binfo {
        std::string name;
        char root_branch_type; // only non-0 for primitive types
        TClass * class_; // only non-NULL for class types
        std::shared_ptr<obj> object;
        
        binfo(const std::string & name_, char root_branch_type_, TClass * class__, const std::shared_ptr<obj> & value): name(name_),
            root_branch_type(root_branch_type_), class_(class__), object(value){}
    };
    
    std::vector<binfo> branches; // TODO: needed?
};


/** \brief Wrapper class for reading objects from a TTree with types only known at runtime
 * 
 * For reading data from a specific branch, two methods with almost same functionality exist:
 *  - connect_branch to read object into an *existing* obj
 *  - open_branch to read object into *new* obj
 * 
 * NOTE: so far, there is not 'disconnect'
 */
class InTree {
public:
    
    explicit InTree(TTree * tree_): tree(tree_){}
    
    InTree(const InTree &) = delete;
    InTree(InTree &&) = default;
    
    static const std::type_info & get_type(TBranch * branch);
    
    // connect a branch to an existing object container
    void connect_branch(const std::string & bname, const std::shared_ptr<obj> & value);
    
    // create a new obj container and connect the branch to it. With type checking.
    template<typename T>
    std::shared_ptr<obj> open_branch(const std::string & bname){
        static_assert(!std::is_pointer<T>::value, "T must not be of pointer type");
        return open_branch(typeid(T), bname);
    }
    
    std::shared_ptr<obj> open_branch(const std::type_info & ti, const std::string & bname);
    
    // without type checking: the obj will have the suitable type for that branch.
    std::shared_ptr<obj> open_branch(const std::string & bname);
    
    // visit all available branches, giving their name and type
    void visit_branches(const std::function<void (const std::string &, const std::type_info &)> & visitor);
    
private:
    TTree * tree;
    
    std::list<std::shared_ptr<obj>> objects; // used to guarantee keeping address-to-address alive long enough. TODO: needed?
};
    
}

