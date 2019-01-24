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
    m_puppiMultiplicity = 0;
    m_neutralPuppiMultiplicity = 0;
    m_neutralHadronPuppiMultiplicity = 0;
    m_photonPuppiMultiplicity = 0;
    m_HFHadronPuppiMultiplicity = 0;
    m_HFEMPuppiMultiplicity = 0;
    m_btag_combinedSecondaryVertex = 0;
    m_btag_combinedSecondaryVertexMVA = 0;
    m_btag_DeepCSV_probb = 0;
    m_btag_DeepCSV_probbb = 0;
    m_btag_BoostedDoubleSecondaryVertexAK8 = 0;
    m_btag_BoostedDoubleSecondaryVertexCA15 = 0;
    m_btag_DeepFlavour_probbb=-2;
    m_btag_DeepFlavour_probb=-2;
    m_btag_DeepFlavour_problepb=-2;
    m_btag_DeepFlavour_probc=-2;
    m_btag_DeepFlavour_probuds=-2;
    m_btag_DeepFlavour_probg=-2;
    m_btag_MassDecorrelatedDeepBoosted_bbvsLight=-2;
    m_btag_MassDecorrelatedDeepBoosted_ccvsLight=-2;
    m_btag_MassDecorrelatedDeepBoosted_TvsQCD=-2;
    m_btag_MassDecorrelatedDeepBoosted_ZHccvsQCD=-2;
    m_btag_MassDecorrelatedDeepBoosted_WvsQCD=-2;
    m_btag_MassDecorrelatedDeepBoosted_ZHbbvsQCD=-2;
    m_btag_MassDecorrelatedDeepBoosted_probHbb=-2;
    m_btag_MassDecorrelatedDeepBoosted_probQCDc=-2;
    m_btag_MassDecorrelatedDeepBoosted_probQCDbb=-2;
    m_btag_MassDecorrelatedDeepBoosted_probTbqq=-2;
    m_btag_MassDecorrelatedDeepBoosted_probTbcq=-2;
    m_btag_MassDecorrelatedDeepBoosted_probTbq=-2;
    m_btag_MassDecorrelatedDeepBoosted_probQCDothers=-2;
    m_btag_MassDecorrelatedDeepBoosted_probQCDb=-2;
    m_btag_MassDecorrelatedDeepBoosted_probTbc=-2;
    m_btag_MassDecorrelatedDeepBoosted_probWqq=-2;
    m_btag_MassDecorrelatedDeepBoosted_probQCDcc=-2;
    m_btag_MassDecorrelatedDeepBoosted_probHcc=-2;
    m_btag_MassDecorrelatedDeepBoosted_probWcq=-2;
    m_btag_MassDecorrelatedDeepBoosted_probZcc=-2;
    m_btag_MassDecorrelatedDeepBoosted_probZqq=-2;
    m_btag_MassDecorrelatedDeepBoosted_probHqqqq=-2;
    m_btag_MassDecorrelatedDeepBoosted_probZbb=-2;
    m_btag_DeepDoubleB_probH=-2;
    m_btag_DeepDoubleB_probQCD=-2;

    m_JEC_factor_raw = 0;
    m_JEC_L1factor_raw = 0;
    m_genjet_index = 0;
    m_hadronFlavor = 0;
    m_pileupID = -2;

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
  float puppiMultiplicity() const{return m_puppiMultiplicity;}
  float neutralPuppiMultiplicity() const{return m_neutralPuppiMultiplicity;}
  float neutralHadronPuppiMultiplicity() const{return m_neutralHadronPuppiMultiplicity;}
  float photonPuppiMultiplicity() const{return m_photonPuppiMultiplicity;}
  float HFHadronPuppiMultiplicity() const{return m_HFHadronPuppiMultiplicity;}
  float HFEMPuppiMultiplicity() const{return m_HFEMPuppiMultiplicity;}
  float btag_combinedSecondaryVertex() const{return m_btag_combinedSecondaryVertex;} // combinedInclusiveSecondaryVertexV2BJetTags
  float btag_combinedSecondaryVertexMVA() const{return m_btag_combinedSecondaryVertexMVA;}
  float btag_DeepCSV() const{return m_btag_DeepCSV_probb + m_btag_DeepCSV_probbb;} // pfDeepCSVJetTags:probb + pfDeepCSVJetTags:probbb
  float btag_BoostedDoubleSecondaryVertexAK8() const{return m_btag_BoostedDoubleSecondaryVertexAK8;}
  float btag_BoostedDoubleSecondaryVertexCA15() const{return m_btag_BoostedDoubleSecondaryVertexCA15;}
  float btag_DeepFlavour_bb() const{return m_btag_DeepFlavour_probbb;}
  float btag_DeepFlavour_b() const{return m_btag_DeepFlavour_probb;}
  float btag_DeepFlavour_lepb() const{return m_btag_DeepFlavour_problepb;}
  float btag_DeepFlavour_uds() const{return m_btag_DeepFlavour_probuds;}
  float btag_DeepFlavour_g() const{return m_btag_DeepFlavour_probg;}
  float btag_DeepFlavour_c() const{return m_btag_DeepFlavour_probc;}
  float btag_DeepFlavour() const{return m_btag_DeepFlavour_probbb+m_btag_DeepFlavour_probb+m_btag_DeepFlavour_problepb;}
  float btag_MassDecorrelatedDeepBoosted_bbvsLight() const{return m_btag_MassDecorrelatedDeepBoosted_bbvsLight;}
  float btag_MassDecorrelatedDeepBoosted_ccvsLight() const{return m_btag_MassDecorrelatedDeepBoosted_ccvsLight;}
  float btag_MassDecorrelatedDeepBoosted_TvsQCD() const{return m_btag_MassDecorrelatedDeepBoosted_TvsQCD;}
  float btag_MassDecorrelatedDeepBoosted_ZHccvsQCD() const{return m_btag_MassDecorrelatedDeepBoosted_ZHccvsQCD;}
  float btag_MassDecorrelatedDeepBoosted_WvsQCD() const{return m_btag_MassDecorrelatedDeepBoosted_WvsQCD;}
  float btag_MassDecorrelatedDeepBoosted_ZHbbvsQCD() const{return m_btag_MassDecorrelatedDeepBoosted_ZHbbvsQCD;}
  float btag_MassDecorrelatedDeepBoosted_probHbb() const{return m_btag_MassDecorrelatedDeepBoosted_probHbb;}
  float btag_MassDecorrelatedDeepBoosted_probQCDc() const{return m_btag_MassDecorrelatedDeepBoosted_probQCDc;}
  float btag_MassDecorrelatedDeepBoosted_probQCDbb() const{return m_btag_MassDecorrelatedDeepBoosted_probQCDbb;}
  float btag_MassDecorrelatedDeepBoosted_probTbqq() const{return m_btag_MassDecorrelatedDeepBoosted_probTbqq;}
  float btag_MassDecorrelatedDeepBoosted_probTbcq() const{return m_btag_MassDecorrelatedDeepBoosted_probTbcq;}
  float btag_MassDecorrelatedDeepBoosted_probTbq() const{return m_btag_MassDecorrelatedDeepBoosted_probTbq;}
  float btag_MassDecorrelatedDeepBoosted_probQCDothers() const{return m_btag_MassDecorrelatedDeepBoosted_probQCDothers;}
  float btag_MassDecorrelatedDeepBoosted_probQCDb() const{return m_btag_MassDecorrelatedDeepBoosted_probQCDb;}
  float btag_MassDecorrelatedDeepBoosted_probTbc() const{return m_btag_MassDecorrelatedDeepBoosted_probTbc;}
  float btag_MassDecorrelatedDeepBoosted_probWqq() const{return m_btag_MassDecorrelatedDeepBoosted_probWqq;}
  float btag_MassDecorrelatedDeepBoosted_probQCDcc() const{return m_btag_MassDecorrelatedDeepBoosted_probQCDcc;}
  float btag_MassDecorrelatedDeepBoosted_probHcc() const{return m_btag_MassDecorrelatedDeepBoosted_probHcc;}
  float btag_MassDecorrelatedDeepBoosted_probZcc() const{return m_btag_MassDecorrelatedDeepBoosted_probZcc;}
  float btag_MassDecorrelatedDeepBoosted_proWcq() const{return m_btag_MassDecorrelatedDeepBoosted_probWcq;}
  float btag_MassDecorrelatedDeepBoosted_probZqq() const{return m_btag_MassDecorrelatedDeepBoosted_probZqq;}
  float btag_MassDecorrelatedDeepBoosted_probHqqqq() const{return m_btag_MassDecorrelatedDeepBoosted_probHqqqq;}
  float btag_MassDecorrelatedDeepBoosted_probZbb() const{return m_btag_MassDecorrelatedDeepBoosted_probZbb;}
  float btag_DeepDoubleB_probH() const{return m_btag_DeepDoubleB_probH;}
  float btag_DeepDoubleB_probQCD() const{return m_btag_DeepDoubleB_probQCD;}

  float JEC_factor_raw() const{return m_JEC_factor_raw;}
  float JEC_L1factor_raw() const{return m_JEC_L1factor_raw;}
  float get_tag(tag t) const { return tags.get_tag(static_cast<int>(t)); }
  JetBTagInfo btaginfo() const{return m_btaginfo;}
  int hadronFlavor() const { return m_hadronFlavor; }
  float pileupID() const {return m_pileupID;}

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
  void set_puppiMultiplicity(float x){m_puppiMultiplicity=x;}
  void set_neutralPuppiMultiplicity(float x){m_neutralPuppiMultiplicity=x;}
  void set_neutralHadronPuppiMultiplicity(float x){m_neutralHadronPuppiMultiplicity=x;}
  void set_photonPuppiMultiplicity(float x){m_photonPuppiMultiplicity=x;}
  void set_HFHadronPuppiMultiplicity(float x){m_HFHadronPuppiMultiplicity=x;}
  void set_HFEMPuppiMultiplicity(float x){m_HFEMPuppiMultiplicity=x;}
  void set_btag_combinedSecondaryVertex(float x){m_btag_combinedSecondaryVertex=x;} // for 72, this is combinedInclusiveSecondaryVertexV2BJetTags
  void set_btag_combinedSecondaryVertexMVA(float x){m_btag_combinedSecondaryVertexMVA=x;}
  void set_btag_DeepCSV_probb(float x){m_btag_DeepCSV_probb=x;} // pfDeepCSVJetTags:probb
  void set_btag_DeepCSV_probbb(float x){m_btag_DeepCSV_probbb=x;} // pfDeepCSVJetTags:probbb
  void set_btag_BoostedDoubleSecondaryVertexAK8(float x){m_btag_BoostedDoubleSecondaryVertexAK8=x;}
  void set_btag_BoostedDoubleSecondaryVertexCA15(float x){m_btag_BoostedDoubleSecondaryVertexCA15=x;}
  void set_btag_DeepFlavour_probbb(float x){m_btag_DeepFlavour_probbb=x;}
  void set_btag_DeepFlavour_probb(float x){m_btag_DeepFlavour_probb=x;}
  void set_btag_DeepFlavour_problepb(float x){m_btag_DeepFlavour_problepb=x;}
  void set_btag_DeepFlavour_probc(float x){m_btag_DeepFlavour_probc=x;}
  void set_btag_DeepFlavour_probuds(float x){m_btag_DeepFlavour_probuds=x;}
  void set_btag_DeepFlavour_probg(float x){m_btag_DeepFlavour_probg=x;}
  void set_btag_MassDecorrelatedDeepBoosted_bbvsLight(float x){m_btag_MassDecorrelatedDeepBoosted_bbvsLight=x;}
  void set_btag_MassDecorrelatedDeepBoosted_ccvsLight(float x){m_btag_MassDecorrelatedDeepBoosted_ccvsLight=x;}
  void set_btag_MassDecorrelatedDeepBoosted_TvsQCD(float x){m_btag_MassDecorrelatedDeepBoosted_TvsQCD=x;}
  void set_btag_MassDecorrelatedDeepBoosted_ZHccvsQCD(float x){m_btag_MassDecorrelatedDeepBoosted_ZHccvsQCD=x;}
  void set_btag_MassDecorrelatedDeepBoosted_WvsQCD(float x){m_btag_MassDecorrelatedDeepBoosted_WvsQCD=x;}
  void set_btag_MassDecorrelatedDeepBoosted_ZHbbvsQCD(float x){m_btag_MassDecorrelatedDeepBoosted_ZHbbvsQCD=x;}
  void set_btag_MassDecorrelatedDeepBoosted_probHbb(float x){m_btag_MassDecorrelatedDeepBoosted_probHbb=x;}
  void set_btag_MassDecorrelatedDeepBoosted_probQCD(float x) { m_btag_MassDecorrelatedDeepBoosted_probQCDc=x;}
  void set_btag_MassDecorrelatedDeepBoosted_probQCDbb(float x) { m_btag_MassDecorrelatedDeepBoosted_probQCDbb=x;}
  void set_btag_MassDecorrelatedDeepBoosted_probTbqq(float x) { m_btag_MassDecorrelatedDeepBoosted_probTbqq=x;}
  void set_btag_MassDecorrelatedDeepBoosted_probTbcq(float x) { m_btag_MassDecorrelatedDeepBoosted_probTbcq=x;}
  void set_btag_MassDecorrelatedDeepBoosted_probTbq(float x) { m_btag_MassDecorrelatedDeepBoosted_probTbq=x;}
  void set_btag_MassDecorrelatedDeepBoosted_probQCDothers(float x) { m_btag_MassDecorrelatedDeepBoosted_probQCDothers=x;}
  void set_btag_MassDecorrelatedDeepBoosted_probQCDb(float x) { m_btag_MassDecorrelatedDeepBoosted_probQCDb=x;}
  void set_btag_MassDecorrelatedDeepBoosted_probTbc(float x) { m_btag_MassDecorrelatedDeepBoosted_probTbc=x;}
  void set_btag_MassDecorrelatedDeepBoosted_probWqq(float x) { m_btag_MassDecorrelatedDeepBoosted_probWqq=x;}
  void set_btag_MassDecorrelatedDeepBoosted_probQCDcc(float x) { m_btag_MassDecorrelatedDeepBoosted_probQCDcc=x;}
  void set_btag_MassDecorrelatedDeepBoosted_probHcc(float x) { m_btag_MassDecorrelatedDeepBoosted_probHcc=x;}
  void set_btag_MassDecorrelatedDeepBoosted_probZcc(float x) { m_btag_MassDecorrelatedDeepBoosted_probZcc=x;}
  void set_btag_MassDecorrelatedDeepBoosted_probWcq(float x) { m_btag_MassDecorrelatedDeepBoosted_probWcq=x;}
  void set_btag_MassDecorrelatedDeepBoosted_probZqq(float x) { m_btag_MassDecorrelatedDeepBoosted_probZqq=x;}
  void set_btag_MassDecorrelatedDeepBoosted_probHqqqq(float x) { m_btag_MassDecorrelatedDeepBoosted_probHqqqq=x;}
  void set_btag_MassDecorrelatedDeepBoosted_probZbb(float x) { m_btag_MassDecorrelatedDeepBoosted_probZbb=x;}
  void set_btag_DeepDoubleB_probH(float x) { m_btag_DeepDoubleB_probH=x;}
  void set_btag_DeepDoubleB_probQCD(float x) { m_btag_DeepDoubleB_probQCD=x;}


  void set_JEC_factor_raw(float x){m_JEC_factor_raw=x;}
  void set_JEC_L1factor_raw(float x){m_JEC_L1factor_raw=x;}
  void set_genjet_index(int x){m_genjet_index=x;}
  void set_tag(tag t, float value) { return tags.set_tag(static_cast<int>(t), value); }
  void set_btaginfo(JetBTagInfo x){m_btaginfo=x;}
  void set_hadronFlavor(int x){ m_hadronFlavor = x; }

  void set_pileupID(float x){m_pileupID = x;}

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
  float m_puppiMultiplicity;
  float m_neutralPuppiMultiplicity;
  float m_neutralHadronPuppiMultiplicity;
  float m_photonPuppiMultiplicity;
  float m_HFHadronPuppiMultiplicity;
  float m_HFEMPuppiMultiplicity;
  float m_btag_combinedSecondaryVertex;
  float m_btag_combinedSecondaryVertexMVA;
  float m_btag_DeepCSV_probb;
  float m_btag_DeepCSV_probbb;
  float m_btag_BoostedDoubleSecondaryVertexAK8;
  float m_btag_BoostedDoubleSecondaryVertexCA15;
  float m_btag_DeepFlavour_probbb;
  float m_btag_DeepFlavour_probb;
  float m_btag_DeepFlavour_problepb;
  float m_btag_DeepFlavour_probuds;
  float m_btag_DeepFlavour_probc;
  float m_btag_DeepFlavour_probg;
  float m_btag_MassDecorrelatedDeepBoosted_bbvsLight;
  float m_btag_MassDecorrelatedDeepBoosted_ccvsLight;
  float m_btag_MassDecorrelatedDeepBoosted_TvsQCD;
  float m_btag_MassDecorrelatedDeepBoosted_ZHccvsQCD;
  float m_btag_MassDecorrelatedDeepBoosted_WvsQCD;
  float m_btag_MassDecorrelatedDeepBoosted_ZHbbvsQCD;
  float m_btag_MassDecorrelatedDeepBoosted_probHbb;
  float m_btag_MassDecorrelatedDeepBoosted_probQCDc;
  float m_btag_MassDecorrelatedDeepBoosted_probQCDbb;
  float m_btag_MassDecorrelatedDeepBoosted_probTbqq;
  float m_btag_MassDecorrelatedDeepBoosted_probTbcq;
  float m_btag_MassDecorrelatedDeepBoosted_probTbq;
  float m_btag_MassDecorrelatedDeepBoosted_probQCDothers;
  float m_btag_MassDecorrelatedDeepBoosted_probQCDb;
  float m_btag_MassDecorrelatedDeepBoosted_probTbc;
  float m_btag_MassDecorrelatedDeepBoosted_probWqq;
  float m_btag_MassDecorrelatedDeepBoosted_probQCDcc;
  float m_btag_MassDecorrelatedDeepBoosted_probHcc;
  float m_btag_MassDecorrelatedDeepBoosted_probWcq;
  float m_btag_MassDecorrelatedDeepBoosted_probZcc;
  float m_btag_MassDecorrelatedDeepBoosted_probZqq;
  float m_btag_MassDecorrelatedDeepBoosted_probHqqqq;
  float m_btag_MassDecorrelatedDeepBoosted_probZbb;
  float m_btag_DeepDoubleB_probH;
  float m_btag_DeepDoubleB_probQCD;


  float m_JEC_factor_raw;
  float m_JEC_L1factor_raw;
  int m_genjet_index;
  int m_hadronFlavor;

  float m_pileupID;

  JetBTagInfo m_btaginfo;

  std::vector<long int> m_lepton_keys;

  Tags tags;
};
