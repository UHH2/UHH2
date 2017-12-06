#pragma once

#include "Particle.h"

class GenTopJet : public Particle {
public:

  const std::vector<Particle> & subjets() const{return m_subjets;}
  void add_subjet(const Particle & p){m_subjets.push_back(p);}
  float tau1() const{return m_tau1;}
  void  set_tau1(float tau1){m_tau1=tau1;}
  float tau2() const{return m_tau2;}
  void  set_tau2(float tau2){m_tau2=tau2;}
  float tau3() const{return m_tau3;}
  void  set_tau3(float tau3){m_tau3=tau3;}
  double chf() const{return m_chf;}
  void  set_chf(double chf){m_chf=chf;}
  double cef() const{return m_cef;}
  void  set_cef(double cef){m_cef=cef;}
  double nhf() const{return m_nhf;}
  void  set_nhf(double nhf){m_nhf=nhf;}
  double nef() const{return m_nef;}
  void  set_nef(double nef){m_nef=nef;}
  
private:
  std::vector<Particle> m_subjets;
  double m_tau1,m_tau2,m_tau3,m_chf,m_cef,m_nhf,m_nef;
};

