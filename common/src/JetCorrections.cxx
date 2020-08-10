#include "UHH2/common/include/JetCorrections.h"
#include "UHH2/common/include/JetCorrectionSets.h"
#include "UHH2/common/include/Utils.h"
#include "UHH2/core/include/Utils.h"

#include "UHH2/JetMETObjects/interface/FactorizedJetCorrector.h"
#include "UHH2/JetMETObjects/interface/JetCorrectorParameters.h"

#include <string>
#include <cmath>

using namespace std;
using namespace uhh2;


void correct_jet(FactorizedJetCorrector & corrector, Jet & jet, const Event & event, JetCorrectionUncertainty* jec_unc, int jec_unc_direction){
  auto factor_raw = jet.JEC_factor_raw();
  corrector.setJetPt(jet.pt() * factor_raw);
  corrector.setJetEta(jet.eta());
  corrector.setJetE(jet.energy() * factor_raw);
  corrector.setJetA(jet.jetArea());
  corrector.setJetPhi(jet.phi());
  corrector.setRho(event.rho);
  auto correctionfactors = corrector.getSubCorrections();
  auto correctionfactor_L1  = correctionfactors.front();
  auto correctionfactor = correctionfactors.back();

  LorentzVector jet_v4_corrected = jet.v4() * (factor_raw *correctionfactor);

  if(jec_unc_direction!=0){
    if (jec_unc==NULL){
      std::cerr << "JEC variation should be applied, but JEC uncertainty object is NULL! Abort." << std::endl;
      exit(EXIT_FAILURE);
    }
    // ignore jets with very low pt or high eta, avoiding a crash from the JESUncertainty tool
    double pt = jet_v4_corrected.Pt();
    double eta = jet_v4_corrected.Eta();
    if (!(pt<5. || fabs(eta)>5.)) {

      jec_unc->setJetEta(eta);
      jec_unc->setJetPt(pt);

      double unc = 0.;
      if (jec_unc_direction == 1){
        unc = jec_unc->getUncertainty(true);
        correctionfactor *= (1 + fabs(unc));
      } else if (jec_unc_direction == -1){
        unc = jec_unc->getUncertainty(false);
        correctionfactor *= (1 - fabs(unc));
      }
      jet_v4_corrected = jet.v4() * (factor_raw *correctionfactor);
    }
  }


  jet.set_v4(jet_v4_corrected);
  jet.set_JEC_factor_raw(1. / correctionfactor);
  jet.set_JEC_L1factor_raw(correctionfactor_L1);

}


namespace {

  // to share some code between JetCorrector and JetLeptonCleaner, provide some methods
  // dealing with jet energy corrections here:
  std::unique_ptr<FactorizedJetCorrector> build_corrector(const std::vector<std::string> & filenames){
    std::vector<JetCorrectorParameters> pars;
    for(const auto & filename : filenames){
      pars.emplace_back(locate_file(filename));
    }
    return uhh2::make_unique<FactorizedJetCorrector>(pars);
  }




  //propagate to MET
  //apply type1 MET correction to RAW MET
  //NB: jet with substracted muon Pt should be used
  void correct_MET(const Event & event, double pt_thresh, double eta_thresh_low, double eta_thresh_high){
    //we start from raw MET
    LorentzVector metv4= event.met->uncorr_v4();
    for(auto & jet : *event.jets){
      //thresholds on the corrected jets: pt > 15, EM fraction < 0.9
      bool to_be_corrected = jet.v4().Pt() > 15.;
      to_be_corrected = to_be_corrected && ( fabs(jet.v4().Eta())<eta_thresh_low || fabs(jet.v4().Eta())>eta_thresh_high || jet.v4().Pt() > pt_thresh );
      to_be_corrected = to_be_corrected && (jet.neutralEmEnergyFraction()+jet.chargedEmEnergyFraction())<0.9;
      if(to_be_corrected){
        //slimmed MET is corrected by L1FastJet
        auto factor_raw = jet.JEC_factor_raw();
        auto L1factor_raw = jet.JEC_L1factor_raw();

        LorentzVector L1corr =   (L1factor_raw*factor_raw)*jet.v4();            //L1 corrected jets
        LorentzVector L123corr = jet.v4();                                      //L123 corrected jets (L23 in case of puppi)
        metv4 -=  L123corr;

        //slimmed MET is corrected by L1FastJet, for Puppi: L1factor_raw = 1 --> L1corr = raw-jet pT.
        metv4 += L1corr;
      }
    }
    event.met->set_pt(metv4.Pt());
    event.met->set_phi(metv4.Phi());
  }

