// system include files
#include <memory>
#include <vector>
#include <string>

// user include files
#include <FWCore/Framework/interface/Frameworkfwd.h>
#include <FWCore/Framework/interface/EDProducer.h>
#include <FWCore/Framework/interface/Event.h>
#include <FWCore/Framework/interface/MakerMacros.h>
#include <FWCore/ParameterSet/interface/ParameterSet.h>

#include <DataFormats/PatCandidates/interface/Muon.h>

// class declaration
class PATMuonUserData : public edm::EDProducer {
 public:
  explicit PATMuonUserData(const edm::ParameterSet&);
  ~PATMuonUserData() {}

 private:
  virtual void produce(edm::Event&, const edm::EventSetup&);

  edm::EDGetTokenT< edm::View<pat::Muon> > src_;
  std::vector<std::string> vmaps_double_;
};

// constructor
PATMuonUserData::PATMuonUserData( const edm::ParameterSet& iConfig ){

  src_ = consumes< edm::View<pat::Muon> >(iConfig.getParameter<edm::InputTag>("src"));
  if(iConfig.exists("vmaps_double")) vmaps_double_ = iConfig.getParameter<std::vector<std::string>>("vmaps_double");

  produces< pat::MuonCollection >();
}

// ------------ method called to produce the data  ------------
void PATMuonUserData::produce(edm::Event& iEvent, const edm::EventSetup& iSetup){

  edm::Handle< edm::View<pat::Muon> > patMuons;
  iEvent.getByToken(src_, patMuons);

  std::vector< edm::Handle< edm::ValueMap<double> > > vmapDs;
  for(unsigned int i=0; i<vmaps_double_.size(); ++i){

    edm::Handle< edm::ValueMap<double> > vmapD;
    iEvent.getByLabel(vmaps_double_.at(i), vmapD);
    vmapDs.push_back(vmapD);
  }

  std::auto_ptr< pat::MuonCollection > newMuons(new pat::MuonCollection);
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

  iEvent.put(newMuons);

  return;
}

DEFINE_FWK_MODULE(PATMuonUserData);
