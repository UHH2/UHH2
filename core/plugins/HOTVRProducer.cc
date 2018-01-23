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
#include "FWCore/Framework/interface/stream/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"

#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
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

class HOTVRProducer : public edm::stream::EDProducer<> {
  public:
    explicit HOTVRProducer(const edm::ParameterSet&);
    ~HOTVRProducer();

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

  private:
    virtual void beginStream(edm::StreamID) override;
    virtual void produce(edm::Event&, const edm::EventSetup&) override;
    virtual void endStream() override;

    virtual pat::Jet createPatJet(const PseudoJet &);
    virtual pat::Jet createPatJet(const PseudoJet &, const std::vector<PseudoJet> &, double, double, double, double, std::vector<double>);

    // ----------member data ---------------------------
    edm::EDGetToken src_token_;
};

//
// constructors and destructor
//
HOTVRProducer::HOTVRProducer(const edm::ParameterSet& iConfig):
  src_token_(consumes<edm::View<pat::PackedCandidate>>(iConfig.getParameter<edm::InputTag>("src")))
{
  produces<pat::JetCollection>();
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
HOTVRProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{

  edm::Handle<edm::View<pat::PackedCandidate>> particles;
  iEvent.getByToken(src_token_, particles);

  // Convert particles to PseudoJets
  std::vector<PseudoJet> _psj;
  for (const auto & cand: *particles) {
    if (std::isnan(cand.p4().Px()) || 
        std::isnan(cand.p4().Py()) || 
        std::isnan(cand.p4().Pz()) || 
        std::isinf(cand.p4().Px()) || 
        std::isinf(cand.p4().Py()) ||
        std::isinf(cand.p4().Pz()))
      continue;

    _psj.push_back(PseudoJet(cand.p4().X(), cand.p4().Y(), cand.p4().Z(), cand.p4().T()));
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

  auto outputJets = std::make_unique<pat::JetCollection>();

  for (unsigned int i = 0; i < hotvr_jets.size(); ++i) {
    double beta = 1.0;
    HOTVRinfo hi = hotvr_jets[i].user_info<HOTVRinfo>();
    Nsubjettiness nSub1(1, OnePass_WTA_KT_Axes(), NormalizedMeasure(beta, hi.max_distance()));
    Nsubjettiness nSub2(2, OnePass_WTA_KT_Axes(), NormalizedMeasure(beta, hi.max_distance()));
    Nsubjettiness nSub3(3, OnePass_WTA_KT_Axes(), NormalizedMeasure(beta, hi.max_distance()));
    double tau1 = nSub1(hotvr_jets[i]);
    double tau2 = nSub2(hotvr_jets[i]);
    double tau3 = nSub3(hotvr_jets[i]);

    // getting jet and subjet area
    double jet_area = 0;
    std::vector<double> subjet_area;
    std::vector<PseudoJet> subjets;
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
    outputJets->push_back(createPatJet(hotvr_jets[i], subjets, tau1, tau2, tau3, jet_area, subjet_area));
  }

  iEvent.put(std::move(outputJets));
}

pat::Jet HOTVRProducer::createPatJet(const PseudoJet & psj)
{
  pat::Jet newJet;
  newJet.setP4(math::XYZTLorentzVector(psj.pt(), psj.eta(), psj.phi(), psj.E()));
  return newJet;
}

pat::Jet HOTVRProducer::createPatJet(const PseudoJet & psj, const std::vector<PseudoJet> &subpsj, double tau1, double tau2, double tau3, double jet_area, std::vector<double> subjet_area)
{
  pat::Jet newJet = createPatJet(psj);
  newJet.setJetArea(jet_area);
  newJet.addUserFloat("tau1", tau1);
  newJet.addUserFloat("tau2", tau2);
  newJet.addUserFloat("tau3", tau3);

  for (uint i=0; i<subpsj.size(); i++) {
    pat::Jet subjet = createPatJet(subpsj[i]);
    subjet.setJetArea(subjet_area[i]);
  }

  return newJet;
}


// ------------ method called once each stream before processing any runs, lumis or events  ------------
void
HOTVRProducer::beginStream(edm::StreamID)
{
}

// ------------ method called once each stream after processing all runs, lumis and events  ------------
void
HOTVRProducer::endStream() {
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
