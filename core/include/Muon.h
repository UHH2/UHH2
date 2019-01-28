#pragma once

#include "Particle.h"
#include "FlavorParticle.h"
#include "source_candidate.h"

#include <stdint.h>
#include <vector>

class Muon : public Particle {

 public:
  enum Selector{
    Tracker,
    PF,
    Global,
    Standalone,
    // 2016v2 selectors:
    Soft,
    Loose,
    Medium,
    Tight,
    Highpt,
    // 2016v3 & 2017 onwards selectors:
    CutBasedIdLoose,
    CutBasedIdMedium, 
    CutBasedIdMediumPrompt, 
    CutBasedIdTight, 
    CutBasedIdGlobalHighPt, 
    CutBasedIdTrkHighPt, 
    SoftCutBasedId, 
    SoftMvaId, 
    MvaLoose, 
    MvaMedium, 
    MvaTight,
    PFIsoVeryLoose,
    PFIsoLoose,
    PFIsoMedium,
    PFIsoTight,
    PFIsoVeryTight,
    TkIsoLoose,
    TkIsoTight,
    MiniIsoLoose,           
    MiniIsoMedium,          
    MiniIsoTight,           
    MiniIsoVeryTight
  };

  /*
  CLASSIFICATION: For each RECO Muon, match to SIM particle, and then:
  - If the SIM is not a Muon, label as Punchthrough (1) except if it is an electron or positron (11)
  - If the SIM is a Muon, then look at it's provenance.
     A) the SIM muon is also a GEN muon, whose parent is NOT A HADRON AND NOT A TAU
        -> classify as "primary" (13).
     B) the SIM muon is also a GEN muon, whose parent is HEAVY FLAVOURED HADRON OR A TAU
        -> classify as "heavy quark" (3) or "tau" (15)
     C) classify as "light flavour/decay" (2)
  In any case, if the TP is not preferentially matched back to the same RECO muon,
  label as Ghost (flip the classification)

  Note that this is only available for 2017 datasets and later
  */
  enum SimType{ 
    Unknown                     = 999, 
    NotMatched                  = 0,
    MatchedPunchthrough         = 1,
    MatchedElectron             = 11,
    MatchedTau                  = 15,
    MatchedPrimaryMuon          = 13,
    MatchedHeavyQuark           = 3,
    MatchedLightQuark           = 2,
    GhostPunchthrough           = -1,
    GhostElectron               = -11,
    GhostTau                    = -15,
    GhostPrimaryMuon            = -13,
    GhostHeavyQuark             = -3,
    GhostLightQuark             = -2
    
  };

  enum MuonTrackType{
    InnerTk,
    OuterTk,
    GlobalTk,
    Segments,
    GlbOrTrk
  };

  enum tag {
    twodcut_dRmin,
    twodcut_pTrel,
  };

  bool get_selector(Selector i) const {
    return (sel_bits & (uint64_t(1) << static_cast<uint64_t>(i)));
  }

  void set_selector(Selector i, bool value) {
    if(value) sel_bits |=   uint64_t(1) << static_cast<uint64_t>(i);
    else      sel_bits &= ~(uint64_t(1) << static_cast<uint64_t>(i));
  }

  Muon(){

    sel_bits = 0;

    m_dxy = 0;
    m_dxy_error = 0;
    m_dz = 0;
    m_dz_error = 0;

    m_globalTrack_normalizedChi2 = 0;
    m_globalTrack_numberOfValidMuonHits = 0;
    m_numberOfMatchedStations = 0;
    m_innerTrack_trackerLayersWithMeasurement = 0;
    m_innerTrack_numberOfValidPixelHits = 0;
    m_innerTrack_validFraction = 0;
    m_combinedQuality_chi2LocalPosition = 0;
    m_combinedQuality_trkKink = 0;
    m_segmentCompatibility = 0;

    m_sumChargedHadronPt = 0;
    m_sumNeutralHadronEt = 0;
    m_sumPhotonEt = 0;
    m_sumPUPt = 0;

    m_pfMINIIso_CH       = 0;
    m_pfMINIIso_NH       = 0;
    m_pfMINIIso_Ph       = 0;
    m_pfMINIIso_PU       = 0;
    m_pfMINIIso_NH_pfwgt = 0;
    m_pfMINIIso_Ph_pfwgt = 0;

    m_simType = Unknown;
    m_simFlavor = 0;
    m_simPdgId = 0;
    m_simMotherPdgId = 0;
    m_simHeaviestMotherFlavor = 0; 

    m_tunePTrackPt = 0;
    m_tunePTrackEta = -999;
    m_tunePTrackPhi = -999;
    m_tunePTrackType = InnerTk;

    m_source_candidates.clear();
  }

