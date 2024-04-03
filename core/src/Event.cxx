#include "UHH2/core/include/Event.h"
#include <sstream>
#include <stdexcept>

using namespace uhh2;
using namespace std;

void Event::clear(){
    run = luminosityBlock = event = -1;
    year = "";
    rho = beamspot_x0 = beamspot_y0 = beamspot_z0 = NAN;
    passEcalBadCalib = true;
    prefiringWeight = prefiringWeightUp = prefiringWeightDown = 1.;
    electrons = 0;
    muons = 0;
    taus = 0;
    photons = 0;
    pvs = 0;
    jets = 0;
    topjets = 0;
    toppuppijets = 0;
    gentopjets = 0;
    genparticles = 0;
    genjets = 0;
    pfparticles = 0;
    met = 0;
    genmet = 0;
    genInfo = 0;
    triggerResults = 0;
    triggerPrescales = 0;
    triggerNames_currentrun.clear();
    triggerNames_currentrun_runid = -1; // this is an invalid runid
}

Event::Event(const GenericEventStructure & ges): GenericEvent(ges){
    clear();
}

Event::~Event(){}


Event::TriggerIndex Event::get_trigger_index(const std::string & triggername) const{
    if(triggername.empty() || triggername == "*"){
        throw invalid_argument("invalid triggername='" + triggername + "' passed to Event::get_trigger_index");
    }
    bool trailing_wildcard = triggername[triggername.size() - 1] == '*';
    return TriggerIndex(0, -1, trailing_wildcard ? triggername.substr(0, triggername.size() - 1) : triggername, trailing_wildcard);
}

bool Event::lookup_trigger_index(TriggerIndex & ti) const{
    if(ti.runid == run){
        return true;
    }
    if(triggerNames_currentrun_runid != run){
        stringstream ss;
        ss << "Event::lookup_trigger: no trigger names for current run available: need trigger names for current runid=" << run << ", but last update was for runid=" << triggerNames_currentrun_runid
           << ". Is reading trigger names configured correctly?";
        throw runtime_error(ss.str());
    }
    int runid = -1;
    size_t index = triggerNames_currentrun.size();
    for(size_t i=0; i<triggerNames_currentrun.size(); ++i){
        bool matches = false;
        if(ti.is_prefix){
            matches = triggerNames_currentrun[i].find(ti.triggername) == 0;
        }
        else{
            matches = triggerNames_currentrun[i] == ti.triggername;
        }
        if(matches){
            if(index == triggerNames_currentrun.size()){ // first match
                index = i;
                runid = run;
            }
            else{
                stringstream ss;
                ss << "Trigger name '" << ti.triggername << (ti.is_prefix ? "*" : "")  << "' not unique: found at index " << ti.index << " and at index " << i << ".";
                throw runtime_error(ss.str());
            }
        }
    }
    if(runid == -1 || index >= triggerNames_currentrun.size()){
        return false;
    }
    ti.runid = runid;
    ti.index = index;
    return true;
}

namespace {

string format_list(const vector<string> & l){
    string result;
    for(const auto & i : l){
        result += i;
        result += "\n";
    }
    return result;
}

}


bool Event::passes_trigger(TriggerIndex & ti) const{
    if(!lookup_trigger_index(ti)){
        return false; // For Run3 only
        // throw runtime_error("Event does not have trigger '" + ti.triggername + "'. Available triggers:\n" + format_list(triggerNames_currentrun));
    }
    if(!triggerResults){
        throw runtime_error("Event::passes_trigger: trigger Results have not beed read in");
    }
    // for(auto &c: triggerNames_currentrun) cout << c << endl;
    if(triggerNames_currentrun.size() != triggerResults->size()){
        stringstream ss;
        ss << "Inconsistent trigger information: trigger names for current run have size " << triggerNames_currentrun.size() << ", but trigger results for current event have size " << triggerResults->size()
        << endl;
        // throw runtime_error(ss.str());
        return false; // For Run3 only
    }
    assert(triggerNames_currentrun.size() == triggerResults->size());
    return triggerResults->at(ti.index);
}


double Event::trigger_prescale(TriggerIndex & ti) const{
    if(!lookup_trigger_index(ti)){
        throw runtime_error("Event does not have trigger '" + ti.triggername + "'. Available triggers:\n" + format_list(triggerNames_currentrun));
    }
    if(!triggerPrescales){
        throw runtime_error("Event::trigger_prescale: trigger prescales have not beed read in; perhaps you are running on MC, prescales are only filled for data.");
    }
    if(triggerNames_currentrun.size() != triggerPrescales->size()){
        stringstream ss;
	ss << "Inconsistent trigger information: trigger names for current run have size " << triggerNames_currentrun.size() << ", but trigger prescales for current event have size " << triggerPrescales->size()
	   << "; perhaps you are running on MC, prescales are only filled for data." << endl;
	throw runtime_error(ss.str());
    }
    assert(triggerNames_currentrun.size() == triggerPrescales->size());
    return triggerPrescales->at(ti.index);
}

double Event::trigger_prescaleL1min(TriggerIndex & ti) const{
    if(!lookup_trigger_index(ti)){
        throw runtime_error("Event does not have trigger '" + ti.triggername + "'. Available triggers:\n" + format_list(triggerNames_currentrun));
    }
    if(!triggerPrescalesL1min){
        throw runtime_error("Event::trigger_prescale: trigger prescales have not beed read in; perhaps you are running on MC, prescales are only filled for data.");
    }
    if(triggerNames_currentrun.size() != triggerPrescalesL1min->size()){
        stringstream ss;
	ss << "Inconsistent trigger information: trigger names for current run have size " << triggerNames_currentrun.size() << ", but trigger prescales for current event have size " << triggerPrescales->size()
	   << "; perhaps you are running on MC, prescales are only filled for data." << endl;
	throw runtime_error(ss.str());
    }
    assert(triggerNames_currentrun.size() == triggerPrescalesL1min->size());
    return triggerPrescalesL1min->at(ti.index);
}

double Event::trigger_prescaleL1max(TriggerIndex & ti) const{
    if(!lookup_trigger_index(ti)){
        throw runtime_error("Event does not have trigger '" + ti.triggername + "'. Available triggers:\n" + format_list(triggerNames_currentrun));
    }
    if(!triggerPrescalesL1max){
        throw runtime_error("Event::trigger_prescale: trigger prescales have not beed read in; perhaps you are running on MC, prescales are only filled for data.");
    }
    if(triggerNames_currentrun.size() != triggerPrescalesL1max->size()){
        stringstream ss;
	ss << "Inconsistent trigger information: trigger names for current run have size " << triggerNames_currentrun.size() << ", but trigger prescales for current event have size " << triggerPrescales->size()
	   << "; perhaps you are running on MC, prescales are only filled for data." << endl;
	throw runtime_error(ss.str());
    }
    assert(triggerNames_currentrun.size() == triggerPrescalesL1max->size());
    return triggerPrescalesL1max->at(ti.index);
}
