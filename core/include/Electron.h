#pragma once

#include "RecParticle.h"
//#include "Particle.h"
#include "Tags.h"
#include "FlavorParticle.h"
#include "source_candidate.h"
#include <vector>

//class Electron : public Particle {
class Electron : public RecParticle {

 public:
  enum tag {
    twodcut_dRmin,
    twodcut_pTrel,
    // 2016
    cutBasedElectronID_Summer16_80X_V1_veto,
    cutBasedElectronID_Summer16_80X_V1_loose,
    cutBasedElectronID_Summer16_80X_V1_medium,
    cutBasedElectronID_Summer16_80X_V1_tight,
    mvaEleID_Spring16_GeneralPurpose_V1_wp90,
    mvaEleID_Spring16_GeneralPurpose_V1_wp80,
    // 2017 & 2018
    cutBasedElectronID_Fall17_94X_V2_veto,
    cutBasedElectronID_Fall17_94X_V2_loose,
    cutBasedElectronID_Fall17_94X_V2_medium,
    cutBasedElectronID_Fall17_94X_V2_tight,
    heepElectronID_HEEPV70,
    mvaEleID_RunIIIWinter22_noIso_V1_wp80,
    mvaEleID_RunIIIWinter22_noIso_V1_wp90,
    mvaEleID_Fall17_noIso_V2_wpLoose,
    mvaEleID_Fall17_iso_V2_wp90,
    mvaEleID_Fall17_iso_V2_wp80,
    mvaEleID_Fall17_iso_V2_wpLoose,

    // EGamma Residuals
    // In general it would be better to implement these corrections as usual class members: see commented out code below. But we use the "Tags" solution here for backwards compatibility with older ntuples of the RunII_106X_v2 branch.
    residual_ecalEnergyPreCorr,
    residual_ecalEnergyErrPreCorr,
    residual_ecalEnergyPostCorr,
    residual_ecalEnergyErrPostCorr,
    residual_ecalTrkEnergyPreCorr,
    residual_ecalTrkEnergyErrPreCorr,
    residual_ecalTrkEnergyPostCorr,
    residual_ecalTrkEnergyErrPostCorr,
    residual_energyScaleValue,
    residual_energySigmaValue,
    residual_energySmearNrSigma,
    residual_energyScaleUp,
    residual_energyScaleDown,
    residual_energyScaleStatUp,
    residual_energyScaleStatDown,
    residual_energyScaleSystUp,
    residual_energyScaleSystDown,
    residual_energyScaleGainUp,
    residual_energyScaleGainDown,
    residual_energyScaleEtUp,
    residual_energyScaleEtDown,
    residual_energySigmaUp,
    residual_energySigmaDown,
    residual_energySigmaPhiUp,
    residual_energySigmaPhiDown,
    residual_energySigmaRhoUp,
    residual_energySigmaRhoDown,
  };

