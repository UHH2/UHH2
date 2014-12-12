#include "UHH2/core/include/obj.h"

#include <vector>
#include <memory>
#include <string>
#include <vector>
#include <list>
#include <cassert>

namespace uhh2 {
    
    
/** \brief A pool of named objects which allowed to track changes by callbacks
 * 
 * A pool of type-erased objects \c obj which allows to
 * communicate assignments to these objects to others by callbacks. This class thus
 * achieves a de-coupling from code changing objects and code that needs to act on such changes.
 * While the class is kept rather general, its main purpose is to aid with
 * the implementation of metadata.
 * 
 * The basic blocks for storing objects is based on the \c obj class explained elsewhere.
 * Because \c obj is neither copiable nor movable, the interface is based on shared_ptr's of \c obj.
 * 
 * A typical use looks like this one the 'writer' side:
 * \code
 *  WatchingObjects wo;
 *  shared_ptr<obj> my_object = obj::create<int>(0);
 *  ...
 *  auto handle = wo.put("my_object", my_object);
 *  wo.assign<int>(handle, 5); // triggers callbacks, see below
 * \endcode
 * 
 * On the 'watcher' side:
 * \code
 *   wo.watch_after<int>("my_object", [](const int & new_value){ cout << "got new value: " << new_value << endl; });
 * \endcode
 * 
 * Beside \c watch_after, there is also \c watch_before which is called just *before* the value is changed with the old value.
 * 
 * The interface with \c watch_before and \c watch_after might look overly complicated at first, as one could also
 * have most effects by a callback with two parameters: old and new value. However, the pool actually manages
 * objects which only exist once (connected to a TTree's branch) and cannot be copied easily in the general code because
 * of type erasure. Therefore, an interface with only one callback with old and new value is not easily implementable within
 * these constraints.
 */
class WatchingObjects {
public:
    
    struct Handle {
        Handle(){}
    private:
        friend class WatchingObjects;
        explicit Handle(int i): index(i){}
        int index = -1;
    };
    
    /** \brief Add an object to the pool
     * 
     * The name must be unique; if the name is already used, a runtime_error is thrown.
     */
    Handle put(const std::string & name, const std::shared_ptr<obj> & object);
    
    /** \brief Test whether an object of this name already exists */
    bool exists(const std::string & name) const {
        return find(name) != -1;
    }
    
    /** \brief Register a callback ('watcher') which is called after a value has changed.
     * 
     * Registering for a non-existent element is valid; the callback will never be called.
     * Registering before putting the resp. element is valid and behaves the same; in particular,
     * a put does *not* trigger a call to the callback.
     */
    template<typename T>
    void watch_after(const std::string & name, const std::function<void (const T &)> & callback){
        watch_after(typeid(T), name, [callback](const void * ptr){ callback(*reinterpret_cast<const T*>(ptr)); });
    }
    
    void watch_after(const std::type_info & ti, const std::string & name, const std::function<void (const void*)> & callback);
    
    /** \brief Register a callback ('watcher') which is called before a value has changed.
     * 
     * The callback is called before the change is made with the old (unchanged) value.
     */
    template<typename T>
    void watch_before(const std::string & name, const std::function<void (const T &)> & callback){
        watch_before(typeid(T), name, [callback](const void * ptr){ callback(*reinterpret_cast<const T*>(ptr)); });
    }
    
    void watch_before(const std::type_info & ti, const std::string & name, const std::function<void (const void*)> & callback);
    
    /** \brief Assign a new value to an object
     *
     * This also triggers the correct callbacks.
     */
    template<typename T, typename U>
    void assign(const Handle & h, U && value);
    
    /** \brief Manually trigger callbacks
     * 
     * Not needed if using \c assign, but is needed if changing the object directly.
     */
    void notify_callbacks_before(const Handle & h);
    void notify_callbacks_after(const Handle & h);
    
    
    /** \brief Get the type for a certain object in the pool */
    const std::type_info & type(const Handle & h) const{
        if(h.index < 0) fail_handle();
        return objects[h.index].ti;
    }
    
    /** \brief Get the value for a certain object in the pool
     * 
     * T must match the type exactly.
     * 
     * Same as
     * \code
     * object.get<T>()
     * \endcode
     * if \c object is the \c obj used in \c put.
     */
    template<typename T>
    const T & get_value(const Handle & h) const {
        if(h.index < 0) fail_handle();
        return objects[h.index].value->get<T>();
    }
    
    /** \brief Visit all objects in the pool
     *
     * The visitor function is called with name, handle, current value as arguments.
     * Note that only items are visited which have been created via 'create' or 'put'; elements which are only 'watch'ed for
     * are not reported.
     */
    void visit(const std::function<void (const std::string &, const Handle &, const std::shared_ptr<obj> &)> & visitor);

private:
    
    typedef std::function<void (const void*)> callback_type;
    
    struct object_info {
        std::string name;
        std::shared_ptr<obj> value;
        std::list<callback_type> callbacks_before;
        std::list<callback_type> callbacks_after;
        const std::type_info & ti;
        
        object_info(const std::string & name_, const std::type_info & ti_): name(name_), ti(ti_) {}
    };
    
    
    int find(const std::string & name) const;
    void fail_type(const object_info & obj, const std::type_info & type) const;
    void fail_handle() const;
    
    std::vector<object_info> objects;
};

template<typename T, typename U>
void WatchingObjects::assign(const Handle & h, U && value){
    if(h.index < 0) fail_handle();
    auto & oi = objects[h.index];
    if(!oi.value) fail_handle();
    if(oi.value->type() != typeid(T)) fail_type(oi, typeid(T));
    for(const auto & c : oi.callbacks_before){
        c(oi.value->address());
    }
    oi.value->assign<T>(std::forward<U>(value));
    for(const auto & c : oi.callbacks_after){
        c(oi.value->address());
    }
}

}

