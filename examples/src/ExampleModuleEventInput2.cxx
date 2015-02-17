#include <iostream>
#include <memory>

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"

using namespace std;
using namespace uhh2;

/** \brief Example for reading and writing additional branches
 * 
 * This time -- as opposed to ExampleModuleEventInput -- do the setup of additional
 * branches in the xml file.
 * 
 * Note that this example only explicitly mentions 'myjets', but the output will still
 * have the 'testbranchname' branch of type int, if this is given in the xml file
 * in the list of additionalBranches.
 */
class ExampleModuleEventInput2: public AnalysisModule {
public:
    
    explicit ExampleModuleEventInput2(Context &);
    virtual bool process(Event & event);

private:
    Event::Handle<vector<Jet>> h_myjets;
};

ExampleModuleEventInput2::ExampleModuleEventInput2(Context & ctx) {
    // declare input in analogy to output (see ExampleModuleEventOutput).
    h_myjets = ctx.get_handle<vector<Jet>>("myjets");
}


bool ExampleModuleEventInput2::process(Event & event) {
    const vector<Jet> & myjets = event.get(h_myjets); // note: using a reference avoids copying the jet vector.
    cout << "Njets = " << myjets.size() << endl;
    
    return true;
}

UHH2_REGISTER_ANALYSIS_MODULE(ExampleModuleEventInput2)
