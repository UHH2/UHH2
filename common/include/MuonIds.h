#pragma once

#include "UHH2/core/include/Event.h"

// see also ElectronIds.h for general comments

class MuonID_nonIso {
public:
    MuonID_nonIso(double ptmin, double etamax);
    bool operator()(const Muon & muon, const uhh2::Event & event) const;
    
private:
    double ptmin, etamax;
};
