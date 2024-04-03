#include "UHH2/common/include/LeptonScaleFactors.h"

#include "UHH2/common/include/MCWeight.h"
#include "UHH2/common/include/TriggerSelection.h"

using namespace std;
using namespace uhh2;


namespace uhh2 {

//____________________________________________________________________________________________________
ElectronRecoScaleFactors::ElectronRecoScaleFactors(
  Context & ctx,
  const boost::optional<bool> & low_pt_electrons,
  const boost::optional<bool> & do_check,
  const boost::optional<std::string> & weight_postfix,
  const boost::optional<std::string> & handle_name,
  const boost::optional<bool> & dummy
):
  fLowPtElectrons(low_pt_electrons ? *low_pt_electrons : false),
  fDoCheck(do_check ? *do_check : true),
  fWeightPostfix(weight_postfix ? *weight_postfix : "reco"),
  fHandleName(handle_name ? *handle_name : "electrons"),
  fDummy(dummy ? *dummy : false),
  fHandleElectrons(ctx.get_handle<vector<Electron>>(fHandleName))
{
  const string syst_direction = ctx.get(fSystDirectionConfigName, "nominal");
  if(fDummy) { // These handle names must match the ones from the MCElecScaleFactor class!
    fWeightHandles.push_back(ctx.declare_event_output<float>((string)"weight_sfelec_"+fWeightPostfix));
    fWeightHandles.push_back(ctx.declare_event_output<float>((string)"weight_sfelec_"+fWeightPostfix+"_up"));
    fWeightHandles.push_back(ctx.declare_event_output<float>((string)"weight_sfelec_"+fWeightPostfix+"_down"));
  }
  else {
    const string base_file_path = (string)getenv("CMSSW_BASE")+"/src/UHH2/"+kBasePathToULEGammaSFs;
    const string above_below = fLowPtElectrons ? "Below" : "Above";
    fYearSwitcher.reset(new YearSwitcher(ctx));
    fYearSwitcher->setupUL16preVFP(
      make_shared<MCElecScaleFactor>(
        ctx,
        base_file_path+"UL16preVFP/egammaEffi_pt"+above_below+"20.txt_EGM2D_UL2016preVFP.root",
        0.0,
        fWeightPostfix,
        syst_direction,
        fHandleName,
        "EGamma_SF2D",
        false
      )
    );
    fYearSwitcher->setupUL16postVFP(
      make_shared<MCElecScaleFactor>(
        ctx,
        base_file_path+"UL16postVFP/egammaEffi_pt"+above_below+"20.txt_EGM2D_UL2016postVFP.root",
        0.0,
        fWeightPostfix,
        syst_direction,
        fHandleName,
        "EGamma_SF2D",
        false
      )
    );
    fYearSwitcher->setupUL17(
      make_shared<MCElecScaleFactor>(
        ctx,
        base_file_path+"UL17/egammaEffi_pt"+above_below+"20.txt_EGM2D_UL2017.root",
        0.0,
        fWeightPostfix,
        syst_direction,
        fHandleName,
        "EGamma_SF2D",
        false
      )
    );
    fYearSwitcher->setupUL18(
      make_shared<MCElecScaleFactor>(
        ctx,
        base_file_path+"UL18/egammaEffi_pt"+above_below+"20.txt_EGM2D_UL2018.root",
        0.0,
        fWeightPostfix,
        syst_direction,
        fHandleName,
        "EGamma_SF2D",
        false
      )
    );
  }
}

bool ElectronRecoScaleFactors::process(Event & event) {
  if(fDummy) {
    for(uint i = 0; i < fWeightHandles.size(); i++) {
      event.set(fWeightHandles.at(i), -1.);
    }
  }
  else {
    if(fDoCheck) {
      for(const Electron & electron : event.get(fHandleElectrons)) {
        if(fLowPtElectrons && electron.v4().pt() > 20.) throw runtime_error("ElectronRecoScaleFactors::process(): Found electron with pT > 20 GeV in collection '"+fHandleName+"' but this module is setup to handle only electrons with pT < 20 GeV");
        else if(!fLowPtElectrons && electron.v4().pt() < 20.) throw runtime_error("ElectronRecoScaleFactors::process(): Found electron with pT < 20 GeV in collection '"+fHandleName+"' but this module is setup to handle only electrons with pT > 20 GeV");
      }
    }
    fYearSwitcher->process(event);
  }
  return true;
}

//____________________________________________________________________________________________________
ElectronIdScaleFactors::ElectronIdScaleFactors(
  Context & ctx,
  const boost::optional<Electron::tag> & tagID,
  const boost::optional<bool> & do_check,
  const boost::optional<std::string> & weight_postfix,
  const boost::optional<std::string> & handle_name,
  const boost::optional<bool> & dummy
):
  fDoCheck(do_check ? *do_check : true),
  fWeightPostfix(weight_postfix ? *weight_postfix : "id"),
  fHandleName(handle_name ? *handle_name : "electrons"),
  fDummy(dummy ? *dummy : false),
  fHandleElectrons(ctx.get_handle<vector<Electron>>(fHandleName))
{
  const string syst_direction = ctx.get(fSystDirectionConfigName, "nominal");
  if(fDummy) { // These handle names must match the ones from the MCElecScaleFactor class!
    fWeightHandles.push_back(ctx.declare_event_output<float>((string)"weight_sfelec_"+fWeightPostfix));
    fWeightHandles.push_back(ctx.declare_event_output<float>((string)"weight_sfelec_"+fWeightPostfix+"_up"));
    fWeightHandles.push_back(ctx.declare_event_output<float>((string)"weight_sfelec_"+fWeightPostfix+"_down"));
  }
  else if(tagID) {
    fElectronID = ElectronTagID(*tagID);
    const string base_file_path = (string)getenv("CMSSW_BASE")+"/src/UHH2/"+kBasePathToULEGammaSFs;
    string file_name_UL16preVFP;
    string file_name_UL16postVFP;
    string file_name_UL17;
    string file_name_UL18;
    switch(*tagID) {
      case Electron::tag::cutBasedElectronID_Fall17_94X_V2_veto :
        file_name_UL16preVFP = "egammaEffi.txt_Ele_Veto_preVFP_EGM2D.root";
        file_name_UL16postVFP = "egammaEffi.txt_Ele_Veto_postVFP_EGM2D.root";
        file_name_UL17 = "egammaEffi.txt_EGM2D_Veto_UL17.root";
        file_name_UL18 = "egammaEffi.txt_Ele_Veto_EGM2D.root";
        break;
      case Electron::tag::cutBasedElectronID_Fall17_94X_V2_loose :
        file_name_UL16preVFP = "egammaEffi.txt_Ele_Loose_preVFP_EGM2D.root";
        file_name_UL16postVFP = "egammaEffi.txt_Ele_Loose_postVFP_EGM2D.root";
        file_name_UL17 = "egammaEffi.txt_EGM2D_Loose_UL17.root";
        file_name_UL18 = "egammaEffi.txt_Ele_Loose_EGM2D.root";
        break;
      case Electron::tag::cutBasedElectronID_Fall17_94X_V2_medium :
        file_name_UL16preVFP = "egammaEffi.txt_Ele_Medium_preVFP_EGM2D.root";
        file_name_UL16postVFP = "egammaEffi.txt_Ele_Medium_postVFP_EGM2D.root";
        file_name_UL17 = "egammaEffi.txt_EGM2D_Medium_UL17.root";
        file_name_UL18 = "egammaEffi.txt_Ele_Medium_EGM2D.root";
        break;
      case Electron::tag::cutBasedElectronID_Fall17_94X_V2_tight :
        file_name_UL16preVFP = "egammaEffi.txt_Ele_Tight_preVFP_EGM2D.root";
        file_name_UL16postVFP = "egammaEffi.txt_Ele_Tight_postVFP_EGM2D.root";
        file_name_UL17 = "egammaEffi.txt_EGM2D_Tight_UL17.root";
        file_name_UL18 = "egammaEffi.txt_Ele_Tight_EGM2D.root";
        break;
      // case Electron::tag::mvaEleID_Fall17_noIso_V2_wp90 :
      //   file_name_UL16preVFP = "egammaEffi.txt_Ele_wp90noiso_preVFP_EGM2D.root";
      //   file_name_UL16postVFP = "egammaEffi.txt_Ele_wp90noiso_postVFP_EGM2D.root";
      //   file_name_UL17 = "egammaEffi.txt_EGM2D_MVA90noIso_UL17.root";
      //   file_name_UL18 = "egammaEffi.txt_Ele_wp90noiso_EGM2D.root";
      //   break;
      // case Electron::tag::mvaEleID_Fall17_noIso_V2_wp80 :
      //   file_name_UL16preVFP = "egammaEffi.txt_Ele_wp80noiso_preVFP_EGM2D.root";
      //   file_name_UL16postVFP = "egammaEffi.txt_Ele_wp80noiso_postVFP_EGM2D.root";
      //   file_name_UL17 = "egammaEffi.txt_EGM2D_MVA80noIso_UL17.root";
      //   file_name_UL18 = "egammaEffi.txt_Ele_wp80noiso_EGM2D.root";
      //   break;
      case Electron::tag::mvaEleID_Fall17_iso_V2_wp90 :
        file_name_UL16preVFP = "egammaEffi.txt_Ele_wp90iso_preVFP_EGM2D.root";
        file_name_UL16postVFP = "egammaEffi.txt_Ele_wp90iso_postVFP_EGM2D.root";
        file_name_UL17 = "egammaEffi.txt_EGM2D_MVA90iso_UL17.root";
        file_name_UL18 = "egammaEffi.txt_Ele_wp90iso_EGM2D.root";
        break;
      case Electron::tag::mvaEleID_Fall17_iso_V2_wp80 :
        file_name_UL16preVFP = "egammaEffi.txt_Ele_wp80iso_preVFP_EGM2D.root";
        file_name_UL16postVFP = "egammaEffi.txt_Ele_wp80iso_postVFP_EGM2D.root";
        file_name_UL17 = "egammaEffi.txt_EGM2D_MVA80iso_UL17.root";
        file_name_UL18 = "egammaEffi.txt_Ele_wp80iso_EGM2D.root";
        break;
      default :
        throw invalid_argument("ElectronIdScaleFactors: No scale factors implemented for given electron ID");
    }
    fYearSwitcher.reset(new YearSwitcher(ctx));
    fYearSwitcher->setupUL16preVFP(
      make_shared<MCElecScaleFactor>(
        ctx,
        base_file_path+"UL16preVFP/"+file_name_UL16preVFP,
        0.0,
        fWeightPostfix,
        syst_direction,
        fHandleName,
        "EGamma_SF2D",
        false
      )
    );
    fYearSwitcher->setupUL16postVFP(
      make_shared<MCElecScaleFactor>(
        ctx,
        base_file_path+"UL16postVFP/"+file_name_UL16postVFP,
        0.0,
        fWeightPostfix,
        syst_direction,
        fHandleName,
        "EGamma_SF2D",
        false
      )
    );
    fYearSwitcher->setupUL17(
      make_shared<MCElecScaleFactor>(
        ctx,
        base_file_path+"UL17/"+file_name_UL17,
        0.0,
        fWeightPostfix,
        syst_direction,
        fHandleName,
        "EGamma_SF2D",
        false
      )
    );
    fYearSwitcher->setupUL18(
      make_shared<MCElecScaleFactor>(
        ctx,
        base_file_path+"UL18/"+file_name_UL18,
        0.0,
        fWeightPostfix,
        syst_direction,
        fHandleName,
        "EGamma_SF2D",
        false
      )
    );
  }
  else {
    throw invalid_argument("ElectronIdScaleFactors: No electron ID provided");
  }
}

bool ElectronIdScaleFactors::process(Event & event) {
  if(fDummy) {
    for(uint i = 0; i < fWeightHandles.size(); i++) {
      event.set(fWeightHandles.at(i), -1.);
    }
  }
  else {
    if(fDoCheck) {
      for(const Electron & electron : event.get(fHandleElectrons)) {
        if(!fElectronID(electron, event)) throw runtime_error("ElectronIdScaleFactors::process(): Collection '"+fHandleName+"' contains an electron that does not fulfill the specific ID which scale factors are supposed to be applied for");
      }
    }
    fYearSwitcher->process(event);
  }
  return true;
}

//____________________________________________________________________________________________________
MuonIdScaleFactors::MuonIdScaleFactors(
  Context & ctx,
  const boost::optional<Muon::Selector> & selectorID,
  const boost::optional<bool> & do_check,
  const boost::optional<std::string> & weight_postfix,
  const boost::optional<std::string> & handle_name,
  const boost::optional<bool> & dummy
):
  fDoCheck(do_check ? *do_check : true),
  fWeightPostfix(weight_postfix ? *weight_postfix : "id"),
  fHandleName(handle_name ? *handle_name : "muons"),
  fDummy(dummy ? *dummy : false),
  fHandleMuons(ctx.get_handle<vector<Muon>>(fHandleName))
{
  const string syst_direction = ctx.get(fSystDirectionConfigName, "nominal");
  if(fDummy) { // These handle names must match the ones from the MCMuonScaleFactor class!
    fWeightHandles.push_back(ctx.declare_event_output<float>((string)"weight_sfmu_"+fWeightPostfix));
    fWeightHandles.push_back(ctx.declare_event_output<float>((string)"weight_sfmu_"+fWeightPostfix+"_up"));
    fWeightHandles.push_back(ctx.declare_event_output<float>((string)"weight_sfmu_"+fWeightPostfix+"_down"));
  }
  else if(selectorID) {
    fMuonID = MuonID(*selectorID);
    const string base_file_path = (string)getenv("CMSSW_BASE")+"/src/UHH2/"+kBasePathToULMuonSFs;
    string hist_name = "NUM_";
    switch(*selectorID) {
      case Muon::Selector::CutBasedIdGlobalHighPt :
        hist_name += "HighPtID";
        break;
      case Muon::Selector::CutBasedIdLoose :
        hist_name += "LooseID";
        break;
      case Muon::Selector::CutBasedIdMedium :
        hist_name += "MediumID";
        break;
      case Muon::Selector::CutBasedIdMediumPrompt :
        hist_name += "MediumPromptID";
        break;
      case Muon::Selector::SoftCutBasedId :
        hist_name += "SoftID";
        break;
      case Muon::Selector::CutBasedIdTight :
        hist_name += "TightID";
        break;
      case Muon::Selector::CutBasedIdTrkHighPt :
        hist_name += "TrkHighPtID";
        break;
      default :
        throw invalid_argument("MuonIdScaleFactors: No scale factors implemented for given muon ID");
    }
    hist_name += "_DEN_TrackerMuons_abseta_pt";
    fYearSwitcher.reset(new YearSwitcher(ctx));
    fYearSwitcher->setupUL16preVFP(
      make_shared<MCMuonScaleFactor>(
        ctx,
        base_file_path+"UL16preVFP/Efficiencies_muon_generalTracks_Z_Run2016_UL_HIPM_ID.root",
        hist_name,
        0.0,
        fWeightPostfix,
        false,
        syst_direction,
        fHandleName,
        true
      )
    );
    fYearSwitcher->setupUL16postVFP(
      make_shared<MCMuonScaleFactor>(
        ctx,
        base_file_path+"UL16postVFP/Efficiencies_muon_generalTracks_Z_Run2016_UL_ID.root",
        hist_name,
        0.0,
        fWeightPostfix,
        false,
        syst_direction,
        fHandleName,
        true
      )
    );
    fYearSwitcher->setupUL17(
      make_shared<MCMuonScaleFactor>(
        ctx,
        base_file_path+"UL17/Efficiencies_muon_generalTracks_Z_Run2017_UL_ID.root",
        hist_name,
        0.0,
        fWeightPostfix,
        false,
        syst_direction,
        fHandleName,
        true
      )
    );
    fYearSwitcher->setupUL18(
      make_shared<MCMuonScaleFactor>(
        ctx,
        base_file_path+"UL18/Efficiencies_muon_generalTracks_Z_Run2018_UL_ID.root",
        hist_name,
        0.0,
        fWeightPostfix,
        false,
        syst_direction,
        fHandleName,
        true
      )
    );
  }
  else {
    throw invalid_argument("MuonIdScaleFactors: No muon ID provided");
  }
}

bool MuonIdScaleFactors::process(Event & event) {
  if(fDummy) {
    for(uint i = 0; i < fWeightHandles.size(); i++) {
      event.set(fWeightHandles.at(i), -1.);
    }
  }
  else {
    if(fDoCheck) {
      for(const Muon & muon : event.get(fHandleMuons)) {
        if(!fMuonID(muon, event)) throw runtime_error("MuonIdScaleFactors::process(): Collection '"+fHandleName+"' contains a muon that does not fulfill the specific ID which scale factors are supposed to be applied for");
      }
    }
    fYearSwitcher->process(event);
  }
  return true;
}

//____________________________________________________________________________________________________
MuonIsoScaleFactors::MuonIsoScaleFactors(
  Context & ctx,
  const boost::optional<Muon::Selector> & selectorISO,
  const boost::optional<Muon::Selector> & selectorID,
  const boost::optional<bool> & do_check,
  const boost::optional<std::string> & weight_postfix,
  const boost::optional<std::string> & handle_name,
  const boost::optional<bool> & dummy
):
  fDoCheck(do_check ? *do_check : true),
  fWeightPostfix(weight_postfix ? *weight_postfix : "iso"),
  fHandleName(handle_name ? *handle_name : "muons"),
  fDummy(dummy ? *dummy : false),
  fHandleMuons(ctx.get_handle<vector<Muon>>(fHandleName))
{
  const string syst_direction = ctx.get(fSystDirectionConfigName, "nominal");
  if(fDummy) { // These handle names must match the ones from the MCMuonScaleFactor class!
    fWeightHandles.push_back(ctx.declare_event_output<float>((string)"weight_sfmu_"+fWeightPostfix));
    fWeightHandles.push_back(ctx.declare_event_output<float>((string)"weight_sfmu_"+fWeightPostfix+"_up"));
    fWeightHandles.push_back(ctx.declare_event_output<float>((string)"weight_sfmu_"+fWeightPostfix+"_down"));
  }
  else if(selectorISO && selectorID) {
    fMuonID = AndId<Muon>(MuonID(*selectorISO), MuonID(*selectorID));
    const string base_file_path = (string)getenv("CMSSW_BASE")+"/src/UHH2/"+kBasePathToULMuonSFs;
    string hist_name = "NUM_";
    if(*selectorISO == Muon::Selector::PFIsoLoose && *selectorID == Muon::Selector::CutBasedIdLoose) {
      hist_name += "LooseRelIso_DEN_LooseID";
    }
    else if(*selectorISO == Muon::Selector::PFIsoLoose && *selectorID == Muon::Selector::CutBasedIdMedium) {
      hist_name += "LooseRelIso_DEN_MediumID";
    }
    else if(*selectorISO == Muon::Selector::PFIsoLoose && *selectorID == Muon::Selector::CutBasedIdMediumPrompt) {
      hist_name += "LooseRelIso_DEN_MediumPromptID";
    }
    else if(*selectorISO == Muon::Selector::PFIsoLoose && *selectorID == Muon::Selector::CutBasedIdTight) {
      hist_name += "LooseRelIso_DEN_TightIDandIPCut";
    }
    else if(*selectorISO == Muon::Selector::TkIsoLoose && *selectorID == Muon::Selector::CutBasedIdGlobalHighPt) {
      hist_name += "LooseRelTkIso_DEN_HighPtIDandIPCut";
    }
    else if(*selectorISO == Muon::Selector::TkIsoLoose && *selectorID == Muon::Selector::CutBasedIdTrkHighPt) {
      hist_name += "LooseRelTkIso_DEN_TrkHighPtIDandIPCut";
    }
    else if(*selectorISO == Muon::Selector::PFIsoTight && *selectorID == Muon::Selector::CutBasedIdMedium) {
      hist_name += "TightRelIso_DEN_MediumID";
    }
    else if(*selectorISO == Muon::Selector::PFIsoTight && *selectorID == Muon::Selector::CutBasedIdMediumPrompt) {
      hist_name += "TightRelIso_DEN_MediumPromptID";
    }
    else if(*selectorISO == Muon::Selector::PFIsoTight && *selectorID == Muon::Selector::CutBasedIdTight) {
      hist_name += "TightRelIso_DEN_TightIDandIPCut";
    }
    else if(*selectorISO == Muon::Selector::TkIsoTight && *selectorID == Muon::Selector::CutBasedIdGlobalHighPt) {
      hist_name += "TightRelTkIso_DEN_HighPtIDandIPCut";
    }
    else if(*selectorISO == Muon::Selector::TkIsoTight && *selectorID == Muon::Selector::CutBasedIdTrkHighPt) {
      hist_name += "TightRelTkIso_DEN_TrkHighPtIDandIPCut";
    }
    else {
      throw invalid_argument("MuonIsoScaleFactors: No scale factors implemented for given combination of muon ID + ISO");
    }
    hist_name += "_abseta_pt";
    fYearSwitcher.reset(new YearSwitcher(ctx));
    fYearSwitcher->setupUL16preVFP(
      make_shared<MCMuonScaleFactor>(
        ctx,
        base_file_path+"UL16preVFP/Efficiencies_muon_generalTracks_Z_Run2016_UL_HIPM_ISO.root",
        hist_name,
        0.0,
        fWeightPostfix,
        false,
        syst_direction,
        fHandleName,
        true
      )
    );
    fYearSwitcher->setupUL16postVFP(
      make_shared<MCMuonScaleFactor>(
        ctx,
        base_file_path+"UL16postVFP/Efficiencies_muon_generalTracks_Z_Run2016_UL_ISO.root",
        hist_name,
        0.0,
        fWeightPostfix,
        false,
        syst_direction,
        fHandleName,
        true
      )
    );
    fYearSwitcher->setupUL17(
      make_shared<MCMuonScaleFactor>(
        ctx,
        base_file_path+"UL17/Efficiencies_muon_generalTracks_Z_Run2017_UL_ISO.root",
        hist_name,
        0.0,
        fWeightPostfix,
        false,
        syst_direction,
        fHandleName,
        true
      )
    );
    fYearSwitcher->setupUL18(
      make_shared<MCMuonScaleFactor>(
        ctx,
        base_file_path+"UL18/Efficiencies_muon_generalTracks_Z_Run2018_UL_ISO.root",
        hist_name,
        0.0,
        fWeightPostfix,
        false,
        syst_direction,
        fHandleName,
        true
      )
    );
  }
  else {
    throw invalid_argument("MuonIsoScaleFactors: No muon ISO and/or muon ID provided");
  }
}

bool MuonIsoScaleFactors::process(Event & event) {
  if(fDummy) {
    for(uint i = 0; i < fWeightHandles.size(); i++) {
      event.set(fWeightHandles.at(i), -1.);
    }
  }
  else {
    if(fDoCheck) {
      for(const Muon & muon : event.get(fHandleMuons)) {
        if(!fMuonID(muon, event)) throw runtime_error("MuonIsoScaleFactors::process(): Collection '"+fHandleName+"' contains a muon that does not fulfill the specific ISO/ID combination which scale factors are supposed to be applied for");
      }
    }
    fYearSwitcher->process(event);
  }
  return true;
}

//____________________________________________________________________________________________________
MuonTriggerScaleFactors::MuonTriggerScaleFactors(
  Context & ctx,
  const boost::optional<bool> & use_Mu50,
  const boost::optional<bool> & do_check,
  const boost::optional<std::string> & weight_postfix,
  const boost::optional<std::string> & handle_name,
  const boost::optional<bool> & absolute_eta,
  const boost::optional<bool> & dummy
):
  fUseMu50(use_Mu50),
  fDoCheck(do_check ? *do_check : true),
  fWeightPostfix(weight_postfix ? *weight_postfix : "trigger"),
  fHandleName(handle_name ? *handle_name : "muons"),
  fAbsEta(absolute_eta ? *absolute_eta : false),
  fDummy(dummy ? *dummy : false),
  fHandleMuons(ctx.get_handle<vector<Muon>>(fHandleName))
{
  const string syst_direction = ctx.get(fSystDirectionConfigName, "nominal");
  if(fDummy) { // These handle names must match the ones from the MCMuonScaleFactor class!
    fWeightHandles.push_back(ctx.declare_event_output<float>((string)"weight_sfmu_"+fWeightPostfix));
    fWeightHandles.push_back(ctx.declare_event_output<float>((string)"weight_sfmu_"+fWeightPostfix+"_up"));
    fWeightHandles.push_back(ctx.declare_event_output<float>((string)"weight_sfmu_"+fWeightPostfix+"_down"));
  }
  else if(fUseMu50) {
    const string base_file_path = (string)getenv("CMSSW_BASE")+"/src/UHH2/"+kBasePathToULMuonSFs;
    string hist_name_UL16preVFP;
    string hist_name_UL16postVFP;
    string hist_name_UL17;
    string hist_name_UL18;
    const string hist_name_extension = fAbsEta ? "_abseta_pt" : "_eta_pt";
    const Year year = extract_year(ctx);
    fTriggerSelection.reset(new OrSelection());
    switch(*fUseMu50) {
      case true :
        hist_name_UL16preVFP = "NUM_Mu50_or_TkMu50_DEN_CutBasedIdGlobalHighPt_and_TkIsoLoose";
        hist_name_UL16postVFP = "NUM_Mu50_or_TkMu50_DEN_CutBasedIdGlobalHighPt_and_TkIsoLoose";
        hist_name_UL17 = "NUM_Mu50_or_OldMu100_or_TkMu100_DEN_CutBasedIdGlobalHighPt_and_TkIsoLoose";
        hist_name_UL18 = "NUM_Mu50_or_OldMu100_or_TkMu100_DEN_CutBasedIdGlobalHighPt_and_TkIsoLoose";
        if(year == Year::isUL16preVFP || year == Year::isUL16postVFP) {
          fTriggerSelection->add(make_shared<TriggerSelection>("HLT_Mu50_v*"));
          fTriggerSelection->add(make_shared<TriggerSelection>("HLT_TkMu50_v*"));
        }
        else if(year == Year::isUL17 || year == Year::isUL18) {
          fTriggerSelection->add(make_shared<TriggerSelection>("HLT_Mu50_v*"));
          fTriggerSelection->add(make_shared<TriggerSelection>("HLT_OldMu100_v*"));
          fTriggerSelection->add(make_shared<TriggerSelection>("HLT_TkMu100_v*"));
        }
        fMuonID = AndId<Muon>(MuonID(Muon::Selector::CutBasedIdGlobalHighPt), MuonID(Muon::Selector::TkIsoLoose));
        break;
      case false :
        hist_name_UL16preVFP = "NUM_IsoMu24_or_IsoTkMu24_DEN_CutBasedIdTight_and_PFIsoTight";
        hist_name_UL16postVFP = "NUM_IsoMu24_or_IsoTkMu24_DEN_CutBasedIdTight_and_PFIsoTight";
        hist_name_UL17 = "NUM_IsoMu27_DEN_CutBasedIdTight_and_PFIsoTight";
        hist_name_UL18 = "NUM_IsoMu24_DEN_CutBasedIdTight_and_PFIsoTight";
        if(year == Year::isUL16preVFP || year == Year::isUL16postVFP) {
          fTriggerSelection->add(make_shared<TriggerSelection>("HLT_IsoMu24_v*"));
          fTriggerSelection->add(make_shared<TriggerSelection>("HLT_IsoTkMu24_v*"));
        }
        else if(year == Year::isUL17) {
          fTriggerSelection->add(make_shared<TriggerSelection>("HLT_IsoMu27_v*"));
        }
        else if(year == Year::isUL18) {
          fTriggerSelection->add(make_shared<TriggerSelection>("HLT_IsoMu24_v*"));
        }
        fMuonID = AndId<Muon>(MuonID(Muon::Selector::CutBasedIdTight), MuonID(Muon::Selector::PFIsoTight));
        break;
    }
    fYearSwitcher.reset(new YearSwitcher(ctx));
    fYearSwitcher->setupUL16preVFP(
      make_shared<MCMuonScaleFactor>(
        ctx,
        base_file_path+"UL16preVFP/Efficiencies_muon_generalTracks_Z_Run2016_UL_HIPM_SingleMuonTriggers.root",
        hist_name_UL16preVFP+hist_name_extension,
        0.0,
        fWeightPostfix,
        false,
        syst_direction,
        fHandleName,
        fAbsEta
      )
    );
    fYearSwitcher->setupUL16postVFP(
      make_shared<MCMuonScaleFactor>(
        ctx,
        base_file_path+"UL16postVFP/Efficiencies_muon_generalTracks_Z_Run2016_UL_SingleMuonTriggers.root",
        hist_name_UL16postVFP+hist_name_extension,
        0.0,
        fWeightPostfix,
        false,
        syst_direction,
        fHandleName,
        fAbsEta
      )
    );
    fYearSwitcher->setupUL17(
      make_shared<MCMuonScaleFactor>(
        ctx,
        base_file_path+"UL17/Efficiencies_muon_generalTracks_Z_Run2017_UL_SingleMuonTriggers.root",
        hist_name_UL17+hist_name_extension,
        0.0,
        fWeightPostfix,
        false,
        syst_direction,
        fHandleName,
        fAbsEta
      )
    );
    fYearSwitcher->setupUL18(
      make_shared<MCMuonScaleFactor>(
        ctx,
        base_file_path+"UL18/Efficiencies_muon_generalTracks_Z_Run2018_UL_SingleMuonTriggers.root",
        hist_name_UL18+hist_name_extension,
        0.0,
        fWeightPostfix,
        false,
        syst_direction,
        fHandleName,
        fAbsEta
      )
    );
  }
  else {
    throw invalid_argument("MuonTriggerScaleFactors: Trigger path not specified");
  }
}

bool MuonTriggerScaleFactors::process(Event & event) {
  if(fDummy) {
    for(uint i = 0; i < fWeightHandles.size(); i++) {
      event.set(fWeightHandles.at(i), -1.);
    }
  }
  else {
    if(fDoCheck) {
      if(!fTriggerSelection->passes(event)) throw runtime_error("MuonTriggerScaleFactors::process(): Event does not pass trigger selection which scale factors are supposed to be applied for");
      for(const Muon & muon : event.get(fHandleMuons)) {
        if(!fMuonID(muon, event)) throw runtime_error("MuonTriggerScaleFactors::process(): Collection '"+fHandleName+"' contains a muon that does not fulfill the ID/ISO combination which the scale factors were derived with");
      }
    }
    fYearSwitcher->process(event);
  }
  return true;
}

}
