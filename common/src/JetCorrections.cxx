#include "UHH2/common/include/JetCorrections.h"
#include "UHH2/common/include/Utils.h"
#include "UHH2/core/include/Utils.h"

#include "UHH2/JetMETObjects/interface/FactorizedJetCorrector.h"
#include "UHH2/JetMETObjects/interface/JetCorrectorParameters.h"

#include <string>

using namespace std;
using namespace uhh2;

//see https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookJetEnergyCorrections#GetTxtFiles how to get the txt files with jet energy corrections from the database

const std::vector<std::string> JERFiles::PHYS14_L123_MC = {"JetMETObjects/data/PHYS14_25_V2_L1FastJet_AK4PFchs.txt",
  "JetMETObjects/data/PHYS14_25_V2_L2Relative_AK4PFchs.txt", "JetMETObjects/data/PHYS14_25_V2_L3Absolute_AK4PFchs.txt"};
  
const std::vector<std::string> JERFiles::PHYS14_L123_DATA = {"JetMETObjects/data/PHYS14_25_V2_L1FastJet_AK4PFchs.txt",
  "JetMETObjects/data/PHYS14_25_V2_L2Relative_AK4PFchs.txt", "JetMETObjects/data/PHYS14_25_V2_L3Absolute_AK4PFchs.txt",
  "JetMETObjects/data/PHYS14_50_V1_L2L3Residual_AK4PFchs.txt" // note: L2L3Residual corrections not implemented in global tag PHYS14_25_V2
};

const std::vector<std::string> JERFiles::PHYS14_L123_AK8PFchs_MC = {"JetMETObjects/data/PHYS14_V2_MC_L1FastJet_AK8PFchs.txt",
  "JetMETObjects/data/PHYS14_V2_MC_L2Relative_AK8PFchs.txt", "JetMETObjects/data/PHYS14_V2_MC_L3Absolute_AK8PFchs.txt"};

// Summer15_50ns
const std::vector<std::string> JERFiles::Summer15_50ns_L123_AK4PFchs_MC = {
  "JetMETObjects/data/Summer15_50nsV5_MC_L1FastJet_AK4PFchs.txt",
  "JetMETObjects/data/Summer15_50nsV5_MC_L2Relative_AK4PFchs.txt",
  "JetMETObjects/data/Summer15_50nsV5_MC_L3Absolute_AK4PFchs.txt"
};

const std::vector<std::string> JERFiles::Summer15_50ns_L123_AK4PFchs_DATA = {
  "JetMETObjects/data/Summer15_50nsV5_DATA_L1FastJet_AK4PFchs.txt",
  "JetMETObjects/data/Summer15_50nsV5_DATA_L2Relative_AK4PFchs.txt",
  "JetMETObjects/data/Summer15_50nsV5_DATA_L3Absolute_AK4PFchs.txt",
  "JetMETObjects/data/Summer15_50nsV5_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer15_50ns_L123_AK8PFchs_MC = {
  "JetMETObjects/data/Summer15_50nsV5_MC_L1FastJet_AK8PFchs.txt",
  "JetMETObjects/data/Summer15_50nsV5_MC_L2Relative_AK8PFchs.txt",
  "JetMETObjects/data/Summer15_50nsV5_MC_L3Absolute_AK8PFchs.txt"
};

const std::vector<std::string> JERFiles::Summer15_50ns_L123_AK8PFchs_DATA = {
  "JetMETObjects/data/Summer15_50nsV5_DATA_L1FastJet_AK8PFchs.txt",
  "JetMETObjects/data/Summer15_50nsV5_DATA_L2Relative_AK8PFchs.txt",
  "JetMETObjects/data/Summer15_50nsV5_DATA_L3Absolute_AK8PFchs.txt",
  "JetMETObjects/data/Summer15_50nsV5_DATA_L2L3Residual_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer15_50ns_L123_AK4PFPuppi_MC = {
  "JetMETObjects/data/Summer15_50nsV5_MC_L1FastJet_AK4PFPuppi.txt",
  "JetMETObjects/data/Summer15_50nsV5_MC_L2Relative_AK4PFPuppi.txt",
  "JetMETObjects/data/Summer15_50nsV5_MC_L3Absolute_AK4PFPuppi.txt"
};

const std::vector<std::string> JERFiles::Summer15_50ns_L123_AK4PFPuppi_DATA = {
  "JetMETObjects/data/Summer15_50nsV5_DATA_L1FastJet_AK4PFPuppi.txt",
  "JetMETObjects/data/Summer15_50nsV5_DATA_L2Relative_AK4PFPuppi.txt",
  "JetMETObjects/data/Summer15_50nsV5_DATA_L3Absolute_AK4PFPuppi.txt",
  "JetMETObjects/data/Summer15_50nsV5_DATA_L2L3Residual_AK4PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Summer15_50ns_L123_AK8PFPuppi_MC = {
  "JetMETObjects/data/Summer15_50nsV5_MC_L1FastJet_AK8PFPuppi.txt",
  "JetMETObjects/data/Summer15_50nsV5_MC_L2Relative_AK8PFPuppi.txt",
  "JetMETObjects/data/Summer15_50nsV5_MC_L3Absolute_AK8PFPuppi.txt"
};

