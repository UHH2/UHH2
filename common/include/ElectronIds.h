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

// Electron Cut-Based ID
// REF https://twiki.cern.ch/twiki/bin/view/CMS/CutBasedElectronIdentificationRun2
bool ElectronID_CSA14_50ns_loose (const Electron&, const uhh2::Event&);
bool ElectronID_CSA14_50ns_medium(const Electron&, const uhh2::Event&);
bool ElectronID_CSA14_50ns_tight (const Electron&, const uhh2::Event&);

bool ElectronID_PHYS14_25ns_veto  (const Electron&, const uhh2::Event&);
bool ElectronID_PHYS14_25ns_loose (const Electron&, const uhh2::Event&);
bool ElectronID_PHYS14_25ns_medium(const Electron&, const uhh2::Event&);
bool ElectronID_PHYS14_25ns_tight (const Electron&, const uhh2::Event&);

bool ElectronID_PHYS14_25ns_veto_noIso  (const Electron&, const uhh2::Event&);
bool ElectronID_PHYS14_25ns_loose_noIso (const Electron&, const uhh2::Event&);
bool ElectronID_PHYS14_25ns_medium_noIso(const Electron&, const uhh2::Event&);
bool ElectronID_PHYS14_25ns_tight_noIso (const Electron&, const uhh2::Event&);

// Electron MVA ID
// REF https://twiki.cern.ch/twiki/bin/view/CMS/MultivariateElectronIdentificationRun2
bool ElectronID_MVAnotrig_PHYS14_loose(const Electron&, const uhh2::Event&);
bool ElectronID_MVAnotrig_PHYS14_tight(const Electron&, const uhh2::Event&);

// Electron selectors for PF MINI-Isolation
class Electron_MINIIso {

 public:
  explicit Electron_MINIIso(const float iso_cut, const std::string& iso_key): iso_cut_(iso_cut), iso_key_(iso_key) {}
  virtual ~Electron_MINIIso() {}

  bool operator()(const Electron&, const uhh2::Event&) const;

 protected:
  float iso_cut_;
  std::string iso_key_;
};
