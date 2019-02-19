#include <UHH2/common/include/ElectronIds.h>

bool Electron_MINIIso::operator()(const Electron& ele, const uhh2::Event&) const {

  float iso(-1.);

  if(!ele.pt()) throw std::runtime_error("Electron_MINIIso::operator() -- null electron transverse momentum: failed to calculate relative MINI-Isolation");

  if     (iso_key_ == "uncorrected") iso = (ele.pfMINIIso_CH() + ele.pfMINIIso_NH() + ele.pfMINIIso_Ph())/ele.pt();
  else if(iso_key_ == "delta-beta")  iso = (ele.pfMINIIso_CH() + std::max(0., ele.pfMINIIso_NH() + ele.pfMINIIso_Ph() - .5*ele.pfMINIIso_PU()))/ele.pt();
  else if(iso_key_ == "pf-weight")   iso = (ele.pfMINIIso_CH() + ele.pfMINIIso_NH_pfwgt() + ele.pfMINIIso_Ph_pfwgt())/ele.pt();
  else throw std::runtime_error("Electron_MINIIso::operator() -- invalid key for MINI-Isolation pileup correction: "+iso_key_);

  return (iso < iso_cut_);
}

//// Cut-Based ID

bool Electron_CutBasedID(const Electron& ele_, const uhh2::Event& evt_, const std::string& tuning_, const std::string& wp_, const bool apply_iso_cut_){

  bool pass(false);

  int wp_idx(-1);
  if     (wp_ == "VETO"  ) wp_idx = 0;
  else if(wp_ == "LOOSE" ) wp_idx = 1;
  else if(wp_ == "MEDIUM") wp_idx = 2;
  else if(wp_ == "TIGHT" ) wp_idx = 3;
  else throw std::runtime_error("Electron_CutBasedID -- undefined working-point tag: "+wp_);

  const float abs_etaSC(fabs(ele_.supercluster_eta()));

  if     (                     abs_etaSC <= 1.479) pass = Electron_CutBasedID(ele_, evt_, tuning_, "barrel", wp_idx, apply_iso_cut_);
  else if(1.479 < abs_etaSC && abs_etaSC <  2.5  ) pass = Electron_CutBasedID(ele_, evt_, tuning_, "endcap", wp_idx, apply_iso_cut_);

  return pass;
}

