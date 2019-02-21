#include <iostream>
#include <memory>

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/Hists.h"

#include "TH1F.h"
#include "TH2F.h"

using namespace std;
using namespace uhh2;

/** \brief Example module to test jet constituents & plot relative difference 
 * between the original jets and jets reconstructed from their constituents.
 */

namespace uhh2examples {


class ExampleJetConstitHists: public uhh2::Hists {
public:
    ExampleJetConstitHists(uhh2::Context & ctx, const std::string & dirname);
    virtual void fill(const uhh2::Event & ev) override; // need this to override abstract base method
    virtual void fill(const LorentzVector & lvJet, const LorentzVector & lvSum);
    virtual ~ExampleJetConstitHists();
private:
    TH1F * hPtCompare, * hEtaCompare, * hPhiCompare;
    vector<float> logBins = {
       1.00000000e-10, 1.23026877e-10, 1.51356125e-10, 1.86208714e-10,
       2.29086765e-10, 2.81838293e-10, 3.46736850e-10, 4.26579519e-10,
       5.24807460e-10, 6.45654229e-10, 7.94328235e-10, 9.77237221e-10,
       1.20226443e-09, 1.47910839e-09, 1.81970086e-09, 2.23872114e-09,
       2.75422870e-09, 3.38844156e-09, 4.16869383e-09, 5.12861384e-09,
       6.30957344e-09, 7.76247117e-09, 9.54992586e-09, 1.17489755e-08,
       1.44543977e-08, 1.77827941e-08, 2.18776162e-08, 2.69153480e-08,
       3.31131121e-08, 4.07380278e-08, 5.01187234e-08, 6.16595002e-08,
       7.58577575e-08, 9.33254301e-08, 1.14815362e-07, 1.41253754e-07,
       1.73780083e-07, 2.13796209e-07, 2.63026799e-07, 3.23593657e-07,
       3.98107171e-07, 4.89778819e-07, 6.02559586e-07, 7.41310241e-07,
       9.12010839e-07, 1.12201845e-06, 1.38038426e-06, 1.69824365e-06,
       2.08929613e-06, 2.57039578e-06, 3.16227766e-06, 3.89045145e-06,
       4.78630092e-06, 5.88843655e-06, 7.24435960e-06, 8.91250938e-06,
       1.09647820e-05, 1.34896288e-05, 1.65958691e-05, 2.04173794e-05,
       2.51188643e-05, 3.09029543e-05, 3.80189396e-05, 4.67735141e-05,
       5.75439937e-05, 7.07945784e-05, 8.70963590e-05, 1.07151931e-04,
       1.31825674e-04, 1.62181010e-04, 1.99526231e-04, 2.45470892e-04,
       3.01995172e-04, 3.71535229e-04, 4.57088190e-04, 5.62341325e-04,
       6.91830971e-04, 8.51138038e-04, 1.04712855e-03, 1.28824955e-03,
       1.58489319e-03, 1.94984460e-03, 2.39883292e-03, 2.95120923e-03,
       3.63078055e-03, 4.46683592e-03, 5.49540874e-03, 6.76082975e-03,
       8.31763771e-03, 1.02329299e-02, 1.25892541e-02, 1.54881662e-02,
       1.90546072e-02, 2.34422882e-02, 2.88403150e-02, 3.54813389e-02,
       4.36515832e-02, 5.37031796e-02, 6.60693448e-02, 8.12830516e-02,
       1.00000000e-01}; // if it goes > 0.1 you have a serious issue!
};

ExampleJetConstitHists::ExampleJetConstitHists(Context & ctx, const string & dirname):
Hists(ctx, dirname)
{
    // These hists should be viewed with log-log axes, since they can cover many orders of magnitude in both axes
    hPtCompare = book<TH1F>("pt_compare", ";Relative |p_{T}^{Jet} - p_{T}^{#sum Constit}|;N", logBins.size()-1, &logBins[0]);
    hEtaCompare = book<TH1F>("eta_compare", ";Relative |#eta^{Jet} - #eta^{#sum Constit}|;N", logBins.size()-1, &logBins[0]);
    hPhiCompare = book<TH1F>("phi_compare", ";Relative |#phi^{Jet} - #phi^{#sum Constit}|;N", logBins.size()-1, &logBins[0]);
}


void ExampleJetConstitHists::fill(const LorentzVector & lvJet, const LorentzVector & lvSum) {
    hPtCompare->Fill(fabs(lvJet.pt() - lvSum.pt()) / std::max(fabs(lvJet.pt()), fabs(lvSum.pt())));
    hEtaCompare->Fill(fabs(lvJet.eta() - lvSum.eta()) / std::max(fabs(lvJet.eta()), fabs(lvSum.eta())));
    hPhiCompare->Fill(fabs(lvJet.phi() - lvSum.phi()) / std::max(fabs(lvJet.phi()), fabs(lvSum.phi())));
}

void ExampleJetConstitHists::fill(const Event & event){
  (void)event; // unused var
}

ExampleJetConstitHists::~ExampleJetConstitHists(){}


class ExampleModuleJetConstituents: public AnalysisModule {
public:

