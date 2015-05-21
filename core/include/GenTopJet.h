#pragma once

#include "Particle.h"

class GenTopJet : public Particle {
public:

  const std::vector<Particle> & subjets() const{return m_subjets;}
  void add_subjet(const Particle & p){m_subjets.push_back(p);}
  
private:
  std::vector<Particle> m_subjets;
};

