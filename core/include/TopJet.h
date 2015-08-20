#pragma once

#include "Jet.h"

#include "Tags.h"

class TopJet: public Jet{

public:
   enum tag {
      mass,
      fRec,
      Ropt,
      massRatioPassed,
      RoptCalc,
      ptForRoptCalc,
    };
   
   static tag tagname2tag(const std::string & tagname){
      if(tagname == "mass") return mass;
      if(tagname == "fRec") return fRec;
      if(tagname == "Ropt") return Ropt;
      if(tagname == "RoptCalc") return RoptCalc;
      if(tagname == "ptForRoptCalc") return ptForRoptCalc;
      if(tagname == "massRatioPassed") return massRatioPassed;
      throw std::runtime_error("unknown tag '" + tagname + "'");
   }

   
  
  TopJet(){
      m_qjets_volatility = m_tau1 = m_tau2 = m_tau3 = m_mvahiggsdiscr = m_prunedmass = -1.0f;
  }

  // getters
  float qjets_volatility() const {return m_qjets_volatility;}
  float tau1() const {return m_tau1;}
  float tau2() const {return m_tau2;}
  float tau3() const {return m_tau3;}
  
  float mvahiggsdiscr() const {return m_mvahiggsdiscr;}

  float prunedmass() const {return m_prunedmass;}

  const std::vector<Jet> & subjets() const{return m_subjets;}
  
  float get_tag(tag t) const{ return tags.get_tag(static_cast<int>(t)); }
  float has_tag(tag t) const{ return tags.has_tag(static_cast<int>(t)); }
 

  // setters
  void set_qjets_volatility(float x){m_qjets_volatility = x;}
  void set_tau1(float x){m_tau1 = x;}
  void set_tau2(float x){m_tau2 = x;}
  void set_tau3(float x){m_tau3 = x;}

  void set_mvahiggsdiscr(float x){m_mvahiggsdiscr = x;}

  void set_prunedmass(float x){m_prunedmass = x;}

  void add_subjet(const Jet & subjet){m_subjets.push_back(subjet);}

  void set_subjets(std::vector<Jet> subjets){ std::swap(m_subjets, subjets);} // note: move not possible in C++98
  
  void set_tag(tag t, float value){ tags.set_tag(static_cast<int>(t), value);}

private:
  std::vector<Jet> m_subjets;

  // substructure information
  float m_qjets_volatility;
  float m_tau1;
  float m_tau2;
  float m_tau3;
  
  float m_mvahiggsdiscr;

  float m_prunedmass;

  Tags tags;
};