  static tag tagname2tag(const std::string & tagname){
    // if(tagname == "cutBasedElectronID-Summer16-80X-V1-veto") return cutBasedElectronID_Summer16_80X_V1_veto;
    // if(tagname == "cutBasedElectronID-Summer16-80X-V1-loose") return cutBasedElectronID_Summer16_80X_V1_loose;
    // if(tagname == "cutBasedElectronID-Summer16-80X-V1-medium") return cutBasedElectronID_Summer16_80X_V1_medium;
    // if(tagname == "cutBasedElectronID-Summer16-80X-V1-tight") return cutBasedElectronID_Summer16_80X_V1_tight;
    // if(tagname == "mvaEleID-Spring16-GeneralPurpose-V1-wp90") return mvaEleID_Spring16_GeneralPurpose_V1_wp90;
    // if(tagname == "mvaEleID-Spring16-GeneralPurpose-V1-wp80") return mvaEleID_Spring16_GeneralPurpose_V1_wp80;

    if(tagname == "cutBasedElectronID-Fall17-94X-V2-veto") return cutBasedElectronID_Fall17_94X_V2_veto;
    if(tagname == "cutBasedElectronID-Fall17-94X-V2-loose") return cutBasedElectronID_Fall17_94X_V2_loose;
    if(tagname == "cutBasedElectronID-Fall17-94X-V2-medium") return cutBasedElectronID_Fall17_94X_V2_medium;
    if(tagname == "cutBasedElectronID-Fall17-94X-V2-tight") return cutBasedElectronID_Fall17_94X_V2_tight;
    if(tagname == "heepElectronID-HEEPV70") return heepElectronID_HEEPV70;
    if(tagname == "mvaEleID_RunIIIWinter22_noIso_V1_wp80") return mvaEleID_RunIIIWinter22_noIso_V1_wp80;
    if(tagname == "mvaEleID_RunIIIWinter22_noIso_V1_wp90") return mvaEleID_RunIIIWinter22_noIso_V1_wp90;
    if(tagname == "mvaEleID-Fall17-iso-V2-wp90") return mvaEleID_Fall17_iso_V2_wp90;
    if(tagname == "mvaEleID-Fall17-iso-V2-wp80") return mvaEleID_Fall17_iso_V2_wp80;
    if(tagname == "mvaEleID-Fall17-iso-V2-wpLoose") return mvaEleID_Fall17_iso_V2_wpLoose;

    // EGamma Residuals
    if(tagname == "residual_ecalEnergyPreCorr") return residual_ecalEnergyPreCorr;
    if(tagname == "residual_ecalEnergyErrPreCorr") return residual_ecalEnergyErrPreCorr;
    if(tagname == "residual_ecalEnergyPostCorr") return residual_ecalEnergyPostCorr;
    if(tagname == "residual_ecalEnergyErrPostCorr") return residual_ecalEnergyErrPostCorr;
    if(tagname == "residual_ecalTrkEnergyPreCorr") return residual_ecalTrkEnergyPreCorr;
    if(tagname == "residual_ecalTrkEnergyErrPreCorr") return residual_ecalTrkEnergyErrPreCorr;
    if(tagname == "residual_ecalTrkEnergyPostCorr") return residual_ecalTrkEnergyPostCorr;
    if(tagname == "residual_ecalTrkEnergyErrPostCorr") return residual_ecalTrkEnergyErrPostCorr;
    if(tagname == "residual_energyScaleValue") return residual_energyScaleValue;
    if(tagname == "residual_energySigmaValue") return residual_energySigmaValue;
    if(tagname == "residual_energySmearNrSigma") return residual_energySmearNrSigma;
    if(tagname == "residual_energyScaleUp") return residual_energyScaleUp;
    if(tagname == "residual_energyScaleDown") return residual_energyScaleDown;
    if(tagname == "residual_energyScaleStatUp") return residual_energyScaleStatUp;
    if(tagname == "residual_energyScaleStatDown") return residual_energyScaleStatDown;
    if(tagname == "residual_energyScaleSystUp") return residual_energyScaleSystUp;
    if(tagname == "residual_energyScaleSystDown") return residual_energyScaleSystDown;
    if(tagname == "residual_energyScaleGainUp") return residual_energyScaleGainUp;
    if(tagname == "residual_energyScaleGainDown") return residual_energyScaleGainDown;
    if(tagname == "residual_energyScaleEtUp") return residual_energyScaleEtUp;
    if(tagname == "residual_energyScaleEtDown") return residual_energyScaleEtDown;
    if(tagname == "residual_energySigmaUp") return residual_energySigmaUp;
    if(tagname == "residual_energySigmaDown") return residual_energySigmaDown;
    if(tagname == "residual_energySigmaPhiUp") return residual_energySigmaPhiUp;
    if(tagname == "residual_energySigmaPhiDown") return residual_energySigmaPhiDown;
    if(tagname == "residual_energySigmaRhoUp") return residual_energySigmaRhoUp;
    if(tagname == "residual_energySigmaRhoDown") return residual_energySigmaRhoDown;

    throw std::runtime_error("unknown Electron::tag '" + tagname + "'");
  }

