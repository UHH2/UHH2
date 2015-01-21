#ifndef Jet_H
#define Jet_H

#include "FlavorParticle.h"
#include "PFParticle.h"

/**
 *  @short jet class
 *  @author Thomas Peiffer
 */

class Jet : public FlavorParticle{

 public:

  Jet(){
     m_nTracks=0;
     m_jetArea=0;
     m_numberOfDaughters=0; 
     m_neutralEmEnergyFraction=0;
     m_neutralHadronEnergyFraction=0;
     m_chargedEmEnergyFraction=0;
     m_chargedHadronEnergyFraction=0;
     m_muonEnergyFraction=0;
     m_photonEnergyFraction=0;
     m_chargedMultiplicity=0;
     m_neutralMultiplicity=0;
     m_muonMultiplicity=0; 
     m_electronMultiplicity=0;
     m_photonMultiplicity=0;
     m_btag_simpleSecondaryVertexHighEff=0;
     m_btag_simpleSecondaryVertexHighPur=0;
     m_btag_combinedSecondaryVertex=0;
     m_btag_combinedSecondaryVertexMVA=0;
     m_btag_jetBProbability=0;
     m_btag_jetProbability=0;
     m_JEC_factor_raw=0;
     m_genjet_index=-1;
     m_pfconstituents_indices.clear();
     m_genjet=NULL;
  };

  ~Jet(){
  };

  LorentzVector genjet_v4() const{
    return genjet().v4();
  };

  Particle genjet() const{
    if(m_genjet){
      return *m_genjet;
    }
    else{
      std::cout << "WARNING: genjet routine called for a jet but genjet collection has not been specified" << std::endl;
      //return 0 particle
      Particle p;
      return p;
    }
  };

  std::vector<unsigned int> pfconstituents_indices() const{return m_pfconstituents_indices;}

  void add_pfconstituents_index(int ind){m_pfconstituents_indices.push_back(ind);}

  int nTracks() const{return m_nTracks;}
  float jetArea() const{return m_jetArea;}
  int numberOfDaughters() const{return m_numberOfDaughters;} 
  float neutralEmEnergyFraction() const{return m_neutralEmEnergyFraction;}
  float neutralHadronEnergyFraction() const{return m_neutralHadronEnergyFraction;}
  float chargedEmEnergyFraction() const{return m_chargedEmEnergyFraction;}
  float chargedHadronEnergyFraction() const{return m_chargedHadronEnergyFraction;}
  float muonEnergyFraction() const{return m_muonEnergyFraction;}
  float photonEnergyFraction() const{return m_photonEnergyFraction;}
  int chargedMultiplicity() const{return m_chargedMultiplicity;}
  int neutralMultiplicity() const{return m_neutralMultiplicity;}
  int muonMultiplicity() const{return m_muonMultiplicity;} 
  int electronMultiplicity() const{return m_electronMultiplicity;}
  int photonMultiplicity() const{return m_photonMultiplicity;}
  float btag_simpleSecondaryVertexHighEff() const{return m_btag_simpleSecondaryVertexHighEff;}
  float btag_simpleSecondaryVertexHighPur() const{return m_btag_simpleSecondaryVertexHighPur;}
  float btag_combinedSecondaryVertex() const{return m_btag_combinedSecondaryVertex;} // combinedInclusiveSecondaryVertexV2BJetTags
  float btag_combinedSecondaryVertexMVA() const{return m_btag_combinedSecondaryVertexMVA;}
  float btag_jetBProbability() const{return m_btag_jetBProbability;}
  float btag_jetProbability() const{return m_btag_jetProbability;}
  float JEC_factor_raw() const{return m_JEC_factor_raw;}
  float genjet_pt() const{return genjet().pt();}
  float genjet_eta() const{return genjet().eta();}
  float genjet_phi() const{return genjet().phi();}
  float genjet_energy() const{return genjet().energy();}
  float genjet_index() const{return m_genjet_index;}

  void set_nTracks(int x){m_nTracks=x;}
  void set_jetArea(float x){m_jetArea=x;}
  void set_numberOfDaughters(int x){m_numberOfDaughters=x;} 
  void set_neutralEmEnergyFraction(float x){m_neutralEmEnergyFraction=x;}
  void set_neutralHadronEnergyFraction(float x){m_neutralHadronEnergyFraction=x;}
  void set_chargedEmEnergyFraction(float x){m_chargedEmEnergyFraction=x;}
  void set_chargedHadronEnergyFraction(float x){m_chargedHadronEnergyFraction=x;}
  void set_muonEnergyFraction(float x){m_muonEnergyFraction=x;}
  void set_photonEnergyFraction(float x){m_photonEnergyFraction=x;}
  void set_chargedMultiplicity(int x){m_chargedMultiplicity=x;}
  void set_neutralMultiplicity(int x){m_neutralMultiplicity=x;}
  void set_muonMultiplicity(int x){m_muonMultiplicity=x;} 
  void set_electronMultiplicity(int x){m_electronMultiplicity=x;}
  void set_photonMultiplicity(int x){m_photonMultiplicity=x;}
  void set_btag_simpleSecondaryVertexHighEff(float x){m_btag_simpleSecondaryVertexHighEff=x;}
  void set_btag_simpleSecondaryVertexHighPur(float x){m_btag_simpleSecondaryVertexHighPur=x;}
  void set_btag_combinedSecondaryVertex(float x){m_btag_combinedSecondaryVertex=x;} // for 72, this is combinedInclusiveSecondaryVertexV2BJetTags
  void set_btag_combinedSecondaryVertexMVA(float x){m_btag_combinedSecondaryVertexMVA=x;}
  void set_btag_jetBProbability(float x){m_btag_jetBProbability=x;}
  void set_btag_jetProbability(float x){m_btag_jetProbability=x;}
  void set_JEC_factor_raw(float x){m_JEC_factor_raw=x;}
  void set_genjet_index(int x){m_genjet_index=x;}

