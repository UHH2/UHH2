#include <iostream>
#include <memory>

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/common/include/GenJetsHists.h"

using namespace std;
using namespace uhh2;

namespace uhh2examples {

/** \brief Basic analysis example of module running on GEN-only ntuples
 * that only have GenInfo and genjets
 * (made with core/python/ntuplewriter_mc_2016v3_GEN_ONLY.py)
 */
class ExampleModuleGenOnly: public AnalysisModule {
public:

    explicit ExampleModuleGenOnly(Context & ctx);
    virtual bool process(Event & event) override;

private:
    std::unique_ptr<Hists> genjetHists;
};


ExampleModuleGenOnly::ExampleModuleGenOnly(Context & ctx){
    cout << "Hello World from ExampleModuleGenOnlyGenOnly!" << endl;
    genjetHists.reset(new GenJetsHists(ctx, "genjets"));
}


bool ExampleModuleGenOnly::process(Event & event) {
    cout << "ExampleModuleGenOnly: Starting to process event (runid, eventid) = (" << event.run << ", " << event.event << "); weight = " << event.weight << endl;

    for (const auto gj : *event.genjets) {
        cout << gj.pt() << " : " << gj.eta() << " : " << gj.phi() << endl;
    }
    genjetHists->fill(event);
    return true;
}

UHH2_REGISTER_ANALYSIS_MODULE(ExampleModuleGenOnly)

}
