#pragma once

class PrimaryVertex{
public:
    
  PrimaryVertex(){
      m_x = m_y = m_z = 0;
      m_nTracks = 0;
      m_chi2 = m_ndof = 0;
  }

  float x() const{return m_x;} 
  float y() const{return m_y;} 
  float z() const{return m_z;} 
  unsigned int nTracks() const{return m_nTracks;} 
  float chi2() const{return m_chi2;} 
  float ndof() const{return m_ndof;}

  float rho() const{return sqrt(m_x*m_x+m_y*m_y);}

  void set_x(float x){m_x=x;} 
  void set_y(float x){m_y=x;} 
  void set_z(float x){m_z=x;} 
  void set_nTracks(unsigned int x){m_nTracks=x;} 
  void set_chi2(float x){m_chi2=x;} 
  void set_ndof(float x){m_ndof=x;}

 private:
  float m_x;
  float m_y;
  float m_z;
  unsigned int m_nTracks;
  float m_chi2;
  float m_ndof;
};
