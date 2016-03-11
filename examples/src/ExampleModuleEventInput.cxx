#include <iostream>
#include <memory>

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"

using namespace std;
using namespace uhh2;

/** \brief Example for reading additional branches in the event input tree
 * 
 * This works on files created with ExampleModuleEventOutput.
 */
class ExampleModuleEventInput: public AnalysisModule {
public:
    
    explicit ExampleModuleEventInput(Context & ctx);
    virtual bool process(Event & event);

private:
    Event::Handle<int> h_myint;
    Event::Handle<vector<Jet>> h_myjets;
    Event::Handle<vector<Particle>> h_mygenjets;
};

ExampleModuleEventInput::ExampleModuleEventInput(Context & ctx) {
    // declare input in analogy to output (see ExampleModuleEventOutput).
    h_myjets = ctx.declare_event_input<vector<Jet>>("myjets");
    h_mygenjets = ctx.declare_event_input<vector<Particle>>("mygenjets");
    h_myint = ctx.declare_event_input<int>(ctx.get("int_branchname"), "myint");
}


bool ExampleModuleEventInput::process(Event & event) {
    // dump the read values:
    cout << "myint = " << event.get(h_myint) << endl;
    
    const vector<Jet> & myjets = event.get(h_myjets); // note: using a reference avoids copying the jet vector.
    cout << "Njets = " << myjets.size() << endl;
    
    const vector<Particle> & mygenjets = event.get(h_mygenjets); 
    cout << "Ngenjets = " << mygenjets.size() << endl;

    return true;
}

UHH2_REGISTER_ANALYSIS_MODULE(ExampleModuleEventInput)
