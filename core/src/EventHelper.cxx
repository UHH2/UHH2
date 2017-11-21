#include "UHH2/core/include/EventHelper.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/AnalysisModule.h"

using namespace uhh2;
using namespace uhh2::detail;
using namespace std;


namespace {
    
template<typename T>
Event::Handle<T> declare_in_out(const std::string & branch_name, const std::string & event_name, Context & ctx){
    auto result = ctx.declare_event_input<T>(branch_name, event_name);
    ctx.declare_event_output<T>(branch_name, event_name);
    return result;
}

}

EventHelper::EventHelper(uhh2::Context & ctx_): ctx(ctx_), event(0), pvs(false), electrons(false), muons(false), taus(false), photons(false), jets(false),
    topjets(false), met(false), genInfo(false), gentopjets(false), genparticles(false), genjets(false), trigger(false), first_event_read(true){
    h_run = declare_in_out<int>("run", "run", ctx);
    h_lumi = declare_in_out<int>("luminosityBlock", "luminosityBlock", ctx);
    h_event = declare_in_out<int>("event", "event", ctx);
    h_rho = declare_in_out<float>("rho", "rho", ctx);
    h_isRealData = declare_in_out<bool>("isRealData", "isRealData", ctx);
    h_bsx = declare_in_out<float>("beamspot_x0", "beamspot_x0", ctx);
    h_bsy = declare_in_out<float>("beamspot_y0", "beamspot_y0", ctx);
    h_bsz = declare_in_out<float>("beamspot_z0", "beamspot_z0", ctx);
}



// use a macro here to implicitly check consistency between all the names
// (boolean member, handles, GenericEvent name, setup method name)
#define IMPL_SETUP(name, type) void EventHelper::setup_##name (const std::string & bname) {\
    if(bname.empty()) return; \
    name = true; \
    h_##name = declare_in_out<type>(bname, #name, ctx); \
}

IMPL_SETUP(pvs, vector<PrimaryVertex>)
IMPL_SETUP(electrons, vector<Electron>)
IMPL_SETUP(muons, vector<Muon>)
IMPL_SETUP(taus, vector<Tau>)
IMPL_SETUP(photons, vector<Photon>)
IMPL_SETUP(jets, vector<Jet>)
IMPL_SETUP(topjets, vector<TopJet>)
IMPL_SETUP(met, MET)
IMPL_SETUP(genInfo, GenInfo)
IMPL_SETUP(gentopjets, vector<GenTopJet>)
IMPL_SETUP(genparticles, vector<GenParticle>)
IMPL_SETUP(genjets, vector<Particle>)

void EventHelper::setup_trigger(){
    trigger = true;
    h_triggerResults = declare_in_out<std::vector<bool>>("triggerResults", "triggerResults", ctx);
    h_triggerPrescales = declare_in_out<std::vector<int>>("triggerPrescales", "triggerPrescales", ctx);
    h_triggerNames = declare_in_out<std::vector<std::string>>("triggerNames", "triggerNames", ctx);
}

void EventHelper::set_event(Event * event_){
    event = event_;
}


void EventHelper::set_infile_triggernames(std::map<int, std::vector<std::string>> triggernames_){
    triggernames = move(triggernames_);
    triggernames_last_runid_event = -1;
}


void EventHelper::event_read(){
    // synchonize the data from GenericEvent to Event data members:
    event->weight = 1.0;
    event->run = event->get(h_run);
    event->luminosityBlock = event->get(h_lumi);
    event->event = event->get(h_event);
    event->rho = event->get(h_rho);
    event->isRealData = event->get(h_isRealData);
    event->beamspot_x0 = event->get(h_bsx);
    event->beamspot_y0 = event->get(h_bsy);
    event->beamspot_z0 = event->get(h_bsz);
    
    if(trigger){
        // fix triggerNames in case of a run change.
        if(triggernames_last_runid_event != event->run){
            auto it = triggernames.find(event->run);
            if(it==triggernames.end()){
                throw runtime_error("event_read: Did not find trigger names for current run");
            }
            event->set_triggernames(it->second);
        }
        triggernames_last_runid_event = event->run;
    }
    
    // note: pointers only need to be set once
    if(first_event_read){
        first_event_read = false;
        if(pvs) event->pvs = &event->get(h_pvs);
        if(electrons) event->electrons = &event->get(h_electrons);
        if(muons) event->muons = &event->get(h_muons);
        if(taus)  event->taus = &event->get(h_taus);
        if(photons)  event->photons = &event->get(h_photons);
        if(jets) event->jets = &event->get(h_jets);
        if(topjets)  event->topjets = &event->get(h_topjets);
        if(met) event->met = &event->get(h_met);
        if(genInfo) event->genInfo = &event->get(h_genInfo);
        if(gentopjets) event->gentopjets = &event->get(h_gentopjets);
        if(genparticles) event->genparticles = & event->get(h_genparticles);
        if(genjets) event->genjets = &event->get(h_genjets);
        if(trigger){
            event->get_triggerResults() = &event->get(h_triggerResults);
	    //DEBUG
	    if(!genjets) event->get_triggerPrescales() = &event->get(h_triggerPrescales);
        }
    }
}

void EventHelper::event_write(){
    event->set(h_run, event->run);
    event->set(h_lumi, event->luminosityBlock);
    event->set(h_event, event->event);
    event->set(h_rho, event->rho);
    event->set(h_isRealData, event->isRealData);
    event->set(h_bsx, event->beamspot_x0);
    event->set(h_bsy, event->beamspot_y0);
    event->set(h_bsz, event->beamspot_z0);
    
    // special case: trigger is saved only once per runid:
    if(trigger){
        if(triggernames_written.find(event->run) == triggernames_written.end()){
            auto it = triggernames.find(event->run);
            if(it==triggernames.end()){
                throw runtime_error("event_write: Did not find trigger names");
            }
            event->set(h_triggerNames, it->second);
            triggernames_written.insert(event->run);
        }
        else{
            event->set(h_triggerNames, vector<string>());
        }
    }
    
    // TODO: can check here whether someone changes the pointers of Event although they shouldn't.
}