  Electron(){

    m_supercluster_eta = 0;
    m_supercluster_phi = 0;
    m_dB = 0;
    m_neutralHadronIso = 0;
    m_chargedHadronIso = 0;
    m_photonIso = 0;
    m_trackIso = 0;
    m_puChargedHadronIso = 0;
    m_gsfTrack_trackerExpectedHitsInner_numberOfLostHits = 0;
    m_gsfTrack_px = 0;
    m_gsfTrack_py = 0;
    m_gsfTrack_pz = 0;
    m_gsfTrack_vx = 0;
    m_gsfTrack_vy = 0;
    m_gsfTrack_vz = 0;
    m_passconversionveto = false;
    m_dEtaIn = 0;
    m_dPhiIn = 0;
    m_sigmaIEtaIEta = 0;
    m_HoverE = 0;
    m_fbrem = 0;
    m_EoverPIn = 0;
    m_EcalEnergy = 0;
    m_hcalOverEcal = 0;
    m_ecalPFClusterIso = 0;
    m_hcalPFClusterIso = 0;
    m_dr03TkSumPt = 0;
    m_mvaGeneralPurpose = 0;
    m_mvaHZZ = 0;
    m_mvaIso = 0;
    m_mvaNoIso = 0;
    m_AEff = 0;

    m_pfMINIIso_CH       = 0;
    m_pfMINIIso_NH       = 0;
    m_pfMINIIso_Ph       = 0;
    m_pfMINIIso_PU       = 0;
    m_pfMINIIso_NH_pfwgt = 0;
    m_pfMINIIso_Ph_pfwgt = 0;

    m_minDeltaRToL1EGamma = 10.;

    m_source_candidates.clear();

    m_Nclusters = 0;
    m_Class = 0;

    m_isEcalDriven = false;
    m_dxy = 0;
    m_dEtaInSeed = 0;
    m_full5x5_e1x5 = 0;
    m_full5x5_e2x5Max = 0;
    m_full5x5_e5x5 = 0;

    m_dxylog = -99;
    m_miniIsoCharged = -99;
    m_miniIsoNeutral = -99;
    m_sip3d = -99;
    m_dzlog = -99;
    m_jetNDauChargedMVASel = -99;
    m_pTRel = -99;
    m_ptRatio = -99;
    m_bTagDeepJetClosestJet = -99;

    m_puppiChargedHadronIso = -99;
    m_puppiNeutralHadronIso = -99;
    m_puppiPhotonIso = -99;
    m_puppiNoLeptonsChargedHadronIso = -99;
    m_puppiNoLeptonsNeutralHadronIso = -99;
    m_puppiNoLeptonsPhotonIso = -99;

    m_electronMVATOP = -99;

    // EGamma Residuals
    /*
    m_ecalEnergyPreCorr = -99;
    m_ecalEnergyErrPreCorr = -99;
    m_ecalEnergyPostCorr = -99;
    m_ecalEnergyErrPostCorr = -99;
    m_ecalTrkEnergyPreCorr = -99;
    m_ecalTrkEnergyErrPreCorr = -99;
    m_ecalTrkEnergyPostCorr = -99;
    m_ecalTrkEnergyErrPostCorr = -99;
    m_energyScaleValue = -99;
    m_energySigmaValue = -99;
    m_energySmearNrSigma = -99;
    m_energyScaleUp = -99;
    m_energyScaleDown = -99;
    m_energyScaleStatUp = -99;
    m_energyScaleStatDown = -99;
    m_energyScaleSystUp = -99;
    m_energyScaleSystDown = -99;
    m_energyScaleGainUp = -99;
    m_energyScaleGainDown = -99;
    m_energyScaleEtUp = -99;
    m_energyScaleEtDown = -99;
    m_energySigmaUp = -99;
    m_energySigmaDown = -99;
    m_energySigmaPhiUp = -99;
    m_energySigmaPhiDown = -99;
    m_energySigmaRhoUp = -99;
    m_energySigmaRhoDown = -99;
    */
  }

