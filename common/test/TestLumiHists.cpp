#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/common/include/LuminosityHists.h"

using namespace uhh2;
using namespace std;

class TestLumiHists: public uhh2::AnalysisModule {
public:
    explicit TestLumiHists(Context & ctx) {
        lumihists.reset(new LuminosityHists(ctx, "lumi"));
    }
    
    virtual bool process(Event & e) override{
        lumihists->fill(e);
        return true;
    }
    
private:
    std::unique_ptr<Hists> lumihists;
};


UHH2_REGISTER_ANALYSIS_MODULE(TestLumiHists)

