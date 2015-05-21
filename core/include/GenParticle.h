#pragma once

#include "FlavorParticle.h"

class GenParticle : public FlavorParticle{
public:
    
  GenParticle(){
      m_status = 0;
      m_index = -1;
      m_mother1 = -1;
      m_mother2 = -1;
      m_daughter1 = -1;
      m_daughter2 = -1;
      m_spin = 0;
  }

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
    return nullptr;
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
    return nullptr;
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

