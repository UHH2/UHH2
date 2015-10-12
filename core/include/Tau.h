#pragma once

#include "Particle.h"
#include "Tags.h"

#include <stdint.h>


class Tau: public Particle{
public:
    
  enum tag { /* for future use (more b-taggers, etc.) */ };
  
  
  // see https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuidePFTauID
  // "Tau ID 2014 (Preparation for Run II)"
  // for those which are "binary" for most variables below. The names correspond to the strings used in PAT, not necessarily to
  // the names used in the twiki page above (e.g. 'by...Rejection' has the name 'against...' here). See at the end of this file for a list
  // of variable names in miniAOD.
  enum bool_id {
    againstElectronLoose = 0, againstElectronMedium=0, againstElectronTight=0,
      againstElectronVLooseMVA5, againstElectronLooseMVA5, againstElectronMediumMVA5, againstElectronTightMVA5, againstElectronVTightMVA5, 
      againstMuonLoose =0 , againstMuonMedium =0 , againstMuonTight=0, againstMuonLoose3, againstMuonTight3,
      againstMuonLooseMVA, againstMuonMediumMVA, againstMuonTightMVA, 
      decayModeFinding, 
      byLooseCombinedIsolationDeltaBetaCorr3Hits, byMediumCombinedIsolationDeltaBetaCorr3Hits, byTightCombinedIsolationDeltaBetaCorr3Hits /* = 19 */,
      byVLooseIsolationMVA3newDMwoLT, byLooseIsolationMVA3newDMwoLT, byMediumIsolationMVA3newDMwoLT, byTightIsolationMVA3newDMwoLT, byVTightIsolationMVA3newDMwoLT, byVVTightIsolationMVA3newDMwoLT,
      byVLooseIsolationMVA3newDMwLT, byLooseIsolationMVA3newDMwLT, byMediumIsolationMVA3newDMwLT, byTightIsolationMVA3newDMwLT, byVTightIsolationMVA3newDMwLT, byVVTightIsolationMVA3newDMwLT, decayModeFindingNewDMs /* = 32 */
  };
  
  bool get_bool(bool_id i) const {
      return (id_bits & (uint64_t(1) << static_cast<uint64_t>(i)));
  }
  
  void set_bool(bool_id i, bool value) {
      if(value){
          id_bits |= uint64_t(1) << static_cast<uint64_t>(i);
      }
      else{
          id_bits &= ~(uint64_t(1) << static_cast<uint64_t>(i));
      }
  }
  
  // some non-bool values ('raw'):
  float againstElectronMVA5raw() const { return m_againstElectronMVA5raw; }
  float againstMuonMVAraw() const { return m_againstMuonMVAraw;}
  float byCombinedIsolationDeltaBetaCorrRaw3Hits() const { return m_byCombinedIsolationDeltaBetaCorrRaw3Hits; }
  float byIsolationMVA3newDMwLTraw() const { return m_byIsolationMVA3newDMwLTraw; }
  float byIsolationMVA3newDMwoLTraw() const { return m_byIsolationMVA3newDMwoLTraw; }
  
  float chargedIsoPtSum() const { return m_chargedIsoPtSum; }
  float neutralIsoPtSum() const { return m_neutralIsoPtSum; }
  float puCorrPtSum() const { return m_puCorrPtSum; }

  int decayMode() const { return m_decayMode; }
  
  
  void set_againstElectronMVA5raw(float value) { m_againstElectronMVA5raw = value;}
  void set_againstMuonMVAraw(float value) { m_againstMuonMVAraw = value; }
  void set_byCombinedIsolationDeltaBetaCorrRaw3Hits(float value){ m_byCombinedIsolationDeltaBetaCorrRaw3Hits = value; }
  void set_byIsolationMVA3newDMwoLTraw(float value) { m_byIsolationMVA3newDMwoLTraw = value; }
  void set_byIsolationMVA3newDMwLTraw(float value) { m_byIsolationMVA3newDMwLTraw = value; }

  void set_chargedIsoPtSum(float value) { m_chargedIsoPtSum = value; }
  void set_neutralIsoPtSum(float value) { m_neutralIsoPtSum = value; }
  void set_puCorrPtSum(float value) { m_puCorrPtSum = value; }
  