const std::vector<std::string> JERFiles::Summer15_50ns_L123_AK8PFPuppi_DATA = {
  "JetMETObjects/data/Summer15_50nsV5_DATA_L1FastJet_AK8PFPuppi.txt",
  "JetMETObjects/data/Summer15_50nsV5_DATA_L2Relative_AK8PFPuppi.txt",
  "JetMETObjects/data/Summer15_50nsV5_DATA_L3Absolute_AK8PFPuppi.txt",
  "JetMETObjects/data/Summer15_50nsV5_DATA_L2L3Residual_AK8PFPuppi.txt",
};

// Summer15_25ns
const std::vector<std::string> JERFiles::Summer15_25ns_L123_AK4PFchs_MC = {
  "JetMETObjects/data/Summer15_25nsV6_MC_L1FastJet_AK4PFchs.txt",
  "JetMETObjects/data/Summer15_25nsV6_MC_L2Relative_AK4PFchs.txt",
  "JetMETObjects/data/Summer15_25nsV6_MC_L3Absolute_AK4PFchs.txt"
};

const std::vector<std::string> JERFiles::Summer15_25ns_L123_AK4PFchs_DATA = {
  "JetMETObjects/data/Summer15_25nsV6_DATA_L1FastJet_AK4PFchs.txt",
  "JetMETObjects/data/Summer15_25nsV6_DATA_L2Relative_AK4PFchs.txt",
  "JetMETObjects/data/Summer15_25nsV6_DATA_L3Absolute_AK4PFchs.txt",
  "JetMETObjects/data/Summer15_25nsV6_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer15_25ns_L123_AK8PFchs_MC = {
  "JetMETObjects/data/Summer15_25nsV6_MC_L1FastJet_AK8PFchs.txt",
  "JetMETObjects/data/Summer15_25nsV6_MC_L2Relative_AK8PFchs.txt",
  "JetMETObjects/data/Summer15_25nsV6_MC_L3Absolute_AK8PFchs.txt"
};

const std::vector<std::string> JERFiles::Summer15_25ns_L123_AK8PFchs_DATA = {
  "JetMETObjects/data/Summer15_25nsV6_DATA_L1FastJet_AK8PFchs.txt",
  "JetMETObjects/data/Summer15_25nsV6_DATA_L2Relative_AK8PFchs.txt",
  "JetMETObjects/data/Summer15_25nsV6_DATA_L3Absolute_AK8PFchs.txt",
  "JetMETObjects/data/Summer15_25nsV6_DATA_L2L3Residual_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer15_25ns_L23_AK8PFchs_MC = {
  "JetMETObjects/data/Summer15_25nsV6_MC_L2Relative_AK8PFchs.txt",
  "JetMETObjects/data/Summer15_25nsV6_MC_L3Absolute_AK8PFchs.txt"
};

const std::vector<std::string> JERFiles::Summer15_25ns_L23_AK8PFchs_DATA = {
  "JetMETObjects/data/Summer15_25nsV6_DATA_L2Relative_AK8PFchs.txt",
  "JetMETObjects/data/Summer15_25nsV6_DATA_L3Absolute_AK8PFchs.txt",
  "JetMETObjects/data/Summer15_25nsV6_DATA_L2L3Residual_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer15_25ns_L123_AK4PFPuppi_MC = {
  "JetMETObjects/data/Summer15_25nsV6_MC_L1FastJet_AK4PFPuppi.txt",
  "JetMETObjects/data/Summer15_25nsV6_MC_L2Relative_AK4PFPuppi.txt",
  "JetMETObjects/data/Summer15_25nsV6_MC_L3Absolute_AK4PFPuppi.txt"
};

const std::vector<std::string> JERFiles::Summer15_25ns_L123_AK4PFPuppi_DATA = {
  "JetMETObjects/data/Summer15_25nsV6_DATA_L1FastJet_AK4PFPuppi.txt",
  "JetMETObjects/data/Summer15_25nsV6_DATA_L2Relative_AK4PFPuppi.txt",
  "JetMETObjects/data/Summer15_25nsV6_DATA_L3Absolute_AK4PFPuppi.txt",
  "JetMETObjects/data/Summer15_25nsV6_DATA_L2L3Residual_AK4PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Summer15_25ns_L123_AK8PFPuppi_MC = {
  "JetMETObjects/data/Summer15_25nsV6_MC_L1FastJet_AK8PFPuppi.txt",
  "JetMETObjects/data/Summer15_25nsV6_MC_L2Relative_AK8PFPuppi.txt",
  "JetMETObjects/data/Summer15_25nsV6_MC_L3Absolute_AK8PFPuppi.txt"
};

const std::vector<std::string> JERFiles::Summer15_25ns_L123_AK8PFPuppi_DATA = {
  "JetMETObjects/data/Summer15_25nsV6_DATA_L1FastJet_AK8PFPuppi.txt",
  "JetMETObjects/data/Summer15_25nsV6_DATA_L2Relative_AK8PFPuppi.txt",
  "JetMETObjects/data/Summer15_25nsV6_DATA_L3Absolute_AK8PFPuppi.txt",
  "JetMETObjects/data/Summer15_25nsV6_DATA_L2L3Residual_AK8PFPuppi.txt",
};

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

