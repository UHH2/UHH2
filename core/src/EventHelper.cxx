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
						topjets(false), toppuppijets(false), met(false),  genmet(false), genInfo(false), gentopjets(false),
						genparticles(false), genjets(false), pfparticles(false), trigger(false),  L1EG_seeds(false), L1J_seeds(false), L1M_seeds(false), L1EtS_seeds(false), L1T_seeds(false), first_event_read(true){
    h_run = declare_in_out<int>("run", "run", ctx);
    h_lumi = declare_in_out<int>("luminosityBlock", "luminosityBlock", ctx);
    h_event = declare_in_out<int>("event", "event", ctx);
    h_rho = declare_in_out<float>("rho", "rho", ctx);
    h_isRealData = declare_in_out<bool>("isRealData", "isRealData", ctx);
    h_passEcalBadCalib = declare_in_out<bool>("passEcalBadCalib", "passEcalBadCalib", ctx);
    h_year = declare_in_out<std::string>("year", "year", ctx);
    h_bsx = declare_in_out<float>("beamspot_x0", "beamspot_x0", ctx);
    h_bsy = declare_in_out<float>("beamspot_y0", "beamspot_y0", ctx);
    h_bsz = declare_in_out<float>("beamspot_z0", "beamspot_z0", ctx);
    h_prefire = declare_in_out<float>("prefiringWeight", "prefiringWeight", ctx);
    h_prefireUp = declare_in_out<float>("prefiringWeightUp", "prefiringWeightUp", ctx);
    h_prefireDown = declare_in_out<float>("prefiringWeightDown", "prefiringWeightDown", ctx);
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
IMPL_SETUP(toppuppijets, vector<TopJet>)
IMPL_SETUP(met, MET)
IMPL_SETUP(genInfo, GenInfo)
IMPL_SETUP(gentopjets, vector<GenTopJet>)
IMPL_SETUP(genparticles, vector<GenParticle>)
IMPL_SETUP(pfparticles, vector<PFParticle>)
IMPL_SETUP(genjets, vector<GenJet>)
IMPL_SETUP(genmet, MET)
IMPL_SETUP(L1EG_seeds, vector<L1EGamma>)
IMPL_SETUP(L1J_seeds, vector<L1Jet>)
IMPL_SETUP(L1M_seeds, vector<L1Muon>)
IMPL_SETUP(L1EtS_seeds, vector<L1EtSum>)
IMPL_SETUP(L1T_seeds, vector<L1Tau>)




void EventHelper::setup_trigger(){
    trigger = true;
    h_triggerResults = declare_in_out<std::vector<bool>>("triggerResults", "triggerResults", ctx);
    h_triggerPrescales = declare_in_out<std::vector<int>>("triggerPrescales", "triggerPrescales", ctx);
    h_triggerPrescalesL1min = declare_in_out<std::vector<int>>("triggerPrescalesL1min", "triggerPrescalesL1min", ctx);
    h_triggerPrescalesL1max = declare_in_out<std::vector<int>>("triggerPrescalesL1max", "triggerPrescalesL1max", ctx);
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
    event->passEcalBadCalib = event->get(h_passEcalBadCalib);
    event->year = event->get(h_year);
    event->beamspot_x0 = event->get(h_bsx);
    event->beamspot_y0 = event->get(h_bsy);
    event->beamspot_z0 = event->get(h_bsz);
    event->prefiringWeight = event->get(h_prefire);
    event->prefiringWeightUp = event->get(h_prefireUp);
    event->prefiringWeightDown = event->get(h_prefireDown);

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
        if(toppuppijets)  event->toppuppijets = &event->get(h_toppuppijets);
        if(met) event->met = &event->get(h_met);
	if(genmet) event->genmet = &event->get(h_genmet);
	try{
        if(genInfo) event->genInfo = &event->get(h_genInfo);}
	catch(const std::runtime_error& error){
	  std::cout<<"Problem with genInfo in EventHelper.cxx"<<std::endl;
	  std::cout<<error.what();
	}
        if(gentopjets) event->gentopjets = &event->get(h_gentopjets);
	try{
        if(genparticles) event->genparticles = & event->get(h_genparticles);}
	catch(const std::runtime_error& error){
	  std::cout<<"Problem with genparticles in EventHelper.cxx"<<std::endl;
	  std::cout<<error.what();
	}
	try{
        if(pfparticles) event->pfparticles = & event->get(h_pfparticles);}
	catch(const std::runtime_error& error){
	  std::cout<<"Problem with pfparticles in EventHelper.cxx"<<std::endl;
	  std::cout<<error.what();
	}

	try{
        if(genjets) event->genjets = &event->get(h_genjets);}
	catch(const std::runtime_error& error){
	  std::cout<<"Problem with genjets in EventHelper.cxx"<<std::endl;
	  std::cout<<error.what();
	}
        if(trigger){
            event->get_triggerResults() = &event->get(h_triggerResults);
	    event->get_triggerPrescales() = &event->get(h_triggerPrescales);
	    event->get_triggerPrescalesL1min() = &event->get(h_triggerPrescalesL1min);
	    event->get_triggerPrescalesL1max() = &event->get(h_triggerPrescalesL1max);
        }
	if(L1EG_seeds){
	  event->L1EG_seeds =  &event->get(h_L1EG_seeds);
	}
	if(L1J_seeds){
	  event->L1J_seeds =  &event->get(h_L1J_seeds);
	}
  if(L1M_seeds){
	  event->L1M_seeds =  &event->get(h_L1M_seeds);
	}
  if(L1EtS_seeds){
	  event->L1EtS_seeds =  &event->get(h_L1EtS_seeds);
	}
  if(L1T_seeds){
	  event->L1T_seeds =  &event->get(h_L1T_seeds);
	}
    }
}

void EventHelper::event_write(){
    event->set(h_run, event->run);
    event->set(h_lumi, event->luminosityBlock);
    event->set(h_event, event->event);
    event->set(h_rho, event->rho);
    event->set(h_isRealData, event->isRealData);
    event->set(h_passEcalBadCalib, event->passEcalBadCalib);
    event->set(h_year, event->year);
    event->set(h_bsx, event->beamspot_x0);
    event->set(h_bsy, event->beamspot_y0);
    event->set(h_bsz, event->beamspot_z0);
    event->set(h_prefire, event->prefiringWeight);
    event->set(h_prefireUp, event->prefiringWeightUp);
    event->set(h_prefireDown, event->prefiringWeightDown);

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
