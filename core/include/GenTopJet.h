#ifndef GenTopJet_H
#define GenTopJet_H

#include "Particle.h"
#include "TLorentzVector.h"


/**
 *  @short top-jet class with only subjets, used for generator information
 *  @author Roman Kogler
 */

class GenTopJet : public Particle {
 public:
  GenTopJet(){
    m_subjets.clear();
  };

  ~GenTopJet(){
  };


  std::vector<Particle> subjets() const{return m_subjets;}
  void add_subjet(Particle p){m_subjets.push_back(p);}
  
 private:
  std::vector<Particle> m_subjets;



};

#endif
