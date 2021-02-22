#pragma once

#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/Selection.h"

#include "UHH2/common/include/Utils.h"

using namespace uhh2;
using namespace std;

class EtaPhiEventCleaner: public uhh2::Selection {
public:
  EtaPhiEventCleaner(uhh2::Context& ctx, int min_run, int max_run, float min_eta, float max_eta, float min_phi, float max_phi, std::string JetCollection = "jets", std::string TopJetCollection = "topjets", bool doElectrons = true, bool doMuons = true);
  virtual bool passes(const uhh2::Event& event) override;
private:
  int min_run;
  int max_run;
  float min_eta;
  float max_eta;
  float min_phi;
  float max_phi;
  std::string JetCollection;
  std::string TopJetCollection;
  bool doJets;
  bool doElectrons;
  bool doMuons;
  uhh2::Event::Handle<std::vector<Jet> > h_Jets;
  uhh2::Event::Handle<std::vector<TopJet> > h_TopJets;
};

// Class which inherits from EtaPhiEventCleaner, specific to the HEM15/16 issues.
// This applies to 2018 only.
// The values are specified in https://hypernews.cern.ch/HyperNews/CMS/get/JetMET/2000.html
class HEMCleanerSelection: public EtaPhiEventCleaner{
public:
  static const int min_run_HEM = 319077; // end of Run B in 2018
  static const int max_run_HEM = -1;
  static constexpr float min_eta_HEM = -2.964;
  static constexpr float max_eta_HEM = -1.305;
  static constexpr float min_phi_HEM = -1.6;
  static constexpr float max_phi_HEM = -0.87;

  HEMCleanerSelection(uhh2::Context& ctx, std::string JetCollection, std::string TopJetCollection, bool doElectrons=true, bool doMuons=true);

};
