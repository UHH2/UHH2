#pragma once

#include "UHH2/core/include/fwd.h"
#include "UHH2/core/include/registry.h"
#include "UHH2/core/include/GenericEvent.h"

#include <string>
#include <map>
#include <type_traits>
#include <typeinfo>

namespace uhh2 {

/**  \brief Abstract base class for all analysis modules, independent of SFrame
 * 
 * Concrete AnalysisModules can be constructed directly by other AnalysisModules. The 'top-level'
 * AnalysisModule, however, is always constructed by the framework, by the C++ class name.
 * For using an AnalysisModule as 'top-level' AnalysisModule, it must
 *   - have a constructor using 'Context &' as argument
 *   - be registered via the REGISTER_ANALYSIS_MODULE macro
 * 
 * How the constructors for other AnalysisModules are used is up to the author of the respective module. For many modules,
 * it makes sense to also use a 'Context &' as constructor argument, in order to be able to save histograms in the output.
 * 
 * An AnalysisModules' lifetime is limited to one dataset. If multiple datasets are processed 'in one go',
 * the modules are destroyed and reconstructed for the new dataset.
 */
class AnalysisModule {
public:
    /** \brief Method called for each event in the dataset
     * 
     * Do the filling of histograms, calculating new variables, etc. in this method;
     * see examples for more explanations.
     * 
     * The return value indicates whether or not the event should be kept for the output for the top-level
     * AnalysisModule.
     */
    virtual bool process(Event & event) = 0;
    
    virtual ~AnalysisModule();
};



/** \brief Utility class for configuration and I/O, passed to AnalysisModule constructors
 * 
 * Configuration:
 * The configuration of a module is represented as key/value pairs of strings, to pass small (configuration) data to
 * the modules. A number of dataset-related keys are always present:
 *  - "dataset_type" is the type as given in the SFrame xml file, should be either "MC" or "DATA"
 *  - "dataset_version" is the version of the dataset as given in the SFrame xml file
 *  - "dataset_lumi" is the integrated luminosity of the dataset in pb^-1, as given in the SFrame xml file
 * Other keys can either be specified in the SFrame xml configuration file (see ExampleModule), or set
 * by other classes, in particular by the top-level AnalysisModule before constructing other AnalysisModules.
 * 
 * Histograms:
 * For writing a histogram, use the \c put method.
 * 
 * Trees:
 * For TTree I/O, it is important to know that there are two types of trees: The (one) event tree and
 * (a number of) user-defined trees.
 * The event tree is handled by the framework, which means that reading entries and writing entries is done
 * automatically. Also most setup (reading in variables) is done centrally, although AnalysisModules can
 * call \c declare_event_output and \c declare_event_input to write/read additional event data.
 * For user-defined trees, both the setup of the tree (creating it and connecting it to variables)
 * as well as writing an entry has to be done through this class with the \c declare_output and the \c write_entry
 * methods. Note that no input is possible for user-defined trees, only output. This limits the use-case of
 * user-defined trees to trees which are read by another software, e.g. for creating training data
 * for MVA analyses.
 */
class Context{
public:
    
    /** \brief Get the value of a key
     * 
     * Throws a runtime_error if the setting with this key is not available (iff !has(key)).
     */
    std::string get(const std::string & key) const{
        std::map<std::string, std::string>::const_iterator it = settings.find(key);
        if(it==settings.end()) fail(key);
        return it->second;
    }
    
    /** \brief Get the value of a key, returning a default value if the key does not exists
     * 
     * This method never throws.
     */
    std::string get(const std::string & key, const std::string & def) const{
        std::map<std::string, std::string>::const_iterator it = settings.find(key);
        if(it==settings.end()) return def;
        else return it->second;
    }
    
    /** \brief Get all key/value pairs as a map
     */
    std::map<std::string, std::string> get_all() const{
        return settings;
    }
    
    /** \brief Test whether a key is available
     */
    bool has(const std::string & key) const{
        return settings.find(key) != settings.end();
    }
    
