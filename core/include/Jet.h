#pragma once

#include "FlavorParticle.h"
#include "Tags.h"
#include "JetBTagInfo.h"

#include <vector>

class Jet : public FlavorParticle {

 public:
  enum tag {
    // Pileup ID working points
    pileup_loose,
    pileup_medium,
    pileup_tight
  };

  static tag tagname2tag(const std::string & tagname){
    if(tagname == "pileup_loose") return pileup_loose;
    if(tagname == "pileup_medium") return pileup_medium;
    if(tagname == "pileup_tight") return pileup_tight;
    throw std::runtime_error("unknown Jet::tag '" + tagname + "'");
  }

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
    // Discriminants should have a initial value of something clear "wrong"
    // -2 is safe as some discrimintants start at -1, some start at 0
    m_btag_combinedSecondaryVertex = -2;
    m_btag_combinedSecondaryVertexMVA = -2;
    m_btag_DeepCSV_probb = -2;
    m_btag_DeepCSV_probbb = -2;
    m_btag_BoostedDoubleSecondaryVertexAK8 = -2;
    m_btag_BoostedDoubleSecondaryVertexCA15 = -2;
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
    m_btag_MassDecorrelatedDeepBoosted_ZvsQCD=-2;
    m_btag_MassDecorrelatedDeepBoosted_ZbbvsQCD=-2;
    m_btag_MassDecorrelatedDeepBoosted_HbbvsQCD=-2;
    m_btag_MassDecorrelatedDeepBoosted_H4qvsQCD=-2;
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
 
    m_btag_DeepDoubleBvLJet_probHbb=-2;
    m_btag_DeepDoubleBvLJet_probQCD=-2;
    m_btag_DeepDoubleCvBJet_probHbb=-2;
    m_btag_DeepDoubleCvBJet_probHcc=-2;
    m_btag_DeepDoubleCvLJet_probHcc=-2;
    m_btag_DeepDoubleCvLJet_probQCD=-2;

    m_btag_MassIndependentDeepDoubleBvLJet_probHbb=-2;
    m_btag_MassIndependentDeepDoubleBvLJet_probQCD=-2;
    m_btag_MassIndependentDeepDoubleCvBJet_probHbb=-2;
    m_btag_MassIndependentDeepDoubleCvBJet_probHcc=-2;
    m_btag_MassIndependentDeepDoubleCvLJet_probHcc=-2;
    m_btag_MassIndependentDeepDoubleCvLJet_probQCD=-2;

    m_btag_DeepBoosted_TvsQCD=-2;
    m_btag_DeepBoosted_WvsQCD=-2;
    m_btag_DeepBoosted_ZvsQCD=-2;
    m_btag_DeepBoosted_ZbbvsQCD=-2;
    m_btag_DeepBoosted_HbbvsQCD=-2;
    m_btag_DeepBoosted_H4qvsQCD=-2;
    m_btag_DeepBoosted_probHbb=-2;
    m_btag_DeepBoosted_probQCDc=-2;
    m_btag_DeepBoosted_probQCDbb=-2;
    m_btag_DeepBoosted_probTbqq=-2;
    m_btag_DeepBoosted_probTbcq=-2;
    m_btag_DeepBoosted_probTbq=-2;
    m_btag_DeepBoosted_probQCDothers=-2;
    m_btag_DeepBoosted_probQCDb=-2;
    m_btag_DeepBoosted_probTbc=-2;
    m_btag_DeepBoosted_probWqq=-2;
    m_btag_DeepBoosted_probQCDcc=-2;
    m_btag_DeepBoosted_probHcc=-2;
    m_btag_DeepBoosted_probWcq=-2;
    m_btag_DeepBoosted_probZcc=-2;
    m_btag_DeepBoosted_probZqq=-2;
    m_btag_DeepBoosted_probHqqqq=-2;
    m_btag_DeepBoosted_probZbb=-2;

    m_JEC_factor_raw = 0;
    m_JEC_L1factor_raw = 0;
    m_genjet_index = -1; // not default of 0, as 0 is a valid index
    m_pileupID = -2;

    m_lepton_keys.clear();
    m_pfcand_indexs.clear();
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

  float btag_MassDecorrelatedDeepBoosted_ZvsQCD() const{return m_btag_MassDecorrelatedDeepBoosted_ZvsQCD;}
  float btag_MassDecorrelatedDeepBoosted_ZbbvsQCD() const{return m_btag_MassDecorrelatedDeepBoosted_ZbbvsQCD;}
  float btag_MassDecorrelatedDeepBoosted_HbbvsQCD() const{return m_btag_MassDecorrelatedDeepBoosted_HbbvsQCD;}
  float btag_MassDecorrelatedDeepBoosted_H4qvsQCD() const{return m_btag_MassDecorrelatedDeepBoosted_H4qvsQCD;}


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

