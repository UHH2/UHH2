// -*- C++ -*-
//
// Package:    UHH2/HOTVRProducer
// Class:      HOTVRProducer
//
/**\class HOTVRProducer HOTVRProducer.cc UHH2/HOTVRProducer/plugins/HOTVRProducer.cc

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
#include "FWCore/Framework/interface/global/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"

#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/Math/interface/LorentzVector.h"

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

class HOTVRProducer : public edm::global::EDProducer<> {
  public:
    explicit HOTVRProducer(const edm::ParameterSet&);
    ~HOTVRProducer();

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

  private:
    virtual void produce(edm::StreamID, edm::Event&, const edm::EventSetup&) const override;

    virtual pat::Jet createPatJet(const PseudoJet &) const;

    // ----------member data ---------------------------
    edm::EDGetToken src_token_;
    const std::string subjetCollName_;
};

//
// constructors and destructor
//
HOTVRProducer::HOTVRProducer(const edm::ParameterSet& iConfig):
  src_token_(consumes<edm::View<reco::Candidate>>(iConfig.getParameter<edm::InputTag>("src"))),
  subjetCollName_("SubJets")
{
  // We make both the fat jets and subjets, and we must store them as separate collections
  produces<pat::JetCollection>();
  produces<pat::JetCollection>(subjetCollName_);
}


HOTVRProducer::~HOTVRProducer()
{

  // do anything here that needs to be done at destruction time
  // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called to produce the data  ------------
void
HOTVRProducer::produce(edm::StreamID id, edm::Event& iEvent, const edm::EventSetup& iSetup) const
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

  // Do the clustering, make jets, nsub, store
  double mu(30.),                      // massjump threshold
         theta(0.7),                   // massjump parameter
         max_r(1.5),                   // maximum allowed distance R
         min_r(0.1),                   // minimum allowed distance R
         rho(600.),                    // cone shrinking parameter
         hotvr_pt_min(30.);            // minimum pT of subjet

  // jet definition with HOTVR plugin
  HOTVR hotvr_plugin(mu, theta, min_r, max_r, rho, hotvr_pt_min, HOTVR::CALIKE);
  JetDefinition jet_def(&hotvr_plugin);

   // Setup cluster sequence
  ClusterSequence cs(_psj, jet_def);
  vector<PseudoJet> hotvr_jets = hotvr_plugin.get_jets();

  // NOTE: There is a problem when getting Nsubjettiness directly
  // from HOTVR jets, because the link to the cluster sequence got lost
  // somehow. Thus the approach here will be to identify the Jets from
  // the cluster sequence with the hotvr_jets by comparing their
  // four-vectors.

  // area definition
  double ghost_maxrap = 4.0;      // maxiumum y of ghost particles
  AreaDefinition area_def(active_area_explicit_ghosts, GhostedAreaSpec(ghost_maxrap, 1, 0.02));

  // setup cluster sequence with area
  HOTVR hotvr_plugin_area(mu, theta, min_r, max_r, rho, hotvr_pt_min, HOTVR::CALIKE);
  JetDefinition jet_def_area(&hotvr_plugin_area);
  ClusterSequenceArea cs_area(_psj, jet_def_area, area_def);
  vector<PseudoJet> hotvr_jets_area = hotvr_plugin_area.get_jets();

  //in a few cases, there are jets in the original clustering without a corresponding jet in the area clustering
  //->add a dummy jet into the area collection and throw a warning because we cannot determine the area for these jets
  if (hotvr_jets_area.size() != hotvr_jets.size()) {

    for (unsigned int i = 0; i < hotvr_jets.size(); ++i) {
        bool matched = false;
        for (unsigned int j = 0; j < hotvr_jets_area.size(); ++j) {
            if( fabs(hotvr_jets[i].pt() - hotvr_jets_area[j].pt())<0.0001 &&
                fabs(hotvr_jets[i].eta() - hotvr_jets_area[j].eta())<0.0001) {
              matched = true;
              break;
            }
        }

        if(!matched) {
          PseudoJet dummy_jet(0, 0, 0, 0);
          hotvr_jets_area.insert(hotvr_jets_area.begin()+i, dummy_jet);
        }
      }

    //sometimes, there are more jets in the area clustering
    //->filter out jets from the area collection which are not matched to any jet in the original clustering
    for (unsigned int i = 0; i < hotvr_jets_area.size(); ++i) {
        //skip dummy jets from previous iteration
        if (hotvr_jets_area[i].pt() == 0) continue;

        bool matched=false;
        for (unsigned int j = 0; j < hotvr_jets.size(); ++j) {
            if( fabs(hotvr_jets[j].pt() - hotvr_jets_area[i].pt())<0.0001 &&
                fabs(hotvr_jets[j].eta() - hotvr_jets_area[i].eta())<0.0001) {
              matched = true;
              break;
            }
        }
        if(!matched) {
          hotvr_jets_area.erase(hotvr_jets_area.begin()+i);
          i--;
        }
    }
  }

  if (hotvr_jets_area.size() != hotvr_jets.size()) {
    throw cms::Exception("MismatchedSizes", "Number of HOTVR jets found with ClusterSequence does not match number of jets with ClusterSequenceArea.");
  }

  auto jetCollection = std::make_unique<pat::JetCollection>();
  auto subjetCollection = std::make_unique<pat::JetCollection>();

  // Follow the similar pattern as in VirtualJetProducer to output both
  // jets & subjets, with correct linkage

  // this is the mapping of subjet to hard jet
  std::vector< std::vector<int> > indices;
  indices.resize(hotvr_jets.size());

  for (unsigned int i = 0; i < hotvr_jets.size(); ++i) {
    double beta = 1.0;
    HOTVRinfo hi = hotvr_jets[i].user_info<HOTVRinfo>();
    Nsubjettiness nSub1(1, OnePass_WTA_KT_Axes(), NormalizedMeasure(beta, hi.max_distance()));
    Nsubjettiness nSub2(2, OnePass_WTA_KT_Axes(), NormalizedMeasure(beta, hi.max_distance()));
    Nsubjettiness nSub3(3, OnePass_WTA_KT_Axes(), NormalizedMeasure(beta, hi.max_distance()));
    double tau1 = nSub1(hotvr_jets[i]);
    double tau2 = nSub2(hotvr_jets[i]);
    double tau3 = nSub3(hotvr_jets[i]);

    std::vector<PseudoJet> subjets;

    double jet_area = 0;
    std::vector<double> subjet_area;

    if (hotvr_jets_area[i].pt()>0) {
      jet_area = hotvr_jets_area[i].area();
      HOTVRinfo hi_area = hotvr_jets_area[i].user_info<HOTVRinfo>();
      subjets = hi_area.subjets();
      for (unsigned int j = 0; j < subjets.size(); ++j) {
        subjet_area.push_back(subjets[j].area());
      }
    } else {
      subjets = hi.subjets();
      for (unsigned int j = 0; j < subjets.size(); ++j) {
        subjet_area.push_back(0);
      }
      edm::LogWarning("NoJetArea") << "HOTVRProducer: could not find area jet for a HOTVR jet; set area and subjet areas to 0.";
    }

    // Convert jet and subjets to pat::Jets
    auto thisPatJet = createPatJet(hotvr_jets[i]);
    thisPatJet.setJetArea(jet_area);
    thisPatJet.addUserFloat("tau1", tau1);
    thisPatJet.addUserFloat("tau2", tau2);
    thisPatJet.addUserFloat("tau3", tau3);
    jetCollection->push_back(thisPatJet);

    for (uint s=0; s<subjets.size(); s++) {
      indices[i].push_back(subjetCollection->size());

      auto subjet = createPatJet(subjets[s]);
      subjet.setJetArea(subjet_area[s]);
      subjetCollection->push_back(subjet);
    }

  } // end loop over jets

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


pat::Jet HOTVRProducer::createPatJet(const PseudoJet & psj) const
{
  pat::Jet newJet;
  newJet.setP4(math::XYZTLorentzVector(psj.px(), psj.py(), psj.pz(), psj.E()));
  return newJet;
}


// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
HOTVRProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(HOTVRProducer);
