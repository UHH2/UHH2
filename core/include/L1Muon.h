#pragma once

#include "Particle.h"
#include <vector>

// Muon L1 trigger objects

class L1Muon : public Particle{
public:
  L1Muon(){
    // v4, energy, charge inherited from Particle

    // bunch crossing info
    m_bx = -100;

    // inherited from L1Candidate.h in CMSSW
    m_hwPt = 0;
    m_hwEta = 0;
    m_hwPhi = 0;
    m_hwQual = 0;
    m_hwIso = 0;

    // inherited from Muon.h in CMSSW
    m_hwCharge = 0;
    m_hwChargeValid = 0;
    m_tfMuonIndex = 0;
    m_hwTag = 0;

    m_hwEtaAtVtx = 0;
    m_hwPhiAtVtx = 0;
    m_etaAtVtx = 0.;
    m_phiAtVtx = 0.;

    m_hwIsoSum = 0;
    m_hwDPhiExtra = 0;
    m_hwDEtaExtra = 0;
    m_hwRank = 0;

    m_debug = false;
  }

  // getters
  int bx() const {return m_bx;}

  int hwPt() const {return m_hwPt;}
  int hwEta() const {return m_hwEta;}
  int hwPhi() const {return m_hwPhi;}
  int hwQual() const {return m_hwQual;}
  int hwIso() const {return m_hwIso;}

  int hwCharge() const { return m_hwCharge; };
  int hwChargeValid() const { return m_hwChargeValid; };
  int tfMuonIndex() const { return m_tfMuonIndex; };
  int hwTag() const { return m_hwTag; };

  int hwEtaAtVtx() const { return m_hwEtaAtVtx; };
  int hwPhiAtVtx() const { return m_hwPhiAtVtx; };
  double etaAtVtx() const { return m_etaAtVtx; };
  double phiAtVtx() const { return m_phiAtVtx; };

  int hwIsoSum() const { return m_hwIsoSum; };
  int hwDPhiExtra() const { return m_hwDPhiExtra; };
  int hwDEtaExtra() const { return m_hwDEtaExtra; };
  int hwRank() const { return m_hwRank; };

  bool debug() const { return m_debug; };

  // setters
  void set_bx(int x) {m_bx=x;}

  void set_hwPt(int x) {m_hwPt=x;}
  void set_hwEta(int x) {m_hwEta=x;}
  void set_hwPhi(int x) {m_hwPhi=x;}
  void set_hwQual(int x) {m_hwQual=x;}
  void set_hwIso(int x) {m_hwIso=x;}

  void set_hwCharge(int x) { m_hwCharge = x; };
  void set_hwChargeValid(int x) { m_hwChargeValid = x; };
  void set_tfMuonIndex(int x) { m_tfMuonIndex = x; };
  void set_hwTag(int x) { m_hwTag = x; };

  void set_hwEtaAtVtx(int x) { m_hwEtaAtVtx = x; };
  void set_hwPhiAtVtx(int x) { m_hwPhiAtVtx = x; };
  void set_etaAtVtx(double x) { m_etaAtVtx = x; };
  void set_phiAtVtx(double x) { m_phiAtVtx = x; };

  void set_hwIsoSum(int x) { m_hwIsoSum = x; };
  void set_hwDPhiExtra(int x) { m_hwDPhiExtra = x; };
  void set_hwDEtaExtra(int x) { m_hwDEtaExtra = x; };
  void set_hwRank(int x) { m_hwRank = x; };

  void set_debug(bool x) { m_debug = x; };

private:
  int m_bx, m_hwPt, m_hwEta, m_hwPhi, m_hwQual, m_hwIso, m_hwCharge, m_hwChargeValid, m_tfMuonIndex, m_hwTag, m_hwEtaAtVtx, m_hwPhiAtVtx, m_hwIsoSum, m_hwDPhiExtra, m_hwDEtaExtra, m_hwRank;
  double m_etaAtVtx, m_phiAtVtx;
  bool m_debug;
};