    /** \brief Set a key to a value
     *
     * Set a configuration value, which can be retrieved later with \c get
     * The \c key will be created if it does not exist. If it exists, the value will be overwritten.
     */
    void set(const std::string & key, const std::string & value){
        settings[key] = value;
    }
        
    /** \brief Put a histogram in the output root file at the specified path
     * 
     * The path is the directories and subdirectories, excluding the histogram name.
     * For example, to create a histogram ending up in the output root file at "selection1/myhisto", use:
     * \code
     * TH1D* histo = new TH1D("myhisto", "myhisto", 100, 0, 1);
     * 
     * //assume 'Context & ctx' is defined here:
     * ctx.put("selection1", histo);
     * \endcode
     * 
     * Leading or trailing slashed in \c path are ignored.
     * 
     * Note that the memory for the Histogram h will be handled by the framework, so do not call
     * 'delete h' for histograms added via this method, and do not add the same histogram twice.
     */
    virtual void put(const std::string & path, TH1 * h) = 0;
    
    /** \brief Declare an input variable in the event tree
     * 
     * T must not be a pointer type. In case event_member_name is empty, it is set to the branch_name.
     */
    template<typename T>
    GenericEvent::Handle<T> declare_event_input(const std::string & branch_name, const std::string & event_member_name = "");
    
    /** \brief Declare an output variable in the event tree
     * 
     * T must not be a pointer type. In case event_member_name is empty, it is set to the branch_name.
     */
    template<typename T>
    GenericEvent::Handle<T> declare_event_output(const std::string & branch_name, const std::string & event_member_name = "");
    
    /** \brief Declare a non-event variable which should be written to the tree identified by tree_id instead.
     * 
     * T must not be of pointer type. Make sure that the lifetime of t exceeds the last call of write_output of the corresponding tree
     */
    /*template<typename T>
    void declare_output(const std::string & treename, const std::string & branchname, T & t);*/
    
    template<typename T>
    GenericEvent::Handle<T> get_handle(const std::string & name){
        return ges.get_handle<T>(name);
    }
    
    /// Write a single entry to the output tree identified by the tree name, using the current contents of the declared output addresses.
    //virtual void write_entry(const std::string & treename) = 0;
    
    /// declare the destructor virtual, as this is a purely virtual base class.
    virtual ~Context();

protected:
    Context(GenericEventStructure & ges_): ges(ges_){}
    
    GenericEventStructure & ges;
    
private:
    
    virtual void do_declare_event_input(const std::type_info & ti, const std::string & bname, const std::string & mname) = 0;
    virtual void do_declare_event_output(const std::type_info & ti, const std::string & bname, const std::string & mname) = 0;
    //virtual void do_declare_output(const std::string & treename, const std::string & branchname, const void * t, const std::type_info & ti) = 0;
    
    void fail(const std::string & key) const;
    
    std::map<std::string, std::string> settings;
};


template<typename T>
GenericEvent::Handle<T> Context::declare_event_input(const std::string & bname, const std::string & ename_){
    static_assert(!std::is_pointer<T>::value, "T must not be of pointer type");
    const auto & ename = ename_.empty() ? bname : ename_;
    do_declare_event_input(typeid(T), bname, ename);
    return ges.get_handle<T>(ename);
}

template<typename T>
GenericEvent::Handle<T> Context::declare_event_output(const std::string & bname, const std::string & ename_){
    static_assert(!std::is_pointer<T>::value, "T must not be of pointer type");
    const auto & ename = ename_.empty() ? bname : ename_;
    do_declare_event_output(typeid(T), bname, ename);
    return ges.get_handle<T>(ename);
}

/*
template<typename T>
void Context::declare_output(const std::string & treename, const std::string & branchname, T & t){
    static_assert(!std::is_pointer<T>::value, "T must not be of pointer type");
    do_declare_output(treename, branchname, static_cast<const void*>(&t), typeid(T));
}
*/

typedef Registry<AnalysisModule, Context &> AnalysisModuleRegistry;
#define UHH2_REGISTER_ANALYSIS_MODULE(T) namespace { int dummy##T = ::uhh2::AnalysisModuleRegistry::register_<T>(#T); }

}
