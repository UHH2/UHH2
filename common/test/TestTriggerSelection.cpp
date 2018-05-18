#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/common/include/TriggerSelection.h"

using namespace uhh2;

class TestTrigger: public uhh2::AnalysisModule {
public:
    explicit TestTrigger(Context &) {
        trigger_selection.reset(new TriggerSelection("HLT_PFHT800Emu_v*"));
    }
    
    virtual bool process(Event & e) override{
        return trigger_selection->passes(e);
    }
    
private:
    std::unique_ptr<Selection> trigger_selection;
};


UHH2_REGISTER_ANALYSIS_MODULE(TestTrigger)


/**
 * List triggers, and check at least one MET fileter and one HLT trigger
 */
class TestTriggerLister: public uhh2::AnalysisModule {
public:
    explicit TestTriggerLister(Context &) {}
    
    virtual bool process(Event & e) override{
      const std::vector<std::string> & trigNames = e.get_current_triggernames();
      
      bool flagTrig = false;
      std::string flagStart = "Flag_";
      
      bool hltTrig = false;
      std::string hltStart = "HLT_";
      
      for (const auto & itr : trigNames) {
        if (!flagTrig && (itr.substr(0, flagStart.size()) == flagStart)) flagTrig = true;
        if (!hltTrig && (itr.substr(0, hltStart.size()) == hltStart)) hltTrig = true;
        auto ti = e.get_trigger_index(itr);
        auto status = e.passes_trigger(ti);
        std::cout << itr << " = " << status << std::endl;
      }
      
      if (!flagTrig)
        throw std::runtime_error("No MET filters ("+flagStart+"*) in the Event!");
      
      if (!hltTrig)
        throw std::runtime_error("No HLT filters ("+hltStart+"*) in the Event!");
      
      return true;
    }
    
};

UHH2_REGISTER_ANALYSIS_MODULE(TestTriggerLister)