  void set_decayMode(int value){ m_decayMode = value; }

  float get_tag(tag t) const { return tags.get_tag(static_cast<int>(t)); }
  void set_tag(tag t, float value) { return tags.set_tag(static_cast<int>(t), value); }
  
  
  Tau(){
      id_bits = 0;
      m_againstElectronMVA5raw = 0;
      m_againstMuonMVAraw = 0;
      m_byCombinedIsolationDeltaBetaCorrRaw3Hits = 0;
      m_byIsolationMVA3newDMwoLTraw = 0;
      m_byIsolationMVA3newDMwLTraw = 0;
      m_decayMode = 0;
  }

private:
    // save the bool-like id variables in an uint64_t as single bits;
    // the bit positions correspond to the int-converted values of the enum bool_id.
    uint64_t id_bits;
    
    float m_againstElectronMVA5raw;
    float m_againstMuonMVAraw;
    float m_byCombinedIsolationDeltaBetaCorrRaw3Hits;
    float m_byIsolationMVA3newDMwoLTraw;
    float m_byIsolationMVA3newDMwLTraw;
    float m_chargedIsoPtSum;
    float m_neutralIsoPtSum;
    float m_puCorrPtSum;
    
    int m_decayMode;
  
    Tags tags;
};



/*
 tau id variables available on PHYS14 miniAOD:

 'againstElectronLoose' 'againstElectronLooseMVA5' 'againstElectronMVA5category' 'againstElectronMVA5raw' 'againstElectronMedium' 'againstElectronMediumMVA5' 'againstElectronTight' 'againstElectronTightMVA5' 'againstElectronVLooseMVA5' 'againstElectronVTightMVA5' 'againstMuonLoose' 'againstMuonLoose2' 'againstMuonLoose3' 'againstMuonLooseMVA' 'againstMuonMVAraw' 'againstMuonMedium' 'againstMuonMedium2' 'againstMuonMediumMVA' 'againstMuonTight' 'againstMuonTight2' 'againstMuonTight3' 'againstMuonTightMVA' 'byCombinedIsolationDeltaBetaCorrRaw3Hits' 'byIsolationMVA3newDMwLTraw' 'byIsolationMVA3newDMwoLTraw' 'byIsolationMVA3oldDMwLTraw' 'byIsolationMVA3oldDMwoLTraw' 'byLooseCombinedIsolationDeltaBetaCorr3Hits' 'byLooseIsolationMVA3newDMwLT' 'byLooseIsolationMVA3newDMwoLT' 'byLooseIsolationMVA3oldDMwLT' 'byLooseIsolationMVA3oldDMwoLT' 'byMediumCombinedIsolationDeltaBetaCorr3Hits' 'byMediumIsolationMVA3newDMwLT' 'byMediumIsolationMVA3newDMwoLT' 'byMediumIsolationMVA3oldDMwLT' 'byMediumIsolationMVA3oldDMwoLT' 'byTightCombinedIsolationDeltaBetaCorr3Hits' 'byTightIsolationMVA3newDMwLT' 'byTightIsolationMVA3newDMwoLT' 'byTightIsolationMVA3oldDMwLT' 'byTightIsolationMVA3oldDMwoLT' 'byVLooseIsolationMVA3newDMwLT' 'byVLooseIsolationMVA3newDMwoLT' 'byVLooseIsolationMVA3oldDMwLT' 'byVLooseIsolationMVA3oldDMwoLT' 'byVTightIsolationMVA3newDMwLT' 'byVTightIsolationMVA3newDMwoLT' 'byVTightIsolationMVA3oldDMwLT' 'byVTightIsolationMVA3oldDMwoLT' 'byVVTightIsolationMVA3newDMwLT' 'byVVTightIsolationMVA3newDMwoLT' 'byVVTightIsolationMVA3oldDMwLT' 'byVVTightIsolationMVA3oldDMwoLT' 'chargedIsoPtSum' 'decayModeFinding' 'decayModeFindingNewDMs' 'neutralIsoPtSum' 'puCorrPtSum'
 */

