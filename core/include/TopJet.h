#pragma once

#include "Jet.h"
#include "JetBTagInfo.h"
#include "Tags.h"

class TopJet: public Jet{
public:
  enum tag { /* for  future use ... */ };
  
  TopJet(){
      m_qjets_volatility = m_tau1 = m_tau2 = m_tau3 = -1.0f;
  }

  // getters
  float qjets_volatility() const {return m_qjets_volatility;}
  float tau1() const {return m_tau1;}
  float tau2() const {return m_tau2;}
  float tau3() const {return m_tau3;}
  
  const std::vector<Jet> & subjets() const{return m_subjets;}
  const std::vector<JetBTagInfo> & subjetsbtaginfo() const{return m_subjetsbtaginfo;}
  
  float get_tag(tag t) const{ return tags.get_tag(static_cast<int>(t)); }

  // setters
  void set_qjets_volatility(float x){m_qjets_volatility = x;}
  void set_tau1(float x){m_tau1 = x;}
  void set_tau2(float x){m_tau2 = x;}
  void set_tau3(float x){m_tau3 = x;}

  void add_subjet(const Jet & subjet){m_subjets.push_back(subjet);}
  void add_subjetbtaginfo(const JetBTagInfo & subjetbtaginfo){m_subjetsbtaginfo.push_back(subjetbtaginfo);}

  void set_subjets(std::vector<Jet> subjets){ std::swap(m_subjets, subjets);} // note: move not possible in C++98
  void set_subjetsbtaginfo(std::vector<JetBTagInfo> subjetsbtaginfo){ std::swap(m_subjetsbtaginfo, subjetsbtaginfo);} // note: move not possible in C++98
  
  void set_tag(tag t, float value){ tags.set_tag(static_cast<int>(t), value);}

private:
  std::vector<Jet> m_subjets;
  std::vector<JetBTagInfo> m_subjetsbtaginfo;

  // substructure information
  float m_qjets_volatility;
  float m_tau1;
  float m_tau2;
  float m_tau3;
  
  Tags tags;
};

