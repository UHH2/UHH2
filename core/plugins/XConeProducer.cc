// -*- C++ -*-
//
// Package:    UHH2/XConeProducer
// Class:      XConeProducer
//
/**\class XConeProducer XConeProducer.cc UHH2/XConeProducer/plugins/XConeProducer.cc

 Description: [one line class summary]

 Implementation:
    [Notes on implementation]
*/
//
// Original Author:  Robin Aggleton
//         Created:  Tue, 23 Jan 2018 13:27:56 GMT
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/global/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"

#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/PatCandidates/interface/Jet.h"

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

class XConeProducer : public edm::global::EDProducer<> {
  public:
    explicit XConeProducer(const edm::ParameterSet&);
    ~XConeProducer();

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

  private:
    virtual void produce(edm::StreamID, edm::Event&, const edm::EventSetup&) const override;

    virtual pat::Jet createPatJet(const fastjet::PseudoJet &) const;
    virtual void initPlugin(std::unique_ptr<NjettinessPlugin> & ptr, int N, double R0, double beta, bool usePseudoXCone) const;

    // ----------member data ---------------------------
    edm::EDGetToken src_token_;
    const std::string subjetCollName_;
    const bool usePseudoXCone_;
    unsigned int NJets_ = 2;
    double RJets_ = 1.2;
    double BetaJets_ = 2.0;
    unsigned int NSubJets_ = 3;
    double RSubJets_ = 0.4;
    double BetaSubJets_ = 2.0;
};

//
// constants, enums and typedefs
//


//
// static data member definitions
//

//
// constructors and destructor
//
XConeProducer::XConeProducer(const edm::ParameterSet& iConfig):
  src_token_(consumes<edm::View<reco::Candidate>>(iConfig.getParameter<edm::InputTag>("src"))),
  subjetCollName_("SubJets"),
  usePseudoXCone_(iConfig.getParameter<bool>("usePseudoXCone")),
  NJets_(iConfig.getParameter<unsigned int>("NJets")),
  RJets_(iConfig.getParameter<double>("RJets")),
  BetaJets_(iConfig.getParameter<double>("BetaJets")),
  NSubJets_(iConfig.getParameter<unsigned int>("NSubJets")),
  RSubJets_(iConfig.getParameter<double>("RSubJets")),
  BetaSubJets_(iConfig.getParameter<double>("BetaSubJets"))
{
  // We make both the fat jets and subjets, and we must store them as separate collections
  produces<pat::JetCollection>();
  produces<pat::JetCollection>(subjetCollName_);
}