  float dxy() const{return m_dxy;}
  float dxy_error() const{return m_dxy_error;}
  float dz() const{return m_dz;}
  float dz_error() const{return m_dz_error;}

  float globalTrack_normalizedChi2()              const { return m_globalTrack_normalizedChi2; }
  int   globalTrack_numberOfValidMuonHits()       const { return m_globalTrack_numberOfValidMuonHits; }
  int   numberOfMatchedStations()                 const { return m_numberOfMatchedStations; }
  int   innerTrack_trackerLayersWithMeasurement() const { return m_innerTrack_trackerLayersWithMeasurement; }
  int   innerTrack_numberOfValidPixelHits()       const { return m_innerTrack_numberOfValidPixelHits; }
  float innerTrack_validFraction()                const { return m_innerTrack_validFraction; }
  float combinedQuality_chi2LocalPosition()       const { return m_combinedQuality_chi2LocalPosition; }
  float combinedQuality_trkKink()                 const { return m_combinedQuality_trkKink; }
  float segmentCompatibility()                    const { return m_segmentCompatibility; }

  float sumChargedHadronPt() const{ return m_sumChargedHadronPt; } 
  float sumNeutralHadronEt() const{ return m_sumNeutralHadronEt; } 
  float sumPhotonEt()        const{ return m_sumPhotonEt; }
  float sumPUPt()            const{ return m_sumPUPt; }

  float pfMINIIso_CH()       const { return  m_pfMINIIso_CH; }
  float pfMINIIso_NH()       const { return m_pfMINIIso_NH; }
  float pfMINIIso_Ph()       const { return m_pfMINIIso_Ph; }
  float pfMINIIso_PU()       const { return m_pfMINIIso_PU; }
  float pfMINIIso_NH_pfwgt() const { return m_pfMINIIso_NH_pfwgt; }
  float pfMINIIso_Ph_pfwgt() const { return m_pfMINIIso_Ph_pfwgt; }

  SimType simType()             const {return m_simType;}
  int simFlavor()               const {return m_simFlavor;}
  int simPdgId()                const {return m_simPdgId;}
  int simMotherPdgId()          const {return m_simMotherPdgId;}
  int simHeaviestMotherFlavor() const {return m_simHeaviestMotherFlavor;} 

  float tunePTrackPt()           const {return m_tunePTrackPt;}
  float tunePTrackEta()          const {return m_tunePTrackEta;}
  float tunePTrackPhi()          const {return m_tunePTrackPhi;}
  MuonTrackType tunePTrackType() const {return m_tunePTrackType;}

  const std::vector<source_candidate>& source_candidates() const { return m_source_candidates; }

  void set_dxy(float x){m_dxy=x;}
  void set_dxy_error(float x){m_dxy_error=x;}
  void set_dz(float x){m_dz=x;} 
  void set_dz_error(float x){m_dz_error=x;} 

  void set_globalTrack_normalizedChi2             (float x){ m_globalTrack_normalizedChi2 = x; }
  void set_globalTrack_numberOfValidMuonHits      (int   x){ m_globalTrack_numberOfValidMuonHits = x; }
  void set_numberOfMatchedStations                (int   x){ m_numberOfMatchedStations = x; }
  void set_innerTrack_trackerLayersWithMeasurement(int   x){ m_innerTrack_trackerLayersWithMeasurement = x; }
  void set_innerTrack_numberOfValidPixelHits      (int   x){ m_innerTrack_numberOfValidPixelHits = x; }
  void set_innerTrack_validFraction               (float x){ m_innerTrack_validFraction = x; }
  void set_combinedQuality_chi2LocalPosition      (float x){ m_combinedQuality_chi2LocalPosition = x; }
  void set_combinedQuality_trkKink                (float x){ m_combinedQuality_trkKink = x; }
  void set_segmentCompatibility                   (float x){ m_segmentCompatibility = x; }

