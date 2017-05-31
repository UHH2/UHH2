#include <iostream>
#include <memory>

#include <UHH2/core/include/AnalysisModule.h>
#include <UHH2/core/include/Event.h>
#include <UHH2/core/include/Selection.h>

#include <UHH2/common/include/CleaningModules.h>
#include <UHH2/common/include/NSelections.h>
#include <UHH2/common/include/LumiSelection.h>
#include <UHH2/common/include/TriggerSelection.h>
#include <UHH2/common/include/JetCorrections.h>
#include <UHH2/common/include/ObjectIdUtils.h>
#include <UHH2/common/include/MuonIds.h>
#include <UHH2/common/include/ElectronIds.h>
#include <UHH2/common/include/JetIds.h>
#include <UHH2/common/include/TopJetIds.h>
#include <UHH2/common/include/TTbarGen.h>
#include <UHH2/common/include/Utils.h>

class test_JER : public uhh2::AnalysisModule {

 public:
  explicit test_JER(uhh2::Context&);
  virtual bool process(uhh2::Event&) override;

 protected:

  // cleaners
  std::unique_ptr<JetCleaner>           jet_IDcleaner;
  std::unique_ptr<JetCorrector>         jet_corrector;
  std::unique_ptr<       JetResolutionSmearer> jetER_smearer1;
  std::unique_ptr<GenericJetResolutionSmearer> jetER_smearer2;

  std::unique_ptr<JetCleaner>           topjet_IDcleaner;
  std::unique_ptr<TopJetCorrector>      topjet_corrector;
  std::unique_ptr<SubJetCorrector>      topjet_subjet_corrector;
  std::unique_ptr<GenericJetResolutionSmearer> topjetER_smearer;
};

test_JER::test_JER(uhh2::Context& ctx){

  //// CONFIGURATION

  const bool isMC = (ctx.get("dataset_type") == "MC");

  ////

  //// OBJ CLEANING
  const JetId jetID(JetPFID(JetPFID::WP_LOOSE));

  std::vector<std::string> JEC_AK4, JEC_AK8;
  if(isMC){

    JEC_AK4 = JERFiles::Summer16_23Sep2016_V4_L123_AK4PFchs_MC;
    JEC_AK8 = JERFiles::Summer16_23Sep2016_V4_L123_AK4PFchs_MC;
  }
  else {

    JEC_AK4 = JERFiles::Summer16_23Sep2016_V4_BCD_L123_AK4PFchs_DATA;
    JEC_AK8 = JERFiles::Summer16_23Sep2016_V4_BCD_L123_AK4PFchs_DATA;
  }

  jet_IDcleaner.reset(new JetCleaner(ctx, jetID));
  jet_corrector.reset(new JetCorrector(ctx, JEC_AK4));
  if(isMC) jetER_smearer1.reset(new        JetResolutionSmearer(ctx));
  //if(isMC) jetER_smearer2.reset(new GenericJetResolutionSmearer(ctx, "jets", "genjets", false));

  ctx.declare_event_input<std::vector<Particle> >(ctx.get("TopJetCollectionGEN"), "topjetsGEN");

  topjet_IDcleaner.reset(new JetCleaner(ctx, jetID));
  topjet_corrector.reset(new TopJetCorrector(ctx, JEC_AK8));
  topjet_subjet_corrector.reset(new SubJetCorrector(ctx, JEC_AK4));
  if(isMC) topjetER_smearer.reset(new GenericJetResolutionSmearer(ctx, "topjets", "topjetsGEN", false, JERSmearing::SF_13TeV_2016, "Spring16_25nsV10_MC_PtResolution_AK8PFchs.txt"));
  ////
}

bool test_JER::process(uhh2::Event& event){

  //// COMMON MODULES
  ////

  //// JET selection
  jet_IDcleaner->process(event);
  jet_corrector->process(event);
  sort_by_pt<Jet>(*event.jets);

  topjet_IDcleaner->process(event);
  topjet_corrector->process(event);
  topjet_subjet_corrector->process(event);
  sort_by_pt<TopJet>(*event.topjets);
  ////

  std::cout << "\n\n******* EVENT *******************************\n";

  std::cout << "------- JETS 0 ------------------------------\n";
  for(const auto& j : *event.jets)    std::cout << "     jet :" << " pt=" << j.pt() << " eta=" << j.eta() << " phi=" << j.phi() << std::endl;
  std::cout << "     MET :" << " pt=" << event.met->pt() << " phi=" << event.met->phi() << std::endl;

  const auto jetsINI(*event.jets);

  if(jetER_smearer1){

    jetER_smearer1->process(event);

    std::cout << "------- JETS 1 [JER smearing 1] -------------\n";
    for(const auto& j : *event.jets)    std::cout << "     jet :" << " pt=" << j.pt() << " eta=" << j.eta() << " phi=" << j.phi() << std::endl;
    std::cout << "     MET :" << " pt=" << event.met->pt() << " phi=" << event.met->phi() << std::endl;
  }


  event.jets->clear();
  event.jets->reserve(jetsINI.size());
  for(const auto& lep : jetsINI) event.jets->push_back(lep);
  sort_by_pt<Jet>(*event.jets);

  if(jetER_smearer2){

    jetER_smearer2->process(event);

    std::cout << "------- JETS 2 [JER smearing 2] -------------\n";
    for(const auto& j : *event.jets)    std::cout << "     jet :" << " pt=" << j.pt() << " eta=" << j.eta() << " phi=" << j.phi() << std::endl;
  }
  //

  std::cout << "------- TOP-JETS 0 --------------------------\n";
  for(const auto& j : *event.topjets) std::cout << " top-jet :" << " pt=" << j.pt() << " eta=" << j.eta() << " phi=" << j.phi() << std::endl;

  if(topjetER_smearer){

    topjetER_smearer->process(event);

    std::cout << "------- TOP-JETS 1 [JER smearing 2] ---------\n";
    for(const auto& j : *event.topjets) std::cout << " top-jet :" << " pt=" << j.pt() << " eta=" << j.eta() << " phi=" << j.phi() << std::endl;
  }
  //

  return false;
}

UHH2_REGISTER_ANALYSIS_MODULE(test_JER)
