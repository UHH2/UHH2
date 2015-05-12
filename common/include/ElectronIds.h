#pragma once

#include "UHH2/core/include/Event.h"

// Note that the electronIds are implemented not by inheriting from a common base class, but
// rather as 'something' that can be used as function with the signature
//   bool electron_id(const Electron &, const Event &);
// This allows to write selections or other modules taking these ids as
//  std::function<bool (const Electron &, const Event &)>
// and allows to pass either function pointers or more complicated objects.
//
// Note that the id function takes the Event as argument; this might look unnecessary at
// first, but actually quite some ids require accessing other event information, e.g. lepton
// ids require pileup density event.rho to calculate corrected PF isolation, or the jet-lepton
// DeltaR and ptrel, etc.

class PtEtaSCCut {
 public:
  PtEtaSCCut(float min_pt_, float max_etaSC_): min_pt(min_pt_), max_etaSC(max_etaSC_) {}

  bool operator()(const Electron& ele, const uhh2::Event&) const {
    return (ele.pt() > min_pt) && (fabs(ele.supercluster_eta()) < max_etaSC);
  }

 private:
  float min_pt, max_etaSC;
};

// see https://twiki.cern.ch/twiki/bin/view/CMS/CutBasedElectronIdentificationRun2
bool ElectronID_CSA14_50ns_loose(const Electron & electron, const uhh2::Event & event);
bool ElectronID_CSA14_50ns_medium(const Electron & electron, const uhh2::Event & event);
bool ElectronID_CSA14_50ns_tight(const Electron & electron, const uhh2::Event & event);

bool ElectronID_PHYS14_25ns_veto(const Electron & electron, const uhh2::Event & event);
bool ElectronID_PHYS14_25ns_loose(const Electron & electron, const uhh2::Event & event);
bool ElectronID_PHYS14_25ns_medium(const Electron & electron, const uhh2::Event & event);
bool ElectronID_PHYS14_25ns_tight(const Electron & electron, const uhh2::Event & event);

bool ElectronID_PHYS14_25ns_loose_noIso(const Electron & electron, const uhh2::Event & event);
bool ElectronID_PHYS14_25ns_medium_noIso(const Electron & electron, const uhh2::Event & event);
bool ElectronID_PHYS14_25ns_tight_noIso(const Electron & electron, const uhh2::Event & event);
