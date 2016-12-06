#include "UHH2/core/include/Event.h"
#include "UHH2/common/include/CleaningModules.h"
#include "UHH2/common/include/Utils.h"


using namespace uhh2;
using namespace std;


JetCleaner::JetCleaner(Context & ctx, const JetId & jet_id_, string const & label_):
    jet_id(jet_id_), hndl(ctx.get_handle<vector<Jet>>(label_)) {}

JetCleaner::JetCleaner(Context & ctx, float minpt, float maxeta, string const & label_):
    jet_id(PtEtaCut(minpt, maxeta)), hndl(ctx.get_handle<vector<Jet>>(label_)) {}

bool JetCleaner::process(Event & event){
    if (!event.is_valid(hndl)) {
        cerr << "In JetCleaner: Handle not valid!\n";
        assert(false);
    }
    vector<Jet> & jet_collection = event.get(hndl);
    clean_collection(jet_collection, event, jet_id);
    return true;
}

JetMuonOverlapRemoval::JetMuonOverlapRemoval(double deltaRmin):
deltaRmin_(deltaRmin){}

bool JetMuonOverlapRemoval::process(Event & event){
   
   assert(event.muons);
   std::vector<Jet> result;
   Muon lepton =event.muons->at(0);

   for(const auto & jet : *event.jets){
      if(deltaR(jet, lepton) > deltaRmin_){
         result.push_back(jet);
      }
   }
   std::swap(result, *event.jets);
   
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

TopJetCleaner::TopJetCleaner(Context & ctx, const TopJetId & topjet_id_, string const & label_):
    topjet_id(topjet_id_), hndl(ctx.get_handle<vector<TopJet>>(label_)) {}

bool TopJetCleaner::process(uhh2::Event & event){
    if (!event.is_valid(hndl)) {
        cerr << "In TopJetCleaner: Handle not valid!\n";
        assert(false);
    }
    vector<TopJet> & topjet_collection = event.get(hndl);
    clean_collection(topjet_collection, event, topjet_id);
    return true;
}