  //propagate JEC to chsMET
  //Attention: the corrected values stored as standart MET values
  void correct_MET(const Event & event, FactorizedJetCorrector & corrector_L1RC, double pt_thresh, double eta_thresh_low, double eta_thresh_high){

    //we start from raw CHS MET
    LorentzVector metv4= LorentzVector(0,0,0,0);
    metv4.SetPt(hypot(event.met->rawCHS_px(),event.met->rawCHS_py()));
    metv4.SetPhi(TMath::ATan2(event.met->rawCHS_py(),event.met->rawCHS_px()));
    for(auto & jet : *event.jets){
      //thresholds on the corrected jets: pt > 15, EM fraction < 0.9
      bool to_be_corrected = jet.v4().Pt() > 15.;
      to_be_corrected = to_be_corrected && ( fabs(jet.v4().Eta())<eta_thresh_low || fabs(jet.v4().Eta())>eta_thresh_high || jet.v4().Pt() > pt_thresh );
      to_be_corrected = to_be_corrected && (jet.neutralEmEnergyFraction()+jet.chargedEmEnergyFraction())<0.9;
      if(to_be_corrected){
        auto factor_raw = jet.JEC_factor_raw();

        corrector_L1RC.setJetPt(jet.pt() * factor_raw);
        corrector_L1RC.setJetEta(jet.eta());
        corrector_L1RC.setJetE(jet.energy() * factor_raw);
        corrector_L1RC.setJetA(jet.jetArea());
        corrector_L1RC.setRho(event.rho);
        auto correctionfactors_L1RC = corrector_L1RC.getSubCorrections();
        auto correctionfactor_L1RC  = correctionfactors_L1RC.back();

        LorentzVector L1RCcorr = (correctionfactor_L1RC*factor_raw)*jet.v4();   //L1RC corrected jets
        LorentzVector L123corr = jet.v4();                                      //L123 corrected jets (L23 in case of puppi)
        metv4 -=  L123corr;
        metv4 += L1RCcorr;
      }
    }

    event.met->set_pt(metv4.Pt());
    event.met->set_phi(metv4.Phi());
  }




  JetCorrectionUncertainty* corrector_uncertainty(uhh2::Context & ctx, const std::vector<std::string> & filenames, int &direction){

    auto dir = ctx.get("jecsmear_direction", "nominal");
    if (ctx.get("dataset_type") != "MC") {
      direction = 0;
    }
    else if(dir == "up"){
      direction = 1;
    }
    else if(dir == "down"){
      direction = -1;
    }
    else if(dir != "nominal"){
      // direction = 0 is default
      throw runtime_error("JetCorrector: invalid value jecsmear_direction='" + dir + "' (valid: 'nominal', 'up', 'down')");
    }

    // Get optional source of JEC, defaults the total uncertainty if the user doesn't specify one
    std::string source = ctx.get("jecsmear_source", "Total");

    //initialize JetCorrectionUncertainty if shift direction is not "nominal", else return NULL pointer
    if(direction!=0){
      //take name from the L1FastJet correction (0th element of filenames) and replace "L1FastJet" by "UncertaintySources" to get the proper name of the uncertainty file
      TString unc_file = locate_file(filenames[0]);
      if (unc_file.Contains("L1FastJet")) {
        unc_file.ReplaceAll("L1FastJet","UncertaintySources");
      }
      else if (unc_file.Contains("L2Relative")) {
        unc_file.ReplaceAll("L2Relative","UncertaintySources");
      }
      else {
        throw runtime_error("WARNING No JEC Uncertainty File found!");
      }
      JetCorrectionUncertainty* jec_uncertainty = new JetCorrectionUncertainty(*(new JetCorrectorParameters(unc_file.Data(), source)));
      return jec_uncertainty;
    }
    return NULL;

  }

}

