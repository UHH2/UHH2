// -*- C++ -*-
//
// Package:    UHH2/XConeProducer
// Class:      XConeProducer
//
/**\class XConeProducer XConeProducer.cc UHH2/XConeProducer/plugins/XConeProducer.cc

 Description: [one line class summary]

 Implementation:
    [Notes on implementation]
*/
//
// Original Author:  Robin Aggleton
//         Created:  Tue, 23 Jan 2018 13:27:56 GMT
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"

#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/PatCandidates/interface/Jet.h"

#include "fastjet/ClusterSequence.hh"
#include "fastjet/ClusterSequenceArea.hh"
#include "fastjet/contrib/HOTVR.hh"
#include "fastjet/contrib/HOTVRinfo.hh"
#include "fastjet/contrib/Nsubjettiness.hh"
#include "fastjet/contrib/XConePlugin.hh"
#include "fastjet/contrib/SoftDrop.hh"

#include "vector"

using namespace fastjet;
using namespace contrib;

//
// class declaration
//

class XConeProducer : public edm::stream::EDProducer<> {
  public:
    explicit XConeProducer(const edm::ParameterSet&);
    ~XConeProducer();

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

  private:
    virtual void beginStream(edm::StreamID) override;
    virtual void produce(edm::Event&, const edm::EventSetup&) override;
    virtual void endStream() override;
    
    virtual pat::Jet createPatJet(const fastjet::PseudoJet &);
    virtual pat::Jet createPatJet(const fastjet::PseudoJet & , const std::vector<fastjet::PseudoJet> &, double , std::vector<double>, float);

    // ----------member data ---------------------------
    edm::EDGetToken src_token_;

    // ----------member data ---------------------------
};

//
// constants, enums and typedefs
//


//
// static data member definitions
//

//
// constructors and destructor
//
XConeProducer::XConeProducer(const edm::ParameterSet& iConfig):
  src_token_(consumes<edm::View<pat::PackedCandidate>>(iConfig.getParameter<edm::InputTag>("src")))
{
  produces<pat::JetCollection>();
}


