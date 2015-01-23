#pragma once

#include "Particle.h"


class FlavorParticle : public Particle{
public:
  FlavorParticle(){
      m_pdgId = 0;
  }
    
  int pdgId() const{return m_pdgId;}
  int flavor() const{return m_pdgId;}

  void set_pdgId(int x){  m_pdgId=x;}
  void set_flavor(int x){  m_pdgId=x;}

private:
  int m_pdgId;
};

