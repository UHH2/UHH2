#pragma once

#include "Particle.h"


class FlavorParticle : public Particle{
public:
  FlavorParticle(){
      m_pdgId = -1e4;
      m_partonFlavour = -1e4;
      m_hadronFlavour = -1e4;
  }
  int partonFlavour() const{return m_partonFlavour;}
  int hadronFlavour() const{return m_hadronFlavour;}
  void set_partonFlavour(int x){  m_partonFlavour=x;}
  void set_hadronFlavour(int x){  m_hadronFlavour=x;}

  int pdgId() const{return m_pdgId;}
  void set_pdgId(int x){  m_pdgId=x;}
  
private:
  int m_pdgId;  
  int m_partonFlavour;//in 94_X CMSSW based on the highest pt of all partons clustered in a jet 
  int m_hadronFlavour;

};

