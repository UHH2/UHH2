#ifndef NtupleWriter_h
#define NtupleWriter_h

#include <memory>

#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/Common/interface/Handle.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDFilter.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "TTree.h"
#include "TMath.h"
#include "TLorentzVector.h"

#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Tau.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/PackedGenParticle.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/TrackReco/interface/HitPattern.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/PdfInfo.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "RecoEgamma/EgammaTools/interface/ConversionTools.h"

#include "EgammaAnalysis/ElectronTools/interface/ElectronEffectiveArea.h"

#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/AnalysisModule.h"

namespace uhh2 {
    class CMSSWContext;
}

class NtupleWriter : public edm::EDFilter {
   public:
      explicit NtupleWriter(const edm::ParameterSet&);
      ~NtupleWriter();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:
      //virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual bool filter(edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;

      virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;

      /// fills constituents of the pat_jet into the Ntuple and stores a reference to those in the provided topjet
      void StoreJetConstituents(const pat::Jet & pat_jet, Jet & topjet);

      /// fill PF candidates from pf_cands to "pfparticles" collection in a cone of radius R0 around inpart (lepton, most likely)
      void StorePFCandsInCone(Particle* part, const std::vector<reco::PFCandidate>& pf_cands, double R0, bool fromiso);

      /// fill PF candidates from packed pf_cands to "pfparticles" collection in a cone of radius R0 around inpart (lepton, most likely)
      void StorePFCandsInCone_packed(Particle* part, const std::vector<pat::PackedCandidate>& pf_cands, double R0);
      
      // fill gen particles from a gen topjet
      void fill_genparticles_jet(const reco::GenJet& reco_genjet, GenJetWithParts& genjet);

      // ----------member data ---------------------------
      TFile *outfile;
      TTree *tr;
      std::string fileName;

      bool doElectrons;
      bool doMuons;
      bool doTaus;
      bool doJets;
      bool doJetsConstituents;
      bool doGenJets;
      bool doGenJetsWithParts;
      bool doTopJets;
      bool doTopJetsConstituents;
      bool doGenTopJets;
      bool doMET;
      bool doPhotons;
      bool doGenInfo;
      bool doAllGenParticles;
      bool doPV;
      bool doTrigger;
      bool doTagInfos;
      bool storePFsAroundLeptons;
      bool doAllPFConstituents;
      bool runOnMiniAOD;
      bool doRho;

      std::unique_ptr<uhh2::GenericEventStructure> ges;
      std::unique_ptr<uhh2::CMSSWContext> context;
      std::unique_ptr<uhh2::Event> event;
      std::unique_ptr<uhh2::AnalysisModule> module;

      edm::EDGetToken rho_token;
      
      std::vector<PFParticle> pfparticles;

      std::vector<edm::EDGetToken> electron_tokens;
      std::vector<std::vector<Electron>> eles;

      std::vector<edm::EDGetToken> muon_tokens;
      std::vector<std::vector<Muon>> mus;

      std::vector<edm::EDGetToken> tau_tokens;
      std::vector<std::vector<Tau>> taus;
      double tau_ptmin;
      double tau_etamax;

      std::vector<edm::EDGetToken> jet_tokens;
      std::vector<std::vector<Jet>> jets;
      double jet_ptmin;
      double jet_etamax;

      std::vector<edm::EDGetToken> genjet_tokens;
      std::vector<std::vector<Particle>> genjets;
      double genjet_ptmin;
      double genjet_etamax;

      std::vector<edm::EDGetToken> topjet_tokens;
      std::vector<std::vector<TopJet> > topjets;
      double topjet_ptmin;
      double topjet_etamax;

      std::vector<std::string> topjet_constituents_sources;
      //std::vector<edm::EDGetToken> topjet_consituents_tokens;
      std::vector<std::string> pf_constituents_sources;
      //std::vector<edm::EDGetToken> pf_consituents_tokens;

      std::vector<edm::EDGetToken> gentopjet_tokens;
      std::vector<std::vector<GenTopJet>> gentopjets;
      double gentopjet_ptmin;
      double gentopjet_etamax;

      std::vector<edm::EDGetToken> genjetwithparts_tokens;
      std::vector<std::vector<GenJetWithParts>> genjetwithparts;
      double genjetwithparts_ptmin;
      double genjetwithparts_etamax;

      std::vector<edm::EDGetToken> photon_tokens;
      std::vector<std::vector<Photon>> phs;

      std::vector<edm::EDGetToken> met_tokens;
      std::vector<MET> met;

      std::vector<edm::EDGetToken> pv_tokens;
      std::vector<std::vector<PrimaryVertex>> pvs;

      std::vector<std::string> pf_around_leptons_sources;

      edm::InputTag genparticle_source;
      //edm::EDGetToken genparticle_token;
      edm::InputTag stablegenparticle_source;   
      //edm::EDGetToken stablegenparticle_token;
      
      edm::InputTag SVComputer_;
      std::vector<std::string> trigger_prefixes;
      std::vector<std::string> triggerNames_outbranch;

      bool newrun, setup_output_branches_done;

      edm::EDGetTokenT<edm::TriggerResults> triggerBits_;
};


#endif
