#pragma once

#include "Particle.h"
#include <vector>

// Muon L1 trigger objects

class L1EtSum : public Particle{
public:
  enum EtSumType {
    kTotalEt,
    kTotalHt,
    kMissingEt,
    kMissingHt,
    kTotalEtx,
    kTotalEty,
    kTotalHtx,
    kTotalHty,
    kEmpty
  };

  L1EtSum(){
    // v4, energy, charge inherited from Particle

    // bunch crossing info
    m_bx = -100;

    // inherited from L1Candidate.h in CMSSW
    m_hwPt = 0;
    m_hwEta = 0;
    m_hwPhi = 0;
    m_hwQual = 0;
    m_hwIso = 0;

    // type info
    m_type = EtSumType::kEmpty;
  }

  // getters
  int bx() const {return m_bx;}

  int hwPt() const {return m_hwPt;}
  int hwEta() const {return m_hwEta;}
  int hwPhi() const {return m_hwPhi;}
  int hwQual() const {return m_hwQual;}
  int hwIso() const {return m_hwIso;}

  EtSumType type() const {return m_type;}

  // setters
  void set_bx(int x) {m_bx=x;}

  void set_hwPt(int x) {m_hwPt=x;}
  void set_hwEta(int x) {m_hwEta=x;}
  void set_hwPhi(int x) {m_hwPhi=x;}
  void set_hwQual(int x) {m_hwQual=x;}
  void set_hwIso(int x) {m_hwIso=x;}

  void set_type(EtSumType x) {m_type=x;}

private:
  int m_bx, m_hwPt, m_hwEta, m_hwPhi, m_hwQual, m_hwIso;
  EtSumType m_type;
};
