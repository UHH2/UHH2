#ifndef FlavorParticle_H
#define FlavorParticle_H

#include "Particle.h"
#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>

/**
 *  @short particle with flavor/pdgId
 *  @author Thomas Peiffer
 */

class FlavorParticle : public Particle{
 public:
  FlavorParticle(){
    m_pdgId=0;
  };
  ~FlavorParticle(){
  };

  int pdgId() const{return m_pdgId;}
  int flavor() const{return m_pdgId;}

  void set_pdgId(int x){  m_pdgId=x;}
  void set_flavor(int x){  m_pdgId=x;}

 private:
  int m_pdgId;
};

#endif
