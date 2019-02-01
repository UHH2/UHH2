// -*- C++ -*-
//
// Package:    UHH2/GenHOTVRProducer
// Class:      GenHOTVRProducer
//
/**\class GenHOTVRProducer GenHOTVRProducer.cc UHH2/GenHOTVRProducer/plugins/GenHOTVRProducer.cc

 Description: [one line class summary]

 Implementation:
    [Notes on implementation]
*/
//
// Original Author:  Robin Aggleton
//         Created:  Tue, 23 Jan 2018 13:27:50 GMT
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
#include "DataFormats/Math/interface/LorentzVector.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "RecoJets/JetProducers/interface/JetSpecific.h"

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

class GenHOTVRProducer : public edm::stream::EDProducer<> {
  public:
    explicit GenHOTVRProducer(const edm::ParameterSet&);
    ~GenHOTVRProducer();

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
  virtual std::vector<reco::CandidatePtr> getConstituents(const std::vector<fastjet::PseudoJet>&fjConstituents);

  private:
    virtual void beginStream(edm::StreamID) override;
    virtual void produce(edm::Event&, const edm::EventSetup&) override;
    virtual void endStream() override;

    virtual pat::Jet createPatJet(const PseudoJet &, const edm::EventSetup&);


    // ----------member data ---------------------------
    edm::EDGetToken src_token_;
    const std::string subjetCollName_;
    double mu,                 // massjump threshold
      theta,                   // massjump parameter
      max_r,                   // maximum allowed distance R
      min_r,                   // minimum allowed distance R
      rho,                     // cone shrinking parameter
      hotvr_pt_min;            // minimum pT of subjet
    std::vector<edm::Ptr<reco::Candidate> > particles_;
    reco::Particle::Point vertex_;
  edm::EDGetTokenT<reco::VertexCollection> input_vertex_token_;
};

//
// constructors and destructor
//
GenHOTVRProducer::GenHOTVRProducer(const edm::ParameterSet& iConfig):
  src_token_(consumes<edm::View<reco::Candidate>>(iConfig.getParameter<edm::InputTag>("src"))),
  subjetCollName_("SubJets"),
  mu(iConfig.getParameter<double>("mu")),
  theta(iConfig.getParameter<double>("theta")),
  max_r(iConfig.getParameter<double>("max_r")),
  min_r(iConfig.getParameter<double>("min_r")),
  rho(iConfig.getParameter<double>("rho")),
  hotvr_pt_min(iConfig.getParameter<double>("hotvr_pt_min"))
{
  produces<pat::JetCollection>();
  produces<pat::JetCollection>(subjetCollName_);
  input_vertex_token_ = consumes<reco::VertexCollection>(edm::InputTag("offlineSlimmedPrimaryVertices"));
}


