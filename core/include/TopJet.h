#pragma once

#include "Jet.h"

#include "Tags.h"

class TopJet: public Jet{

public:
   enum tag {
      mass,
      fRec,
      Ropt,
      massRatioPassed,
      RoptCalc,
      ptForRoptCalc,
      z_ratio,
      trackSipdSig_3,
      trackSipdSig_2,
      trackSipdSig_1,
      trackSipdSig_0,
      trackSipdSig_1_0,
      trackSipdSig_0_0,
      trackSipdSig_1_1,
      trackSipdSig_0_1,
      trackSip2dSigAboveCharm_0,
      trackSip2dSigAboveBottom_0,
      trackSip2dSigAboveBottom_1,
      tau1_trackEtaRel_0,
      tau1_trackEtaRel_1,
      tau1_trackEtaRel_2,
      tau0_trackEtaRel_0,
      tau0_trackEtaRel_1,
      tau0_trackEtaRel_2,
      tau_vertexMass_0,
      tau_vertexEnergyRatio_0,
      tau_vertexDeltaR_0,
      tau_flightDistance2dSig_0,
      tau_vertexMass_1,
      tau_vertexEnergyRatio_1,
      tau_flightDistance2dSig_1,
      jetNTracks,
      nSV
    };
   
   static tag tagname2tag(const std::string & tagname){
      if(tagname == "mass") return mass;
      if(tagname == "fRec") return fRec;
      if(tagname == "Ropt") return Ropt;
      if(tagname == "RoptCalc") return RoptCalc;
      if(tagname == "ptForRoptCalc") return ptForRoptCalc;
      if(tagname == "massRatioPassed") return massRatioPassed;
      if(tagname == "z_ratio") return z_ratio;
      if(tagname == "trackSipdSig_3") return trackSipdSig_3;
      if(tagname == "trackSipdSig_2") return trackSipdSig_2;
      if(tagname == "trackSipdSig_1") return trackSipdSig_1;
      if(tagname == "trackSipdSig_0") return trackSipdSig_0;
      if(tagname == "trackSipdSig_1_0") return trackSipdSig_1_0;
      if(tagname == "trackSipdSig_0_0") return trackSipdSig_0_0;
      if(tagname == "trackSipdSig_1_1") return trackSipdSig_1_1;
      if(tagname == "trackSipdSig_0_1") return trackSipdSig_0_1;
      if(tagname == "trackSip2dSigAboveCharm_0") return trackSip2dSigAboveCharm_0;
      if(tagname == "trackSip2dSigAboveBottom_0") return trackSip2dSigAboveBottom_0;
      if(tagname == "trackSip2dSigAboveBottom_1") return trackSip2dSigAboveBottom_1;
      if(tagname == "tau1_trackEtaRel_0") return tau1_trackEtaRel_0;
      if(tagname == "tau1_trackEtaRel_1") return tau1_trackEtaRel_1;
      if(tagname == "tau1_trackEtaRel_2") return tau1_trackEtaRel_2;
      if(tagname == "tau0_trackEtaRel_0") return tau0_trackEtaRel_0;
      if(tagname == "tau0_trackEtaRel_1") return tau0_trackEtaRel_1;
      if(tagname == "tau0_trackEtaRel_2") return tau0_trackEtaRel_2;
      if(tagname == "tau_vertexMass_0") return tau_vertexMass_0;
      if(tagname == "tau_vertexEnergyRatio_0") return tau_vertexEnergyRatio_0;
      if(tagname == "tau_vertexDeltaR_0") return tau_vertexDeltaR_0;
      if(tagname == "tau_flightDistance2dSig_0") return tau_flightDistance2dSig_0;
      if(tagname == "tau_vertexMass_1") return tau_vertexMass_1;
      if(tagname == "tau_vertexEnergyRatio_1") return tau_vertexEnergyRatio_1;
      if(tagname == "tau_flightDistance2dSig_1") return tau_flightDistance2dSig_1;
      if(tagname == "jetNTracks") return jetNTracks;
      if(tagname == "nSV") return nSV;
      throw std::runtime_error("unknown tag '" + tagname + "'");
   }

