#pragma once

#include "FlavorParticle.h"


class GenJet : public FlavorParticle{
public:
  GenJet(){
    m_genparticles_indices.clear();
    m_chf = m_cef = m_nhf = m_nef = m_muf = -1.0;
    m_chargedMultiplicity = m_neutralMultiplicity = m_muonMultiplicity = m_electronMultiplicity = m_photonMultiplicity = 0;
  }
  ~GenJet(){};


  std::vector<unsigned int> genparticles_indices() const{return m_genparticles_indices;}
  void add_genparticles_index(int ind){m_genparticles_indices.push_back(ind);}

  double chf() const{return m_chf;}  // charged hadron energy fraction
  void  set_chf(double chf){m_chf=chf;}
  double cef() const{return m_cef;}  // charged electromagnetic energy fraction AKA electrons
  void  set_cef(double cef){m_cef=cef;}
  double nhf() const{return m_nhf;}  // neutral hadron energy fraction
  void  set_nhf(double nhf){m_nhf=nhf;}
  double nef() const{return m_nef;}  // neutral electromagnetic energy fraction AKA photons
  void  set_nef(double nef){m_nef=nef;}
  double muf() const{return m_muf;}  // muon energy fraction
  void  set_muf(double muf){m_muf=muf;}

  int chargedMultiplicity() const{return m_chargedMultiplicity;} // All charged particles, including electrons and muons
  void set_chargedMultiplicity(int x){m_chargedMultiplicity=x;}
  int neutralMultiplicity() const{return m_neutralMultiplicity;} // All neutrals, including photons
  void set_neutralMultiplicity(int x){m_neutralMultiplicity=x;}
  int muonMultiplicity() const{return m_muonMultiplicity;}
  void set_muonMultiplicity(int x){m_muonMultiplicity=x;}
  int electronMultiplicity() const{return m_electronMultiplicity;}
  void set_electronMultiplicity(int x){m_electronMultiplicity=x;}
  int photonMultiplicity() const{return m_photonMultiplicity;}
  void set_photonMultiplicity(int x){m_photonMultiplicity=x;}

private:
  std::vector<unsigned int> m_genparticles_indices;
  double m_chf,m_cef,m_nhf,m_nef,m_muf;// jet energy fractions calculated from stable partons used for jet clustering
  int m_chargedMultiplicity, m_neutralMultiplicity, m_muonMultiplicity, m_electronMultiplicity, m_photonMultiplicity;
};

