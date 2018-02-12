#include <iostream>
#include <memory>

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/common/include/EventHists.h"

using namespace std;
using namespace uhh2;

namespace uhh2examples {

/** \brief Simple analysis to dump pileup hists to ROOT file, for use in MCPileupReweight
 *
 */
class MakePileupHistModule: public AnalysisModule {
public:
    
    explicit MakePileupHistModule(Context & ctx);
    virtual bool process(Event & event) override;

private:
    std::unique_ptr<Hists> h_pu;
};


MakePileupHistModule::MakePileupHistModule(Context & ctx){
    
    h_pu.reset(new EventHists(ctx, "input_Event"));

}


bool MakePileupHistModule::process(Event & event) {
    h_pu->fill(event);
    
    return true;
}

// as we want to run the ExampleCycleNew directly with AnalysisModuleRunner,
// make sure the MakePileupHistModule is found by class name. This is ensured by this macro:
UHH2_REGISTER_ANALYSIS_MODULE(MakePileupHistModule)

}
