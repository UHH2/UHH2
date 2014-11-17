#include "UHH2/core/include/Event.h"
#include "UHH2/common/include/CleaningModules.h"

using namespace uhh2;
using namespace std;

JetCleaner::JetCleaner(double minpt_, double maxeta_): minpt(minpt_), maxeta(maxeta_){}

bool JetCleaner::process(Event & event){
    // move all failing jets to the end of the vector event.jets
    auto failing_jets_begin = std::remove_if(event.jets->begin(), event.jets->end(),
                   [&](const Jet & j){return j.pt() < minpt || fabs(j.eta()) > maxeta; });
    // erase the failing ones:
    event.jets->erase(failing_jets_begin, event.jets->end());
    return true;
}


MuonCleaner::MuonCleaner(const muon_id_type & muon_id_): muon_id(muon_id_){}

bool MuonCleaner::process(uhh2::Event & event){
    assert(event.muons);
    vector<Muon> cleaned_muons;
    for(const auto & mu : *event.muons){
        if(muon_id(mu, event)){
            cleaned_muons.push_back(mu);
        }
    }
    *event.muons = move(cleaned_muons);
    return true;
}


ElectronCleaner::ElectronCleaner(const ele_id_type & ele_id_): ele_id(ele_id_){}

bool ElectronCleaner::process(uhh2::Event & event){
    assert(event.electrons);
    vector<Electron> cleaned_electrons;
    for(const auto & ele : *event.electrons){
        if(ele_id(ele, event)){
            cleaned_electrons.push_back(ele);
        }
    }
    *event.electrons = move(cleaned_electrons);
    return true;
}

