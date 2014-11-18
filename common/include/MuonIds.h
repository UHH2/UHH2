#pragma once

#include "UHH2/core/include/Event.h"

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
    MuonIDKinematic(double ptmin, double etamax);
    bool operator()(const Muon & muon, const uhh2::Event & event) const;
    
private:
    double ptmin, etamax;
};
