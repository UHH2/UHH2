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

namespace {
    
// to share some code between JetCorrector and JetLeptonCleaner, provide some methods
// dealing with jet energy corrections here:
std::unique_ptr<FactorizedJetCorrector> build_corrector(const std::vector<std::string> & filenames){
    std::vector<JetCorrectorParameters> pars;
    for(const auto & filename : filenames){
        pars.emplace_back(locate_file(filename));
    }
    return make_unique<FactorizedJetCorrector>(pars);
}

void correct_jet(FactorizedJetCorrector & corrector, Jet & jet, const Event & event){
    auto factor_raw = jet.JEC_factor_raw();
    corrector.setJetPt(jet.pt() * factor_raw);
    corrector.setJetEta(jet.eta());
    corrector.setJetE(jet.energy() * factor_raw);
    corrector.setJetA(jet.jetArea());
    corrector.setRho(event.rho);
    auto correctionfactor = corrector.getCorrection();
    jet.set_v4(jet.v4() * (factor_raw * correctionfactor));
    jet.set_JEC_factor_raw(1. / correctionfactor);
}

void correct_subjets(FactorizedJetCorrector & corrector, TopJet & topjet, const Event & event){
    auto subjets_top=topjet.subjets();
    auto subjets_JECraw=topjet.subJEC_raw();
    topjet.rm_subjets();
    topjet.rm_JEC();
    for (unsigned int i=0; i<subjets_top.size();i++)
    { 
      //It is not needed to uncorrect the subjets because the ntuplewriter corrects the subjets at level 0, that is: they're not corrected.
      //However the subJEC_raw value is set to the full correction, thus by uncorrecting we would un-apply a correction that was never applied in the first place.
      //The analyzer must be careful to apply subjet JECs only *once* in the entire analysis.
      corrector.setJetPt(subjets_top[i].pt());
      corrector.setJetEta(subjets_top[i].eta());
      corrector.setJetE(subjets_top[i].energy());
      corrector.setJetA(topjet.subArea()[i]);
      corrector.setRho(event.rho);
      auto correctionfactor = corrector.getCorrection();
      subjets_top[i].set_v4(subjets_top[i].v4() * correctionfactor);
      topjet.add_subjet(subjets_top[i]);
      topjet.add_subJEC_raw(1. / correctionfactor);
    }
}



}

JetCorrector::JetCorrector(const std::vector<std::string> & filenames){
    corrector = build_corrector(filenames);
}
    
bool JetCorrector::process(uhh2::Event & event){
    assert(event.jets);
    for(auto & jet : *event.jets){
        correct_jet(*corrector, jet, event);
    }
    return true;
}

// note: implement here because only here (and not in the header file), the destructor of FactorizedJetCorrector is known
JetCorrector::~JetCorrector(){}


TopJetCorrector::TopJetCorrector(const std::vector<std::string> & filenames){
    corrector = build_corrector(filenames);
}
    
bool TopJetCorrector::process(uhh2::Event & event){
    assert(event.topjets);
    for(auto & jet : *event.topjets){
        correct_jet(*corrector, jet, event);
    }
    return true;
}

// note: implement here because only here (and not in the header file), the destructor of FactorizedJetCorrector is known
TopJetCorrector::~TopJetCorrector(){}

SubJetCorrector::SubJetCorrector(const std::vector<std::string> & filenames){
    corrector = build_corrector(filenames);
}
    
bool SubJetCorrector::process(uhh2::Event & event){
    assert(event.topjets);
    for(auto & jet : *event.topjets){
        correct_subjets(*corrector, jet, event);
    }
    return true;
}

// note: implement here because only here (and not in the header file), the destructor of FactorizedJetCorrector is known
SubJetCorrector::~SubJetCorrector(){}


// ** JetLeptonCleaner

JetLeptonCleaner::JetLeptonCleaner(const std::vector<std::string> & filenames){
    corrector = build_corrector(filenames);
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
                    correct_jet(*corrector, jet, event);
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
                    correct_jet(*corrector, jet, event);
                }
            }
        }
    }
    return true;
}

// see ~JetCorrector
JetLeptonCleaner::~JetLeptonCleaner(){}


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
    jer_applied = ctx.create_metadata<bool>("jer_applied", true);
    // make sure to get value only from input file, otherwise the assignment in 'process' would be reported as well,
    // and this class would complain about its own setting jer_applied to true.
    ctx.register_metadata_callback<bool>("jer_applied", [this](bool){this->jer_already_applied();}, Context::metadata_source_policy::infile_only);
}

void JetResolutionSmearer::jer_already_applied(){
    throw runtime_error("JetResolutionSmearer: tried to apply jet resolution smearing, although metadata indicates that it already has been applied!");
}

bool JetResolutionSmearer::process(uhh2::Event & event) {
    if(first_event){
        jer_applied = true;
        first_event = false;
    }
    //numbers taken from https://twiki.cern.ch/twiki/bin/view/CMS/JetResolution
    // from 8TeV JER measurement.
    constexpr const size_t n = 7;
    static float eta_hi[n] = {0.5, 1.1, 1.7, 2.3, 2.8, 3.2, 5.0};
    static float c_nominal[n] = {1.079, 1.099, 1.121, 1.208, 1.254, 1.395, 1.056};
    static float c_up[n] = {1.105, 1.127, 1.150, 1.254, 1.316, 1.458, 1.247};
    static float c_down[n] = {1.053, 1.071, 1.092, 1.162, 1.192, 1.332, 0.865};
    
    
    if(!event.jets || !event.genjets){
        throw runtime_error("JetResolutionSmearer: need jets and genjets to operate, but at least one of these is missing.");
    }
    
    LorentzVector met;
    if(event.met) {
        met = event.met->v4();
    }
    for(unsigned int i=0; i<event.jets->size(); ++i) {
        auto & jet = event.jets->at(i);
        jet.set_genjet(event.genjets);
        float genpt = jet.genjet_pt();
        //ignore unmatched jets (which have zero vector) or jets with very low pt:
        if(genpt < 15.0) {
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

