#pragma once

#include "RecParticle.h"
#include "Tags.h"
#include "FlavorParticle.h"
#include "source_candidate.h"
#include <vector>


class Photon : public RecParticle {
public:

  enum tag {
    // 2016
    cutBasedPhotonID_Spring16_V2p2_loose,
    cutBasedPhotonID_Spring16_V2p2_medium,
    cutBasedPhotonID_Spring16_V2p2_tight,
    mvaPhoID_Spring16_nonTrig_V1_wp90,
    mvaPhoID_Spring16_nonTrig_V1_wp80,
    // 2017 & 2018
    cutBasedPhotonID_Fall17_94X_V2_loose,
    cutBasedPhotonID_Fall17_94X_V2_medium,
    cutBasedPhotonID_Fall17_94X_V2_tight,
    mvaPhoID_Fall17_iso_V2_wp90,
    mvaPhoID_Fall17_iso_V2_wp80,

    // EGamma Residuals
    // In general it would be better to implement these corrections as usual class members: see commented out code below. But we use the "Tags" solution here for backwards compatibility with older ntuples of the RunII_106X_v2 branch.
    residual_ecalEnergyPreCorr,
    residual_ecalEnergyErrPreCorr,
    residual_ecalEnergyPostCorr,
    residual_ecalEnergyErrPostCorr,
    residual_ecalTrkEnergyPreCorr,
    residual_ecalTrkEnergyErrPreCorr,
    residual_ecalTrkEnergyPostCorr,
    residual_ecalTrkEnergyErrPostCorr,
    residual_energyScaleValue,
    residual_energySigmaValue,
    residual_energySmearNrSigma,
    residual_energyScaleUp,
    residual_energyScaleDown,
    residual_energyScaleStatUp,
    residual_energyScaleStatDown,
    residual_energyScaleSystUp,
    residual_energyScaleSystDown,
    residual_energyScaleGainUp,
    residual_energyScaleGainDown,
    residual_energyScaleEtUp,
    residual_energyScaleEtDown,
    residual_energySigmaUp,
    residual_energySigmaDown,
    residual_energySigmaPhiUp,
    residual_energySigmaPhiDown,
    residual_energySigmaRhoUp,
    residual_energySigmaRhoDown,
  };

  static tag tagname2tag(const std::string & tagname){
    if(tagname == "cutBasedPhotonID-Spring16-V2p2-loose") return cutBasedPhotonID_Spring16_V2p2_loose;
    if(tagname == "cutBasedPhotonID-Spring16-V2p2-medium") return cutBasedPhotonID_Spring16_V2p2_medium;
    if(tagname == "cutBasedPhotonID-Spring16-V2p2-tight") return cutBasedPhotonID_Spring16_V2p2_tight;
    if(tagname == "mvaPhoID-Spring16-nonTrig-V1-wp90") return mvaPhoID_Spring16_nonTrig_V1_wp90;
    if(tagname == "mvaPhoID-Spring16-nonTrig-V1-wp80") return mvaPhoID_Spring16_nonTrig_V1_wp80;
    if(tagname == "cutBasedPhotonID-Fall17-94X-V2-loose") return cutBasedPhotonID_Fall17_94X_V2_loose;
    if(tagname == "cutBasedPhotonID-Fall17-94X-V2-medium") return cutBasedPhotonID_Fall17_94X_V2_medium;
    if(tagname == "cutBasedPhotonID-Fall17-94X-V2-tight") return cutBasedPhotonID_Fall17_94X_V2_tight;
    if(tagname == "mvaPhoID-RunIIFall17-v2-wp90") return mvaPhoID_Fall17_iso_V2_wp90;
    if(tagname == "mvaPhoID-RunIIFall17-v2-wp80") return mvaPhoID_Fall17_iso_V2_wp80;

    // EGamma Residuals
    if(tagname == "residual_ecalEnergyPreCorr") return residual_ecalEnergyPreCorr;
    if(tagname == "residual_ecalEnergyErrPreCorr") return residual_ecalEnergyErrPreCorr;
    if(tagname == "residual_ecalEnergyPostCorr") return residual_ecalEnergyPostCorr;
    if(tagname == "residual_ecalEnergyErrPostCorr") return residual_ecalEnergyErrPostCorr;
    if(tagname == "residual_ecalTrkEnergyPreCorr") return residual_ecalTrkEnergyPreCorr;
    if(tagname == "residual_ecalTrkEnergyErrPreCorr") return residual_ecalTrkEnergyErrPreCorr;
    if(tagname == "residual_ecalTrkEnergyPostCorr") return residual_ecalTrkEnergyPostCorr;
    if(tagname == "residual_ecalTrkEnergyErrPostCorr") return residual_ecalTrkEnergyErrPostCorr;
    if(tagname == "residual_energyScaleValue") return residual_energyScaleValue;
    if(tagname == "residual_energySigmaValue") return residual_energySigmaValue;
    if(tagname == "residual_energySmearNrSigma") return residual_energySmearNrSigma;
    if(tagname == "residual_energyScaleUp") return residual_energyScaleUp;
    if(tagname == "residual_energyScaleDown") return residual_energyScaleDown;
    if(tagname == "residual_energyScaleStatUp") return residual_energyScaleStatUp;
    if(tagname == "residual_energyScaleStatDown") return residual_energyScaleStatDown;
    if(tagname == "residual_energyScaleSystUp") return residual_energyScaleSystUp;
    if(tagname == "residual_energyScaleSystDown") return residual_energyScaleSystDown;
    if(tagname == "residual_energyScaleGainUp") return residual_energyScaleGainUp;
    if(tagname == "residual_energyScaleGainDown") return residual_energyScaleGainDown;
    if(tagname == "residual_energyScaleEtUp") return residual_energyScaleEtUp;
    if(tagname == "residual_energyScaleEtDown") return residual_energyScaleEtDown;
    if(tagname == "residual_energySigmaUp") return residual_energySigmaUp;
    if(tagname == "residual_energySigmaDown") return residual_energySigmaDown;
    if(tagname == "residual_energySigmaPhiUp") return residual_energySigmaPhiUp;
    if(tagname == "residual_energySigmaPhiDown") return residual_energySigmaPhiDown;
    if(tagname == "residual_energySigmaRhoUp") return residual_energySigmaRhoUp;
    if(tagname == "residual_energySigmaRhoDown") return residual_energySigmaRhoDown;

    throw std::runtime_error("unknown Photon::tag '" + tagname + "'");
  }

