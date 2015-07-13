// c++ include files
#include <memory>
#include <vector>
#include <string>

// default include files
#include <FWCore/Framework/interface/Frameworkfwd.h>
#include <FWCore/Framework/interface/EDProducer.h>
#include <FWCore/Framework/interface/Event.h>
#include <FWCore/Framework/interface/MakerMacros.h>
#include <FWCore/ParameterSet/interface/ParameterSet.h>

// non-default include files
#include <DataFormats/PatCandidates/interface/Electron.h>

class PATElectronUserData : public edm::EDProducer {
 public:
  explicit PATElectronUserData(const edm::ParameterSet&);
  ~PATElectronUserData() {}

 private:
  virtual void produce(edm::Event&, const edm::EventSetup&);

  edm::EDGetTokenT< edm::View<pat::Electron> > src_;
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

  std::string mva_NoTrig_;
  std::string mva_Trig_;
};

PATElectronUserData::PATElectronUserData(const edm::ParameterSet& iConfig){

  src_ = consumes< edm::View<pat::Electron> >(iConfig.getParameter<edm::InputTag>("src"));

  vmaps_bool_ = iConfig.getParameter<edm::ParameterSet>("vmaps_bool");
  for(unsigned int i=0; i<vmaps_bool_.getParameterNames().size(); ++i){

    vmap_link link;
    link.vname = vmaps_bool_.getParameterNames().at(i);
    link.token = consumes< edm::ValueMap<bool> >(vmaps_bool_.getParameter<edm::InputTag>(link.vname));

    vmaps_bool_links_.push_back(link);
  }

  vmaps_float_ = iConfig.getParameter<edm::ParameterSet>("vmaps_float");
  for(unsigned int i=0; i<vmaps_float_.getParameterNames().size(); ++i){

    vmap_link link;
    link.vname = vmaps_float_.getParameterNames().at(i);
    link.token = consumes< edm::ValueMap<float> >(vmaps_float_.getParameter<edm::InputTag>(link.vname));

    vmaps_float_links_.push_back(link);
  }

  if(iConfig.exists("vmaps_double")) vmaps_double_ = iConfig.getParameter<std::vector<std::string>>("vmaps_double");

  if(iConfig.exists("mva_NoTrig")) mva_NoTrig_ = iConfig.getParameter<std::string>("mva_NoTrig");
  if(iConfig.exists("mva_Trig"))   mva_Trig_   = iConfig.getParameter<std::string>("mva_Trig");

  produces< pat::ElectronCollection >();
}

void PATElectronUserData::produce(edm::Event& iEvent, const edm::EventSetup& iSetup){

  edm::Handle< edm::View<pat::Electron> > patElecs;
  iEvent.getByToken(src_, patElecs);

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
  for(unsigned int i=0; i<vmaps_double_.size(); ++i){

    edm::Handle< edm::ValueMap<double> > vmapD;
    iEvent.getByLabel(vmaps_double_.at(i), vmapD);
    vmapDs.push_back(vmapD);
  }

  std::auto_ptr< pat::ElectronCollection > newElecs(new pat::ElectronCollection);
  newElecs->reserve(patElecs->size());

  for(unsigned int i=0; i<patElecs->size(); ++i){

    newElecs->push_back(patElecs->at(i));
    pat::Electron& ele = newElecs->back();

    for(unsigned int j=0; j<vmapBs.size(); ++j){

      if(ele.hasUserInt(vmaps_bool_links_.at(j).vname))
	throw cms::Exception("InputError") << "@@@ PATElectronUserData::produce -- PAT user-int label already exists: " << vmaps_bool_links_.at(j).vname;

      const bool& val = (*(vmapBs.at(j)))[patElecs->refAt(i)];
      ele.addUserInt(vmaps_bool_links_.at(j).vname, int(val));
    }

    for(unsigned int j=0; j<vmapFs.size(); ++j){

      if(ele.hasUserFloat(vmaps_float_links_.at(j).vname))
	throw cms::Exception("InputError") << "@@@ PATElectronUserData::produce -- PAT user-float label already exists: " << vmaps_float_links_.at(j).vname;

      const float& val = (*(vmapFs.at(j)))[patElecs->refAt(i)];
      ele.addUserFloat(vmaps_float_links_.at(j).vname, float(val));
    }

    for(unsigned int j=0; j<vmapDs.size(); ++j){

      if(ele.hasUserFloat(vmaps_double_.at(j)))
	throw cms::Exception("InputError") << "@@@ PATElectronUserData::produce -- PAT user-float label already exists: " << vmaps_double_.at(j);

      const double& val = (*(vmapDs.at(j)))[patElecs->refAt(i)];
      ele.addUserFloat(vmaps_double_.at(j), float(val));
    }

    if(ele.hasUserFloat(mva_NoTrig_)) throw cms::Exception("InputError") << "@@@ PATElectronUserData::produce -- PAT user-float for 'mvaNoTrig' not found";
    if(ele.hasUserFloat(mva_Trig_))   throw cms::Exception("InputError") << "@@@ PATElectronUserData::produce -- PAT user-float for 'mvaTrig' not found";

    const float notrigMVA = ele.userFloat(mva_NoTrig_);
    const float trigMVA   = ele.userFloat(mva_Trig_);

    ele.addUserFloat("mvaNoTrig", notrigMVA);
    ele.addUserFloat("mvaTrig"  ,   trigMVA);
  }

  iEvent.put(newElecs);

  return;
}

DEFINE_FWK_MODULE(PATElectronUserData);