bool Electron_CutBasedID(const Electron& ele_, const uhh2::Event& evt_, const std::string& tuning_, const std::string& eleSC_pos_, const int wp_idx_, const bool apply_iso_cut_){

  assert(evt_.pvs);
  if(!evt_.pvs->size()) return false;
  const auto& pv = (*evt_.pvs)[0];

  const float abs_d0 = fabs(ele_.gsfTrack_dxy_vertex(pv.x(), pv.y()));
  const float abs_dz = fabs(ele_.gsfTrack_dz_vertex (pv.x(), pv.y(), pv.z()));

  if(ele_.EcalEnergy() <= 0.) return false;
  const float abs_ooEmooP = fabs(1./ele_.EcalEnergy() - ele_.EoverPIn()/ele_.EcalEnergy());

  /*float C0 = -1.;
  const float C0_barrel = ele_.HoverE() - 1.12/ele_.EcalEnergy() - (0.0368*evt_.rho)/ele_.EcalEnergy();
  const float C0_endcap = ele_.HoverE() - 0.5/ele_.EcalEnergy() - (0.201*evt_.rho)/ele_.EcalEnergy();
  if(eleSC_pos_ == "barrel") C0 = C0_barrel;
  else if(eleSC_pos_ == "endcap") C0 = C0_endcap;
  else throw std::runtime_error("Invalid value for variable eleSC_pos_. May be 'barrel' or 'endcap'.");
  */
  float C0 = ElectronID::CutBased_LUT.at(tuning_).at(eleSC_pos_).at("HoverE_C0").at(wp_idx_);
  float C1 = ElectronID::CutBased_LUT.at(tuning_).at(eleSC_pos_).at("HoverE_C1").at(wp_idx_);
  float C2 = ElectronID::CutBased_LUT.at(tuning_).at(eleSC_pos_).at("HoverE_C2").at(wp_idx_);
  float HoverE_cut = C0+C1/ele_.EcalEnergy()+C2*evt_.rho/ele_.EcalEnergy();

  const int expMissingHits = ele_.gsfTrack_trackerExpectedHitsInner_numberOfLostHits();

  const int passConvVeto = int(ele_.passconversionveto());

  if(!( ele_.sigmaIEtaIEta() < ElectronID::CutBased_LUT.at(tuning_).at(eleSC_pos_).at("sigmaIetaIeta").at(wp_idx_)  )) return false;// sigmaIetaIeta
  if(!( fabs(ele_.dEtaIn())  < ElectronID::CutBased_LUT.at(tuning_).at(eleSC_pos_).at("|dEtaIn|")     .at(wp_idx_)  )) return false;// |dEtaIn|
  if(!( fabs(ele_.dPhiIn())  < ElectronID::CutBased_LUT.at(tuning_).at(eleSC_pos_).at("|dPhiIn|")     .at(wp_idx_)  )) return false;// |dPhiIn|
  if(!( ele_.HoverE()        < HoverE_cut)) return false;// HoverE
  if(!( abs_ooEmooP          < ElectronID::CutBased_LUT.at(tuning_).at(eleSC_pos_).at("|ooEmooP|")    .at(wp_idx_)  )) return false;// |ooEmooP|
  if(!( abs_d0               < ElectronID::CutBased_LUT.at(tuning_).at(eleSC_pos_).at("|d0|")         .at(wp_idx_)  )) return false;// |d0|
  if(!( abs_dz               < ElectronID::CutBased_LUT.at(tuning_).at(eleSC_pos_).at("|dz|")         .at(wp_idx_)  )) return false;// |dz|
  if(!( expMissingHits  <= int(ElectronID::CutBased_LUT.at(tuning_).at(eleSC_pos_).at("missingHits")  .at(wp_idx_)) )) return false;// expected missing inner hits
  if(!( passConvVeto    >= int(ElectronID::CutBased_LUT.at(tuning_).at(eleSC_pos_).at("passConvVeto") .at(wp_idx_)) )) return false;// conversion veto

  if(apply_iso_cut_){
    float relIso_C0 = ElectronID::CutBased_LUT.at(tuning_).at(eleSC_pos_).at("relIsoEA_C0").at(wp_idx_);
    float relIso_C1 = ElectronID::CutBased_LUT.at(tuning_).at(eleSC_pos_).at("relIsoEA_C1").at(wp_idx_);
    float relIso_cut = relIso_C0 + relIso_C1/ele_.pt();

    const float pfIsoEA = ele_.relIsorho(evt_.rho);
    if(!( pfIsoEA            < relIso_cut )) return false;// pfIso (PU correction: effective-area)
  }

  return true;
}

// --- Cut-Based ID: Summer16
bool ElectronID_Summer16_veto        (const Electron& ele, const uhh2::Event& evt){ return Electron_CutBasedID(ele, evt, "Summer16", "VETO"  , true) ; }
bool ElectronID_Summer16_veto_noIso  (const Electron& ele, const uhh2::Event& evt){ return Electron_CutBasedID(ele, evt, "Summer16", "VETO"  , false); }

bool ElectronID_Summer16_loose       (const Electron& ele, const uhh2::Event& evt){ return Electron_CutBasedID(ele, evt, "Summer16", "LOOSE" , true) ; }
bool ElectronID_Summer16_loose_noIso (const Electron& ele, const uhh2::Event& evt){ return Electron_CutBasedID(ele, evt, "Summer16", "LOOSE" , false); }

bool ElectronID_Summer16_medium      (const Electron& ele, const uhh2::Event& evt){ return Electron_CutBasedID(ele, evt, "Summer16", "MEDIUM", true) ; }
bool ElectronID_Summer16_medium_noIso(const Electron& ele, const uhh2::Event& evt){ return Electron_CutBasedID(ele, evt, "Summer16", "MEDIUM", false); }

bool ElectronID_Summer16_tight       (const Electron& ele, const uhh2::Event& evt){ return Electron_CutBasedID(ele, evt, "Summer16", "TIGHT" , true) ; }
bool ElectronID_Summer16_tight_noIso (const Electron& ele, const uhh2::Event& evt){ return Electron_CutBasedID(ele, evt, "Summer16", "TIGHT" , false); }

