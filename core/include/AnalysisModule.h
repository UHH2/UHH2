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

    /**
     * \brief Method to call after finishing last event in a given InputData block
     *
     * e.g. Printing statistics, finalizing a histogram contents
     *
     * Note that AnalysisModules only last for a given InputData block,
     * after which they are destroyed, and a new object created for the next InputData.
     * Hence it doesn't make sense to also have an equivalent "endCycle" at the end of each Cycle.
     */
    virtual void endInputData();

    virtual ~AnalysisModule();
};



/** \brief Utility class for configuration and I/O, passed to AnalysisModule constructors
 *
 * Configuration:
 * The configuration of a module is represented as key/value pairs of strings, to pass small (configuration) data to
 * the modules. A number of dataset-related keys are always present when running via SFrame:
 *  - "dataset_type" is the type as given in the SFrame xml file, should be either "MC" or "DATA"
 *  - "dataset_lumi" is the integrated luminosity ('Lumi') of the dataset in pb^-1, as given in the SFrame xml file
 *  - "dataset_year" is the year of data/MC set as 'Year' parameter in xml file
 *  - "target_lumi" is the target luminosity in pb^-1 for MC reweighting, as specified in the SFrame xml in Cycle in 'TargetLumi'
 *
 * Also, all the dataset-metadata is added with the prefix "meta_" (refer to the Metadata documentation for details).
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

    /** \brief Set sample metadata
     *
     * The metadata will be available via 'get' with the key "meta_" + name. Unlike other
     * settings, it will be written to the output root file, and read in from the root file.
     *
     * A metadata name must not contain the sequence "===" or end with "=". The value must not contain any of "\r\n\0".
     * (This ensures that all metadata can be stored in a simple format name1===value1\nname2====value2\n...
     * in a single string).
     *
     * In general, metadata is sample-wide immutable data, so *changing* a metadata value is something
     * delicate and is generally not recommended. If it has to be done anyway (e.g. because a wrong value
     * has been written which should now be corrected), make sure to do it *very* early in the processing,
     * i.e. before any other AnalysisModule can read the old value. This also ensures that the new value
     * is set before the first event -- and the old(?) metadata -- is written to the output file.
     * As changing a metadata value can be problematic, this will only be done for force = true. Otherwise,
     * an exception is thrown.
     */
    void set_metadata(const std::string & name, const std::string & value, bool force = false);

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

    /** \brief Remove a branch from the output event tree
     */
    void undeclare_event_output(const std::string & branch_name) {
        do_undeclare_event_output(branch_name);
    }

    /** \brief Remove all branches from the output event tree
     */
    void undeclare_all_event_output() {
        do_undeclare_all_event_output();
    }


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

protected:

    explicit Context(GenericEventStructure & ges_);

    GenericEventStructure & ges; // the structure updated by get_handle/declare_event_input/declare_event_output.

    virtual void do_declare_event_input(const std::type_info & ti, const std::string & bname, const std::string & mname) = 0;
    virtual void do_declare_event_output(const std::type_info & ti, const std::string & bname, const std::string & mname) = 0;
    virtual void do_undeclare_event_output(const std::string & branch_name) = 0;
    virtual void do_undeclare_all_event_output() = 0;

    std::map<std::string, std::string> settings;

private:
    void fail(const std::string & key) const;
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
// mark as unused otherwise generates warnings
#define UHH2_REGISTER_ANALYSIS_MODULE(T) namespace { __attribute__ ((unused)) int dummy##T = ::uhh2::AnalysisModuleRegistry::register_<T>(#T); }

}
