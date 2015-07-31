#include <iostream>
#include <memory>

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/common/include/EventHists.h"

using namespace uhh2;

/** \brief module to produce histograms of the number of primary vertices for PU-reweighting
 */
class TestPUReweighting: public AnalysisModule {

 public:
  explicit TestPUReweighting(Context & ctx);
  virtual bool process(Event & event) override;

 private:
   std::unique_ptr<Hists> h_event;
};

TestPUReweighting::TestPUReweighting(Context & ctx){

  h_event.reset(new EventHists(ctx, "input_Event"));
}

bool TestPUReweighting::process(Event & event) {

  // dump input content
  h_event->fill(event);
  
  return true;
}

UHH2_REGISTER_ANALYSIS_MODULE(TestPUReweighting)
