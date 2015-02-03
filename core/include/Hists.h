#pragma once

#include "UHH2/core/include/fwd.h"
#include "UHH2/core/include/AnalysisModule.h"

#include <string>
#include <type_traits>
#include <stdexcept>
#include <map>

namespace uhh2 {

/** \brief Base class for histogram classes
 *
 * To use it, derive from this class and implement the constructor with the same signature as the one here.
 * In the constructor, book the histograms to produce via the protected book template method.
 *
 * In your AnalysisModule's constructor, instantiate the Hists classes as required, using the output directory
 * as constructor argument. In your AnalysisModule's 'process' method, call 'fill' for all events which
 * you want to fill the histograms for (usually the events passing a certain selection).
 */
class Hists{
public:
    
    Hists(Context & ctx_, const std::string & dirname_);
    
    virtual ~Hists();
    
    // fill the histograms based on the event content; this is called for each (selected) event.
    virtual void fill(const Event & ev) = 0;
    
protected:
    
    /** \brief Create a histogram in the output file
     * 
     * HTYPE should be a root histogram type such as TH1D, TH2D, TH1F, etc. Then, use the method with the
     * same constructor arguments you would use for the constructor of the histogram, e.g.:
     * \code
     * // TH1D * hist1 = new TH1D("hist1_name", "hist1_title", 100, 0, 1)
     * // becomes:
     * TH1D * hist1 = book<TH1D>("hist1_name", "hist1_title", 100, 0, 1);
     * 
     * // new TH2D("hist2d_name", "hist2d_title", 100, 0, 1, 200, 0, 2)
     * // becomes:
     * book<TH2D>("hist2d_name", "hist2d_title", 100, 0, 1, 200, 0, 2);
     * \endcode
     *
     * The histogram will be saved in the output file under the directory name given in the constructor. To keep
     * the convention that all histograms of one class are in the same directory in the output root file,
     * it is not allowed to specify further subdirectories at this point, and a runtime_error is thrown
     * if trying to add a histogram with a '/' in the name. (If you want to add histograms to subdirectories, use the
     * Context::put method directly or create another Hist class with the subdirecory name as constructor argument.)
     * 
     * Histograms created via this methods are handed over to the framework via Context::put, so the
     * lifetime and memory is managed by the framework. So please do not call delete on the returned pointer.
     */
    template<typename HTYPE, typename... TARGS>
    HTYPE* book(const char * name, TARGS... args){
        static_assert(std::is_base_of<TH1, HTYPE>::value, "for book<HTYPE>, HTYPE, must inherit from TH1!");
        std::string sname = name;
        if(sname.find('/')!=std::string::npos){
            throw std::runtime_error(" name '" + sname + "' illegal: '/' in histogram names not allowed in Hists (use Context::put directly for putting histograms in subdirectories)");
        }
        HTYPE * h = new HTYPE(name, std::forward<TARGS>(args)...);
        h->SetName(sname.c_str());
        h->Sumw2();
        ctx.put(dirname, h);
        histos[sname] = h;
        return h;
    }
    
    // utility function allowing to use strings as name and title for histogram construction.
    template<typename HTYPE, typename... TARGS>
    HTYPE* book(const std::string & name, const std::string & title, TARGS... args){
        return book<HTYPE>(name.c_str(), title.c_str(), std::forward<TARGS>(args)...);
    }
    
    
    /** \brief Get a histogram previously booked via 'book'
     * 
     * If a histogram of the given name does not exist, a runtime_error exception is thrown.
     * 
     * Note that calling this method for each event (e.g. from AnalysisModule::process) leads to poor
     * performance, as the histogram is looked up by the string 'name' each time, and string comparisons
     * tend to be CPU-intensive.
     * To gain performance, it is better to save the histogram pointer (returned by book or by hist)
     * in a member variable of your Hists class and thus avoid calling this method for each event.
     */
    TH1* hist(const char * name);
    
private:
    Context & ctx;
    std::string dirname; // without leading/trailing slashes
    std::map<std::string, TH1*> histos;
};

}
