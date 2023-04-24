#include <memory>
#include <vector>
#include <string>

#include <FWCore/Framework/interface/Frameworkfwd.h>
#include <FWCore/Framework/interface/one/EDProducer.h>
#include <FWCore/Framework/interface/Event.h>
#include <FWCore/Framework/interface/MakerMacros.h>
#include <FWCore/ParameterSet/interface/ParameterSet.h>

#include <DataFormats/PatCandidates/interface/Photon.h>

// Possibly not needed? In any case, moved to this location:
//#include <CommonTools/Egamma/interface/EffectiveAreas.h>

class PATPhotonUserData : public edm::one::EDProducer<> {
 public:
  explicit PATPhotonUserData(const edm::ParameterSet&);
  virtual ~PATPhotonUserData() {}

 private:
  virtual void produce(edm::Event&, const edm::EventSetup&);

  edm::EDGetTokenT< edm::View<pat::Photon> > src_;
  edm::EDGetTokenT< edm::View<reco::Vertex> > vertices_;

  struct vmap_link {
    std::string     vname;
    edm::EDGetToken token;
  };

  edm::ParameterSet      vmaps_bool_;
  std::vector<vmap_link> vmaps_bool_links_;

  edm::ParameterSet      vmaps_float_;
  std::vector<vmap_link> vmaps_float_links_;

  std::vector<std::string> vmaps_double_;
  std::vector<edm::EDGetToken> vmaps_token_;

};

PATPhotonUserData::PATPhotonUserData(const edm::ParameterSet& iConfig){

  src_ = consumes< edm::View<pat::Photon> >(iConfig.getParameter<edm::InputTag>("src"));

  if(iConfig.exists("vmaps_bool")) vmaps_bool_ = iConfig.getParameter<edm::ParameterSet>("vmaps_bool");
  for(unsigned int i=0; i<vmaps_bool_.getParameterNames().size(); ++i){

    vmap_link link;
    link.vname = vmaps_bool_.getParameterNames().at(i);
    link.token = consumes< edm::ValueMap<bool> >(vmaps_bool_.getParameter<edm::InputTag>(link.vname));

    vmaps_bool_links_.push_back(link);
  }

  if(iConfig.exists("vmaps_float")) vmaps_float_ = iConfig.getParameter<edm::ParameterSet>("vmaps_float");
  for(unsigned int i=0; i<vmaps_float_.getParameterNames().size(); ++i){

    vmap_link link;
    link.vname = vmaps_float_.getParameterNames().at(i);
    link.token = consumes< edm::ValueMap<float> >(vmaps_float_.getParameter<edm::InputTag>(link.vname));

    vmaps_float_links_.push_back(link);
  }

  if(iConfig.exists("vmaps_double")) {
    vmaps_double_ = iConfig.getParameter<std::vector<std::string>>("vmaps_double");
    for(unsigned int i=0; i<vmaps_double_.size(); i++){
      vmaps_token_.push_back(consumes<edm::ValueMap<double> >(edm::InputTag(vmaps_double_.at(i))));
    }
  }

  produces< pat::PhotonCollection >();
}

void PATPhotonUserData::produce(edm::Event& iEvent, const edm::EventSetup& iSetup){

  edm::Handle< edm::View<pat::Photon> > patPhons;
  iEvent.getByToken(src_, patPhons);

  std::vector< edm::Handle< edm::ValueMap<bool> > > vmapBs;
  for(unsigned int i=0; i<vmaps_bool_links_.size(); ++i){

    edm::Handle< edm::ValueMap<bool> > vmapB;
    iEvent.getByToken(vmaps_bool_links_.at(i).token, vmapB);
    vmapBs.push_back(vmapB);
  }

  std::vector< edm::Handle< edm::ValueMap<float> > > vmapFs;
  for(unsigned int i=0; i<vmaps_float_links_.size(); ++i){

    edm::Handle< edm::ValueMap<float> > vmapF;
    iEvent.getByToken(vmaps_float_links_.at(i).token, vmapF);
    vmapFs.push_back(vmapF);
  }

  std::vector< edm::Handle< edm::ValueMap<double> > > vmapDs;
  for(unsigned int i=0; i<vmaps_token_.size(); ++i){

    edm::Handle< edm::ValueMap<double> > vmapD;
    iEvent.getByToken(vmaps_token_.at(i), vmapD);
    vmapDs.push_back(vmapD);
  }

  std::unique_ptr< pat::PhotonCollection > newPhons(new pat::PhotonCollection);
  newPhons->reserve(patPhons->size());

  for(unsigned int i=0; i<patPhons->size(); ++i){

    newPhons->push_back(patPhons->at(i));
    pat::Photon& pho = newPhons->back();

    for(unsigned int j=0; j<vmapBs.size(); ++j){

      if(pho.hasUserInt(vmaps_bool_links_.at(j).vname))
        throw cms::Exception("InputError") << "@@@ PATPhotonUserData::produce -- PAT user-int label already exists: " << vmaps_bool_links_.at(j).vname;

      const bool& val = (*(vmapBs.at(j)))[patPhons->refAt(i)];
      pho.addUserInt(vmaps_bool_links_.at(j).vname, int(val));
    }

    for(unsigned int j=0; j<vmapFs.size(); ++j){

      if(pho.hasUserFloat(vmaps_float_links_.at(j).vname))
        throw cms::Exception("InputError") << "@@@ PATPhotonUserData::produce -- PAT user-float label already exists: " << vmaps_float_links_.at(j).vname;

      const float& val = (*(vmapFs.at(j)))[patPhons->refAt(i)];
      pho.addUserFloat(vmaps_float_links_.at(j).vname, float(val));
    }

    for(unsigned int j=0; j<vmapDs.size(); ++j){

      if(pho.hasUserFloat(vmaps_double_.at(j)))
        throw cms::Exception("InputError") << "@@@ PATPhotonUserData::produce -- PAT user-float label already exists: " << vmaps_double_.at(j);

      const double& val = (*(vmapDs.at(j)))[patPhons->refAt(i)];
      pho.addUserFloat(vmaps_double_.at(j), float(val));
    }
  }

  iEvent.put(std::move(newPhons));

  return;
}

DEFINE_FWK_MODULE(PATPhotonUserData);
