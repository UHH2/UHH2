#include <iostream>
#include <memory>

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/common/include/CommonModules.h"
#include "UHH2/common/include/CleaningModules.h"
#include "UHH2/common/include/ElectronHists.h"
#include "UHH2/common/include/NSelections.h"
#include "UHH2/common/include/YearRunSwitchers.h"
#include "UHH2/examples/include/ExampleSelections.h"
#include "UHH2/examples/include/ExampleHists.h"

using namespace std;
using namespace uhh2;

namespace uhh2examples {


/**
 * Dummy module just to print some text
 */
class DummyModule: public AnalysisModule {
public:
    explicit DummyModule(const std::string & note):
    note_(note)
    {}
    
    virtual bool process(Event & event){
        cout << note_ << endl;
        return true;
    }
private:
    std::string note_;
};


/** \brief Example of how to handle setting up modules for different years, run periods
 * 
 */
class ExampleModuleYearRunSwitch: public AnalysisModule {
public:
    
    explicit ExampleModuleYearRunSwitch(Context & ctx);
    virtual bool process(Event & event) override;

private:
    
    std::unique_ptr<YearSwitcher> dummyTextAll, jetCleanerAll;

};


ExampleModuleYearRunSwitch::ExampleModuleYearRunSwitch(Context & ctx){
    cout << "Hello World from ExampleModuleYearRunSwitch!" << endl;
    
    // Setup and instance of DummyModule module to be run for each year
    // Note that e.g. the 2016 one will be run for both 2016v2 & 2016v3 ntuples
    dummyTextAll.reset(new YearSwitcher()); 
    dummyTextAll->setup2016(new DummyModule("I am 2016"));
    dummyTextAll->setup2017(new DummyModule("I am 2017"));
    dummyTextAll->setup2018(new DummyModule("I am 2018"));
    
    // Here we specify a particular module for 2017v2 ntuples. This will be run
    // instead of the one created in setup2017() above.
    // 2017v1 ntuples will still use that one though.
    dummyTextAll->setup2017v2(new DummyModule("I am 2017v2"));

    
    // Here is a more realistic example - only for 2017, 
    // we decide to exclude HF jets and raise the pT cut as well
    jetCleanerAll.reset(new YearSwitcher()); 
    jetCleanerAll->setup2016(new JetCleaner(ctx, 30.0, 5.));
    jetCleanerAll->setup2017(new JetCleaner(ctx, 50.0, 3));
    jetCleanerAll->setup2018(new JetCleaner(ctx, 30.0, 5));
    
}


bool ExampleModuleYearRunSwitch::process(Event & event) {
    
    cout << "ExampleModuleYearRunSwitch: Starting to process event (runid, eventid) = (" << event.run << ", " << event.event << "); weight = " << event.weight << endl;
    
    // the YearSwitcher uses the event.year stored in the ntuple, 
    // ignoring whatver is specified in the dataset Version
    dummyTextAll->process(event);
    jetCleanerAll->process(event);

    return true;
}

// as we want to run the ExampleCycleNew directly with AnalysisModuleRunner,
// make sure the ExampleModuleYearRunSwitch is found by class name. This is ensured by this macro:
UHH2_REGISTER_ANALYSIS_MODULE(ExampleModuleYearRunSwitch)

}
