#pragma once

#include "Particle.h"

class GenTopJet : public GenJet {
public:

  GenTopJet() {
    m_tau1 = m_tau2 = m_tau3 = m_tau4 = m_ecfN2_beta1 = m_ecfN2_beta2 = m_ecfN3_beta1 = m_ecfN3_beta2 = -1;
  }

  const std::vector<GenJet> & subjets() const{return m_subjets;}
  float softdropmass() const {LorentzVector v; for (const auto s : m_subjets) v += s.v4(); return v.M(); }
  void add_subjet(const GenJet & p){m_subjets.push_back(p);}

  float tau1() const{return m_tau1;}
  float tau2() const{return m_tau2;}
  float tau3() const{return m_tau3;}
  float tau4() const{return m_tau4;}

  // energy correlation functions, N2 & N3, each with beta=1 or beta=2
  float ecfN2_beta1() const {return m_ecfN2_beta1;}
  float ecfN2_beta2() const {return m_ecfN2_beta2;}
  float ecfN3_beta1() const {return m_ecfN3_beta1;}
  float ecfN3_beta2() const {return m_ecfN3_beta2;}

  void set_tau1(float tau1){m_tau1=tau1;}
  void set_tau2(float tau2){m_tau2=tau2;}
  void set_tau3(float tau3){m_tau3=tau3;}
  void set_tau4(float tau4){m_tau4=tau4;}

  void set_ecfN2_beta1(float x){m_ecfN2_beta1 = x;}
  void set_ecfN2_beta2(float x){m_ecfN2_beta2 = x;}
  void set_ecfN3_beta1(float x){m_ecfN3_beta1 = x;}
  void set_ecfN3_beta2(float x){m_ecfN3_beta2 = x;}
private:
  std::vector<GenJet> m_subjets;
  float m_tau1, m_tau2, m_tau3, m_tau4;
  float m_ecfN2_beta1, m_ecfN2_beta2, m_ecfN3_beta1, m_ecfN3_beta2;

};

