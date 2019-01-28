#pragma once

#include "Particle.h"

class GenTopJet : public GenJet {
public:

  GenTopJet() {
    m_tau1 = m_tau2 = m_tau3 = -1;
  }

  const std::vector<GenJet> & subjets() const{return m_subjets;}
  void add_subjet(const GenJet & p){m_subjets.push_back(p);}
  float tau1() const{return m_tau1;}
  void  set_tau1(float tau1){m_tau1=tau1;}
  float tau2() const{return m_tau2;}
  void  set_tau2(float tau2){m_tau2=tau2;}
  float tau3() const{return m_tau3;}
  void  set_tau3(float tau3){m_tau3=tau3;}
 
private:
  std::vector<GenJet> m_subjets;
  double m_tau1,m_tau2,m_tau3;
};