  float btag_DeepDoubleBvLJet_probHbb() const{return m_btag_DeepDoubleBvLJet_probHbb;}
  float btag_DeepDoubleBvLJet_probQCD() const{return m_btag_DeepDoubleBvLJet_probQCD;}
  float btag_DeepDoubleCvBJet_probHbb() const{return m_btag_DeepDoubleCvBJet_probHbb;}
  float btag_DeepDoubleCvBJet_probHcc() const{return m_btag_DeepDoubleCvBJet_probHcc;}
  float btag_DeepDoubleCvLJet_probHcc() const{return m_btag_DeepDoubleCvLJet_probHcc;}
  float btag_DeepDoubleCvLJet_probQCD() const{return m_btag_DeepDoubleCvLJet_probQCD;}

  float btag_MassIndependentDeepDoubleBvLJet_probHbb() const{return m_btag_MassIndependentDeepDoubleBvLJet_probHbb;}
  float btag_MassIndependentDeepDoubleBvLJet_probQCD() const{return m_btag_MassIndependentDeepDoubleBvLJet_probQCD;}
  float btag_MassIndependentDeepDoubleCvBJet_probHbb() const{return m_btag_MassIndependentDeepDoubleCvBJet_probHbb;}
  float btag_MassIndependentDeepDoubleCvBJet_probHcc() const{return m_btag_MassIndependentDeepDoubleCvBJet_probHcc;}
  float btag_MassIndependentDeepDoubleCvLJet_probHcc() const{return m_btag_MassIndependentDeepDoubleCvLJet_probHcc;}
  float btag_MassIndependentDeepDoubleCvLJet_probQCD() const{return m_btag_MassIndependentDeepDoubleCvLJet_probQCD;}


  float btag_DeepBoosted_TvsQCD() const{return m_btag_DeepBoosted_TvsQCD;}
  float btag_DeepBoosted_WvsQCD() const{return m_btag_DeepBoosted_WvsQCD;}
  float btag_DeepBoosted_ZvsQCD() const{return m_btag_DeepBoosted_ZvsQCD;}
  float btag_DeepBoosted_ZbbvsQCD() const{return m_btag_DeepBoosted_ZbbvsQCD;}
  float btag_DeepBoosted_HbbvsQCD() const{return m_btag_DeepBoosted_HbbvsQCD;}
  float btag_DeepBoosted_H4qvsQCD() const{return m_btag_DeepBoosted_H4qvsQCD;}
  float btag_DeepBoosted_probHbb() const{return m_btag_DeepBoosted_probHbb;}
  float btag_DeepBoosted_probQCDc() const{return m_btag_DeepBoosted_probQCDc;}
  float btag_DeepBoosted_probQCDbb() const{return m_btag_DeepBoosted_probQCDbb;}
  float btag_DeepBoosted_probTbqq() const{return m_btag_DeepBoosted_probTbqq;}
  float btag_DeepBoosted_probTbcq() const{return m_btag_DeepBoosted_probTbcq;}
  float btag_DeepBoosted_probTbq() const{return m_btag_DeepBoosted_probTbq;}
  float btag_DeepBoosted_probQCDothers() const{return m_btag_DeepBoosted_probQCDothers;}
  float btag_DeepBoosted_probQCDb() const{return m_btag_DeepBoosted_probQCDb;}
  float btag_DeepBoosted_probTbc() const{return m_btag_DeepBoosted_probTbc;}
  float btag_DeepBoosted_probWqq() const{return m_btag_DeepBoosted_probWqq;}
  float btag_DeepBoosted_probQCDcc() const{return m_btag_DeepBoosted_probQCDcc;}
  float btag_DeepBoosted_probHcc() const{return m_btag_DeepBoosted_probHcc;}
  float btag_DeepBoosted_probZcc() const{return m_btag_DeepBoosted_probZcc;}
  float btag_DeepBoosted_proWcq() const{return m_btag_DeepBoosted_probWcq;}
  float btag_DeepBoosted_probZqq() const{return m_btag_DeepBoosted_probZqq;}
  float btag_DeepBoosted_probHqqqq() const{return m_btag_DeepBoosted_probHqqqq;}
  float btag_DeepBoosted_probZbb() const{return m_btag_DeepBoosted_probZbb;}

  float JEC_factor_raw() const{return m_JEC_factor_raw;}
  float JEC_L1factor_raw() const{return m_JEC_L1factor_raw;}
  float genjet_index() const{return m_genjet_index;}
  float get_tag(tag t) const { return tags.get_tag(static_cast<int>(t)); }
  float has_tag(tag t) const { return tags.has_tag(static_cast<int>(t)); }
  JetBTagInfo btaginfo() const{return m_btaginfo;}
  float pileupID() const {return m_pileupID;}

