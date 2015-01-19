#include "UHH2/core/include/WatchingObjects.h"
#include "UHH2/core/include/Utils.h"

using namespace uhh2;
using namespace std;

WatchingObjects::Handle WatchingObjects::put(const std::string & name, const std::shared_ptr<obj> & object){
    int index = find(name);
    if(index >= 0){
        if(objects[index].value){
            throw runtime_error("WatchingObjects::create: object with name '" + name + "' already exists");
        }
        else{
            if(objects[index].ti != object->type()){
                fail_type(objects[index], object->type());
            }
            objects[index].value = object;
            return Handle(index);
        }
    }
    objects.emplace_back(name, object->type());
    objects.back().value = object;
    return Handle(objects.size() - 1);
}

void WatchingObjects::notify_callbacks_before(const Handle & h){
    if(h.index < 0) fail_handle();
    auto & oi = objects[h.index];
    if(!oi.value) fail_handle();
    for(const auto & c : oi.callbacks_before){
        c(oi.value->address());
    }
}

void WatchingObjects::notify_callbacks_after(const Handle & h){
    if(h.index < 0) fail_handle();
    auto & oi = objects[h.index];
    if(!oi.value) fail_handle();
    for(const auto & c : oi.callbacks_after){
        c(oi.value->address());
    }
}

void WatchingObjects::watch_after(const std::type_info & ti, const std::string & name, const std::function<void (const void*)> & callback){
    int index = find(name);
    if(index >= 0){
        if(objects[index].value->type() != ti){
            fail_type(objects[index], ti);
        }
    }
    else{
        // create new:
        objects.emplace_back(name, ti);
        index = objects.size() - 1;
    }
    objects[index].callbacks_after.push_back(callback);
}

void WatchingObjects::watch_before(const std::type_info & ti, const std::string & name, const std::function<void (const void*)> & callback){
    int index = find(name);
    if(index >= 0){
        if(objects[index].value->type() != ti){
            fail_type(objects[index], ti);
        }
    }
    else{
        // create new:
        objects.emplace_back(name, ti);
        index = objects.size() - 1;
    }
    objects[index].callbacks_before.push_back(callback);
}

void WatchingObjects::visit(const std::function<void (const std::string &, const Handle &, const std::shared_ptr<obj> &)> & visitor){
    int i=0;
    for(const auto & oi : objects){
        if(!oi.value) continue;
        visitor(oi.name, Handle(i++), oi.value);
    }
}

int WatchingObjects::find(const std::string & name) const{
    for(size_t i=0; i<objects.size(); ++i){
        if(objects[i].name == name) return i;
    }
    return -1;
}

void WatchingObjects::fail_type(const object_info & oi, const std::type_info & type) const {
    throw invalid_argument("WatchingObjects: Wrong type for object '" + oi.name + "': exists in pool with type '"
                         + demangle(oi.ti.name()) + "', but was accessed using type '" + demangle(type.name()) + "'");
}


void WatchingObjects::fail_handle() const {
    throw invalid_argument("WatchingObjects: invalid handle");
}
