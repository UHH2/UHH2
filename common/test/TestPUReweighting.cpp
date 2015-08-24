#include <iostream>
#include <memory>

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/common/include/EventHists.h"
#include "UHH2/common/include/CleaningModules.h"
#include "UHH2/common/include/LumiSelection.h"
#include "UHH2/common/include/NSelections.h"

using namespace uhh2;

/** \brief module to produce histograms of the number of primary vertices for PU-reweighting
 */
class TestPUReweighting: public AnalysisModule {

 public:
  explicit TestPUReweighting(Context & ctx);
  virtual bool process(Event & event) override;

 private:
   std::unique_ptr<PrimaryVertexCleaner>  pvcleaner;
   std::unique_ptr<Hists> h_event;
   std::unique_ptr<Selection> lumi_selection;
   std::unique_ptr<Selection> npv_sel;
};

TestPUReweighting::TestPUReweighting(Context & ctx){

   PrimaryVertexId pvid=StandardPrimaryVertexId();
   pvcleaner.reset(new PrimaryVertexCleaner(pvid));
   npv_sel.reset(new NPVSelection(1,-1,pvid));
   
   lumi_selection.reset(new LumiSelection(ctx));

   h_event.reset(new EventHists(ctx, "input_Event"));
}

bool TestPUReweighting::process(Event & event) {
   
   pvcleaner->process(event);
   
   if (event.isRealData)
      {
         if(!lumi_selection->passes(event)) return false;
      }
   
   bool pass_npv = npv_sel->passes(event);
   if (!pass_npv) return false;
   
   // dump input content
   h_event->fill(event);
  
  return true;
}

UHH2_REGISTER_ANALYSIS_MODULE(TestPUReweighting)