  float supercluster_eta() const{return m_supercluster_eta;}
  float supercluster_phi() const{return m_supercluster_phi;}
  float dB() const{return m_dB;}
  float neutralHadronIso() const{return m_neutralHadronIso;}
  float chargedHadronIso() const{return m_chargedHadronIso;}
  float photonIso() const{return m_photonIso;}
  float trackIso() const{return m_trackIso;}
  float puChargedHadronIso() const{return m_puChargedHadronIso;}
  int gsfTrack_trackerExpectedHitsInner_numberOfLostHits() const{return m_gsfTrack_trackerExpectedHitsInner_numberOfLostHits;}
  float gsfTrack_px() const{return m_gsfTrack_px;}
  float gsfTrack_py() const{return m_gsfTrack_py;}
  float gsfTrack_pz() const{return m_gsfTrack_pz;}
  float gsfTrack_vx() const{return m_gsfTrack_vx;}
  float gsfTrack_vy() const{return m_gsfTrack_vy;}
  float gsfTrack_vz() const{return m_gsfTrack_vz;}
  bool passconversionveto() const{return m_passconversionveto;}
  float dEtaIn() const{return m_dEtaIn;}
  float dPhiIn() const{return m_dPhiIn;}
  float sigmaIEtaIEta() const{return m_sigmaIEtaIEta;}
  float HoverE() const{return m_HoverE;}
  float fbrem() const{return m_fbrem;}
  float EoverPIn() const{return m_EoverPIn;}
  float EcalEnergy() const{return m_EcalEnergy;}
  float hcalOverEcal    () const { return m_hcalOverEcal; }
  float ecalPFClusterIso() const { return m_ecalPFClusterIso; }
  float hcalPFClusterIso() const { return m_hcalPFClusterIso; }
  float dr03TkSumPt     () const { return m_dr03TkSumPt; }
  float mvaGeneralPurpose() const{return m_mvaGeneralPurpose;} // 2016
  float mvaHZZ() const{return m_mvaHZZ;}  // 2016
  float mvaIso() const { return m_mvaIso; } // 2017 & 2018
  float mvaNoIso() const { return m_mvaNoIso; } // 2017 & 2018
  float effArea() const{return m_AEff;}

  float pfMINIIso_CH      () const { return m_pfMINIIso_CH; }
  float pfMINIIso_NH      () const { return m_pfMINIIso_NH; }
  float pfMINIIso_Ph      () const { return m_pfMINIIso_Ph; }
  float pfMINIIso_PU      () const { return m_pfMINIIso_PU; }
  float pfMINIIso_NH_pfwgt() const { return m_pfMINIIso_NH_pfwgt; }
  float pfMINIIso_Ph_pfwgt() const { return m_pfMINIIso_Ph_pfwgt; }

  double minDeltaRToL1EGamma() const{return m_minDeltaRToL1EGamma;}

  float dxylog()                          const { return m_dxylog;}
  float miniIsoCharged()                  const { return m_miniIsoCharged;}
  float miniIsoNeutral()                  const { return m_miniIsoNeutral;}
  float sip3d()                           const { return m_sip3d;}
  float dzlog()                           const { return m_dzlog;}
  float jetNDauChargedMVASel()            const { return m_jetNDauChargedMVASel;}
  float pTRel()                           const { return m_pTRel;}
  float ptRatio()                         const { return m_ptRatio;}
  float bTagDeepJetClosestJet()           const { return m_bTagDeepJetClosestJet;}

  float puppiChargedHadronIso()           const { return m_puppiChargedHadronIso;}
  float puppiNeutralHadronIso()           const { return m_puppiNeutralHadronIso;}
  float puppiPhotonIso()                  const { return m_puppiPhotonIso;}
  float puppiNoLeptonsChargedHadronIso()  const { return m_puppiNoLeptonsChargedHadronIso;}
  float puppiNoLeptonsNeutralHadronIso()  const { return m_puppiNoLeptonsNeutralHadronIso;}
  float puppiNoLeptonsPhotonIso()         const { return m_puppiNoLeptonsPhotonIso;}

  float electronMVATOP()                      const { return m_electronMVATOP;}

