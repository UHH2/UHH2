#include <memory>
#include <vector>
#include <string>

#include <FWCore/Framework/interface/Frameworkfwd.h>
#include <FWCore/Framework/interface/one/EDProducer.h>
#include <FWCore/Framework/interface/Event.h>
#include <FWCore/Framework/interface/MakerMacros.h>
#include <FWCore/ParameterSet/interface/ParameterSet.h>

#include <DataFormats/PatCandidates/interface/Electron.h>
#include <CommonTools/Egamma/interface/EffectiveAreas.h>

class PATElectronUserData : public edm::one::EDProducer<> {
 public:
  explicit PATElectronUserData(const edm::ParameterSet&);
  virtual ~PATElectronUserData() {}

 private:
  virtual void produce(edm::Event&, const edm::EventSetup&);

  edm::EDGetTokenT< edm::View<pat::Electron> > src_;
  edm::EDGetTokenT< edm::View<reco::Vertex> > vertices_;

  struct vmap_link {
    std::string     vname;
    edm::EDGetToken token;
  };

  std::vector<std::string> vmaps_double_;
  std::vector<edm::EDGetToken> vmaps_token_;

  EffectiveAreas effAreas_;

  std::string mva_GeneralPurpose_;
  std::string mva_HZZ_;

  std::string mva_Iso_;
  std::string mva_NoIso_;
};

PATElectronUserData::PATElectronUserData(const edm::ParameterSet& iConfig):
  effAreas_(iConfig.getParameter<edm::FileInPath>("effAreas_file").fullPath()) {

  src_ = consumes< edm::View<pat::Electron> >(iConfig.getParameter<edm::InputTag>("src"));

  if(iConfig.exists("vmaps_double")) {
    vmaps_double_ = iConfig.getParameter<std::vector<std::string>>("vmaps_double");
    for(unsigned int i=0; i<vmaps_double_.size(); i++){
      vmaps_token_.push_back(consumes<edm::ValueMap<double> >(edm::InputTag(vmaps_double_.at(i))));
    }
  }

  produces< pat::ElectronCollection >();
}

void PATElectronUserData::produce(edm::Event& iEvent, const edm::EventSetup& iSetup){

  edm::Handle< edm::View<pat::Electron> > patElecs;
  iEvent.getByToken(src_, patElecs);

  std::vector< edm::Handle< edm::ValueMap<double> > > vmapDs;
  for(unsigned int i=0; i<vmaps_token_.size(); ++i){

    edm::Handle< edm::ValueMap<double> > vmapD;
    //iEvent.getByLabel(vmaps_double_.at(i), vmapD);
    iEvent.getByToken(vmaps_token_.at(i), vmapD);
    vmapDs.push_back(vmapD);
  }

  std::unique_ptr< pat::ElectronCollection > newElecs(new pat::ElectronCollection);
  newElecs->reserve(patElecs->size());

  for(unsigned int i=0; i<patElecs->size(); ++i){

    newElecs->push_back(patElecs->at(i));
    pat::Electron& ele = newElecs->back();

    for(unsigned int j=0; j<vmapDs.size(); ++j){

      if(ele.hasUserFloat(vmaps_double_.at(j)))
        throw cms::Exception("InputError") << "@@@ PATElectronUserData::produce -- PAT user-float label already exists: " << vmaps_double_.at(j);

      const double& val = (*(vmapDs.at(j)))[patElecs->refAt(i)];
      ele.addUserFloat(vmaps_double_.at(j), float(val));
    }

    const float eA  = effAreas_.getEffectiveArea(fabs(ele.superCluster()->eta()));
    ele.addUserFloat("EffArea", eA);
  }

  iEvent.put(std::move(newElecs));

  return;
}

DEFINE_FWK_MODULE(PATElectronUserData);
