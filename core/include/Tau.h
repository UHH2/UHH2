#pragma once

#include "Particle.h"
#include "Tags.h"


class Tau: public Particle{
public:
    
  enum tag { /* for future use (more b-taggers, etc.) */ };
  
  Tau(){
      m_decayModeFinding = false; 
      m_decayModeFindingNewDMs = false;
      m_againstElectronLoose = false;
      m_againstElectronLooseMVA5 = false;
      m_againstElectronMVA5category = false;
      m_againstElectronMVA5raw = false;
      m_againstElectronMedium  = false;
      m_againstElectronMediumMVA5 = false;
      m_againstElectronTight  = false;
      m_againstElectronTightMVA5 = false; 
      m_againstElectronVLooseMVA5  = false;
      m_againstElectronVTightMVA5 = false;
      m_againstMuonLoose  = false;
      m_againstMuonLoose2  = false;
      m_againstMuonLoose3 = false;
      m_againstMuonLooseMVA = false; 
      m_againstMuonMVAraw  = false;
      m_againstMuonMedium  = false;
      m_againstMuonMedium2  = false;
      m_againstMuonMediumMVA  = false;
      m_againstMuonTight  = false;
      m_againstMuonTight2  = false;
      m_againstMuonTight3 = false;
      m_againstMuonTightMVA = false;
      m_byCombinedIsolationDeltaBetaCorrRaw3Hits = false;
      m_byIsolationMVA3newDMwLTraw  = false;
      m_byIsolationMVA3newDMwoLTraw  = false;
      m_byIsolationMVA3oldDMwLTraw  = false;
      m_byIsolationMVA3oldDMwoLTraw  = false;
      m_byLooseCombinedIsolationDeltaBetaCorr3Hits = false;
      m_byLooseIsolationMVA3newDMwLT  = false;
      m_byLooseIsolationMVA3newDMwoLT  = false;
      m_byLooseIsolationMVA3oldDMwLT  = false;
      m_byLooseIsolationMVA3oldDMwoLT = false;
      m_byMediumCombinedIsolationDeltaBetaCorr3Hits = false;
      m_byMediumIsolationMVA3newDMwLT  = false;
      m_byMediumIsolationMVA3newDMwoLT  = false;
      m_byMediumIsolationMVA3oldDMwLT  = false;
      m_byMediumIsolationMVA3oldDMwoLT = false;
      m_byTightCombinedIsolationDeltaBetaCorr3Hits = false;
      m_byTightIsolationMVA3newDMwLT  = false;
      m_byTightIsolationMVA3newDMwoLT = false;
      m_byTightIsolationMVA3oldDMwLT = false;
      m_byTightIsolationMVA3oldDMwoLT  = false;
      m_byVLooseIsolationMVA3newDMwLT  = false;
      m_byVLooseIsolationMVA3newDMwoLT  = false;
      m_byVLooseIsolationMVA3oldDMwLT  = false;
      m_byVLooseIsolationMVA3oldDMwoLT  = false;
      m_byVTightIsolationMVA3newDMwLT  = false;
      m_byVTightIsolationMVA3newDMwoLT = false;
      m_byVTightIsolationMVA3oldDMwLT  = false;
      m_byVTightIsolationMVA3oldDMwoLT = false;
      m_byVVTightIsolationMVA3newDMwLT = false;
      m_byVVTightIsolationMVA3newDMwoLT  = false;
      m_byVVTightIsolationMVA3oldDMwLT  = false;
      m_byVVTightIsolationMVA3oldDMwoLT = false;
      m_decayMode = 0;
  }

  bool decayModeFinding() const{return m_decayModeFinding;} 
  bool decayModeFindingNewDMs() const{return m_decayModeFindingNewDMs;} 

