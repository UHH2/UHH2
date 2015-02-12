#pragma once

#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/Utils.h"

// see also ElectronIds.h for general comments


// the tight muon id according to
// https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideMuonId
class MuonIDTight {
public:
    bool operator()(const Muon & muon, const uhh2::Event & event) const;
};

// only kinematic cuts, with no further id
class MuonIDKinematic {
public:
    MuonIDKinematic(double ptmin, double etamax) DEPRECATED("Use more general PtEtaCut instead");
    bool operator()(const Muon & muon, const uhh2::Event & event) const;
    
private:
    double ptmin, etamax;
};

//relativ Isolation for Muon check above twiki for reference
class MuonIso {
 public:
  MuonIso(double iso = 0.15);
  bool operator()(const Muon & muon, const uhh2::Event & event) const;
 private:
  double iso;
};
