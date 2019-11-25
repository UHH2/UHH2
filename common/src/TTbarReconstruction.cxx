#include "UHH2/common/include/TTbarReconstruction.h"
#include "UHH2/core/include/LorentzVector.h"
#include "UHH2/common/include/Utils.h"
#include "UHH2/core/include/Utils.h"

#include <cassert>

using namespace uhh2;
using namespace std;

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

	    //search jet with highest pt assigned to leptonic top
            int blep_idx(-1);
            float maxpt(-1.);
            for(unsigned int i=0; i<hyp.toplep_jets().size(); ++i){
              if(maxpt< hyp.toplep_jets().at(i).pt()){
                maxpt = hyp.toplep_jets().at(i).pt();
                blep_idx = i;
              }
            }
            if(blep_idx != -1) hyp.set_blep_v4(hyp.toplep_jets().at(blep_idx).v4());

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

////
HighMassSkipJetsTTbarReconstruction::HighMassSkipJetsTTbarReconstruction(Context & ctx, const NeutrinoReconstructionMethod & neutrinofunction, const string & label, const int & N_skip_jets): m_neutrinofunction(neutrinofunction) {
    h_recohyps = ctx.declare_event_output<vector<ReconstructionHypothesis>>(label);
    h_primlep = ctx.get_handle<FlavorParticle>("PrimaryLepton");
    n_skip_jets_ = N_skip_jets;
}

HighMassSkipJetsTTbarReconstruction::~HighMassSkipJetsTTbarReconstruction() {}

bool HighMassSkipJetsTTbarReconstruction::process(uhh2::Event & event) {
    assert(event.jets);
    assert(event.met);

    //find primary charged lepton
    const Particle & lepton = event.get(h_primlep);
    std::vector<ReconstructionHypothesis> recoHyps;
    //reconstruct neutrino
    std::vector<LorentzVector> neutrinos = m_neutrinofunction( lepton.v4(), event.met->v4());
    //    unsigned int n_jets = event.jets->size();
    unsigned int n_jets = event.jets->size()-n_skip_jets_;
    if(n_jets>9) n_jets=9; //avoid crashes in events with many jets
    //    if(n_jets>3) n_jets=3; //TEST: speed up 
    // idea: loop over 3^Njet possibilities and write the current loop
    // index j in the 3-base system. The Njets digits represent whether
    // to assign each jet to the hadronic side (0), leptonic side (1),
    // or none of them (2).
    const unsigned int max_j = pow(3, n_jets);

    //loop over neutrino solutions and jet assignments to fill hyotheses
    //    if(neutrinos.size()<2) return false;
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
            for (unsigned int k=n_skip_jets_; k<event.jets->size(); k++) {
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

	    //search jet with highest pt assigned to leptonic top
            int blep_idx(-1);
            float maxpt(-1.);
            for(unsigned int i=0; i<hyp.toplep_jets().size(); ++i){
              if(maxpt< hyp.toplep_jets().at(i).pt()){
                maxpt = hyp.toplep_jets().at(i).pt();
                blep_idx = i;
              }
            }
            if(blep_idx != -1) hyp.set_blep_v4(hyp.toplep_jets().at(blep_idx).v4());

            //fill only hypotheses with at least one jet assigned to each top quark
            if(hadjets>0 && lepjets>0) {
                hyp.set_tophad_v4(tophad_v4);
                hyp.set_toplep_v4(toplep_v4);
		// const auto dummytop = &event.topjets->at(0);
		// hyp.set_tophad_topjet_ptr(dummytop);// FixMe: change dummy value to smth more useful?
                recoHyps.emplace_back(move(hyp));
            }
        } // 3^n_jets jet combinations
    } // neutrinos
    event.set(h_recohyps, move(recoHyps));
    return true;
}


TopTagReconstruction::TopTagReconstruction(Context & ctx, const NeutrinoReconstructionMethod & neutrinofunction, const string & label, TopJetId tjetid, float minDR_tj_j):
  m_neutrinofunction(neutrinofunction), topjetID_(tjetid), minDR_topjet_jet_(minDR_tj_j) {

  h_recohyps = ctx.declare_event_output<vector<ReconstructionHypothesis>>(label);
  h_primlep = ctx.get_handle<FlavorParticle>("PrimaryLepton");
}

bool TopTagReconstruction::process(uhh2::Event & event) {

  assert(event.jets && event.topjets);
  assert(event.met);

  std::vector<ReconstructionHypothesis> recoHyps;

  const Particle& lepton = event.get(h_primlep);
  std::vector<LorentzVector> neutrinos = m_neutrinofunction(lepton.v4(), event.met->v4());

  for(const auto& tj : *event.topjets){

    if(!topjetID_(tj, event)) continue;

    // jet candidates for leptonic-top (not overlapping with top-tagged jet)
    std::vector<const Jet*> tlep_jets;
    tlep_jets.reserve(event.jets->size());
    for(const auto & jet : *event.jets)
      if(deltaR(tj, jet) > minDR_topjet_jet_) tlep_jets.push_back(&jet);

    const unsigned int jet_combs = pow(2, tlep_jets.size());

    for(const auto& neutrino_p4 : neutrinos){

      for(unsigned int i=1; i<jet_combs; ++i){

        ReconstructionHypothesis hyp;
        hyp.set_lepton(lepton);
        hyp.set_neutrino_v4(neutrino_p4);

        LorentzVector tophad_v4(tj.v4());
        hyp.add_tophad_jet(tj);
        hyp.set_tophad_topjet_ptr(&tj);

        LorentzVector toplep_v4(lepton.v4() + neutrino_p4);

        for(unsigned int j=0; j<tlep_jets.size(); ++j){
          // index for jet assignment to top leg (0=none, 1=leptonic-top)
          int jet_topidx = int(i/(pow(2,j))) % 2;

          if(jet_topidx == 1){
            toplep_v4 += tlep_jets.at(j)->v4();
            hyp.add_toplep_jet(*tlep_jets.at(j));
          }
        }

        // b-jet of leptonic top (pt-leading)
        int blep_idx(-1);
        float maxpt(-1.);
        for(unsigned int i=0; i<hyp.toplep_jets().size(); ++i){
          if(maxpt< hyp.toplep_jets().at(i).pt()){
            maxpt = hyp.toplep_jets().at(i).pt();
            blep_idx = i;
          }
        }
        if(blep_idx != -1) hyp.set_blep_v4(hyp.toplep_jets().at(blep_idx).v4());

        if(hyp.tophad_jets().size() && hyp.toplep_jets().size()){
          hyp.set_tophad_v4(tophad_v4);
          hyp.set_toplep_v4(toplep_v4);
          recoHyps.emplace_back(std::move(hyp));
        }
      }
    }
  }

  event.set(h_recohyps, std::move(recoHyps));
  return true;
}

std::vector<LorentzVector> NeutrinoReconstruction(const LorentzVector & lepton, const LorentzVector & met) {
    LorentzVector lepton_ = lepton;
    lepton_.SetE(lepton_.P()); // set mass of lepton to 0
    TVector3 lepton_pT = toVector(lepton_);
    lepton_pT.SetZ(0);
    TVector3 neutrino_pT = toVector(met);
    neutrino_pT.SetZ(0);
    constexpr float mass_w = 80.399f;
    float mu = mass_w * mass_w / 2 + lepton_pT * neutrino_pT;
    float A = - (lepton_pT * lepton_pT);
    float B = mu * lepton_.pz();
    float C = mu * mu - lepton_.e() * lepton_.e() * (neutrino_pT * neutrino_pT);
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
