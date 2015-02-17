#include "UHH2/common/include/EventHists.h"
#include "TH1F.h"

using namespace uhh2;

EventHists::EventHists(uhh2::Context & ctx, const std::string & dirname): Hists(ctx, dirname){
    N_PrimVertices = book<TH1F>("N_PrimVertices", "number of primary vertices", 56, -0.5, 55.5);
    Weights = book<TH1F>("Weights", "weights", 100,0,2);
    MET = book<TH1F>("MET", "missing E_{T}", 200,0,1000);
    HT = book<TH1F>("HT", "H_{T} Jets", 100, 0, 3500);
    HTLep = book<TH1F>("HTLep", "H_{T} Lep", 100, 0, 1000);
    ST = book<TH1F>("ST", "S_{T}", 100, 0, 5000);

    h_ht = ctx.get_handle<double>("HT");
    h_primlep = ctx.get_handle<FlavorParticle>("PrimaryLepton");
}


void EventHists::fill(const uhh2::Event & e){
    assert(e.met);
    assert(e.pvs);
    N_PrimVertices->Fill(e.pvs->size(), e.weight);
    Weights->Fill(e.weight);
    MET->Fill(e.met->pt(), e.weight);
    auto met = e.met->pt();
    float primlep_pt = -1.0f, ht = -1.0f;
    if(e.is_valid(h_primlep)){
        primlep_pt = e.get(h_primlep).pt();
        HTLep->Fill(primlep_pt + met, e.weight);
    }
    if(e.is_valid(h_ht)){
        ht = e.get(h_ht);
        HT->Fill(ht, e.weight);
    }
    if(primlep_pt >= 0.0f && ht >= 0.0f){
        ST->Fill(ht + primlep_pt + met, e.weight);
    }
}
