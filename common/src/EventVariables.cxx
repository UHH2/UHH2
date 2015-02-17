#include "UHH2/common/include/EventVariables.h"

using namespace uhh2;

HTCalculator::HTCalculator(Context & ctx, const boost::optional<JetId> & jetid_, const std::string & name):
        jetid(jetid_){
    h_ht = ctx.get_handle<double>(name);
}


bool HTCalculator::process(Event & event){
    double ht = 0.0;
    for(const auto & jet : *event.jets){
        if(jetid && !(*jetid)(jet, event)) continue;
        ht += jet.pt();
    }
    event.set(h_ht, ht);
    return true;
}

HTlepCalculator::HTlepCalculator(Context & ctx, const boost::optional<ElectronId> & electronid_, const boost::optional<MuonId> & muonid_, const boost::optional<TauId> & tauid_, const std::string & name):
        electronid(electronid_), muonid(muonid_), tauid(tauid_){
    h_htlep = ctx.get_handle<double>(name);
}
bool HTlepCalculator::process(Event & event){
    double htlep = 0.0;
    for(const auto & electron : *event.electrons){
        if(electronid && !(*electronid)(electron, event)) continue;
        htlep += electron.pt();
    }
	for(const auto & muon : *event.muons){
        if(muonid && !(*muonid)(muon, event)) continue;
        htlep += muon.pt();
    }
	for(const auto & tau : *event.taus){
        if(tauid && !(*tauid)(tau, event)) continue;
        htlep += tau.pt();
    }
	if(event.met) {
        htlep += event.met->pt();
    }
       
    event.set(h_htlep, htlep);
    return true;
}