  const std::vector<long int>& lepton_keys() const { return m_lepton_keys; }
  const std::vector<long int>& pfcand_indexs() const { return m_pfcand_indexs; }

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
  void set_btag_MassDecorrelatedDeepBoosted_ZvsQCD(float x) {m_btag_MassDecorrelatedDeepBoosted_ZvsQCD=x;}
  void set_btag_MassDecorrelatedDeepBoosted_ZbbvsQCD(float x) {m_btag_MassDecorrelatedDeepBoosted_ZbbvsQCD=x;}
  void set_btag_MassDecorrelatedDeepBoosted_HbbvsQCD(float x) {m_btag_MassDecorrelatedDeepBoosted_HbbvsQCD=x;}
  void set_btag_MassDecorrelatedDeepBoosted_H4qvsQCD(float x) {m_btag_MassDecorrelatedDeepBoosted_H4qvsQCD=x;}

  void set_btag_MassDecorrelatedDeepBoosted_probHbb(float x){m_btag_MassDecorrelatedDeepBoosted_probHbb=x;}
  void set_btag_MassDecorrelatedDeepBoosted_probQCDc(float x) { m_btag_MassDecorrelatedDeepBoosted_probQCDc=x;}
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

  void set_btag_DeepDoubleBvLJet_probHbb(float x) {  m_btag_DeepDoubleBvLJet_probHbb=x;}
  void set_btag_DeepDoubleBvLJet_probQCD(float x) {  m_btag_DeepDoubleBvLJet_probQCD=x;}
  void set_btag_DeepDoubleCvBJet_probHbb(float x) {  m_btag_DeepDoubleCvBJet_probHbb=x;}
  void set_btag_DeepDoubleCvBJet_probHcc(float x) {  m_btag_DeepDoubleCvBJet_probHcc=x;}
  void set_btag_DeepDoubleCvLJet_probHcc(float x) {  m_btag_DeepDoubleCvLJet_probHcc=x;}
  void set_btag_DeepDoubleCvLJet_probQCD(float x) {  m_btag_DeepDoubleCvLJet_probQCD=x;}

  void set_btag_MassIndependentDeepDoubleBvLJet_probHbb(float x) {  m_btag_MassIndependentDeepDoubleBvLJet_probHbb=x;}
  void set_btag_MassIndependentDeepDoubleBvLJet_probQCD(float x) {  m_btag_MassIndependentDeepDoubleBvLJet_probQCD=x;}
  void set_btag_MassIndependentDeepDoubleCvBJet_probHbb(float x) {  m_btag_MassIndependentDeepDoubleCvBJet_probHbb=x;}
  void set_btag_MassIndependentDeepDoubleCvBJet_probHcc(float x) {  m_btag_MassIndependentDeepDoubleCvBJet_probHcc=x;}
  void set_btag_MassIndependentDeepDoubleCvLJet_probHcc(float x) {  m_btag_MassIndependentDeepDoubleCvLJet_probHcc=x;}
  void set_btag_MassIndependentDeepDoubleCvLJet_probQCD(float x) {  m_btag_MassIndependentDeepDoubleCvLJet_probQCD=x;}

  void set_btag_DeepBoosted_TvsQCD(float x){m_btag_DeepBoosted_TvsQCD=x;}
  void set_btag_DeepBoosted_WvsQCD(float x){m_btag_DeepBoosted_WvsQCD=x;}
  void set_btag_DeepBoosted_ZvsQCD(float x) {m_btag_DeepBoosted_ZvsQCD=x;}
  void set_btag_DeepBoosted_ZbbvsQCD(float x) {m_btag_DeepBoosted_ZbbvsQCD=x;}
  void set_btag_DeepBoosted_HbbvsQCD(float x) {m_btag_DeepBoosted_HbbvsQCD=x;}
  void set_btag_DeepBoosted_H4qvsQCD(float x) {m_btag_DeepBoosted_H4qvsQCD=x;}

