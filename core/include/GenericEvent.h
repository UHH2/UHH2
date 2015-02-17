#pragma once

#include <typeinfo>
#include <typeindex>
#include <algorithm>
#include <memory>
#include <boost/optional.hpp>

namespace uhh2 {

/** \brief A generic, extensible event container for saving arbitrary kind of data
 * 
 * This event container is a replacement of event data structs and allows to dynamically define
 * the actual 'content' of the struct.
 * 
 * Using this class is split in two steps: First, the structure of the event container is defined via GenericEventStructure.
 * Using this structure, an actual GenericEvent is created. This separation corresponds to the declaration of a struct to
 * actually defining one and allocating memory for it. It also has the (wanted) side-effect that all members have to be declared
 * at an early point of the program, in particular outside of any tight loops.
 * 
 * Elements are identified by their type and name; it is possible to use the same name more than once for different types.
 * The exact same type has to be used for identifying an element in 'get' and 'set'; so it is NOT possible to 'get' via
 * a base class of what has been used to 'set' the data.
 * 
 * To invalidate the value of a data member, call 'set_validity(..., false)'. This marks the member data as invalid
 * and makes a subsequent 'get' fail with an error. This invalidation mechanism is mainly used as consistency check to prevent
 * using outdated data from the previous event: Just before a new event is read, all data is marked as invalid. (While such a mechanism
 * could also be implemented via a "erase" method, that would entail frequent re-allocation in tight loops which is avoided
 * here. Also, erase has other problems, see below.)
 * 
 * In addition to the get/set interface, it is also possible to register a callback via 'set_get_callback' which is
 * called if 'get' is called on an invalid data member. This allows implementing lazy evaluation (or lazy reads) for
 * data. Note that the callback is *only* called when accessing members in 'invalid' state. This effectively caches
 * the result of the callback.
 * 
 * Implementation note: most methods are implemented twice, once as template methods taking the type of the data member
 * as template argument and once as a 'raw' version using a runtime type_info as additional argument instead of a compile-time
 * template argument. Users usually only need the templated version; the 'raw' versions are needed by the framework to support
 * setting members whose type is not known at compile time but only at run time. Also, this avoids template code bloat as templated
 * versions of the methods just forward the call to the 'raw' ones.
 */
class EventAccess_;
class GenericEventStructure {
friend class GenericEvent;
public:
    class RawHandle;
    
    template<typename T>
    class Handle {
        friend class GenericEventStructure;
        friend class GenericEvent;
        uint64_t index;
        Handle<T>(const RawHandle & handle): index(handle.index){}
    public:
        Handle<T>(): index(-1){}
        bool operator==(const Handle<T> & other) const{
            return index == other.index;
        }
    };
    
    class RawHandle {
        friend class GenericEventStructure;
        friend class GenericEvent;
        friend class EventAccess_;
        uint64_t index;
        template<typename T>
        RawHandle(const Handle<T> & h): index(h.index){}
        explicit RawHandle(uint64_t index_): index(index_){}
        
    public:
        RawHandle(): index(-1){}
        bool operator==(const RawHandle & other) const {
            return index == other.index;
        }
    };
    
    template<typename T>
    Handle<T> get_handle(const std::string & name){
        return Handle<T>(get_raw_handle(typeid(T), name));
    }
    
    RawHandle get_raw_handle(const std::type_info & ti, const std::string & name);
    
    template<typename T>
    std::string name(const Handle<T> & handle){
        return name(RawHandle(handle));
    }
    
    std::string name(const RawHandle & handle);
    
private:
    struct member_info {
        std::string name;
        const std::type_info & type; // points to static global data
        
        member_info(const std::string & name_, const std::type_info & ti): name(name_), type(ti){}
    };
    std::vector<member_info> member_infos;
};

class GenericEvent {
friend class EventAccess_;
public:
    enum class state { nonexistent, invalid, valid };
    
    typedef GenericEventStructure::RawHandle RawHandle;
    template<class T> using Handle = GenericEventStructure::Handle<T>;
    
    explicit GenericEvent(const GenericEventStructure &);
    
