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
#include "FWCore/Framework/interface/stream/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"

#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/Candidate/interface/Particle.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "RecoJets/JetProducers/interface/JetSpecific.h"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/ClusterSequenceArea.hh"
#include "fastjet/contrib/Nsubjettiness.hh"
#include "fastjet/contrib/XConePlugin.hh"
#include "fastjet/contrib/SoftDrop.hh"

#include "vector"

using namespace fastjet;
using namespace contrib;
using namespace std;

//
// class declaration
//

class XConeProducer : public edm::stream::EDProducer<> {
  public:
    explicit XConeProducer(const edm::ParameterSet&);
    ~XConeProducer();

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    // This method copies the constituents from the fjConstituents method to an output of CandidatePtr's.
    virtual std::vector<reco::CandidatePtr> getConstituents(const std::vector<fastjet::PseudoJet>&fjConstituents);
    static pat::Jet rekeyJet(const pat::Jet & jet, edm::Handle<edm::View<reco::Candidate>> & candHandle);

  private:
  virtual void produce(edm::Event&, const edm::EventSetup&) override;

  virtual pat::Jet createPatJet(const fastjet::PseudoJet &) const;
  virtual pat::Jet createPatJet(const fastjet::PseudoJet &, const edm::EventSetup&);

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
  const bool printWarning_;

