#include "UHH2/common/include/ElectronHists.h"
#include "UHH2/common/include/Utils.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/LorentzVector.h"

#include "TH1F.h"
#include "TH2F.h"

#include <vector>

using namespace uhh2;
using namespace std;

ElectronHists::ElectronHists(Context & ctx, const std::string & dname, const std::string & collection_, bool gen_plots): Hists(ctx, dname), collection(collection_){
  h_jets = ctx.get_handle<vector<Jet>>(collection);
  h_topjets = ctx.get_handle<vector<TopJet>>(collection);

  number      = book<TH1F>("number",      "number of electrons",7,-0.5,6.5);

  pt          = book<TH1F>("pt",          "p_{T} electron",           100,0,500);
  eta         = book<TH1F>("eta",         "#eta electron",            100,-3,3);
  phi         = book<TH1F>("phi",         "#phi electron",            100,-M_PI,M_PI);
  isolation   = book<TH1F>("isolation",   "relIso electron",          100,0,0.5);
  charge      = book<TH1F>("charge",      "electron charge",          3,-1.5,1.5);
  ptrel       = book<TH1F>("ptrel",       "p_{T}^{rel}(e,jet)",       40, 0, 200.);
  deltaRmin   = book<TH1F>("deltaRmin",   "#Delta R_{min}(e,jet)",    40, 0, 2.0);
  deltaRmin_ptrel = book<TH2F>("deltaRmin_ptrel", ";#Delta R_{min}(e,jet);p_{T}^{rel}(e,jet)", 40, 0, 2.0, 40, 0, 200.);

  pt_1        = book<TH1F>("pt_1",        "p_{T} electron 1",         100,0,500);
  eta_1       = book<TH1F>("eta_1",       "#eta electron 1",          100,-3,3);
  phi_1       = book<TH1F>("phi_1",       "#phi electron 1",          100,-M_PI,M_PI);
  isolation_1 = book<TH1F>("isolation_1", "relIso electron 1",        100,0,0.5);
  charge_1    = book<TH1F>("charge_1",    "electron 1 charge",        3,-1.5,1.5);
  ptrel_1     = book<TH1F>("ptrel_1",     "p_{T}^{rel}(e 1,jet)",     40, 0, 200.);
  deltaRmin_1 = book<TH1F>("deltaRmin_1", "#Delta R_{min}(e 1,jet)",  40, 0, 2.0);
  deltaRmin_ptrel_1 = book<TH2F>("deltaRmin_ptrel_1", ";#Delta R_{min}(e 1,jet);p_{T}^{rel}(e 1,jet)", 40, 0, 2.0, 40, 0, 200.);

  pt_2        = book<TH1F>("pt_2",        "p_{T} electron 2",         100,0,500);
  eta_2       = book<TH1F>("eta_2",       "#eta electron 2",          100,-3,3);
  phi_2       = book<TH1F>("phi_2",       "#phi electron 2",          100,-M_PI,M_PI);
  isolation_2 = book<TH1F>("isolation_2", "relIso electron 2",        100,0,0.5);
  ptrel_2     = book<TH1F>("ptrel_2",     "p_{T}^{rel}(e 2,jet)",     40, 0, 200.);
  charge_2    = book<TH1F>("charge_2",    "electron 2 charge",        3,-1.5,1.5);
  deltaRmin_2 = book<TH1F>("deltaRmin_2", "#Delta R_{min}(e 2,jet)",  40, 0, 2.0);
  deltaRmin_ptrel_2 = book<TH2F>("deltaRmin_ptrel_2", ";#Delta R_{min}(e 2,jet);p_{T}^{rel}(e 2,jet)", 40, 0, 2.0, 40, 0, 200.);

  if (gen_plots) {
    eff_sub     = book<TH1F>("eff_sub",     "p_{T}",                100,0,500);
    eff_tot     = book<TH1F>("eff_tot",     "p_{T}",                100,0,500);
    pt_response = book<TH1F>("pt_response", "(p_{T, gen} - p_{T, reco}) / p_{T, gen}",
    800,-1.,1.);
  } else {
    eff_sub     = 0;
    eff_tot     = 0;
    pt_response = 0;
  }
}

void ElectronHists::fill(const Event & event){
  assert(event.electrons);
  bool isJet = event.is_valid(h_jets);
  bool isTopJet = event.is_valid(h_topjets);

  auto w = event.weight;
  number->Fill(event.electrons->size(), w);

  if (eff_sub && event.genparticles) {
    for (const auto & gp: *event.genparticles) {
      if (abs(gp.pdgId()) == 11) {
        auto gp_pt = gp.pt();
        eff_tot->Fill(gp_pt, w);
        auto ele = closestParticle(gp, *event.electrons);
        if (ele && deltaR(gp, *ele) < 0.1) {
          eff_sub->Fill(gp_pt, w);
          pt_response->Fill((gp_pt - ele->pt()) / gp_pt, w);
        }
      }
    }
  }

  // buffer values for ptrel and drmin to avoid recomputation:
  vector<float> drmin_buf;
  vector<float> ptrel_buf;
  for(const auto & ele : *event.electrons){
    pt->Fill(ele.pt(), w);
    eta->Fill(ele.eta(), w);
    phi->Fill(ele.phi(), w);
    isolation->Fill(ele.relIso(), w);
    charge->Fill(ele.charge(), w);

    if(isJet || isTopJet){
      double drmin_val;
      if (isJet) {
        auto nj = nextJet(ele, event.get(h_jets));
        drmin_val = nj ? deltaR(ele, *nj) : numeric_limits<float>::infinity();
        ptrel_buf.push_back(pTrel(ele, nj));
      } else if (isTopJet) {
        auto nj = closestParticle(ele, event.get(h_topjets));
        drmin_val = nj ? deltaR(ele, *nj) : numeric_limits<float>::infinity();
        ptrel_buf.push_back(pTrel(ele, nj));
      }
      drmin_buf.push_back(drmin_val);
      ptrel->Fill(ptrel_buf.back(), w);
      deltaRmin->Fill(drmin_buf.back(), w);
      deltaRmin_ptrel->Fill(drmin_buf.back(), ptrel_buf.back(), w);
    }
  }

  if(event.electrons->size() > 0){
    const auto & ele = (*event.electrons)[0];
    pt_1->Fill(ele.pt(), w);
    eta_1->Fill(ele.eta(), w);
    phi_1->Fill(ele.phi(), w);
    isolation_1->Fill(ele.relIso(), w);
    charge_1->Fill(ele.charge(), w);
    if(isJet || isTopJet){
      ptrel_1->Fill(ptrel_buf[0], w);
      deltaRmin_1->Fill(drmin_buf[0], w);
      deltaRmin_ptrel_1->Fill(drmin_buf[0], ptrel_buf[0], w);
    }
  }
  if(event.electrons->size() > 1){
    const auto & ele = (*event.electrons)[1];
    pt_2->Fill(ele.pt(), w);
    eta_2->Fill(ele.eta(), w);
    phi_2->Fill(ele.phi(), w);
    isolation_2->Fill(ele.relIso(), w);
    charge_2->Fill(ele.charge(), w);
    if(isJet || isTopJet){
      ptrel_2->Fill(ptrel_buf[1], w);
      deltaRmin_2->Fill(drmin_buf[1], w);
      deltaRmin_ptrel_2->Fill(drmin_buf[1], ptrel_buf[1], w);
    }
  }
}
