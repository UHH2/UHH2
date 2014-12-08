#ifndef Particle_H
#define Particle_H

#include <vector>
#include "Math/LorentzVector.h"
#include "Math/PtEtaPhiE4D.h"
#include "TObject.h"

typedef ROOT::Math::LorentzVector< ROOT::Math::PtEtaPhiE4D< Double32_t > > LorentzVector;

/**
 *  @short generic particle class
 *  @author Thomas Peiffer
 */

class Particle{
 public:
  /// Default constructor
  Particle(){
    m_charge=0;
    m_pt=0; 
    m_eta=0; 
    m_phi=0; 
    m_energy=0; 
  };
  /// Default destructor
  ~Particle(){
  };

  /// four-momentum
  LorentzVector v4() const{
    LorentzVector v4;
    v4.SetPt(m_pt);
    v4.SetEta(m_eta);
    v4.SetPhi(m_phi);
    v4.SetE(m_energy);
    return v4;
  };
  
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

  /// distance in phi to particle p2
  double deltaPhi(const Particle & p2) const{
    double deltaphi = fabs(this->phi() - p2.phi());
    if(deltaphi > M_PI) deltaphi = 2* M_PI - deltaphi;
    return deltaphi;
  }
  /// distance in eta-phi plane to particle p2
  double deltaR(const Particle & p2) const{
    double deltaeta = m_eta - p2.m_eta;
    double dphi = deltaPhi(p2);
    return sqrt(deltaeta * deltaeta + dphi * dphi);
  }
  
  double deltaPhi(const LorentzVector & l) const {
      double deltaphi = fabs(this->phi() - l.phi());
      if(deltaphi > M_PI) deltaphi = 2* M_PI - deltaphi;
      return deltaphi;
  }
  
  double deltaR(const LorentzVector & l) const{
    double deltaeta = m_eta - l.eta();
    double dphi = deltaPhi(l);
    return sqrt(deltaeta * deltaeta + dphi * dphi);
  }

 private:

  short m_charge;
  float m_pt; 
  float m_eta; 
  float m_phi; 
  float m_energy; 

};

#endif
