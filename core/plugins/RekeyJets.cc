// -*- C++ -*-
//
// Package:    UHH2/RekeyJets
// Class:      RekeyJets
//
/**\class RekeyJets RekeyJets.cc UHH2/RekeyJets/plugins/RekeyJets.cc

 Description: Replace daughters of jet with those matched by key from another collection
  e.g. replace puppi jet daughters with equivalent particles from packedPFCandidates

  NB do not attempt to do this to the subjets - you will get into difficulty,
  because in 10_2_10 there is no way to "remove" the old subjets, only append
  new ones. So you have to change subjet label, which may not be optimal.

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

#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/Common/interface/RefToPtr.h"

#include "vector"

using std::cout;
using std::endl;


//
// class declaration
//

class RekeyJets : public edm::stream::EDProducer<> {
  public:
    explicit RekeyJets(const edm::ParameterSet&);
    ~RekeyJets();

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

  private:
    virtual void beginStream(edm::StreamID) override;
    virtual void produce(edm::Event&, const edm::EventSetup&) override;
    virtual void endStream() override;

    edm::EDGetTokenT<pat::JetCollection> srcJetToken_;
    edm::EDGetTokenT<edm::View<reco::Candidate>> srcCandToken_;
};

//
// constructors and destructor
//
RekeyJets::RekeyJets(const edm::ParameterSet& iConfig):
  srcJetToken_(consumes<pat::JetCollection>(iConfig.getParameter<edm::InputTag>("jetSrc"))),
  srcCandToken_(consumes<edm::View<reco::Candidate>>(iConfig.getParameter<edm::InputTag>("candidateSrc")))
{
  produces<pat::JetCollection>();
}


RekeyJets::~RekeyJets()
{

  // do anything here that needs to be done at destruction time
  // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called to produce the data  ------------
void
RekeyJets::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  edm::Handle<pat::JetCollection> jetHandle;
  iEvent.getByToken(srcJetToken_, jetHandle);

  edm::Handle<edm::View<reco::Candidate>> candHandle;
  iEvent.getByToken(srcCandToken_, candHandle);

  std::unique_ptr<pat::JetCollection> outJets = std::make_unique<pat::JetCollection>();

  for (const auto & jetItr : *jetHandle) {
    pat::Jet newJet(jetItr);
    // setup daughters of new jet to be the equivalent ones from the candidateSrc
    newJet.clearDaughters();
    for (const auto & dauItr : jetItr.daughterPtrVector()) {
      const unsigned long key = dauItr.key();
      edm::Ptr<reco::Candidate> newDau = candHandle->ptrAt(key);
      newJet.addDaughter(newDau);
    }
    outJets->push_back(newJet);
  }

  iEvent.put(std::move(outJets));

}


// ------------ method called once each stream before processing any runs, lumis or events  ------------
void
RekeyJets::beginStream(edm::StreamID)
{
}

// ------------ method called once each stream after processing all runs, lumis and events  ------------
void
RekeyJets::endStream() {
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
RekeyJets::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("jetSrc");
  desc.add<edm::InputTag>("candidateSrc");
  descriptions.add("rekeyJets", desc);
}


//define this as a plug-in
DEFINE_FWK_MODULE(RekeyJets);
