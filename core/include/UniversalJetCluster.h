#pragma once

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/GenParticle.h"
#include "UHH2/core/include/Jet.h"
#include "UHH2/core/include/TopJet.h"

#include "fastjet/ClusterSequence.hh"
#include "fastjet/ClusterSequenceArea.hh"
#include "fastjet/contrib/HOTVR.hh"
#include "fastjet/contrib/HOTVRinfo.hh"
#include "fastjet/contrib/Nsubjettiness.hh"
#include "fastjet/contrib/XConePlugin.hh"
#include "fastjet/contrib/SoftDrop.hh"

#include "vector"
//#include <fstream>

/*
 * UniversalJetCluster implements a tool for clustering XCone and HOTVR Jets
 */
class UniversalJetCluster
{
 public:

  UniversalJetCluster(std::vector<PFParticle> *pfparticles, bool, bool);

  // getter
  std::vector<TopJet> GetHOTVRTopJets();
  std::vector<TopJet> GetXCone33Jets();

 private:
  std::vector<fastjet::PseudoJet> _psj;
  std::vector<TopJet> _hotvrTopJets;
  std::vector<TopJet> _xcone33TopJets;

  fastjet::PseudoJet ConvertPFToPsj(const PFParticle & pfp);
  Jet ConvertPsjToJet(const fastjet::PseudoJet & psj, double jet_area);
  Jet ConvertPsjToJet(const fastjet::PseudoJet & psj);
  TopJet ConvertPsjToTopJet(const fastjet::PseudoJet & psj, const std::vector<fastjet::PseudoJet> & subpsj, double tau1, double tau2, double tau3, double jet_area, std::vector<double> subjet_area);
  TopJet ConvertPsjToTopJet(const fastjet::PseudoJet & psj, const std::vector<fastjet::PseudoJet> &subpsj, double jet_area, std::vector<double> subjet_area, float sd_mass);

  void ClusterHOTVR();
  void ClusterXCone33();
};
