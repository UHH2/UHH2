#include "UHH2/common/include/TTbarReconstruction.h"
#include "UHH2/core/include/LorentzVector.h"
#include "UHH2/common/include/Utils.h"
#include "UHH2/core/include/Utils.h"

#include <cassert>

using namespace uhh2;
using namespace std;

PrimaryLepton::PrimaryLepton(Context & ctx) {
    h_primlep = ctx.get_handle<FlavorParticle>("PrimaryLepton");
}

bool PrimaryLepton::process(uhh2::Event & event) {
    assert(event.muons || event.electrons);
    double ptmax = -infinity;
    FlavorParticle primlep;
    if(event.electrons) {
        for(const auto & ele : *event.electrons) {
            if(ele.pt() > ptmax) {
                ptmax = ele.pt();
                primlep = ele;
            }
        }
    }
    if(event.muons) {
        for(const auto & mu : *event.muons) {
            if(mu.pt() > ptmax) {
                ptmax = mu.pt();
                primlep = mu;
            }
        }
    }
    event.set(h_primlep, std::move(primlep));
    return true;
}

PrimaryLepton::~PrimaryLepton() {}

HighMassTTbarReconstruction::HighMassTTbarReconstruction(Context & ctx, const NeutrinoReconstructionMethod & neutrinofunction, const string & label): m_neutrinofunction(neutrinofunction) {
    h_recohyps = ctx.declare_event_output<vector<ReconstructionHypothesis>>(label);
    h_primlep = ctx.get_handle<FlavorParticle>("PrimaryLepton");
}

HighMassTTbarReconstruction::~HighMassTTbarReconstruction() {}

bool HighMassTTbarReconstruction::process(uhh2::Event & event) {
    assert(event.jets);
    assert(event.met);
    //find primary charged lepton
    const Particle & lepton = event.get(h_primlep);
    std::vector<ReconstructionHypothesis> recoHyps;
    //reconstruct neutrino
    std::vector<LorentzVector> neutrinos = m_neutrinofunction( lepton.v4(), event.met->v4());
    unsigned int n_jets = event.jets->size();
    if(n_jets>10) n_jets=10; //avoid crashes in events with many jets
    // idea: loop over 3^Njet possibilities and write the current loop
    // index j in the 3-base system. The Njets digits represent whether
    // to assign each jet to the hadronic side (0), leptonic side (1),
    // or none of them (2).
    const unsigned int max_j = pow(3, n_jets);

    //loop over neutrino solutions and jet assignments to fill hyotheses
    for(const auto & neutrino_p4 : neutrinos) {
        const LorentzVector wlep_v4 = lepton.v4() + neutrino_p4;
        for (unsigned int j=0; j < max_j; j++) {
            LorentzVector tophad_v4;
            LorentzVector toplep_v4 = wlep_v4;
            int hadjets=0;
            int lepjets=0;
            int num = j;
            ReconstructionHypothesis hyp;
            hyp.set_lepton(lepton);
            hyp.set_neutrino_v4(neutrino_p4);
            for (unsigned int k=0; k<n_jets; k++) {
                if(num%3==0) {
                    tophad_v4 = tophad_v4 + event.jets->at(k).v4();
                    hyp.add_tophad_jet(event.jets->at(k));
                    hadjets++;
                }

                if(num%3==1) {
                    toplep_v4 = toplep_v4 + event.jets->at(k).v4();
                    hyp.add_toplep_jet(event.jets->at(k));
                    lepjets++;
                }
                //in case num%3==2 do not take this jet at all
                //shift the trigits of num to the right:
                num /= 3;
            }
            //fill only hypotheses with at least one jet assigned to each top quark
            if(hadjets>0 && lepjets>0) {
                hyp.set_tophad_v4(tophad_v4);
                hyp.set_toplep_v4(toplep_v4);
                recoHyps.emplace_back(move(hyp));
            }
        } // 3^n_jets jet combinations
    } // neutrinos
    event.set(h_recohyps, move(recoHyps));
    return true;
}


TopTagReconstruction::TopTagReconstruction(Context & ctx, const NeutrinoReconstructionMethod & neutrinofunction, const string & label): m_neutrinofunction(neutrinofunction) {
    h_recohyps = ctx.declare_event_output<vector<ReconstructionHypothesis>>(label);
    h_primlep = ctx.get_handle<FlavorParticle>("PrimaryLepton");
}

bool TopTagReconstruction::process(uhh2::Event &) {
    std::cerr<< "TopTagger is not yet implemented in UHH2! As soon as it is implemented you can import this function from TopFitCalc::CalculateTopTag in UHHAnalysis." << std::endl;
    return true;
}

TopTagReconstruction::~TopTagReconstruction() {}

std::vector<LorentzVector> NeutrinoReconstruction(const LorentzVector & lepton, const LorentzVector & met) {
    TVector3 lepton_pT = toVector(lepton);
    lepton_pT.SetZ(0);
    TVector3 neutrino_pT = toVector(met);
    neutrino_pT.SetZ(0);
    constexpr float mass_w = 80.399f;
    float mu = mass_w * mass_w / 2 + lepton_pT * neutrino_pT;
    float A = - (lepton_pT * lepton_pT);
    float B = mu * lepton.pz();
    float C = mu * mu - lepton.e() * lepton.e() * (neutrino_pT * neutrino_pT);
    float discriminant = B * B - A * C;
    std::vector<LorentzVector> solutions;
    if (0 >= discriminant) {
        // Take only real part of the solution for pz:
        LorentzVectorXYZE solution (met.Px(),met.Py(),-B / A,0);
        solution.SetE(solution.P());
        solutions.emplace_back(toPtEtaPhi(solution));
    }
    else {
        discriminant = sqrt(discriminant);
        LorentzVectorXYZE solution (met.Px(),met.Py(),(-B - discriminant) / A,0);
        solution.SetE(solution.P());
        solutions.emplace_back(toPtEtaPhi(solution));

        LorentzVectorXYZE solution2 (met.Px(),met.Py(),(-B + discriminant) / A,0);
        solution2.SetE(solution2.P());
        solutions.emplace_back(toPtEtaPhi(solution2));
    }
    return solutions;
}