  // EGamma Residuals
  /*
  float ecalEnergyPreCorr() const { return m_ecalEnergyPreCorr; }
  float ecalEnergyErrPreCorr() const { return m_ecalEnergyErrPreCorr; }
  float ecalEnergyPostCorr() const { return m_ecalEnergyPostCorr; }
  float ecalEnergyErrPostCorr() const { return m_ecalEnergyErrPostCorr; }
  float ecalTrkEnergyPreCorr() const { return m_ecalTrkEnergyPreCorr; }
  float ecalTrkEnergyErrPreCorr() const { return m_ecalTrkEnergyErrPreCorr; }
  float ecalTrkEnergyPostCorr() const { return m_ecalTrkEnergyPostCorr; }
  float ecalTrkEnergyErrPostCorr() const { return m_ecalTrkEnergyErrPostCorr; }
  float energyScaleValue() const { return m_energyScaleValue; }
  float energySigmaValue() const { return m_energySigmaValue; }
  float energySmearNrSigma() const { return m_energySmearNrSigma; }
  float energyScaleUp() const { return m_energyScaleUp; }
  float energyScaleDown() const { return m_energyScaleDown; }
  float energyScaleStatUp() const { return m_energyScaleStatUp; }
  float energyScaleStatDown() const { return m_energyScaleStatDown; }
  float energyScaleSystUp() const { return m_energyScaleSystUp; }
  float energyScaleSystDown() const { return m_energyScaleSystDown; }
  float energyScaleGainUp() const { return m_energyScaleGainUp; }
  float energyScaleGainDown() const { return m_energyScaleGainDown; }
  float energyScaleEtUp() const { return m_energyScaleEtUp; }
  float energyScaleEtDown() const { return m_energyScaleEtDown; }
  float energySigmaUp() const { return m_energySigmaUp; }
  float energySigmaDown() const { return m_energySigmaDown; }
  float energySigmaPhiUp() const { return m_energySigmaPhiUp; }
  float energySigmaPhiDown() const { return m_energySigmaPhiDown; }
  float energySigmaRhoUp() const { return m_energySigmaRhoUp; }
  float energySigmaRhoDown() const { return m_energySigmaRhoDown; }
  */

  const std::vector<source_candidate>& source_candidates() const { return m_source_candidates; }

  void set_supercluster_eta(float x){m_supercluster_eta=x;}
  void set_supercluster_phi(float x){m_supercluster_phi=x;}
  void set_dB(float x){m_dB=x;}
  void set_neutralHadronIso(float x){m_neutralHadronIso=x;}
  void set_chargedHadronIso(float x){m_chargedHadronIso=x;}
  void set_photonIso(float x){m_photonIso=x;}
  void set_trackIso(float x){m_trackIso=x;}
  void set_puChargedHadronIso(float x){m_puChargedHadronIso=x;}
  void set_gsfTrack_trackerExpectedHitsInner_numberOfLostHits(int x){m_gsfTrack_trackerExpectedHitsInner_numberOfLostHits=x;}
  void set_gsfTrack_px(float x){m_gsfTrack_px=x;}
  void set_gsfTrack_py(float x){m_gsfTrack_py=x;}
  void set_gsfTrack_pz(float x){m_gsfTrack_pz=x;}
  void set_gsfTrack_vx(float x){m_gsfTrack_vx=x;}
  void set_gsfTrack_vy(float x){m_gsfTrack_vy=x;}
  void set_gsfTrack_vz(float x){m_gsfTrack_vz=x;}
  void set_passconversionveto(bool x){m_passconversionveto=x;}
  void set_dEtaIn(float x){m_dEtaIn=x;}
  void set_dPhiIn(float x){m_dPhiIn=x;}
  void set_sigmaIEtaIEta(float x){m_sigmaIEtaIEta=x;}  // this is the 'full5x5sigma ieta ieta', (not the gsfelecton->sigmaIetaIeta)
  void set_HoverE(float x){m_HoverE=x;}
  void set_fbrem(float x){m_fbrem=x;}
  void set_EoverPIn(float x){m_EoverPIn=x;}
  void set_EcalEnergy(float x){m_EcalEnergy=x;}
  void set_hcalOverEcal    (float x){ m_hcalOverEcal     = x; }
  void set_ecalPFClusterIso(float x){ m_ecalPFClusterIso = x; }
  void set_hcalPFClusterIso(float x){ m_hcalPFClusterIso = x; }
  void set_dr03TkSumPt     (float x){ m_dr03TkSumPt      = x; }
  void set_mvaGeneralPurpose(float x){m_mvaGeneralPurpose=x;}
  void set_mvaHZZ(float x){m_mvaHZZ=x;}
  void set_mvaIso(float x){m_mvaIso=x;}
  void set_mvaNoIso(float x){m_mvaNoIso=x;}
  void set_effArea(float x){m_AEff=x;}

