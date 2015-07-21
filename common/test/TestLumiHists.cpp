#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/common/include/LuminosityHists.h"
#include "UHH2/common/include/NSelections.h"
#include "UHH2/common/include/LumiSelection.h"

using namespace uhh2;
using namespace std;

class TestLumiHists: public uhh2::AnalysisModule {
public:
    explicit TestLumiHists(Context & ctx) {
        lumihists.reset(new LuminosityHists(ctx, "lumi"));
	lumi_selection.reset(new LumiSelection(ctx));
    }
    
    virtual bool process(Event & e) override{
      if(lumi_selection->passes(e)){
        lumihists->fill(e);
	return true;
      }
      return false;
    }
    
private:
    std::unique_ptr<Hists> lumihists;
    std::unique_ptr<Selection> lumi_selection;
};


UHH2_REGISTER_ANALYSIS_MODULE(TestLumiHists)

