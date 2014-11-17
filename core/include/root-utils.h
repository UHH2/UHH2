#pragma once

#include "TTree.h"
#include <string>
#include <typeinfo>

namespace uhh2{
    
// Use TTree::Branch with types only known at runtime: given a pointer to T, setup a branch. While this is possible
// with templates, it's not if the type is only known at runtime (like here), so
// for this case, we have to so some painful stuff by reading and re-writing root code ...
void tree_branch(TTree * tree, const std::string & bname, void * addr, void ** addraddr, const std::type_info & ti);

    
}

