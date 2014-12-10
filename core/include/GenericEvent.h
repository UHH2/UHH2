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
public:
    enum class state { nonexistent, invalid, valid };
    
    typedef GenericEventStructure::RawHandle RawHandle;
    template<class T> using Handle = GenericEventStructure::Handle<T>;
    
    explicit GenericEvent(const GenericEventStructure &);
    
    // prevent copying and moving, as we manage memory
    GenericEvent(const GenericEvent &) = delete;
    GenericEvent(GenericEvent &&) = delete;
    
    /** \brief Get an element of this event
     *
     * If the data member does not exist (state 'nonexistent'), an exception is thrown.
     * 
     * In other states, the behavior depends on 'check_valid':
     * If check_valid is true and the member is in state 'invalid', and exception is thrown, otherwise
     * a reference to the member data is returned.
     */
    template<typename T>
    T & get(const Handle<T> & handle, bool check_valid = true){
        return *(reinterpret_cast<T*>(get(typeid(T), handle, check_valid, false)));
    }
    
    template<typename T>
    const T & get(const Handle<T> & handle, bool check_valid = true) const{
        return *(reinterpret_cast<const T*>(get(typeid(T), handle, check_valid, false)));
    }
    
    template<typename T>
    T & get_default(const Handle<T> & handle, T & default_value){
        T * result = reinterpret_cast<T*>(get(typeid(T), handle, true, true));
        if(result == 0){
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
    
    // type-erased versions:
    void * get(const std::type_info & ti, const RawHandle & handle, bool check_valid = true, bool allow_null = false);
    const void * get(const std::type_info & ti, const RawHandle & handle, bool check_valid = true, bool allow_null = false) const;
    
    /** \brief Set an element, allocating memory if necessary
     *
     * The element's new presence state will be 'valid'.
     */
    template<typename T>
    void set(const Handle<T> & handle, T value){
        check(typeid(T), RawHandle(handle), "set");
        member_data & md = member_datas[handle.index];
        if(md.data != 0){
            *(reinterpret_cast<T*>(md.data)) = std::move(value);
        }
        else{
            md.data = new T(std::move(value));
            md.eraser.reset(new eraser<T>());
        }
        md.valid = true;
    }
    
    // set a data member via the handle to the given pointer. This pointer memory will NOT
    // be managed by this Event container. The caller is responsible to keep the pointer valid long enough
    // and dispose of the memory.
    void set_unmanaged(const std::type_info & ti, const RawHandle & handle, void * data);
    
    template<typename T>
    void set_unmanaged(const Handle<T> & handle, T * data){
        set_unmanaged(typeid(T), handle, data);
    }

    /** \brief Install a get callback
     * 
     * The callback is called whenever 'get' is called on an invalid member; after the call,
     * the member state is set to 'valid', i.e. the callback is not called again
     * until the member is marked as 'invalid'.
     * 
     * The data member identified by T and name must exist already. Its state will be set to
     * invalid to trigger the callback on the next call to 'get'.
     */
    template<typename T>
    void set_get_callback(const Handle<T> & handle, boost::optional<std::function<void ()>> callback){
        set_get_callback(typeid(T), handle, std::move(callback));
    }
    
    void set_get_callback(const std::type_info & ti, const RawHandle & handle, boost::optional<std::function<void ()>> callback);
    
    /** \brief Set the validity flag of an element
     * 
     * This can be used to manually change the flag from 'valid' to 'invalid' and vice versa.
     */
    template<typename T>
    void set_validity(const Handle<T> & handle, bool valid){
        set_validity(typeid(T), handle, valid);
    }
    
    void set_validity(const std::type_info & ti, const RawHandle & handle, bool valid);
    
    /** \brief Get the current state of an element
     */
    template<typename T>
    state get_state(const Handle<T> & handle) const{
        return get_state(typeid(T), handle);
    }
    
    state get_state(const std::type_info & ti, const RawHandle & handle) const;
    
    /** \brief Set all member data states to invalid
     * 
     * This is usually called just before reading a new event, to mark all previous
     * data (from the previous event) as invalid to catch unwanted re-use of old data.
     */
    void invalidate_all();
    
    ~GenericEvent();
    
private:
    void fail(const std::type_info & ti, const RawHandle & handle, const std::string & msg) const;
    void check(const std::type_info & ti, const RawHandle & handle, const std::string & where) const;
    
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
    // Life-cycle: it is created upon handle creation with data=0, eraser=0, generator=none, valid=false. At this point, it is considered nonexistent
    // 'set' sets data, eraser, valid=true. Now, it is 'valid'.
    // 'set_validity' can be used to set it to 'invalid' or 'valid' now.
    // 'set_get_callback' sets generator
    // destructor calls the eraser on data, if eraser is set.
    struct member_data {
        bool valid;
        void * data; // managed by eraser. Can be 0 if not 'set' yet, but handle exists, or if unmanaged.
        std::unique_ptr<eraser_base> eraser;
        boost::optional<std::function<void ()>> generator;
        
        ~member_data();
        member_data(const member_data &) = delete;
        member_data(member_data &&) = default; // note: default is save although this class manages the memory of 'data', as a move will make 'eraser' null, preventing double delete of data.
        void operator=(const member_data & other) = delete;
        void operator=(member_data && other) = delete;
        
        member_data(): valid(false), data(0) {}
    };
    
    const std::vector<GenericEventStructure::member_info> member_infos;
    mutable std::vector<member_data> member_datas; // note: mutable required for lazy generation of data with const access
};

}