  TopJet(){
      m_qjets_volatility = m_tau1 = m_tau2 = m_tau3 = m_tau4 = m_softdropmass = m_tau1_groomed = m_tau2_groomed = m_tau3_groomed = m_tau4_groomed = m_ecfN2_beta1 = m_ecfN2_beta2 = m_ecfN3_beta1 = m_ecfN3_beta2 = -1.0f;
      m_mvahiggsdiscr = -2;
      m_btag_BoostedDoubleSecondaryVertexAK8 = -2;
      m_btag_BoostedDoubleSecondaryVertexCA15 = -2;
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

  }

  // getters
  float qjets_volatility() const {return m_qjets_volatility;}
  float tau1() const {return m_tau1;}
  float tau2() const {return m_tau2;}
  float tau3() const {return m_tau3;}
  float tau4() const {return m_tau4;}
  float tau1_groomed() const {return m_tau1_groomed;}
  float tau2_groomed() const {return m_tau2_groomed;}
  float tau3_groomed() const {return m_tau3_groomed;}
  float tau4_groomed() const {return m_tau4_groomed;}
  
  // energy correlation functions, N2 & N3, each with beta=1 or beta=2
  // calculated on softdrop jets
  float ecfN2_beta1() const {return m_ecfN2_beta1;}
  float ecfN2_beta2() const {return m_ecfN2_beta2;}
  float ecfN3_beta1() const {return m_ecfN3_beta1;}
  float ecfN3_beta2() const {return m_ecfN3_beta2;}

  float mvahiggsdiscr() const {return m_mvahiggsdiscr;}

  float softdropmass() const {return m_softdropmass;}

  const std::vector<Jet> & subjets() const{return m_subjets;}
  
  float get_tag(tag t) const{ return tags.get_tag(static_cast<int>(t)); }
  float has_tag(tag t) const{ return tags.has_tag(static_cast<int>(t)); }

  //b-taggers for fat jets
  float btag_BoostedDoubleSecondaryVertexAK8() const{return m_btag_BoostedDoubleSecondaryVertexAK8;}
  float btag_BoostedDoubleSecondaryVertexCA15() const{return m_btag_BoostedDoubleSecondaryVertexCA15;}
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

  //binary scores, see https://gitlab.cern.ch/DeepAK8/NNKit/tree/for94X
  float btag_DeepBoosted_TvsQCD() const{return m_btag_DeepBoosted_TvsQCD;}
  float btag_DeepBoosted_WvsQCD() const{return m_btag_DeepBoosted_WvsQCD;}
  float btag_DeepBoosted_ZvsQCD() const{return m_btag_DeepBoosted_ZvsQCD;}
  float btag_DeepBoosted_ZbbvsQCD() const{return m_btag_DeepBoosted_ZbbvsQCD;}
  float btag_DeepBoosted_ZHbbvsQCD() const{return (m_btag_DeepBoosted_probZbb+m_btag_DeepBoosted_probHbb)/(m_btag_DeepBoosted_probZbb+m_btag_DeepBoosted_probHbb+btag_DeepBoosted_raw_score_qcd());}
  float btag_DeepBoosted_HbbvsQCD() const{return m_btag_DeepBoosted_HbbvsQCD;}
  float btag_DeepBoosted_H4qvsQCD() const{return m_btag_DeepBoosted_H4qvsQCD;}

  float btag_MassDecorrelatedDeepBoosted_TvsQCD() const{return m_btag_MassDecorrelatedDeepBoosted_TvsQCD;}
  float btag_MassDecorrelatedDeepBoosted_ZHccvsQCD() const{return m_btag_MassDecorrelatedDeepBoosted_ZHccvsQCD;}
  float btag_MassDecorrelatedDeepBoosted_WvsQCD() const{return m_btag_MassDecorrelatedDeepBoosted_WvsQCD;}
  float btag_MassDecorrelatedDeepBoosted_ZHbbvsQCD() const{return m_btag_MassDecorrelatedDeepBoosted_ZHbbvsQCD;}
  float btag_MassDecorrelatedDeepBoosted_ZvsQCD() const{return m_btag_MassDecorrelatedDeepBoosted_ZvsQCD;}
  float btag_MassDecorrelatedDeepBoosted_ZbbvsQCD() const{return m_btag_MassDecorrelatedDeepBoosted_ZbbvsQCD;}
  float btag_MassDecorrelatedDeepBoosted_HbbvsQCD() const{return m_btag_MassDecorrelatedDeepBoosted_HbbvsQCD;}
  float btag_MassDecorrelatedDeepBoosted_H4qvsQCD() const{return m_btag_MassDecorrelatedDeepBoosted_H4qvsQCD;}

