#ifndef GenParticle_H
#define GenParticle_H

#include "FlavorParticle.h"
#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>

/**
 *  @short generator particle class
 *  @author Thomas Peiffer
 */

class GenParticle : public FlavorParticle{
 public:
  GenParticle(){
    m_status=0;
    m_index=0;
    m_mother1=0;
    m_mother2=0;
    m_daughter1=0;
    m_daughter2=0;
    m_spin=0;
  };
  ~GenParticle(){
  };

  int status() const{return m_status;}
  int index() const{return m_index;}
  int mother1() const{return m_mother1;}
  int mother2() const{return m_mother2;}
  int daughter1() const{return m_daughter1;}
  int daughter2() const{return m_daughter2;}
  int spin() const{return m_spin;}

  //return mother 1 or 2 (ind<=1 or ind>=2)
  const GenParticle* mother(const std::vector<GenParticle> *gplist, int ind=1) const {
    for(unsigned int i=0; i< gplist->size(); ++i){
      if(ind<=1){
	if(this->m_mother1 == gplist->at(i).index()){
	  return &(gplist->at(i));
	}
      }
      else{
	if(this->m_mother2 == gplist->at(i).index()){
	  return &(gplist->at(i));
	}	
      }
    }
    //std::cout << "WARNING: Mother " << ind << " not found in list of GenParticles" << std::endl;
    return 0;
  }

  //return daughter 1 or 2 (ind<=1 or ind>=2)
  const GenParticle* daughter(const std::vector<GenParticle> *gplist, int ind=1) const {
    for(unsigned int i=0; i< gplist->size(); ++i){
      if(ind<=1){
	if(this->m_daughter1 == gplist->at(i).index()){
	  return &(gplist->at(i));
	}
      }
      else{
	if(this->m_daughter2 == gplist->at(i).index()){
	  return &(gplist->at(i));
	}	
      }
    }
    //std::cout << "WARNING: Daughter " << ind << " not found in list of GenParticles" << std::endl;
    return 0;
  }
  
  //print list of particles in one event with their characteristics 
  void Print(const std::vector<GenParticle> *gplist) const{
    std::cout << std::setw(10) << this->m_index << '|';
    std::cout << std::setw(10) << this->pdgId() << '|';
    std::cout << std::setw(10) << this->m_status << '|';
    if(this->mother(gplist, 1)){
      std::ostringstream convert1;
      convert1  << "id:" << this->mother(gplist, 1)->pdgId() << ", ind:" << this->m_mother1;
      std::cout << std::setw(20) << convert1.str() << '|';
    }
    else{std::cout << std::setw(20) << "none" << '|';}
    if(this->mother(gplist, 2)){
      std::ostringstream convert2;
      convert2 << "id:" << this->mother(gplist, 2)->pdgId() << ", ind:" << this->m_mother2;
      std::cout << std::setw(20) << convert2.str() << '|';
    }
    else{std::cout << std::setw(20) << "none" << '|';}
    if(this->daughter(gplist, 1)){
      std::ostringstream convert3;
      convert3 << "id:" << this->daughter(gplist, 1)->pdgId() << ", ind:" << this->m_daughter1;
      std::cout << std::setw(20) << convert3.str() << '|';
    }
    else{std::cout << std::setw(20) << "none" << '|';}
    if(this->daughter(gplist, 2)){
      std::ostringstream convert4;
      convert4 << "id:" << this->daughter(gplist, 2)->pdgId() << ", ind:" << this->m_daughter2;
      std::cout << std::setw(20) << convert4.str() << '|';
    }
    else{std::cout << std::setw(20) << "none" << '|';}
    std::cout << std::setw(10) << this->v4().Px() << '|';
    std::cout << std::setw(10) << this->v4().Py() << '|';
    std::cout << std::setw(10) << this->v4().Pz() << '|';
    std::cout << std::setw(10) << this->energy() << '|';
    std::cout << std::setw(10) << this->pt() << '|';
    double m2 = this->v4().M2();
    double m;
    if (m2>0) m = sqrt(m2);
    else m = (-1)*sqrt((-1)*m2);
    std::cout << std::setw(10) << m << std::endl; 
    return;
  }


  void set_status(int x){  m_status=x;}
  void set_index(int x){  m_index=x;}
  void set_mother1(int x){  m_mother1=x;}
  void set_mother2(int x){  m_mother2=x;}
  void set_daughter1(int x){  m_daughter1=x;}
  void set_daughter2(int x){  m_daughter2=x;}
  void set_spin(int x){  m_spin=x;}

 private:
  short m_status;
  unsigned short m_index;

  unsigned short m_mother1;
  unsigned short m_mother2;
  unsigned short m_daughter1;
  unsigned short m_daughter2;
  short m_spin;
 

};

#endif