void correct_jet(FactorizedJetCorrector & corrector, Jet & jet, const Event & event, JetCorrectionUncertainty* jec_unc = NULL, int jec_unc_direction=0){
    auto factor_raw = jet.JEC_factor_raw();
    corrector.setJetPt(jet.pt() * factor_raw);
    corrector.setJetEta(jet.eta());
    corrector.setJetE(jet.energy() * factor_raw);
    corrector.setJetA(jet.jetArea());
    corrector.setRho(event.rho);
    auto correctionfactor = corrector.getCorrection();

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
      unc_file.ReplaceAll("L1FastJet","Uncertainty");
      JetCorrectionUncertainty* jec_uncertainty = new JetCorrectionUncertainty(unc_file.Data());
      return jec_uncertainty;
    }
    return NULL;
    
}

}

JetCorrector::JetCorrector(uhh2::Context & ctx, const std::vector<std::string> & filenames){
    corrector = build_corrector(filenames);
    direction = 0;
    jec_uncertainty = corrector_uncertainty(ctx, filenames, direction) ;
}
    
bool JetCorrector::process(uhh2::Event & event){
    assert(event.jets);
    for(auto & jet : *event.jets){
        correct_jet(*corrector, jet, event, jec_uncertainty, direction);
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
    label_ = jet_label;
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

////


JetResolutionSmearer::JetResolutionSmearer(uhh2::Context & ctx){
    smear_met = string2bool(ctx.get("jersmear_smear_met", "false"));
    auto dir = ctx.get("jersmear_direction", "nominal");
    if(dir == "up"){
        direction = 1;
    }
    else if(dir == "down"){
        direction = -1;
    }
    else if(dir != "nominal"){
        // direction = 0 is default
        throw runtime_error("JetResolutionSmearer: invalid value jersmear_direction='" + dir + "' (valid: 'nominal', 'up', 'down')");
    }
    if(ctx.get("meta_jer_applied", "") == "true"){
        throw runtime_error("JetResolutionSmearer: tried to apply jet resolution smearing, although metadata indicates that it already has been applied!");
    }
    ctx.set_metadata("jer_applied", "true");
}

bool JetResolutionSmearer::process(uhh2::Event & event) {
    //numbers taken from https://twiki.cern.ch/twiki/bin/view/CMS/JetResolution
    // from 13TeV JER measurement.
    constexpr const size_t n = 7;
    static float eta_hi[n] = {0.8, 1.3, 1.9, 2.5, 3.0, 3.2, 5.0};
    static float c_nominal[n] = {1.061, 1.088, 1.106, 1.126, 1.343, 1.303, 1.320};
    static float c_up[n] = {1.084, 1.117, 1.136, 1.220, 1.466, 1.414, 1.606};
    static float c_down[n] = {1.038, 1.059, 1.076, 1.032, 1.220, 1.192, 1.034};
    
    
    if(!event.jets || !event.genjets){
        throw runtime_error("JetResolutionSmearer: need jets and genjets to operate, but at least one of these is missing.");
    }
    
    LorentzVector met;
    if(event.met) {
        met = event.met->v4();
    }
    for(unsigned int i=0; i<event.jets->size(); ++i) {
        auto & jet = event.jets->at(i);
        // find next genjet:
        auto closest_genjet = closestParticle(jet, *event.genjets);
        // ignore unmatched jets (=no genjets at all or large DeltaR), or jets with very low genjet pt:
        if(closest_genjet == nullptr || deltaR(*closest_genjet, jet) > 0.3) continue;
        auto genpt = closest_genjet->pt();
        if(genpt < 15.0f) {
            continue;
        }
        LorentzVector jet_v4 = jet.v4();
        LorentzVector jet_v4_raw = jet_v4 * jet.JEC_factor_raw();
        float recopt = jet_v4.pt();
        float abseta = fabs(jet_v4.eta());
        size_t ieta = 0;
        while(ieta < n && eta_hi[ieta] < abseta) ++ieta;
        if(ieta == n) ieta = n-1;
        
        float c;
        if(direction == 0){
            c = c_nominal[ieta];
        }
        else if(direction == 1){
            c = c_up[ieta];
        }
        else{
            c = c_down[ieta];
        }
        float new_pt = std::max(0.0f, genpt + c * (recopt - genpt));
        jet_v4 *= new_pt / recopt;
        jet.set_v4(jet_v4);
    
        //propagate JER shifts to MET by using same factor, but for raw jet p4:
        if(smear_met){
            met += jet_v4_raw;
            jet_v4_raw *= new_pt / recopt;
            met -= jet_v4_raw;
        }
    }
    //store changed MET
    if(event.met) {
        event.met->set_pt(met.Pt());
        event.met->set_phi(met.Phi());
    }
    return true;
}

JetResolutionSmearer::~JetResolutionSmearer(){}