  bool has_genjet() const{return m_genjet_index>=0;}

  void set_genjet(std::vector<Particle>* genjets){
    if(!genjets) return; //no genjet collection -> keep NULL pointer
    if(m_genjet_index<0 || m_genjet_index>(int)genjets->size()) { //genjet collection provided but no matched genjet found -> set genjet pointer to 0 particle
      static Particle* p =  new Particle();
      m_genjet = p;
      return;
    }
    m_genjet = &genjets->at(m_genjet_index);
  }

  bool pfID(){
    //pf ID has already been applied when using goodPatJets
    if(numberOfDaughters()>1 
       && neutralHadronEnergyFraction()<0.99
       && neutralEmEnergyFraction()<0.99){
      
      if(fabs(eta())>=2.4)
        return true;
      
      if(chargedEmEnergyFraction()<0.99
        && chargedHadronEnergyFraction()>0
        && chargedMultiplicity()>0)
        return true;   
    }
    return false;
  }

  void fill_PF_variables(std::vector<PFParticle*> pfparts){

    float mu_energy=0;
    int n_mu=0; 
    float ele_energy=0;
    int n_ele=0; 
    float ch_energy=0;
    //int n_ch=0; 
    float nh_energy=0;
    //int n_nh=0; 
    float ph_energy=0;
    int n_ph=0; 
    int n_dau=0;
    int n_cm=0;
    int n_nm=0;
    int jet_charge=0;
    for(unsigned int i=0; i< m_pfconstituents_indices.size(); i++){
      if(m_pfconstituents_indices[i]>pfparts.size()){
        std::cerr << "ERROR: PFParticle index out of range in this jet, check list of particles given to fill_PF_variables routine" << std::endl;
        continue;
      }
      n_dau++;
      if(pfparts.at(m_pfconstituents_indices[i])->charge()!=0) n_cm++;
      else n_nm++;
      jet_charge+=pfparts.at(m_pfconstituents_indices[i])->charge();
      if(pfparts.at(m_pfconstituents_indices[i])->particleID()==PFParticle::eMu){ n_mu++;  mu_energy+=pfparts.at(m_pfconstituents_indices[i])->v4().E();}
      if(pfparts.at(m_pfconstituents_indices[i])->particleID()==PFParticle::eGamma){ n_ph++;  ph_energy+=pfparts.at(m_pfconstituents_indices[i])->v4().E();} 
      if(pfparts.at(m_pfconstituents_indices[i])->particleID()==PFParticle::eH){ /*n_ch++;*/  ch_energy+=pfparts.at(m_pfconstituents_indices[i])->v4().E();} 
      if(pfparts.at(m_pfconstituents_indices[i])->particleID()==PFParticle::eH0){ /*n_nh++;*/  nh_energy+=pfparts.at(m_pfconstituents_indices[i])->v4().E();} 
      if(pfparts.at(m_pfconstituents_indices[i])->particleID()==PFParticle::eE){ n_ele++;  ele_energy+=pfparts.at(m_pfconstituents_indices[i])->v4().E();} 
    }
    this->set_charge(jet_charge);
    this->set_numberOfDaughters(n_dau);
    this->set_chargedMultiplicity(n_cm);
    this->set_neutralMultiplicity(n_nm);
    this->set_muonMultiplicity(n_mu);
    this->set_muonEnergyFraction(mu_energy/this->v4().E());
    this->set_chargedHadronEnergyFraction(ch_energy/this->v4().E());
    this->set_neutralHadronEnergyFraction(nh_energy/this->v4().E());
    this->set_electronMultiplicity(n_ele);
    this->set_chargedEmEnergyFraction(ele_energy/this->v4().E());
    this->set_photonMultiplicity(n_ph);
    this->set_photonEnergyFraction(ph_energy/this->v4().E());
    this->set_neutralEmEnergyFraction(ph_energy/this->v4().E());
  }

 private:
  
  int m_nTracks;
  float m_jetArea;
  int m_numberOfDaughters; 
  float m_neutralEmEnergyFraction;
  float m_neutralHadronEnergyFraction;
  float m_chargedEmEnergyFraction;
  float m_chargedHadronEnergyFraction;
  float m_muonEnergyFraction;
  float m_photonEnergyFraction;
  int m_chargedMultiplicity;
  int m_neutralMultiplicity;
  int m_muonMultiplicity; 
  int m_electronMultiplicity;
  int m_photonMultiplicity;
  float m_btag_simpleSecondaryVertexHighEff;
  float m_btag_simpleSecondaryVertexHighPur;
  float m_btag_combinedSecondaryVertex;
  float m_btag_combinedSecondaryVertexMVA;
  float m_btag_jetBProbability;
  float m_btag_jetProbability;
  float m_JEC_factor_raw;
  int m_genjet_index;
  Particle* m_genjet; //!

  std::vector<unsigned int> m_pfconstituents_indices;


};

#endif
