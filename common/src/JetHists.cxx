#include "UHH2/common/include/JetHists.h"
#include "UHH2/common/include/Utils.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/LorentzVector.h"

#include "TH1F.h"
#include "TH2F.h"

#include <vector>

using namespace uhh2;
using namespace std;

JetHists::JetHists(Context & ctx, const std::string & dname): Hists(ctx, dname){
    number = book<TH1F>("number","number of jets",20, 0, 20);
    
    pt = book<TH1F>("pt","p_{T} all jets",50,20,1500);
    eta = book<TH1F>("eta","#eta all jets",50,-3,3);
    phi = book<TH1F>("phi","#phi all jets",50,-M_PI,M_PI);
    
    pt_1 = book<TH1F>("pt_1","p_{T} jet 1",50,100,1500);
    eta_1 = book<TH1F>("eta_1","#eta jet 1",50,-3,3);
    phi_1 = book<TH1F>("phi_1","#phi jet 1",50,-M_PI,M_PI);
    deltaRmin_1 = book<TH1F>("deltaRmin_1", "#Delta R_{min}(first jet,nearest jet)", 40, 0, 2.0);
    
    pt_2 = book<TH1F>("pt_2","p_{T} jet 2",50,20,1000);
    eta_2 = book<TH1F>("eta_2","#eta jet 2",50,-3,3);
    phi_2 = book<TH1F>("phi_2","#phi jet 2",50,-M_PI,M_PI);
    deltaRmin_2 = book<TH1F>("deltaRmin_2", "#Delta R_{min}(2nd jet,nearest jet)", 40, 0, 2.0);

    pt_3 = book<TH1F>("pt_3","p_{T} jet 3",50,20,500);
    eta_3 = book<TH1F>("eta_3","#eta jet 3",50,-3,3);
    phi_3 = book<TH1F>("phi_3","#phi jet 3",50,-M_PI,M_PI);

    pt_4 = book<TH1F>("pt_4","p_{T} jet 4",50,20,250);
    eta_4 = book<TH1F>("eta_4","#eta jet 4",50,-3,3);
    phi_4 = book<TH1F>("phi_4","#phi jet 4",50,-M_PI,M_PI);

    m_1 = book<TH1F>("m_1","M^{jet 1} [GeV/c^{2}]", 100, 0, 300);
    m_2 = book<TH1F>("m_2","M^{jet 2} [GeV/c^{2}]", 100, 0, 300);
    m_3 = book<TH1F>("m_3","M^{jet 3} [GeV/c^{2}]", 100, 0, 300);
    m_4 = book<TH1F>("m_4","M^{jet 4} [GeV/c^{2}]", 100, 0, 300);

}

void JetHists::fill(const Event & event){
    auto w = event.weight;
    assert(event.jets);
    number->Fill(event.jets->size(), w);
    
    for(const auto & jet : *event.jets){
        pt->Fill(jet.pt(), w);
        eta->Fill(jet.eta(), w);
        phi->Fill(jet.phi(), w);
    }
    
    if(event.jets->size() > 0){
        const auto & jet = (*event.jets)[0];
        pt_1->Fill(jet.pt(), w);
        eta_1->Fill(jet.eta(), w);
        phi_1->Fill(jet.phi(), w);
        m_1->Fill(jet.v4().M(), w);
        auto next_jet = nextJet(jet, *event.jets);
        auto drmin = next_jet ? deltaR(jet, *next_jet) : numeric_limits<float>::infinity();
        deltaRmin_1->Fill(drmin, w);
    }
    if(event.jets->size() > 1){
        const auto & jet = (*event.jets)[1];
        pt_2->Fill(jet.pt(), w);
        eta_2->Fill(jet.eta(), w);
        phi_2->Fill(jet.phi(), w);
        m_2->Fill(jet.v4().M(), w);
        auto next_jet = nextJet(jet, *event.jets);
        auto drmin = next_jet ? deltaR(jet, *next_jet) : numeric_limits<float>::infinity();
        deltaRmin_2->Fill(drmin, w);
    }
    
    if(event.jets->size() > 2){
        const auto & jet = (*event.jets)[2];
        pt_3->Fill(jet.pt(), w);
        eta_3->Fill(jet.eta(), w);
        phi_3->Fill(jet.phi(), w);
        m_3->Fill(jet.v4().M(), w);
    }
    if(event.jets->size() > 3){
        const auto & jet = (*event.jets)[3];
        pt_4->Fill(jet.pt(), w);
        eta_4->Fill(jet.eta(), w);
        phi_4->Fill(jet.phi(), w);
        m_4->Fill(jet.v4().M(), w);
    }
    
}
