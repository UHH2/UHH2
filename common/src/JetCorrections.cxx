#include "UHH2/common/include/JetCorrections.h"
#include "UHH2/common/include/Utils.h"
#include "UHH2/core/include/Utils.h"

#include "UHH2/JetMETObjects/interface/FactorizedJetCorrector.h"
#include "UHH2/JetMETObjects/interface/JetCorrectorParameters.h"

#include <string>

using namespace std;
using namespace uhh2;

//see https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookJetEnergyCorrections#GetTxtFiles how to get the txt files with jet energy corrections from the database

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

//Fall15_25nsV2
const std::vector<std::string> JERFiles::Fall15_25ns_L123_AK4PFchs_MC = {
  "JetMETObjects/data/Fall15_25nsV2_MC_L1FastJet_AK4PFchs.txt",
  "JetMETObjects/data/Fall15_25nsV2_MC_L2Relative_AK4PFchs.txt",
  "JetMETObjects/data/Fall15_25nsV2_MC_L3Absolute_AK4PFchs.txt"
};

const std::vector<std::string> JERFiles::Fall15_25ns_L123_AK8PFchs_MC = {
  "JetMETObjects/data/Fall15_25nsV2_MC_L1FastJet_AK8PFchs.txt",
  "JetMETObjects/data/Fall15_25nsV2_MC_L2Relative_AK8PFchs.txt",
  "JetMETObjects/data/Fall15_25nsV2_MC_L3Absolute_AK8PFchs.txt"
};

const std::vector<std::string> JERFiles::Fall15_25ns_L23_AK8PFchs_MC = {
  "JetMETObjects/data/Fall15_25nsV2_MC_L2Relative_AK8PFchs.txt",
  "JetMETObjects/data/Fall15_25nsV2_MC_L3Absolute_AK8PFchs.txt"
};

const std::vector<std::string> JERFiles::Fall15_25ns_L123_AK4PFPuppi_MC = {
  "JetMETObjects/data/Fall15_25nsV2_MC_L1FastJet_AK4PFPuppi.txt",
  "JetMETObjects/data/Fall15_25nsV2_MC_L2Relative_AK4PFPuppi.txt",
  "JetMETObjects/data/Fall15_25nsV2_MC_L3Absolute_AK4PFPuppi.txt"
};

const std::vector<std::string> JERFiles::Fall15_25ns_L123_AK8PFPuppi_MC = {
  "JetMETObjects/data/Fall15_25nsV2_MC_L1FastJet_AK8PFPuppi.txt",
  "JetMETObjects/data/Fall15_25nsV2_MC_L2Relative_AK8PFPuppi.txt",
  "JetMETObjects/data/Fall15_25nsV2_MC_L3Absolute_AK8PFPuppi.txt"
};