  bool  againstElectronLoose() const{return m_againstElectronLoose;}
  bool  againstElectronLooseMVA5() const{return m_againstElectronLooseMVA5;}
  bool  againstElectronMVA5category() const{return m_againstElectronMVA5category;}
  bool  againstElectronMVA5raw() const{return m_againstElectronMVA5raw;}
  bool  againstElectronMedium() const{return m_againstElectronMedium;}
  bool  againstElectronMediumMVA5() const{return m_againstElectronMediumMVA5;}
  bool  againstElectronTight() const{return m_againstElectronTight;}
  bool  againstElectronTightMVA5() const{return m_againstElectronTightMVA5;}
  bool  againstElectronVLooseMVA5() const{return m_againstElectronVLooseMVA5;}
  bool  againstElectronVTightMVA5() const{return m_againstElectronVTightMVA5;}
  bool  againstMuonLoose() const{return m_againstMuonLoose;}
  bool  againstMuonLoose2() const{return m_againstMuonLoose2;}
  bool  againstMuonLoose3() const{return m_againstMuonLoose3;}
  bool  againstMuonLooseMVA() const{return m_againstMuonLooseMVA;}
  bool  againstMuonMVAraw() const{return m_againstMuonMVAraw;}
  bool  againstMuonMedium() const{return m_againstMuonMedium;}
  bool  againstMuonMedium2() const{return m_againstMuonMedium2;}
  bool  againstMuonMediumMVA() const{return m_againstMuonMediumMVA;}
  bool  againstMuonTight() const{return m_againstMuonTight;}
  bool  againstMuonTight2() const{return m_againstMuonTight2;}
  bool  againstMuonTight3() const{return m_againstMuonTight3;}
  bool  againstMuonTightMVA() const{return m_againstMuonTightMVA;}
  bool  byCombinedIsolationDeltaBetaCorrRaw3Hits() const{return m_byCombinedIsolationDeltaBetaCorrRaw3Hits;}
  bool  byIsolationMVA3newDMwLTraw() const{return m_byIsolationMVA3newDMwLTraw;}
  bool  byIsolationMVA3newDMwoLTraw() const{return m_byIsolationMVA3newDMwoLTraw;}
  bool  byIsolationMVA3oldDMwLTraw() const{return m_byIsolationMVA3oldDMwLTraw;}
  bool  byIsolationMVA3oldDMwoLTraw() const{return m_byIsolationMVA3oldDMwoLTraw;}
  bool  byLooseCombinedIsolationDeltaBetaCorr3Hits() const{return m_byLooseCombinedIsolationDeltaBetaCorr3Hits;}
  bool  byLooseIsolationMVA3newDMwLT() const{return m_byLooseIsolationMVA3newDMwLT;}
  bool  byLooseIsolationMVA3newDMwoLT() const{return m_byLooseIsolationMVA3newDMwoLT;}
  bool  byLooseIsolationMVA3oldDMwLT() const{return m_byLooseIsolationMVA3oldDMwLT;}
  bool  byLooseIsolationMVA3oldDMwoLT() const{return m_byLooseIsolationMVA3oldDMwoLT;}
  bool  byMediumCombinedIsolationDeltaBetaCorr3Hits() const{return m_byMediumCombinedIsolationDeltaBetaCorr3Hits;}
  bool  byMediumIsolationMVA3newDMwLT() const{return m_byMediumIsolationMVA3newDMwLT;}
  bool  byMediumIsolationMVA3newDMwoLT() const{return m_byMediumIsolationMVA3newDMwoLT;}
  bool  byMediumIsolationMVA3oldDMwLT() const{return m_byMediumIsolationMVA3oldDMwLT;}
  bool  byMediumIsolationMVA3oldDMwoLT() const{return m_byMediumIsolationMVA3oldDMwoLT;}
  bool  byTightCombinedIsolationDeltaBetaCorr3Hits() const{return m_byTightCombinedIsolationDeltaBetaCorr3Hits;}
  bool  byTightIsolationMVA3newDMwLT() const{return m_byTightIsolationMVA3newDMwLT;}
  bool  byTightIsolationMVA3newDMwoLT() const{return m_byTightIsolationMVA3newDMwoLT;}
  bool  byTightIsolationMVA3oldDMwLT() const{return m_byTightIsolationMVA3oldDMwLT;}
  bool  byTightIsolationMVA3oldDMwoLT() const{return m_byTightIsolationMVA3oldDMwoLT;}
  bool  byVLooseIsolationMVA3newDMwLT() const{return m_byVLooseIsolationMVA3newDMwLT;}
  bool  byVLooseIsolationMVA3newDMwoLT() const{return m_byVLooseIsolationMVA3newDMwoLT;}
  bool  byVLooseIsolationMVA3oldDMwLT() const{return m_byVLooseIsolationMVA3oldDMwLT;}
  bool  byVLooseIsolationMVA3oldDMwoLT() const{return m_byVLooseIsolationMVA3oldDMwoLT;}
  bool  byVTightIsolationMVA3newDMwLT() const{return m_byVTightIsolationMVA3newDMwLT;}
  bool  byVTightIsolationMVA3newDMwoLT() const{return m_byVTightIsolationMVA3newDMwoLT;}
  bool  byVTightIsolationMVA3oldDMwLT() const{return m_byVTightIsolationMVA3oldDMwLT;}
  bool  byVTightIsolationMVA3oldDMwoLT() const{return m_byVTightIsolationMVA3oldDMwoLT;}
  bool  byVVTightIsolationMVA3newDMwLT() const{return m_byVVTightIsolationMVA3newDMwLT;}
  bool  byVVTightIsolationMVA3newDMwoLT() const{return m_byVVTightIsolationMVA3newDMwoLT;}
  bool  byVVTightIsolationMVA3oldDMwLT() const{return m_byVVTightIsolationMVA3oldDMwLT;}
  bool  byVVTightIsolationMVA3oldDMwoLT() const{return m_byVVTightIsolationMVA3oldDMwoLT;}
  float get_tag(tag t) const { return tags.get_tag(static_cast<int>(t)); }
    
