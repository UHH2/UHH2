#include <iostream>
#include <memory>

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/common/include/CommonModules.h"
#include "UHH2/common/include/CleaningModules.h"
#include "UHH2/common/include/ElectronHists.h"
#include "UHH2/common/include/NSelections.h"
#include "UHH2/examples/include/ExampleSelections.h"
#include "UHH2/examples/include/ExampleHists.h"

using namespace std;
using namespace uhh2;

namespace uhh2examples {

/** \brief Basic analysis example of an AnalysisModule (formerly 'cycle') in UHH2
 *  
 * This is the central class which calls other AnalysisModules, Hists or Selection classes.
 * This AnalysisModule, in turn, is called (via AnalysisModuleRunner) by SFrame.
 */
class ExampleJetComposition: public AnalysisModule {
public:
    
    explicit ExampleJetComposition(Context & ctx);
    virtual bool process(Event & event) override;

private:
    
  //    std::unique_ptr<CommonModules> common;
    
    std::unique_ptr<JetCleaner> jetcleaner;
   
    // declare the Selections to use. Use unique_ptr to ensure automatic call of delete in the destructor,
    // to avoid memory leaks.
    std::unique_ptr<Selection> njet_sel, dijet_sel;
    
    // store the Hists collection as member variables. Again, use unique_ptr to avoid memory leaks.
  std::unique_ptr<Hists> h_nocuts, h_njet, h_lowPU, h_mediumPU,h_highPU;//, h_dijet, h_ele;
};


ExampleJetComposition::ExampleJetComposition(Context & ctx){
    // In the constructor, the typical tasks are to initialize the
    // member variables, in particular the AnalysisModules such as
    // CommonModules or some cleaner module, Selections and Hists.
    // But you can do more and e.g. access the configuration, as shown below.
    
    cout << "Hello World from ExampleJetComposition!" << endl;
    
    // If needed, access the configuration of the module here, e.g.:
    string testvalue = ctx.get("TestKey", "<not set>");
    cout << "TestKey in the configuration was: " << testvalue << endl;
    
    // If running in SFrame, the keys "dataset_version", "dataset_type", "dataset_lumi",
    // and "target_lumi" are set to the according values in the xml file. For CMSSW, these are
    // not set automatically, but can be set in the python config file.
    for(auto & kv : ctx.get_all()){
        cout << " " << kv.first << " = " << kv.second << endl;
    }
    
    // 1. setup other modules. CommonModules and the JetCleaner:
    //    common.reset(new CommonModules());
    // TODO: configure common here, e.g. by 
    // calling common->set_*_id or common->disable_*
    //    common->init(ctx);
    jetcleaner.reset(new JetCleaner(ctx, 190.0, 5.2)); 
    
    // note that the JetCleaner is only kept for the sake of example;
    // instead of constructing a jetcleaner explicitly,
    // the cleaning can also be achieved with less code via CommonModules with:
    // common->set_jet_id(PtEtaCut(30.0, 2.4));
    // before the 'common->init(ctx)' line.
    
    // 2. set up selections
    njet_sel.reset(new NJetSelection(1)); // see common/include/NSelections.h
    // dijet_sel.reset(new DijetSelection()); // see ExampleSelections

    // 3. Set up Hists classes:
    h_nocuts.reset(new ExampleHists(ctx, "NoCuts"));
    h_njet.reset(new ExampleHists(ctx, "Njet"));
    h_lowPU.reset(new ExampleHists(ctx, "h_lowPU"));
    h_highPU.reset(new ExampleHists(ctx, "h_highPU"));
    h_mediumPU.reset(new ExampleHists(ctx, "h_mediumPU"));

    // h_dijet.reset(new ExampleHists(ctx, "Dijet"));
    // h_ele.reset(new ElectronHists(ctx, "ele_nocuts"));
}


bool ExampleJetComposition::process(Event & event) {
    // This is the main procedure, called for each event. Typically,
    // do some pre-processing by calling the modules' process method
    // of the modules constructed in the constructor (1).
    // Then, test whether the event passes some selection and -- if yes --
    // use it to fill the histograms (2).
    // Finally, decide whether or not to keep the event in the output (3);
    // this is controlled by the return value of this method: If it
    // returns true, the event is kept; if it returns false, the event
    // is thrown away.
    
  //    cout << "ExampleJetComposition: Starting to process event (runid, eventid) = (" << event.run << ", " << event.event << "); weight = " << event.weight << endl;
    
    // 1. run all modules other modules.
    //    common->process(event);
    jetcleaner->process(event);
    
    // 2. test selections and fill histograms
    //  h_ele->fill(event);
    h_nocuts->fill(event);
    
    bool njet_selection = njet_sel->passes(event);
    if(!njet_selection) return false;
    h_njet->fill(event);
    int nPU = 0;
    if(!event.isRealData)  nPU =event.genInfo->pileup_TrueNumInteractions();
    if(nPU<10)
      h_lowPU->fill(event);
    if(nPU>10 && nPU<30)
      h_mediumPU->fill(event);
    if(nPU>30)
      h_highPU->fill(event);

    
    return true;
}

// as we want to run the ExampleCycleNew directly with AnalysisModuleRunner,
// make sure the ExampleJetComposition is found by class name. This is ensured by this macro:
UHH2_REGISTER_ANALYSIS_MODULE(ExampleJetComposition)

}
