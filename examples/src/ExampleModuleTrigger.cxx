#include <iostream>
#include <memory>

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/common/include/CleaningModules.h"
#include "UHH2/common/include/ElectronHists.h"
#include "UHH2/examples/include/ExampleSelections.h"
#include "UHH2/examples/include/ExampleHists.h"

using namespace std;
using namespace uhh2;

/** \brief Example for accessing the trigger results of the event
 * 
 * This class is to show how to access trigger results directly. Usually, in analysis,
 * one should use the trigger selection modules in common.
 */
class ExampleModuleTrigger: public AnalysisModule {
public:
    
    explicit ExampleModuleTrigger(Context & ctx);
    virtual bool process(Event & event) override;

private:
    string triggername;
    bool first_event;
    Event::TriggerIndex trigindex;
};


ExampleModuleTrigger::ExampleModuleTrigger(Context & ctx){
    triggername = ctx.get("TriggerName");
}


bool ExampleModuleTrigger::process(Event & event) {
    if(first_event){
        first_event = false;
        trigindex = event.get_trigger_index(triggername);
        cout << "Available triggers: " << endl;
        for(const auto & tname : event.get_current_triggernames()){
            cout << tname  << endl;
        }
    }
    
    // lookup_trigger allows to check whether a trigger is available.
    // The code here is not really that useful, because the default
    // behavior of event.passes_trigger is also to throw an exception in case
    // a trigger does not exist.
    if(!event.lookup_trigger_index(trigindex)){
        throw runtime_error("Trigger with name '" + triggername + "' not available");
    }
    
    bool passed = event.passes_trigger(trigindex);
    
    cout << "event " << event.event;
    
    if(passed){
        cout << " passed ";
    }
    else{
        cout << " did not pass ";
    }
    cout << triggername << endl;
    return true;
}

UHH2_REGISTER_ANALYSIS_MODULE(ExampleModuleTrigger)