XConeProducer::~XConeProducer()
{

  // do anything here that needs to be done at destruction time
  // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// Setup either XConePlugin or PseudoXConePlugin and assign to ptr
// Use NjettinessPlugin as it is the base class of both XConePlugin and PseudoXConePlugin
void
XConeProducer::initPlugin(std::unique_ptr<NjettinessPlugin> & ptr, int N, double R0, double beta, bool usePseudoXCone) const
{
  if (usePseudoXCone) {
    ptr.reset(new PseudoXConePlugin(N, R0, beta));
  } else {
    ptr.reset(new XConePlugin(N, R0, beta));
  }
}


// ------------ method called to produce the data  ------------
void
XConeProducer::produce(edm::StreamID id, edm::Event& iEvent, const edm::EventSetup& iSetup) const
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

  // make sure to have enough particles with non-zero momentum in event
  unsigned int minParticleCount = NJets_ * 3;
  if (_psj.size() < minParticleCount) {
    auto jetCollection = std::make_unique<pat::JetCollection>();
    iEvent.put(std::move(jetCollection));
    auto subjetCollection = std::make_unique<pat::JetCollection>();
    iEvent.put(std::move(subjetCollection), subjetCollName_);
    return;
  }

  // declare jet collections
  auto jetCollection = std::make_unique<pat::JetCollection>();
  auto subjetCollection = std::make_unique<pat::JetCollection>();
  std::vector< std::vector<int> > indices;

  // Run clustering of fatjets
  vector<PseudoJet> fatjets;
  std::unique_ptr<NjettinessPlugin> plugin_xcone;
  initPlugin(plugin_xcone, NJets_, RJets_, BetaJets_, usePseudoXCone_);
  double ghost_maxrap = 4.0;      // maxiumum y of ghost particles
  AreaDefinition area_def(active_area, GhostedAreaSpec(ghost_maxrap));
  JetDefinition jet_def_xcone(plugin_xcone.get());
  ClusterSequence clust_seq_xcone(_psj, jet_def_xcone);
  fatjets = sorted_by_pt(clust_seq_xcone.inclusive_jets(0));

  // get SoftDrop Mass for every fat jet
  SoftDrop sd(0.0, 0.1, 1.2);
  vector<float> sd_mass;
  for(unsigned int i=0; i<fatjets.size(); i++){
    PseudoJet sdjet = sd(fatjets[i]);
    sd_mass.push_back(sdjet.m());
  }

  // check if subjets should be clustered
  bool doSubjets = true;
  if(NSubJets_ == 0) doSubjets = false;

  // first get and wirte list of particles in fatjets:
  // if particle i is clustered in jet j, the i-th entry of the list == j
  vector<int> list_fat;
  list_fat.clear();
  list_fat = clust_seq_xcone.particle_jet_indices(fatjets);

  // loop over all fatjets and cluster subjets
  for(unsigned int i=0; i<fatjets.size(); i++){
    // get set of particles in fatjet i
    vector<PseudoJet> particle_in_fatjet;
    for (unsigned int ipart=0; ipart < _psj.size(); ++ipart) {
      if (list_fat[ipart] < 0) continue;
      else if (abs(list_fat[ipart])==i) {
        particle_in_fatjet.push_back(_psj.at(ipart));
      }
    }

    // check if there are more particles then required subjets
    bool enoughParticles = (particle_in_fatjet.size() > NSubJets_);

    // Run second clustering step (subjets) for each fat jet
    vector<PseudoJet> subjets;
    vector<double> subjet_area;
    if (enoughParticles && doSubjets) {
      std::unique_ptr<NjettinessPlugin> plugin_xcone_sub;
      initPlugin(plugin_xcone_sub, NSubJets_, RSubJets_, BetaSubJets_, usePseudoXCone_);
      JetDefinition jet_def_sub(plugin_xcone_sub.get());
      ClusterSequenceArea clust_seq_sub(particle_in_fatjet, jet_def_sub, area_def);
      subjets = sorted_by_pt(clust_seq_sub.inclusive_jets(0));
      for (unsigned int j = 0; j < subjets.size(); ++j) subjet_area.push_back(subjets[j].area());
    }

    // jet area for fat jet
    double jet_area = 0;
    // double jet_area = fatjets[i].area();

    // pat-ify fatjets
    auto patJet = createPatJet(fatjets[i]);
    patJet.setJetArea(jet_area);
    patJet.addUserFloat("softdropmass", sd_mass[i]);
    jetCollection->push_back(patJet);

    indices.resize(jetCollection->size());
    for (uint s=0; s<subjets.size(); s++) {
      indices[i].push_back(subjetCollection->size());
      auto subjet = createPatJet(subjets[s]);
      subjet.setJetArea(subjet_area[s]);
      subjetCollection->push_back(subjet);
    }

  }

  edm::OrphanHandle<pat::JetCollection> subjetHandleAfterPut = iEvent.put(std::move(subjetCollection), subjetCollName_);

  // setup refs between jets & subjets
  for (auto & jetItr : *jetCollection) {
    for (const auto ind : indices) {
      pat::JetPtrCollection subjetPtrs;
      for (const auto indItr : ind) {
        edm::Ptr<pat::Jet> subjetPtr(subjetHandleAfterPut, indItr);
        subjetPtrs.push_back(subjetPtr);
      }
      jetItr.addSubjets(subjetPtrs);
    }
  }
  iEvent.put(std::move(jetCollection));
}

pat::Jet XConeProducer::createPatJet(const PseudoJet & psj) const
{
  pat::Jet newJet;
  newJet.setP4(math::XYZTLorentzVector(psj.px(), psj.py(), psj.pz(), psj.E()));
  return newJet;
}


// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
XConeProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(XConeProducer);
