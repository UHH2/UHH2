#ifndef PFParticle_H
#define PFParticle_H

#include "Particle.h"

/**
 *  @short PF candidate class
 *  @author Roman Kogler
 */

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
     m_track_p=0.; 
     m_ecal_en=0.; 
     m_hcal_en=0.; 
     m_jetparticle=false;
     m_isoparticle=false;
     m_puisoparticle=false;
     m_fromPV=false;
  };
  
  ~PFParticle(){
  };

  EParticleID particleID(){return (EParticleID)m_particleID;}
  double track_mom(){return m_track_p;} 
  double ecal_en(){return m_ecal_en;} 
  double hcal_en(){return m_hcal_en;} 
  /// returns true, if this PFParticle has been clustered to a top-jet
  bool isJetParticle(){return m_jetparticle;}
  /// returns true, if this PFParticle has been used to calculate lepton isolation
  bool isIsoParticle(){return m_isoparticle;}
  /// returns true, if this PFParticle has been used to calculate pile-up contribution to lepton isolation
  bool isPUIsoParticle(){return m_puisoparticle;}  
  /// returns true, if the particle is a charged particle from primary vertex
  bool fromPV(){return m_fromPV;}

  void set_particleID(EParticleID id){m_particleID = id;}
  void set_track_mom(double p){m_track_p = p;} 
  void set_ecal_en(double e){m_ecal_en = e;} 
  void set_hcal_en(double e){m_hcal_en = e;} 
  void set_isJetParticle(bool x){m_jetparticle = x;}
  void set_isIsoParticle(bool x){m_isoparticle = x;}
  void set_isPUIsoParticle(bool x){m_puisoparticle = x;}
  void set_fromPV(bool x){m_fromPV = x;}

 private:
  
  char m_particleID;
  double m_track_p; 
  double m_ecal_en; 
  double m_hcal_en; 

  bool m_jetparticle;
  bool m_isoparticle;
  bool m_puisoparticle;
  bool m_fromPV;

};

#endif
