#include "UHH2/core/include/GenericEvent.h"
#include "UHH2/core/include/Utils.h"

#include <stdexcept>
#include <iostream>
#include <cassert>
#include <sstream>

using namespace uhh2;
using namespace std;

GenericEventStructure::RawHandle GenericEventStructure::get_raw_handle(const std::type_info & ti, const std::string & name){
    // check if it exists already. Note that this is a slow (O(N)) operation, but
    // this should be ok as we do not expect this method to be called often
    for(size_t i=0; i<member_infos.size(); ++i){
        if(member_infos[i].name == name && member_infos[i].type == ti){
            return RawHandle(i);
        }
    }
    member_infos.emplace_back(name, ti);
    return RawHandle(member_infos.size() - 1);
}

std::string GenericEventStructure::name(const RawHandle & handle){
    assert(handle.index < member_infos.size());
    return member_infos[handle.index].name;
}


GenericEvent::GenericEvent(const GenericEventStructure & es): member_infos(es.member_infos), member_datas(member_infos.size()){}


void GenericEvent::fail(const std::type_info & ti, const GenericEventStructure::RawHandle & handle, const string & msg) const{
    std::stringstream errmsg;
    errmsg << "Event: error for member of type '" << demangle(ti.name()) << "' at index " << handle.index;
    if(handle.index < member_infos.size()){
        errmsg << " with name '" << member_infos[handle.index].name << "'";
    }
    errmsg << ": " << msg;
    throw std::runtime_error(errmsg.str());
}


void GenericEvent::set_unmanaged(const std::type_info & ti, const RawHandle & handle, void * data){
    check(ti, handle, "set_unmanaged");
    member_data & md = member_datas[handle.index];
    if(md.data != 0 && md.eraser){
        md.eraser->operator()(md.data);
    }
    md.eraser.reset();
    md.data = data;
    md.valid = true;
}

void * GenericEvent::get(const std::type_info & ti, const GenericEventStructure::RawHandle & handle, bool check_valid, bool allow_null){
    return const_cast<void*>(static_cast<const GenericEvent*>(this)->get(ti, handle, check_valid, allow_null));
}

const void * GenericEvent::get(const std::type_info & ti, const GenericEventStructure::RawHandle & handle, bool check_valid, bool allow_null) const{
    check(ti, handle, "get");
    member_data & md = member_datas[handle.index];
    if(md.data == 0){
        if(allow_null) return 0;
        else{
            fail(ti, handle, "member data pointer is null, i.e. this member is known but was never initialized with 'set'");
        }
    }
    // easiest case: data is there and valid:
    if(md.valid){
        return md.data;
    }
    // it's not valid, try to generate it:
    if(md.generator){
        try{
            (*md.generator)();
        }
        catch(...){
            const auto & mi = member_infos[handle.index];
            std::cerr << "Exception while trying to generate element '" << mi.name << "' of type '" << demangle(ti.name()) << "'" << std::endl;
            throw;
        }
        md.valid = true;
        return md.data;
    }
    // if it's not valid and cannot be generated, return the pointer
    // to the 'invalid' member:
    if(check_valid){
        fail(ti, handle, "member data is marked as invalid, i.e. this member is known and has associated memory, but was not marked valid via 'set' or 'set_validity'"); // does not return
        return 0; // only to make compiler happy
    }
    else{
        return md.data;
    }
}

void GenericEvent::check(const std::type_info & ti, const RawHandle & handle, const std::string & where) const{
    if(handle.index >= member_datas.size()){
        fail(ti, handle, where + ": index out of bounds, i.e. the handle used is invalid");
    }
    const auto & mi = member_infos[handle.index];
    if(mi.type!=ti){
        fail(ti, handle, where + ": type mismatch");
    }
}

void GenericEvent::set_validity(const std::type_info & ti, const GenericEventStructure::RawHandle & handle, bool valid){
    check(ti, handle, "set_validity");
    member_data & md = member_datas[handle.index];
    md.valid = valid;
}

GenericEvent::member_data::~member_data(){
    if(eraser){
        eraser->operator()(data);
    }
}

void GenericEvent::invalidate_all(){
    for(auto & md : member_datas){
        md.valid = false;
    }
}

void GenericEvent::set_get_callback(const std::type_info & ti, const RawHandle & handle, boost::optional<std::function<void ()>> callback){
    check(ti, handle, "set_get_callback");
    member_data & md = member_datas[handle.index];
    md.valid = false;
    md.generator = move(callback);
}

GenericEvent::state GenericEvent::get_state(const std::type_info & ti, const RawHandle & handle) const{
    check(ti, handle, "get_state");
    const member_data & md = member_datas[handle.index];
    if(md.data==0) return state::nonexistent;
    return md.valid ? state::valid : state::invalid;
}

GenericEvent::~GenericEvent(){
    // nothing to do: member data belongs to member_datas
}