  void set_pfMINIIso_CH      (float x){ m_pfMINIIso_CH       = x; }
  void set_pfMINIIso_NH      (float x){ m_pfMINIIso_NH       = x; }
  void set_pfMINIIso_Ph      (float x){ m_pfMINIIso_Ph       = x; }
  void set_pfMINIIso_PU      (float x){ m_pfMINIIso_PU       = x; }
  void set_pfMINIIso_NH_pfwgt(float x){ m_pfMINIIso_NH_pfwgt = x; }
  void set_pfMINIIso_Ph_pfwgt(float x){ m_pfMINIIso_Ph_pfwgt = x; }

  void set_minDeltaRToL1EGamma(double x){m_minDeltaRToL1EGamma = x;}

  void set_source_candidates(const std::vector<source_candidate>& vsc){ m_source_candidates = vsc; }
  void add_source_candidate (const source_candidate& sc){ m_source_candidates.push_back(sc); }

  bool  has_tag(tag t) const { return tags.has_tag(static_cast<int>(t)); }
  float get_tag(tag t) const { return tags.get_tag(static_cast<int>(t)); }
  void  set_tag(tag t, float value) { tags.set_tag(static_cast<int>(t), value);}

  void set_Nclusters(int x){m_Nclusters = x;}
  void set_Class(int x){m_Class =x;}
  void set_isEcalDriven(bool x){m_isEcalDriven = x;}
  void set_dxy(float x){m_dxy = x;}
  void set_dEtaInSeed(float x){m_dEtaInSeed=x;}
  void set_full5x5_e1x5(float x){m_full5x5_e1x5 = x;}
  void set_full5x5_e2x5Max(float x){m_full5x5_e2x5Max = x;}
  void set_full5x5_e5x5(float x){m_full5x5_e5x5 = x;}

  void set_dxylog(float x)                           { m_dxylog=x;}
  void set_miniIsoCharged(float x)                   { m_miniIsoCharged=x;}
  void set_miniIsoNeutral(float x)                   { m_miniIsoNeutral=x;}
  void set_sip3d(float x)                            { m_sip3d=x;}
  void set_dzlog(float x)                            { m_dzlog=x;}
  void set_jetNDauChargedMVASel(float x)             { m_jetNDauChargedMVASel=x;}
  void set_electronMVATOP(float x)                   { m_electronMVATOP=x;}
  void set_pTRel(float x)                            { m_pTRel=x;}
  void set_ptRatio(float x)                          { m_ptRatio=x;}
  void set_bTagDeepJetClosestJet(float x)            { m_bTagDeepJetClosestJet=x;}

  void set_puppiChargedHadronIso(float x)            { m_puppiChargedHadronIso=x;}
  void set_puppiNeutralHadronIso(float x)            { m_puppiNeutralHadronIso=x;}
  void set_puppiPhotonIso(float x)                   { m_puppiPhotonIso=x;}
  void set_puppiNoLeptonsChargedHadronIso(float x)   { m_puppiNoLeptonsChargedHadronIso=x;}
  void set_puppiNoLeptonsNeutralHadronIso(float x)   { m_puppiNoLeptonsNeutralHadronIso=x;}
  void set_puppiNoLeptonsPhotonIso(float x)          { m_puppiNoLeptonsPhotonIso=x;}

