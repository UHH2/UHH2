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

  const int expMissingHits = ele_.gsfTrack_trackerExpectedHitsInner_numberOfLostHits();

  const int passConvVeto = int(ele_.passconversionveto());

  if(!( ele_.sigmaIEtaIEta() < ElectronID::CutBased_LUT.at(tuning_).at(eleSC_pos_).at("sigmaIetaIeta").at(wp_idx_)  )) return false;// sigmaIetaIeta
  if(!( fabs(ele_.dEtaIn())  < ElectronID::CutBased_LUT.at(tuning_).at(eleSC_pos_).at("|dEtaIn|")     .at(wp_idx_)  )) return false;// |dEtaIn|
  if(!( fabs(ele_.dPhiIn())  < ElectronID::CutBased_LUT.at(tuning_).at(eleSC_pos_).at("|dPhiIn|")     .at(wp_idx_)  )) return false;// |dPhiIn|
  if(!( ele_.HoverE()        < ElectronID::CutBased_LUT.at(tuning_).at(eleSC_pos_).at("HoverE")       .at(wp_idx_)  )) return false;// HoverE
  if(!( abs_ooEmooP          < ElectronID::CutBased_LUT.at(tuning_).at(eleSC_pos_).at("|ooEmooP|")    .at(wp_idx_)  )) return false;// |ooEmooP|
  if(!( abs_d0               < ElectronID::CutBased_LUT.at(tuning_).at(eleSC_pos_).at("|d0|")         .at(wp_idx_)  )) return false;// |d0|
  if(!( abs_dz               < ElectronID::CutBased_LUT.at(tuning_).at(eleSC_pos_).at("|dz|")         .at(wp_idx_)  )) return false;// |dz|
  if(!( expMissingHits  <= int(ElectronID::CutBased_LUT.at(tuning_).at(eleSC_pos_).at("missingHits")  .at(wp_idx_)) )) return false;// expected missing inner hits
  if(!( passConvVeto    >= int(ElectronID::CutBased_LUT.at(tuning_).at(eleSC_pos_).at("passConvVeto") .at(wp_idx_)) )) return false;// conversion veto

  if(apply_iso_cut_){

    const float pfIsoEA = ele_.relIsorho(evt_.rho);
    if(!( pfIsoEA            < ElectronID::CutBased_LUT.at(tuning_).at(eleSC_pos_).at("relIsoEA")     .at(wp_idx_)  )) return false;// pfIso (PU correction: effective-area)
  }

  return true;
}

// --- Cut-Based ID: PHYS14 25ns
bool ElectronID_PHYS14_25ns_veto        (const Electron& ele, const uhh2::Event& evt){ return Electron_CutBasedID(ele, evt, "PHYS14_25ns", "VETO"  , true) ; }
bool ElectronID_PHYS14_25ns_veto_noIso  (const Electron& ele, const uhh2::Event& evt){ return Electron_CutBasedID(ele, evt, "PHYS14_25ns", "VETO"  , false); }

bool ElectronID_PHYS14_25ns_loose       (const Electron& ele, const uhh2::Event& evt){ return Electron_CutBasedID(ele, evt, "PHYS14_25ns", "LOOSE" , true) ; }
bool ElectronID_PHYS14_25ns_loose_noIso (const Electron& ele, const uhh2::Event& evt){ return Electron_CutBasedID(ele, evt, "PHYS14_25ns", "LOOSE" , false); }

bool ElectronID_PHYS14_25ns_medium      (const Electron& ele, const uhh2::Event& evt){ return Electron_CutBasedID(ele, evt, "PHYS14_25ns", "MEDIUM", true) ; }
bool ElectronID_PHYS14_25ns_medium_noIso(const Electron& ele, const uhh2::Event& evt){ return Electron_CutBasedID(ele, evt, "PHYS14_25ns", "MEDIUM", false); }

bool ElectronID_PHYS14_25ns_tight       (const Electron& ele, const uhh2::Event& evt){ return Electron_CutBasedID(ele, evt, "PHYS14_25ns", "TIGHT" , true) ; }
bool ElectronID_PHYS14_25ns_tight_noIso (const Electron& ele, const uhh2::Event& evt){ return Electron_CutBasedID(ele, evt, "PHYS14_25ns", "TIGHT" , false); }

// --- Cut-Based ID: Spring15 50ns
bool ElectronID_Spring15_50ns_veto        (const Electron& ele, const uhh2::Event& evt){ return Electron_CutBasedID(ele, evt, "Spring15_50ns", "VETO"  , true) ; }
bool ElectronID_Spring15_50ns_veto_noIso  (const Electron& ele, const uhh2::Event& evt){ return Electron_CutBasedID(ele, evt, "Spring15_50ns", "VETO"  , false); }

bool ElectronID_Spring15_50ns_loose       (const Electron& ele, const uhh2::Event& evt){ return Electron_CutBasedID(ele, evt, "Spring15_50ns", "LOOSE" , true) ; }
bool ElectronID_Spring15_50ns_loose_noIso (const Electron& ele, const uhh2::Event& evt){ return Electron_CutBasedID(ele, evt, "Spring15_50ns", "LOOSE" , false); }

bool ElectronID_Spring15_50ns_medium      (const Electron& ele, const uhh2::Event& evt){ return Electron_CutBasedID(ele, evt, "Spring15_50ns", "MEDIUM", true) ; }
bool ElectronID_Spring15_50ns_medium_noIso(const Electron& ele, const uhh2::Event& evt){ return Electron_CutBasedID(ele, evt, "Spring15_50ns", "MEDIUM", false); }

