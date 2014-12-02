#include "UHH2/VLQToHiggsPairProd/include/StandardHists.h"
#include "UHH2/core/include/Event.h"

#include "TH1F.h"
#include <iostream>

using namespace std;
using namespace uhh2;

StandardHists::StandardHists(Context & ctx, const string & dirname): Hists(ctx, dirname){
  // book all histograms here
  // jets
  book<TH1F>("N_jets", "N_{jets}", 20, 0, 20);  
  book<TH1F>("eta_jet1", "#eta^{jet 1}", 40, -2.5, 2.5);
  book<TH1F>("eta_jet2", "#eta^{jet 2}", 40, -2.5, 2.5);
  book<TH1F>("eta_jet3", "#eta^{jet 3}", 40, -2.5, 2.5);
  book<TH1F>("eta_jet4", "#eta^{jet 4}", 40, -2.5, 2.5);

  // leptons
  book<TH1F>("N_mu", "N^{#mu}", 10, 0, 10);
  book<TH1F>("pt_mu", "p_{T}^{#mu} [GeV/c]", 40, 0, 200);
  book<TH1F>("eta_mu", "#eta^{#mu}", 40, -2.1, 2.1);
  book<TH1F>("reliso_mu", "#mu rel. Iso", 40, 0, 0.5);

  // primary vertices
  book<TH1F>("N_pv", "N^{PV}", 50, 0, 50);
  
  // b variables
  book<TH1F>("DeltaR_bb", "#Delta R_{bb}", 50, 0, 5);
}


void StandardHists::fill(Event & event){
    // fill the histograms. Don't forget to always use the weight when filling:
    double weight = event.weight;
    
    std::vector<Jet>* jets = event.jets;
    int Njets = jets->size();
    hist("N_jets")->Fill(Njets, weight);
    
    if(Njets>=1){
        hist("eta_jet1")->Fill(jets->at(0).eta(), weight);
    }
    if(Njets>=2){
        hist("eta_jet2")->Fill(jets->at(1).eta(), weight);
    }
    if(Njets>=3){
        hist("eta_jet3")->Fill(jets->at(2).eta(), weight);
    }
    if(Njets>=4){
        hist("eta_jet4")->Fill(jets->at(3).eta(), weight);
    }
    
    int Nmuons = event.muons->size();
    hist("N_mu")->Fill(Nmuons, weight);
    for (const Muon & thismu : *event.muons){
        hist("pt_mu")->Fill(thismu.pt(), weight);
        hist("eta_mu")->Fill(thismu.eta(), weight);
        hist("reliso_mu")->Fill(thismu.relIso(), weight);
    }
    
    int Npvs = event.pvs->size();
    hist("N_pv")->Fill(Npvs, weight);
    
    GenParticle const * b1 = 0;
    GenParticle const * b2 = 0;
    
    for (GenParticle const & igenp : *event.genparticles){
        if (abs(igenp.pdgId()) == 5){
            GenParticle const * imother = igenp.mother(event.genparticles);
            if (!imother) imother = igenp.mother(event.genparticles, 2);
            if (!imother) continue;
            else if (abs(imother->pdgId()) == 25) {
                if (!b1) b1 = &igenp;
                else if (b1->index() != igenp.index()){
                    GenParticle const * b1mother = b1->mother(event.genparticles);
                    if (!b1mother) b1mother = b1->mother(event.genparticles, 2);
                    if (b1mother->index() == imother->index()) b2 = &igenp;
                }
            }
        }
    }
    
    if (b1 && b2){
        double deltaR = b1->deltaR(*b2);
        hist("DeltaR_bb")->Fill(deltaR, weight);
    }
  
  
}

StandardHists::~StandardHists(){}
