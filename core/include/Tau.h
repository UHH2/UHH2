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
      againstElectronVLooseMVA6, againstElectronLooseMVA6, againstElectronMediumMVA6, againstElectronTightMVA6, againstElectronVTightMVA6, 
      againstMuonLoose3, againstMuonTight3,
      decayModeFinding, decayModeFindingNewDMs,
      byLooseCombinedIsolationDeltaBetaCorr3Hits, byMediumCombinedIsolationDeltaBetaCorr3Hits, byTightCombinedIsolationDeltaBetaCorr3Hits,
      byVLooseIsolationMVA3newDMwoLT, byLooseIsolationMVA3newDMwoLT, byMediumIsolationMVA3newDMwoLT, byTightIsolationMVA3newDMwoLT, byVTightIsolationMVA3newDMwoLT, byVVTightIsolationMVA3newDMwoLT,
      byVLooseIsolationMVArun2v1DBnewDMwLT, byLooseIsolationMVArun2v1DBnewDMwLT, byMediumIsolationMVArun2v1DBnewDMwLT, byTightIsolationMVArun2v1DBnewDMwLT, byVTightIsolationMVArun2v1DBnewDMwLT, byVVTightIsolationMVArun2v1DBnewDMwLT
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
  float byCombinedIsolationDeltaBetaCorrRaw3Hits() const { return m_byCombinedIsolationDeltaBetaCorrRaw3Hits; }
  float byIsolationMVArun2v1DBnewDMwLTraw() const { return m_byIsolationMVArun2v1DBnewDMwLTraw; }
  float byIsolationMVA3newDMwoLTraw() const { return m_byIsolationMVA3newDMwoLTraw; }
  
  float chargedIsoPtSum() const { return m_chargedIsoPtSum; }
  float neutralIsoPtSum() const { return m_neutralIsoPtSum; }
  float puCorrPtSum() const { return m_puCorrPtSum; }
  

  /* float byLoosePileupWeightedIsolation3Hits() const { return m_byLoosePileupWeightedIsolation3Hits; }  */
  /* float byMediumPileupWeightedIsolation3Hits() const { return m_byMediumPileupWeightedIsolation3Hits; }  */
  /* float byTightPileupWeightedIsolation3Hits() const { return m_byTightPileupWeightedIsolation3Hits; }  */
  /* float byPileupWeightedIsolationRaw3Hits() const { return m_byPileupWeightedIsolationRaw3Hits; }  */
  float neutralIsoPtSumWeight() const { return m_neutralIsoPtSumWeight; }
  float footprintCorrection() const { return m_footprintCorrection; }
  float photonPtSumOutsideSignalCone() const { return m_photonPtSumOutsideSignalCone; }
  

 

  int decayMode() const { return m_decayMode; }
  
  void set_byCombinedIsolationDeltaBetaCorrRaw3Hits(float value){ m_byCombinedIsolationDeltaBetaCorrRaw3Hits = value; }
  void set_byIsolationMVA3newDMwoLTraw(float value) { m_byIsolationMVA3newDMwoLTraw = value; }
  void set_byIsolationMVArun2v1DBnewDMwLTraw(float value) { m_byIsolationMVArun2v1DBnewDMwLTraw = value; }

  void set_chargedIsoPtSum(float value) { m_chargedIsoPtSum = value; }
  void set_neutralIsoPtSum(float value) { m_neutralIsoPtSum = value; }
  void set_puCorrPtSum(float value) { m_puCorrPtSum = value; }
  
  void set_decayMode(int value){ m_decayMode = value; }

  
  /* void set_byLoosePileupWeightedIsolation3Hits(float value) { m_byLoosePileupWeightedIsolation3Hits = value;}  */
  /* void set_byMediumPileupWeightedIsolation3Hits(float value) { m_byMediumPileupWeightedIsolation3Hits = value; }  */
  /* void set_byTightPileupWeightedIsolation3Hits(float value){ m_byTightPileupWeightedIsolation3Hits = value; }  */

  /* void set_byPileupWeightedIsolationRaw3Hits(float value) { m_byPileupWeightedIsolationRaw3Hits = value;}  */
  void set_neutralIsoPtSumWeight(float value) { m_neutralIsoPtSumWeight = value;}
  void set_footprintCorrection(float value) { m_footprintCorrection = value;}
  void set_photonPtSumOutsideSignalCone(float value) { m_photonPtSumOutsideSignalCone = value;}
  


  float get_tag(tag t) const { return tags.get_tag(static_cast<int>(t)); }
  void set_tag(tag t, float value) { return tags.set_tag(static_cast<int>(t), value); }
  
  
  Tau(){
      id_bits = 0;
      m_byCombinedIsolationDeltaBetaCorrRaw3Hits = 0;
      m_byIsolationMVA3newDMwoLTraw = 0;
      m_byIsolationMVArun2v1DBnewDMwLTraw = 0;
      m_chargedIsoPtSum = 0;
      m_neutralIsoPtSum = 0;
      m_puCorrPtSum = 0;
      m_neutralIsoPtSumWeight = 0;
      m_footprintCorrection = 0;
      m_photonPtSumOutsideSignalCone = 0;
      /* m_byPileupWeightedIsolationRaw3Hits = 0; */
      m_decayMode = 0;
  }

