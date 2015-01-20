#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/common/include/TriggerSelection.h"

using namespace uhh2;

class TestTrigger: public uhh2::AnalysisModule {
public:
    explicit TestTrigger(Context &) {
        trigger_selection.reset(new TriggerSelection("HLT_Mu40_v*"));
    }
    
    virtual bool process(Event & e) override{
        return trigger_selection->passes(e);
    }
    
private:
    std::unique_ptr<Selection> trigger_selection;
};


UHH2_REGISTER_ANALYSIS_MODULE(TestTrigger)

