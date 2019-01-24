#pragma once

#include "UHH2/core/include/Hists.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/LorentzVector.h"

#include <vector>
#include <string>

#include "TH1F.h"

class GenJetsHists : public uhh2::Hists {
 public:
  struct ParticleHist{
      TH1F* pt, *eta, *phi, *mass; 
  };
  GenJetsHists(uhh2::Context & ctx, const std::string & dirname,  const unsigned int NumberOfPlottedJets=4, const std::string & collection_="");
  GenJetsHists::ParticleHist book_ParticleHist(const std::string & axisSuffix, const std::string & histSuffix, double minPt, double maxPt);
  void fill_ParticleHist(const Particle & jet, ParticleHist & particle_hist, double  weight);
  virtual void fill(const uhh2::Event & ev) override;
 private:
  std::string collection;
  std::vector<ParticleHist> single_ParticleHists;
  ParticleHist alljets;
  TH1F* number;
  uhh2::Event::Handle<std::vector<GenJet> > h_jets;
};