JetCorrector::JetCorrector(uhh2::Context & ctx, const std::vector<std::string> & filenames, const std::vector<std::string> & filenames_L1RC){
  corrector = build_corrector(filenames);
  direction = 0;

  //MET should only be corrected using AK8 jets, iff there is no AK4 collection that could be used for this because the calculation of our raw MET is based on AK4 jets
  used_ak4chs = ctx.get("JetCollection")=="jetsAk4CHS";
  used_ak4puppi = ctx.get("JetCollection")=="jetsAk4Puppi";
  metprop_possible_ak8chs = ctx.get("JetCollection")=="jetsAk8CHS";
  metprop_possible_ak8puppi = ctx.get("JetCollection")=="jetsAk8Puppi";

  //MET is always corrected using the jet collection stated in the "JetCollection" Item in the context and only in case one of the stated jet collections is used.
  //Particularly, only one of these two AK8 collections should be used.
  propagate_to_met = used_ak4chs || used_ak4puppi || metprop_possible_ak8chs || metprop_possible_ak8puppi;
  if(!propagate_to_met) cout << "WARNING in JetCorrections.cxx: You specified a jet collection in the 'JetCollection' item in the config file that is not suited to correct MET. You should change that if you are using MET." << endl;

  //The first two collections are standard MET collections. The third one is only used for derivation of JECs
  used_slimmedmet = ctx.get("METName")=="slimmedMETs";
  used_puppimet = ctx.get("METName")=="slimmedMETsPuppi";


  //if CHS MET is used, the correction is based on the (L123 - L1RC) scheme, else it is based on the standard (L123-L1).
  //See also: https://twiki.cern.ch/twiki/bin/viewauth/CMS/METType1Type2Formulae
  //If using CHS MET and therefore going for (L123 - L1RC),
  //the L1RC corrections have to be provided in a separate const std::vector<std::string>. This must only contain the L1RC correction.
  if(filenames_L1RC.size() == 1)
  corrector_L1RC = build_corrector(filenames_L1RC);
  //create dummy if L1RC is not needed. It is not applied anyway
  else corrector_L1RC = build_corrector(filenames);

  jec_uncertainty = corrector_uncertainty(ctx, filenames, direction) ;
}

bool JetCorrector::process(uhh2::Event & event){
  assert(event.jets);

  //apply jet corrections
  for(auto & jet : *event.jets){
    correct_jet(*corrector, jet, event, jec_uncertainty, direction);
  }

  return true;
}

bool JetCorrector::correct_met(uhh2::Event & event, const bool & isCHSmet, double pt_thresh, double eta_thresh_low, double eta_thresh_high){
  assert(event.jets);
  if(!isCHSmet){ //for standart MET collection (most of the case) proceed with standart correction
    //propagate jet corrections to MET
    correct_MET(event, pt_thresh, eta_thresh_low,  eta_thresh_high);
  }
  else{
    correct_MET(event, *corrector_L1RC, pt_thresh, eta_thresh_low,  eta_thresh_high);
  }

  return true;
}

// note: implement here because only here (and not in the header file), the destructor of FactorizedJetCorrector is known
JetCorrector::~JetCorrector(){}


TopJetCorrector::TopJetCorrector(uhh2::Context & ctx, const std::vector<std::string> & filenames){
  corrector = build_corrector(filenames);
  direction = 0;
  jec_uncertainty = corrector_uncertainty(ctx, filenames, direction) ;
}

bool TopJetCorrector::process(uhh2::Event & event){
  assert(event.topjets);
  for(auto & jet : *event.topjets){
    correct_jet(*corrector, jet, event, jec_uncertainty, direction);
  }
  return true;
}

// note: implement here because only here (and not in the header file), the destructor of FactorizedJetCorrector is known
TopJetCorrector::~TopJetCorrector(){}

SubJetCorrector::SubJetCorrector(uhh2::Context & ctx, const std::vector<std::string> & filenames){
  corrector = build_corrector(filenames);
  direction = 0;
  jec_uncertainty = corrector_uncertainty(ctx, filenames, direction) ;
}

bool SubJetCorrector::process(uhh2::Event & event){
  assert(event.topjets);
  for(auto & topjet : *event.topjets){
    auto subjets = topjet.subjets();
    for (auto & subjet : subjets) {
      correct_jet(*corrector, subjet, event, jec_uncertainty, direction);
    }
    topjet.set_subjets(move(subjets));
  }
  return true;
}

// note: implement here because only here (and not in the header file), the destructor of FactorizedJetCorrector is known
SubJetCorrector::~SubJetCorrector(){}

GenericJetCorrector::GenericJetCorrector(uhh2::Context & ctx, const std::vector<std::string> & filenames, const std::string & collectionname){
  corrector = build_corrector(filenames);
  direction = 0;
  jec_uncertainty = corrector_uncertainty(ctx, filenames, direction) ;
  h_jets = ctx.get_handle<std::vector<Jet> >(collectionname);
}

bool GenericJetCorrector::process(uhh2::Event & event){

  const auto jets = &event.get(h_jets);
  assert(jets);
  for(auto & jet : *jets){
    correct_jet(*corrector, jet, event, jec_uncertainty, direction);
  }
  return true;
}

// note: implement here because only here (and not in the header file), the destructor of FactorizedJetCorrector is known
GenericJetCorrector::~GenericJetCorrector(){}

GenericTopJetCorrector::GenericTopJetCorrector(uhh2::Context & ctx, const std::vector<std::string> & filenames, const std::string & collectionname){
  corrector = build_corrector(filenames);
  direction = 0;
  jec_uncertainty = corrector_uncertainty(ctx, filenames, direction) ;
  h_jets = ctx.get_handle<std::vector<TopJet> >(collectionname);
}

