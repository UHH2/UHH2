#pragma once

#include "Particle.h"
#include "FlavorParticle.h"

#include <stdint.h>
#include <vector>

class Muon : public Particle {

 public:
  enum bool_id {
    soft = 0, loose, medium, tight, highpt,
    tracker, pf, global, standalone
  };

  enum tag {
    twodcut_dRmin,
    twodcut_pTrel,
  };

  bool get_bool(bool_id i) const {
    return (id_bits & (uint64_t(1) << static_cast<uint64_t>(i)));
  }

  void set_bool(bool_id i, bool value) {
    if(value) id_bits |=   uint64_t(1) << static_cast<uint64_t>(i);
    else      id_bits &= ~(uint64_t(1) << static_cast<uint64_t>(i));
  }

  struct source_candidate {

    long int key;
    float px;
    float py;
    float pz;
    float E;
  };

  Muon(){

    id_bits = 0;

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

  float pfMINIIso_CH()       const { return m_pfMINIIso_CH; }
  float pfMINIIso_NH()       const { return m_pfMINIIso_NH; }
  float pfMINIIso_Ph()       const { return m_pfMINIIso_Ph; }
  float pfMINIIso_PU()       const { return m_pfMINIIso_PU; }
  float pfMINIIso_NH_pfwgt() const { return m_pfMINIIso_NH_pfwgt; }
  float pfMINIIso_Ph_pfwgt() const { return m_pfMINIIso_Ph_pfwgt; }

  std::vector<source_candidate> source_candidates() const { return m_source_candidates; }

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
  void set_sumPhotonEt(float x){m_sumPhotonEt=x;}
  void set_sumPUPt(float x){m_sumPUPt=x;}

  void set_pfMINIIso_CH      (float x){ m_pfMINIIso_CH       = x; }
  void set_pfMINIIso_NH      (float x){ m_pfMINIIso_NH       = x; }
  void set_pfMINIIso_Ph      (float x){ m_pfMINIIso_Ph       = x; }
  void set_pfMINIIso_PU      (float x){ m_pfMINIIso_PU       = x; }
  void set_pfMINIIso_NH_pfwgt(float x){ m_pfMINIIso_NH_pfwgt = x; }
  void set_pfMINIIso_Ph_pfwgt(float x){ m_pfMINIIso_Ph_pfwgt = x; }

  void add_source_candidate(const source_candidate& sc){ m_source_candidates.push_back(sc); }

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
  uint64_t id_bits;

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

  std::vector<source_candidate> m_source_candidates;

  Tags tags;
};