  reco::Particle::Point  vertex_;
  std::vector<edm::Ptr<reco::Candidate> > particles_;
  edm::EDGetTokenT<reco::VertexCollection> input_vertex_token_;
  bool doRekey_;
  edm::EDGetTokenT<edm::View<reco::Candidate>> rekeyCandSrcToken_;
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
  BetaSubJets_(iConfig.getParameter<double>("BetaSubJets")),
  printWarning_(iConfig.exists("printWarning") ? iConfig.getParameter<bool>("printWarning") : false),
  doRekey_(iConfig.exists("doRekey") ? iConfig.getParameter<bool>("doRekey") : false)
{
  // We make both the fat jets and subjets, and we must store them as separate collections
  produces<pat::JetCollection>();
  produces<pat::JetCollection>(subjetCollName_);
  input_vertex_token_ = consumes<reco::VertexCollection>(edm::InputTag("offlineSlimmedPrimaryVertices"));
  if (doRekey_) {
    // Add option to rekey fatjets & their subjets, i.e. make their duaghters point to the equivalent
    // particle in another collection (the one specified by rekeyCandidateSrc)
    // This is useful if you want to e.g. replace the PUPPI daughters with their packedPFCandidate equivalents
    // for more compact constituent storage. This means the user then has to apply PUPPI weight
    // when analysing constituents.
    // Do it in this producer, because doing it afterwards is a massive pain - subjets of a pat::Jet
    // are not replaceable
    rekeyCandSrcToken_ = consumes<edm::View<reco::Candidate>>(iConfig.getParameter<edm::InputTag>("rekeyCandidateSrc"));
  }
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
void XConeProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  //  cout<<"--- START event --- "<<endl;
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
  else vertex_ = reco::Particle::Point(0,0,0);

  edm::Handle<edm::View<reco::Candidate>> rekeyCandHandle;
  if (doRekey_) {
    iEvent.getByToken(rekeyCandSrcToken_, rekeyCandHandle);
  }

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
    //    cout<<"Input store index #"<<i<<" with energy = "<<particles->ptrAt(i_gl)->energy()<<" size of particles_ = "<<particles_.size()<<endl;
    i++;
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

  // Run clustering of fatjets
  vector<PseudoJet> fatjets;
  std::unique_ptr<NjettinessPlugin> plugin_xcone;
  initPlugin(plugin_xcone, NJets_, RJets_, BetaJets_, usePseudoXCone_);
  double ghost_maxrap = 4.0;      // maxiumum y of ghost particles
  //  AreaDefinition area_def(active_area, GhostedAreaSpec(ghost_maxrap));
  AreaDefinition area_def(active_area_explicit_ghosts, GhostedAreaSpec(ghost_maxrap, 1, 0.02));

  JetDefinition jet_def_xcone(plugin_xcone.get());
  ClusterSequenceArea clust_seq_xcone(_psj, jet_def_xcone, area_def);
  //  ClusterSequence clust_seq_xcone(_psj, jet_def_xcone);
  fatjets = sorted_by_pt(clust_seq_xcone.inclusive_jets(0));

  // get SoftDrop Mass for every fat jet
  SoftDrop sd(0.0, 0.1, 1.2);
  vector<float> sd_mass;
  for(unsigned int i=0; i<fatjets.size(); i++){
    PseudoJet sdjet = sd(fatjets[i]);
    sd_mass.push_back(sdjet.m());
  }


  // Check we actually got the number of jets we requested
  if (fatjets.size() != NJets_) {
    if (printWarning_) {
      edm::LogWarning("XConeTooFewJets") << "Only found " << fatjets.size() << " jets but requested " << NJets_ << ".\n"
                                         << "Have added in blank jets to make " << NJets_ << " jets." << endl;
    }
    for (uint iJet=fatjets.size(); iJet < NJets_; iJet++) {
      fatjets.push_back(PseudoJet(0, 0, 0, 0));
      sd_mass.push_back(0.);
    }
  }

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

  // loop over all fatjets and cluster subjets
  for(unsigned int i=0; i<fatjets.size(); i++){
    // get set of particles in fatjet i
    vector<PseudoJet> particle_in_fatjet;
    for (unsigned int ipart=0; ipart < _psj.size(); ++ipart) {
      if (list_fat[ipart] < 0) continue;
      else if (abs(list_fat[ipart])==i) {
        PseudoJet tmp_particle = _psj.at(ipart);
        tmp_particle.set_user_index(_psj.at(ipart).user_index());//important: store index for later linking between clustered jet and constituence
        particle_in_fatjet.push_back(tmp_particle);
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
      for (unsigned int j = 0; j < subjets.size(); ++j) {
        if(subjets[j].has_area())
          subjet_area.push_back(subjets[j].area());
        else subjet_area.push_back(0);

        indices[i].push_back(subjetCollection->size()); // store index of subjet in the whole subjet collection
        pat::Jet subjet = createPatJet(subjets[j], iSetup);
        if (doRekey_) {
          subjet = rekeyJet(subjet, rekeyCandHandle);
        }
        subjet.setJetArea(subjet_area[j]);
        subjetCollection->push_back(subjet);
      }
    }

    // Check we got the number of subjets we asked for
    if (doSubjets && subjets.size() != NSubJets_ && printWarning_) {
      edm::LogWarning("XConeTooFewSubjets") << "Only found " << subjets.size() << " subjets but requested " << NSubJets_ << ". "
                                            << " Fatjet had " << particle_in_fatjet.size() << " constituents.\n"<<endl;
    }

    // jet area for fat jet
    double jet_area = 0;
    if(fatjets[i].has_area())
      jet_area = fatjets[i].area();

    // pat-ify fatjets
    pat::Jet patJet = createPatJet(fatjets[i], iSetup);
    if (doRekey_) {
      patJet = rekeyJet(patJet, rekeyCandHandle);
    }
    patJet.setJetArea(jet_area);
    patJet.addUserFloat("softdropmass", sd_mass[i]);
    jetCollection->push_back(patJet);

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

pat::Jet XConeProducer::createPatJet(const PseudoJet & psj) const
{
  pat::Jet newJet;
  newJet.setP4(math::XYZTLorentzVector(psj.px(), psj.py(), psj.pz(), psj.E()));
  return newJet;
}


pat::Jet XConeProducer::createPatJet(const PseudoJet & fjJet, const edm::EventSetup& iSetup)
{
  pat::Jet patjet;
  if(fjJet.px()==0 && fjJet.py()==0 && fjJet.pz()==0){//jet or sub-jet was created artificially
    patjet.setP4(math::XYZTLorentzVector(fjJet.px(), fjJet.py(), fjJet.pz(), fjJet.E()));
  }
  else{//jet or sub-jet is real
    //inspired by https://github.com/cms-sw/cmssw/blob/master/RecoJets/JetProducers/plugins/VirtualJetProducer.cc#L687
    // get the constituents from fastjet
    std::vector<fastjet::PseudoJet> const & fjConstituents = fastjet::sorted_by_pt(fjJet.constituents());
    //    cout<<"Size of fjConstituents = "<<fjConstituents.size()<<endl;
    // convert them to CandidatePtr vector
    std::vector<reco::CandidatePtr> const & constituents = getConstituents(fjConstituents);
    //    cout<<"Size of constituents = "<<constituents.size()<<endl;
    // write the specifics to the jet (simultaneously sets 4-vector, vertex).
    // These are overridden functions that will call the appropriate
    // specific allocator.
    reco::Particle::LorentzVector jet4v = reco::Particle::LorentzVector(fjJet.px(), fjJet.py(), fjJet.pz(),fjJet.E());
    reco::PFJet pfjet;
    reco::writeSpecific(pfjet,jet4v,vertex_,constituents, iSetup);// https://github.com/ahlinist/cmssw/blob/master/RecoJets/JetProducers/src/JetSpecific.cc#L91
    patjet = pat::Jet(pfjet);

  }
  return patjet;
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



// ----------- Copied from
// --------- https://github.com/cms-sw/cmssw/blob/CMSSW_10_2_X/RecoJets/JetProducers/plugins/VirtualJetProducer.cc
vector<reco::CandidatePtr> XConeProducer::getConstituents(const vector<fastjet::PseudoJet>&fjConstituents)
{
  vector<reco::CandidatePtr> result; result.reserve(fjConstituents.size()/2);
  //double out_PT = 0;
  for (unsigned int i=0;i<fjConstituents.size();i++) {
    auto index = fjConstituents[i].user_index();
    if ( index >= 0 && static_cast<unsigned int>(index) < particles_.size() ) {
      result.emplace_back(particles_[index]);
      // out_PT +=particles_[index]->energy();
      // cout<<"Output index #"<<index<<" with energy = "<<particles_[index]->energy()<<endl;
    }
  }
  //  cout<<"OUT particles energy = "<<out_PT<<endl;
  return result;
}


// Rekey a jet with equivalent constituents from another collection
pat::Jet XConeProducer::rekeyJet(const pat::Jet & jet, edm::Handle<edm::View<reco::Candidate>> & candHandle) {
  pat::Jet newJet(jet);
  newJet.clearDaughters();
  for (const auto & dauItr : jet.daughterPtrVector()) {
    const unsigned long key = dauItr.key();
    edm::Ptr<reco::Candidate> newDau = candHandle->ptrAt(key);
    newJet.addDaughter(newDau);
  }
  return newJet;
}

//define this as a plug-in
DEFINE_FWK_MODULE(XConeProducer);
