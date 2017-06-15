#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/common/include/LuminosityHists.h"
#include "UHH2/common/include/NSelections.h"
#include "UHH2/common/include/LumiSelection.h"
#include "UHH2/common/include/TriggerSelection.h"

using namespace uhh2;
using namespace std;

class TestLumiHists: public uhh2::AnalysisModule {
public:
  explicit TestLumiHists(Context & ctx): metfilter_selection(ctx, "metfilter") {
    lumihists.reset(new LuminosityHists(ctx, "lumi"));
    lumihists_trigger.reset(new LuminosityHists(ctx, "lumi_trigger","",true));
	lumi_selection.reset(new LumiSelection(ctx));
	trigger_selection.reset(new TriggerSelection("HLT_Ele27_eta2p1_WPLoose_Gsf_v*")); 
	//trigger_selection.reset(new TriggerSelection("HLT_PFHT800_v*")); 
	//trigger_selection.reset(new TriggerSelection("HLT_IsoMu24_eta2p1_v*")); 
	//metfilter_selection.add<TriggerSelection>("HBHE", "Flag_HBHENoiseFilter");
	//metfilter_selection.add<TriggerSelection>("CSC", "Flag_CSCTightHaloFilter");
	//metfilter_selection.add<TriggerSelection>("GoodVrt", "Flag_goodVertices");
    }
    
    virtual bool process(Event & e) override{
      if(lumi_selection->passes(e)){
	lumihists->fill(e);

	if(trigger_selection->passes(e)){
	  //uhh2::Event::TriggerIndex HBHE_ind = e.get_trigger_index("Flag_HBHENoiseFilter");
	  //uhh2::Event::TriggerIndex gV_ind = e.get_trigger_index("Flag_goodVertices");
	  //uhh2::Event::TriggerIndex CSC_ind = e.get_trigger_index("Flag_CSCTightHaloFilter");
	  //bool metfilters_valid = /*e.lookup_trigger_index(HBHE_ind) &&*/ e.lookup_trigger_index(CSC_ind) /*&& e.lookup_trigger_index(gV_ind)*/;
	  //if(!metfilters_valid) std::cout << "No MET filter flags found for this event -- run = " << e.run << std::endl;
	  //if(!metfilters_valid || metfilter_selection.passes(e)){
	    //if(ele_selection->passes(e)){
	  lumihists_trigger->fill(e);
	  return true;
	  //}
	  //}
	}
      }
      return false;
    }
    
private:
    std::unique_ptr<Hists> lumihists;
    std::unique_ptr<Hists> lumihists_trigger;
    std::unique_ptr<Selection> lumi_selection;
    std::unique_ptr<Selection> trigger_selection;  
    AndSelection metfilter_selection;

};


UHH2_REGISTER_ANALYSIS_MODULE(TestLumiHists)

