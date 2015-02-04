#include "UHH2/common/include/JetHists.h"
#include "UHH2/common/include/Utils.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/LorentzVector.h"

#include "TH1F.h"
#include "TH2F.h"

#include <vector>

using namespace uhh2;
using namespace std;

JetHists::JetHists(Context & ctx,
                   const std::string & dname,
                   const std::string & collection,
                   bool gen_plots,
                   float gen_plots_eta_min,
                   float gen_plots_eta_max): 
    Hists(ctx, dname),
    gen_plots_eta_min_(gen_plots_eta_min),
    gen_plots_eta_max_(gen_plots_eta_max)
{
    number = book<TH1F>("number","number of jets",21, -.5, 20.5);
    
    pt = book<TH1F>("pt","p_{T} all jets",50,20,1500);
    eta = book<TH1F>("eta","#eta all jets",100,-5,5);
    phi = book<TH1F>("phi","#phi all jets",50,-M_PI,M_PI);
    csv = book<TH1F>("csv","csv-disriminator all jets",50,0,1);

    pt_1 = book<TH1F>("pt_1","p_{T} jet 1",50,100,1500);
    eta_1 = book<TH1F>("eta_1","#eta jet 1",100,-5,5);
    phi_1 = book<TH1F>("phi_1","#phi jet 1",50,-M_PI,M_PI);
    csv_1 = book<TH1F>("csv_1","csv-disriminator jet 1",50,0,1);
    deltaRmin_1 = book<TH1F>("deltaRmin_1", "#Delta R_{min}(first jet,nearest jet)", 40, 0, 2.0);
    
    pt_2 = book<TH1F>("pt_2","p_{T} jet 2",50,20,1000);
    eta_2 = book<TH1F>("eta_2","#eta jet 2",100,-5,5);
    phi_2 = book<TH1F>("phi_2","#phi jet 2",50,-M_PI,M_PI);
    csv_2 = book<TH1F>("csv_2","csv-disriminator jet 2",50,0,1);
    deltaRmin_2 = book<TH1F>("deltaRmin_2", "#Delta R_{min}(2nd jet,nearest jet)", 40, 0, 2.0);

    pt_3 = book<TH1F>("pt_3","p_{T} jet 3",50,20,500);
    eta_3 = book<TH1F>("eta_3","#eta jet 3",100,-5,5);
    phi_3 = book<TH1F>("phi_3","#phi jet 3",50,-M_PI,M_PI);
    csv_3 = book<TH1F>("csv_3","csv-disriminator jet 3",50,0,1);

    pt_4 = book<TH1F>("pt_4","p_{T} jet 4",50,20,250);
    eta_4 = book<TH1F>("eta_4","#eta jet 4",100,-5,5);
    phi_4 = book<TH1F>("phi_4","#phi jet 4",50,-M_PI,M_PI);
    csv_4 = book<TH1F>("csv_4","csv-disriminator jet 4",50,0,1);

    m_1 = book<TH1F>("m_1","M^{jet 1} [GeV/c^{2}]", 100, 0, 300);
    m_2 = book<TH1F>("m_2","M^{jet 2} [GeV/c^{2}]", 100, 0, 300);
    m_3 = book<TH1F>("m_3","M^{jet 3} [GeV/c^{2}]", 100, 0, 300);
    m_4 = book<TH1F>("m_4","M^{jet 4} [GeV/c^{2}]", 100, 0, 300);

    if (gen_plots) {
        eff_sub     = book<TH1F>("eff_sub",     "p_{T}",                75,0,1500);
        eff_tot     = book<TH1F>("eff_tot",     "p_{T}",                75,0,1500);
        pt_response = book<TH1F>("pt_response", "(p_{T, gen} - p_{T, reco}) / p_{T, gen}",
                                                                        800,-1.,1.);
    } else {
        eff_sub     = 0;
        eff_tot     = 0;
        pt_response = 0;
    }

    h_jets = ctx.get_handle<std::vector<Jet> >(collection);
}

void JetHists::fill(const Event & event){
    auto w = event.weight;

    const auto jets = (event.get_state(h_jets) == GenericEvent::state::valid)
                      ? &event.get(h_jets) : event.jets;

    assert(jets);
    number->Fill(jets->size(), w);

    if (eff_sub && event.genparticles) {
        for (const auto & gp: *event.genparticles) {
            if (-6 < gp.pdgId() && gp.pdgId() < 6 
                && abs(gp.eta()) > gen_plots_eta_min_
                && abs(gp.eta()) < gen_plots_eta_max_) {
                auto gp_pt = gp.pt();
                eff_tot->Fill(gp_pt, w);
                auto j = closestParticle(gp, *jets);
                if (j && deltaR(gp, *j) < 0.2) {
                    eff_sub->Fill(gp_pt, w);
                    pt_response->Fill((gp_pt - j->pt()) / gp_pt, w);
                }
            }
        }
    }

    for(const auto & jet : *jets){
        pt->Fill(jet.pt(), w);
        eta->Fill(jet.eta(), w);
        phi->Fill(jet.phi(), w);
        csv->Fill(jet.btag_combinedSecondaryVertex(), w);
    }
    
    if(jets->size() > 0){
        const auto & jet = (*jets)[0];
        pt_1->Fill(jet.pt(), w);
        eta_1->Fill(jet.eta(), w);
        phi_1->Fill(jet.phi(), w);
        csv_1->Fill(jet.btag_combinedSecondaryVertex(), w);
        m_1->Fill(jet.v4().M(), w);
        auto next_jet = nextJet(jet, *jets);
        auto drmin = next_jet ? deltaR(jet, *next_jet) : numeric_limits<float>::infinity();
        deltaRmin_1->Fill(drmin, w);
    }
    if(jets->size() > 1){
        const auto & jet = (*jets)[1];
        pt_2->Fill(jet.pt(), w);
        eta_2->Fill(jet.eta(), w);
        phi_2->Fill(jet.phi(), w);
        csv_2->Fill(jet.btag_combinedSecondaryVertex(), w);
        m_2->Fill(jet.v4().M(), w);
        auto next_jet = nextJet(jet, *jets);
        auto drmin = next_jet ? deltaR(jet, *next_jet) : numeric_limits<float>::infinity();
        deltaRmin_2->Fill(drmin, w);
    }
    
    if(jets->size() > 2){
        const auto & jet = (*jets)[2];
        pt_3->Fill(jet.pt(), w);
        eta_3->Fill(jet.eta(), w);
        phi_3->Fill(jet.phi(), w);
        csv_3->Fill(jet.btag_combinedSecondaryVertex(), w);
        m_3->Fill(jet.v4().M(), w);
    }
    if(jets->size() > 3){
        const auto & jet = (*jets)[3];
        pt_4->Fill(jet.pt(), w);
        eta_4->Fill(jet.eta(), w);
        phi_4->Fill(jet.phi(), w);
        csv_4->Fill(jet.btag_combinedSecondaryVertex(), w);
        m_4->Fill(jet.v4().M(), w);
    }
    
}
