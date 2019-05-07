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
#include <UHH2/common/include/JetCorrectionSets.h>
#include <UHH2/common/include/ObjectIdUtils.h>
#include <UHH2/common/include/MuonIds.h>
#include <UHH2/common/include/ElectronIds.h>
#include <UHH2/common/include/JetIds.h>
#include <UHH2/common/include/TopJetIds.h>
#include <UHH2/common/include/TTbarGen.h>
#include <UHH2/common/include/Utils.h>

class test_JLCkey : public uhh2::AnalysisModule {

 public:
  explicit test_JLCkey(uhh2::Context&);
  virtual bool process(uhh2::Event&) override;

 protected:

  // cleaners
  std::unique_ptr<MuonCleaner>     muo_cleaner;
  std::unique_ptr<ElectronCleaner> ele_cleaner;
  std::unique_ptr<JetCleaner>      jet_IDcleaner;
  std::unique_ptr<JetCorrector>    jet_corrector;
  std::unique_ptr<JetLeptonCleaner>                jetlepton_cleaner1;
  std::unique_ptr<JetLeptonCleaner_by_KEYmatching> jetlepton_cleaner2;
};

test_JLCkey::test_JLCkey(uhh2::Context& ctx){

  //// CONFIGURATION

  const bool isMC = (ctx.get("dataset_type") == "MC");

  ////

  //// OBJ CLEANING
  const     MuonId muo(AndId<Muon>    (PtEtaCut  (50., 2.1), MuonID(Muon::CutBasedIdMedium)));
  const ElectronId ele(AndId<Electron>(PtEtaSCCut(50., 2.5), ElectronID_Summer16_tight_noIso));

  muo_cleaner.reset(new     MuonCleaner(muo));
  ele_cleaner.reset(new ElectronCleaner(ele));
  //

  const JetId jetID(JetPFID(JetPFID::WP_TIGHT_CHS));

  std::vector<std::string> JEC_AK4, JEC_AK8;
  if(isMC){

    JEC_AK4 = JERFiles::JECFilesMC("Summer16_07Aug2017", "11", "AK4PFchs");
    JEC_AK8 = JERFiles::JECFilesMC("Summer16_07Aug2017", "11", "AK8PFchs");
  }
  else {

    JEC_AK4 = JERFiles::JECFilesDATA("Summer16_07Aug2017", "11", "AK4PFchs", "B");
    JEC_AK8 = JERFiles::JECFilesDATA("Summer16_07Aug2017", "11", "AK8PFchs", "B");
  }

  jet_IDcleaner.reset(new JetCleaner(ctx, jetID));
  jet_corrector.reset(new JetCorrector(ctx, JEC_AK4));
  jetlepton_cleaner1.reset(new JetLeptonCleaner(ctx, JEC_AK4));
  jetlepton_cleaner1->set_drmax(.4);
  jetlepton_cleaner2.reset(new JetLeptonCleaner_by_KEYmatching(ctx, JEC_AK4));
  ////
}

bool test_JLCkey::process(uhh2::Event& event){

  //// COMMON MODULES
  ////

  //// LEPTON selection
  muo_cleaner->process(event);
  sort_by_pt<Muon>(*event.muons);

  ele_cleaner->process(event);
  sort_by_pt<Electron>(*event.electrons);

  const bool pass_lep1 = ((event.muons->size() >= 1) || (event.electrons->size() >= 1));
  if(!pass_lep1) return false;
  ////

  //// JET selection
  jet_IDcleaner->process(event);
  jet_corrector->process(event);
  sort_by_pt<Jet>(*event.jets);

  const auto jetsINI(*event.jets);
  ////

  std::cout << "******* EVENT **********\n";

  std::cout << "------- MUONS  ---------\n";
  for(const auto& l : *event.muons){

    std::cout << " muo :" << " pt=" << l.pt() << " eta=" << l.eta() << " phi=" << l.phi();

    std::cout << " sc-keys=";
    for(const auto& sc : l.source_candidates()) std::cout << sc.key << ", ";
    std::cout << "\n";
  }

  std::cout << "------- ELECS  ---------\n";
  for(const auto& l : *event.electrons){

    std::cout << " ele :" << " pt=" << l.pt() << " eta=" << l.eta() << " phi=" << l.phi();

    std::cout << " sc-keys=";
    for(const auto& sc : l.source_candidates()) std::cout << sc.key << ", ";
    std::cout << "\n";
  }

  std::cout << "------- JETS 0 ---------\n";
  for(const auto& j : *event.jets){

    std::cout << " jet :" << " pt=" << j.pt() << " eta=" << j.eta() << " phi=" << j.phi();

    std::cout << " keys=";
    for(const auto& k : j.lepton_keys()) std::cout << k << ", ";
    std::cout << "\n";
  }

  //
  jetlepton_cleaner1->process(event);

  std::cout << "------- JETS 1 ---------\n";
  for(const auto& j : *event.jets){

    std::cout << " jet :" << " pt=" << j.pt() << " eta=" << j.eta() << " phi=" << j.phi();

    std::cout << " keys=";
    for(const auto& k : j.lepton_keys()) std::cout << k << ", ";
    std::cout << "\n";
  }
  //

  event.jets->clear();
  event.jets->reserve(jetsINI.size());
  for(const auto& j : jetsINI) event.jets->push_back(j);
  sort_by_pt<Jet>(*event.jets);

  //
  jetlepton_cleaner2->process(event);

  std::cout << "------- JETS 2 ---------\n";
  for(const auto& j : *event.jets){

    std::cout << " jet :" << " pt=" << j.pt() << " eta=" << j.eta() << " phi=" << j.phi();

    std::cout << " keys=";
    for(const auto& k : j.lepton_keys()) std::cout << k << ", ";
    std::cout << "\n";
  }
  //

  return false;
}

UHH2_REGISTER_ANALYSIS_MODULE(test_JLCkey)
