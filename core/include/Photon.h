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
    mvaPhoID_Fall17_iso_V2_wp80
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


  //  float get_tag(tag t) const{ return tags.get_tag(static_cast<int>(t));}

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

  void set_source_candidates(const std::vector<source_candidate>& vsc){ m_source_candidates = vsc; }
  void add_source_candidate (const source_candidate& sc){ m_source_candidates.push_back(sc); }

  bool  has_tag(tag t) const { return tags.has_tag(static_cast<int>(t)); }
  float get_tag(tag t) const { return tags.get_tag(static_cast<int>(t)); }
  void  set_tag(tag t, float value) { tags.set_tag(static_cast<int>(t), value);}

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

  std::vector<source_candidate> m_source_candidates;

  Tags tags;
};
