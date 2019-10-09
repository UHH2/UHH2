#pragma once

#include "Particle.h"

class PFParticle : public Particle{

public:

  enum EParticleID {
    eX=0,          /**< undefined */
    eH,            /**< charged hadron */
    eE,            /**< electron  */
    eMu,           /**< muon  */
    eGamma,        /**< photon */
    eH0,           /**< neutral hadron */
    eH_HF,         /**< HF tower identified as a hadron */
    eEgamma_HF     /**< HF tower identified as an EM particle */
  };

  PFParticle(){
     m_particleID=eX;
     m_puppi_weight=1.;
     m_puppi_weight_nolep=1;
  };
  
  ~PFParticle(){
  };

  EParticleID particleID() const {return m_particleID;}
  float puppiWeight() const {return m_puppi_weight;}
  float puppiWeightNoLep() const {return m_puppi_weight_nolep;}

  void set_particleID(EParticleID id){m_particleID = id;}
  void set_puppiWeight(float e){m_puppi_weight = e;}
  void set_puppiWeightNoLep(float e){m_puppi_weight_nolep = e;}

 private:
  
  EParticleID m_particleID;
  float m_puppi_weight;
  float m_puppi_weight_nolep;

};
