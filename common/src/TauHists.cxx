#include "UHH2/common/include/TauHists.h"
#include "UHH2/common/include/Utils.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/LorentzVector.h"

#include "TH1F.h"
#include "TH2F.h"

#include <vector>

using namespace uhh2;
using namespace std;

TauHists::TauHists(Context & ctx, const std::string & dname): Hists(ctx, dname){
    number = book<TH1F>("number","number of taus",7,-0.5,6.5);
    
    pt = book<TH1F>("pt","p_{T} tau",100,0,500);
    eta = book<TH1F>("eta","#eta tau",100,-3,3);
    phi = book<TH1F>("phi","#phi tau",100,-M_PI,M_PI);
    charge = book<TH1F>("charge", "tau charge",3,-1.5,1.5);
    ptrel = book<TH1F>("ptrel", "p_{T}^{rel}(tau,jet)", 40, 0, 200.);
    deltaRmin = book<TH1F>("deltaRmin", "#Delta R_{min}(tau,jet)", 40, 0, 2.0);
    deltaRmin_ptrel = book<TH2F>("deltaRmin_ptrel", "x=#Delta R_{min}(tau,jet) y=p_{T}^{rel}(tau,jet)", 40, 0, 2.0, 40, 0, 200.);
    
    Double_t bins[5] = {20, 60, 120, 200, 800};
    pt_1 = book<TH1F>("pt_1","p_{T} tau 1",50,0,800);
    pt_1_binned = book<TH1F>("pt_1_binned","p_{T} tau 1",4,bins);
    pt_muon1_tau1 = book<TH1F>("pt_muon1_tau1","p_{T} leading tau + p_{T} leading muon",50,0,800);
    pt_muon1_tau1_binned = book<TH1F>("pt_muon1_tau1_binned","p_{T} leading tau + p_{T} leading muon",4,bins);
    eta_1 = book<TH1F>("eta_1","#eta tau 1",100,-3,3);
    phi_1 = book<TH1F>("phi_1","#phi tau 1",100,-M_PI,M_PI);
    charge_1 = book<TH1F>("charge_1", "tau 1 charge",3,-1.5,1.5);
    ptrel_1 = book<TH1F>("ptrel_1", "p_{T}^{rel}(tau 1,jet)", 40, 0, 200.);
    deltaRmin_1 = book<TH1F>("deltaRmin_1", "#Delta R_{min}(tau 1,jet)", 40, 0, 2.0);
    deltaRmin_ptrel_1 = book<TH2F>("deltaRmin_ptrel_1", "x=#Delta R_{min}(e 1,jet) y=p_{T}^{rel}(tau 1,jet)", 40, 0, 2.0, 40, 0, 200.);
    
    pt_2 = book<TH1F>("pt_2","p_{T} tau 2",50,0,500);
    eta_2 = book<TH1F>("eta_2","#eta tau 2",100,-3,3);
    phi_2 = book<TH1F>("phi_2","#phi tau 2",100,-M_PI,M_PI);
    ptrel_2 = book<TH1F>("ptrel_2", "p_{T}^{rel}(tau 2,jet)", 40, 0, 200.);
    charge_2 = book<TH1F>("charge_2", "tau 2 charge",3,-1.5,1.5);
    deltaRmin_2 = book<TH1F>("deltaRmin_2", "#Delta R_{min}(tau 2,jet)", 40, 0, 2.0);
    deltaRmin_ptrel_2 = book<TH2F>("deltaRmin_ptrel_2", "x=#Delta R_{min}(tau 2,jet) y=p_{T}^{rel}(tau 2,jet)", 40, 0, 2.0, 40, 0, 200.);
}

void TauHists::fill(const Event & event){
    auto w = event.weight;
    assert(event.taus);
    number->Fill(event.taus->size(), w);
    
    // buffer values for ptrel and drmin to avoid recomputation:
    vector<float> drmin_buf(event.taus->size(), 0.0);
    vector<float> ptrel_buf(event.taus->size(), 0.0);
    for(const auto & tau : *event.taus){
        pt->Fill(tau.pt(), w);
        eta->Fill(tau.eta(), w);
        phi->Fill(tau.phi(), w);
        charge->Fill(tau.charge(), w);
        
        if(event.jets){
            auto nj = nextJet(tau, *event.jets);
            auto drmin_val = nj ? deltaR(tau, *nj) : numeric_limits<float>::infinity();
            drmin_buf.push_back(drmin_val);
            ptrel_buf.push_back(pTrel(tau, nj));
            ptrel->Fill(ptrel_buf.back(), w);
            deltaRmin->Fill(drmin_buf.back(), w);
            deltaRmin_ptrel->Fill(drmin_buf.back(), ptrel_buf.back(), w);
        }
    }
    
    if(event.taus->size() > 0){
        const auto & tau = (*event.taus)[0];
        pt_1->Fill(tau.pt(), w);
        pt_1_binned->Fill(tau.pt(), w);
        eta_1->Fill(tau.eta(), w);
        phi_1->Fill(tau.phi(), w);
        charge_1->Fill(tau.charge(), w);
        if(event.jets){
            ptrel_1->Fill(ptrel_buf[0], w);
            deltaRmin_1->Fill(drmin_buf[0], w);
            deltaRmin_ptrel_1->Fill(drmin_buf[0], ptrel_buf[0], w);
        }
    }

    if(event.muons->size() && event.taus->size() > 0){
        const auto & muon = (*event.muons)[0];
	const auto & tau = (*event.taus)[0];
	pt_muon1_tau1->Fill(muon.pt()+tau.pt(), w);
	pt_muon1_tau1_binned->Fill(muon.pt()+tau.pt(), w);
    }

    if(event.taus->size() > 1){
        const auto & tau = (*event.taus)[1];
        pt_2->Fill(tau.pt(), w);
        eta_2->Fill(tau.eta(), w);
        phi_2->Fill(tau.phi(), w);
        charge_2->Fill(tau.charge(), w);
        if(event.jets){
            ptrel_2->Fill(ptrel_buf[1], w);
            deltaRmin_2->Fill(drmin_buf[1], w);
            deltaRmin_ptrel_2->Fill(drmin_buf[1], ptrel_buf[1], w);
        }
    }
}
