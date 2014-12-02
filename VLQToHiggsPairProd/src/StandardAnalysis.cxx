#include <iostream>
#include <memory>

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/common/include/CleaningModules.h"
#include "UHH2/VLQToHiggsPairProd/include/StandardHists.h"
#include "UHH2/VLQToHiggsPairProd/include/StandardSelections.h"
#include "UHH2/VLQToHiggsPairProd/include/GenHists.h"

using namespace std;
using namespace uhh2;

/** \brief Basic analysis example of an AnalysisModule (formerly 'cycle') in UHH2
 * 
 * This is the central class which calls other AnalysisModules, Hists or Selection classes.
 * This AnalysisModule, in turn, is called (via AnalysisModuleRunner) by SFrame.
 */
class StandardAnalysis: public AnalysisModule {
public:
    
    explicit StandardAnalysis(Context & ctx);
    virtual bool process(Event & event);

private:
    
    std::vector<std::unique_ptr<AnalysisModule>> modules;
   
    // declare the Selections to use. Use unique_ptr to ensure automatic call of delete in the destructor,
    // to avoid memory leaks.
//     std::unique_ptr<Selection> njet_sel, bsel;
//     std::unique_ptr<AndSelection> final_selection;
//     
//     // store the Hists collection as member variables. Again, use unique_ptr to avoid memory leaks.
    std::unique_ptr<Hists> h_nocuts;
};


StandardAnalysis::StandardAnalysis(Context & ctx){
    
    // If needed, access the configuration of the module here, e.g.:
    string testvalue = ctx.get("TestKey", "<not set>");
    cout << "TestKey in the configuration was: " << testvalue << endl;
    
    // If running in SFrame, the keys "dataset_version", "dataset_type" and "dataset_lumi"
    // are set to the according values in the xml file. For CMSSW, these are
    // not set automatically, but can be set in the python config file.
    for(auto kv : ctx.get_all()){
        cout << " " << kv.first << " = " << kv.second << endl;
    }
    
    // set up modules, e.g. jet cleaning.
    modules.emplace_back(new JetCleaner(30.0, 2.4));
    
    // set up selections:
//     njet_sel.reset(new NJetSelection(2));
//     bsel.reset(new NBTagSelection(1));
    // use AndSelection to create cutflow of both selections.
    // NOTE: adding selections to AndSelection via add constructs them in-place;
    // the first argument to add is a description (to be used as name of the selection
    // in tables or histograms), followed by the constructor arguments of that Selection
    // class.
//     final_selection.reset(new AndSelection(ctx, "final"));
//     final_selection->add<NJetSelection>("n_jets >= 2", 2);
//     final_selection->add<NBTagSelection>("nb >= 1", 1);

    // Set up Hists classes:
    h_nocuts.reset(new GenHists(ctx, "NoCuts"));
//     h_njet.reset(new StandardHists(ctx, "Njet"));
//     h_bsel.reset(new StandardHists(ctx, "Bsel"));
}


bool StandardAnalysis::process(Event & event) {
    // This is the main procedure, called for each event. Typically, do some pre-processing,
    // such as filtering objects (applying jet pt cuts, lepton selections, etc.),
    // then test which selection(s) the event passes and fill according histograms.
    
    cout << "StandardAnalysis: Starting to process event (runid, eventid) = (" << event.run << ", " << event.event << ")" << endl;
    
    // run all modules (here: only jet cleaning).
//     for(auto & m: modules){
//         m->process(event);
//     }
//     
    h_nocuts->fill(event);
//     
//     bool njet_selection = njet_sel->passes(event);
//     if(njet_selection){
//         h_njet->fill(event);
//     }
//     bool bjet_selection = bsel->passes(event);
//     if(bjet_selection){
//         h_bsel->fill(event);
//     }
    return false;
}

// as we want to run the ExampleCycleNew directly with AnalysisModuleRunner,
// make sure the StandardAnalysis is found by class name. This is ensured by this macro:
UHH2_REGISTER_ANALYSIS_MODULE(StandardAnalysis)