bool ElectronID_Spring15_50ns_tight       (const Electron& ele, const uhh2::Event& evt){ return Electron_CutBasedID(ele, evt, "Spring15_50ns", "TIGHT" , true) ; }
bool ElectronID_Spring15_50ns_tight_noIso (const Electron& ele, const uhh2::Event& evt){ return Electron_CutBasedID(ele, evt, "Spring15_50ns", "TIGHT" , false); }

// --- Cut-Based ID: Spring15 25ns
bool ElectronID_Spring15_25ns_veto        (const Electron& ele, const uhh2::Event& evt){ return Electron_CutBasedID(ele, evt, "Spring15_25ns", "VETO"  , true) ; }
bool ElectronID_Spring15_25ns_veto_noIso  (const Electron& ele, const uhh2::Event& evt){ return Electron_CutBasedID(ele, evt, "Spring15_25ns", "VETO"  , false); }

bool ElectronID_Spring15_25ns_loose       (const Electron& ele, const uhh2::Event& evt){ return Electron_CutBasedID(ele, evt, "Spring15_25ns", "LOOSE" , true) ; }
bool ElectronID_Spring15_25ns_loose_noIso (const Electron& ele, const uhh2::Event& evt){ return Electron_CutBasedID(ele, evt, "Spring15_25ns", "LOOSE" , false); }

bool ElectronID_Spring15_25ns_medium      (const Electron& ele, const uhh2::Event& evt){ return Electron_CutBasedID(ele, evt, "Spring15_25ns", "MEDIUM", true) ; }
bool ElectronID_Spring15_25ns_medium_noIso(const Electron& ele, const uhh2::Event& evt){ return Electron_CutBasedID(ele, evt, "Spring15_25ns", "MEDIUM", false); }

bool ElectronID_Spring15_25ns_tight       (const Electron& ele, const uhh2::Event& evt){ return Electron_CutBasedID(ele, evt, "Spring15_25ns", "TIGHT" , true) ; }
bool ElectronID_Spring15_25ns_tight_noIso (const Electron& ele, const uhh2::Event& evt){ return Electron_CutBasedID(ele, evt, "Spring15_25ns", "TIGHT" , false); }
////

//// General Purpose MVA ID

bool Electron_GeneralPurposeMVAID(const Electron& ele_, const uhh2::Event&, const std::string& tuning_, const std::string& wp_){

  std::string category("");

  const float pt(ele_.pt()), abs_etaSC(fabs(ele_.supercluster_eta()));
  if(pt > 10.){

    if                         (abs_etaSC < 0.8)   category = "high-pt_barrel1";
    else if(0.8 <= abs_etaSC && abs_etaSC < 1.479) category = "high-pt_barrel2";
    else if                    (abs_etaSC < 2.5)   category = "high-pt_endcap";
  }
  else return false;

  int wp_idx(-1);
  if     (wp_ == "80p_sigeff") wp_idx = 0;
  else if(wp_ == "90p_sigeff") wp_idx = 1;
  else throw std::runtime_error("Electron_GeneralPurposeMVAID -- undefined working-point tag: "+wp_);

  const float MVA(ele_.mvaGeneralPurpose());

  return (MVA > ElectronID::GeneralPurposeMVA_LUT.at(tuning_).at(category).at(wp_idx));
}

bool ElectronID_MVAGeneralPurpose_Spring16_loose(const Electron& ele, const uhh2::Event& evt){ return Electron_GeneralPurposeMVAID(ele, evt, "Spring16", "90p_sigeff"); }
bool ElectronID_MVAGeneralPurpose_Spring16_tight(const Electron& ele, const uhh2::Event& evt){ return Electron_GeneralPurposeMVAID(ele, evt, "Spring16", "80p_sigeff"); }


//// HZZ MVA ID

bool Electron_HZZMVAID(const Electron& ele_, const uhh2::Event&, const std::string& tuning_, const std::string& wp_){ 

  std::string category("");
  const float pt(ele_.pt()), abs_etaSC(fabs(ele_.supercluster_eta()));
  if(5. < pt && pt <= 10.){ 
    if                         (abs_etaSC < 0.8)   category = "low-pt_barrel1";
    else if(0.8 <= abs_etaSC && abs_etaSC < 1.479) category = "low-pt_barrel2";
    else if                    (abs_etaSC < 2.5)   category = "low-pt_endcap";
  }
  if(pt > 10.){
    if                         (abs_etaSC < 0.8)   category = "high-pt_barrel1";
    else if(0.8 <= abs_etaSC && abs_etaSC < 1.479) category = "high-pt_barrel2";
    else if                    (abs_etaSC < 2.5)   category = "high-pt_endcap";
  }
  else return false; 

  int wp_idx(-1);
  if     (wp_ == "98p_sigeff") wp_idx = 0;
  else throw std::runtime_error("Electron_HZZPurposeMVAID -- undefined working-point tag: "+wp_);

  const float MVA(ele_.mvaHZZ());

  return (MVA > ElectronID::HZZMVA_LUT.at(tuning_).at(category).at(wp_idx));

}

bool ElectronID_MVAHZZ_Spring16_loose(const Electron& ele, const uhh2::Event& evt){ return Electron_HZZMVAID(ele, evt, "Spring16", "98p_sigeff"); }

////

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
