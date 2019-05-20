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
        (void) event;
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

    std::unique_ptr<YearSwitcher> dummyTextYearSwitcher, jetCleanerYearSwitcher, megaYearSwitcher;
    std::unique_ptr<RunSwitcher> dummyTextRun18, dummyTextRun17;

    // Note that these must be shared pointers,
    // since we store them in a YearSwitcher
    std::shared_ptr<RunSwitcher> megaRunSwitcher16, megaRunSwitcher17, megaRunSwitcher18;

};


ExampleModuleYearRunSwitch::ExampleModuleYearRunSwitch(Context & ctx){
    cout << "Hello World from ExampleModuleYearRunSwitch!" << endl;

    // Setup and instance of DummyModule module to be run for each year
    // Note that e.g. the 2016 one will be run for both 2016v2 & 2016v3 ntuples
    dummyTextYearSwitcher.reset(new YearSwitcher(ctx));
    dummyTextYearSwitcher->setup2016(std::make_shared<DummyModule>("I am 2016"));
    dummyTextYearSwitcher->setup2017(std::make_shared<DummyModule>("I am 2017"));
    dummyTextYearSwitcher->setup2018(std::make_shared<DummyModule>("I am 2018"));

    // Here we specify a particular module for 2017v2 ntuples. This will be run
    // instead of the one created in setup2017() above.
    // 2017v1 ntuples will still use that one though.
    dummyTextYearSwitcher->setup2017v2(std::make_shared<DummyModule>("I am 2017v2"));

    // Here is a more realistic example - we apply some jet cuts,
    // but only for 2017 we decide to exclude HF jets and raise the pT cut as well
    jetCleanerYearSwitcher.reset(new YearSwitcher(ctx));
    jetCleanerYearSwitcher->setup2016(std::make_shared<JetCleaner>(ctx, 30.0, 5.));
    jetCleanerYearSwitcher->setup2017(std::make_shared<JetCleaner>(ctx, 50.0, 3));
    jetCleanerYearSwitcher->setup2018(std::make_shared<JetCleaner>(ctx, 30.0, 5));


    // Here we setup modules for specific run periods in a given year
    dummyTextRun18.reset(new RunSwitcher(ctx, "2018"));
    dummyTextRun18->setupRun("A", std::make_shared<DummyModule>("2018 Run A"));
    dummyTextRun18->setupRun("B", std::make_shared<DummyModule>("2018 Run B"));
    dummyTextRun18->setupRun("C", std::make_shared<DummyModule>("2018 Run C"));
    dummyTextRun18->setupRun("D", std::make_shared<DummyModule>("2018 Run D"));

    // // much easier way to iterate over all Run periods:
    dummyTextRun17.reset(new RunSwitcher(ctx, "2017"));
    for (const auto & itr : {"B", "C", "D", "E", "F"}) {
        dummyTextRun17->setupRun(itr, std::make_shared<DummyModule>("2017 Run " + std::string(itr)));
    }

    // Finally, we can also combine these both
    // Good for e.g. JECs
    megaRunSwitcher16.reset(new RunSwitcher(ctx, "2016"));
    for (const auto & runItr : runPeriods2016) { // runPeriods defined in common/include/Utils.h
        megaRunSwitcher16->setupRun(runItr, std::make_shared<DummyModule>("Super! 2016 + Run " + runItr));
    }

    megaRunSwitcher17.reset(new RunSwitcher(ctx, "2017"));
    for (const auto & runItr : runPeriods2017) {
        megaRunSwitcher17->setupRun(runItr, std::make_shared<DummyModule>("Mega! 2017 + Run " + runItr));
    }

    megaRunSwitcher18.reset(new RunSwitcher(ctx, "2018"));
    for (const auto & runItr : runPeriods2018) {
        megaRunSwitcher18->setupRun(runItr, std::make_shared<DummyModule>("Ausgezeichnet! 2018 + Run " + runItr));
    }

    megaYearSwitcher.reset(new YearSwitcher(ctx));
    megaYearSwitcher->setup2016(megaRunSwitcher16);
    megaYearSwitcher->setup2017(megaRunSwitcher17);
    megaYearSwitcher->setup2018(megaRunSwitcher18);

}


bool ExampleModuleYearRunSwitch::process(Event & event) {

    cout << "ExampleModuleYearRunSwitch: Starting to process event (runid, eventid) = (" << event.run << ", " << event.event << "); weight = " << event.weight << endl;

    dummyTextYearSwitcher->process(event);
    jetCleanerYearSwitcher->process(event);

    // note that for 2016, these are just ignored and all events return true
    dummyTextRun17->process(event);
    dummyTextRun18->process(event);

    megaYearSwitcher->process(event);

    return true;
}

// as we want to run the ExampleCycleNew directly with AnalysisModuleRunner,
// make sure the ExampleModuleYearRunSwitch is found by class name. This is ensured by this macro:
UHH2_REGISTER_ANALYSIS_MODULE(ExampleModuleYearRunSwitch)

}
