#ifndef GenInfo_H
#define GenInfo_H

class GenInfo{

 public:
  GenInfo(){
    m_binningValues.clear();
    m_weights.clear();
    m_alphaQCD=0;
    m_alphaQED=0;
    m_qScale=0;
    m_pdf_id1=0;
    m_pdf_id2=0;
    m_pdf_x1=0;
    m_pdf_x2=0;
    m_pdf_xPDF1=0;
    m_pdf_xPDF2=0;
    m_pdf_scalePDF=0;
    m_pileup_NumInteractions_intime=0; 
    m_pileup_NumInteractions_ootbefore=0; 
    m_pileup_NumInteractions_ootafter=0;
    m_pileup_TrueNumInteractions=0; 
  }

  ~GenInfo(){
  };

  std::vector<float> binningValues() const{return m_binningValues;}
  std::vector<float> weights() const{return m_weights;}
  float alphaQCD() const{return m_alphaQCD;}
  float alphaQED() const{return m_alphaQED;}
  float qScale() const{return m_qScale;}
  int pdf_id1() const{return m_pdf_id1;}
  int pdf_id2() const{return m_pdf_id2;}
  float pdf_x1() const{return m_pdf_x1;}
  float pdf_x2() const{return m_pdf_x2;}
  float pdf_xPDF1() const{return m_pdf_xPDF1;}
  float pdf_xPDF2() const{return m_pdf_xPDF2;}
  float pdf_scalePDF() const{return m_pdf_scalePDF;}
  int pileup_NumInteractions_intime() const{return m_pileup_NumInteractions_intime;} 
  int pileup_NumInteractions_ootbefore() const{return m_pileup_NumInteractions_ootbefore;} 
  int pileup_NumInteractions_ootafter() const{return m_pileup_NumInteractions_ootafter;}
  float pileup_TrueNumInteractions() const{return m_pileup_TrueNumInteractions;}  

  void add_binningValue(float x){m_binningValues.push_back(x);}
  void add_weight(float x){m_weights.push_back(x);}
  void clear_binningValues(){m_binningValues.clear();}
  void clear_weights(){m_weights.clear();}
  void set_alphaQCD(float x){m_alphaQCD=x;}
  void set_alphaQED(float x){m_alphaQED=x;}
  void set_qScale(float x){m_qScale=x;}
  void set_pdf_id1(int x){m_pdf_id1=x;}
  void set_pdf_id2(int x){m_pdf_id2=x;}
  void set_pdf_x1(float x){m_pdf_x1=x;}
  void set_pdf_x2(float x){m_pdf_x2=x;}
  void set_pdf_xPDF1(float x){m_pdf_xPDF1=x;}
  void set_pdf_xPDF2(float x){m_pdf_xPDF2=x;}
  void set_pdf_scalePDF(float x){m_pdf_scalePDF=x;}
  void set_pileup_NumInteractions_intime(int x){m_pileup_NumInteractions_intime=x;} 
  void set_pileup_NumInteractions_ootbefore(int x){m_pileup_NumInteractions_ootbefore=x;} 
  void set_pileup_NumInteractions_ootafter(int x){m_pileup_NumInteractions_ootafter=x;}
  void set_pileup_TrueNumInteractions(float x){m_pileup_TrueNumInteractions=x;}  


 private:
  std::vector<float> m_binningValues;
  std::vector<float> m_weights;
  float m_alphaQCD;
  float m_alphaQED;
  float m_qScale;
  int m_pdf_id1;
  int m_pdf_id2;
  float m_pdf_x1;
  float m_pdf_x2;
  float m_pdf_xPDF1;
  float m_pdf_xPDF2;
  float m_pdf_scalePDF;
  int m_pileup_NumInteractions_intime; 
  int m_pileup_NumInteractions_ootbefore; 
  int m_pileup_NumInteractions_ootafter;
  float m_pileup_TrueNumInteractions;  //poisson mean

};

#endif
