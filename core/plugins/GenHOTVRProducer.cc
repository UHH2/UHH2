// -*- C++ -*-
//
// Package:    UHH2/GenHOTVRProducer
// Class:      GenHOTVRProducer
//
/**\class GenHOTVRProducer GenHOTVRProducer.cc UHH2/GenHOTVRProducer/plugins/GenHOTVRProducer.cc

 Description: [one line class summary]

 Implementation:
    [Notes on implementation]
*/
//
// Original Author:  Robin Aggleton
//         Created:  Tue, 23 Jan 2018 13:27:50 GMT
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

#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/Math/interface/LorentzVector.h"

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

class GenHOTVRProducer : public edm::stream::EDProducer<> {
  public:
    explicit GenHOTVRProducer(const edm::ParameterSet&);
    ~GenHOTVRProducer();

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

  private:
    virtual void beginStream(edm::StreamID) override;
    virtual void produce(edm::Event&, const edm::EventSetup&) override;
    virtual void endStream() override;

    virtual pat::Jet createPatJet(const PseudoJet &);

    // ----------member data ---------------------------
    edm::EDGetToken src_token_;
    const std::string subjetCollName_;
    double mu,                 // massjump threshold
      theta,                   // massjump parameter
      max_r,                   // maximum allowed distance R
      min_r,                   // minimum allowed distance R
      rho,                     // cone shrinking parameter
      hotvr_pt_min;            // minimum pT of subjet
};

//
// constructors and destructor
//
GenHOTVRProducer::GenHOTVRProducer(const edm::ParameterSet& iConfig):
  src_token_(consumes<edm::View<reco::Candidate>>(iConfig.getParameter<edm::InputTag>("src"))),
  subjetCollName_("SubJets"),
  mu(iConfig.getParameter<double>("mu")),
  theta(iConfig.getParameter<double>("theta")),
  max_r(iConfig.getParameter<double>("max_r")),
  min_r(iConfig.getParameter<double>("min_r")),
  rho(iConfig.getParameter<double>("rho")),
  hotvr_pt_min(iConfig.getParameter<double>("hotvr_pt_min"))
{
  produces<pat::JetCollection>();
  produces<pat::JetCollection>(subjetCollName_);
}


GenHOTVRProducer::~GenHOTVRProducer()
{

  // do anything here that needs to be done at destruction time
  // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called to produce the data  ------------
void
GenHOTVRProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  // Set the fastjet random seed to a deterministic function
  // of the run/lumi/event.
  // NOTE!!! The fastjet random number sequence is a global singleton.
  // Thus, we have to create an object and get access to the global singleton
  // in order to change it.
  // Taken from VirtualJetProducer
  fastjet::GhostedAreaSpec gas;
  std::vector<int> seeds(2);
  unsigned int runNum_uint = static_cast <unsigned int> (iEvent.id().run());
  unsigned int evNum_uint = static_cast <unsigned int> (iEvent.id().event());
  uint minSeed_ = 14327;
  seeds[0] = std::max(runNum_uint, minSeed_ + 3) + 3 * evNum_uint;
  seeds[1] = std::max(runNum_uint, minSeed_ + 5) + 5 * evNum_uint;
  gas.set_random_status(seeds);

  edm::Handle<edm::View<reco::Candidate>> particles;
  iEvent.getByToken(src_token_, particles);

  // Convert particles to PseudoJets
  std::vector<PseudoJet> _psj;
  for (const auto & cand: *particles) {
    if (std::isnan(cand.px()) ||
        std::isnan(cand.py()) ||
        std::isnan(cand.pz()) ||
        std::isinf(cand.px()) ||
        std::isinf(cand.py()) ||
        std::isinf(cand.pz()) ||
        (cand.pt() == 0))
      continue;

    _psj.push_back(PseudoJet(cand.px(), cand.py(), cand.pz(), cand.energy()));
  }

  // Do the clustering, make jets, nsub, store
  // jet definition with HOTVR plugin
  HOTVR hotvr_plugin(mu, theta, min_r, max_r, rho, hotvr_pt_min, HOTVR::CALIKE);
  JetDefinition jet_def(&hotvr_plugin);

   // Setup cluster sequence
  ClusterSequence cs(_psj, jet_def);
  vector<PseudoJet> hotvr_jets = hotvr_plugin.get_jets();

  auto jetCollection = std::make_unique<pat::JetCollection>();
  auto subjetCollection = std::make_unique<pat::JetCollection>();

  // this is the mapping of subjet to hard jet
  std::vector< std::vector<int> > indices;
  indices.resize(hotvr_jets.size());

  for (unsigned int i = 0; i < hotvr_jets.size(); ++i) {
    // Convert jet and subjets to pat::Jets
    auto thisPatJet = createPatJet(hotvr_jets[i]);
    // dummy for now
    thisPatJet.addUserFloat("tau1", 0);
    thisPatJet.addUserFloat("tau2", 0);
    thisPatJet.addUserFloat("tau3", 0);
    jetCollection->push_back(thisPatJet);

    HOTVRinfo hi = hotvr_jets[i].user_info<HOTVRinfo>();
    std::vector<PseudoJet> subjets = hi.subjets();
    for (uint s=0; s<subjets.size(); s++) {
      indices[i].push_back(subjetCollection->size());
      auto subjet = createPatJet(subjets[s]);
      subjetCollection->push_back(subjet);
    }

  } // end loop over jets
  
  // Following inspired by CompoundJetProducer/VirtualJetProducer
  edm::OrphanHandle<pat::JetCollection> subjetHandleAfterPut = iEvent.put(std::move(subjetCollection), subjetCollName_);

  // setup refs between jets & subjets using indices of subjets in the SubjetCollection
  uint jetInd = 0;
  for (auto & jetItr : *jetCollection) {
    std::vector<int> & ind = indices[jetInd];
    pat::JetPtrCollection subjetPtrs;
    for (const auto indItr : ind) {
      edm::Ptr<pat::Jet> subjetPtr(subjetHandleAfterPut, indItr);
      subjetPtrs.push_back(subjetPtr);
    }
    jetItr.addSubjets(subjetPtrs);
    jetInd++;
  }
  iEvent.put(std::move(jetCollection));
}


pat::Jet GenHOTVRProducer::createPatJet(const PseudoJet & psj)
{
  pat::Jet newJet;
  newJet.setP4(math::XYZTLorentzVector(psj.px(), psj.py(), psj.pz(), psj.E()));
  return newJet;
}


// ------------ method called once each stream before processing any runs, lumis or events  ------------
void
GenHOTVRProducer::beginStream(edm::StreamID)
{
}

// ------------ method called once each stream after processing all runs, lumis and events  ------------
void
GenHOTVRProducer::endStream() {
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
GenHOTVRProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(GenHOTVRProducer);
