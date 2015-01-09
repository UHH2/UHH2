#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/WatchingObjects.h"

#include <stdexcept>

using namespace uhh2;
using namespace std;


class uhh2::Context::MetadataImpl {
public:
    void register_metadata_callback(const std::type_info & ti, const std::string & name, const std::function<void (const void * ptr)> & cb, metadata_source_policy pol, bool call_before_change){
        callbacks.emplace_back(callback{ti, name, cb, pol, call_before_change});
    }
    
    void put_metadata(const std::string & name, bool from_infile, const std::shared_ptr<obj> & object, bool store_in_output){
        void * addr = object->address();
        if(from_infile){
            // WatchingObjects::put takes care of throwing in case of doublicate names
            object_infos.insert(make_pair(addr, object_info{store_in_output, from_infile, object, name, infile_objects.put(name, object)}));
        }
        else{
            object_infos.insert(make_pair(addr, object_info{store_in_output, from_infile, object, name, handle_objects.put(name, object)}));
        }
    }
    
    void notify_callbacks_after(void * object_addr){
        if(phase==0){
            install_callbacks();
            phase = 1;
        }
        else{
            assert(callbacks.empty()); // if this fails, it means that a callback has been registered after a metadata values has been set.
               // this should not happen, as setting up callbacks is only allowed in the AnalysisModule constructor, and setting
               // metadata values only during process.
        }
        auto it = object_infos.find(object_addr);
        assert(it != object_infos.end());
        if(it->second.from_infile){
            infile_objects.notify_callbacks_after(it->second.handle);
        }
        else{
            handle_objects.notify_callbacks_after(it->second.handle);
        }
    }
    
    void notify_callbacks_before(void * object_addr){
        if(phase==0){
            install_callbacks();
            phase = 1;
        }
        else{
            assert(callbacks.empty()); // if this fails, it means that a callback has been registered after a metadata values has been set.
               // this should not happen, as setting up callbacks is only allowed in the AnalysisModule constructor, and setting
               // metadata values only during process.
        }
        auto it = object_infos.find(object_addr);
        assert(it != object_infos.end());
        if(it->second.from_infile){
            infile_objects.notify_callbacks_before(it->second.handle);
        }
        else{
            handle_objects.notify_callbacks_before(it->second.handle);
        }
    }
    
    void visit_metadata_for_output(const std::function<void (const std::string & name, const std::shared_ptr<obj> & object)> & callback){
        if(object_infos.empty()){
            return;
        }
        for(const auto & ptr_oi : object_infos){
            const auto & oi = ptr_oi.second;
            if(!oi.store_in_output) continue;
            // propagate to output, if either:
            // * value comes from handle (=!from_infile) or
            // * value comes from input file and is not shadowed
            // where 'shadowed' means that the same name exists in both object pools: infile and handle.
            bool do_report = !oi.from_infile || !(infile_objects.exists(oi.name) && handle_objects.exists(oi.name));
            if(do_report){
                callback(oi.name, oi.object);
            }
        }
        // the visitor callback just called might have installed new callbacks; add them as well:
        install_callbacks();
    }
    
private:
    void install_callbacks(){
         // setup callbacks, considering source policy:
        for(auto & cb : callbacks){
            bool infile_exists = infile_objects.exists(cb.name);
            bool handle_exists = handle_objects.exists(cb.name);
            if(!infile_exists && !handle_exists) continue;
            WatchingObjects * source = nullptr;
            switch(cb.source_policy){
                case metadata_source_policy::infile_only:
                    if(infile_exists) source = &infile_objects;
                    break;
                case metadata_source_policy::handle_only:
                    if(handle_exists) source = &handle_objects;
                    break;
                case metadata_source_policy::infile_then_handle:
                    if(infile_exists) source = &infile_objects;
                    else source = &handle_objects;
                    break;
                case metadata_source_policy::handle_then_infile:
                    if(handle_exists) source = &handle_objects;
                    else source = &infile_objects;
                    break;
            }
            if(source == nullptr) continue;
            if(cb.call_before_change){
                source->watch_before(cb.ti, cb.name, std::move(cb.callback));
            }
            else{
                source->watch_after(cb.ti, cb.name, std::move(cb.callback));
            }
        }
        callbacks.clear();
    }
    
    // two object pools: one for infile, one created by handles
    WatchingObjects infile_objects;
    WatchingObjects handle_objects;
    
    struct object_info {
        bool store_in_output;
        bool from_infile;
        std::shared_ptr<obj> object;
        std::string name;
        WatchingObjects::Handle handle;
    };
    
    std::map<void*, object_info> object_infos;
    
    // list of callbacks. This is kept during setup phase, for correct shadowing handling,
    // which is done after all setup is complete, when processing the first metadata value assignment.
    struct callback {
        const std::type_info & ti;
        std::string name;
        std::function<void (const void*)> callback;
        metadata_source_policy source_policy;
        bool call_before_change;
    };
    std::list<callback> callbacks;
    int phase = 0;
    
};

AnalysisModule::~AnalysisModule(){}

Context::Context(GenericEventStructure & ges_): ges(ges_), meta(new MetadataImpl()) {}

Context::~Context(){}

void Context::fail(const std::string & key) const{
    throw std::runtime_error("did not find setting '" + key + "'");
}

void Context::register_metadata_callback(const std::type_info & ti, const std::string & name, const std::function<void (const void * ptr)> & callback, metadata_source_policy pol, bool call_before_change){
    meta->register_metadata_callback(ti, name, callback, pol, call_before_change);
}

void Context::put_metadata(const std::string & name, bool from_infile, const std::shared_ptr<obj> & object, bool store_in_output){
    meta->put_metadata(name, from_infile, object, store_in_output);
}

void Context::notify_callbacks_after(void * object_addr){
    meta->notify_callbacks_after(object_addr);
}

void Context::notify_callbacks_before(void * object_addr){
    meta->notify_callbacks_before(object_addr);
}

void Context::visit_metadata_for_output(const std::function<void (const std::string & name, const std::shared_ptr<obj> & object)> & callback){
    meta->visit_metadata_for_output(callback);
}

/** \brief AnalysisModule which does nothing and lets all events pass
 * 
 * Only useful for testing.
 */
class NoopAnalysisModule: public AnalysisModule {
public:
    NoopAnalysisModule(Context &){}
    virtual bool process(Event &){
        return true;
    }
};

UHH2_REGISTER_ANALYSIS_MODULE(NoopAnalysisModule)
