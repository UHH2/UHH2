#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/common/include/PrintingModules.h"
#include "UHH2/common/include/CleaningModules.h"
#include "UHH2/common/include/MuonIds.h"
#include "UHH2/common/include/Utils.h"


#include <iostream>
#include <sstream>

using namespace uhh2;
using namespace std;

namespace {

struct JetLeptonSubtractionResult {
    int matched_jet_index = -1;
    float matched_jet_dr = numeric_limits<float>::infinity();
    float matched_jet_pt_before = -1.0f;
    bool dr_ok = false;
    bool lepton_energy_ok = false;
    bool warning_jet_flipped = false;
};


JetLeptonSubtractionResult DoJetLeptonSubtraction(const Muon & mu, vector<Jet> & jets){
    JetLeptonSubtractionResult result;
    auto closestJet = closestParticle(mu, jets);
    if(closestJet == nullptr){ // no match, do nothing:
        return result;
    }
    result.matched_jet_index = closestJet - &jets[0];
    result.matched_jet_dr = deltaR(*closestJet, mu);
    result.dr_ok = result.matched_jet_dr < 0.4f;
    
    auto & jet = jets[result.matched_jet_index];
    
    auto jet_p4_raw = jet.v4() * jet.JEC_factor_raw();
    result.matched_jet_pt_before = jet_p4_raw.pt();
    // note that muon energy fraction as stored in the jet refers to the raw jet energy.
    double muon_energy_in_jet = jet_p4_raw.E() * jet.muonEnergyFraction();
    double new_muon_energy_in_jet = muon_energy_in_jet - mu.energy();
    
    // test compatibility of the hypothesis that the muon has been clustered to the jet with
    // the jet information. The hypothesis is rejected if the muon energy in the jet is too small
    // (but allow 10% off). Note that in general (for muon multiplicity > 1), the muon energy in
    // the jet might be larger than from the single muon; make sure to consider that in the test
    // by requiring one direction in the comparison only in case the muon multiplicity is 1.
    if(new_muon_energy_in_jet < -0.1 * mu.energy() || (jet.muonMultiplicity() == 1 && new_muon_energy_in_jet > 0.1 * mu.energy())){
        return result; // with lepton_energy_ok = false;
    }
    result.lepton_energy_ok = true;
    jet_p4_raw -= mu.v4();
    // if that subtraction flipped the jet direction (angle between new and old > 90 degrees or pi/2), emit a warning and set its momentum to 0.
    // Only warn if pt > 5GeV (otherwise, the jet is 0 anyway for all practical purposes).
    if(jet_p4_raw.pt() > 5 && deltaR(jet_p4_raw, jet) > M_PI/2){
        //cout << "Warning: subtracting lepton flipped jet direction" << endl;
        jet.set_v4(LorentzVector());
        result.warning_jet_flipped = true;
        return result;
    }
    // re-correct jet. First, set p4_raw = p4_corrected such that
    // the 'correct_jet' method does what it should do if using JEC_factor_raw ...
    jet.set_JEC_factor_raw(1.0);
    jet.set_v4(jet_p4_raw);
    // set new muon multiplicity and muon energy fraction:
    jet.set_muonMultiplicity(jet.muonMultiplicity() - 1);
    jet.set_muonEnergyFraction(max(new_muon_energy_in_jet / jet_p4_raw.E(), 0.0));
    // IMPORTANT: jet is not recorrected here, have to run JetCorrector, which will now do the right thing for this jet ...
    return result;
}



}



class TestJetLeptonCleaner: public uhh2::AnalysisModule {
public:
    explicit TestJetLeptonCleaner(Context & ctx) {
        jet_printer.reset(new JetPrinter("jets", 30.0));
        jet_cleaner.reset(new JetCleaner(ctx, 30.0, 2.4));
        muon_printer.reset(new MuonPrinter());
        muon_cleaner.reset(new MuonCleaner(AndId<Muon>(PtEtaCut(30., 2.4), MuonIDTight(), MuonIso(0.12))));
    }
    
    virtual bool process(Event & e) override{
        muon_cleaner->process(e);
        if(e.muons->empty()) return true;
        ++n_total;
        int imu = -1;
        vector<Jet> jets_before = *e.jets;
        vector<JetLeptonSubtractionResult> results;
        stringstream message;
        for(const auto & mu: *e.muons){
            ++imu;
            auto result = DoJetLeptonSubtraction(mu, *e.jets);
            results.push_back(result);
            if(!result.dr_ok){
                message << "Warning for muon " << imu << ": did not find any jet close enough to the muon" << endl;
            }
            else if(!result.lepton_energy_ok){
                message << "Warning for muon " << imu << ": did find nearby jet (index: " << result.matched_jet_index << ", DR=" << result.matched_jet_dr << "), but muon energy is inconsistent!" << endl;
            }
            else if(result.warning_jet_flipped){
                message << "Warning for muon " << imu << ": Jet direction flipped" << endl;
            }
        }
        
        // after all muons are processed: clean jet collection and check again:
        jet_cleaner->process(e);
        imu = -1;
        for(const auto & mu: *e.muons){
            ++imu;
            auto cp = closestParticle(mu, *e.jets);
            if(cp==nullptr) continue;
            auto dr = deltaR(mu, *cp);
            if(dr < 0.4f){
                auto & result = results[imu];
                message << "For muon " << imu << ": Subtraction performed with jet " << result.matched_jet_index << " (raw pt before = " << result.matched_jet_pt_before << "). Now closest jet to muon has pt=" << cp->pt() << "; DR=" << dr << "; ptrel = " << pTrel(mu, cp) << endl;
            }
        }
        
        string msg = message.str();
        if(!msg.empty()){
            ++n_weird;
            cout << endl << "Found suspicious event with eventid=" << e.event << ": " << endl << msg;
            auto jets_after = *e.jets;
            *e.jets = jets_before;
            muon_printer->process(e);
            jet_printer->process(e);
            cout << "Jets after jet cleaning: " << endl;
            *e.jets = jets_after;
            jet_printer->process(e);
        }
        return true;
    }
    
    virtual ~TestJetLeptonCleaner(){
        cout << "Weird events: " << n_weird << " / " << n_total << endl;
    }
    
private:
    std::unique_ptr<AnalysisModule> muon_cleaner, muon_printer, jet_printer, jet_cleaner;
    int n_weird = 0, n_total = 0;
};


UHH2_REGISTER_ANALYSIS_MODULE(TestJetLeptonCleaner)