  void set_btag_DeepBoosted_probHbb(float x){m_btag_DeepBoosted_probHbb=x;}
  void set_btag_DeepBoosted_probQCDc(float x) { m_btag_DeepBoosted_probQCDc=x;}
  void set_btag_DeepBoosted_probQCDbb(float x) { m_btag_DeepBoosted_probQCDbb=x;}
  void set_btag_DeepBoosted_probTbqq(float x) { m_btag_DeepBoosted_probTbqq=x;}
  void set_btag_DeepBoosted_probTbcq(float x) { m_btag_DeepBoosted_probTbcq=x;}
  void set_btag_DeepBoosted_probTbq(float x) { m_btag_DeepBoosted_probTbq=x;}
  void set_btag_DeepBoosted_probQCDothers(float x) { m_btag_DeepBoosted_probQCDothers=x;}
  void set_btag_DeepBoosted_probQCDb(float x) { m_btag_DeepBoosted_probQCDb=x;}
  void set_btag_DeepBoosted_probTbc(float x) { m_btag_DeepBoosted_probTbc=x;}
  void set_btag_DeepBoosted_probWqq(float x) { m_btag_DeepBoosted_probWqq=x;}
  void set_btag_DeepBoosted_probQCDcc(float x) { m_btag_DeepBoosted_probQCDcc=x;}
  void set_btag_DeepBoosted_probHcc(float x) { m_btag_DeepBoosted_probHcc=x;}
  void set_btag_DeepBoosted_probZcc(float x) { m_btag_DeepBoosted_probZcc=x;}
  void set_btag_DeepBoosted_probWcq(float x) { m_btag_DeepBoosted_probWcq=x;}
  void set_btag_DeepBoosted_probZqq(float x) { m_btag_DeepBoosted_probZqq=x;}
  void set_btag_DeepBoosted_probHqqqq(float x) { m_btag_DeepBoosted_probHqqqq=x;}
  void set_btag_DeepBoosted_probZbb(float x) { m_btag_DeepBoosted_probZbb=x;}

  void set_JEC_factor_raw(float x){m_JEC_factor_raw=x;}
  void set_JEC_L1factor_raw(float x){m_JEC_L1factor_raw=x;}
  void set_genjet_index(int x){m_genjet_index=x;}
  void set_tag(tag t, float value) { return tags.set_tag(static_cast<int>(t), value); }
  void set_btaginfo(JetBTagInfo x){m_btaginfo=x;}

  void set_pileupID(float x){m_pileupID = x;}

  void set_lepton_keys(const std::vector<long int>& vlk){ m_lepton_keys = vlk; }
  void add_lepton_key (const long int k){ m_lepton_keys.push_back(k); }

  void set_pfcand_indexs(const std::vector<long int>& vlk){ m_pfcand_indexs = vlk; }
  void add_pfcand_index (const long int k){ m_pfcand_indexs.push_back(k); }

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

  float m_btag_MassDecorrelatedDeepBoosted_ZvsQCD;
  float m_btag_MassDecorrelatedDeepBoosted_ZbbvsQCD;
  float m_btag_MassDecorrelatedDeepBoosted_HbbvsQCD;
  float m_btag_MassDecorrelatedDeepBoosted_H4qvsQCD;

  float m_btag_DeepBoosted_ZvsQCD;
  float m_btag_DeepBoosted_ZbbvsQCD;
  float m_btag_DeepBoosted_HbbvsQCD;
  float m_btag_DeepBoosted_H4qvsQCD;



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

  float m_btag_DeepDoubleBvLJet_probHbb;
  float m_btag_DeepDoubleBvLJet_probQCD;
  float m_btag_DeepDoubleCvBJet_probHbb;
  float m_btag_DeepDoubleCvBJet_probHcc;
  float m_btag_DeepDoubleCvLJet_probHcc;
  float m_btag_DeepDoubleCvLJet_probQCD;

  float m_btag_MassIndependentDeepDoubleBvLJet_probHbb;
  float m_btag_MassIndependentDeepDoubleBvLJet_probQCD;
  float m_btag_MassIndependentDeepDoubleCvBJet_probHbb;
  float m_btag_MassIndependentDeepDoubleCvBJet_probHcc;
  float m_btag_MassIndependentDeepDoubleCvLJet_probHcc;
  float m_btag_MassIndependentDeepDoubleCvLJet_probQCD;

  float m_btag_DeepBoosted_TvsQCD;
  float m_btag_DeepBoosted_WvsQCD;
  float m_btag_DeepBoosted_probHbb;
  float m_btag_DeepBoosted_probQCDc;
  float m_btag_DeepBoosted_probQCDbb;
  float m_btag_DeepBoosted_probTbqq;
  float m_btag_DeepBoosted_probTbcq;
  float m_btag_DeepBoosted_probTbq;
  float m_btag_DeepBoosted_probQCDothers;
  float m_btag_DeepBoosted_probQCDb;
  float m_btag_DeepBoosted_probTbc;
  float m_btag_DeepBoosted_probWqq;
  float m_btag_DeepBoosted_probQCDcc;
  float m_btag_DeepBoosted_probHcc;
  float m_btag_DeepBoosted_probWcq;
  float m_btag_DeepBoosted_probZcc;
  float m_btag_DeepBoosted_probZqq;
  float m_btag_DeepBoosted_probHqqqq;
  float m_btag_DeepBoosted_probZbb;


  float m_JEC_factor_raw;
  float m_JEC_L1factor_raw;
  int m_genjet_index;

  float m_pileupID;

  JetBTagInfo m_btaginfo;

  std::vector<long int> m_lepton_keys;
  std::vector<long int> m_pfcand_indexs;
  Tags tags;
};
