#pragma once

#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/Utils.h"

// see also ElectronIds.h for general comments

// https://twiki.cern.ch/twiki/bin/view/CMS/SWGuideMuonIdRun2
class MuonIDLoose {
 public:
  bool operator()(const Muon&, const uhh2::Event&) const;
};

class MuonIDMedium {
 public:
  bool operator()(const Muon&, const uhh2::Event&) const;
};

class MuonIDTight {
 public:
  bool operator()(const Muon&, const uhh2::Event&) const;
};

class MuonIDHighPt {
 public:
  bool operator()(const Muon&, const uhh2::Event&) const;
};

// only kinematic cuts, with no further id
class MuonIDKinematic {
public:
    MuonIDKinematic(double ptmin, double etamax) DEPRECATED("Use more general PtEtaCut instead");
    bool operator()(const Muon & muon, const uhh2::Event & event) const;
    
private:
    double ptmin, etamax;
};

//relativ Isolation for Muon check above twiki for reference
class MuonIso {
 public:
  MuonIso(double iso = 0.15);
  bool operator()(const Muon & muon, const uhh2::Event & event) const;
 private:
  double iso;
};

// Muon selectors for PF MINI-Isolation
class Muon_MINIIso {

 public:
  explicit Muon_MINIIso(const float iso_cut, const std::string& iso_key): iso_cut_(iso_cut), iso_key_(iso_key) {}
  virtual ~Muon_MINIIso() {}

  bool operator()(const Muon&, const uhh2::Event&) const;

 protected:
  float iso_cut_;
  std::string iso_key_;
};

class MuonIDMedium_ICHEP {
 public:
  bool operator()(const Muon&, const uhh2::Event&) const;
};