bool GenericTopJetCorrector::process(uhh2::Event & event){

  const auto jets = &event.get(h_jets);
  assert(jets);
  for(auto & jet : *jets){
    correct_jet(*corrector, jet, event, jec_uncertainty, direction);
  }
  return true;
}

// note: implement here because only here (and not in the header file), the destructor of FactorizedJetCorrector is known
GenericTopJetCorrector::~GenericTopJetCorrector(){}

GenericSubJetCorrector::GenericSubJetCorrector(uhh2::Context & ctx, const std::vector<std::string> & filenames, const std::string & _collectionname){
  collectionname = _collectionname;
  corrector = build_corrector(filenames);
  direction = 0;
  jec_uncertainty = corrector_uncertainty(ctx, filenames, direction) ;
  h_topjets = ctx.get_handle<std::vector<TopJet> >(collectionname);
}

bool GenericSubJetCorrector::process(uhh2::Event & event){

  const auto topjets = &event.get(h_topjets);

  assert(topjets);
  for(auto & topjet : *topjets){
    auto subjets = topjet.subjets();
    for (auto & subjet : subjets) {
      correct_jet(*corrector, subjet, event, jec_uncertainty, direction);
    }
    topjet.set_subjets(move(subjets));
  }
  return true;
}

// note: implement here because only here (and not in the header file), the destructor of FactorizedJetCorrector is known
GenericSubJetCorrector::~GenericSubJetCorrector(){}

// ** JetLeptonCleaner

JetLeptonCleaner::JetLeptonCleaner(uhh2::Context & ctx, const std::vector<std::string> & filenames){
  corrector = build_corrector(filenames);
  direction = 0;
  jec_uncertainty = corrector_uncertainty(ctx, filenames, direction) ;
}

bool JetLeptonCleaner::process(uhh2::Event & event){
  assert(event.jets);
  if(event.muons){
    for(const auto & mu : *event.muons){
      if(mu_id && !(mu_id(mu, event))) continue;
      for(auto & jet : *event.jets){
        if(deltaR(jet, mu) < drmax && jet.muonMultiplicity() > 0){
          auto jet_p4_raw = jet.v4() * jet.JEC_factor_raw();
          // note that muon energy fraction as stored in the jet refers to the raw jet energy.
          double muon_energy_in_jet = jet_p4_raw.E() * jet.muonEnergyFraction();
          double new_muon_energy_in_jet = muon_energy_in_jet - mu.energy();

          // test compatibility of the hypothesis that the muon has been clustered to the jet with
          // the jet information. The hypothesis is rejected if the muon energy in the jet is too small
          // (but allow 10% off). Note that in general (for muon multiplicity > 1), the muon energy in
          // the jet might be larger than from the single muon; make sure to consider that in the test
          // by requiring one direction in the comparison only in case the muon multiplicity is 1.
          if(new_muon_energy_in_jet < -0.1 * mu.energy() || (jet.muonMultiplicity() == 1 && new_muon_energy_in_jet > 0.1 * mu.energy())){
            continue;
          }
          jet_p4_raw -= mu.v4();
          // if that subtraction flipped the jet direction (angle between new and old > 90 degrees or pi/2), emit a warning and set its momentum to 0.
          // Only warn if pt > 5GeV (otherwise, the jet is 0 anyway for all practical purposes).
          if(jet_p4_raw.pt() > 5 && deltaR(jet_p4_raw, jet) > M_PI/2){
            cout << "Warning: subtracting lepton flipped jet direction" << endl;
            jet.set_v4(LorentzVector());
            continue;
          }
          // re-correct jet. First, set p4_raw = p4_corrected such that
          // the 'correct_jet' method does what it should do if using JEC_factor_raw ...
          jet.set_JEC_factor_raw(1.0);
          jet.set_v4(jet_p4_raw);
          // set new muon multiplicity and muon energy fraction:
          jet.set_muonMultiplicity(jet.muonMultiplicity() - 1);
          jet.set_muonEnergyFraction(max(new_muon_energy_in_jet / jet_p4_raw.E(), 0.0));
          //                    correct_jet(*corrector, jet, event, jec_uncertainty, direction, false);
          correct_jet(*corrector, jet, event, jec_uncertainty, direction);
        }
      }
    }
  }
  if(event.electrons){
    for(const auto & ele : *event.electrons){
      if(ele_id && !(ele_id(ele, event))) continue;
      for(auto & jet : *event.jets){
        if(deltaR(jet, ele) < drmax && jet.electronMultiplicity() > 0){
          auto jet_p4_raw = jet.v4() * jet.JEC_factor_raw();
          double electron_energy_in_jet = jet_p4_raw.E() * jet.chargedEmEnergyFraction();
          double new_electron_energy_in_jet = electron_energy_in_jet - ele.energy();

          if(new_electron_energy_in_jet < -0.1 * ele.energy() || (jet.chargedEmEnergyFraction() == 1 && new_electron_energy_in_jet > 0.1 * ele.energy())){
            continue;
          }
          jet_p4_raw -= ele.v4();
          if(jet_p4_raw.pt() > 5 && deltaR(jet_p4_raw, jet) > M_PI/2){
            cout << "Warning: subtracting lepton flipped jet direction" << endl;
            jet.set_v4(LorentzVector());
            continue;
          }
          // re-correct jet:
          jet.set_JEC_factor_raw(1.0);
          jet.set_v4(jet_p4_raw);
          jet.set_electronMultiplicity(jet.electronMultiplicity() - 1);
          jet.set_chargedEmEnergyFraction(max(new_electron_energy_in_jet / jet_p4_raw.E(), 0.0));
          correct_jet(*corrector, jet, event, jec_uncertainty, direction);
        }
      }
    }
  }
  return true;
}

