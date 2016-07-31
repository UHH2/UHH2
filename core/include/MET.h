#pragma once

class MET{
public:
    
  MET(){
      m_pt = 0;
      m_phi = 0;
      m_mEtSig = 0;
      m_uncorr_pt = 0;
      m_uncorr_phi = 0;
      m_shiftedPx_JetEnUp = 0;
      m_shiftedPx_JetEnDown = 0;
      m_shiftedPx_JetResUp = 0;
      m_shiftedPx_JetResDown = 0;
      m_shiftedPx_UnclusteredEnDown = 0;
      m_shiftedPx_UnclusteredEnUp = 0;
      m_shiftedPx_ElectronEnUp = 0;
      m_shiftedPx_ElectronEnDown = 0;
      m_shiftedPx_TauEnUp = 0;
      m_shiftedPx_TauEnDown = 0;
      m_shiftedPx_MuonEnDown = 0;
      m_shiftedPx_MuonEnUp = 0;
      m_shiftedPy_JetEnUp = 0;
      m_shiftedPy_JetEnDown = 0;
      m_shiftedPy_JetResUp = 0;
      m_shiftedPy_JetResDown = 0;
      m_shiftedPy_UnclusteredEnDown = 0;
      m_shiftedPy_UnclusteredEnUp = 0;
      m_shiftedPy_ElectronEnUp = 0;
      m_shiftedPy_ElectronEnDown = 0;
      m_shiftedPy_TauEnUp = 0;
      m_shiftedPy_TauEnDown = 0;
      m_shiftedPy_MuonEnDown = 0;
      m_shiftedPy_MuonEnUp = 0;
      /* m_corr_x = 0; */
      /* m_corr_y = 0; */
      /* //  m_corr_SumEt = 0; */
  }

  /// transverse momentum
   float pt() const{return m_pt;}
   /// phi
   float phi() const{return m_phi;}
   /// transverse momentum significance
   float mEtSig() const{return m_mEtSig;}

   /// uncorrected transverse momentum
   float uncorr_pt() const{return m_uncorr_pt;}
   /// uncorrected phi
   float uncorr_phi() const{return m_uncorr_phi;}
   /* /// corr in Px */
   /* float corr_x() const{return m_corr_x;} */
   /* /// corr in Py */
   /* float corr_y() const{return m_corr_y;} */
   /// corr in SumEt
   //   float corr_SumEt() const{return m_corr_SumEt;}
   
   float shiftedPx_JetEnUp() const{return m_shiftedPx_JetEnUp;}
   
   float shiftedPx_JetEnDown() const{return m_shiftedPx_JetEnDown;} 

   float shiftedPx_JetResUp() const{return m_shiftedPx_JetResUp;}
   
   float shiftedPx_JetResDown() const{return m_shiftedPx_JetResDown;}

   float shiftedPx_UnclusteredEnDown() const{return m_shiftedPx_UnclusteredEnDown;}

   float shiftedPx_UnclusteredEnUp() const{return m_shiftedPx_UnclusteredEnUp;}

   float shiftedPx_ElectronEnUp() const{return m_shiftedPx_ElectronEnUp;}

   float shiftedPx_ElectronEnDown() const{return m_shiftedPx_ElectronEnDown;}

   float shiftedPx_TauEnUp() const{return m_shiftedPx_TauEnUp;}

   float shiftedPx_TauEnDown() const{return m_shiftedPx_TauEnDown;}
   
   float shiftedPx_MuonEnDown() const{return m_shiftedPx_MuonEnDown;}
   
   float shiftedPy_JetEnUp() const{return m_shiftedPy_JetEnUp;}
   
   float shiftedPy_JetEnDown() const{return m_shiftedPy_JetEnDown;} 

   float shiftedPy_JetResUp() const{return m_shiftedPy_JetResUp;}
   
   float shiftedPy_JetResDown() const{return m_shiftedPy_JetResDown;}