// --- Cut-Based ID: Fall17
bool ElectronID_Fall17_veto        (const Electron& ele, const uhh2::Event& evt){ return Electron_CutBasedID(ele, evt, "Fall17", "VETO"  , true) ; }
bool ElectronID_Fall17_veto_noIso  (const Electron& ele, const uhh2::Event& evt){ return Electron_CutBasedID(ele, evt, "Fall17", "VETO"  , false); }

bool ElectronID_Fall17_loose       (const Electron& ele, const uhh2::Event& evt){ return Electron_CutBasedID(ele, evt, "Fall17", "LOOSE" , true) ; }
bool ElectronID_Fall17_loose_noIso (const Electron& ele, const uhh2::Event& evt){ return Electron_CutBasedID(ele, evt, "Fall17", "LOOSE" , false); }

bool ElectronID_Fall17_medium      (const Electron& ele, const uhh2::Event& evt){ return Electron_CutBasedID(ele, evt, "Fall17", "MEDIUM", true) ; }
bool ElectronID_Fall17_medium_noIso(const Electron& ele, const uhh2::Event& evt){ return Electron_CutBasedID(ele, evt, "Fall17", "MEDIUM", false); }

bool ElectronID_Fall17_tight       (const Electron& ele, const uhh2::Event& evt){ return Electron_CutBasedID(ele, evt, "Fall17", "TIGHT" , true) ; }
bool ElectronID_Fall17_tight_noIso (const Electron& ele, const uhh2::Event& evt){ return Electron_CutBasedID(ele, evt, "Fall17", "TIGHT" , false); }

////

//// General Purpose MVA ID

bool Electron_MVAID(const Electron& ele_, const uhh2::Event&, const std::string& tuning_, const std::string& wp_, const bool iso_){

  std::string category("");

  const float pt(ele_.pt()), abs_etaSC(fabs(ele_.supercluster_eta()));
  const float ebSplit = 0.8;
  const float ebeeSplit = 1.479;

  if (abs_etaSC > 2.5) return false;
  if (pt > 10.){
    if       (abs_etaSC < ebSplit)  category = "high-pt_barrel1";
    else if (abs_etaSC < ebeeSplit) category = "high-pt_barrel2";
    else                            category = "high-pt_endcap";
  } else if (pt > 5.) {
    if       (abs_etaSC < ebSplit)  category = "low-pt_barrel1";
    else if (abs_etaSC < ebeeSplit) category = "low-pt_barrel2";
    else                            category = "low-pt_endcap";
  }
  else return false;

  int wp_idx(-1);
  if (wp_ == "loose") wp_idx = 0;
  else throw std::runtime_error("Electron_MVAID -- undefined working-point tag: "+wp_);

  // FIXME: better way to handle this...?
  if (iso_) {
    const float MVA = ele_.mvaIso();
    return (MVA > ElectronID::MVA_LUT_Iso.at(tuning_).at(category).at(wp_idx));
  } else {
    const float MVA = ele_.mvaNoIso();
    return (MVA > ElectronID::MVA_LUT_NoIso.at(tuning_).at(category).at(wp_idx));
  }
}

bool ElectronID_MVA_Fall17_loose_iso(const Electron& ele, const uhh2::Event& evt){ return Electron_MVAID(ele, evt, "Fall17", "loose", true); }
bool ElectronID_MVA_Fall17_loose_noIso(const Electron& ele, const uhh2::Event& evt){ return Electron_MVAID(ele, evt, "Fall17", "loose", false); }

//// HEEP ID

bool ElectronID_HEEP_RunII_25ns(const Electron& ele, const uhh2::Event&){ return Electron_HEEP(ele, "RunII_25ns", "CMS_WorkPoint_NoIso"); }

bool Electron_HEEP(const Electron& ele_, const std::string& tuning_, const std::string& wp_){

  bool pass(false);

  int wp_idx(-1);
  if(wp_ == "CMS_WorkPoint_NoIso") wp_idx = 0;
  else throw std::runtime_error("Electron_HEEP -- undefined working-point tag: "+wp_);

  const float abs_etaSC(fabs(ele_.supercluster_eta()));

  if     (                     abs_etaSC < 1.4442) pass = Electron_HEEP(ele_, tuning_, "barrel", wp_idx);
  else if(1.566 < abs_etaSC && abs_etaSC < 2.5   ) pass = Electron_HEEP(ele_, tuning_, "endcap", wp_idx); 
  else                                             pass = false;

  return pass;
}