  void set_sumChargedHadronPt(float x){m_sumChargedHadronPt=x;} 
  void set_sumNeutralHadronEt(float x){m_sumNeutralHadronEt=x;} 
  void set_sumPhotonEt       (float x){m_sumPhotonEt=x;}
  void set_sumPUPt           (float x){m_sumPUPt=x;}

  void set_pfMINIIso_CH      (float x){ m_pfMINIIso_CH       = x; }
  void set_pfMINIIso_NH      (float x){ m_pfMINIIso_NH       = x; }
  void set_pfMINIIso_Ph      (float x){ m_pfMINIIso_Ph       = x; }
  void set_pfMINIIso_PU      (float x){ m_pfMINIIso_PU       = x; }
  void set_pfMINIIso_NH_pfwgt(float x){ m_pfMINIIso_NH_pfwgt = x; }
  void set_pfMINIIso_Ph_pfwgt(float x){ m_pfMINIIso_Ph_pfwgt = x; }

  void set_simType            (SimType x){m_simType = x;}
  void set_simFlavor              (int x){m_simFlavor = x;}
  void set_simPdgId               (int x){m_simPdgId = x;}
  void set_simMotherPdgId         (int x){m_simMotherPdgId = x;}
  void set_simHeaviestMotherFlavor(int x){m_simHeaviestMotherFlavor = x;} 

  void set_tunePTrackPt  (float x){ m_tunePTrackPt = x; }
  void set_tunePTrackEta (float x){ m_tunePTrackEta = x; }
  void set_tunePTrackPhi (float x){ m_tunePTrackPhi = x; }
  void set_tunePTrackType(MuonTrackType x){ m_tunePTrackType = x; }

  void set_source_candidates(const std::vector<source_candidate>& vsc){ m_source_candidates = vsc; }
  void add_source_candidate (const source_candidate& sc){ m_source_candidates.push_back(sc); }

  bool  has_tag(tag t) const { return tags.has_tag(static_cast<int>(t)); }
  float get_tag(tag t) const { return tags.get_tag(static_cast<int>(t)); }
  void  set_tag(tag t, float value) { tags.set_tag(static_cast<int>(t), value); }

  float relIso() const{
    return ( m_sumChargedHadronPt + std::max( 0.0, m_sumNeutralHadronEt + m_sumPhotonEt - 0.5*m_sumPUPt) ) / pt();
  }

  operator FlavorParticle() const {

    FlavorParticle fp;
    fp.set_charge(this->charge());
    fp.set_pt(this->pt());
    fp.set_eta(this->eta());
    fp.set_phi(this->phi());
    fp.set_energy(this->energy());
    fp.set_pdgId(-13*this->charge());

    return fp;
  }

 private:
  uint64_t sel_bits;

  float m_dxy;
  float m_dxy_error;
  float m_dz;
  float m_dz_error;

  float m_globalTrack_normalizedChi2;
  int   m_globalTrack_numberOfValidMuonHits;
  int   m_numberOfMatchedStations;
  int   m_innerTrack_trackerLayersWithMeasurement;
  int   m_innerTrack_numberOfValidPixelHits;
  float m_innerTrack_validFraction;
  float m_combinedQuality_chi2LocalPosition;
  float m_combinedQuality_trkKink;
  float m_segmentCompatibility;

  float m_sumChargedHadronPt;
  float m_sumNeutralHadronEt;
  float m_sumPhotonEt;
  float m_sumPUPt;

  float m_pfMINIIso_CH;
  float m_pfMINIIso_NH;
  float m_pfMINIIso_Ph;
  float m_pfMINIIso_PU;
  float m_pfMINIIso_NH_pfwgt;
  float m_pfMINIIso_Ph_pfwgt;

  SimType m_simType;
  int m_simFlavor;
  int m_simPdgId;
  int m_simMotherPdgId;
  int m_simHeaviestMotherFlavor;

  float m_tunePTrackPt;
  float m_tunePTrackEta;
  float m_tunePTrackPhi;
  MuonTrackType m_tunePTrackType;

  std::vector<source_candidate> m_source_candidates;

  Tags tags;
};
