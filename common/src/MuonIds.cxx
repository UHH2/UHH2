#include "UHH2/common/include/MuonIds.h"

using namespace uhh2;

MuonIDKinematic::MuonIDKinematic(double ptmin_, double etamax_): ptmin(ptmin_), etamax(etamax_){}

bool MuonIDKinematic::operator()(const Muon & muon, const Event &) const {
    return muon.pt() > ptmin and fabs(muon.eta()) < etamax;
}
    
bool MuonIDTight::operator()(const Muon & muon, const Event &) const {
    return muon.get_bool(Muon::tight);
}

MuonIso::MuonIso(double iso_):iso(iso_){}

bool MuonIso::operator()(const Muon & muon, const uhh2::Event &) const {
  if(muon.relIso()>iso) return false;
  return true;
}

