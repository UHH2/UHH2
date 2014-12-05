#ifndef ReconstructionHypothesis_H
#define ReconstructionHypothesis_H

#include "../core/include/Particle.h"

struct qualityflag{
  std::string label;
  float discriminator;
};

/**
 *  @short container class to store the results of the top quark reconstruction
 */

class ReconstructionHypothesis{

 public:


  ReconstructionHypothesis(){
    m_tophad_jets_ind.clear();
    m_toplep_jets_ind.clear();
    m_blep_ind=-1;
    LorentzVector v4(0,0,0,0);
    m_toplep_v4=v4;
    m_tophad_v4=v4;
    m_neutrino_v4=v4;
    m_lepton.set_v4(v4);
    m_lepton.set_charge(0);
    m_qualityflags.clear();
    m_blep_v4=v4;
  };
  ~ReconstructionHypothesis(){};

  LorentzVector toplep_v4() const{return m_toplep_v4;}
  LorentzVector tophad_v4() const{return m_tophad_v4;} 
  LorentzVector neutrino_v4() const{return m_neutrino_v4;} 
  Particle lepton() const{return m_lepton;}
  std::vector<unsigned int> tophad_jets_indices() const{return m_tophad_jets_ind;}
  std::vector<unsigned int> toplep_jets_indices() const{return m_toplep_jets_ind;}
  LorentzVector top_v4() const{ return m_lepton.charge() > 0 ? m_toplep_v4 : m_tophad_v4;}
  LorentzVector antitop_v4() const{ return m_lepton.charge() < 0 ? m_toplep_v4 : m_tophad_v4;}
  LorentzVector wlep_v4() const{ return m_neutrino_v4+m_lepton.v4();}
  LorentzVector blep_v4() const{return m_blep_v4;}
  int blep_index() const{ return m_blep_ind;}

  /// get the discriminator value for this hypothesis
  float discriminator(std::string l){
    float discr=0;
    bool found=false;
    for(unsigned int i=0; i<m_qualityflags.size(); ++i){
      if(l==m_qualityflags[i].label){
	discr = m_qualityflags[i].discriminator;
	found = true;
	break;
      }
    }
    if(!found) std::cerr << "WARNING: discriminator with label " << l << " not found in hypothesis, return 0." <<std::endl;
    return discr;
  }
  
  void set_blep_v4(LorentzVector v4){m_blep_v4=v4;}
  void set_toplep_v4(LorentzVector v4){m_toplep_v4=v4;}
  void set_tophad_v4(LorentzVector v4){m_tophad_v4=v4;} 
  void set_neutrino_v4(LorentzVector v4){m_neutrino_v4=v4;}
  void add_toplep_jet_index(unsigned int j){m_toplep_jets_ind.push_back(j);}
  void add_tophad_jet_index(unsigned int j){m_tophad_jets_ind.push_back(j);}
  void set_blep_index(int j){m_blep_ind=j;}
  void set_lepton(Particle p){m_lepton=p;}
  void add_qualityflag(qualityflag q){m_qualityflags.push_back(q);}
  void add_qualityflag(std::string label, float discr){
    qualityflag qflag;
    qflag.label = label;
    qflag.discriminator = discr;
    add_qualityflag(qflag);
  };

  void clear_jetindices(){
    m_tophad_jets_ind.clear();
    m_toplep_jets_ind.clear();
  }
  
  /// test if a discriminator value with a certian label has already been added
  bool has_discriminator(std::string label){
    for(unsigned int i=0; i< m_qualityflags.size(); ++i){
      if(m_qualityflags[i].label == label) return true;
    }
    return false;
  }

 private:

  LorentzVector m_blep_v4;
  LorentzVector m_toplep_v4;
  LorentzVector m_tophad_v4;
  LorentzVector m_neutrino_v4;

  //indices to the jets in the jet list assigned to hadronic and leptonic tops
  std::vector<unsigned int> m_tophad_jets_ind;
  std::vector<unsigned int> m_toplep_jets_ind;
  //index to the jet with highest pt assigned to the leptonic top
  int m_blep_ind;

  Particle m_lepton;

  std::vector<qualityflag> m_qualityflags;

};


#endif
