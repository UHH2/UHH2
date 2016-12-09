#include "UHH2/common/include/MuonIds.h"

using namespace uhh2;

MuonIDKinematic::MuonIDKinematic(double ptmin_, double etamax_): ptmin(ptmin_), etamax(etamax_){}

bool MuonIDKinematic::operator()(const Muon & muon, const Event &) const {
    return muon.pt() > ptmin and fabs(muon.eta()) < etamax;
}

bool MuonIDLoose ::operator()(const Muon& muo, const Event&) const { return muo.get_bool(Muon::loose);  }
bool MuonIDMedium::operator()(const Muon& muo, const Event&) const { return muo.get_bool(Muon::medium); }
bool MuonIDTight ::operator()(const Muon& muo, const Event&) const { return muo.get_bool(Muon::tight);  }
bool MuonIDHighPt::operator()(const Muon& muo, const Event&) const { return muo.get_bool(Muon::highpt); }
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

bool MuonIDMedium_ICHEP::operator()(const Muon& muo, const Event&) const { 
   
   bool goodglobalmu = false;
   bool tightsegmentcomp =false;
   
   if (!muo.get_bool(Muon::loose)) return false;
   if (!(muo.innerTrack_validFraction() > 0.49 )) return false;
   
   if((muo.get_bool(Muon::global)) &&  (muo.globalTrack_normalizedChi2() < 3) && (muo.combinedQuality_chi2LocalPosition() < 12) && (muo.combinedQuality_trkKink() < 20) && (muo.segmentCompatibility() > 0.303)) goodglobalmu =true;
   if(muo.segmentCompatibility() > 0.451) tightsegmentcomp = true;

   return (goodglobalmu || tightsegmentcomp);
}
