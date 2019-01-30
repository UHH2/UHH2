#include "UHH2/core/include/Event.h"
#include <sstream>
#include <stdexcept>

using namespace uhh2;
using namespace std;

void Event::clear(){
    run = luminosityBlock = event = -1;
    rho = beamspot_x0 = beamspot_y0 = beamspot_z0 = NAN;
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
    triggerPrescalesL1min = 0;
    triggerPrescalesL1max = 0;
    triggerNames_currentrun.clear();
    triggerNames_currentrun_runid = -1; // this is an invalid runid
    doOnce = true;
    L1EG_seeds = 0;
    L1J_seeds = 0;
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
    ti.index = triggerNames_currentrun.size();
    for(size_t i=0; i<triggerNames_currentrun.size(); ++i){
        bool matches = false;
        if(ti.is_prefix){
            matches = triggerNames_currentrun[i].find(ti.triggername) == 0;
        }
        else{
            matches = triggerNames_currentrun[i] == ti.triggername;
        }
        if(matches){
            if(ti.index == triggerNames_currentrun.size()){ // first match
                ti.index = i;
                ti.runid = run;
            }
            else{
                // FIXME: return to always throwing after ntuples made fixing MC trigger error
                if (doOnce) {
                    stringstream ss;
                    ss << "Trigger name '" << ti.triggername << (ti.is_prefix ? "*" : "")  << "' not unique: found at index " << ti.index << " and at index " << i << ".";
                    if (isRealData) {
                        throw runtime_error(ss.str());
                    } else {
                        ss << " This is probably due to double-storing triggers, or a non-unique wildcard match.";
                        cout << ss.str() << endl;
                        doOnce = false;
                    }
                }
            }
        }
    }
    if(ti.runid == -1 || ti.index >= triggerNames_currentrun.size()){
        return false;
    }
    
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
        throw runtime_error("Event does not have trigger '" + ti.triggername + "'. Available triggers:\n" + format_list(triggerNames_currentrun));
    }
    if(!triggerResults){
        throw runtime_error("Event::passes_trigger: trigger Results have not beed read in");
    }
     if(triggerNames_currentrun.size() != triggerResults->size()){
         stringstream ss;
         ss << "Inconsistent trigger information: trigger names for current run have size " << triggerNames_currentrun.size() << ", but trigger results for current event have size " << triggerResults->size()
            << endl;
         throw runtime_error(ss.str());
     }
     assert(triggerNames_currentrun.size() == triggerResults->size());
    return triggerResults->at(ti.index);
}


//bool Event::trigger_prescale(TriggerIndex & ti) const{
int Event::trigger_prescale(TriggerIndex & ti) const{
    if(!lookup_trigger_index(ti)){
        throw runtime_error("Event does not have trigger '" + ti.triggername + "'. Available triggers:\n" + format_list(triggerNames_currentrun));
    }
    if(!triggerPrescales){
        throw runtime_error("Event::trigger_prescale: trigger prescales have not beed read in; perhaps you are running on MC, prescales are only filled for data.");
    }

    //Forgot to add prescales for bit with BX info :(
    //ToDo: add it in the next production and uncomment check below
    /*
    if(triggerNames_currentrun.size() != triggerPrescales->size()){
        stringstream ss;
	ss << "Inconsistent trigger information: trigger names for current run have size " << triggerNames_currentrun.size() << ", but trigger prescales for current event have size " << triggerPrescales->size()
	   << "; perhaps you are running on MC, prescales are only filled for data." << endl;
	throw runtime_error(ss.str());
    }
    assert(triggerNames_currentrun.size() == triggerPrescales->size());
    */

    //    cout<<" triggerPrescales->at(ti.index) = "<<triggerPrescales->at(ti.index)<<endl;
    return triggerPrescales->at(ti.index);
}


int Event::trigger_prescaleL1min(TriggerIndex & ti) const{
    if(!lookup_trigger_index(ti)){
        throw runtime_error("Event does not have trigger '" + ti.triggername + "'. Available triggers:\n" + format_list(triggerNames_currentrun));
    }
    if(!triggerPrescalesL1min){
        throw runtime_error("Event::trigger_prescaleL1min: trigger prescales have not beed read in; perhaps you are running on MC, prescales are only filled for data.");
    }
    if(triggerNames_currentrun.size() != triggerPrescalesL1min->size()){
        stringstream ss;
	ss << "Inconsistent trigger information: trigger names for current run have size " << triggerNames_currentrun.size() << ", but trigger prescales for current event have size " << triggerPrescalesL1min->size()
	   << "; perhaps you are running on MC, prescales are only filled for data." << endl;
	throw runtime_error(ss.str());
    }
    assert(triggerNames_currentrun.size() == triggerPrescalesL1min->size());
    return triggerPrescalesL1min->at(ti.index);
}


int Event::trigger_prescaleL1max(TriggerIndex & ti) const{
    if(!lookup_trigger_index(ti)){
        throw runtime_error("Event does not have trigger '" + ti.triggername + "'. Available triggers:\n" + format_list(triggerNames_currentrun));
    }
    if(!triggerPrescalesL1max){
        throw runtime_error("Event::trigger_prescale: trigger prescales have not beed read in; perhaps you are running on MC, prescales are only filled for data.");
    }
    if(triggerNames_currentrun.size() != triggerPrescalesL1max->size()){
        stringstream ss;
	ss << "Inconsistent trigger information: trigger names for current run have size " << triggerNames_currentrun.size() << ", but trigger prescales for current event have size " << triggerPrescalesL1max->size()
	   << "; perhaps you are running on MC, prescales are only filled for data." << endl;
	throw runtime_error(ss.str());
    }
    assert(triggerNames_currentrun.size() == triggerPrescalesL1max->size());
    return triggerPrescalesL1max->at(ti.index);
}
