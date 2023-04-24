#include <memory>
#include <vector>
#include <string>

#include <FWCore/Framework/interface/Frameworkfwd.h>
#include <FWCore/Framework/interface/one/EDProducer.h>
#include <FWCore/Framework/interface/Event.h>
#include <FWCore/Framework/interface/MakerMacros.h>
#include <FWCore/ParameterSet/interface/ParameterSet.h>

#include <DataFormats/PatCandidates/interface/Muon.h>

class PATMuonUserData : public edm::one::EDProducer<> {
 public:
  explicit PATMuonUserData(const edm::ParameterSet&);
  virtual ~PATMuonUserData() {}

 private:
  virtual void produce(edm::Event&, const edm::EventSetup&);

  edm::EDGetTokenT< edm::View<pat::Muon> > src_;
  std::vector<std::string> vmaps_double_;
  std::vector<edm::EDGetToken> vmaps_token_;
};

// constructor
PATMuonUserData::PATMuonUserData( const edm::ParameterSet& iConfig ){

  src_ = consumes< edm::View<pat::Muon> >(iConfig.getParameter<edm::InputTag>("src"));
  if(iConfig.exists("vmaps_double")) {
    vmaps_double_ = iConfig.getParameter<std::vector<std::string>>("vmaps_double");
    for(unsigned int i=0; i<vmaps_double_.size(); i++){
      vmaps_token_.push_back(consumes<edm::ValueMap<double> >(edm::InputTag(vmaps_double_.at(i))));
    }
  }

  produces< pat::MuonCollection >();
}

// ------------ method called to produce the data  ------------
void PATMuonUserData::produce(edm::Event& iEvent, const edm::EventSetup& iSetup){

  edm::Handle< edm::View<pat::Muon> > patMuons;
  iEvent.getByToken(src_, patMuons);

  std::vector< edm::Handle< edm::ValueMap<double> > > vmapDs;
  for(unsigned int i=0; i<vmaps_token_.size(); ++i){

    edm::Handle< edm::ValueMap<double> > vmapD;
    iEvent.getByToken(vmaps_token_.at(i), vmapD);
    vmapDs.push_back(vmapD);
  }

  std::unique_ptr< pat::MuonCollection > newMuons(new pat::MuonCollection);
  newMuons->reserve(patMuons->size());

  for(unsigned int i=0; i<patMuons->size(); ++i){

    newMuons->push_back(patMuons->at(i));
    pat::Muon& muo = newMuons->back();

    for(unsigned int j=0; j<vmapDs.size(); ++j){

      if(muo.hasUserFloat(vmaps_double_.at(j)))
        throw cms::Exception("InputError") << "@@@ PATMuonUserData::produce -- PAT user-float label already exists: " << vmaps_double_.at(j);

      const double& val = (*(vmapDs.at(j)))[patMuons->refAt(i)];
      muo.addUserFloat(vmaps_double_.at(j), float(val));
    }
  }

  iEvent.put(std::move(newMuons));

  return;
}

DEFINE_FWK_MODULE(PATMuonUserData);
