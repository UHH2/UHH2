#include <vector>
#include <iostream>

#include "UHH2/common/include/EventHists.h"
#include "TH1F.h"

using namespace uhh2;

EventHists::EventHists(uhh2::Context & ctx, const std::string & dirname): Hists(ctx, dirname){
    N_PrimVertices = book<TH1F>("N_PrimVertices", "number of primary vertices", 50, 0, 50);
    Weights = book<TH1F>("Weights", "weights", 100,0,2);
    MET = book<TH1F>("MET", "missing E_{T}", 200,0,1000);
    HT = book<TH1F>("HT", "H_{T} Jets", 100, 0, 3500);
    HTLep = book<TH1F>("HTLep", "H_{T} Lep", 100, 0, 1000);
    ST = book<TH1F>("ST", "S_{T}", 100, 0, 5000);

    std::vector<float> log_bins;
    for (float f=1e-10; f<1e+11; f*=10) {
        log_bins.push_back(1.*f);
        log_bins.push_back(2.*f);
        log_bins.push_back(5.*f);
    }
    WeightsLogBins = book<TH1F>("WeightsLogBins", "weights with log-bins", log_bins.size()-3, log_bins.data());

    h_ht = ctx.get_handle<double>("HT");
    h_st = ctx.get_handle<double>("ST");
    h_primlep = ctx.get_handle<FlavorParticle>("PrimaryLepton");
}


void EventHists::fill(const uhh2::Event & e){
    assert(e.met);
    assert(e.pvs);
    N_PrimVertices->Fill(e.genInfo->pileup_TrueNumInteractions(), e.weight);
    Weights->Fill(e.weight);
    WeightsLogBins->Fill(e.weight);
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
    if (e.is_valid(h_st)) {
        ST->Fill(e.get(h_st), e.weight);
    } else if(primlep_pt >= 0.0f && ht >= 0.0f) {
        ST->Fill(ht + primlep_pt + met, e.weight);
    }
}