    explicit ExampleModuleJetConstituents(Context & ctx);
    virtual bool process(Event & event) override;
    static LorentzVector constructConstituentSum(std::vector<PFParticle> * pfparticles, Jet * jet);
    static LorentzVector constructConstituentSum(std::vector<GenParticle> * genparticles, GenJet * genjet);
    static bool isClose(float a, float b, float relDiff=1E-2); // loose tolerance as often packing loses some precision
    static bool compareLVs(const LorentzVector & lv1, const LorentzVector & lv2);
private:
    unique_ptr<ExampleJetConstitHists> jetHists, topjetHists, genjetHists, gentopjetHists;

};


ExampleModuleJetConstituents::ExampleModuleJetConstituents(Context & ctx)
{
    cout << "Hello World from ExampleModuleJetConstituents!" << endl;
    jetHists.reset(new ExampleJetConstitHists(ctx, "jetHists"));
    topjetHists.reset(new ExampleJetConstitHists(ctx, "topjetHists"));
    genjetHists.reset(new ExampleJetConstitHists(ctx, "genjetHists"));
    gentopjetHists.reset(new ExampleJetConstitHists(ctx, "gentopjetHists"));
}


LorentzVector ExampleModuleJetConstituents::constructConstituentSum(std::vector<PFParticle> * pfparticles, Jet * jet) {
    if (pfparticles == nullptr) { throw std::runtime_error("pfparticles is nullptr"); }
    if (jet == nullptr) { throw std::runtime_error("Jet is nullptr"); }

    LorentzVector consistSum;
    for (const auto candInd : jet->pfcand_indexs()) {
        consistSum += pfparticles->at(candInd).v4();
    }
    return consistSum;
}

LorentzVector ExampleModuleJetConstituents::constructConstituentSum(std::vector<GenParticle> * genparticles, GenJet * genjet) {
    if (genparticles == nullptr) { throw std::runtime_error("genparticles is nullptr"); }
    if (genjet == nullptr) { throw std::runtime_error("genjet is nullptr"); }

    LorentzVector consistSum;
    for (const auto candInd : genjet->genparticles_indices()) {
        consistSum += genparticles->at(candInd).v4();
    }
    return consistSum;
}

bool ExampleModuleJetConstituents::isClose(float a, float b, float relDiff) {
    return fabs(a-b) <= (relDiff * std::max(fabs(a), fabs(b)));
}

bool ExampleModuleJetConstituents::compareLVs(const LorentzVector & lv1, const LorentzVector & lv2) {
    if (!(   isClose(lv1.pt(), lv2.pt())
          && isClose(lv1.eta(), lv2.eta())
          && isClose(lv1.phi(), lv2.phi()))) {
        cout << "Sum jet constituents != jet" << endl;
        cout << "  - Jet: pt: " << lv1.pt() << " eta: " << lv1.eta() << " phi: " << lv1.phi() << endl;
        cout << "  - Sum: pt: " << lv2.pt() << " eta: " << lv2.eta() << " phi: " << lv2.phi() << endl;
        return false;
    }
    return true;
}

bool ExampleModuleJetConstituents::process(Event & event) {
    cout << "*** EVENT: " << event.event << endl;

    if (event.jets != nullptr) {
        cout << "--- Jets: " << event.jets->size() << endl;
        for (auto & jetItr : *event.jets) {
            auto sumJet = constructConstituentSum(event.pfparticles, &jetItr);
            LorentzVector rawJet = jetItr.v4() * jetItr.JEC_factor_raw();  // NB need uncorrected jet
            compareLVs(rawJet, sumJet);
            jetHists->fill(rawJet, sumJet);
        }
    }

    if (event.topjets != nullptr) {
        cout << "--- TopJets: " << event.topjets->size() << endl;
        for (auto & jetItr : *event.topjets) {
            auto sumJet = constructConstituentSum(event.pfparticles, &jetItr);
            LorentzVector rawJet = jetItr.v4() * jetItr.JEC_factor_raw();  // NB need uncorrected jet
            compareLVs(rawJet, sumJet);
            topjetHists->fill(rawJet, sumJet);
        }
    }

    if (event.genjets != nullptr) {
        cout << "--- GenJets: " << event.genjets->size() << endl;
        for (auto & jetItr : *event.genjets) {
            auto sumJet = constructConstituentSum(event.genparticles, &jetItr);
            compareLVs(jetItr.v4(), sumJet);
            genjetHists->fill(jetItr.v4(), sumJet);
        }
    }

    if (event.gentopjets != nullptr) {
        cout << "--- GenTopJets: " << event.gentopjets->size() << endl;
        for (auto & jetItr : *event.gentopjets) {
            auto sumJet = constructConstituentSum(event.genparticles, &jetItr);
            compareLVs(jetItr.v4(), sumJet);
            gentopjetHists->fill(jetItr.v4(), sumJet);
        }
    }
    return true;
}

// as we want to run the ExampleCycleNew directly with AnalysisModuleRunner,
// make sure the ExampleModuleJetConstituents is found by class name. This is ensured by this macro:
UHH2_REGISTER_ANALYSIS_MODULE(ExampleModuleJetConstituents)

}