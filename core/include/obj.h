#pragma once

#include <memory>

namespace uhh2 {

    
/** \brief Type-erasing container for a single object of a fixed type
 * 
 * The object is never empty and the underlying object has an immutable address; obj itself is not movable,
 * s.t. the address-of-address is also immutable. This is important to interact with ROOT
 * libraries, esp. when using these object wrappers in TTrees which require the address-to-address
 * in SetBranchAddress for composite objects.
 * 
 * As obj is not movable, it is not used as value directly, but only as pointer, usually shared_ptr.
 * To create an instance, use the static 'create' methods which return such a shared_ptr.
 */
// obj is similar to GenericEvent in that it is also type-erasing and serves similar purposes
// for interacting with ROOT TTrees by keeping immutable address-of-address. At least in principle,
// the GenericEvent could probably be re-implemented as container of several obj; this is not done mainly
// for historical reasons; however, keep that in mind when extending / changing obj.
class obj {
public:
    // do not allow copying or moving to have fixed address-of-address (while only saving address directly).
    obj(const obj &) = delete;
    obj(obj &&) = delete;
    obj() = delete;
    
    /** \brief Initialize with raw pointer and deleter
     */
    static std::shared_ptr<obj> create(const std::type_info & ti, void * ptr, std::function<void (void*)> eraser){
        return std::shared_ptr<obj>(new obj(ti, ptr, std::move(eraser)));
    }
    
    /** \brief Initialize via constructor of T
     * 
     * The parameters U are passed-through to the constructor of T.
     */
    template<typename T, typename... U>
    static std::shared_ptr<obj> create(U && ... val){
        auto result = create(typeid(T), nullptr, [](void * ptr){ delete reinterpret_cast<T*>(ptr);});
        result->addr = new T(std::forward<U>(val)...);
        return result;
    }
        
    /** \brief Assign a new value, allocating memory if currently empty.
     * 
     * Note that T must match the type exactly, not just be compatible. Therefore,
     * the method is written to require an explicit type. For example, one can use:
     * 
     * \code
     * obj o(typeid(float));
     * o.assign<float>(5.0); // Ok, although 5.0 is double
     * \endcode
     */
    template<typename T, typename U>
    void assign(U && obj);
    
    /// get the value; will fail if the type does not match exactly
    template<typename T>
    T& get();
    
    /// get the value; will fail if the type does not match exactly
    template<typename T>
    const T& get() const;
    
    /// get the type of this object
    const std::type_info & type() const{
        return type_;
    }
    
    /// get the address at which the object data is stored
    void * address() const{
        return addr;
    }
    
    /// get the address at which the address to the object data is stored
    void * const * address_of_address() const{
        return &addr;
    }
    
    /// get the address at which the address to the object data is stored
    void ** address_of_address() {
        return &addr;
    }
    
    ~obj(){
        if(eraser){
            eraser(addr);
        }
    }
    
private:
    obj(const std::type_info & ti, void * ptr, std::function<void (void*)> eraser);
    
    void fail_type(const std::type_info & ti) const;
    
    const std::type_info & type_;
    void * addr = nullptr;
    std::function<void (void*)> eraser;
};


template<typename T, typename U>
void obj::assign(U && obj){
    if(typeid(T)!=type_) fail_type(typeid(T));
    if(addr == nullptr){
        addr = new T(std::forward<U>(obj));
        eraser = [](void * ptr) {delete reinterpret_cast<T*>(ptr);};
    }
    else{
        *reinterpret_cast<T*>(addr) = std::forward<U>(obj);
    }
}

template<typename T>
T& obj::get(){
    if(typeid(T)!=type_) fail_type(typeid(T));
    return *reinterpret_cast<T*>(addr);
}

template<typename T>
const T& obj::get() const{
    if(typeid(T)!=type_) fail_type(typeid(T));
    return *reinterpret_cast<const T*>(addr);
}

}

