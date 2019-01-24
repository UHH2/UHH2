#pragma once

#include "Particle.h"


class GenJet : public Particle{
public:
  GenJet(){
    m_partonFlavour = -1e4;
    m_hadronFlavour = -1e4;
    m_genparticles_indices.clear();
    m_chf = m_cef = m_nhf = m_nef = m_muf = -1.0;
  }
  ~GenJet(){};

  int partonFlavour() const{return m_partonFlavour;}
  int hadronFlavour() const{return m_hadronFlavour;}
  void set_partonFlavour(int x){  m_partonFlavour=x;}
  void set_hadronFlavour(int x){  m_hadronFlavour=x;}

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
  int m_partonFlavour;//in 94_X CMSSW based on the highest pt of all partons clustered in a jet 
  int m_hadronFlavour;
  std::vector<unsigned int> m_genparticles_indices;
  double m_chf,m_cef,m_nhf,m_nef,m_muf;
};

