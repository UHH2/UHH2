#include "UHH2/common/include/MuonHists.h"
#include "UHH2/common/include/Utils.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/LorentzVector.h"

#include "TH1F.h"
#include "TH2F.h"

#include <vector>

using namespace uhh2;
using namespace std;

MuonHists::MuonHists(Context & ctx, const std::string & dname): Hists(ctx, dname){
    number = book<TH1F>("number","number of muons",7,-0.5,6.5);
    
    pt = book<TH1F>("pt","p_{T} muon",100,0,500);
    eta = book<TH1F>("eta","#eta muon",100,-3,3);
    phi = book<TH1F>("phi","#phi muon",100,-M_PI,M_PI);
    isolation = book<TH1F>("isolation","relIso muon",100,0,0.5);
    charge = book<TH1F>("charge", "muon charge",3,-1.5,1.5);
    ptrel = book<TH1F>("ptrel", "p_{T}^{rel}(mu,jet)", 40, 0, 200.);
    deltaRmin = book<TH1F>("deltaRmin", "#Delta R_{min}(mu,jet)", 40, 0, 2.0);
    deltaRmin_ptrel = book<TH2F>("deltaRmin_ptrel", "x=#Delta R_{min}(mu,jet) y=p_{T}^{rel}(mu,jet)", 40, 0, 2.0, 40, 0, 200.);
    
    pt_1 = book<TH1F>("pt_1","p_{T} muon 1",100,0,500);
    eta_1 = book<TH1F>("eta_1","#eta muon 1",100,-3,3);
    phi_1 = book<TH1F>("phi_1","#phi muon 1",100,-M_PI,M_PI);
    isolation_1 = book<TH1F>("isolation_1","relIso muon 1",100,0,0.5);
    charge_1 = book<TH1F>("charge_1", "muon 1 charge",3,-1.5,1.5);
    ptrel_1 = book<TH1F>("ptrel_1", "p_{T}^{rel}(mu 1,jet)", 40, 0, 200.);
    deltaRmin_1 = book<TH1F>("deltaRmin_1", "#Delta R_{min}(mu 1,jet)", 40, 0, 2.0);
    deltaRmin_ptrel_1 = book<TH2F>("deltaRmin_ptrel_1", "x=#Delta R_{min}(e 1,jet) y=p_{T}^{rel}(mu 1,jet)", 40, 0, 2.0, 40, 0, 200.);
    
    pt_2 = book<TH1F>("pt_2","p_{T} muon 2",100,0,500);
    eta_2 = book<TH1F>("eta_2","#eta muon 2",100,-3,3);
    phi_2 = book<TH1F>("phi_2","#phi muon 2",100,-M_PI,M_PI);
    isolation_2 = book<TH1F>("isolation_2","relIso muon 2",100,0,0.5);
    ptrel_2 = book<TH1F>("ptrel_2", "p_{T}^{rel}(mu 2,jet)", 40, 0, 200.);
    charge_2 = book<TH1F>("charge_2", "muon 2 charge",3,-1.5,1.5);
    deltaRmin_2 = book<TH1F>("deltaRmin_2", "#Delta R_{min}(mu 2,jet)", 40, 0, 2.0);
    deltaRmin_ptrel_2 = book<TH2F>("deltaRmin_ptrel_2", "x=#Delta R_{min}(mu 2,jet) y=p_{T}^{rel}(mu 2,jet)", 40, 0, 2.0, 40, 0, 200.);
}

void MuonHists::fill(const Event & event){
    auto w = event.weight;
    assert(event.muons);
    number->Fill(event.muons->size(), w);
    
    // buffer values for ptrel and drmin to avoid recomputation:
    vector<float> drmin_buf(event.muons->size(), 0.0);
    vector<float> ptrel_buf(event.muons->size(), 0.0);
    for(const auto & muon : *event.muons){
        pt->Fill(muon.pt(), w);
        eta->Fill(muon.eta(), w);
        phi->Fill(muon.phi(), w);
        isolation->Fill(muon.relIso(), w);
        charge->Fill(muon.charge(), w);
        
        if(event.jets){
            auto nj = nextJet(muon, *event.jets);
            auto drmin_val = nj ? deltaR(muon, *nj) : numeric_limits<float>::infinity();
            drmin_buf.push_back(drmin_val);
            ptrel_buf.push_back(pTrel(muon, nj));
            ptrel->Fill(ptrel_buf.back(), w);
            deltaRmin->Fill(drmin_buf.back(), w);
            deltaRmin_ptrel->Fill(drmin_buf.back(), ptrel_buf.back(), w);
        }
    }
    
    if(event.muons->size() > 0){
        const auto & muon = (*event.muons)[0];
        pt_1->Fill(muon.pt(), w);
        eta_1->Fill(muon.eta(), w);
        phi_1->Fill(muon.phi(), w);
        isolation_1->Fill(muon.relIso(), w);
        charge_1->Fill(muon.charge(), w);
        if(event.jets){
            ptrel_1->Fill(ptrel_buf[0], w);
            deltaRmin_1->Fill(drmin_buf[0], w);
            deltaRmin_ptrel_1->Fill(drmin_buf[0], ptrel_buf[0], w);
        }
    }
    if(event.muons->size() > 1){
        const auto & muon = (*event.muons)[1];
        pt_2->Fill(muon.pt(), w);
        eta_2->Fill(muon.eta(), w);
        phi_2->Fill(muon.phi(), w);
        isolation_2->Fill(muon.relIso(), w);
        charge_2->Fill(muon.charge(), w);
        if(event.jets){
            ptrel_2->Fill(ptrel_buf[1], w);
            deltaRmin_2->Fill(drmin_buf[1], w);
            deltaRmin_ptrel_2->Fill(drmin_buf[1], ptrel_buf[1], w);
        }
    }
}