// see ~JetCorrector
JetLeptonCleaner::~JetLeptonCleaner(){}


//// jet-lepton cleaning module using the matching of the candidates' keys

JetLeptonCleaner_by_KEYmatching::JetLeptonCleaner_by_KEYmatching(uhh2::Context& ctx, const std::vector<std::string> & filenames, const std::string& jet_label){

  h_jets_ = ctx.get_handle<std::vector<Jet>>(jet_label);
  h_topjets_ = ctx.get_handle<std::vector<TopJet>>(jet_label);
  corrector = build_corrector(filenames);
  direction = 0;
  jec_uncertainty = corrector_uncertainty(ctx, filenames, direction) ;
}

JetLeptonCleaner_by_KEYmatching::~JetLeptonCleaner_by_KEYmatching(){}

bool JetLeptonCleaner_by_KEYmatching::do_cleaning(Jet & jet, uhh2::Event& event) {
  bool correct_p4(false);
  auto jet_p4_raw = jet.v4() * jet.JEC_factor_raw();

  const auto& jet_lepton_keys = jet.lepton_keys();

  // muon-cleaning
  if(event.muons){

    for(const auto& muo : *event.muons){

      if(muo_id && !(muo_id(muo, event))) continue;

      for(const auto& muo_cand : muo.source_candidates()){

        if(std::find(jet_lepton_keys.begin(), jet_lepton_keys.end(), muo_cand.key) != jet_lepton_keys.end()){

          correct_p4 = true;

          LorentzVectorXYZE muo_cand_p4;
          muo_cand_p4.SetPx(muo_cand.px);
          muo_cand_p4.SetPy(muo_cand.py);
          muo_cand_p4.SetPz(muo_cand.pz);
          muo_cand_p4.SetE (muo_cand.E);

          jet_p4_raw -= muo_cand_p4;
        }
      }
    }
  }

  // electron-cleaning
  if(event.electrons){

    for(const auto& ele : *event.electrons){

      if(ele_id && !(ele_id(ele, event))) continue;

      for(const auto& ele_cand : ele.source_candidates()){

        if(std::find(jet_lepton_keys.begin(), jet_lepton_keys.end(), ele_cand.key) != jet_lepton_keys.end()){

          correct_p4 = true;

          LorentzVectorXYZE ele_cand_p4;
          ele_cand_p4.SetPx(ele_cand.px);
          ele_cand_p4.SetPy(ele_cand.py);
          ele_cand_p4.SetPz(ele_cand.pz);
          ele_cand_p4.SetE (ele_cand.E);

          jet_p4_raw -= ele_cand_p4;
        }
      }
    }
  }

  // jet-p4 correction
  if(correct_p4){

    jet.set_JEC_factor_raw(1.);
    jet.set_v4(jet_p4_raw);

    correct_jet(*corrector, jet, event, jec_uncertainty, direction);
    return true;
  }
  return false;
}

bool JetLeptonCleaner_by_KEYmatching::process(uhh2::Event& event){

  std::vector<Jet> * jets = NULL;
  std::vector<TopJet> * topjets = NULL;
  if (event.is_valid(h_jets_)) {
    jets = &event.get(h_jets_);
  } else {
    assert(event.is_valid(h_topjets_));
    topjets = &event.get(h_topjets_);
  }

  if (jets) {
    for(auto& jet : *jets ){
      do_cleaning(jet, event);
    }
  }
  else if (topjets) {
    for(auto& jet : *topjets ){
      do_cleaning(jet, event);
    }
  }
  else {
    std::cerr << "Neither jet nor topjet collection found for this name!\n";
    assert(false);
  }

  return true;

}