    // prevent copying and moving, as we manage memory
    GenericEvent(const GenericEvent &) = delete;
    GenericEvent(GenericEvent &&) = delete;
    
    /** \brief Get element data for a member
     *
     * If the data member does not exist (state 'nonexistent'), or the Handle is invalid, an exception is thrown.
     * 
     * In other states, the behavior depends on 'check_valid':
     * If check_valid is true and the member is in state 'invalid', and exception is thrown, otherwise
     * a reference to the member data is returned.
     */
    template<typename T>
    T & get(const Handle<T> & handle){
        return *(reinterpret_cast<T*>(get(typeid(T), handle, true, false)));
    }
    
    template<typename T>
    const T & get(const Handle<T> & handle) const{
        return *(reinterpret_cast<const T*>(get(typeid(T), handle, true, false)));
    }
    
    template<typename T>
    T & get_default(const Handle<T> & handle, T & default_value){
        T * result = reinterpret_cast<T*>(get(typeid(T), handle, true, true));
        if(result == nullptr){
            return default_value;
        }
        else{
            return *result;
        }
    }
    
    template<typename T>
    const T & get_default(const Handle<T> & handle, const T & default_value) const{
        return const_cast<GenericEvent*>(this)->get_default(handle, const_cast<T&>(default_value));
    }
    
    /** \brief Set an element, allocating memory if necessary
     * 
     * This either calls the constructor T(U) if this is the first time
     * the data is set for that handle; otherwise, it uses t::operator=(U).
     *
     * The element's new state will be 'valid'.
     */
    template<typename T, typename U>
    void set(const Handle<T> & handle, U && value){
        check(typeid(T), RawHandle(handle), "set");
        member_data & md = member_datas[handle.index];
        if(md.data != nullptr){
            *(reinterpret_cast<T*>(md.data)) = std::forward<U>(value);
        }
        else{
            md.data = new T(std::forward<U>(value));
            md.eraser.reset(new eraser<T>());
        }
        md.valid = true;
    }
    
    // test whether the data pointed to by handle is available and marked as valid.
    // In case a callback is installed, it might be called from is_valid.
    template<typename T>
    bool is_valid(const Handle<T> & handle) const {
        return get_state(typeid(T), handle) == state::valid;
    }
    
    ~GenericEvent();

    
    // NOTE: All of the 'protected' methods can be accessed via the EventAccess_ class, see below.
    // This should be used only from the core framework, however.
protected:
    
    /// type-erased versions of get:
    void * get(const std::type_info & ti, const RawHandle & handle, bool check_valid, bool allow_null);
    const void * get(const std::type_info & ti, const RawHandle & handle, bool check_valid, bool allow_null) const;
    
    /** \brief Install a get callback
     * 
     * The callback is called whenever 'get' is called on an invalid member; after the call,
     * the member state is set to 'valid', i.e. the callback is not called again
     * until the member is marked as 'invalid'.
     * 
     * The data member identified by T and name must exist (be non-null) already. Its state will be set to
     * invalid to trigger the callback on the next call to 'get'.
     */
    void set_get_callback(const std::type_info & ti, const RawHandle & handle, std::function<void ()> callback);
    
    /// This can be used to manually change the flag from 'valid' to 'invalid' and vice versa.
    void set_validity(const std::type_info & ti, const RawHandle & handle, bool valid);
    
    /// Get the current state of an element
    state get_state(const std::type_info & ti, const RawHandle & handle) const;
    
    // set a data member via the handle to the given pointer. This pointer memory will NOT
    // be managed by this Event container. The caller is responsible to keep the pointer valid long enough
    // and dispose of the memory.
    // Only valid if the member is currently null or if data matches the address already present (this
    // is to ensure that an address -- once set -- always remains the same, so that this assumption
    // is safe e.g. when setting up output trees).
    void set_unmanaged(const std::type_info & ti, const RawHandle & handle, void * data);
    
    /** \brief Set all member data states to invalid
     * 
     * This is usually called just before reading a new event, to mark all previous
     * data (from the previous event) as invalid to catch unwanted re-use of old data.
     */
    void invalidate_all();
    
