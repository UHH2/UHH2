#include "UHH2/common/include/CommonModules.h"
#include "UHH2/common/include/JetCorrections.h"
#include "UHH2/common/include/JetCorrectionSets.h"
#include "UHH2/common/include/MCWeight.h"
#include "UHH2/common/include/EventVariables.h"
#include "UHH2/common/include/CleaningModules.h"
#include "UHH2/common/include/EventVariables.h"
#include "UHH2/common/include/LumiSelection.h"
#include "UHH2/common/include/Utils.h"
#include "UHH2/common/include/TriggerSelection.h"

using namespace uhh2;
using namespace std;

void CommonModules::fail_if_init() const{
  if(init_done){
    throw invalid_argument("CommonModules::init already called!");
  }
}


CommonModules::CommonModules(){
  working_point = JetPFID::WP_TIGHT_CHS;

  jec_tag_2016 = "Summer16_07Aug2017";
  jec_ver_2016 = "11";

  jec_tag_2017 = "Fall17_17Nov2017";
  jec_ver_2017 = "32";

  jec_tag_2018 = "Autumn18";
  jec_ver_2018 = "19";

  jec_jet_coll = "AK4PFchs";
}


void CommonModules::init(Context & ctx, const std::string & SysType_PU){
  if(init_done){
    throw invalid_argument("CommonModules::init called twice!");
  }
  init_done = true;

  is_mc = ctx.get("dataset_type") == "MC";
  year = extract_year(ctx);

  // setup correction jet type for JECs
  if (jec || jetlepcleaner) {
    std::string userJetColl = string2lowercase(ctx.get("JetCollection"));

    std::string algo = "AK4";
    // algo size
    if (userJetColl.find("ak8") != std::string::npos) {
      algo = "AK8";
    } else if (userJetColl.find("ak4") == std::string::npos) {
      std::cout << "Cannot determine jet cone + radius (neither AK4 nor AK8) - going to assume it is AK4 for JECs" << std::endl;
    }

    std::string pus = "PFchs";
    // Pileup subtraction
    if (userJetColl.find("puppi") != std::string::npos) {
      pus = "PFPuppi";
    } else if (userJetColl.find("chs") == std::string::npos) {
      std::cout << "Cannot determine pileup subtraction (neither CHS nor PUPPI) - going to assume it is CHS for JECs" << std::endl;
    }
    jec_jet_coll = algo + pus;
  }

  //set default PV id;
  PrimaryVertexId pvid=StandardPrimaryVertexId();
  if(pvfilter) modules.emplace_back(new PrimaryVertexCleaner(pvid));
  if(is_mc){
    if(mclumiweight)  modules.emplace_back(new MCLumiWeight(ctx));
    if(mcpileupreweight) modules.emplace_back(new MCPileupReweight(ctx,SysType_PU));
    if(jec){
      jet_corrector_MC.reset(new YearSwitcher(ctx));
      jet_corrector_MC->setup2016(std::make_shared<JetCorrector>(ctx, JERFiles::JECFilesMC(jec_tag_2016, jec_ver_2016, jec_jet_coll)));
      jet_corrector_MC->setup2017(std::make_shared<JetCorrector>(ctx, JERFiles::JECFilesMC(jec_tag_2017, jec_ver_2017, jec_jet_coll)));
      jet_corrector_MC->setup2018(std::make_shared<JetCorrector>(ctx, JERFiles::JECFilesMC(jec_tag_2018, jec_ver_2018, jec_jet_coll)));
    }
    if(jersmear) jet_resolution_smearer.reset(new JetResolutionSmearer(ctx));
  }
  else{
    if(lumisel) lumi_selection.reset(new LumiSelection(ctx));
    if(jec){
      jec_switcher_16.reset(new RunSwitcher(ctx, "2016"));
      for (const auto & runItr : runPeriods2016) { // runPeriods defined in common/include/Utils.h
        jec_switcher_16->setupRun(runItr, std::make_shared<JetCorrector>(ctx, JERFiles::JECFilesDATA(jec_tag_2016, jec_ver_2016, jec_jet_coll, runItr)));
      }

      jec_switcher_17.reset(new RunSwitcher(ctx, "2017"));
      for (const auto & runItr : runPeriods2017) {
        jec_switcher_17->setupRun(runItr, std::make_shared<JetCorrector>(ctx, JERFiles::JECFilesDATA(jec_tag_2017, jec_ver_2017, jec_jet_coll, runItr)));
      }

      jec_switcher_18.reset(new RunSwitcher(ctx, "2018"));
      for (const auto & runItr : runPeriods2018) {
        jec_switcher_18->setupRun(runItr, std::make_shared<JetCorrector>(ctx, JERFiles::JECFilesDATA(jec_tag_2018, jec_ver_2018, jec_jet_coll, runItr)));
      }

      jet_corrector_data.reset(new YearSwitcher(ctx));
      jet_corrector_data->setup2016(jec_switcher_16);
      jet_corrector_data->setup2017(jec_switcher_17);
      jet_corrector_data->setup2018(jec_switcher_18);
    }
  }
  if(metfilters){
    // https://twiki.cern.ch/twiki/bin/viewauth/CMS/MissingETOptionalFiltersRun2
    metfilters_selection.reset(new AndSelection(ctx, "metfilters"));
    metfilters_selection->add<TriggerSelection>("HBHENoiseFilter", "Flag_HBHENoiseFilter");
    metfilters_selection->add<TriggerSelection>("HBHENoiseIsoFilter", "Flag_HBHENoiseIsoFilter");
    metfilters_selection->add<TriggerSelection>("globalSuperTightHalo2016Filter", "Flag_globalSuperTightHalo2016Filter");
    metfilters_selection->add<TriggerSelection>("EcalDeadCellTriggerPrimitiveFilter", "Flag_EcalDeadCellTriggerPrimitiveFilter");
    if (!is_mc) metfilters_selection->add<TriggerSelection>("eeBadScFilter", "Flag_eeBadScFilter");
    // metfilters_selection->add<TriggerSelection>("BadChargedCandidateFilter", "Flag_BadChargedCandidateFilter"); // Not recommended, under review. Separate module in ntuple_generator for 2016v2
    if (year != Year::is2016v2) {
      metfilters_selection->add<TriggerSelection>("BadPFMuonFilter", "Flag_BadPFMuonFilter");
    } else {
      metfilters_selection->add<TriggerSelection>("BadPFMuonFilter", "Extra_BadPFMuonFilter");
    }
    metfilters_selection->add<TriggerSelection>("goodVertices", "Flag_goodVertices");
    metfilters_selection->add<EcalBadCalibSelection>("EcalBadCalibSelection"); // Use this instead of Flag_ecalBadCalibFilter, uses ecalBadCalibReducedMINIAODFilter in ntuple_generator
    if(pvfilter) metfilters_selection->add<NPVSelection>("1 good PV",1,-1,pvid);
  }
  if(eleid) modules.emplace_back(new ElectronCleaner(eleid));
  if(phoid) modules.emplace_back(new PhotonCleaner(phoid));
  if(muid)  modules.emplace_back(new MuonCleaner(muid));

  if(tauid) modules.emplace_back(new TauCleaner(tauid));
  if(jetpfidcleaner){
    modules.emplace_back(new JetCleaner(ctx, JetPFID(working_point)));
  }
  if(jetlepcleaner) {
    if(is_mc) {
      JLC_MC.reset(new YearSwitcher(ctx));
      JLC_MC->setup2016(std::make_shared<JetLeptonCleaner_by_KEYmatching>(ctx, JERFiles::JECFilesMC(jec_tag_2016, jec_ver_2016, jec_jet_coll)));
      JLC_MC->setup2017(std::make_shared<JetLeptonCleaner_by_KEYmatching>(ctx, JERFiles::JECFilesMC(jec_tag_2017, jec_ver_2017, jec_jet_coll)));
      JLC_MC->setup2018(std::make_shared<JetLeptonCleaner_by_KEYmatching>(ctx, JERFiles::JECFilesMC(jec_tag_2018, jec_ver_2018, jec_jet_coll)));
    }
    else{
      JLC_switcher_16.reset(new RunSwitcher(ctx, "2016"));
      for (const auto & runItr : runPeriods2016) {
        JLC_switcher_16->setupRun(runItr, std::make_shared<JetLeptonCleaner_by_KEYmatching>(ctx, JERFiles::JECFilesDATA(jec_tag_2016, jec_ver_2016, jec_jet_coll, runItr)));
      }

      JLC_switcher_17.reset(new RunSwitcher(ctx, "2017"));
      for (const auto & runItr : runPeriods2017) {
        JLC_switcher_17->setupRun(runItr, std::make_shared<JetLeptonCleaner_by_KEYmatching>(ctx, JERFiles::JECFilesDATA(jec_tag_2017, jec_ver_2017, jec_jet_coll, runItr)));
      }

      JLC_switcher_18.reset(new RunSwitcher(ctx, "2018"));
      for (const auto & runItr : runPeriods2018) {
        JLC_switcher_18->setupRun(runItr, std::make_shared<JetLeptonCleaner_by_KEYmatching>(ctx, JERFiles::JECFilesDATA(jec_tag_2018, jec_ver_2018, jec_jet_coll, runItr)));
      }

      JLC_data.reset(new YearSwitcher(ctx));
      JLC_data->setup2016(JLC_switcher_16);
      JLC_data->setup2017(JLC_switcher_17);
      JLC_data->setup2018(JLC_switcher_18);
    }
  }
  modules.emplace_back(new HTCalculator(ctx,HT_jetid));
  if(jetid) jet_cleaner.reset(new JetCleaner(ctx, jetid));
  print_setup();
}

