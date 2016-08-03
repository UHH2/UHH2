#pragma once

#include "FlavorParticle.h"
#include "Tags.h"
#include "JetBTagInfo.h"

#include <vector>

class Jet : public FlavorParticle {

 public:
  enum tag { /* for future use (more b-taggers, etc.) */ };

  Jet(){

    m_jetArea = 0;
    m_numberOfDaughters = 0;
    m_neutralEmEnergyFraction = 0;
    m_neutralHadronEnergyFraction = 0;
    m_chargedEmEnergyFraction = 0;
    m_chargedHadronEnergyFraction = 0;
    m_muonEnergyFraction = 0;
    m_photonEnergyFraction = 0;
    m_chargedMultiplicity = 0;
    m_neutralMultiplicity = 0;
    m_muonMultiplicity = 0;
    m_electronMultiplicity = 0;
    m_photonMultiplicity = 0;
    m_btag_simpleSecondaryVertexHighEff = 0;
    m_btag_combinedSecondaryVertex = 0;
    m_btag_combinedSecondaryVertexMVA = 0;
    m_btag_jetBProbability = 0;
    m_btag_jetProbability = 0;
    m_btag_BoostedDoubleSecondaryVertexAK8 = 0;
    m_btag_BoostedDoubleSecondaryVertexCA15 = 0;
    m_JEC_factor_raw = 0;
    m_JEC_L1factor_raw = 0;
    m_genjet_index = 0;
    m_hadronFlavor = 0;

    m_lepton_keys.clear();
  }

  float jetArea() const{return m_jetArea;}
  int numberOfDaughters() const{return m_numberOfDaughters;} 
  float neutralEmEnergyFraction() const{return m_neutralEmEnergyFraction;}
  float neutralHadronEnergyFraction() const{return m_neutralHadronEnergyFraction;}
  float chargedEmEnergyFraction() const{return m_chargedEmEnergyFraction;}
  float chargedHadronEnergyFraction() const{return m_chargedHadronEnergyFraction;}
  float muonEnergyFraction() const{return m_muonEnergyFraction;}
  float photonEnergyFraction() const{return m_photonEnergyFraction;}
  int chargedMultiplicity() const{return m_chargedMultiplicity;}
  int neutralMultiplicity() const{return m_neutralMultiplicity;}
  int muonMultiplicity() const{return m_muonMultiplicity;} 
  int electronMultiplicity() const{return m_electronMultiplicity;}
  int photonMultiplicity() const{return m_photonMultiplicity;}
  float btag_simpleSecondaryVertexHighEff() const{return m_btag_simpleSecondaryVertexHighEff;}
  float btag_combinedSecondaryVertex() const{return m_btag_combinedSecondaryVertex;} // combinedInclusiveSecondaryVertexV2BJetTags
  float btag_combinedSecondaryVertexMVA() const{return m_btag_combinedSecondaryVertexMVA;}
  float btag_jetBProbability() const{return m_btag_jetBProbability;}
  float btag_jetProbability() const{return m_btag_jetProbability;}
  float btag_BoostedDoubleSecondaryVertexAK8() const{return m_btag_BoostedDoubleSecondaryVertexAK8;}
  float btag_BoostedDoubleSecondaryVertexCA15() const{return m_btag_BoostedDoubleSecondaryVertexCA15;}
  float JEC_factor_raw() const{return m_JEC_factor_raw;}
  float JEC_L1factor_raw() const{return m_JEC_L1factor_raw;}
  float get_tag(tag t) const { return tags.get_tag(static_cast<int>(t)); }
  JetBTagInfo btaginfo() const{return m_btaginfo;}
  int hadronFlavor() const { return m_hadronFlavor; }

  const std::vector<long int>& lepton_keys() const { return m_lepton_keys; }

  void set_jetArea(float x){m_jetArea=x;}
  void set_numberOfDaughters(int x){m_numberOfDaughters=x;} 
  void set_neutralEmEnergyFraction(float x){m_neutralEmEnergyFraction=x;}
  void set_neutralHadronEnergyFraction(float x){m_neutralHadronEnergyFraction=x;}
  void set_chargedEmEnergyFraction(float x){m_chargedEmEnergyFraction=x;}
  void set_chargedHadronEnergyFraction(float x){m_chargedHadronEnergyFraction=x;}
  void set_muonEnergyFraction(float x){m_muonEnergyFraction=x;}
  void set_photonEnergyFraction(float x){m_photonEnergyFraction=x;}
  void set_chargedMultiplicity(int x){m_chargedMultiplicity=x;}
  void set_neutralMultiplicity(int x){m_neutralMultiplicity=x;}
  void set_muonMultiplicity(int x){m_muonMultiplicity=x;} 
  void set_electronMultiplicity(int x){m_electronMultiplicity=x;}
  void set_photonMultiplicity(int x){m_photonMultiplicity=x;}
  void set_btag_simpleSecondaryVertexHighEff(float x){m_btag_simpleSecondaryVertexHighEff=x;}
  void set_btag_combinedSecondaryVertex(float x){m_btag_combinedSecondaryVertex=x;} // for 72, this is combinedInclusiveSecondaryVertexV2BJetTags
  void set_btag_combinedSecondaryVertexMVA(float x){m_btag_combinedSecondaryVertexMVA=x;}
  void set_btag_jetBProbability(float x){m_btag_jetBProbability=x;}
  void set_btag_jetProbability(float x){m_btag_jetProbability=x;}
  void set_btag_BoostedDoubleSecondaryVertexAK8(float x){m_btag_BoostedDoubleSecondaryVertexAK8=x;}
  void set_btag_BoostedDoubleSecondaryVertexCA15(float x){m_btag_BoostedDoubleSecondaryVertexCA15=x;}
  void set_JEC_factor_raw(float x){m_JEC_factor_raw=x;}
  void set_JEC_L1factor_raw(float x){m_JEC_L1factor_raw=x;}
  void set_genjet_index(int x){m_genjet_index=x;}
  void set_tag(tag t, float value) { return tags.set_tag(static_cast<int>(t), value); }
  void set_btaginfo(JetBTagInfo x){m_btaginfo=x;}
  void set_hadronFlavor(int x){ m_hadronFlavor = x; }

  void set_lepton_keys(const std::vector<long int>& vlk){ m_lepton_keys = vlk; }
  void add_lepton_key (const long int k){ m_lepton_keys.push_back(k); }

 private:
  float m_jetArea;
  int m_numberOfDaughters;
  float m_neutralEmEnergyFraction;
  float m_neutralHadronEnergyFraction;
  float m_chargedEmEnergyFraction;
  float m_chargedHadronEnergyFraction;
  float m_muonEnergyFraction;
  float m_photonEnergyFraction;
  int m_chargedMultiplicity;
  int m_neutralMultiplicity;
  int m_muonMultiplicity;
  int m_electronMultiplicity;
  int m_photonMultiplicity;
  float m_btag_simpleSecondaryVertexHighEff;
  float m_btag_combinedSecondaryVertex;
  float m_btag_combinedSecondaryVertexMVA;
  float m_btag_jetBProbability;
  float m_btag_jetProbability;
  float m_btag_BoostedDoubleSecondaryVertexAK8;
  float m_btag_BoostedDoubleSecondaryVertexCA15;
  float m_JEC_factor_raw;
  float m_JEC_L1factor_raw;
  int m_genjet_index;
  int m_hadronFlavor;

  JetBTagInfo m_btaginfo;

  std::vector<long int> m_lepton_keys;

  Tags tags;
};
