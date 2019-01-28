#pragma once

#include "FlavorParticle.h"


class GenJet : public FlavorParticle{
public:
  GenJet(){
    m_genparticles_indices.clear();
    m_chf = m_cef = m_nhf = m_nef = m_muf = -1.0;
  }
  ~GenJet(){};


  std::vector<unsigned int> genparticles_indices() const{return m_genparticles_indices;}
  void add_genparticles_index(int ind){m_genparticles_indices.push_back(ind);}

  double chf() const{return m_chf;}
  void  set_chf(double chf){m_chf=chf;}
  double cef() const{return m_cef;}
  void  set_cef(double cef){m_cef=cef;}
  double nhf() const{return m_nhf;}
  void  set_nhf(double nhf){m_nhf=nhf;}
  double nef() const{return m_nef;}
  void  set_nef(double nef){m_nef=nef;}
  double muf() const{return m_muf;}
  void  set_muf(double muf){m_muf=muf;}

private:
  std::vector<unsigned int> m_genparticles_indices;
  double m_chf,m_cef,m_nhf,m_nef,m_muf;// jet energy fractions calculated from stable partons used for jet clustering
};