  Photon(){
    m_vertex_x = -1e4;
    m_vertex_y = -1e4;
    m_vertex_z = -1e4;
    m_supercluster_eta = -1e4;
    m_supercluster_phi = -1e4;

    m_puppiChargedHadronIso = -1;
    m_puppiNeutralHadronIso = -1;
    m_puppiPhotonIso = -1;
    m_trackIso =-1;
    m_ecalIso =-1;
    m_hcalIso =-1;
    m_caloIso =-1;
    m_patParticleIso =-1;
    m_chargedHadronIso =-1;
    m_neutralHadronIso =-1;
    m_photonIso =-1;
    m_puChargedHadronIso =-1;

    m_minDeltaRToL1EGamma = 10.;

    // EGamma Residuals
    /*
    m_ecalEnergyPreCorr = -99;
    m_ecalEnergyErrPreCorr = -99;
    m_ecalEnergyPostCorr = -99;
    m_ecalEnergyErrPostCorr = -99;
    m_ecalTrkEnergyPreCorr = -99;
    m_ecalTrkEnergyErrPreCorr = -99;
    m_ecalTrkEnergyPostCorr = -99;
    m_ecalTrkEnergyErrPostCorr = -99;
    m_energyScaleValue = -99;
    m_energySigmaValue = -99;
    m_energySmearNrSigma = -99;
    m_energyScaleUp = -99;
    m_energyScaleDown = -99;
    m_energyScaleStatUp = -99;
    m_energyScaleStatDown = -99;
    m_energyScaleSystUp = -99;
    m_energyScaleSystDown = -99;
    m_energyScaleGainUp = -99;
    m_energyScaleGainDown = -99;
    m_energyScaleEtUp = -99;
    m_energyScaleEtDown = -99;
    m_energySigmaUp = -99;
    m_energySigmaDown = -99;
    m_energySigmaPhiUp = -99;
    m_energySigmaPhiDown = -99;
    m_energySigmaRhoUp = -99;
    m_energySigmaRhoDown = -99;
    */

    m_source_candidates.clear();
  }

  float vertex_x() const{return m_vertex_x;}
  float vertex_y() const{return m_vertex_y;}
  float vertex_z() const{return m_vertex_z;}
  float supercluster_eta() const{return m_supercluster_eta;}
  float supercluster_phi() const{return m_supercluster_phi;}
  //  float trackIso() const{return m_trackIso;}
  float puppiChargedHadronIso() const{return m_puppiChargedHadronIso;}
  float puppiNeutralHadronIso() const{return m_puppiNeutralHadronIso;}
  float puppiPhotonIso() const{return m_puppiPhotonIso;}

