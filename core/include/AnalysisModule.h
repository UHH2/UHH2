#pragma once

#include "UHH2/core/include/fwd.h"
#include "UHH2/core/include/obj.h"
#include "UHH2/core/include/registry.h"
#include "UHH2/core/include/GenericEvent.h"

#include <string>
#include <map>
#include <type_traits>
#include <typeinfo>
#include <cassert>

namespace uhh2 {

/**  \brief Abstract base class for all analysis modules, independent of SFrame
 * 
 * To implement an analysis, derive from this class and override the 'process' method, which
 * will be called for each event.
 *
 * Derived classes of AnalysisModules can be either constructed by other AnalysisModules or
 * by the framework in which case the C++ class name is specified in the xml configuration file.
 * In the latter use-case, the AnalysisModule is called a 'top-level' module. In order for
 * a class to be used this way, it must:
 *   - have a constructor using 'Context &' as only argument and
 *   - be registered via the UHH2_REGISTER_ANALYSIS_MODULE macro
 *
 * How the constructors for other AnalysisModules (not run via the framework) is up to the author of
 * the respective class. For many classes, it makes sense to also use a 'Context &' as constructor argument,
 * in order to be able to access event content via handles or to save histograms.
 *
 * An AnalysisModules' lifetime is limited to the processing of one dataset.
 * If multiple datasets are processed 'in one go', the framework takes care of destroying and re-constructing all
 * AnalysisModules at the transition from one to the next dataset.
 */
class AnalysisModule {
public:
    /** \brief Method called for each event in the dataset
     *
     * Do the filling of histograms, calculating new variables, etc. in this method;
     * see examples for more explanations.
     *
     * The return value indicates whether or not the event should be kept for the output. Whether the return
     * value is actually interpreted this way is up to the caller, however; for 'top-level' analysis modules
     * (which are configured by the xml file and called by the framework), this will be the case, but if called from
     * other AnalysisModules', it depends on the conventions and implementation followed there.
     */
    virtual bool process(Event & event) = 0;

    virtual ~AnalysisModule();
};



/** \brief Utility class for configuration and I/O, passed to AnalysisModule constructors
 *
 * Configuration:
 * The configuration of a module is represented as key/value pairs of strings, to pass small (configuration) data to
 * the modules. A number of dataset-related keys are always present when running via SFrame:
 *  - "dataset_type" is the type as given in the SFrame xml file, should be either "MC" or "DATA"
 *  - "dataset_version" is the 'Version' of the dataset as given in the SFrame xml file
 *  - "dataset_lumi" is the integrated luminosity ('Lumi') of the dataset in pb^-1, as given in the SFrame xml file
 *  - "target_lumi" is the target luminosity in pb^-1 for MC reweighting, as specified in the SFrame xml in Cycle in 'TargetLumi'
 * Other keys can either be specified in the SFrame xml configuration file (see ExampleModule), or set
 * by other classes, in particular by the top-level AnalysisModule before constructing other AnalysisModules.
 * 
 * When running in CMSSW, none of the above keys is set by the framework, but all of them can be set explicitly
 * as cms.string in the 'AnalysisModule' PSet in the EDFilter configuration of NtuplerWriter.
 *
 * Histograms:
 * For writing a histogram, use the \c put method.
 *
 * Event TTree:
 * The event tree is handled by the framework, which means that reading entries and writing entries is done
 * automatically. Also most setup (reading in variables) is usually done centrally (in AnalysisModuleRunner).
 * To define event input or output beyond the standard set of variables, use
 * \c declare_event_output and \c declare_event_input.
 */
class Context {
public:

    /// declare the destructor virtual, as this is a purely virtual base class.
    virtual ~Context();

    Context(const Context &) = delete;
    Context(Context &&) = default;

    /** \brief Get the value of a key
     *
     * Throws a runtime_error if the setting with this key is not available (iff !has(key)).
     */
    std::string get(const std::string & key) const {
        std::map<std::string, std::string>::const_iterator it = settings.find(key);
        if(it==settings.end()) fail(key);
        return it->second;
    }

    /** \brief Get the value of a key, returning a default value if the key does not exists
     */
    std::string get(const std::string & key, const std::string & def) const {
        std::map<std::string, std::string>::const_iterator it = settings.find(key);
        if(it==settings.end()) return def;
        else return it->second;
    }

    /** \brief Get all key/value pairs as a map
     */
    std::map<std::string, std::string> get_all() const {
        return settings;
    }

    /** \brief Test whether a key is available
     */
    bool has(const std::string & key) const {
        return settings.find(key) != settings.end();
    }

