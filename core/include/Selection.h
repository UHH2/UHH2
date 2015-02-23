#pragma once

#include "UHH2/core/include/fwd.h"
#include <string>
#include <memory>
#include <vector>

namespace uhh2 {

/** \brief Base class for event selections
 *
 * To implement your selection, derive from this class and implement the \c passes method.
 * To use an existing selection, create the derived class in the AnalysisModule constructor
 * and call the \c passes method in the AnalysisModule's 'process' method to test whether the
 * event passes the selection.
 */
class Selection {
public:
    /// return whether or not the event passes this selection.
    virtual bool passes(const Event & event) = 0;
    virtual ~Selection();
};


/** \brief Apply an logical AND of a given list of Selections, and fill a cutflow histogram
 * 
 * To use this class, construct it in your AnalysisModule's constructor, passing the Context (which is needed
 * to save the output histogram). In the AnalysisModule::process class, call the 'passes' method as for any other \c Selection.
 * 
 * To add a Selection module, say \c NJetSelection, use the template add method:
 * \code
 * AndSelection sel;
 * sel.add<NJetSelection>("njet >= 1", 1, -1);
 * \endcode
 */
class AndSelection: public Selection {
public:
    /// if cutflow_histo_name is empty, no histogram is created
    explicit AndSelection(Context & ctx, const std::string & cutflow_histo_name = "");
    
    /// add the selection module. The description is used for the cutflow histogram as x-axis label and in the summary printed at the end of the SFrame job.
    template<typename TSEL, typename... CARGS>
    void add(const std::string & description, CARGS... cargs){
        // note: writing the add method this way instead of taking a pointer to Selection directly circumvents
        // the issues with memory ownership nicely.
        selections.emplace_back(new TSEL(cargs...));
        descriptions.push_back(description);
    }

    /// add an already defined selection -> WARNING: if this is an AndSelection producing cutflow histograms, bear in mind that this might
    /// be filled twice in the event!
    void add(const std::string & description, std::shared_ptr<Selection> sel){
        selections.push_back(sel);
        descriptions.push_back(description);   
    }
    
    virtual ~AndSelection();
    
    virtual bool passes(const Event & event) override;
    
private:
    void create_histos();
    
    Context & ctx;
    
    std::string cutflow_hname;
    
    std::vector<std::shared_ptr<Selection> > selections;
    std::vector<std::string> descriptions;
    
    TH1D * cutflow_raw, * cutflow_weighted; // owned by Context
};

}