  float trackIso() const{return m_trackIso;}
  float ecalIso() const{return m_ecalIso;}
  float hcalIso() const{return m_hcalIso;}
  float caloIso() const{return m_caloIso;}
  float patParticleIso() const{return m_patParticleIso;}
  float chargedHadronIso() const{return m_chargedHadronIso;}
  float neutralHadronIso() const{return m_neutralHadronIso;}
  float photonIso() const{return m_photonIso;}
  float puChargedHadronIso() const{return m_puChargedHadronIso;}

  double minDeltaRToL1EGamma() const{return m_minDeltaRToL1EGamma;}

  //  float get_tag(tag t) const{ return tags.get_tag(static_cast<int>(t));}

  // EGamma Residuals
  /*
  float ecalEnergyPreCorr() const { return m_ecalEnergyPreCorr; }
  float ecalEnergyErrPreCorr() const { return m_ecalEnergyErrPreCorr; }
  float ecalEnergyPostCorr() const { return m_ecalEnergyPostCorr; }
  float ecalEnergyErrPostCorr() const { return m_ecalEnergyErrPostCorr; }
  float ecalTrkEnergyPreCorr() const { return m_ecalTrkEnergyPreCorr; }
  float ecalTrkEnergyErrPreCorr() const { return m_ecalTrkEnergyErrPreCorr; }
  float ecalTrkEnergyPostCorr() const { return m_ecalTrkEnergyPostCorr; }
  float ecalTrkEnergyErrPostCorr() const { return m_ecalTrkEnergyErrPostCorr; }
  float energyScaleValue() const { return m_energyScaleValue; }
  float energySigmaValue() const { return m_energySigmaValue; }
  float energySmearNrSigma() const { return m_energySmearNrSigma; }
  float energyScaleUp() const { return m_energyScaleUp; }
  float energyScaleDown() const { return m_energyScaleDown; }
  float energyScaleStatUp() const { return m_energyScaleStatUp; }
  float energyScaleStatDown() const { return m_energyScaleStatDown; }
  float energyScaleSystUp() const { return m_energyScaleSystUp; }
  float energyScaleSystDown() const { return m_energyScaleSystDown; }
  float energyScaleGainUp() const { return m_energyScaleGainUp; }
  float energyScaleGainDown() const { return m_energyScaleGainDown; }
  float energyScaleEtUp() const { return m_energyScaleEtUp; }
  float energyScaleEtDown() const { return m_energyScaleEtDown; }
  float energySigmaUp() const { return m_energySigmaUp; }
  float energySigmaDown() const { return m_energySigmaDown; }
  float energySigmaPhiUp() const { return m_energySigmaPhiUp; }
  float energySigmaPhiDown() const { return m_energySigmaPhiDown; }
  float energySigmaRhoUp() const { return m_energySigmaRhoUp; }
  float energySigmaRhoDown() const { return m_energySigmaRhoDown; }
  */

  void set_vertex_x(float x){m_vertex_x=x;}
  void set_vertex_y(float x){m_vertex_y=x;}
  void set_vertex_z(float x){m_vertex_z=x;}
  void set_supercluster_eta(float x){m_supercluster_eta=x;}
  void set_supercluster_phi(float x){m_supercluster_phi=x;}

  void set_puppiChargedHadronIso(float x){m_puppiChargedHadronIso=x;}
  void set_puppiNeutralHadronIso(float x){m_puppiNeutralHadronIso=x;}
  void set_puppiPhotonIso(float x){m_puppiPhotonIso=x;}

  void set_trackIso(float x){m_trackIso =x;}
  void set_ecalIso(float x){m_ecalIso =x;}
  void set_hcalIso(float x){m_hcalIso =x;}
  void set_caloIso(float x){m_caloIso =x;}
  void set_patParticleIso(float x){m_patParticleIso =x;}
  void set_chargedHadronIso(float x){m_chargedHadronIso =x;}
  void set_neutralHadronIso(float x){m_neutralHadronIso =x;}
  void set_photonIso(float x){m_photonIso =x;}
  void set_puChargedHadronIso(float x){m_puChargedHadronIso =x;}

  //  void set_tag(tag t, float value){tags.set_tag(static_cast<int>(t), value);}

  void set_minDeltaRToL1EGamma(double x){m_minDeltaRToL1EGamma = x;}

  void set_source_candidates(const std::vector<source_candidate>& vsc){ m_source_candidates = vsc; }
  void add_source_candidate (const source_candidate& sc){ m_source_candidates.push_back(sc); }