JetResolutionSmearer::JetResolutionSmearer(uhh2::Context & ctx){
  // Auto-determine recommended resolution txt file & SFs from year + jet & PU algorithms
  std::string jetstr = uhh2::string2lowercase(ctx.get("JetCollection"));

  std::string jetAlgoRadius;
  if (jetstr.find("ak4") != std::string::npos) {
    jetAlgoRadius = "AK4";
  } else if (jetstr.find("ak8") != std::string::npos) {
    jetAlgoRadius = "AK8";
  } else {
    throw runtime_error("JetCollection does not contain AK4 or AK8 - cannot determine filename for JetResolutionSmearer");
  }

  std::string puName;
  if (jetstr.find("chs") != std::string::npos) {
    puName = "chs";
  } else if (jetstr.find("puppi") != std::string::npos) {
    puName = "Puppi";
  } else {
    throw runtime_error("JetCollection not CHS or Puppi - cannot determine filename for JetResolutionSmearer");
  }

  // Official recommendations:
  // https://twiki.cern.ch/twiki/bin/view/CMS/JetResolution#JER_Scaling_factors_and_Uncertai
  const Year & year = extract_year(ctx);
  std::string version = "";
  if (year == Year::is2016v2 || year == Year::is2016v3) {
    version = "Summer16_25nsV1";
  } else if (year == Year::is2017v1 || year == Year::is2017v2) {
    version = "Fall17_V3";
  } else if (year == Year::is2018) {
    version = "Autumn18_V7";
  }  else if (year == Year::isUL17) {
    version = "Summer19UL17_JRV2";
  } else if (year == Year::isUL18) {
    version = "Autumn18_V7";
    std::cout << "WARNING: UL18 JER need updating - currently no recommendation" << std::endl;
  } else {
    throw runtime_error("Cannot find suitable jet resolution file & scale factors for this year for JetResolutionSmearer");
  }

  std::string scaleFactorFilename  = "JRDatabase/textFiles/"+version+"_MC/"+version+"_MC_SF_"+jetAlgoRadius+"PF"+puName+".txt";
  std::string resolutionFilename = "JRDatabase/textFiles/"+version+"_MC/"+version+"_MC_PtResolution_"+jetAlgoRadius+"PF"+puName+".txt";

  m_gjrs = new GenericJetResolutionSmearer(ctx, "jets", "genjets", scaleFactorFilename, resolutionFilename);

}

JetResolutionSmearer::JetResolutionSmearer(uhh2::Context & ctx, const std::string& scaleFactorFilename, const std::string& resolutionFilename){
  m_gjrs = new GenericJetResolutionSmearer(ctx, "jets", "genjets", scaleFactorFilename, resolutionFilename);
}

bool JetResolutionSmearer::process(uhh2::Event & event) {
  if(event.isRealData) return true;
  m_gjrs->process(event);
  return true;
}

JetResolutionSmearer::~JetResolutionSmearer(){}

////

GenericJetResolutionSmearer::GenericJetResolutionSmearer(uhh2::Context& ctx, const std::string& recjet_label, const std::string& genjet_label, const TString& scaleFactorFilename, const TString& resolutionFilename){

  if(ctx.get("meta_jer_applied__"+recjet_label, "") != "true") ctx.set_metadata("jer_applied__"+recjet_label, "true");
  else throw std::runtime_error("GenericJetResolutionSmearer::GenericJetResolutionSmearer -- JER smearing already applied to this RECO-jets collection: "+recjet_label);

  const std::string& dir = ctx.get("jersmear_direction", "nominal");
  if     (dir == "nominal") direction =  0;
  else if(dir == "up")      direction =  1;
  else if(dir == "down")    direction = -1;
  else throw std::runtime_error("GenericJetResolutionSmearer::GenericJetResolutionSmearer -- invalid value jersmear_direction='"+dir+"' (valid: 'nominal', 'up', 'down')");

  h_recjets_    = ctx.get_handle<std::vector<Jet> >      (recjet_label);
  h_rectopjets_ = ctx.get_handle<std::vector<TopJet> >   (recjet_label);

  h_genjets_    = ctx.get_handle<std::vector<GenJet> > (genjet_label);
  h_gentopjets_ = ctx.get_handle<std::vector<GenTopJet> >(genjet_label);

  //read in file for jet resolution (taken from https://github.com/cms-jet/JRDatabase/blob/master/textFiles/)
  TString scaleFactorFilename_ = scaleFactorFilename.Contains("JRDatabase/textFiles/")? scaleFactorFilename : "JRDatabase/textFiles/"+scaleFactorFilename;
  TString resolutionFilename_ = resolutionFilename.Contains("JRDatabase/textFiles/")?  resolutionFilename  : "JRDatabase/textFiles/"+resolutionFilename;
  res_sf_ = JME::JetResolutionScaleFactor(locate_file(scaleFactorFilename_.Data()));
  resolution_ = JME::JetResolution(locate_file(resolutionFilename_.Data()));

}

