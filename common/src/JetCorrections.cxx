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
	  unc = jec_unc->getUncertainty(1);
	  correctionfactor *= (1 + fabs(unc));
	} else if (jec_unc_direction == -1){
	  unc = jec_unc->getUncertainty(-1);
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
    if(dir == "up"){
        direction = 1;
    }
    else if(dir == "down"){
        direction = -1;
    }
    else if(dir != "nominal"){
        // direction = 0 is default
        throw runtime_error("JetCorrector: invalid value jecsmear_direction='" + dir + "' (valid: 'nominal', 'up', 'down')");
    }

    //initialize JetCorrectionUncertainty if shift direction is not "nominal", else return NULL pointer
    if(direction!=0){
      //take name from the L1FastJet correction (0th element of filenames) and replace "L1FastJet" by "Uncertainty" to get the proper name of the uncertainty file
      TString unc_file = locate_file(filenames[0]);
      if (unc_file.Contains("L1FastJet")) {
        unc_file.ReplaceAll("L1FastJet","Uncertainty");
      }
      else if (unc_file.Contains("L2Relative")) {
        unc_file.ReplaceAll("L2Relative","Uncertainty");
      }
      else {
        throw runtime_error("WARNING No JEC Uncertainty File found!");
      }
      JetCorrectionUncertainty* jec_uncertainty = new JetCorrectionUncertainty(unc_file.Data());
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

GenericSubJetCorrector::GenericSubJetCorrector(uhh2::Context & ctx, const std::vector<std::string> & filenames, const std::string & collectionname){
    corrector = build_corrector(filenames);
    direction = 0;
    jec_uncertainty = corrector_uncertainty(ctx, filenames, direction) ;
    h_jets = ctx.get_handle<std::vector<TopJet> >(collectionname);
}

bool GenericSubJetCorrector::process(uhh2::Event & event){

    const auto topjets = &event.get(h_jets);
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


//// ----- modules for Jet Energy Resolution data/MC corrections -----

// https://twiki.cern.ch/twiki/bin/view/CMS/JetResolution [13TeV JER measurement]

const JERSmearing::SFtype1 JERSmearing::SF_13TeV_2016 = {
  // 0 = upper jet-eta limit
  // 1 = JER SF
  // 2 = JER SF + 1sigma
  // 3 = JER SF - 1sigma
  {{0.5, 1.109, 1.117, 1.101}},
  {{0.8, 1.138, 1.151, 1.125}},
  {{1.1, 1.114, 1.127, 1.101}},
  {{1.3, 1.123, 1.147, 1.099}},
  {{1.7, 1.084, 1.092, 1.070}},
  {{1.9, 1.082, 1.117, 1.047}},
  {{2.1, 1.140, 1.187, 1.093}},
  {{2.3, 1.067, 1.120, 1.014}},
  {{2.5, 1.177, 1.218, 1.136}},
  {{2.8, 1.364, 1.403, 1.325}},
  {{3.0, 1.857, 1.928, 1.786}},
  {{3.2, 1.328, 1.350, 1.306}},
  {{5.0, 1.160, 1.189, 1.131}},
};


// https://twiki.cern.ch/twiki/bin/view/CMS/JetResolution [13TeV JER measurement]
// New JER files from Marek (8 Aug 2017)
const JERSmearing::SFtype1 JERSmearing::SF_13TeV_2016_03Feb2017 = {
  // 0 = upper jet-eta limit
  // 1 = JER SF
  // 2 = JER SF + 1sigma
  // 3 = JER SF - 1sigma
  {{0.5, 1.093, 1.201, 0.983}},
  {{0.8, 1.144, 1.259, 1.029}},
  {{1.1, 1.087, 1.202, 0.972}},
  {{1.3, 1.119, 1.234, 1.006}},
  {{1.7, 1.093, 1.218, 0.996}},
  {{1.9, 1.072, 1.199, 0.977}},
  {{2.1, 1.122, 1.266, 1.030}},
  {{2.3, 1.127, 1.305, 0.997}},
  {{2.5, 1.206, 1.370, 1.102}},
  {{2.8, 1.340, 1.485, 1.197}},
  {{3.0, 1.761, 1.957, 1.551}},
  {{3.2, 1.317, 1.452, 1.182}},
  {{5.0, 1.140, 1.270, 1.018}},
};


// https://twiki.cern.ch/twiki/bin/view/CMS/JetResolution [13TeV JER measurement]
// Summer16_25nsV1 to be used with 2017 MC as long as results on 2017 data are not official
const JERSmearing::SFtype1 JERSmearing::SF_13TeV_Summer16_25nsV1 = {
  // 0 = upper jet-eta limit
  // 1 = JER SF
  // 2 = JER SF + 1sigma
  // 3 = JER SF - 1sigma
  {{0.522, 1.1595, 1.2240, 1.0950}},
  {{0.783, 1.1948, 1.2600, 1.1296}},
  {{1.131, 1.1464, 1.2096, 1.0832}},
  {{1.305, 1.1609, 1.2634, 1.0584}},
  {{1.740, 1.1278, 1.2264, 1.0292}},
  {{1.930, 1.1000, 1.2079, 0.9921}},
  {{2.043, 1.1426, 1.2640, 1.0212}},
  {{2.322, 1.1512, 1.2652, 1.0372}},
  {{2.500, 1.2963, 1.5334, 1.0592}},
  {{2.853, 1.3418, 1.5509, 1.1327}},
  {{2.964, 1.7788, 1.9796, 1.5780}},
  {{3.139, 1.1869, 1.3112, 1.0626}},
  {{5.191, 1.1922, 1.3410, 1.0434}},
};


// 2017
const JERSmearing::SFtype1 JERSmearing::SF_13TeV_Fall17 = {
  // 0 = upper jet-eta limit
  // 1 = JER SF
  // 2 = JER SF + 1sigma
  // 3 = JER SF - 1sigma
  {{0.522, 1.05325, 1.0563, 1.0502}},
  {{0.783, 1.12443, 1.13123, 1.11763}},
  {{1.131, 1.07745, 1.08264, 1.07226}},
  {{1.305, 1.07706, 1.08918, 1.06494}},
  {{1.74, 1.07232, 1.07863, 1.06601}},
  {{1.93, 1.07241, 1.09621, 1.04861}},
  {{2.043, 1.08814, 1.14028, 1.036}},
  {{2.322, 1.07464, 1.10508, 1.0442}},
  {{2.5, 1.07105, 1.16178, 0.980317}},
  {{2.853, 1.46856, 1.55712, 1.38}},
  {{2.964, 2.65947, 2.72613, 2.59281}},
  {{3.139, 1.43135, 1.44844, 1.41426}},
  {{5.191, 1.31982, 1.34467, 1.29497}},

};


// 2017 --> To be used with 2017 data and 94X MC
const JERSmearing::SFtype1 JERSmearing::SF_13TeV_Fall17_V3 = {
  // 0 = upper jet-eta limit
  // 1 = JER SF
  // 2 = JER SF + 1sigma
  // 3 = JER SF - 1sigma
  {{0.522, 1.14320, 1.16543, 1.12097}},
  {{0.783, 1.18152, 1.22987, 1.13317}},
  {{1.131, 1.09887, 1.14444, 1.05330}},
  {{1.305, 1.11365, 1.25332, 0.973979}},
  {{1.740, 1.13072, 1.27776, 0.98369}},
  {{1.930, 1.15996, 1.25759, 1.06232}},
  {{2.043, 1.23926, 1.43013, 1.04839}},
  {{2.322, 1.26039, 1.41049, 1.11029}},
  {{2.500, 1.40853, 1.61049, 1.20657}},
  {{2.853, 1.99090, 2.55927, 1.42253}},
  {{2.964, 2.29227, 2.66654, 1.91800}},
  {{3.139, 1.26957, 1.37847, 1.16067}},
  {{5.191, 1.15425, 1.30663, 1.00187}},

};

const JERSmearing::SFtype1 JERSmearing::SF_13TeV_Fall17_V3_RunBCDEF_Madgraph = {
  // 0 = upper jet-eta limit
  // 1 = JER SF
  // 2 = JER SF + 1sigma
  // 3 = JER SF - 1sigma
  {{0.522, 1.11931, 1.15247, 1.08615}},
  {{0.783, 1.12502, 1.1584, 1.09164}},
  {{1.131, 1.09921, 1.13388, 1.06454}},
  {{1.305, 1.09461, 1.18637, 1.00285}},
  {{1.74, 1.10174, 1.14909, 1.05439}},
  {{1.93, 1.13043, 1.26472, 0.99613}},
  {{2.043, 1.15272, 1.24681, 1.05864}},
  {{2.322, 1.16604, 1.30741, 1.02468}},
  {{2.5, 1.33917, 1.48519, 1.19315}},
  {{2.853, 1.77981, 2.20226, 1.35736}},
  {{2.964, 2.36411, 2.67408, 2.05414}},
  {{3.139, 1.25818, 1.35299, 1.16337}},
  {{5.191, 1.10801, 1.22069, 0.995326}},
};

// For 2018, from V7 onwards, you must use the text files for SF instead,
// since they are now pt-dependent

////

// 2018 -> to be used with combined dataset
const JERSmearing::SFtype1 JERSmearing::SF_13TeV_Autumn18_V1 = {
  // 0 = upper jet-eta limit
  // 1 = JER SF
  // 2 = JER SF + 1sigma
  // 3 = JER SF - 1sigma
  {{0.522, 1.15, 1.193, 1.107}},
  {{0.783, 1.134, 1.214, 1.054}},
  {{1.131, 1.102, 1.154, 1.05}},
  {{1.305, 1.134, 1.246, 1.022}},
  {{1.740, 1.104, 1.315, 0.893}},
  {{1.930, 1.149, 1.308, 0.99}},
  {{2.043, 1.148, 1.357, 0.939}},
  {{2.322, 1.114, 1.305, 0.923}},
  {{2.500, 1.347, 1.621, 1.073}},
  {{2.853, 2.137, 2.661, 1.613}},
  {{2.964, 1.65,  2.591, 0.709}},
  {{3.139, 1.225, 1.419, 1.031}},
  {{5.191, 1.082, 1.28, 0.884}},

};

// 2018 -> to be used with RunABC
const JERSmearing::SFtype1 JERSmearing::SF_13TeV_Autumn18_RunABC_V1 = {
  // 0 = upper jet-eta limit
  // 1 = JER SF
  // 2 = JER SF + 1sigma
  // 3 = JER SF - 1sigma

  {{0.522, 1.1609, 1.2161, 1.1057}},
  {{0.783, 1.1309, 1.1919, 1.0699}},
  {{1.131, 1.0918, 1.127, 1.0566}},
  {{1.305, 1.1064, 1.2195, 0.9933}},
  {{1.740, 1.1097, 1.2303, 0.9891}},
  {{1.930, 1.1554, 1.2374, 1.0734}},
  {{2.043, 1.1843, 1.3794, 0.9892}},
  {{2.322, 1.1401, 1.307, 0.9732}},
  {{2.500, 1.1818, 1.4145, 0.9491}},
  {{2.853, 1.7778, 2.2437, 1.3119}},
  {{2.964, 1.3718, 2.3225, 0.4211}},
  {{3.139, 1.2725, 1.4535, 1.0915}},
  {{5.191, 1.1255, 1.3156, 0.9354}},

};

// 2018 -> to be used with RunD
const JERSmearing::SFtype1 JERSmearing::SF_13TeV_Autumn18_RunD_V1 = {
  // 0 = upper jet-eta limit
  // 1 = JER SF
  // 2 = JER SF + 1sigma
  // 3 = JER SF - 1sigma

  {{0.522, 1.1401, 1.1724, 1.1078}},
  {{0.783, 1.1370, 1.2339, 1.0401}},
  {{1.131, 1.1109, 1.1785, 1.0433}},
  {{1.305, 1.1585, 1.2696, 1.0474}},
  {{1.740, 1.0997, 1.3894, 0.81}},
  {{1.930, 1.1428, 1.3692, 0.9164}},
  {{2.043, 1.1157, 1.3373, 0.8941}},
  {{2.322, 1.0903, 1.3034, 0.8772}},
  {{2.500, 1.4930, 1.8036, 1.1824}},
  {{2.853, 2.4518, 3.0275, 1.8761}},
  {{2.964, 1.8935, 2.8254, 0.9616}},
  {{3.139, 1.1826, 1.3884, 0.9768}},
  {{5.191, 1.0439, 1.2479, 0.8399}},

};

////

// 2018 -> to be used with combined dataset
const JERSmearing::SFtype1 JERSmearing::SF_13TeV_Autumn18_RunABCD_V4 = {
  // 0 = upper jet-eta limit
  // 1 = JER SF
  // 2 = JER SF + 1sigma
  // 3 = JER SF - 1sigma

  {{0.522, 1.15448, 1.18531, 1.123650}},
  {{0.783, 1.14808, 1.19957, 1.096590}},
  {{1.131, 1.09985, 1.13844, 1.061260}},
  {{1.305, 1.09287, 1.17851, 1.007230}},
  {{1.740, 1.10930, 1.18108, 1.037530}},
  {{1.930, 1.10049, 1.15198, 1.049000}},
  {{2.043, 1.06031, 1.19040, 0.930216}},
  {{2.322, 1.12870, 1.18184, 1.075560}},
  {{2.500, 1.33970, 1.45443, 1.224970}},
  {{2.853, 2.03250, 2.56856, 1.496440}},
  {{2.964, 2.05671, 2.36272, 1.750700}},
  {{3.139, 1.18680, 1.22439, 1.149210}},
  {{5.191, 1.09220, 1.14111, 1.043290}},

};

////

// 2018 -> to be used with RunABC
const JERSmearing::SFtype1 JERSmearing::SF_13TeV_Autumn18_RunABC_V4 = {
  // 0 = upper jet-eta limit
  // 1 = JER SF
  // 2 = JER SF + 1sigma
  // 3 = JER SF - 1sigma
  {{0.522, 1.1677, 1.2154, 1.1200}},
  {{0.783, 1.1475, 1.1995, 1.0954}},
  {{1.131, 1.1029, 1.1339, 1.0719}},
  {{1.305, 1.0781, 1.1731, 0.9830}},
  {{1.740, 1.1006, 1.1852, 1.0161}},
  {{1.930, 1.1019, 1.1405, 1.0633}},
  {{2.043, 1.0459, 1.2036, 0.8881}},
  {{2.322, 1.1612, 1.2258, 1.0966}},
  {{2.500, 1.2299, 1.3386, 1.1212}},
  {{2.853, 1.6736, 2.0528, 1.2944}},
  {{2.964, 1.7292, 1.9300, 1.5285}},
  {{3.139, 1.2257, 1.2709, 1.1805}},
  {{5.191, 1.0733, 1.1409, 1.0057}},

};

// 2018 -> to be used with RunD
const JERSmearing::SFtype1 JERSmearing::SF_13TeV_Autumn18_RunD_V4 = {
  // 0 = upper jet-eta limit
  // 1 = JER SF
  // 2 = JER SF + 1sigma
  // 3 = JER SF - 1sigma
  {{0.522, 1.1588, 1.1980, 1.1196}},
  {{0.783, 1.1504, 1.2231, 1.0776}},
  {{1.131, 1.1253, 1.1596, 1.0910}},
  {{1.305, 1.1217, 1.2043, 1.0390}},
  {{1.740, 1.1069, 1.1959, 1.0179}},
  {{1.930, 1.0916, 1.1346, 1.0486}},
  {{2.043, 1.0977, 1.1927, 1.0026}},
  {{2.322, 1.1177, 1.1898, 1.0456}},
  {{2.500, 1.4494, 1.5997, 1.2992}},
  {{2.853, 2.3588, 2.9999, 1.7177}},
  {{2.964, 2.2520, 2.6062, 1.8978}},
  {{3.139, 1.1759, 1.2298, 1.1219}},
  {{5.191, 1.0777, 1.1319, 1.0235}},

};

////

JetResolutionSmearer::JetResolutionSmearer(uhh2::Context & ctx){
  // Auto-determine correct resolution txt file & SFs from year + jet & PU algorithms
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

  std::string filenameAppend = jetAlgoRadius+"PF"+puName+".txt";

  const Year & year = extract_year(ctx);
  JERSmearing::SFtype1 JER_sf = {};
  std::string sfFilename = "";
  std::string resFilename = "";
  if (year == Year::is2016v2 || year == Year::is2016v3) {
    JER_sf = JERSmearing::SF_13TeV_Summer16_25nsV1;
    resFilename = "2016/Summer16_25nsV1_MC_PtResolution_" + filenameAppend;
  } else if (year == Year::is2017v1 || year == Year::is2017v2) {
    JER_sf = JERSmearing::SF_13TeV_Fall17_V3;
    resFilename = "2017/Fall17_V3_MC_PtResolution_" + filenameAppend;
  } else if (year == Year::is2018) {
    sfFilename = "common/data/2018/Autumn18_V7_MC_SF_" + filenameAppend;
    resFilename = "2018/Autumn18_V7_MC_PtResolution_" + filenameAppend;
  } else {
    throw runtime_error("Cannot find suitable jet resolution file & scale factors for this year for JetResolutionSmearer");
  }

  if (sfFilename != "") {
    m_gjrs = new GenericJetResolutionSmearer(ctx, "jets", "genjets", sfFilename, resFilename);
  } else if (JER_sf.size() > 0) {
    m_gjrs = new GenericJetResolutionSmearer(ctx, "jets", "genjets", JER_sf, resFilename);
  } else {
    throw runtime_error("No valid JER SF either as text file nor JERSmearing::SFtype1");
  }

}


JetResolutionSmearer::JetResolutionSmearer(uhh2::Context & ctx, const JERSmearing::SFtype1& JER_sf, const std::string& resFilename){
  m_gjrs = new GenericJetResolutionSmearer(ctx, "jets", "genjets", JER_sf, resFilename);
}

JetResolutionSmearer::JetResolutionSmearer(uhh2::Context & ctx, const std::string& scaleFactorFilename, const std::string& resFilename){
  m_gjrs = new GenericJetResolutionSmearer(ctx, "jets", "genjets", scaleFactorFilename, resFilename);
}

bool JetResolutionSmearer::process(uhh2::Event & event) {
  if(event.isRealData) return true;
  m_gjrs->process(event);
  return true;
}

JetResolutionSmearer::~JetResolutionSmearer(){}

////

GenericJetResolutionSmearer::GenericJetResolutionSmearer(uhh2::Context& ctx, const std::string& recjet_label, const std::string& genjet_label, const JERSmearing::SFtype1& JER_sf, const TString ResolutionFileName){

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

  JER_SFs_ = JER_sf;

  //read in file for jet resolution (taken from https://github.com/cms-jet/JRDatabase/blob/master/textFiles/)
  TString filename = "common/data/" + ResolutionFileName;
  resolution_ = JME::JetResolution(locate_file(filename.Data()));
}

GenericJetResolutionSmearer::GenericJetResolutionSmearer(uhh2::Context& ctx, const std::string& recjet_label, const std::string& genjet_label, const TString ScaleFactorFileName, const TString ResolutionFileName){

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
  TString filename = "common/data/" + ResolutionFileName;
  resolution_ = JME::JetResolution(locate_file(filename.Data()));

  res_sf_ = JME::JetResolutionScaleFactor(locate_file(ScaleFactorFileName.Data()));

  JER_SFs_ = {}; // set explicitly to empty so we know it isn't used
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
float GenericJetResolutionSmearer::getScaleFactor(float pt, float eta)
{
  // Using the JERSmearing::SFtype1 structure
  if (res_sf_.getResolutionObject() == nullptr) {
    // Check if there is a valid scale factor for this jet (i.e. jet is within bounds for parameters)
    int ieta(-1);
    float abseta = fabs(eta);

    for(unsigned int idx=0; idx<JER_SFs_.size(); ++idx){

      const float min_eta = idx ? JER_SFs_.at(idx-1).at(0) : 0.;
      const float max_eta =       JER_SFs_.at(idx)  .at(0);

      if(min_eta <= abseta && abseta < max_eta){ ieta = idx; break; }
    }
    if(ieta < 0) {
      return -1.;
    }

    // Get the scale factor for this jet
    float c = -1.;
    if (direction == 0) {
      c = JER_SFs_.at(ieta).at(1);
    } else if (direction == 1) {
      c = JER_SFs_.at(ieta).at(2);
    } else {
      c = JER_SFs_.at(ieta).at(3);
    }

    return c;
  }

  // Using the JetResolutionScaleFactor object
  else {
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
