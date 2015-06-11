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

#include <EgammaAnalysis/ElectronTools/interface/EGammaMvaEleEstimatorCSA14.h>

// class declaration
class PATElectronUserData : public edm::EDProducer {
 public:
  explicit PATElectronUserData(const edm::ParameterSet&);
  ~PATElectronUserData() {}

 private:
  virtual void produce(edm::Event&, const edm::EventSetup&);

  edm::EDGetTokenT< edm::View<pat::Electron> > src_;
  std::vector<std::string> float_vmaps_;

  std::vector<std::string> weights_mvaNoTrig_;
  std::unique_ptr<EGammaMvaEleEstimatorCSA14> egammaMVAnotrig;

  std::vector<std::string> weights_mvaTrig_;
  std::unique_ptr<EGammaMvaEleEstimatorCSA14> egammaMVAtrig;
};

// constructor
PATElectronUserData::PATElectronUserData(const edm::ParameterSet& iConfig){

  src_ = consumes< edm::View<pat::Electron> >(iConfig.getParameter<edm::InputTag>("src"));
  if(iConfig.exists("float_vmaps")) float_vmaps_ = iConfig.getParameter<std::vector<std::string>>("float_vmaps");

  if(iConfig.exists("weights_mvaTrig")){

    weights_mvaTrig_ = iConfig.getParameter<std::vector<std::string>>("weights_mvaTrig");

    std::vector<std::string> weights_mvaTrig_FP;
    weights_mvaTrig_FP.reserve(weights_mvaTrig_.size());
    for(int i=0; i<int(weights_mvaTrig_.size()); ++i){
      weights_mvaTrig_FP.push_back(edm::FileInPath(weights_mvaTrig_.at(i)).fullPath());
    }
    
    egammaMVAtrig.reset(new EGammaMvaEleEstimatorCSA14());
    egammaMVAtrig->initialize("BDT", EGammaMvaEleEstimatorCSA14::kTrig, true, weights_mvaTrig_FP);
  }

  if(iConfig.exists("weights_mvaNoTrig")){

    weights_mvaNoTrig_ = iConfig.getParameter<std::vector<std::string>>("weights_mvaNoTrig");

    std::vector<std::string> weights_mvaNoTrig_FP;
    weights_mvaNoTrig_FP.reserve(weights_mvaNoTrig_.size());
    for(int i=0; i<int(weights_mvaNoTrig_.size()); ++i){
      weights_mvaNoTrig_FP.push_back(edm::FileInPath(weights_mvaNoTrig_.at(i)).fullPath());
    }
    
    egammaMVAnotrig.reset(new EGammaMvaEleEstimatorCSA14());
    egammaMVAnotrig->initialize("BDT", EGammaMvaEleEstimatorCSA14::kNonTrig, true, weights_mvaNoTrig_FP);
  }

  produces< pat::ElectronCollection >();
}

// ------------ method called to produce the data  ------------
void PATElectronUserData::produce(edm::Event& iEvent, const edm::EventSetup& iSetup){

  edm::Handle< edm::View<pat::Electron> > patElecs;
  iEvent.getByToken(src_, patElecs);

  std::vector< edm::Handle< edm::ValueMap<double> > > fvmaps;
  for(int i=0; i<int(float_vmaps_.size()); ++i){

    edm::Handle< edm::ValueMap<double> > fvmap;
    iEvent.getByLabel(float_vmaps_.at(i), fvmap);
    fvmaps.push_back(fvmap);
  }

  std::auto_ptr< pat::ElectronCollection > newElecs(new pat::ElectronCollection);
  newElecs->reserve(patElecs->size());

  for(int i=0; i<int(patElecs->size()); ++i){

    newElecs->push_back(patElecs->at(i));
    pat::Electron& ele = newElecs->back();

    float notrigMVA = egammaMVAnotrig->mvaValue(ele, false);
    float trigMVA   = egammaMVAtrig  ->mvaValue(ele, false);

    ele.addUserFloat("mvaNoTrig", notrigMVA);
    ele.addUserFloat("mvaTrig", trigMVA);

    for(int j=0; j<int(fvmaps.size()); ++j){

      if(ele.hasUserFloat(float_vmaps_.at(j)))
	throw cms::Exception("InputError") << "@@@ PATElectronUserData::produce -- PAT user-float label already exists: " << float_vmaps_.at(j);

      const double& val = (*(fvmaps.at(j)))[patElecs->refAt(i)];
      ele.addUserFloat(float_vmaps_.at(j), float(val));
    }
  }

  iEvent.put(newElecs);

  return;
}

DEFINE_FWK_MODULE(PATElectronUserData);
