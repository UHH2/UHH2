#include "UHH2/VLQToHiggsPairProd/include/GenHists.h"
#include "UHH2/core/include/Event.h"

#include "TH1F.h"
#include <iostream>

using namespace std;
using namespace uhh2;

GenHists::GenHists(Context & ctx, const string & dirname): Hists(ctx, dirname){
  // book all histograms here
  // kinematical variables 
  book<TH1F>("eta_lead", "#eta_{Tp}(lead)", 40, -2.5, 2.5);
  book<TH1F>("eta_subl", "#eta_{Tp}(sublead)", 40, -2.5, 2.5);
  
  book<TH1F>("pt_lead", "p_{T}_{Tp}(lead) [GeV/c]", 150, 0, 1500);
  book<TH1F>("pt_subl", "p_{T}_{Tp}(sublead) [GeV/c]", 150, 0, 1500);
  
  book<TH1F>("phi_lead", "#phi_{Tp}(lead)", 64, -3.2, 3.2);
  book<TH1F>("phi_subl", "#phi_{Tp}(sublead)", 64, -3.2, 3.2);
  
  // decay modes
  book<TH1F>("tp_decay", "Tprime decay modes", 30, 0, 30);
  book<TH1F>("h_decay", "Higgs decay modes", 30, 0, 30);
  
  // b variables
  book<TH1F>("DeltaR_bb", "#Delta R_{bb}", 50, 0, 5);
}


void GenHists::fill(Event & event){
    // fill the histograms. Don't forget to always use the weight when filling:
//     double weight = event.weight;
    
    std::vector<GenParticle> const * genparticles = event.genparticles;
    
    GenParticle const * tp1 = 0;
    GenParticle const * tp2 = 0;
    GenParticle const * h1 = 0;
    GenParticle const * h2 = 0;
    GenParticle const * b1 = 0;
    GenParticle const * b2 = 0;
    
    for (GenParticle const & igenp : *genparticles){
        if (abs(igenp.pdgId()) == 8){
            if (!tp1) tp1 = &igenp;
            else if (!tp2) tp2 = &igenp;
        }
        if (abs(igenp.pdgId()) == 25 && abs(igenp.mother(genparticles)->pdgId()) == 8){
            if (!h1) h1 = &igenp;
            else if (!h2) h2 = &igenp;
        }
        if (abs(igenp.pdgId()) == 5){
            GenParticle const * imother = igenp.mother(genparticles);
            if (!imother) imother = igenp.mother(genparticles, 2);
            if (!imother) continue;
            else if (abs(imother->pdgId()) == 25) {
                if (!b1) b1 = &igenp;
                else if (b1->index() != igenp.index()){
                    GenParticle const * b1mother = b1->mother(genparticles);
                    if (!b1mother) b1mother = b1->mother(genparticles, 2);
                    if (b1mother->index() == imother->index()) b2 = &igenp;
                }
            }
        }
    }
    
    double pt_lead = 0.;
    double pt_subl = 0.;
    double eta_lead = 0.;
    double eta_subl = 0.;
    double phi_lead = 0.;
    double phi_subl = 0.;
    
    if (tp1) {
        pt_lead = tp1->pt();
        eta_lead = tp1->eta();
        phi_lead = tp1->phi();
        GenParticle const * daughter1 = tp1->daughter(genparticles);
        GenParticle const * daughter2 = tp1->daughter(genparticles, 2);
        int decay1 = 0;
        int decay2 = 0;
        if (daughter1) decay1 = abs(daughter1->pdgId());
        if (daughter2) decay2 = abs(daughter2->pdgId());
        hist("tp_decay")->Fill(decay1);
        hist("tp_decay")->Fill(decay2);
    }
    
    if (tp2) {
        if (tp2->pt() > pt_lead) {
            pt_lead = tp2->pt();
            eta_lead = tp2->eta();
            phi_lead = tp2->phi();
            pt_subl = tp1->pt();
            eta_subl = tp1->eta();
            phi_subl = tp1->phi();
        }
        else {
            pt_subl = tp2->pt();
            eta_subl = tp2->eta();
            phi_subl = tp2->phi();
        }
        GenParticle const * daughter1 = tp2->daughter(genparticles);
        GenParticle const * daughter2 = tp2->daughter(genparticles, 2);
        int decay1 = 0;
        int decay2 = 0;
        if (daughter1) decay1 = abs(daughter1->pdgId());
        if (daughter2) decay2 = abs(daughter2->pdgId());
        hist("tp_decay")->Fill(decay1);
        hist("tp_decay")->Fill(decay2);
    }
    
//     cout << "Test0" << endl;
    hist("pt_lead")->Fill(pt_lead);
    hist("pt_subl")->Fill(pt_subl);
    hist("eta_lead")->Fill(eta_lead);
    hist("eta_subl")->Fill(eta_subl);
    hist("phi_lead")->Fill(phi_lead);
    hist("phi_subl")->Fill(phi_subl);
//     cout << "Test1" << endl;
    
    if (h1){
        GenParticle const * daughter1 = h1->daughter(genparticles);
        GenParticle const * daughter2 = h1->daughter(genparticles, 2);
        int decay1 = 0;
        int decay2 = 0;
        if (daughter1) decay1 = abs(daughter1->pdgId());
        if (daughter2) decay2 = abs(daughter2->pdgId());
        hist("h_decay")->Fill(decay1);
        hist("h_decay")->Fill(decay2);
    }
    
    if (h2){
        GenParticle const * daughter1 = h2->daughter(genparticles);
        GenParticle const * daughter2 = h2->daughter(genparticles, 2);
        int decay1 = 0;
        int decay2 = 0;
        if (daughter1) decay1 = abs(daughter1->pdgId());
        if (daughter2) decay2 = abs(daughter2->pdgId());
        hist("h_decay")->Fill(decay1);
        hist("h_decay")->Fill(decay2);
    }
    
    if (b1 && b2){
        double deltaR = b1->deltaR(*b2);
//         cout << "Test2" << endl;
        hist("DeltaR_bb")->Fill(deltaR);
//         cout << "Test3" << endl;
    }
    
//     cout << "Test4" << endl;
  
  
}

GenHists::~GenHists(){}
