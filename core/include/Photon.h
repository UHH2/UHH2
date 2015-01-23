#pragma once

#include "Particle.h"
#include "Tags.h"

class Photon: public Particle{
public:
    
  enum tag { /* for future use */};
  
  Photon(){
    m_vertex_x = 0; 
    m_vertex_y = 0; 
    m_vertex_z = 0; 
    m_supercluster_eta = 0; 
    m_supercluster_phi = 0; 
    m_trackIso = 0;
  }

  float vertex_x() const{return m_vertex_x;} 
  float vertex_y() const{return m_vertex_y;} 
  float vertex_z() const{return m_vertex_z;} 
  float supercluster_eta() const{return m_supercluster_eta;} 
  float supercluster_phi() const{return m_supercluster_phi;} 
  float trackIso() const{return m_trackIso;} 
  
  float get_tag(tag t) const{ return tags.get_tag(static_cast<int>(t));}

  void set_vertex_x(float x){m_vertex_x=x;} 
  void set_vertex_y(float x){m_vertex_y=x;} 
  void set_vertex_z(float x){m_vertex_z=x;} 
  void set_supercluster_eta(float x){m_supercluster_eta=x;} 
  void set_supercluster_phi(float x){m_supercluster_phi=x;} 
  void set_trackIso(float x){m_trackIso=x;} 
  
  void set_tag(tag t, float value){tags.set_tag(static_cast<int>(t), value);}
 
private:
  float m_vertex_x;
  float m_vertex_y;
  float m_vertex_z;
  float m_supercluster_eta;
  float m_supercluster_phi;
  float m_trackIso;
  
  Tags tags;
};