private:
    // save the bool-like id variables in an uint64_t as single bits;
    // the bit positions correspond to the int-converted values of the enum bool_id.
    uint64_t id_bits;
    
    float m_byCombinedIsolationDeltaBetaCorrRaw3Hits;
    float m_byIsolationMVA3newDMwoLTraw;
    float m_byIsolationMVArun2v1DBnewDMwLTraw;
    float m_chargedIsoPtSum;
    float m_neutralIsoPtSum;
    float m_puCorrPtSum;
    
    /* float m_byLoosePileupWeightedIsolation3Hits;  */
    /* float m_byMediumPileupWeightedIsolation3Hits; */
    /* float m_byTightPileupWeightedIsolation3Hits;  */
    /* float m_byPileupWeightedIsolationRaw3Hits;    */
    float m_neutralIsoPtSumWeight;
    float m_footprintCorrection;
    float m_photonPtSumOutsideSignalCone;
    
    
    int m_decayMode;
  
    Tags tags;
};

/*
 tau id variables available on Run II 2016 MINIAOD v1:

The available IDs are: 'againstElectronLooseMVA6' 'againstElectronMVA6Raw' 'againstElectronMVA6category' 'againstElectronMediumMVA6' 'againstElectronTightMVA6' 'againstElectronVLooseMVA6' 'againstElectronVTightMVA6' 'againstMuonLoose3' 'againstMuonTight3' 'byCombinedIsolationDeltaBetaCorrRaw3Hits' 'byIsolationMVArun2v1DBdR03oldDMwLTraw' 'byIsolationMVArun2v1DBnewDMwLTraw' 'byIsolationMVArun2v1DBoldDMwLTraw' 'byIsolationMVArun2v1PWdR03oldDMwLTraw' 'byIsolationMVArun2v1PWnewDMwLTraw' 'byIsolationMVArun2v1PWoldDMwLTraw' 'byLooseCombinedIsolationDeltaBetaCorr3Hits' 'byLooseIsolationMVArun2v1DBdR03oldDMwLT' 'byLooseIsolationMVArun2v1DBnewDMwLT' 'byLooseIsolationMVArun2v1DBoldDMwLT' 'byLooseIsolationMVArun2v1PWdR03oldDMwLT' 'byLooseIsolationMVArun2v1PWnewDMwLT' 'byLooseIsolationMVArun2v1PWoldDMwLT' 'byMediumCombinedIsolationDeltaBetaCorr3Hits' 'byMediumIsolationMVArun2v1DBdR03oldDMwLT' 'byMediumIsolationMVArun2v1DBnewDMwLT' 'byMediumIsolationMVArun2v1DBoldDMwLT' 'byMediumIsolationMVArun2v1PWdR03oldDMwLT' 'byMediumIsolationMVArun2v1PWnewDMwLT' 'byMediumIsolationMVArun2v1PWoldDMwLT' 'byPhotonPtSumOutsideSignalCone' 'byTightCombinedIsolationDeltaBetaCorr3Hits' 'byTightIsolationMVArun2v1DBdR03oldDMwLT' 'byTightIsolationMVArun2v1DBnewDMwLT' 'byTightIsolationMVArun2v1DBoldDMwLT' 'byTightIsolationMVArun2v1PWdR03oldDMwLT' 'byTightIsolationMVArun2v1PWnewDMwLT' 'byTightIsolationMVArun2v1PWoldDMwLT' 'byVLooseIsolationMVArun2v1DBdR03oldDMwLT' 'byVLooseIsolationMVArun2v1DBnewDMwLT' 'byVLooseIsolationMVArun2v1DBoldDMwLT' 'byVLooseIsolationMVArun2v1PWdR03oldDMwLT' 'byVLooseIsolationMVArun2v1PWnewDMwLT' 'byVLooseIsolationMVArun2v1PWoldDMwLT' 'byVTightIsolationMVArun2v1DBdR03oldDMwLT' 'byVTightIsolationMVArun2v1DBnewDMwLT' 'byVTightIsolationMVArun2v1DBoldDMwLT' 'byVTightIsolationMVArun2v1PWdR03oldDMwLT' 'byVTightIsolationMVArun2v1PWnewDMwLT' 'byVTightIsolationMVArun2v1PWoldDMwLT' 'byVVTightIsolationMVArun2v1DBdR03oldDMwLT' 'byVVTightIsolationMVArun2v1DBnewDMwLT' 'byVVTightIsolationMVArun2v1DBoldDMwLT' 'byVVTightIsolationMVArun2v1PWdR03oldDMwLT' 'byVVTightIsolationMVArun2v1PWnewDMwLT' 'byVVTightIsolationMVArun2v1PWoldDMwLT' 'chargedIsoPtSum' 'chargedIsoPtSumdR03' 'decayModeFinding' 'decayModeFindingNewDMs' 'footprintCorrection' 'footprintCorrectiondR03' 'neutralIsoPtSum' 'neutralIsoPtSumWeight' 'neutralIsoPtSumWeightdR03' 'neutralIsoPtSumdR03' 'photonPtSumOutsideSignalCone' 'photonPtSumOutsideSignalConedR03' 'puCorrPtSum' .

*/
