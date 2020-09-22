#include "UHH2/common/include/DetectorCleaning.h"
#include "UHH2/core/include/Event.h"


EtaPhiEventCleaner::EtaPhiEventCleaner(Context& ctx, int min_run_,
  int max_run_, float min_eta_, float max_eta_, float min_phi_, float max_phi_,
  string jetCollection_, bool doJets_, bool doElectrons_, bool doMuons_):
  min_run(min_run_), max_run(max_run_), min_eta(min_eta_), max_eta(max_eta_),
  min_phi(min_phi_), max_phi(max_phi_), jetCollection(jetCollection_),
  doJets(doJets_), doElectrons(doElectrons_), doMuons(doMuons_){
    h_jets = ctx.get_handle<vector<Jet>>(jetCollection);
    h_topjets = ctx.get_handle<vector<TopJet>>(jetCollection);
  }

/* Remove events in data if at least one electron/muon/jet (depending on doJets,
doElectrons and doMuon) is found in area specified by the eta and phi
intervals. Runs only if the run rumber is in the interval [min_run, max_run]
(including bounds). Can pass -1 as max_run to ignore max_run. */
bool EtaPhiEventCleaner::passes(const Event& event){
  // only run on Data
  if (!event.isRealData) {
    return true;
  }

  // Run only if run is between min_run and max_run
  if (event.run < min_run ) return true;
  if (max_run != -1 && max_run < event.run) return true;

  if (doJets){
    // Get the jets for the event, depening on whether they are jets or topJets.
    vector<Jet> jets;
    if (event.is_valid(h_topjets)) jets.assign((&event.get(h_topjets))->begin(), (&event.get(h_topjets))->end());
    else if (event.is_valid(h_jets)) jets.assign((&event.get(h_jets))->begin(), (&event.get(h_jets))->end());
    else throw logic_error("DetectorCleaning: No valid jet collection given.");

    for (auto & jet: jets){
      if (min_eta < jet.eta() && jet.eta() < max_eta){
        if (min_phi < jet.phi() && jet.phi() < max_phi){
          return false;
        }
      }
    }
  }

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

HEMCleanerSelection::HEMCleanerSelection(Context& ctx, string jetCollection, bool doJets,
  bool doElectrons, bool doMuons):
  EtaPhiEventCleaner(ctx, min_run_HEM, max_run_HEM, min_eta_HEM,
    max_eta_HEM,min_phi_HEM, max_phi_HEM, jetCollection, doJets, doElectrons,
    doMuons){};