GenHOTVRProducer::~GenHOTVRProducer()
{

  // do anything here that needs to be done at destruction time
  // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called to produce the data  ------------
void
GenHOTVRProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
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
  //copy vertex from pv-collection, see example https://github.com/cms-sw/cmssw/blob/master/RecoJets/JetProducers/plugins/VirtualJetProducer.cc#L292
  edm::Handle<reco::VertexCollection> pvCollection;
  iEvent.getByToken(input_vertex_token_ , pvCollection);
  if (!pvCollection->empty()) vertex_=pvCollection->begin()->position();
  else  vertex_=reco::Jet::Point(0,0,0);
  particles_.clear(); 

  edm::Handle<edm::View<reco::Candidate>> particles;
  iEvent.getByToken(src_token_, particles);

  // Convert particles to PseudoJets
  std::vector<PseudoJet> _psj;
  int i=0;
  int i_gl=-1;
  for (const auto & cand: *particles) {
    i_gl++;
    if (std::isnan(cand.px()) ||
        std::isnan(cand.py()) ||
        std::isnan(cand.pz()) ||
        std::isinf(cand.px()) ||
        std::isinf(cand.py()) ||
        std::isinf(cand.pz()) ||
        (cand.pt() == 0))
      continue;

    //    _psj.push_back(PseudoJet(cand.px(), cand.py(), cand.pz(), cand.energy()));
    PseudoJet tmp_particle = PseudoJet(cand.px(), cand.py(), cand.pz(), cand.energy());
    tmp_particle.set_user_index(i);//important: store index for later linking between clustered jet and constituence
    _psj.push_back(tmp_particle);
    particles_.push_back(particles->ptrAt(i_gl));
    i++;
  }

  // Do the clustering, make jets, nsub, store
  // jet definition with HOTVR plugin
  HOTVR hotvr_plugin(mu, theta, min_r, max_r, rho, hotvr_pt_min, HOTVR::CALIKE);
  JetDefinition jet_def(&hotvr_plugin);

   // Setup cluster sequence
  ClusterSequence cs(_psj, jet_def);
  vector<PseudoJet> hotvr_jets = hotvr_plugin.get_jets();

  auto jetCollection = std::make_unique<pat::JetCollection>();
  auto subjetCollection = std::make_unique<pat::JetCollection>();

  // this is the mapping of subjet to hard jet
  std::vector< std::vector<int> > indices;
  indices.resize(hotvr_jets.size());

  for (unsigned int i = 0; i < hotvr_jets.size(); ++i) {
    // Convert jet and subjets to pat::Jets
    auto thisPatJet = createPatJet(hotvr_jets[i], iSetup);
    // dummy for now
    thisPatJet.addUserFloat("tau1", 0);
    thisPatJet.addUserFloat("tau2", 0);
    thisPatJet.addUserFloat("tau3", 0);
    jetCollection->push_back(thisPatJet);

    HOTVRinfo hi = hotvr_jets[i].user_info<HOTVRinfo>();
    std::vector<PseudoJet> subjets = hi.subjets();
    for (uint s=0; s<subjets.size(); s++) {
      indices[i].push_back(subjetCollection->size());
      auto subjet = createPatJet(subjets[s], iSetup);
      subjetCollection->push_back(subjet);
    }

  } // end loop over jets
  
  // Following inspired by CompoundJetProducer/VirtualJetProducer
  edm::OrphanHandle<pat::JetCollection> subjetHandleAfterPut = iEvent.put(std::move(subjetCollection), subjetCollName_);

  // setup refs between jets & subjets using indices of subjets in the SubjetCollection
  uint jetInd = 0;
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


pat::Jet GenHOTVRProducer::createPatJet(const PseudoJet & fjJet, const edm::EventSetup& iSetup)
{

  pat::Jet patjet;
  if(fjJet.px()==0 && fjJet.py()==0 && fjJet.pz()==0){//jet or sub-jet was created artificially
    patjet.setP4(math::XYZTLorentzVector(fjJet.px(), fjJet.py(), fjJet.pz(), fjJet.E()));
  }
  else{//jet or sub-jet is real
    //inspired by https://github.com/cms-sw/cmssw/blob/master/RecoJets/JetProducers/plugins/VirtualJetProducer.cc#L687
    // get the constituents from fastjet
    std::vector<fastjet::PseudoJet> const & fjConstituents = fastjet::sorted_by_pt(fjJet.constituents());
    // convert them to CandidatePtr vector
    std::vector<reco::CandidatePtr> const & constituents = getConstituents(fjConstituents);
    // write the specifics to the jet (simultaneously sets 4-vector, vertex).
    // These are overridden functions that will call the appropriate
    // specific allocator.
    reco::Particle::LorentzVector jet4v = reco::Particle::LorentzVector(fjJet.px(), fjJet.py(), fjJet.pz(),fjJet.E());  
    /*    reco::PFJet pfjet;
	  reco::writeSpecific(pfjet,jet4v,vertex_,constituents, iSetup);// https://github.com/ahlinist/cmssw/blob/master/RecoJets/JetProducers/src/JetSpecific.cc#L91
	  patjet = pat::Jet(pfjet);
    */
    reco::GenJet genjet;
    reco::writeSpecific(genjet,jet4v,vertex_,constituents, iSetup);
    patjet = pat::Jet((reco::Jet)genjet);
  }
  return patjet;
}


// ------------ method called once each stream before processing any runs, lumis or events  ------------
void
GenHOTVRProducer::beginStream(edm::StreamID)
{
}

// ------------ method called once each stream after processing all runs, lumis and events  ------------
void
GenHOTVRProducer::endStream() {
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
GenHOTVRProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

// ----------- Copied from 
// --------- https://github.com/cms-sw/cmssw/blob/CMSSW_10_2_X/RecoJets/JetProducers/plugins/VirtualJetProducer.cc
vector<reco::CandidatePtr>
GenHOTVRProducer::getConstituents(const vector<fastjet::PseudoJet>&fjConstituents)
{
  vector<reco::CandidatePtr> result; result.reserve(fjConstituents.size()/2);
  for (unsigned int i=0;i<fjConstituents.size();i++) {
    auto index = fjConstituents[i].user_index();
    if ( index >= 0 && static_cast<unsigned int>(index) < particles_.size() ) {
      result.emplace_back(particles_[index]);
    }
  }
  return result;
}


//define this as a plug-in
DEFINE_FWK_MODULE(GenHOTVRProducer);
