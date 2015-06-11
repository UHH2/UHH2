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
  std::vector<std::string> float_vmaps_;
};

// constructor
PATMuonUserData::PATMuonUserData( const edm::ParameterSet& iConfig ){

  src_ = consumes< edm::View<pat::Muon> >(iConfig.getParameter<edm::InputTag>("src"));
  if(iConfig.exists("float_vmaps")) float_vmaps_ = iConfig.getParameter<std::vector<std::string>>("float_vmaps");

  produces< pat::MuonCollection >();
}

// ------------ method called to produce the data  ------------
void PATMuonUserData::produce(edm::Event& iEvent, const edm::EventSetup& iSetup){

  edm::Handle< edm::View<pat::Muon> > patMuons;
  iEvent.getByToken(src_, patMuons);

  std::vector< edm::Handle< edm::ValueMap<double> > > fvmaps;
  for(int i=0; i<int(float_vmaps_.size()); ++i){

    edm::Handle< edm::ValueMap<double> > fvmap;
    iEvent.getByLabel(float_vmaps_.at(i), fvmap);
    fvmaps.push_back(fvmap);
  }

  std::auto_ptr< pat::MuonCollection > newMuons(new pat::MuonCollection);
  newMuons->reserve(patMuons->size());

  for(int i=0; i<int(patMuons->size()); ++i){

    newMuons->push_back(patMuons->at(i));
    pat::Muon& muo = newMuons->back();

    for(int j=0; j<int(fvmaps.size()); ++j){

      if(muo.hasUserFloat(float_vmaps_.at(j)))
        throw cms::Exception("InputError") << "@@@ PATMuonUserData::produce -- PAT user-float label already exists: " << float_vmaps_.at(j);

      const double& val = (*(fvmaps.at(j)))[patMuons->refAt(i)];
      muo.addUserFloat(float_vmaps_.at(j), float(val));
    }
  }

  iEvent.put(newMuons);

  return;
}

DEFINE_FWK_MODULE(PATMuonUserData);