  // EGamma Residuals
  /*
  void set_ecalEnergyPreCorr(float x) { m_ecalEnergyPreCorr=x; }
  void set_ecalEnergyErrPreCorr(float x) { m_ecalEnergyErrPreCorr=x; }
  void set_ecalEnergyPostCorr(float x) { m_ecalEnergyPostCorr=x; }
  void set_ecalEnergyErrPostCorr(float x) { m_ecalEnergyErrPostCorr=x; }
  void set_ecalTrkEnergyPreCorr(float x) { m_ecalTrkEnergyPreCorr=x; }
  void set_ecalTrkEnergyErrPreCorr(float x) { m_ecalTrkEnergyErrPreCorr=x; }
  void set_ecalTrkEnergyPostCorr(float x) { m_ecalTrkEnergyPostCorr=x; }
  void set_ecalTrkEnergyErrPostCorr(float x) { m_ecalTrkEnergyErrPostCorr=x; }
  void set_energyScaleValue(float x) { m_energyScaleValue=x; }
  void set_energySigmaValue(float x) { m_energySigmaValue=x; }
  void set_energySmearNrSigma(float x) { m_energySmearNrSigma=x; }
  void set_energyScaleUp(float x) { m_energyScaleUp=x; }
  void set_energyScaleDown(float x) { m_energyScaleDown=x; }
  void set_energyScaleStatUp(float x) { m_energyScaleStatUp=x; }
  void set_energyScaleStatDown(float x) { m_energyScaleStatDown=x; }
  void set_energyScaleSystUp(float x) { m_energyScaleSystUp=x; }
  void set_energyScaleSystDown(float x) { m_energyScaleSystDown=x; }
  void set_energyScaleGainUp(float x) { m_energyScaleGainUp=x; }
  void set_energyScaleGainDown(float x) { m_energyScaleGainDown=x; }
  void set_energyScaleEtUp(float x) { m_energyScaleEtUp=x; }
  void set_energyScaleEtDown(float x) { m_energyScaleEtDown=x; }
  void set_energySigmaUp(float x) { m_energySigmaUp=x; }
  void set_energySigmaDown(float x) { m_energySigmaDown=x; }
  void set_energySigmaPhiUp(float x) { m_energySigmaPhiUp=x; }
  void set_energySigmaPhiDown(float x) { m_energySigmaPhiDown=x; }
  void set_energySigmaRhoUp(float x) { m_energySigmaRhoUp=x; }
  void set_energySigmaRhoDown(float x) { m_energySigmaRhoDown=x; }
  */

  float gsfTrack_dxy_vertex(const float point_x, const float point_y) const{
    return ( - (m_gsfTrack_vx-point_x) * m_gsfTrack_py + (m_gsfTrack_vy-point_y) * m_gsfTrack_px ) / sqrt(m_gsfTrack_px*m_gsfTrack_px+m_gsfTrack_py*m_gsfTrack_py);
  };

  float gsfTrack_dz_vertex(const float point_x, const float point_y, const float point_z) const{
    return (m_gsfTrack_vz-point_z) - ((m_gsfTrack_vx-point_x)*m_gsfTrack_px+(m_gsfTrack_vy-point_y)*m_gsfTrack_py)/(m_gsfTrack_px*m_gsfTrack_px+m_gsfTrack_py*m_gsfTrack_py) * m_gsfTrack_pz;
  }

  float relIso() const{
    return ( m_chargedHadronIso +  m_neutralHadronIso + m_photonIso  ) / pt();
  }

  float relIsodb() const{
    return ( m_chargedHadronIso + std::max( 0.0, m_neutralHadronIso + m_photonIso - 0.5*m_puChargedHadronIso ) ) / pt();
  }

  float relIsorho(const double rho) const{
    return ( m_chargedHadronIso + std::max( 0.0, m_neutralHadronIso + m_photonIso - rho*m_AEff ) ) / pt();
  }

  operator FlavorParticle() const {

    FlavorParticle fp;
    fp.set_charge(this->charge());
    fp.set_pt(this->pt());
    fp.set_eta(this->eta());
    fp.set_phi(this->phi());
    fp.set_energy(this->energy());
    fp.set_pdgId(-13*this->charge());

    return fp;
  }

  int Nclusters() const {return m_Nclusters;}
  int Class() const {return m_Class;}
  bool isEcalDriven() const {return m_isEcalDriven;}
  float dxy() const {return m_dxy;}
  float full5x5_e1x5() const { return m_full5x5_e1x5;}
  float full5x5_e2x5Max() const { return m_full5x5_e2x5Max;}
  float full5x5_e5x5() const {return m_full5x5_e5x5;}
  float dEtaInSeed() const {return m_dEtaInSeed;}

