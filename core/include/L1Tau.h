#pragma once

#include "Particle.h"
#include <vector>

// Tau L1 trigger objects

class L1Tau : public Particle{
public:
  L1Tau(){
    // v4, energy, charge inherited from Particle

    // bunch crossing info
    m_bx = -100;

    // inherited from L1Candidate.h in CMSSW
    m_hwPt = 0;
    m_hwEta = 0;
    m_hwPhi = 0;
    m_hwQual = 0;
    m_hwIso = 0;

    // inherited from Tau.h in CMSSW
    m_towerIEta = 0;
    m_towerIPhi = 0;
    m_rawEt = 0;
    m_isoEt = 0;
    m_nTT = 0;
    m_hasEM = false;
    m_isMerged = false;
  }

  // getters
  int bx() const {return m_bx;}

  int hwPt() const {return m_hwPt;}
  int hwEta() const {return m_hwEta;}
  int hwPhi() const {return m_hwPhi;}
  int hwQual() const {return m_hwQual;}
  int hwIso() const {return m_hwIso;}

  int towerIEta() const {return m_towerIEta;}
  int towerIPhi() const {return m_towerIPhi;}
  int rawEt() const {return m_rawEt;}
  int isoEt() const {return m_isoEt;}
  int nTT() const {return m_nTT;}
  bool hasEM() const {return m_hasEM;}
  bool isMerged() const {return m_isMerged;}

  // setters
  void set_bx(int x) {m_bx=x;}

  void set_hwPt(int x) {m_hwPt=x;}
  void set_hwEta(int x) {m_hwEta=x;}
  void set_hwPhi(int x) {m_hwPhi=x;}
  void set_hwQual(int x) {m_hwQual=x;}
  void set_hwIso(int x) {m_hwIso=x;}

  void set_towerIEta(int x) {m_towerIEta=x;}
  void set_towerIPhi(int x) {m_towerIPhi=x;}
  void set_rawEt(int x) {m_rawEt=x;}
  void set_isoEt(int x) {m_isoEt=x;}
  void set_nTT(int x) {m_nTT=x;}
  void set_hasEM(bool x) {m_hasEM=x;}
  void set_isMerged(bool x) {m_isMerged=x;}

private:
  int m_bx, m_hwPt, m_hwEta, m_hwPhi, m_hwQual, m_hwIso, m_towerIEta, m_towerIPhi, m_rawEt, m_isoEt, m_nTT;
  bool m_hasEM, m_isMerged;
};
