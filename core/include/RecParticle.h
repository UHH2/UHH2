#pragma once

#include "Particle.h"

/* Desined for leptons
takes into account track reconstrcution errors only! */

class RecParticle : public Particle{
public:
 RecParticle(): m_ptError(0), m_etaError(0), m_phiError(0){
  }
  /// error of transverse momentum
  float ptError() const {return m_ptError;}  
  /// error of pseudo-rapidity
  float etaError() const {return m_etaError;} 
  /// error of phi
  float phiError() const{return m_phiError;}
  /* /// error of energy */
  /* float energyError() const{return m_energyError;} */

  void set_ptError(float x){ m_ptError = x;}
  void set_etaError(float x){ m_etaError = x;}
  void set_phiError(float x){m_phiError=x;}
  //  void set_energyError(float x){m_energyError=x;}

private:
  float m_ptError;
  float m_etaError;
  float m_phiError;
  //  float m_energyError;
};

