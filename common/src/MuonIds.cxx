#include "UHH2/common/include/MuonIds.h"

using namespace uhh2;

MuonIDKinematic::MuonIDKinematic(double ptmin_, double etamax_): ptmin(ptmin_), etamax(etamax_){}

bool MuonIDKinematic::operator()(const Muon & muon, const Event &) const {
    return muon.pt() > ptmin and fabs(muon.eta()) < etamax;
}
    
bool MuonIDTight::operator()(const Muon & muon, const Event & event) const {
    if(event.pvs == 0) throw std::invalid_argument("MuonIDTight requires reading the primary vertices Event::pvs");
    if(event.pvs->size() == 0) return false;
    if(!muon.isGlobalMuon()) return false;
    if(!muon.isPFMuon()) return false;
    float nchi2 = muon.globalTrack_chi2() / muon.globalTrack_ndof();
    if(nchi2 > 10.) return false;
    if(muon.globalTrack_numberOfValidMuonHits() <= 0) return false;
    if(muon.numberOfMatchedStations() <= 1) return false;
    if(muon.dB() > 0.2) return false;
    if(fabs(muon.vertex_z() - event.pvs->at(0).z()) > 0.5) return false;
    if(muon.innerTrack_numberOfValidPixelHits() <= 0) return false;
    if(muon.innerTrack_trackerLayersWithMeasurement() <= 5) return false;
    return true;
}

MuonIso::MuonIso(double iso_):iso(iso_){}

bool MuonIso::operator()(const Muon & muon, const uhh2::Event &) const {
  if(muon.relIso()>iso) return false;
  return true;
}
