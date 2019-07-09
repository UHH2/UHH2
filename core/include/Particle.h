#pragma once

#include "UHH2/core/include/LorentzVector.h"


class Particle{
public:

   Particle(): m_charge(0), m_pt(0), m_eta(0), m_phi(0), m_energy(0){
   }

  /// four-momentum
  LorentzVector v4() const{
    LorentzVector v4;
    v4.SetPt(m_pt);
    v4.SetEta(m_eta);
    v4.SetPhi(m_phi);
    v4.SetE(m_energy);
    return v4;
  }

  /// charge
  short charge() const{return m_charge;}
  /// transverse momentum
  float pt() const {return m_pt;}
  /// pseudo-rapidity
  float eta() const{return m_eta;}
  /// phi
  float phi() const{return m_phi;}
  ///energy
  float energy() const{return m_energy;}

  /// set charge
  void set_charge(short charge){m_charge=charge;}
  /// set transverse momentum
  void set_pt(float pt){m_pt=pt;}
  /// set pseudo-rapidity
  void set_eta(float eta){m_eta=eta;}
  /// set phi
  void set_phi(float phi){m_phi=phi;}
  /// set energy
  void set_energy(float energy){m_energy=energy;}

  /// set four-momentum
  void set_v4(const LorentzVector & v4){
    set_pt(v4.Pt());
    set_eta(v4.Eta());
    set_phi(v4.Phi());
    set_energy(v4.E());
  }

private:

  short m_charge;
  float m_pt;
  float m_eta;
  float m_phi;
  float m_energy;
};

inline bool operator==(const Particle& lhs, const Particle& rhs) {
  return (lhs.v4() == rhs.v4());
}