 private:
  float m_supercluster_eta;
  float m_supercluster_phi;
  float m_dB;
  float m_neutralHadronIso;
  float m_chargedHadronIso;
  float m_photonIso;
  float m_trackIso;
  float m_puChargedHadronIso;
  int m_gsfTrack_trackerExpectedHitsInner_numberOfLostHits;
  float m_gsfTrack_px;
  float m_gsfTrack_py;
  float m_gsfTrack_pz;
  float m_gsfTrack_vx;
  float m_gsfTrack_vy;
  float m_gsfTrack_vz;
  bool m_passconversionveto;
  float m_dEtaIn;
  float m_dPhiIn;
  float m_sigmaIEtaIEta;
  float m_HoverE;
  float m_fbrem;
  float m_EoverPIn;
  float m_EcalEnergy;
  float m_hcalOverEcal;
  float m_ecalPFClusterIso;
  float m_hcalPFClusterIso;
  float m_dr03TkSumPt;
  float m_mvaGeneralPurpose;
  float m_mvaHZZ;
  float m_mvaIso;
  float m_mvaNoIso;
  float m_AEff;

  float m_pfMINIIso_CH;
  float m_pfMINIIso_NH;
  float m_pfMINIIso_Ph;
  float m_pfMINIIso_PU;
  float m_pfMINIIso_NH_pfwgt;
  float m_pfMINIIso_Ph_pfwgt;

  double m_minDeltaRToL1EGamma;

  std::vector<source_candidate> m_source_candidates;

  Tags tags;

  int m_Nclusters;//Number of clusters in Super Cluster
  int m_Class;//Classification (see CMS-EGM-13-001) //UNKNOWN =-1, GOLDEN =0, BIGBREM =1, BADTRACK =2, SHOWERING =3, GAP =4

  //additional vars for HEEP cut
  bool m_isEcalDriven;
  float m_dxy;
  float m_dEtaInSeed;//instead of the super cluster eta, it uses the eta of the seed cluster of the super cluster, which should be more accurate
  float m_full5x5_e1x5;
  float m_full5x5_e2x5Max;
  float m_full5x5_e5x5;

  float m_dxylog;
  float m_miniIsoCharged;
  float m_miniIsoNeutral;
  float m_sip3d;
  float m_dzlog;
  float m_jetNDauChargedMVASel;
  float m_pTRel;
  float m_ptRatio;
  float m_bTagDeepJetClosestJet;

  float m_puppiChargedHadronIso;
  float m_puppiNeutralHadronIso;
  float m_puppiPhotonIso;
  float m_puppiNoLeptonsChargedHadronIso;
  float m_puppiNoLeptonsNeutralHadronIso;
  float m_puppiNoLeptonsPhotonIso;

  float m_electronMVATOP;

  // EGamma Residuals
  /*
  float m_ecalEnergyPreCorr;
  float m_ecalEnergyErrPreCorr;
  float m_ecalEnergyPostCorr;
  float m_ecalEnergyErrPostCorr;
  float m_ecalTrkEnergyPreCorr;
  float m_ecalTrkEnergyErrPreCorr;
  float m_ecalTrkEnergyPostCorr;
  float m_ecalTrkEnergyErrPostCorr;
  float m_energyScaleValue;
  float m_energySigmaValue;
  float m_energySmearNrSigma;
  float m_energyScaleUp;
  float m_energyScaleDown;
  float m_energyScaleStatUp;
  float m_energyScaleStatDown;
  float m_energyScaleSystUp;
  float m_energyScaleSystDown;
  float m_energyScaleGainUp;
  float m_energyScaleGainDown;
  float m_energyScaleEtUp;
  float m_energyScaleEtDown;
  float m_energySigmaUp;
  float m_energySigmaDown;
  float m_energySigmaPhiUp;
  float m_energySigmaPhiDown;
  float m_energySigmaRhoUp;
  float m_energySigmaRhoDown;
  */
};