bool CommonModules::process(uhh2::Event & event){
  if(!init_done){
    throw runtime_error("CommonModules::init not called (has to be called in AnalysisModule constructor)");
  }

  for(auto & m : modules){
    m->process(event);
  }

  if(jetlepcleaner){
    if (is_mc) {
      JLC_MC->process(event);
    } else {
      JLC_data->process(event);
    }
  }

  if(jec){
    if (is_mc) {
      jet_corrector_MC->process(event);
    } else {
      jet_corrector_data->process(event);
    }
  }

  if(jersmear && is_mc) jet_resolution_smearer->process(event);

  //set do_metcorrection = true in case you applied jet lepton cleaning by yourself and before calling common modules
  if((jetlepcleaner && jec) || (do_metcorrection && jec)){
    if (is_mc) {
      // some casting needed to get back to derived type
      std::shared_ptr<JetCorrector> jc = std::dynamic_pointer_cast<JetCorrector>(jet_corrector_MC->module());
      jc->correct_met(event);
    } else {
      std::shared_ptr<RunSwitcher> rs = std::dynamic_pointer_cast<RunSwitcher>(jet_corrector_data->module());
      std::shared_ptr<JetCorrector> jc = std::dynamic_pointer_cast<JetCorrector>(rs->module(event));
      jc->correct_met(event);
    }
  }
  // else if(jec || jetlepcleaner) cout <<"WARNING: You used CommonModules for either JEC or jet-lepton-cleaning but MET is not corrected. Please be aware of this." << endl;

  if(jetid) jet_cleaner->process(event);

  if(jetptsort){
    sort_by_pt(*event.jets);
  }

  // Put the return parts last, such that every other modifying module always runs
  // This avoids bugs where this function is exited early, but the user expects
  // the other modules to always run
  if(event.isRealData && lumisel){
    if(!lumi_selection->passes(event)) return false;
  }

  if(metfilters){
    if(!metfilters_selection->passes(event)) return false;
  }

  return true;
}