bool GenericJetResolutionSmearer::process(uhh2::Event& evt){

  if(evt.isRealData) return true;

  std::vector<Jet>*    rec_jets(0);
  std::vector<TopJet>* rec_topjets(0);

  if     (evt.is_valid(h_recjets_))    rec_jets    = &evt.get(h_recjets_);
  else if(evt.is_valid(h_rectopjets_)) rec_topjets = &evt.get(h_rectopjets_);
  else throw std::runtime_error("GenericJetResolutionSmearer::process -- invalid handle to RECO-jets");

  std::vector<GenJet>*  gen_jets(0);
  std::vector<GenTopJet>* gen_topjets(0);

  if     (evt.is_valid(h_genjets_))    gen_jets    = &evt.get(h_genjets_);
  else if(evt.is_valid(h_gentopjets_)) gen_topjets = &evt.get(h_gentopjets_);
  else throw std::runtime_error("GenericJetResolutionSmearer::process -- invalid handle to GEN-jets");

  if     (rec_jets    && gen_jets)    apply_JER_smearing(*rec_jets   , *gen_jets   , 0.4, evt.rho);
  else if(rec_topjets && gen_jets)    apply_JER_smearing(*rec_topjets, *gen_jets   , 0.8, evt.rho);
  else if(rec_topjets && gen_topjets) apply_JER_smearing(*rec_topjets, *gen_topjets, 0.8, evt.rho);
  else throw std::runtime_error("GenericJetResolutionSmearer::process -- invalid combination of RECO-GEN jet collections");

  return true;
}

/**
* Here is where the actual smearing is done to each jet in the collection
* The "hybrid" method is used:
* - if there is a matching genjet, we use the "scaling" method,
*   using the relative reco-gen difference to rescale the jet
* - if there is no matching genjet, we use the "stochastic" method,
*   using a random number drawn from a Gaussian of the jet resolution to rescale the jet
*
* In both cases, we need the jet resolution, and a resolution scale factor (SF),
* which scales MC resolution to data.
*
* See https://twiki.cern.ch/twiki/bin/view/CMS/JetResolution#Smearing_procedures
*/
template<typename RJ, typename GJ>
void GenericJetResolutionSmearer::apply_JER_smearing(std::vector<RJ>& rec_jets, const std::vector<GJ>& gen_jets, float radius, float rho){

  for(unsigned int i=0; i<rec_jets.size(); ++i){

    auto& jet = rec_jets.at(i);

    LorentzVector jet_v4 = jet.v4();
    float recopt = jet_v4.pt();
    float recoeta = jet_v4.eta();
    float abseta = fabs(recoeta);

    // find next genjet:
    auto closest_genjet = closestParticle(jet, gen_jets);
    float genpt = -1.;

    // Get resolution for this jet:
    float resolution = resolution_.getResolution({{JME::Binning::JetPt, recopt}, {JME::Binning::JetEta, recoeta}, {JME::Binning::Rho, rho}});

    // Resolution can be nan if bad formula parameters - check here
    // Generally this should be reported! This is a Bad Thing
    if (isnan(resolution)) {
      if (recopt < 35) { // leniency in this problematic region, hopefully fixed in future version of JER
        cout << "WARNING: getResolution() evaluated to nan. Since this jet is in problematic region, it will instead be set to 0." << endl;
        cout << "Input eta : rho : pt = " << recoeta << " : " << rho << ": " << recopt << endl;
        resolution = 0.;
      } else {
        throw std::runtime_error("getResolution() evaluated to nan. Input eta : rho : pt = " + double2string(recoeta) + " : " + double2string(rho) + " : " + double2string(recopt));
      }
    }

    // Test if acceptable genjet match:
    // Ignore unmatched jets (= no genjets at all, or large DeltaR relative to jet radius),
    // or jets where the difference between recojet & genjet is much larger
    // than the expected resolution, or the genjet pt is too small.
    // These jets will instead be treated with the stochastic method.
    if(!(closest_genjet == nullptr) && uhh2::deltaR(*closest_genjet, jet) < 0.5*radius){
      genpt = closest_genjet->pt();
    }
    if( fabs(genpt-recopt) > 3*resolution*recopt){
      genpt=-1;
    }
    if(genpt < 15.0f) {
      genpt=-1.;
    }

    // Get the scale factor for this jet
    float c = getScaleFactor(recopt, recoeta);
    if (c < 0) {
      std::cout << "WARNING: GenericJetResolutionSmearer: no scale factor found for this jet with pt : eta = " << recopt << " : " << recoeta << std::endl;
      std::cout << "         No JER smearing will be applied." << std::endl;
    }

    // Calculate the new pt
    float new_pt = -1.;
    // Use scaling method in case a matching generator jet was found
    if(genpt>0){
      new_pt = std::max(0.0f, genpt + c * (recopt - genpt));
    }
    // Use stochastic method if no generator jet could be matched to the reco jet
    else{
      // Initialize random generator with eta-dependend random seed to be reproducible
      TRandom rand((int)(1000*abseta));
      float random_gauss = rand.Gaus(0, resolution);
      new_pt = recopt * (1 + random_gauss*sqrt(std::max(c*c-1, 0.0f)));
    }
    jet_v4 *= new_pt / recopt;

    // Update JEC_factor_raw needed for smearing MET
    float factor_raw = jet.JEC_factor_raw();
    factor_raw *= recopt/new_pt;

    jet.set_JEC_factor_raw(factor_raw);
    jet.set_v4(jet_v4);
  }

  return;
}

