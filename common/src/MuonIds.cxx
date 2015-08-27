#include "UHH2/common/include/MuonIds.h"

using namespace uhh2;

MuonIDKinematic::MuonIDKinematic(double ptmin_, double etamax_): ptmin(ptmin_), etamax(etamax_){}

bool MuonIDKinematic::operator()(const Muon & muon, const Event &) const {
    return muon.pt() > ptmin and fabs(muon.eta()) < etamax;
}

bool MuonIDLoose ::operator()(const Muon& muo, const Event&) const { return muo.get_bool(Muon::loose);  }
bool MuonIDMedium::operator()(const Muon& muo, const Event&) const { return muo.get_bool(Muon::medium); }
bool MuonIDTight ::operator()(const Muon& muo, const Event&) const { return muo.get_bool(Muon::tight);  }

MuonIso::MuonIso(double iso_):iso(iso_){}

bool MuonIso::operator()(const Muon & muon, const uhh2::Event &) const {
  if(muon.relIso()>iso) return false;
  return true;
}
///

bool Muon_MINIIso::operator()(const Muon& muo, const uhh2::Event&) const {

  float iso(-1.);

  if(!muo.pt()) throw std::runtime_error("Muon_MINIIso::operator() -- null muon transverse momentum: failed to calculate relative MINI-Isolation");

  if     (iso_key_ == "uncorrected") iso = (muo.pfMINIIso_CH() + muo.pfMINIIso_NH() + muo.pfMINIIso_Ph())/muo.pt();
  else if(iso_key_ == "delta-beta")  iso = (muo.pfMINIIso_CH() + std::max(0., muo.pfMINIIso_NH() + muo.pfMINIIso_Ph() - .5*muo.pfMINIIso_PU()))/muo.pt();
  else if(iso_key_ == "pf-weight")   iso = (muo.pfMINIIso_CH() + muo.pfMINIIso_NH_pfwgt() + muo.pfMINIIso_Ph_pfwgt())/muo.pt();
  else throw std::runtime_error("Muon_MINIIso::operator() -- invalid key for MINI-Isolation pileup correction: "+iso_key_);

  return (iso < iso_cut_);
}