void CommonModules::disable_mclumiweight(){
  fail_if_init();
  mclumiweight = false;
}

void CommonModules::disable_mcpileupreweight(){
  fail_if_init();
  mcpileupreweight = false;
}

void CommonModules::disable_jec(){
  fail_if_init();
  jec = false;
}

void CommonModules::disable_jersmear(){
  fail_if_init();
  jersmear = false;
}

void CommonModules::disable_lumisel(){
  fail_if_init();
  lumisel = false;
}

void CommonModules::disable_metfilters(){
  fail_if_init();
  metfilters = false;
}

void CommonModules::disable_pvfilter(){
  fail_if_init();
  pvfilter = false;
}

void CommonModules::print_setup() const {
  std::map<std::string, bool> settings_map = {
    {"MC luminosity reweighting", mclumiweight},
    {"MC pileup reweighting", mcpileupreweight},
    {"Golden Lumi", lumisel},
    {"JECs", jec},
    {"JER smearing", jersmear},
    {"Jet-Lepton cleaning", jetlepcleaner},
    {"MET Type-1 corrections", ((jetlepcleaner && jec) || (do_metcorrection && jec))},
    {"MET filters", metfilters},
    {"PV filter", pvfilter},
    {"Jet pT sorting", jetptsort},
    {"Jet PF ID cleaner", jetpfidcleaner},
    {"Jet ID", (bool) jetid},
    {"Electron ID", (bool) eleid},
    {"Photon ID", (bool) phoid},
    {"Muon ID", (bool) muid},
    {"Tau ID", (bool) tauid},
  };

  cout << "----------------------------------------------------------------------------------------------------" << endl;
  cout << "CommonModules setup:" << endl;
  cout << "----------------------------------------------------------------------------------------------------" << endl;
  cout << "is MC? = " << (is_mc ? "TRUE" : "FALSE") << endl;
  cout << "Year = " << year_str_map.at(year) << endl; // can't use [] accessor as not const
  cout << endl;
  for (auto const & [name, flag] : settings_map) {
    cout << name << " = " << (flag ? "ON" : "OFF") << endl;
  }
  cout << endl;
  if (jec || jetlepcleaner || do_metcorrection) {
    cout << "2016 JECs: " << jec_tag_2016 << " V" << jec_ver_2016 << " for " << jec_jet_coll << endl;
    cout << "2017 JECs: " << jec_tag_2017 << " V" << jec_ver_2017 << " for " << jec_jet_coll << endl;
    cout << "2018 JECs: " << jec_tag_2018 << " V" << jec_ver_2018 << " for " << jec_jet_coll << endl;
  }
  cout << endl;

  // Now some special messages
  // coopy logic from process()
  if (!((jetlepcleaner && jec) || (do_metcorrection && jec)) && (jec || jetlepcleaner)) {
    cout << "WARNING: You used CommonModules for either JEC or jet-lepton-cleaning but MET is not corrected." << endl;
  }
  cout << "----------------------------------------------------------------------------------------------------" << endl;
}

class TestCommonModules: public AnalysisModule {
public:
  TestCommonModules(uhh2::Context & ctx){
    common.reset(new CommonModules());
    // in a non-trivial usage, would call
    // common->set_*_id   and
    // common->disable_*  here.
    common->init(ctx);
  }

  virtual bool process(Event & event) override {
    bool pass_cm = common->process(event);
    if(!pass_cm) std::cout << "Event rejected by common modules" << std::endl;
    return pass_cm;
  }
private:
  std::unique_ptr<CommonModules> common;
};

UHH2_REGISTER_ANALYSIS_MODULE(TestCommonModules)