/**
* Get the resolution scale factor for a jet with given pt, eta.
* Automatically accounts for the up & down variations, which is already
* specified as a member variable.
*
* This hides away the fact that we might be using a vector of floats,
* or the JetResolutionScaleFactor object.

* If SF < 0, then indicates something has gone wrong or is out of bounds,
* and the jet should be skipped
*/
float GenericJetResolutionSmearer::getScaleFactor(float pt, float eta) {
  float c = -1;
  if (direction == 0) {
    c = res_sf_.getScaleFactor({{JME::Binning::JetPt, pt}, {JME::Binning::JetEta, eta}});
  } else if (direction == 1) {
    c = res_sf_.getScaleFactor({{JME::Binning::JetPt, pt}, {JME::Binning::JetEta, eta}}, Variation::UP);
  } else {
    c = res_sf_.getScaleFactor({{JME::Binning::JetPt, pt}, {JME::Binning::JetEta, eta}}, Variation::DOWN);
  }
  return c;
}


//// -----------------------------------------------------------------
SoftDropMassCalculator::SoftDropMassCalculator(uhh2::Context & ctx, bool applyCorrections, const std::string & puppiCorrFilename, const std::string & jetCollName):
applyCorrections_(applyCorrections)
{
  h_topjets_ = ctx.get_handle<std::vector<TopJet>>(jetCollName);
  if (applyCorrections_) {
    puppiCorrFile.reset(TFile::Open(locate_file(puppiCorrFilename).c_str()));
    puppisd_corrGEN.reset((TF1*) puppiCorrFile->Get("puppiJECcorr_gen"));
    puppisd_corrRECO_cen.reset((TF1*) puppiCorrFile->Get("puppiJECcorr_reco_0eta1v3"));
    puppisd_corrRECO_for.reset((TF1*) puppiCorrFile->Get("puppiJECcorr_reco_1v3eta2v5"));
  }
}

bool SoftDropMassCalculator::process(uhh2::Event & evt) {
  std::vector<TopJet>* topjets(0);

  if (evt.is_valid(h_topjets_)) topjets = &evt.get(h_topjets_);
  else throw std::runtime_error("SoftDropMassCalculator::process -- invalid handle to topjets");

  for (auto & jet : *topjets) {
    float puppi_softdrop_mass = calcSDmass(jet);
    if (applyCorrections_) { puppi_softdrop_mass *= getPUPPIweight(jet.pt(), jet.eta()); }
    jet.set_softdropmass(puppi_softdrop_mass);
  }

  return true;
}

float SoftDropMassCalculator::calcSDmass(const TopJet & jet) {
  // Calculate uncorrected SD mass from subjets
  LorentzVector puppi_softdrop;
  for (auto & subjet : jet.subjets()) {
    // Important, must use UNCORRECTED subjet
    puppi_softdrop += (subjet.JEC_factor_raw() * subjet.v4());
  }

  return inv_mass_safe(puppi_softdrop);
}

float SoftDropMassCalculator::getPUPPIweight(float pt, float eta) {
  // TODO: add check /safety if outside x range
  // Calculate correction factor for SD mass
  // The final correction is a combination of 2 factors:
  // one that corrects reco -> gen,
  // and one that corrects the generator W mass to its PDG value.
  //
  // pt and eta shold be either from CHS AK8 jets or PUPPI
  // AK8 jets with full JEC applied,
  // but not the PUPPI softdrop AK8 jet pt and eta
  float genCorr = puppisd_corrGEN->Eval(pt);
  float recoCorr = (fabs(eta) <= 1.3) ? puppisd_corrRECO_cen->Eval(pt) : puppisd_corrRECO_for->Eval(pt);
  return genCorr * recoCorr;
}
//// -----------------------------------------------------------------