   float shiftedPy_UnclusteredEnDown() const{return m_shiftedPy_UnclusteredEnDown;}

   float shiftedPy_UnclusteredEnUp() const{return m_shiftedPy_UnclusteredEnUp;}

   float shiftedPy_ElectronEnUp() const{return m_shiftedPy_ElectronEnUp;}

   float shiftedPy_ElectronEnDown() const{return m_shiftedPy_ElectronEnDown;}

   float shiftedPy_TauEnUp() const{return m_shiftedPy_TauEnUp;}

   float shiftedPy_TauEnDown() const{return m_shiftedPy_TauEnDown;}
   
   float shiftedPy_MuonEnDown() const{return m_shiftedPy_MuonEnDown;}

   float shiftedPy_MuonEnUp() const{return m_shiftedPy_MuonEnUp;}
   
   
   /// set transverse momentum
   void set_pt(float pt){m_pt=pt;}  
   /// set phi
   void set_phi(float phi){m_phi=phi;}
   /// set transverse momentum significance
   void set_mEtSig(float mEtSig){m_mEtSig=mEtSig;}

   /// set uncorrected transverse momentum
   void set_uncorr_pt(float pt){m_uncorr_pt=pt;}  
   /// set uncorrected phi
   void set_uncorr_phi(float phi){m_uncorr_phi=phi;}

   /* /// set corr in Px, Py, SumEt */
   /* void set_corr_x(float x){m_corr_x=x;} */
   /* void set_corr_y(float x){m_corr_y=x;} */
   /* //   void set_corr_SumEt(float x)(m_corr_SumEt=x;} */

   void set_shiftedPx_JetEnUp(float shiftedPx_JetEnUp) {m_shiftedPx_JetEnUp = shiftedPx_JetEnUp;}
   
   void set_shiftedPx_JetEnDown(float shiftedPx_JetEnDown) {m_shiftedPx_JetEnDown = shiftedPx_JetEnDown;}
   
   void set_shiftedPx_JetResUp(float shiftedPx_JetResUp) {m_shiftedPx_JetResUp = shiftedPx_JetResUp;}

   void set_shiftedPx_JetResDown(float shiftedPx_JetResDown) {m_shiftedPx_JetResDown = shiftedPx_JetResDown;}

   void set_shiftedPx_UnclusteredEnDown(float shiftedPx_UnclusteredEnDown) {m_shiftedPx_UnclusteredEnDown = shiftedPx_UnclusteredEnDown;}

   void set_shiftedPx_UnclusteredEnUp(float shiftedPx_UnclusteredEnUp) {m_shiftedPx_UnclusteredEnUp = shiftedPx_UnclusteredEnUp;}

   void set_shiftedPx_ElectronEnUp(float shiftedPx_ElectronEnUp) {m_shiftedPx_ElectronEnUp = shiftedPx_ElectronEnUp;}

   void set_shiftedPx_ElectronEnDown(float shiftedPx_ElectronEnDown) {m_shiftedPx_ElectronEnDown = shiftedPx_ElectronEnDown;}

   void set_shiftedPx_TauEnUp(float shiftedPx_TauEnUp) {m_shiftedPx_TauEnUp = shiftedPx_TauEnUp;}

   void set_shiftedPx_TauEnDown(float shiftedPx_TauEnDown) {m_shiftedPx_TauEnDown = shiftedPx_TauEnDown;}

   void set_shiftedPx_MuonEnDown(float shiftedPx_MuonEnDown) {m_shiftedPx_MuonEnDown = shiftedPx_MuonEnDown;}

   void set_shiftedPx_MuonEnUp(float shiftedPx_MuonEnUp) {m_shiftedPx_MuonEnUp = shiftedPx_MuonEnUp;}

   void set_shiftedPy_JetEnUp(float shiftedPy_JetEnUp) {m_shiftedPy_JetEnUp = shiftedPy_JetEnUp;}
   