XConeProducer::~XConeProducer()
{

  // do anything here that needs to be done at destruction time
  // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called to produce the data  ------------
void
XConeProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  edm::Handle<std::vector<pat::PackedCandidate>> particles;
  iEvent.getByToken(src_token_, particles);

  if (particles->size() < 15) {
    auto jetCollection = std::make_unique<pat::JetCollection>();
    iEvent.put(std::move(jetCollection));
    auto subjetCollection = std::make_unique<pat::JetCollection>();
    iEvent.put(std::move(subjetCollection), subjetCollName_);
    return;
  }

  // Convert particles to PseudoJets
  std::vector<PseudoJet> _psj;
  for (const auto & cand: *particles) {
    if (std::isnan(cand.px()) ||
        std::isnan(cand.py()) ||
        std::isnan(cand.pz()) ||
        std::isinf(cand.px()) ||
        std::isinf(cand.py()) ||
        std::isinf(cand.pz()))
      continue;

    _psj.push_back(PseudoJet(cand.px(), cand.py(), cand.pz(), cand.energy()));
  }

  // Run first clustering step (N=2, R=1.2)
  vector<PseudoJet> fatjets;
  XConePlugin plugin_xcone(2, 1.2, 2.0);
  double ghost_maxrap = 4.0;      // maxiumum y of ghost particles
  AreaDefinition area_def(active_area, GhostedAreaSpec(ghost_maxrap));
  JetDefinition jet_def_xcone(&plugin_xcone);
  ClusterSequence clust_seq_xcone(_psj, jet_def_xcone);
  fatjets = sorted_by_pt(clust_seq_xcone.inclusive_jets(0));

  // get SoftDrop Mass
  SoftDrop sd(0.0, 0.1, 1.2);
  PseudoJet sdjet1 = sd(fatjets[0]);
  PseudoJet sdjet2 = sd(fatjets[1]);
  float sd_mass1 = sdjet1.m();
  float sd_mass2 = sdjet2.m();

  // get and wirte list: if particle i is clustered in jet j, the i-th entry of the list == j
  vector<int> list_fat;
  list_fat.clear();
  list_fat = clust_seq_xcone.particle_jet_indices(fatjets);
  vector<PseudoJet> particle_in_fat1, particle_in_fat2;

  // get one set of particles for each jet
  for (unsigned int ipart=0; ipart < _psj.size(); ++ipart) {
    if (list_fat[ipart]==0) {
      particle_in_fat1.push_back(_psj.at(ipart)); // get vector as PseudoJet for fatjet 1
    }
    if (list_fat[ipart]==1) {
      particle_in_fat2.push_back(_psj.at(ipart)); // get vector as PseudoJet for fatjet 2
    }
  }

  if(particle_in_fat1.size() < 3 || particle_in_fat2.size()<3) {
    cms::Exception("InsufficientParticles", 
                   "Not enough particles to run second XCone step");
  }

  // Run second clustering step (N=3, R=0.4) for each fat jet
  vector<PseudoJet> subjets_1, subjets_2;

  // subjets from fat jet 1
  XConePlugin plugin_xcone_sub1(3, 0.4, 2.0);
  JetDefinition jet_def_sub1(&plugin_xcone_sub1);
  ClusterSequenceArea clust_seq_sub1(particle_in_fat1, jet_def_sub1, area_def);
  subjets_1 = sorted_by_pt(clust_seq_sub1.inclusive_jets(0));

  //subjets from fat jet 2
  XConePlugin plugin_xcone_sub2(3, 0.4, 2.0);
  JetDefinition jet_def_sub2(&plugin_xcone_sub2);
  ClusterSequenceArea clust_seq_sub2(particle_in_fat2, jet_def_sub2, area_def);
  subjets_2 = sorted_by_pt(clust_seq_sub2.inclusive_jets(0));

  // set TopJets with subjets and JetArea

  //double jet1_area = fatjets[0].area();
  //double jet2_area = fatjets[1].area();
  vector<double> subjet1_area;
  vector<double> subjet2_area;
  for (unsigned int j = 0; j < subjets_1.size(); ++j) subjet1_area.push_back(subjets_1[j].area());
  for (unsigned int k = 0; k < subjets_2.size(); ++k) subjet2_area.push_back(subjets_2[k].area());

  double jet1_area = 0;
  double jet2_area = 0;
  /*
  vector<double> subjet1_area;
  vector<double> subjet2_area;
  for (unsigned int j = 0; j < subjets_1.size(); ++j) subjet1_area.push_back(0.);
  for (unsigned int k = 0; k < subjets_2.size(); ++k) subjet2_area.push_back(0.);
  */
  auto outputJets = std::make_unique<pat::JetCollection>();
  outputJets->push_back(createPatJet(fatjets[0], subjets_1, jet1_area, subjet1_area, sd_mass1));
  outputJets->push_back(createPatJet(fatjets[1], subjets_2, jet2_area, subjet2_area, sd_mass2));
  iEvent.put(std::move(outputJets));
}

pat::Jet XConeProducer::createPatJet(const PseudoJet & psj)
{
  pat::Jet newJet;
  newJet.setP4(math::XYZTLorentzVector(psj.pt(), psj.eta(), psj.phi(), psj.E()));
  return newJet;
}

pat::Jet XConeProducer::createPatJet(const PseudoJet & psj, const vector<PseudoJet> &subpsj, double jet_area, vector<double> subjet_area, float sd_mass)
{
  pat::Jet newJet = createPatJet(psj);
  newJet.setJetArea(jet_area);
  newJet.addUserFloat("softdropmass", sd_mass);
  for (uint i=0; i<subpsj.size(); i++) {
    pat::Jet subjet = createPatJet(subpsj[i]);
    subjet.setJetArea(subjet_area[i]);
  }

  return newJet;
}

// ------------ method called once each stream before processing any runs, lumis or events  ------------
void
XConeProducer::beginStream(edm::StreamID)
{
}

// ------------ method called once each stream after processing all runs, lumis and events  ------------
void
XConeProducer::endStream() {
}


// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
XConeProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(XConeProducer);