  //raw scores for each sub-category
  float btag_DeepBoosted_probQCDb() const{return m_btag_DeepBoosted_probQCDb;}
  float btag_DeepBoosted_probQCDbb() const{return m_btag_DeepBoosted_probQCDbb;}
  float btag_DeepBoosted_probQCDc() const{return m_btag_DeepBoosted_probQCDc;}
  float btag_DeepBoosted_probQCDcc() const{return m_btag_DeepBoosted_probQCDcc;}
  float btag_DeepBoosted_probQCDothers() const{return m_btag_DeepBoosted_probQCDothers;}

  float btag_DeepBoosted_probTbqq() const{return m_btag_DeepBoosted_probTbqq;}
  float btag_DeepBoosted_probTbcq() const{return m_btag_DeepBoosted_probTbcq;}
  float btag_DeepBoosted_probTbq() const{return m_btag_DeepBoosted_probTbq;}
  float btag_DeepBoosted_probTbc() const{return m_btag_DeepBoosted_probTbc;}

  float btag_DeepBoosted_probWqq() const{return m_btag_DeepBoosted_probWqq;}
  float btag_DeepBoosted_probWcq() const{return m_btag_DeepBoosted_probWcq;}

  float btag_DeepBoosted_probZcc() const{return m_btag_DeepBoosted_probZcc;}
  float btag_DeepBoosted_probZqq() const{return m_btag_DeepBoosted_probZqq;}
  float btag_DeepBoosted_probZbb() const{return m_btag_DeepBoosted_probZbb;}

  float btag_DeepBoosted_probHbb() const{return m_btag_DeepBoosted_probHbb;}
  float btag_DeepBoosted_probHcc() const{return m_btag_DeepBoosted_probHcc;}
  float btag_DeepBoosted_probHqqqq() const{return m_btag_DeepBoosted_probHqqqq;}


  //raw scores, following https://gitlab.cern.ch/DeepAK8/NNKit/blob/for94X/FatJetNN/interface/FatJetNNHelper.h#L75
  float btag_DeepBoosted_raw_score_qcd() const{return m_btag_DeepBoosted_probQCDb+m_btag_DeepBoosted_probQCDbb+m_btag_DeepBoosted_probQCDc+m_btag_DeepBoosted_probQCDcc+m_btag_DeepBoosted_probQCDothers;}
  float btag_DeepBoosted_raw_score_top() const{return m_btag_DeepBoosted_probTbq+m_btag_DeepBoosted_probTbc+m_btag_DeepBoosted_probTbqq+m_btag_DeepBoosted_probTbcq;}
  float btag_DeepBoosted_raw_score_w() const{return m_btag_DeepBoosted_probWqq+m_btag_DeepBoosted_probWcq;}
  float btag_DeepBoosted_raw_score_z() const{return m_btag_DeepBoosted_probZcc+m_btag_DeepBoosted_probZbb+m_btag_DeepBoosted_probZqq;}
  float btag_DeepBoosted_raw_score_h() const{return m_btag_DeepBoosted_probHbb+m_btag_DeepBoosted_probHcc+m_btag_DeepBoosted_probHqqqq;}


  //raw score for each sub-category of mass decorrelated DeepAK8 tagger
  float btag_MassDecorrelatedDeepBoosted_bbvsLight() const{return m_btag_MassDecorrelatedDeepBoosted_bbvsLight;}
  float btag_MassDecorrelatedDeepBoosted_ccvsLight() const{return m_btag_MassDecorrelatedDeepBoosted_ccvsLight;}
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

