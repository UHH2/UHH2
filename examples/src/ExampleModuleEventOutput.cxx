#include <iostream>
#include <memory>

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"

using namespace std;
using namespace uhh2;

/** \brief Example for creating additional branches in the event output tree
 */
class ExampleModuleEventOutput: public AnalysisModule {
public:
    
    explicit ExampleModuleEventOutput(Context & ctx);
    virtual bool process(Event & event);

private:
    Event::Handle<int> h_myint;
    Event::Handle<vector<Jet>> h_myjets;
    Event::Handle<vector<Particle>> h_mygenjets;

    int myint;
};

ExampleModuleEventOutput::ExampleModuleEventOutput(Context & ctx): myint(0){
    // declare the output. The branchname will be "myjets" and the product will be named "myjets" in the event container as well.
    h_myjets = ctx.declare_event_output<vector<Jet>>("myjets");

    // declare the output "mygenjets", the output will only be stored for datasets with Type="MC"
    h_mygenjets = ctx.declare_event_output<vector<Particle>>("mygenjets");
    
    // declare output where the branchname is different from the name in the event:
    h_myint = ctx.declare_event_output<int>(ctx.get("int_branchname"), "myint");

    // unwanted output can be removed by branchname:
    ctx.undeclare_event_output("slimmedJets");
}


bool ExampleModuleEventOutput::process(Event & event) {
    // set myint to some value, here just a counter of events
    event.set(h_myint, myint);
    ++myint;
    
    // create a vector<jet>, e.g. filter the jets in the event according to some criterion:
    vector<Jet> myjets;
    for(const Jet & jet : *event.jets){
        if(jet.pt() > 30.0 && fabs(jet.eta() < 2.4)){
            myjets.push_back(jet);
        }
    }

    // create a vector of gen jets with some filter criterion:
    vector<Particle> mygenjets;
    if(event.genjets){
        for(const Particle & genjet : *event.genjets){
            if(genjet.pt() > 30.0 && fabs(genjet.eta() < 2.4)){
	        mygenjets.push_back(genjet);
	    }
	}
    }

    // note on std::move: this is not strictly required, but used as an optimization 
    // to avoid copying the whole vector.
    event.set(h_myjets, std::move(myjets));
    event.set(h_mygenjets, std::move(mygenjets));
    return true;
}

UHH2_REGISTER_ANALYSIS_MODULE(ExampleModuleEventOutput)
