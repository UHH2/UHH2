#pragma once

#include "Particle.h"
#include <vector>

// L1Jet seeds 
//see https://cmssdt.cern.ch/lxr/source/DataFormats/L1Trigger/interface/Jet.h?v=CMSSW_8_4_0&%21v=CMSSW_9_4_0

class L1Jet : public Particle{
public:
  L1Jet(){
      m_bx = -100;
      m_towerIeta = 0; // ieta of seed tower       
      m_towerIphi = 0; // iphi of seed tower           
      m_puEt = 0; // 
      m_seedEt = 0; // 
      m_rawEt = 0; // 


  }
    
  int bx() const{return m_bx;}
  int towerIeta() const{return m_towerIeta;}
  int towerIphi() const{return m_towerIphi;}

  void set_bx(int x){  m_bx=x;}
  void set_towerIeta(int x){ m_towerIeta=x;}
  void set_towerIphi(int x){ m_towerIphi=x;}
  void set_puEt(int x){ m_puEt=x;}
  void set_seedEt(int x){ m_seedEt=x;}
  void set_rawEt(int x){ m_rawEt=x;}

private:
  int m_bx, m_towerIeta, m_towerIphi, m_puEt, m_seedEt, m_rawEt;
};

