#pragma once

#include "Particle.h"
#include <vector>

// L1EGamma seeds 
//see https://twiki.cern.ch/twiki/pub/CMS/HowToL1TriggerMenu/17.03.14_L1EG_parameters.pdf
//see https://cmssdt.cern.ch/lxr/source/DataFormats/L1Trigger/interface/EGamma.h?v=CMSSW_8_4_0&%21v=CMSSW_9_4_0

class L1EGamma : public Particle{
public:
  L1EGamma(){
      m_bx = -100;
      m_iso = -100;
      m_towerIeta = 0; // ieta of seed tower       
      m_towerIphi = 0; // iphi of seed tower           
      m_NTT = 0; // n towers above threshold
      m_Shape = 0; // cluster shape variable


  }
    
  int bx() const{return m_bx;}
  int iso() const{return m_iso;}
  int nTT() const{return m_NTT;}
  int shape() const{return m_Shape;}
  int towerIeta() const{return m_towerIeta;}
  int towerIphi() const{return m_towerIphi;}

  void set_bx(int x){  m_bx=x;}
  void set_iso(int x){  m_iso=x;}
  void set_towerIeta(int x){ m_towerIeta=x;}
  void set_towerIphi(int x){ m_towerIphi=x;}
  void set_nTT(int x){ m_NTT=x;}
  void set_Shape(int x){ m_Shape=x;}

private:
  int m_bx, m_iso, m_towerIeta, m_towerIphi, m_NTT, m_Shape;
};