bool Electron_HEEP(const Electron& ele_, const std::string& tuning_, const std::string& eleSC_pos_, const int wp_idx_){

  const float Et            = ele_.pt();
  const float dEtaInSeed    = ele_.dEtaInSeed();
  const float dPhiIn        = ele_.dPhiIn();
  const float HoverE        = ele_.HoverE();
  const float sigmaIEtaIEta = ele_.sigmaIEtaIEta();
  const float E25over55     = ele_.full5x5_e5x5() ? ele_.full5x5_e2x5Max()/ele_.full5x5_e5x5() : -1;
  const float E15over55     = ele_.full5x5_e5x5() ? ele_.full5x5_e1x5()   /ele_.full5x5_e5x5() : -1;
  const float misshits      = ele_.gsfTrack_trackerExpectedHitsInner_numberOfLostHits();
  const float dxy           = fabs(ele_.dxy());

  float HoverE_threshold(-1);
  if(ele_.energy()){

    HoverE_threshold  = ElectronID::HEEP_LUT.at(tuning_).at(eleSC_pos_).at("HoverE coef") .at(wp_idx_) * (1./ele_.energy());
    HoverE_threshold += ElectronID::HEEP_LUT.at(tuning_).at(eleSC_pos_).at("HoverE const").at(wp_idx_);
  }

//  /* debugging */
//  std::cout << std::endl;
//  std::cout << "Et"            << " = " << Et            << std::endl;
//  std::cout << "dEtaInSeed"    << " = " << dEtaInSeed    << std::endl;
//  std::cout << "dPhiIn"        << " = " << dPhiIn        << std::endl;
//  std::cout << "HoverE"        << " = " << HoverE        << std::endl;
//  std::cout << "sigmaIEtaIEta" << " = " << sigmaIEtaIEta << std::endl;
//  std::cout << "E25over55"     << " = " << E25over55     << std::endl;
//  std::cout << "E15over55"     << " = " << E15over55     << std::endl;
//  std::cout << "misshits"      << " = " << misshits      << std::endl;
//  std::cout << "dxy"           << " = " << dxy           << std::endl;
//  std::cout << std::endl;
//  /*************/

  if(!ele_.isEcalDriven()) return false;

  if(!( Et               >  ElectronID::HEEP_LUT.at(tuning_).at(eleSC_pos_).at("Et")           .at(wp_idx_) )) return false;
  if(!( fabs(dEtaInSeed) <  ElectronID::HEEP_LUT.at(tuning_).at(eleSC_pos_).at("|dEtaInSeed|") .at(wp_idx_) )) return false;
  if(!( fabs(dPhiIn)     <  ElectronID::HEEP_LUT.at(tuning_).at(eleSC_pos_).at("|dPhiIn|")     .at(wp_idx_) )) return false;
  if(!( sigmaIEtaIEta    <  ElectronID::HEEP_LUT.at(tuning_).at(eleSC_pos_).at("sigmaIEtaIEta").at(wp_idx_) )) return false;
  if(!( misshits         <= ElectronID::HEEP_LUT.at(tuning_).at(eleSC_pos_).at("missingHits")  .at(wp_idx_) )) return false;
  if(!( fabs(dxy)        <  ElectronID::HEEP_LUT.at(tuning_).at(eleSC_pos_).at("|dxy|")        .at(wp_idx_) )) return false;

  if(!( HoverE           < HoverE_threshold )) return false;

  const bool E25over55_pass = (E25over55 > ElectronID::HEEP_LUT.at(tuning_).at(eleSC_pos_).at("E2x5 over E5x5").at(wp_idx_));
  const bool E15over55_pass = (E15over55 > ElectronID::HEEP_LUT.at(tuning_).at(eleSC_pos_).at("E1x5 over E5x5").at(wp_idx_));
  if(!( E25over55_pass || E15over55_pass )) return false;

  return true;
}
