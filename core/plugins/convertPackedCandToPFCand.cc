#include <FWCore/Framework/interface/Frameworkfwd.h>
#include <FWCore/Framework/interface/stream/EDProducer.h>
#include <FWCore/Framework/interface/Event.h>
#include <FWCore/Framework/interface/MakerMacros.h>
#include <FWCore/ParameterSet/interface/ParameterSet.h>
 
#include <DataFormats/PatCandidates/interface/PackedCandidate.h>
#include <DataFormats/ParticleFlowCandidate/interface/PFCandidate.h>
#include <memory>

class convertPackedCandToPFCand : public edm::stream::EDProducer<> {

 public:
  convertPackedCandToPFCand(const edm::ParameterSet& iConfig);
  ~convertPackedCandToPFCand() {}
  
 private:
  virtual void produce(edm::Event& iEvent, const edm::EventSetup& iSetup) override;
  edm::EDGetTokenT< pat::PackedCandidateCollection > src_;
};

convertPackedCandToPFCand::convertPackedCandToPFCand(const edm::ParameterSet& iConfig){

  src_ = consumes< pat::PackedCandidateCollection >(iConfig.getParameter<edm::InputTag>("src"));
  produces<reco::PFCandidateCollection>();
}

void convertPackedCandToPFCand::produce(edm::Event& iEvent,const edm::EventSetup& iSetup){

  edm::Handle< pat::PackedCandidateCollection > packedCands_;
  iEvent.getByToken(src_, packedCands_);

  std::unique_ptr< reco::PFCandidateCollection > recoPFCands(new reco::PFCandidateCollection());

  reco::PFCandidate dummy;
  for(int iCand=0; iCand<int(packedCands_->size()); ++iCand){

    reco::PFCandidate intPFCand(packedCands_->at(iCand).charge(),packedCands_->at(iCand).p4(), dummy.translatePdgIdToType(packedCands_->at(iCand).pdgId()));
    recoPFCands->push_back(intPFCand);
  }

  iEvent.put(std::move(recoPFCands));

  return;
}

DEFINE_FWK_MODULE(convertPackedCandToPFCand);