  int decayMode() const{return m_decayMode;}

  void set_decayModeFinding(bool x){m_decayModeFinding=x;} 
  void set_decayModeFindingNewDMs(bool x){m_decayModeFindingNewDMs=x;} 

  void set_againstElectronLoose(bool x){m_againstElectronLoose = x;}
  void set_againstElectronLooseMVA5(bool x){m_againstElectronLooseMVA5 = x;}
  void set_againstElectronMVA5category(bool x){m_againstElectronMVA5category = x;}
  void set_againstElectronMVA5raw(bool x){m_againstElectronMVA5raw = x;}
  void set_againstElectronMedium(bool x){m_againstElectronMedium = x;}
  void set_againstElectronMediumMVA5(bool x){m_againstElectronMediumMVA5 = x;}
  void set_againstElectronTight(bool x){m_againstElectronTight = x;}
  void set_againstElectronTightMVA5(bool x){m_againstElectronTightMVA5 = x;}
  void set_againstElectronVLooseMVA5(bool x){m_againstElectronVLooseMVA5 = x;}
  void set_againstElectronVTightMVA5(bool x){m_againstElectronVTightMVA5 = x;}
  void set_againstMuonLoose(bool x){m_againstMuonLoose = x;}
  void set_againstMuonLoose2(bool x){m_againstMuonLoose2 = x;}
  void set_againstMuonLoose3(bool x){m_againstMuonLoose3 = x;}
  void set_againstMuonLooseMVA(bool x){m_againstMuonLooseMVA = x;}
  void set_againstMuonMVAraw(bool x){m_againstMuonMVAraw = x;}
  void set_againstMuonMedium(bool x){m_againstMuonMedium = x;}
  void set_againstMuonMedium2(bool x){m_againstMuonMedium2 = x;}
  void set_againstMuonMediumMVA(bool x){m_againstMuonMediumMVA = x;}
  void set_againstMuonTight(bool x){m_againstMuonTight = x;}
  void set_againstMuonTight2(bool x){m_againstMuonTight2 = x;}
  void set_againstMuonTight3(bool x){m_againstMuonTight3 = x;}
  void set_againstMuonTightMVA(bool x){m_againstMuonTightMVA = x;}
  void set_byCombinedIsolationDeltaBetaCorrRaw3Hits(bool x){m_byCombinedIsolationDeltaBetaCorrRaw3Hits = x;}
  void set_byIsolationMVA3newDMwLTraw(bool x){m_byIsolationMVA3newDMwLTraw = x;}
  void set_byIsolationMVA3newDMwoLTraw(bool x){m_byIsolationMVA3newDMwoLTraw = x;}
  void set_byIsolationMVA3oldDMwLTraw(bool x){m_byIsolationMVA3oldDMwLTraw = x;}
  void set_byIsolationMVA3oldDMwoLTraw(bool x){m_byIsolationMVA3oldDMwoLTraw = x;}
  void set_byLooseCombinedIsolationDeltaBetaCorr3Hits(bool x){m_byLooseCombinedIsolationDeltaBetaCorr3Hits = x;}
  void set_byLooseIsolationMVA3newDMwLT(bool x){m_byLooseIsolationMVA3newDMwLT = x;}
  void set_byLooseIsolationMVA3newDMwoLT(bool x){m_byLooseIsolationMVA3newDMwoLT = x;}
  void set_byLooseIsolationMVA3oldDMwLT(bool x){m_byLooseIsolationMVA3oldDMwLT = x;}
  void set_byLooseIsolationMVA3oldDMwoLT(bool x){m_byLooseIsolationMVA3oldDMwoLT = x;}
  void set_byMediumCombinedIsolationDeltaBetaCorr3Hits(bool x){m_byMediumCombinedIsolationDeltaBetaCorr3Hits = x;}
  void set_byMediumIsolationMVA3newDMwLT(bool x){m_byMediumIsolationMVA3newDMwLT = x;}
  void set_byMediumIsolationMVA3newDMwoLT(bool x){m_byMediumIsolationMVA3newDMwoLT = x;}
  void set_byMediumIsolationMVA3oldDMwLT(bool x){m_byMediumIsolationMVA3oldDMwLT = x;}
  void set_byMediumIsolationMVA3oldDMwoLT(bool x){m_byMediumIsolationMVA3oldDMwoLT = x;}
  void set_byTightCombinedIsolationDeltaBetaCorr3Hits(bool x){m_byTightCombinedIsolationDeltaBetaCorr3Hits = x;}
  void set_byTightIsolationMVA3newDMwLT(bool x){m_byTightIsolationMVA3newDMwLT = x;}
  void set_byTightIsolationMVA3newDMwoLT(bool x){m_byTightIsolationMVA3newDMwoLT = x;}
  void set_byTightIsolationMVA3oldDMwLT(bool x){m_byTightIsolationMVA3oldDMwLT = x;}
  void set_byTightIsolationMVA3oldDMwoLT(bool x){m_byTightIsolationMVA3oldDMwoLT = x;}
  void set_byVLooseIsolationMVA3newDMwLT(bool x){m_byVLooseIsolationMVA3newDMwLT = x;}
  void set_byVLooseIsolationMVA3newDMwoLT(bool x){m_byVLooseIsolationMVA3newDMwoLT = x;}
  void set_byVLooseIsolationMVA3oldDMwLT(bool x){m_byVLooseIsolationMVA3oldDMwLT = x;}
  void set_byVLooseIsolationMVA3oldDMwoLT(bool x){m_byVLooseIsolationMVA3oldDMwoLT = x;}
  void set_byVTightIsolationMVA3newDMwLT(bool x){m_byVTightIsolationMVA3newDMwLT = x;}
  void set_byVTightIsolationMVA3newDMwoLT(bool x){m_byVTightIsolationMVA3newDMwoLT = x;}
  void set_byVTightIsolationMVA3oldDMwLT(bool x){m_byVTightIsolationMVA3oldDMwLT = x;}
  void set_byVTightIsolationMVA3oldDMwoLT(bool x){m_byVTightIsolationMVA3oldDMwoLT = x;}
  void set_byVVTightIsolationMVA3newDMwLT(bool x){m_byVVTightIsolationMVA3newDMwLT = x;}
  void set_byVVTightIsolationMVA3newDMwoLT(bool x){m_byVVTightIsolationMVA3newDMwoLT = x;}
  void set_byVVTightIsolationMVA3oldDMwLT(bool x){m_byVVTightIsolationMVA3oldDMwLT = x;}
  void set_byVVTightIsolationMVA3oldDMwoLT(bool x){m_byVVTightIsolationMVA3oldDMwoLT = x;}
  