    // get an existing / create a new handle. Note that this
    // (unlike all other methods, and unlike the user-point-of-view!) allows to effectively
    // change the event structure *after* creating the Event instance.
    RawHandle get_raw_handle(const std::type_info & ti, const std::string & name){
        RawHandle result = structure.get_raw_handle(ti, name);
        if(result.index >= member_datas.size()){
            member_datas.resize(result.index + 1);
        }
        return result;
    }
    
private:
    void fail(const std::type_info & ti, const RawHandle & handle, const std::string & msg) const;
    void check(const std::type_info & ti, const RawHandle & handle, const std::string & where) const;
    
    void try_to_generate(const std::type_info & ti, const RawHandle & handle) const;
    
    struct eraser_base{
        virtual ~eraser_base(){}
        virtual void operator()(void *) = 0;
    };
    
    template<typename T>
    struct eraser: public eraser_base {
        virtual void operator()(void * p){
            T * t = reinterpret_cast<T*>(p);
            delete t;
        }
    };
    
    // type-erased data member.
    // Life-cycle: it is created upon handle creation with data=0, eraser=0, generator=none, valid=false.
    // 'set' sets data, eraser, valid=true. Now, it is 'valid'.
    // 'set_validity' can be used to set it to 'invalid' or 'valid' now.
    // 'set_get_callback' sets generator
    // destructor calls the eraser on data, if eraser is set.
    struct member_data {
        bool valid = false;
        void * data = nullptr; // managed by eraser. Can be 0 if not 'set' yet, but handle exists, or if unmanaged.
        std::unique_ptr<eraser_base> eraser;
        std::function<void ()> generator;
        
        ~member_data();
        member_data(const member_data &) = delete;
        member_data(member_data &&) = default; // note: default is save although this class manages the memory of 'data', as a move will make 'eraser' null, preventing double delete of data.
        member_data(){}
        void operator=(const member_data & other) = delete;
        void operator=(member_data && other) = delete;
    };
    
    GenericEventStructure structure;
    mutable std::vector<member_data> member_datas; // note: mutable required for lazy generation of data with const access
};


// Class which provides access to all the protected methods of GenericEvent.
// NOTE: This is for framework-internal use only, do not use it in analysis code, where you should
// never need to use access the internals exposed here. Rather, work with the public interfaces of GenericEvent(Structure).
class EventAccess_ {
public:
    static void * get(GenericEvent & event, const std::type_info & ti, const GenericEvent::RawHandle & handle, bool check_valid, bool allow_null){
        return event.get(ti, handle, check_valid, allow_null);
    }
    
    static const void * get(const GenericEvent & event, const std::type_info & ti, const GenericEvent::RawHandle & handle, bool check_valid, bool allow_null){
        return event.get(ti, handle, check_valid, allow_null);
    }

    static void set_get_callback(GenericEvent & event,const std::type_info & ti, const GenericEvent::RawHandle & handle, std::function<void ()> callback){
        event.set_get_callback(ti, handle, std::move(callback));
    }
    
    static void set_validity(GenericEvent & event,const std::type_info & ti, const GenericEvent::RawHandle & handle, bool valid){
        event.set_validity(ti, handle, valid);
    }
    
    static GenericEvent::state get_state(const GenericEvent & event,const std::type_info & ti, const GenericEvent::RawHandle & handle){
        return event.get_state(ti, handle);
    }
    
    static void set_unmanaged(GenericEvent & event,const std::type_info & ti, const GenericEvent::RawHandle & handle, void * data){
        event.set_unmanaged(ti, handle, data);
    }
    
    template<typename T>
    static void set_unmanaged(GenericEvent & event, const GenericEvent::Handle<T> & handle, void * data){
        event.set_unmanaged(typeid(T), handle, data);
    }

    static void invalidate_all(GenericEvent & event){
        event.invalidate_all();
    }
    
    static GenericEvent::RawHandle get_raw_handle(GenericEvent & event, const std::type_info & ti, const std::string & name){
        return event.get_raw_handle(ti, name);
    }
};

}


