#include "UHH2/core/include/Event.h"
#include "UHH2/common/include/CleaningModules.h"

using namespace uhh2;
using namespace std;

namespace {
    
// common code to filter out objects from a collection according to an object id
template<typename T>
void clean_collection(vector<T> & objects, const uhh2::Event & event, const std::function<bool (const T &, const Event &)> obj_id){
    vector<T> result;
    for(const auto & obj : objects){
        if(obj_id(obj, event)){
            result.push_back(obj);
        }
    }
    std::swap(result, objects);
}
    
}


JetCleaner::JetCleaner(const JetId & jet_id_): jet_id(jet_id_){}

JetCleaner::JetCleaner(float minpt, float maxeta): jet_id(PtEtaCut(minpt, maxeta)){}

bool JetCleaner::process(Event & event){
    clean_collection(*event.jets, event, jet_id);
    return true;
}


PrimaryVertexCleaner::PrimaryVertexCleaner(const PrimaryVertexId & vtx_id_): vtx_id(vtx_id_){}

bool PrimaryVertexCleaner::process(Event & event){
    assert(event.pvs);
    clean_collection(*event.pvs, event, vtx_id);\
    return true;
}

MuonCleaner::MuonCleaner(const MuonId & muon_id_): muon_id(muon_id_){}

bool MuonCleaner::process(uhh2::Event & event){
    assert(event.muons);
    clean_collection(*event.muons, event, muon_id);
    return true;
}


ElectronCleaner::ElectronCleaner(const ElectronId & ele_id_): ele_id(ele_id_){}

bool ElectronCleaner::process(uhh2::Event & event){
    assert(event.electrons);
    clean_collection(*event.electrons, event, ele_id);
    return true;
}

TauCleaner::TauCleaner(const TauId & tau_id_): tau_id(tau_id_){}

bool TauCleaner::process(uhh2::Event & event){
    assert(event.taus);
    clean_collection(*event.taus, event, tau_id);
    return true;
}



TopJetCleaner::TopJetCleaner(const TopJetId & topjet_id_): topjet_id(topjet_id_){}

bool TopJetCleaner::process(uhh2::Event & event){
    assert(event.topjets);
    clean_collection(*event.topjets, event, topjet_id);
    return true;
}
