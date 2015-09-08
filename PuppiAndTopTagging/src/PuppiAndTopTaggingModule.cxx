#include <iostream>
#include <memory>

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/common/include/CommonModules.h"
#include "UHH2/common/include/CleaningModules.h"
#include "UHH2/common/include/ElectronHists.h"
#include "UHH2/common/include/NSelections.h"
#include "UHH2/common/include/JetCorrections.h"
#include "UHH2/common/include/TopJetIds.h"
#include "UHH2/common/include/Utils.h"
#include "UHH2/common/include/TTbarGen.h"
#include "UHH2/common/include/CollectionProducer.h"

#include "UHH2/PuppiAndTopTagging/include/PuppiAndTopTaggingSelections.h"
#include "UHH2/PuppiAndTopTagging/include/PuppiAndTopTaggingHists.h"
#include "UHH2/PuppiAndTopTagging/include/MatchingSignal.h"
#include "UHH2/PuppiAndTopTagging/include/MatchingBackground.h"
#include "UHH2/common/include/TTbarGen.h"

using namespace std;
using namespace uhh2;

class PuppiAndTopTaggingModule: public AnalysisModule {
public:
   
   explicit PuppiAndTopTaggingModule(Context & ctx);
   virtual bool process(Event & event) override;
   
private:
   bool is_signal;
   
   uhh2::Event::Handle<std::vector<TopJet> > h_heptopjetsCHS;
   uhh2::Event::Handle<std::vector<TopJet> > h_heptopjetsPuppi;

   std::unique_ptr<TopJetCleaner> topjet_cleaner;
   std::unique_ptr<TopJetCorrector> topjet_corrector;
   std::unique_ptr<TopJetCorrector> topjetPuppi_corrector;
   std::unique_ptr<TopJetCorrector> heptopjetCHS_corrector;
   std::unique_ptr<TopJetCorrector> heptopjetPuppi_corrector;
   std::unique_ptr<AnalysisModule> ttgenprod;
   std::unique_ptr<AnalysisModule> match_signal_hepttPuppi;
   std::unique_ptr<AnalysisModule> match_signal_hepttCHS;
   std::unique_ptr<AnalysisModule> match_bg_hepttPuppi;
   std::unique_ptr<AnalysisModule> match_bg_hepttCHS;
   std::unique_ptr<AnalysisModule> match_bg;
   
   std::unique_ptr<AnalysisModule> HepTTPuppicollectionprod;
   std::unique_ptr<AnalysisModule> HepTTCHScollectionprod;
   std::unique_ptr<AnalysisModule> CMSTTPuppicollectionprod;
   
   std::unique_ptr<Hists> fakerate_hists;
   std::unique_ptr<PuppiAndTopTaggingHists> efficiency_hepttPuppi_hists;
   std::unique_ptr<PuppiAndTopTaggingHists> efficiency_hepttCHS_hists;
};


PuppiAndTopTaggingModule::PuppiAndTopTaggingModule(Context & ctx){
   
   is_signal = (ctx.get("dataset_version") == "ZprimeToTT_M-2000"); 
   
   HepTTPuppicollectionprod.reset(new CollectionProducer<TopJet>(ctx, "patJetsHepTopTagPuppiPacked_daughters", "h_heptopjetsPuppi",TopJetId(PtEtaCut(150., 2.4))));
   HepTTCHScollectionprod.reset(new CollectionProducer<TopJet>(ctx, "patJetsHepTopTagCHSPacked_daughters", "h_heptopjetsCHS",TopJetId(PtEtaCut(150., 2.4))));
 
   ttgenprod.reset(new TTbarGenProducer(ctx, "ttbargen", false));
   
   match_signal_hepttPuppi.reset(new MatchingSignal(ctx, "h_heptopjetsPuppi","patJetsHepTopTagPuppiPacked_daughters"));
   match_signal_hepttCHS.reset(new MatchingSignal(ctx, "h_heptopjetsCHS", "patJetsHepTopTagCHSPacked_daughters"));
   
   match_bg_hepttPuppi.reset(new MatchingBackground(ctx, "h_heptopjetsPuppi","patJetsHepTopTagPuppiPacked_daughters"));
   match_bg_hepttCHS.reset(new MatchingBackground(ctx, "h_heptopjetsCHS", "patJetsHepTopTagCHSPacked_daughters"));
   

   topjet_cleaner.reset(new TopJetCleaner(TopJetId(PtEtaCut(350., 2.4)))); 
   topjet_corrector.reset(new TopJetCorrector(JERFiles::Summer15_50ns_L123_AK8PFPuppi_MC));   //wie kann ich verschiedene collections korrigieren?
   // // topjetPuppi_corrector.reset(new TopJetCorrector(JERFiles::Summer15_50ns_L123_AK8PFPuppi_MC));
   // // heptopjetCHS_corrector.reset(new TopJetCorrector(JERFiles::Summer15_50ns_L123_AK8PFPuppi_MC));
   // // heptopjetPuppi_corrector.reset(new TopJetCorrector(JERFiles::Summer15_50ns_L123_AK8PFPuppi_MC));
   
   const TopJetId HTTTopJetId = AndId<TopJet>(HEPTopTagV2(), Tau32()); 

   efficiency_hepttPuppi_hists.reset(new PuppiAndTopTaggingHists(ctx,"efficiency_hepttPuppi", "patJetsHepTopTagPuppiPacked_daughters"));
   efficiency_hepttPuppi_hists->set_TopJetId(HTTTopJetId); 
  
   efficiency_hepttCHS_hists.reset(new PuppiAndTopTaggingHists(ctx,"efficiency_hepttCHS", "patJetsHepTopTagCHSPacked_daughters"));
   efficiency_hepttCHS_hists->set_TopJetId(HTTTopJetId); 
}


bool PuppiAndTopTaggingModule::process(Event & event) {  
   
   HepTTPuppicollectionprod->process(event);
   HepTTCHScollectionprod->process(event);
 
   topjet_corrector->process(event);
   topjet_cleaner->process(event);
   sort_by_pt<TopJet>(*event.topjets);
      
   if(!is_signal) {
      match_bg_hepttPuppi->process(event);
      match_bg_hepttCHS->process(event);
   }
   else 
      {
         ttgenprod->process(event);
         match_signal_hepttPuppi->process(event);
         match_signal_hepttCHS->process(event);
      }
   
   efficiency_hepttPuppi_hists->fill(event);
   efficiency_hepttCHS_hists->fill(event);
   return true;

}
UHH2_REGISTER_ANALYSIS_MODULE(PuppiAndTopTaggingModule)