  bool  has_tag(tag t) const { return tags.has_tag(static_cast<int>(t)); }
  float get_tag(tag t) const { return tags.get_tag(static_cast<int>(t)); }
  void  set_tag(tag t, float value) { tags.set_tag(static_cast<int>(t), value);}

  // EGamma Residuals
  /*
  void set_ecalEnergyPreCorr(float x) { m_ecalEnergyPreCorr=x; }
  void set_ecalEnergyErrPreCorr(float x) { m_ecalEnergyErrPreCorr=x; }
  void set_ecalEnergyPostCorr(float x) { m_ecalEnergyPostCorr=x; }
  void set_ecalEnergyErrPostCorr(float x) { m_ecalEnergyErrPostCorr=x; }
  void set_ecalTrkEnergyPreCorr(float x) { m_ecalTrkEnergyPreCorr=x; }
  void set_ecalTrkEnergyErrPreCorr(float x) { m_ecalTrkEnergyErrPreCorr=x; }
  void set_ecalTrkEnergyPostCorr(float x) { m_ecalTrkEnergyPostCorr=x; }
  void set_ecalTrkEnergyErrPostCorr(float x) { m_ecalTrkEnergyErrPostCorr=x; }
  void set_energyScaleValue(float x) { m_energyScaleValue=x; }
  void set_energySigmaValue(float x) { m_energySigmaValue=x; }
  void set_energySmearNrSigma(float x) { m_energySmearNrSigma=x; }
  void set_energyScaleUp(float x) { m_energyScaleUp=x; }
  void set_energyScaleDown(float x) { m_energyScaleDown=x; }
  void set_energyScaleStatUp(float x) { m_energyScaleStatUp=x; }
  void set_energyScaleStatDown(float x) { m_energyScaleStatDown=x; }
  void set_energyScaleSystUp(float x) { m_energyScaleSystUp=x; }
  void set_energyScaleSystDown(float x) { m_energyScaleSystDown=x; }
  void set_energyScaleGainUp(float x) { m_energyScaleGainUp=x; }
  void set_energyScaleGainDown(float x) { m_energyScaleGainDown=x; }
  void set_energyScaleEtUp(float x) { m_energyScaleEtUp=x; }
  void set_energyScaleEtDown(float x) { m_energyScaleEtDown=x; }
  void set_energySigmaUp(float x) { m_energySigmaUp=x; }
  void set_energySigmaDown(float x) { m_energySigmaDown=x; }
  void set_energySigmaPhiUp(float x) { m_energySigmaPhiUp=x; }
  void set_energySigmaPhiDown(float x) { m_energySigmaPhiDown=x; }
  void set_energySigmaRhoUp(float x) { m_energySigmaRhoUp=x; }
  void set_energySigmaRhoDown(float x) { m_energySigmaRhoDown=x; }
  */

private:
  float m_vertex_x;
  float m_vertex_y;
  float m_vertex_z;
  float m_supercluster_eta;
  float m_supercluster_phi;
  //  float m_trackIso;

  float m_puppiChargedHadronIso;
  float m_puppiNeutralHadronIso;
  float m_puppiPhotonIso;

  float m_trackIso;
  float m_ecalIso;
  float m_hcalIso;
  float m_caloIso;
  float m_patParticleIso;
  float m_chargedHadronIso;
  float m_neutralHadronIso;
  float m_photonIso;
  float m_puChargedHadronIso;

  double m_minDeltaRToL1EGamma;

  std::vector<source_candidate> m_source_candidates;

  Tags tags;

  // EGamma Residuals
  /*
  float m_ecalEnergyPreCorr;
  float m_ecalEnergyErrPreCorr;
  float m_ecalEnergyPostCorr;
  float m_ecalEnergyErrPostCorr;
  float m_ecalTrkEnergyPreCorr;
  float m_ecalTrkEnergyErrPreCorr;
  float m_ecalTrkEnergyPostCorr;
  float m_ecalTrkEnergyErrPostCorr;
  float m_energyScaleValue;
  float m_energySigmaValue;
  float m_energySmearNrSigma;
  float m_energyScaleUp;
  float m_energyScaleDown;
  float m_energyScaleStatUp;
  float m_energyScaleStatDown;
  float m_energyScaleSystUp;
  float m_energyScaleSystDown;
  float m_energyScaleGainUp;
  float m_energyScaleGainDown;
  float m_energyScaleEtUp;
  float m_energyScaleEtDown;
  float m_energySigmaUp;
  float m_energySigmaDown;
  float m_energySigmaPhiUp;
  float m_energySigmaPhiDown;
  float m_energySigmaRhoUp;
  float m_energySigmaRhoDown;
  */
};
