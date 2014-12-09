#include "UHH2/common/include/JetCorrections.h"
#include "UHH2/common/include/Utils.h"
#include "UHH2/core/include/Utils.h"

#include "UHH2/JetMETObjects/interface/FactorizedJetCorrector.h"
#include "UHH2/JetMETObjects/interface/JetCorrectorParameters.h"

#include <string>

using namespace std;
using namespace uhh2;

const std::vector<std::string> JERFiles::PHYS14_L123_MC = {"JetMETObjects/data/PHYS14_50_V1_L1FastJet_AK4PFchs.txt",
  "JetMETObjects/data/PHYS14_50_V1_L2Relative_AK4PFchs.txt", "JetMETObjects/data/PHYS14_50_V1_L3Absolute_AK4PFchs.txt"};
  
const std::vector<std::string> JERFiles::PHYS14_L123_DATA = {"JetMETObjects/data/PHYS14_50_V1_L1FastJet_AK4PFchs.txt",
  "JetMETObjects/data/PHYS14_50_V1_L2Relative_AK4PFchs.txt", "JetMETObjects/data/PHYS14_50_V1_L3Absolute_AK4PFchs.txt",
  "JetMETObjects/data/PHYS14_50_V1_L2L3Residual_AK4PFchs.txt"
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
    throw runtime_error("JetResolutionSmearer: not yet implemented");
    smear_met = string2bool(ctx.get("propagate_jercorr_to_met", "false"));
}

bool JetResolutionSmearer::process(uhh2::Event & event) {
    // FIXME: implement
    return true;
}

JetResolutionSmearer::~JetResolutionSmearer(){}