    /** \brief Set a key to a value
     *
     * Set a configuration value, which can be retrieved later with \c get
     * The \c key will be created if it does not exist. If it exists, the value will be overwritten.
     */
    void set(const std::string & key, const std::string & value) {
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


    /** \brief Get a handle by type and name, allowing to set / get event contents
     *
     * This should be called during setup only (e.g. in the AnalysisModule constructor). The returned
     * handle can be used later with the corresponding Event to get or set data. Using the same
     * name and type for a handle will refer to the same data; so using the same name and type across
     * AnalysisModules allows data exchange while avoiding explicit code dependency.
     */
    template<typename T>
    GenericEvent::Handle<T> get_handle(const std::string & name) {
        return ges.get_handle<T>(name);
    }
    
    
    /// resolution order for metadata sources
    enum class metadata_source_policy {
        infile_only, handle_only, infile_then_handle, handle_then_infile
    };
    
    /** \brief register a callback to be called when new metadata is available
     *
     * metadata announced via the registered callback should be considered valid for all event
     * processed in the future until a new value is announced via the callback.
     *
     * metadata can have two different sources:
     * 1. another AnalysisModule via create_metadata and MetaDataHandle::set.
     * 2. the input file
     *
     * The source of the value announced via the callback depends on the value of resolution. The default
     * uses the value from the handle if available and uses the value from the input file otherwise. This
     * is what one often wants as this typically is the 'most up-to-date' value for that metadata. It is also
     * the policy used to write output.
     *
     * If metadata for that name is available, but the type T does not match, the framework aborts execution
     * with an exception.
     */
    template<typename T>
    void register_metadata_callback(const std::string & name, const std::function<void (const T & t)> & callback, metadata_source_policy pol = metadata_source_policy::handle_then_infile){
        register_metadata_callback(typeid(T), name, [callback](const void * ptr){ callback(*reinterpret_cast<const T*>(ptr)); }, pol, false);
    }

    template<typename T>
    class MetadataObject {
    public:
        
        void operator=(const T & new_metadata){
            assert(object && ctx);
            ctx->notify_callbacks_before(object->address());
            object->assign<T>(new_metadata);
            ctx->notify_callbacks_after(object->address());
        }
        
        MetadataObject() = default;

    private:
        friend class Context;
        MetadataObject(const std::shared_ptr<obj> & object_, Context * ctx_): object(object_), ctx(ctx_) {}

        std::shared_ptr<obj> object;
        Context * ctx = nullptr;
    };

    /** \brief Create new metadata of type T and the given name
     *
     * Metadata can be thought of event-data which is valid for more than one event in
     * a row in the event processing. A typical example would be sample cross section/luminosity or
     * information about the processing history. Another example are the trigger names which are used to give names
     * to the indices in the trigger bool-vector.
     * To exploit the fact that these data are the same for many events in a row during the processing of one dataset,
     * this kind of data is not saved in the Event directly, but instead transferred via the methods
     * declared below: create_metadata<T> creates new metadata of type T with a given name. Via the returned
     * object, the metadata value can be set. This metadata should be considered valid for the current
     * event and all subsequent events, until a new assignment is made (note that metadata validity never
     * extends across datasets because all data is cleared between datasets anyway).
     * 
     * store_in_output controls whether or not the value is saved in the output file.
     *
     * It is important to assign a value to the metadata object *before* any (other) module uses the (old, outdated) value
     * from the previous event. It is therefore recommended that producers of metadata are kept in some few, specific
     * AnalysisModules and are run first for each event. Also, each consumer should
     * throw an exception if the first metadata value is announced *after* the first event has been processed;
     * this should catch most use-before-set bugs.
     *
     * If metadata of that name was already created, a runtime_error is thrown. Note that it is not
     * considered as error creating metadata which also exists in the input, as long as it is the same type;
     * in this case, see register_metadata_callback about which data is actually used.
     * 
     * Metadata output:
     *  - for metadata created here only, \c store_in_output controls whether the metadata object created here is written to output
     *  - Metadata only available in the input file is also written to the output.
     *  - For Metadata which is both available in the input and created via \c create_metadata, the behavior is the same as
     *    if it was only created here and not available in the input.
     * 
     * In particular, this means that:
     *  - all metadata will be written to output by default, even if no module explicitly asks for it
     *  - it is possible to 'erase' metadata from the input (=not write it to output) by creating it here with \c store_in_output=false
     */
    template<typename T>
    MetadataObject<T> create_metadata(const std::string & name, bool store_in_output){
        auto object = obj::create<T>();
        put_metadata(name, false, object, store_in_output);
        return MetadataObject<T>(object, this);
    }

protected:

    explicit Context(GenericEventStructure & ges_);

    GenericEventStructure & ges;
    
    // should be called by derived classes to get all metadata to write to output after processing at least one event.
    void visit_metadata_for_output(const std::function<void (const std::string & name, const std::shared_ptr<obj> & object)> & callback);
    
    // type-erased metadata handling:
    void register_metadata_callback(const std::type_info & ti, const std::string & name, const std::function<void (const void * ptr)> & callback,
                                    metadata_source_policy pol, bool call_before_change);
    
    void put_metadata(const std::string & name, bool from_infile, const std::shared_ptr<obj> & object, bool store_in_output);
    
    void notify_callbacks_before(void * object_addr);
    void notify_callbacks_after(void * object_addr);

private:

    template<typename T>
    friend class MetadataObject;

    virtual void do_declare_event_input(const std::type_info & ti, const std::string & bname, const std::string & mname) = 0;
    virtual void do_declare_event_output(const std::type_info & ti, const std::string & bname, const std::string & mname) = 0;
    
    std::map<std::string, std::string> settings;
    
    // below here: derived classes do not need to care
    void fail(const std::string & key) const;
    
    class MetadataImpl;
    std::unique_ptr<MetadataImpl> meta;
};


template<typename T>
GenericEvent::Handle<T> Context::declare_event_input(const std::string & bname, const std::string & ename_) {
    static_assert(!std::is_pointer<T>::value, "T must not be of pointer type");
    const auto & ename = ename_.empty() ? bname : ename_;
    do_declare_event_input(typeid(T), bname, ename);
    return ges.get_handle<T>(ename);
}

template<typename T>
GenericEvent::Handle<T> Context::declare_event_output(const std::string & bname, const std::string & ename_) {
    static_assert(!std::is_pointer<T>::value, "T must not be of pointer type");
    const auto & ename = ename_.empty() ? bname : ename_;
    do_declare_event_output(typeid(T), bname, ename);
    return ges.get_handle<T>(ename);
}

typedef Registry<AnalysisModule, Context &> AnalysisModuleRegistry;
#define UHH2_REGISTER_ANALYSIS_MODULE(T) namespace { int dummy##T = ::uhh2::AnalysisModuleRegistry::register_<T>(#T); }

}