  //raw scores of mass decorrelated DeepAK8 tagger, following https://gitlab.cern.ch/DeepAK8/NNKit/blob/for94X/FatJetNN/interface/FatJetNNHelper.h#L75
  float btag_MassDecorrelatedDeepBoosted_raw_score_qcd() const{return m_btag_MassDecorrelatedDeepBoosted_probQCDb+m_btag_MassDecorrelatedDeepBoosted_probQCDbb+m_btag_MassDecorrelatedDeepBoosted_probQCDc+m_btag_MassDecorrelatedDeepBoosted_probQCDcc+m_btag_MassDecorrelatedDeepBoosted_probQCDothers;}
  float btag_MassDecorrelatedDeepBoosted_raw_score_top()const{return m_btag_MassDecorrelatedDeepBoosted_probTbq+m_btag_MassDecorrelatedDeepBoosted_probTbc+m_btag_MassDecorrelatedDeepBoosted_probTbqq+m_btag_MassDecorrelatedDeepBoosted_probTbcq;}
  float btag_MassDecorrelatedDeepBoosted_raw_score_w()const{return m_btag_MassDecorrelatedDeepBoosted_probWqq+m_btag_MassDecorrelatedDeepBoosted_probWcq;}
  float btag_MassDecorrelatedDeepBoosted_raw_score_z()const{return m_btag_MassDecorrelatedDeepBoosted_probZcc+m_btag_MassDecorrelatedDeepBoosted_probZbb+m_btag_MassDecorrelatedDeepBoosted_probZqq;}
  float btag_MassDecorrelatedDeepBoosted_raw_score_h()const{return m_btag_MassDecorrelatedDeepBoosted_probHbb+m_btag_MassDecorrelatedDeepBoosted_probHcc+m_btag_MassDecorrelatedDeepBoosted_probHqqqq;}




  


  // setters
  void set_qjets_volatility(float x){m_qjets_volatility = x;}
  void set_tau1(float x){m_tau1 = x;}
  void set_tau2(float x){m_tau2 = x;}
  void set_tau3(float x){m_tau3 = x;}
  void set_tau4(float x){m_tau4 = x;}
  void set_tau1_groomed(float x){m_tau1_groomed = x;}
  void set_tau2_groomed(float x){m_tau2_groomed = x;}
  void set_tau3_groomed(float x){m_tau3_groomed = x;}
  void set_tau4_groomed(float x){m_tau4_groomed = x;}

  void set_ecfN2_beta1(float x){m_ecfN2_beta1 = x;}
  void set_ecfN2_beta2(float x){m_ecfN2_beta2 = x;}
  void set_ecfN3_beta1(float x){m_ecfN3_beta1 = x;}
  void set_ecfN3_beta2(float x){m_ecfN3_beta2 = x;}

  void set_mvahiggsdiscr(float x){m_mvahiggsdiscr = x;}

  void set_softdropmass(float x){m_softdropmass = x;}

  void add_subjet(const Jet & subjet){m_subjets.push_back(subjet);}

  void set_subjets(std::vector<Jet> subjets){ std::swap(m_subjets, subjets);} // note: move not possible in C++98
  
  void set_tag(tag t, float value){ tags.set_tag(static_cast<int>(t), value);}

  void set_btag_BoostedDoubleSecondaryVertexAK8(float x){m_btag_BoostedDoubleSecondaryVertexAK8=x;}
  void set_btag_BoostedDoubleSecondaryVertexCA15(float x){m_btag_BoostedDoubleSecondaryVertexCA15=x;}

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

private:
  std::vector<Jet> m_subjets;

  // substructure information
  float m_qjets_volatility;
  float m_tau1;
  float m_tau2;
  float m_tau3;
  float m_tau4;
  float m_tau1_groomed;
  float m_tau2_groomed;
  float m_tau3_groomed;
  float m_tau4_groomed;
  
  float m_ecfN2_beta1;
  float m_ecfN2_beta2;
  float m_ecfN3_beta1;
  float m_ecfN3_beta2;

  float m_mvahiggsdiscr;
  float m_softdropmass;

  float m_btag_BoostedDoubleSecondaryVertexAK8;
  float m_btag_BoostedDoubleSecondaryVertexCA15;
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

  Tags tags;
};

