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
      m_qjets_volatility = m_tau1 = m_tau2 = m_tau3 = m_tau4 = m_mvahiggsdiscr = m_prunedmass = m_softdropmass = m_tau1_groomed = m_tau2_groomed = m_tau3_groomed = m_tau4_groomed = m_ecfN2_beta1 = m_ecfN2_beta2 = m_ecfN3_beta1 = m_ecfN3_beta2 = -1.0f;
  }

  // getters
  float qjets_volatility() const {return m_qjets_volatility;}
  float tau1() const {return m_tau1;}
  float tau2() const {return m_tau2;}
  float tau3() const {return m_tau3;}
  float tau1_groomed() const {return m_tau1_groomed;}
  float tau2_groomed() const {return m_tau2_groomed;}
  float tau3_groomed() const {return m_tau3_groomed;}
  
  // energy correlation functions, N2 & N3, each with beta=1 or beta=2
  // calculated on softdrop jets
  float ecfN2_beta1() const {return m_ecfN2_beta1;}
  float ecfN2_beta2() const {return m_ecfN2_beta2;}
  float ecfN3_beta1() const {return m_ecfN3_beta1;}
  float ecfN3_beta2() const {return m_ecfN3_beta2;}

  float mvahiggsdiscr() const {return m_mvahiggsdiscr;}

  float prunedmass() const {return m_prunedmass;}

  float softdropmass() const {return m_softdropmass;}

  const std::vector<Jet> & subjets() const{return m_subjets;}
  
  float get_tag(tag t) const{ return tags.get_tag(static_cast<int>(t)); }
  float has_tag(tag t) const{ return tags.has_tag(static_cast<int>(t)); }
 

  // setters
  void set_qjets_volatility(float x){m_qjets_volatility = x;}
  void set_tau1(float x){m_tau1 = x;}
  void set_tau2(float x){m_tau2 = x;}
  void set_tau3(float x){m_tau3 = x;}
  void set_tau1_groomed(float x){m_tau1_groomed = x;}
  void set_tau2_groomed(float x){m_tau2_groomed = x;}
  void set_tau3_groomed(float x){m_tau3_groomed = x;}

  void set_ecfN2_beta1(float x){m_ecfN2_beta1 = x;}
  void set_ecfN2_beta2(float x){m_ecfN2_beta2 = x;}
  void set_ecfN3_beta1(float x){m_ecfN3_beta1 = x;}
  void set_ecfN3_beta2(float x){m_ecfN3_beta2 = x;}

  void set_mvahiggsdiscr(float x){m_mvahiggsdiscr = x;}

  void set_prunedmass(float x){m_prunedmass = x;}

  void set_softdropmass(float x){m_softdropmass = x;}

  void add_subjet(const Jet & subjet){m_subjets.push_back(subjet);}

  void set_subjets(std::vector<Jet> subjets){ std::swap(m_subjets, subjets);} // note: move not possible in C++98
  
  void set_tag(tag t, float value){ tags.set_tag(static_cast<int>(t), value);}

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

  float m_prunedmass;

  float m_softdropmass;

  Tags tags;
};