  void set_decayMode(int x){m_decayMode=x;}
  void set_tag(tag t, float value) { return tags.set_tag(static_cast<int>(t), value); }

 private:

  bool m_decayModeFinding; 
  bool m_decayModeFindingNewDMs;
  bool m_againstElectronLoose;
  bool m_againstElectronLooseMVA5;
  bool m_againstElectronMVA5category;
  bool m_againstElectronMVA5raw;
  bool m_againstElectronMedium;
  bool m_againstElectronMediumMVA5;
  bool m_againstElectronTight;
  bool m_againstElectronTightMVA5; 
  bool m_againstElectronVLooseMVA5;
  bool m_againstElectronVTightMVA5;
  bool m_againstMuonLoose;
  bool m_againstMuonLoose2;
  bool m_againstMuonLoose3;
  bool m_againstMuonLooseMVA; 
  bool m_againstMuonMVAraw;
  bool m_againstMuonMedium;
  bool m_againstMuonMedium2;
  bool m_againstMuonMediumMVA;
  bool m_againstMuonTight;
  bool m_againstMuonTight2;
  bool m_againstMuonTight3;
  bool m_againstMuonTightMVA;
  bool m_byCombinedIsolationDeltaBetaCorrRaw3Hits;
  bool m_byIsolationMVA3newDMwLTraw;
  bool m_byIsolationMVA3newDMwoLTraw;
  bool m_byIsolationMVA3oldDMwLTraw;
  bool m_byIsolationMVA3oldDMwoLTraw;
  bool m_byLooseCombinedIsolationDeltaBetaCorr3Hits;
  bool m_byLooseIsolationMVA3newDMwLT;
  bool m_byLooseIsolationMVA3newDMwoLT;
  bool m_byLooseIsolationMVA3oldDMwLT;
  bool m_byLooseIsolationMVA3oldDMwoLT;
  bool m_byMediumCombinedIsolationDeltaBetaCorr3Hits;
  bool m_byMediumIsolationMVA3newDMwLT;
  bool m_byMediumIsolationMVA3newDMwoLT;
  bool m_byMediumIsolationMVA3oldDMwLT;
  bool m_byMediumIsolationMVA3oldDMwoLT;
  bool m_byTightCombinedIsolationDeltaBetaCorr3Hits;
  bool m_byTightIsolationMVA3newDMwLT;
  bool m_byTightIsolationMVA3newDMwoLT;
  bool m_byTightIsolationMVA3oldDMwLT;
  bool m_byTightIsolationMVA3oldDMwoLT;
  bool m_byVLooseIsolationMVA3newDMwLT;
  bool m_byVLooseIsolationMVA3newDMwoLT;
  bool m_byVLooseIsolationMVA3oldDMwLT;
  bool m_byVLooseIsolationMVA3oldDMwoLT;
  bool m_byVTightIsolationMVA3newDMwLT;
  bool m_byVTightIsolationMVA3newDMwoLT;
  bool m_byVTightIsolationMVA3oldDMwLT;
  bool m_byVTightIsolationMVA3oldDMwoLT;
  bool m_byVVTightIsolationMVA3newDMwLT;
  bool m_byVVTightIsolationMVA3newDMwoLT;
  bool m_byVVTightIsolationMVA3oldDMwLT;
  bool m_byVVTightIsolationMVA3oldDMwoLT;

  int m_decayMode;
  
  Tags tags;
};

