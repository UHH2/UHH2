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
  jec_tag = "Fall17_17Nov2017";
  jec_ver = "32";
  jec_jet_coll = "AK4PFchs";
}


void CommonModules::init(Context & ctx, const std::string & SysType_PU){
  if(init_done){
    throw invalid_argument("CommonModules::init called twice!");
  }
  init_done = true;

  is_mc = ctx.get("dataset_type") == "MC";
  year = extract_year(ctx);

  //set default PV id;
  PrimaryVertexId pvid=StandardPrimaryVertexId();
  if(pvfilter) modules.emplace_back(new PrimaryVertexCleaner(pvid));
  if(is_mc){
    if(mclumiweight)  modules.emplace_back(new MCLumiWeight(ctx));
    if(mcpileupreweight) modules.emplace_back(new MCPileupReweight(ctx,SysType_PU));
    if(jec){
      jet_corrector_MC.reset(new JetCorrector(ctx, JERFiles::JECFilesMC(jec_tag, jec_ver, jec_jet_coll)));
    }
    if(jersmear) jet_resolution_smearer.reset(new JetResolutionSmearer(ctx));
  }
  else{
    if(lumisel) lumi_selection.reset(new LumiSelection(ctx));
    if(jec){
      jet_corrector_B.reset(new JetCorrector(ctx, JERFiles::JECFilesDATA(jec_tag, jec_ver, jec_jet_coll, "B")));
      jet_corrector_C.reset(new JetCorrector(ctx, JERFiles::JECFilesDATA(jec_tag, jec_ver, jec_jet_coll, "C")));
      jet_corrector_D.reset(new JetCorrector(ctx, JERFiles::JECFilesDATA(jec_tag, jec_ver, jec_jet_coll, "D")));
      jet_corrector_E.reset(new JetCorrector(ctx, JERFiles::JECFilesDATA(jec_tag, jec_ver, jec_jet_coll, "E")));
      jet_corrector_F.reset(new JetCorrector(ctx, JERFiles::JECFilesDATA(jec_tag, jec_ver, jec_jet_coll, "F")));
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
  if(muid)  modules.emplace_back(new MuonCleaner(muid));
  if(tauid) modules.emplace_back(new TauCleaner(tauid));
  if(jetpfidcleaner){
    modules.emplace_back(new JetCleaner(ctx, JetPFID(working_point)));
  }
  if(jetlepcleaner) {
    if(is_mc) JLC_MC.reset(new JetLeptonCleaner_by_KEYmatching(ctx, JERFiles::JECFilesMC(jec_tag, jec_ver, jec_jet_coll)));
    else{
      JLC_B.reset(new JetLeptonCleaner_by_KEYmatching(ctx, JERFiles::JECFilesDATA(jec_tag, jec_ver, jec_jet_coll, "B")));
      JLC_C.reset(new JetLeptonCleaner_by_KEYmatching(ctx, JERFiles::JECFilesDATA(jec_tag, jec_ver, jec_jet_coll, "C")));
      JLC_D.reset(new JetLeptonCleaner_by_KEYmatching(ctx, JERFiles::JECFilesDATA(jec_tag, jec_ver, jec_jet_coll, "DE")));
      JLC_E.reset(new JetLeptonCleaner_by_KEYmatching(ctx, JERFiles::JECFilesDATA(jec_tag, jec_ver, jec_jet_coll, "DE")));
      JLC_F.reset(new JetLeptonCleaner_by_KEYmatching(ctx, JERFiles::JECFilesDATA(jec_tag, jec_ver, jec_jet_coll, "F")));
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
  if(event.isRealData && lumisel){
    if(!lumi_selection->passes(event)) return false;
  }
  for(auto & m : modules){
    m->process(event);
  }
  if(metfilters){
    if(!metfilters_selection->passes(event)) return false;
  }

  if(jetlepcleaner){
    if(is_mc) JLC_MC->process(event);
    else{
      if(event.run <= runnr_B)      JLC_B->process(event);
      else if(event.run <= runnr_C) JLC_C->process(event);
      else if(event.run <= runnr_D) JLC_D->process(event);
      else if(event.run <= runnr_E) JLC_E->process(event);
      else if(event.run <= runnr_F) JLC_F->process(event);
      else throw runtime_error("CommonModules.cxx: run number not covered by if-statements in process-routine.");
    }
  }

  if(jec){
    if(is_mc)jet_corrector_MC->process(event);
    else{
      if(event.run <= runnr_B)      jet_corrector_B->process(event);
      else if(event.run <= runnr_C) jet_corrector_C->process(event);
      else if(event.run <= runnr_D) jet_corrector_D->process(event);
      else if(event.run <= runnr_E) jet_corrector_E->process(event);
      else if(event.run <= runnr_F) jet_corrector_F->process(event);
      else throw runtime_error("CommonModules.cxx: run number not covered by if-statements in process-routine.");
    }
  }

  if(jersmear && is_mc) jet_resolution_smearer->process(event);

  //set do_metcorrection = true in case you applied jet lepton cleaning by yourself and before calling common modules
  if((jetlepcleaner && jec) || (do_metcorrection && jec)){
    if(is_mc) jet_corrector_MC->correct_met(event);
    else{
      if(event.run <= runnr_B)      jet_corrector_B->correct_met(event);
      else if(event.run <= runnr_C) jet_corrector_C->correct_met(event);
      else if(event.run <= runnr_D) jet_corrector_D->correct_met(event);
      else if(event.run <= runnr_E) jet_corrector_E->correct_met(event);
      else if(event.run <= runnr_F) jet_corrector_F->correct_met(event);
      else throw runtime_error("CommonModules.cxx: run number not covered by if-statements in process-routine.");
    }
  }
  // else if(jec || jetlepcleaner) cout <<"WARNING: You used CommonModules for either JEC or jet-lepton-cleaning but MET is not corrected. Please be aware of this." << endl;

  if(jetid) jet_cleaner->process(event);

  if(jetptsort){
    sort_by_pt(*event.jets);
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
    {"MET Type-1 corrections", do_metcorrection},
    {"MET filters", metfilters},
    {"PV filter", pvfilter},
    {"Jet pT sorting", jetptsort},
    {"Jet PF ID cleaner", jetpfidcleaner},
    {"Jet ID", (bool) jetid},
    {"Electron ID", (bool) eleid},
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
    cout << "JECs: " << jec_tag << " V" << jec_ver << " for " << jec_jet_coll << endl;
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