   void set_shiftedPy_JetEnDown(float shiftedPy_JetEnDown) {m_shiftedPy_JetEnDown = shiftedPy_JetEnDown;}
   
   void set_shiftedPy_JetResUp(float shiftedPy_JetResUp) {m_shiftedPy_JetResUp = shiftedPy_JetResUp;}

   void set_shiftedPy_JetResDown(float shiftedPy_JetResDown) {m_shiftedPy_JetResDown = shiftedPy_JetResDown;}

   void set_shiftedPy_UnclusteredEnDown(float shiftedPy_UnclusteredEnDown) {m_shiftedPy_UnclusteredEnDown = shiftedPy_UnclusteredEnDown;}

   void set_shiftedPy_UnclusteredEnUp(float shiftedPy_UnclusteredEnUp) {m_shiftedPy_UnclusteredEnUp = shiftedPy_UnclusteredEnUp;}

   void set_shiftedPy_ElectronEnUp(float shiftedPy_ElectronEnUp) {m_shiftedPy_ElectronEnUp = shiftedPy_ElectronEnUp;}

   void set_shiftedPy_ElectronEnDown(float shiftedPy_ElectronEnDown) {m_shiftedPy_ElectronEnDown = shiftedPy_ElectronEnDown;}

   void set_shiftedPy_TauEnUp(float shiftedPy_TauEnUp) {m_shiftedPy_TauEnUp = shiftedPy_TauEnUp;}

   void set_shiftedPy_TauEnDown(float shiftedPy_TauEnDown) {m_shiftedPy_TauEnDown = shiftedPy_TauEnDown;}

   void set_shiftedPy_MuonEnDown(float shiftedPy_MuonEnDown) {m_shiftedPy_MuonEnDown = shiftedPy_MuonEnDown;}

   void set_shiftedPy_MuonEnUp(float shiftedPy_MuonEnUp) {m_shiftedPy_MuonEnUp = shiftedPy_MuonEnUp;}

   /// convert missing transverse energy into 4-vector
   LorentzVector v4(){
      LorentzVector met(0,0,0,0);
      met.SetPt(m_pt);
      met.SetPhi(m_phi);
      return met;
   }

   /// convert missing transverse energy into 4-vector
   LorentzVector uncorr_v4(){
      LorentzVector met(0,0,0,0);
      met.SetPt(m_uncorr_pt);
      met.SetPhi(m_uncorr_phi);
      return met;
   }
   
private:
   float m_pt;
   float m_phi;
   float m_mEtSig;
   float m_shiftedPx_JetEnUp;
   float m_shiftedPx_JetEnDown;
   float m_shiftedPx_JetResUp; 
   float m_shiftedPx_JetResDown; 
   float m_shiftedPx_UnclusteredEnUp;
   float m_shiftedPx_UnclusteredEnDown;
   float m_shiftedPx_ElectronEnUp;
   float m_shiftedPx_ElectronEnDown;
   float m_shiftedPx_TauEnUp;
   float m_shiftedPx_TauEnDown;
   float m_shiftedPx_MuonEnDown;
   float m_shiftedPx_MuonEnUp;
   float m_shiftedPy_JetEnUp;
   float m_shiftedPy_JetEnDown;
   float m_shiftedPy_JetResUp; 
   float m_shiftedPy_JetResDown; 
   float m_shiftedPy_UnclusteredEnUp;
   float m_shiftedPy_UnclusteredEnDown;
   float m_shiftedPy_ElectronEnUp;
   float m_shiftedPy_ElectronEnDown;
   float m_shiftedPy_TauEnUp;
   float m_shiftedPy_TauEnDown;
   float m_shiftedPy_MuonEnDown;
   float m_shiftedPy_MuonEnUp;
   float m_uncorr_pt;
   float m_uncorr_phi;
   /* float m_corr_x; */
   /* float m_corr_y; */
   //   float m_corr_SumEt;
};
