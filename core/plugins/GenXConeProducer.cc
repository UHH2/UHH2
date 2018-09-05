// -*- C++ -*-
//
// Package:    UHH2/GenXConeProducer
// Class:      GenXConeProducer
//
/**\class GenXConeProducer GenXConeProducer.cc UHH2/GenXConeProducer/plugins/GenXConeProducer.cc

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
#include "FWCore/Framework/interface/stream/EDProducer.h"

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

#include "vector"

using namespace fastjet;
using namespace contrib;

//
// class declaration
//

class GenXConeProducer : public edm::stream::EDProducer<> {
  public:
    explicit GenXConeProducer(const edm::ParameterSet&);
    ~GenXConeProducer();

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

  private:
    virtual void beginStream(edm::StreamID) override;
    virtual void produce(edm::Event&, const edm::EventSetup&) override;
    virtual void endStream() override;

    virtual pat::Jet createPatJet(const fastjet::PseudoJet &);
    virtual void initPlugin(std::unique_ptr<NjettinessPlugin> & ptr, int N, double R0, double beta, bool usePseudoXCone);

    // ----------member data ---------------------------
    edm::EDGetToken src_token_;
    const std::string subjetCollName_;
    const bool usePseudoXCone_;
    const bool doLeptonSpecific_;
    unsigned int NJets_ = 2;
    double RJets_ = 1.2;
    double BetaJets_ = 2.0;
    unsigned int NSubJets_ = 3;
    double RSubJets_ = 0.4;
    double BetaSubJets_ = 2.0;
    double DRLeptonJet_ = 999.;
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
GenXConeProducer::GenXConeProducer(const edm::ParameterSet& iConfig):
  src_token_(consumes<edm::View<reco::Candidate>>(iConfig.getParameter<edm::InputTag>("src"))),
  subjetCollName_("SubJets"),
  usePseudoXCone_(iConfig.getParameter<bool>("usePseudoXCone")),
  doLeptonSpecific_(iConfig.getParameter<bool>("doLeptonSpecific")),
  NJets_(iConfig.getParameter<unsigned int>("NJets")),
  RJets_(iConfig.getParameter<double>("RJets")),
  BetaJets_(iConfig.getParameter<double>("BetaJets")),
  NSubJets_(iConfig.getParameter<unsigned int>("NSubJets")),
  RSubJets_(iConfig.getParameter<double>("RSubJets")),
  BetaSubJets_(iConfig.getParameter<double>("BetaSubJets")),
  DRLeptonJet_(iConfig.exists("DRLeptonJet") ? iConfig.getParameter<double>("DRLeptonJet") : 999.)
{
  // We make both the fat jets and subjets, and we must store them as separate collections
  produces<pat::JetCollection>();
  produces<pat::JetCollection>(subjetCollName_);
}


GenXConeProducer::~GenXConeProducer()
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
GenXConeProducer::initPlugin(std::unique_ptr<NjettinessPlugin> & ptr, int N, double R0, double beta, bool usePseudoXCone)
{
  if (usePseudoXCone) {
    ptr.reset(new PseudoXConePlugin(N, R0, beta));
  } else {
    ptr.reset(new XConePlugin(N, R0, beta));
  }
}


// ------------ method called to produce the data  ------------
void
GenXConeProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
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

  const reco::Candidate * lepton(nullptr);
  float lepton_max_pt = 0;

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

    if (doLeptonSpecific_) {
      uint pdgid = abs(cand.pdgId());
      auto candPt = cand.pt();
      if ((pdgid== 11 || pdgid == 13) && (candPt > lepton_max_pt)) {
        lepton = &cand;
        lepton_max_pt = candPt;
      }
    }
  }

  if (doLeptonSpecific_ && (lepton == nullptr)) {
    edm::LogWarning("NoGenXConeLepton") << "No lepton found in GenXConeProducer" << std::endl;
  }


  // make sure to have enough particles with non-zero momentum in event
  unsigned int minParticleCount = NJets_ * NSubJets_;
  if (doLeptonSpecific_ && (lepton != nullptr)) minParticleCount -= 1;  // since one jet will use NSubJets_-1

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

  // Run clustering of fatjets
  vector<PseudoJet> fatjets;
  std::unique_ptr<NjettinessPlugin> plugin_xcone;
  initPlugin(plugin_xcone, NJets_, RJets_, BetaJets_, usePseudoXCone_);
  JetDefinition jet_def_xcone(plugin_xcone.get());
  ClusterSequence clust_seq_xcone(_psj, jet_def_xcone);
  fatjets = sorted_by_pt(clust_seq_xcone.inclusive_jets(0));

  if (fatjets.size() != NJets_) {
    edm::LogWarning("GenXConeTooFewJets") << "XConePlugin has only found " << fatjets.size() << " jets but requested " << NJets_;
  }
  // Note to future dev: if you want to add SoftDrop, you must use the full
  // constructor, otherwise your fatjet will only have 1 constitutent,
  // making it useless for subjets. It will also ensure that the proper
  // reclustering with C/A will happen

  // check if subjets should be clustered
  bool doSubjets = true;
  if(NSubJets_ == 0) doSubjets = false;

  // first get and write list of particles in fatjets:
  // if particle i is clustered in jet j, the i-th entry of the list == j
  vector<int> list_fat;
  list_fat.clear();
  list_fat = clust_seq_xcone.particle_jet_indices(fatjets);
  // this is the mapping of subjet to hard jet
  std::vector< std::vector<int> > indices;
  indices.resize(fatjets.size());

  // For lepton specific clustering, we find the fatjet closest to the lepton,
  // and then use N-1 on its constituents.
  uint lepton_jet_ind = 999;
  if (doLeptonSpecific_ && (lepton != nullptr)) {
    double dr_min = 999;
    for (uint iFj=0; iFj < fatjets.size(); iFj++) {
      const auto & fj = fatjets[iFj];
      double this_dr = deltaR(fj, *lepton);
      if (this_dr < dr_min && this_dr < DRLeptonJet_) {
        dr_min = this_dr;
        lepton_jet_ind = iFj;
      }
    }
  }

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

    auto thisNSubJets_ = NSubJets_;
    if (doLeptonSpecific_ && (i == lepton_jet_ind)) {
      --thisNSubJets_;
    }

    // check if there are more particles then required subjets
    bool enoughParticles = (particle_in_fatjet.size() > thisNSubJets_);

    // Run second clustering step (subjets) for each fat jet
    vector<PseudoJet> subjets;
    //ClusterSequence * clust_seq_sub;
    if (enoughParticles && doSubjets) {
      std::unique_ptr<NjettinessPlugin> plugin_xcone_sub;
      initPlugin(plugin_xcone_sub, thisNSubJets_, RSubJets_, BetaSubJets_, usePseudoXCone_);
      JetDefinition jet_def_sub(plugin_xcone_sub.get());
      ClusterSequence * clust_seq_sub = new ClusterSequence(particle_in_fatjet, jet_def_sub);
      // subjets = sorted_by_pt(clust_seq_sub->inclusive_jets());
      subjets = clust_seq_sub->inclusive_jets();
    }


    if (subjets.size() != thisNSubJets_) {
      edm::LogWarning("GenXConeTooFewSubjets") << "Only found " << subjets.size() << " subjets but requested " << thisNSubJets_ << ". "
          << " Fatjet had " << particle_in_fatjet.size() << " constituents.\n"
          << "Have added in blank subjets to make " << thisNSubJets_ << " subjets." << endl;
      for (uint iSub=subjets.size(); iSub < thisNSubJets_; iSub++) {
        subjets.push_back(PseudoJet(0, 0, 0, 0));
      }
    }

    // pat-ify fatjets
    auto patJet = createPatJet(fatjets[i]);
    jetCollection->push_back(patJet);

    for (uint s=0; s<subjets.size(); s++) {
      indices[i].push_back(subjetCollection->size());
      auto subjet = createPatJet(subjets[s]);
      subjetCollection->push_back(subjet);
    }

  }

  // Following inspired by CompoundJetProducer/VirtualJetProducer
  edm::OrphanHandle<pat::JetCollection> subjetHandleAfterPut = iEvent.put(std::move(subjetCollection), subjetCollName_);

  // setup refs between jets & subjets using indices of subjets in the SubjetCollection
  int jetInd = 0;
  for (auto & jetItr : *jetCollection) {
    std::vector<int> & ind = indices[jetInd];
    pat::JetPtrCollection subjetPtrs;
    for (const auto indItr : ind) {
      edm::Ptr<pat::Jet> subjetPtr(subjetHandleAfterPut, indItr);
      subjetPtrs.push_back(subjetPtr);
    }
    jetItr.addSubjets(subjetPtrs);
    jetInd++;
  }
  iEvent.put(std::move(jetCollection));
}

pat::Jet GenXConeProducer::createPatJet(const PseudoJet & psj)
{
  pat::Jet newJet;
  newJet.setP4(math::XYZTLorentzVector(psj.px(), psj.py(), psj.pz(), psj.E()));
  return newJet;
}

// ------------ method called once each stream before processing any runs, lumis or events  ------------
void
GenXConeProducer::beginStream(edm::StreamID)
{
}

// ------------ method called once each stream after processing all runs, lumis and events  ------------
void
GenXConeProducer::endStream() {
}


// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
GenXConeProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(GenXConeProducer);
