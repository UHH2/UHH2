#pragma once

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/Particle.h"
#include "UHH2/core/include/GenParticle.h"
#include "UHH2/core/include/GenTopJet.h"

#include "fastjet/ClusterSequence.hh"
#include "fastjet/contrib/HOTVR.hh"
#include "fastjet/contrib/HOTVRinfo.hh"
#include "fastjet/contrib/XConePlugin.hh"
#include "fastjet/contrib/SoftDrop.hh"

#include "vector"

class UniversalGenJetCluster
{
 public:

  UniversalGenJetCluster(std::vector<GenParticle> *genparticles);

  // getter
  std::vector<GenTopJet> GetHOTVRTopJets();
  std::vector<GenTopJet> GetXCone23Jets();
  std::vector<GenTopJet> GetXCone33Jets();
  std::vector<GenTopJet> GetXCone33Jets_softdrop();

 private:
  std::vector<fastjet::PseudoJet> _psj;
  std::vector<GenTopJet> _hotvrGenTopJets;
  std::vector<GenTopJet> _xcone23TopJets;
  std::vector<GenTopJet> _xcone33TopJets;
  std::vector<GenTopJet> _xcone33TopJets_softdrop;
  GenParticle _lepton;
  bool _lep_found;

  //  fastjet::PseudoJet ConvertGenToPsj(GenParticle * genp);
  fastjet::PseudoJet ConvertGenToPsj(const GenParticle & genp); //TEST
  Particle ConvertPsjToParticle(const fastjet::PseudoJet & psj);
  GenTopJet ConvertPsjToGenTopJet(const fastjet::PseudoJet & psj, const std::vector<fastjet::PseudoJet> & subpsj);

  void ClusterHOTVR();
  void ClusterXCone23();
  void ClusterXCone33();

};
