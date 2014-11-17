#include "UHH2/common/include/MuonIds.h"

using namespace uhh2;

MuonID_nonIso::MuonID_nonIso(double ptmin_, double etamax_): ptmin(ptmin_), etamax(etamax_){}

bool MuonID_nonIso::operator()(const Muon & muon, const Event &) const {
    return muon.pt() > ptmin and fabs(muon.eta()) < etamax;
}
    
