#ifndef GenInfo_H
#define GenInfo_H

class GenInfo{
 public:
     
  GenInfo(){
    m_alphaQCD = 0;
    m_alphaQED = 0;
    m_qScale = 0;
    m_pdf_id1 = 0;
    m_pdf_id2 = 0;
    m_pdf_x1 = 0;
    m_pdf_x2 = 0;
    m_pdf_xPDF1 = 0;
    m_pdf_xPDF2 = 0;
    m_pdf_scalePDF = 0;
    m_pileup_NumInteractions_intime = 0; 
    m_pileup_NumInteractions_ootbefore = 0; 
    m_pileup_NumInteractions_ootafter = 0;
    m_pileup_TrueNumInteractions = 0;  //poisson mean
    m_originalXWGTUP = 0;
    m_PU_pT_hat_max = 0;
  }

  const std::vector<float> & binningValues() const{return m_binningValues;}
  //nominal generator weights, for instance positive and negative weights from aMC@NLO
  const std::vector<float> & weights() const{return m_weights;}
  //list of weights for systematic variations like PDF and Q^2
  const std::vector<float> & systweights() const{return m_systweights;}
  //max pt-hat of the PU
  float PU_pT_hat_max() const{return m_PU_pT_hat_max;}

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
  float originalXWGTUP() const{return m_originalXWGTUP;}

  void add_binningValue(float x){m_binningValues.push_back(x);}
  void add_weight(float x){m_weights.push_back(x);}
  void add_systweight(float x){m_systweights.push_back(x);}
  void clear_binningValues(){m_binningValues.clear();}
  void clear_weights(){m_weights.clear();}
  void clear_systweights(){m_systweights.clear();}
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
  void set_originalXWGTUP(float x){m_originalXWGTUP=x;}
  void set_PU_pT_hat_max(float x){m_PU_pT_hat_max=x;}

 private:
  std::vector<float> m_binningValues;
  std::vector<float> m_weights;
  std::vector<float> m_systweights; 

  float m_originalXWGTUP;
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
  float m_PU_pT_hat_max;
};

#endif