const std::vector<std::string> JERFiles::Fall15_25ns_L123_AK4PFchs_DATA = {
  "JetMETObjects/data/Fall15_25nsV2_DATA_L1FastJet_AK4PFchs.txt",
  "JetMETObjects/data/Fall15_25nsV2_DATA_L2Relative_AK4PFchs.txt",
  "JetMETObjects/data/Fall15_25nsV2_DATA_L3Absolute_AK4PFchs.txt",
  "JetMETObjects/data/Fall15_25nsV2_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall15_25ns_L123_AK8PFchs_DATA = {
  "JetMETObjects/data/Fall15_25nsV2_DATA_L1FastJet_AK8PFchs.txt",
  "JetMETObjects/data/Fall15_25nsV2_DATA_L2Relative_AK8PFchs.txt",
  "JetMETObjects/data/Fall15_25nsV2_DATA_L3Absolute_AK8PFchs.txt",
  "JetMETObjects/data/Fall15_25nsV2_DATA_L2L3Residual_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall15_25ns_L23_AK8PFchs_DATA = {
  "JetMETObjects/data/Fall15_25nsV2_DATA_L2Relative_AK8PFchs.txt",
  "JetMETObjects/data/Fall15_25nsV2_DATA_L3Absolute_AK8PFchs.txt",
  "JetMETObjects/data/Fall15_25nsV2_DATA_L2L3Residual_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall15_25ns_L123_AK4PFPuppi_DATA = {
  "JetMETObjects/data/Fall15_25nsV2_DATA_L1FastJet_AK4PFPuppi.txt",
  "JetMETObjects/data/Fall15_25nsV2_DATA_L2Relative_AK4PFPuppi.txt",
  "JetMETObjects/data/Fall15_25nsV2_DATA_L3Absolute_AK4PFPuppi.txt",
  "JetMETObjects/data/Fall15_25nsV2_DATA_L2L3Residual_AK4PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Fall15_25ns_L123_AK8PFPuppi_DATA = {
  "JetMETObjects/data/Fall15_25nsV2_DATA_L1FastJet_AK8PFPuppi.txt",
  "JetMETObjects/data/Fall15_25nsV2_DATA_L2Relative_AK8PFPuppi.txt",
  "JetMETObjects/data/Fall15_25nsV2_DATA_L3Absolute_AK8PFPuppi.txt",
  "JetMETObjects/data/Fall15_25nsV2_DATA_L2L3Residual_AK8PFPuppi.txt",
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
const JERSmearing::SFtype1 JERSmearing::SF_13TeV_2015 = {
  // 0 = upper jet-eta limit
  // 1 = JER SF
  // 2 = JER SF + 1sigma
  // 3 = JER SF - 1sigma
  {{0.5, 1.095, 1.113, 1.077}},
  {{0.8, 1.120, 1.148, 1.092}},
  {{1.1, 1.097, 1.114, 1.080}},
  {{1.3, 1.103, 1.136, 1.070}},
  {{1.7, 1.118, 1.132, 1.104}},
  {{1.9, 1.100, 1.133, 1.067}},
  {{2.1, 1.162, 1.206, 1.116}},
  {{2.3, 1.160, 1.208, 1.112}},
  {{2.5, 1.161, 1.221, 1.101}},
  {{2.8, 1.209, 1.268 ,1.150}},
  {{3.0, 1.564, 1.885, 1.243}},
  {{3.2, 1.384, 1.417, 1.351}},
  {{5.0, 1.216, 1.266, 1.166}},
};

////

JetResolutionSmearer::JetResolutionSmearer(uhh2::Context & ctx, const JERSmearing::SFtype1& JER_sf){
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

    JER_SFs_ = JER_sf;
}

bool JetResolutionSmearer::process(uhh2::Event & event) {

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

        int ieta(-1);
        for(unsigned int idx=0; idx<JER_SFs_.size(); ++idx){

          const float min_eta = idx ? JER_SFs_.at(idx-1).at(0) : 0.;
          const float max_eta =       JER_SFs_.at(idx)  .at(0);

          if(min_eta <= abseta && abseta < max_eta){ ieta = idx; break; }
        }
        if(ieta < 0) throw std::runtime_error("JetResolutionSmearer: index for JER-smearing SF not found");

        float c;
        if(direction == 0){
            c = JER_SFs_.at(ieta).at(1);
        }
        else if(direction == 1){
            c = JER_SFs_.at(ieta).at(2);
        }
        else{
            c = JER_SFs_.at(ieta).at(3);
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

////

GenericJetResolutionSmearer::GenericJetResolutionSmearer(uhh2::Context& ctx, const std::string& recjet_label, const std::string& genjet_label, const bool allow_met_smearing, const JERSmearing::SFtype1& JER_sf){

  if(ctx.get("meta_jer_applied__"+recjet_label, "") != "true") ctx.set_metadata("jer_applied__"+recjet_label, "true");
  else throw std::runtime_error("GenericJetResolutionSmearer::GenericJetResolutionSmearer -- JER smearing already applied to this RECO-jets collection: "+recjet_label);

  const std::string& dir = ctx.get("jersmear_direction", "nominal");
  if     (dir == "nominal") direction =  0;
  else if(dir == "up")      direction =  1;
  else if(dir == "down")    direction = -1;
  else throw std::runtime_error("GenericJetResolutionSmearer::GenericJetResolutionSmearer -- invalid value jersmear_direction='"+dir+"' (valid: 'nominal', 'up', 'down')");

  h_recjets_    = ctx.get_handle<std::vector<Jet> >      (recjet_label);
  h_rectopjets_ = ctx.get_handle<std::vector<TopJet> >   (recjet_label);

  h_genjets_    = ctx.get_handle<std::vector<Particle> > (genjet_label);
  h_gentopjets_ = ctx.get_handle<std::vector<GenTopJet> >(genjet_label);

  smear_met = allow_met_smearing ? string2bool(ctx.get("jersmear_smear_met", "false")) : false;

  if(smear_met){

    if(ctx.get("meta_jer_applied_on_met", "") != "true") ctx.set_metadata("jer_applied_on_met", "true");
    else throw std::runtime_error("GenericJetResolutionSmearer::GenericJetResolutionSmearer -- JER smearing already propagated to MET measurement: jet_label="+recjet_label);
  }

  JER_SFs_ = JER_sf;
}

bool GenericJetResolutionSmearer::process(uhh2::Event& evt){

  if(evt.isRealData) return true;

  std::vector<Jet>*    rec_jets(0);
  std::vector<TopJet>* rec_topjets(0);

  if     (evt.is_valid(h_recjets_))    rec_jets    = &evt.get(h_recjets_);
  else if(evt.is_valid(h_rectopjets_)) rec_topjets = &evt.get(h_rectopjets_);
  else throw std::runtime_error("GenericJetResolutionSmearer::process -- invalid handle to RECO-jets");

  std::vector<Particle>*  gen_jets(0);
  std::vector<GenTopJet>* gen_topjets(0);

  if     (evt.is_valid(h_genjets_))    gen_jets    = &evt.get(h_genjets_);
  else if(evt.is_valid(h_gentopjets_)) gen_topjets = &evt.get(h_gentopjets_);
  else throw std::runtime_error("GenericJetResolutionSmearer::process -- invalid handle to GEN-jets");

  LorentzVector met;
  if(evt.met) met = evt.met->v4();

  if     (rec_jets    && gen_jets)    apply_JER_smearing(*rec_jets   , *gen_jets   , met);
  else if(rec_topjets && gen_jets)    apply_JER_smearing(*rec_topjets, *gen_jets   , met);
  else if(rec_topjets && gen_topjets) apply_JER_smearing(*rec_topjets, *gen_topjets, met);
  else throw std::runtime_error("GenericJetResolutionSmearer::process -- invalid combination of RECO-GEN jet collections");

  // update MET
  if(evt.met && smear_met){

    evt.met->set_pt (met.Pt());
    evt.met->set_phi(met.Phi());
  }

  return true;
}

//// -----------------------------------------------------------------
