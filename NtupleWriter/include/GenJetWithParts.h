#ifndef GenJetWithParts_H
#define GenJetWithParts_H

#include "Particle.h"
#include "TLorentzVector.h"


class GenJetWithParts : public Particle {
 public:
  GenJetWithParts(){
    m_genparticles_indices.clear();
  };

  ~GenJetWithParts(){
  };

  std::vector<unsigned int> genparticles_indices() const{return m_genparticles_indices;}
  void add_genparticles_index(int ind){m_genparticles_indices.push_back(ind);}

 private:
std::vector<unsigned int> m_genparticles_indices;


};

#endif
