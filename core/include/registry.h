#pragma once

#include <memory>
#include <stdexcept>
#include <map>
#include <type_traits>

namespace uhh2 {

template<typename base_type, typename... ctypes>
class Registry{
public:
    static std::unique_ptr<base_type> build(const std::string & key, ctypes... cargs){
        return instance().ibuild(key, cargs...);
    }
    
    template<typename T>
    static int register_(const std::string & key){
        return instance().template iregister<T>(key);
    }
    
private:
    static Registry & instance(){
        static Registry i;
        return i;
    }
    
    std::unique_ptr<base_type> ibuild(const std::string & key, ctypes... cargs) const;
    
    template<typename T>
    int iregister(const std::string & key);
    
    Registry(){}
    
    struct Factory{
        virtual std::unique_ptr<base_type> operator()(ctypes... cargs) const = 0;
        virtual ~Factory(){}
    };
    
    template<typename T>
    struct FactoryDefault: public Factory{
        virtual std::unique_ptr<base_type> operator()(ctypes... cargs) const override{
            return std::unique_ptr<base_type>(new T(cargs...));
        }
        virtual ~FactoryDefault(){}
    };
    
    std::map<std::string, std::unique_ptr<Factory>> id_to_factory;
};


template<typename base_type, typename... ctypes>
template<typename T>
int Registry<base_type, ctypes...>::iregister(const std::string & key){
    static_assert(std::is_base_of<base_type, T>::value, "T must inherit from base_type"); // If this line leads to a compile error, it means that you tried to register a type through the wrong registry
    // e.g. you tried to register a class T through the AnalysisModuleRegistry (e.g. via REGISTER_ANALYSIS_MODULE(T)), although T does not inherit from AnalysisModule.
    auto it = id_to_factory.find(key);
    if(it != id_to_factory.end()){
        throw std::runtime_error("Registry: tried to register type with same name twice (name: '" + key + "')");
    }
    //std::string keytmp = key;
    id_to_factory.insert(make_pair(key, std::unique_ptr<Factory>(new FactoryDefault<T>())));
    return 0;
}

template<typename base_type, typename... ctypes>
std::unique_ptr<base_type> Registry<base_type, ctypes...>::ibuild(const std::string & key, ctypes... cargs) const {
    auto it = id_to_factory.find(key);
    if(it==id_to_factory.end()) throw std::runtime_error("Registry: did not find registered type of name '" + key + "'");
    try{
        return it->second->operator()(cargs...);
    }
    catch(std::exception & ex){
        throw std::runtime_error("Registry: exception while trying to build type '" + key + "': " + ex.what());
    }
}

}
