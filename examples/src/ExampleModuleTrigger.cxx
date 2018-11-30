#include <iostream>
#include <memory>

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/common/include/TriggerSelection.h"

using namespace std;
using namespace uhh2;

/** \brief Example for accessing the trigger results of the event
 *
 * This class is to show how to access trigger results both directly,
 * and in a more convenient manner using a TriggerSelection object.
 */
class ExampleModuleTrigger: public AnalysisModule {
public:

    explicit ExampleModuleTrigger(Context & ctx);
    virtual bool process(Event & event) override;

private:
    string triggername, metfiltername;
    bool first_event;
    Event::TriggerIndex trigindex;
    std::unique_ptr<Selection> met_filter_selection;
};


ExampleModuleTrigger::ExampleModuleTrigger(Context & ctx):
    first_event(true)
{
    triggername = ctx.get("TriggerName");
    metfiltername = ctx.get("MetFilterName");
    met_filter_selection.reset(new TriggerSelection(metfiltername));
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

    // lookup_trigger_index allows to check whether a trigger is available.
    // Note that the default behaviour of event.passes_trigger
    // is also to throw an exception in case a trigger does not exist,
    // so normally one should use a TriggerSelection object.
    // However it is useful if you also wish to lookup the trigger prescale,
    // since you need the TriggerIndex
    if(!event.lookup_trigger_index(trigindex)){
        throw runtime_error("Trigger with name '" + triggername + "' not available");
    }

    bool passedTrigger = event.passes_trigger(trigindex);
    string passStr = passedTrigger ? " passed " : " did not pass ";
    int trigPrescale = -1;
    if (event.isRealData) trigPrescale = event.trigger_prescale(trigindex);
    cout << "event " << event.event << passStr << "trigger " << triggername << " with prescale " << trigPrescale << endl;

    // More most analyses, you would use a TriggerSelection object like this:
    bool passedMetFilter = met_filter_selection->passes(event);
    passStr = passedMetFilter ? " passed " : " did not pass ";
    cout << "event " << event.event << passStr << "met filter " << metfiltername << endl;

    return true;
}

UHH2_REGISTER_ANALYSIS_MODULE(ExampleModuleTrigger)
