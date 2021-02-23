#include "UHH2/common/include/DetectorCleaning.h"
#include "UHH2/core/include/Event.h"


EtaPhiEventCleaner::EtaPhiEventCleaner(Context& ctx, const int min_run_, const int max_run_, const float min_eta_, const float max_eta_, const float min_phi_, const float max_phi_,
  const string& JetCollection_, const string& TopJetCollection_, const bool doElectrons_, const bool doMuons_):
  min_run(min_run_), max_run(max_run_), min_eta(min_eta_), max_eta(max_eta_), min_phi(min_phi_), max_phi(max_phi_),
  JetCollection(JetCollection_), TopJetCollection(TopJetCollection_), doElectrons(doElectrons_), doMuons(doMuons_) {

  h_Jets = ctx.get_handle<vector<Jet>>(JetCollection);
  h_TopJets = ctx.get_handle<vector<TopJet>>(TopJetCollection);
}

/* Remove events in data if at least one electron/muon/jet/TopJet is found in area
specified by the eta and phi intervals. Runs only if the run rumber is in the interval [min_run, max_run]
(including bounds). Can pass -1 as max_run to ignore max_run. */
bool EtaPhiEventCleaner::passes(const Event& event){
  // only run on Data
  if (!event.isRealData) {
    return true;
  }

  // Run only if run is between min_run and max_run
  if (event.run < min_run ) return true;
  if (max_run != -1 && max_run < event.run) return true;



  if (event.is_valid(h_Jets)) {
    for (auto & jet: event.get(h_Jets)){
      if (min_eta < jet.eta() && jet.eta() < max_eta){
        if (min_phi < jet.phi() && jet.phi() < max_phi){
          return false;
        }
      }
    }
  } else if (JetCollection!="") throw logic_error("DetectorCleaning: No valid Jet collection given. JetCollection = "+JetCollection);

  if (event.is_valid(h_TopJets)) {
    for (auto & jet: event.get(h_TopJets)){
      if (min_eta < jet.eta() && jet.eta() < max_eta){
        if (min_phi < jet.phi() && jet.phi() < max_phi){
          return false;
        }
      }
    }
  } else if (TopJetCollection!="") throw logic_error("DetectorCleaning: No valid TopJet collection given. TopJetCollection = "+TopJetCollection);


  if (doElectrons){
    for(const auto & ele : *event.electrons){
      if (min_eta < ele.eta() && ele.eta() < max_eta){
        if (min_phi < ele.phi() && ele.phi() < max_phi){
          return false;
        }
      }
    }
  }

  if (doMuons){
    for(const auto & muon : *event.muons){
      if (min_eta < muon.eta() && muon.eta() < max_eta){
        if (min_phi < muon.phi() && muon.phi() < max_phi){
          return false;
        }
      }
    }
  }
  return true;
}

HEMCleanerSelection::HEMCleanerSelection(Context& ctx, const string& JetCollection, const string& TopJetCollection, const bool doElectrons, const bool doMuons):
EtaPhiEventCleaner(ctx, min_run_HEM, max_run_HEM, min_eta_HEM, max_eta_HEM,min_phi_HEM, max_phi_HEM, JetCollection, TopJetCollection, doElectrons, doMuons){};
