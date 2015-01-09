#include "UHH2/common/include/JetCorrections.h"
#include "UHH2/common/include/Utils.h"
#include "UHH2/core/include/Utils.h"

#include "UHH2/JetMETObjects/interface/FactorizedJetCorrector.h"
#include "UHH2/JetMETObjects/interface/JetCorrectorParameters.h"

#include <string>

using namespace std;
using namespace uhh2;

const std::vector<std::string> JERFiles::PHYS14_L123_MC = {"JetMETObjects/data/PHYS14_25_V2_L1FastJet_AK4PFchs.txt",
  "JetMETObjects/data/PHYS14_25_V2_L2Relative_AK4PFchs.txt", "JetMETObjects/data/PHYS14_25_V2_L3Absolute_AK4PFchs.txt"};
  
const std::vector<std::string> JERFiles::PHYS14_L123_DATA = {"JetMETObjects/data/PHYS14_25_V2_L1FastJet_AK4PFchs.txt",
  "JetMETObjects/data/PHYS14_25_V2_L2Relative_AK4PFchs.txt", "JetMETObjects/data/PHYS14_25_V2_L3Absolute_AK4PFchs.txt",
  "JetMETObjects/data/PHYS14_50_V1_L2L3Residual_AK4PFchs.txt" // note: L2L3Residual corrections not implemented in global tag PHYS14_25_V2
};

JetCorrector::JetCorrector(const std::vector<std::string> & filenames){
    std::vector<JetCorrectorParameters> pars;
    //see https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookJetEnergyCorrections#GetTxtFiles how to get the txt files with jet energy corrections from the database
    for(const auto & filename : filenames){
        pars.emplace_back(locate_file(filename));
    }
    corrector.reset(new FactorizedJetCorrector(pars));
}
    
bool JetCorrector::process(uhh2::Event & event){
    assert(event.jets);
    for(auto & jet : *event.jets){
        auto factor_raw = jet.JEC_factor_raw();
        corrector->setJetPt(jet.pt() * factor_raw);
        corrector->setJetEta(jet.eta());
        corrector->setJetE(jet.energy() * factor_raw);
        corrector->setJetA(jet.jetArea());
        corrector->setRho(event.rho);
        auto correctionfactor = corrector->getCorrection();
        jet.set_v4(jet.v4() * (factor_raw * correctionfactor));
        jet.set_JEC_factor_raw(1. / correctionfactor);
    }
    return true;
}

JetCorrector::~JetCorrector(){}

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
    ctx.register_metadata_callback<bool>("jer_applied", [this](bool v){this->set_jer_applied(v);}, Context::metadata_source_policy::infile_only);
}

void JetResolutionSmearer::set_jer_applied(bool v){
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

