#ifndef NtupleWriter_h
#define NtupleWriter_h

#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/Run.h"

#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/JetReco/interface/GenJet.h"

#include "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h"
#include "DataFormats/PatCandidates/interface/PackedTriggerPrescales.h"
#include "DataFormats/PatCandidates/interface/Jet.h"

#include "DataFormats/L1TGlobal/interface/GlobalAlgBlk.h"
#include "DataFormats/L1Trigger/interface/EGamma.h"
#include "DataFormats/L1Trigger/interface/Jet.h"
#include "DataFormats/L1Trigger/interface/Muon.h"

#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/AnalysisModule.h"
#include "TTree.h"

#include <memory>

namespace uhh2 {
    class CMSSWContext;
    class NtupleWriterModule;
    class NtupleWriterTopJets;
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

      // fill gen particles and other info from a reco::jet
      void fill_geninfo_recojet(const reco::Jet& reco_genjet, GenJet& genjet, bool &add);

      // fill gen particles and other info from a pat-jet (for XCONE, HOTVR, etc)
      void fill_geninfo_patjet(const pat::Jet& pat_genjet, GenJet& genjet, bool &add);

      /* //Add pf candidate to event */
      /* size_t add_pfpart(const reco::Candidate & pf, std::vector<PFParticle> & pfparts); */

      // ----------member data ---------------------------
      TFile *outfile;
      TTree *tr;
      std::string fileName, year;

      bool doGenJets;
      bool doGenTopJets;
      bool doMET;
      bool doGenMET;
      bool doPhotons;
      bool doGenInfo;
      bool doStableGenParticles;
      unsigned doGenJetConstituentsNjets;
      double doGenJetConstituentsMinJetPt;
      unsigned doGenTopJetConstituentsNjets;
      double doGenTopJetConstituentsMinJetPt;
      unsigned doGenxconeJetConstituentsNjets;
      double doGenxconeJetConstituentsMinJetPt;
      unsigned doGenxconeDijetJetConstituentsNjets;
      double doGenxconeDijetJetConstituentsMinJetPt;
      unsigned doGenhotvrJetConstituentsNjets;
      double doGenhotvrJetConstituentsMinJetPt;

      unsigned doPFJetConstituentsNjets;
      double doPFJetConstituentsMinJetPt;
      bool doPFJetConstituents;
      unsigned doPFTopJetConstituentsNjets;
      double doPFTopJetConstituentsMinJetPt;
      bool doPFTopJetConstituents;
      unsigned doPFxconeJetConstituentsNjets;
      double doPFxconeJetConstituentsMinJetPt;
      bool doPFxconeJetConstituents;
      unsigned doPFhotvrJetConstituentsNjets;
      double doPFhotvrJetConstituentsMinJetPt;
      bool doPFhotvrJetConstituents;
      unsigned doPFxconeDijetJetConstituentsNjets;
      double doPFxconeDijetJetConstituentsMinJetPt;
      bool doPFxconeDijetJetConstituents;


      bool doPV;
      bool doTrigger;
      bool doL1seed;
      bool doEcalBadCalib;
      bool doPrefire;
      bool runOnMiniAOD;
      bool doRho;
      bool doTrigHTEmu;
      bool doAllPFParticles;

      bool doEleAddVars;//Add variables to trace possible issues with the ECAL slew rate mitigation

      // added by Alex and Dennis
      bool doHOTVR;
      bool doXCone;
      bool doGenHOTVR;
      bool doGenXCone;
      bool doXCone_dijet;
      bool doGenXCone_dijet;

      // in order of initialization:
      std::unique_ptr<uhh2::GenericEventStructure> ges;
      std::unique_ptr<uhh2::CMSSWContext> context;
      std::vector<std::unique_ptr<uhh2::NtupleWriterModule>> writer_modules;
      std::unique_ptr<uhh2::Event> event;
      std::unique_ptr<uhh2::AnalysisModule> module;

      edm::EDGetToken rho_token;
      edm::EDGetToken bs_token;
      edm::EDGetToken generator_token;
      edm::EDGetToken pus_token;
      edm::EDGetToken lhe_token;
      edm::EDGetToken genjetflavor_token;
      edm::EDGetToken dupECALClusters_token;
      edm::EDGetToken hitsNotReplaced_token;
      std::vector<edm::EDGetToken> genjet_tokens;
      std::vector<std::vector<GenJet>> genjets;
      double genjet_ptmin;
      double genjet_etamax;

      edm::EDGetToken pf_collection_token;

      std::vector<edm::EDGetToken> photon_tokens;
      std::vector<std::vector<Photon>> phs;

      std::vector<edm::EDGetToken> met_tokens;
      std::vector<MET> met;

      std::vector<edm::EDGetToken> genmet_tokens;
      std::vector<MET> genmet;

      std::vector<edm::EDGetToken> pv_tokens;
      std::vector<std::vector<PrimaryVertex>> pvs;

      edm::EDGetToken genparticle_token;
      edm::EDGetToken stablegenparticle_token;

      std::vector<std::string> trigger_prefixes;
      std::vector<std::string> triggerNames_outbranch;

      edm::EDGetTokenT<bool> ecalBadCalibFilterUpdate_token;

      edm::EDGetTokenT<double> prefweight_token;
      edm::EDGetTokenT<double> prefweightup_token;
      edm::EDGetTokenT<double> prefweightdown_token;

      bool newrun, setup_output_branches_done, firstEvent;

      edm::EDGetTokenT<edm::TriggerResults> triggerBits_;
      edm::EDGetTokenT<edm::TriggerResults>  metfilterBits_;
      edm::EDGetTokenT<pat::PackedTriggerPrescales> triggerPrescales_;
      edm::EDGetTokenT<pat::PackedTriggerPrescales> triggerPrescalesL1min_;
      edm::EDGetTokenT<pat::PackedTriggerPrescales> triggerPrescalesL1max_;
      edm::EDGetTokenT<pat::TriggerObjectStandAloneCollection> triggerObjects_;

      std::vector<std::vector<FlavorParticle> > triggerObjects_out;
      std::vector<std::string> triggerObjects_sources;
      std::vector<edm::EDGetTokenT<bool>> extraTriggers_tokens;
      std::vector<std::string> extraTriggers_names;

      std::vector<bool> skipMETUncertainties;

      std::vector<edm::EDGetToken> hotvr_tokens;
      std::vector<std::vector<TopJet>> hotvrJets;

      std::vector<edm::EDGetToken> xcone_tokens;
      std::vector<std::vector<TopJet>> xconeJets;
      std::vector<edm::EDGetToken> xcone_tokens_dijet;
      std::vector<std::vector<TopJet>> xconeJets_dijet;

      std::vector<edm::EDGetToken> genhotvr_tokens;
      std::vector<std::vector<GenTopJet>> genhotvrJets;

      std::vector<edm::EDGetToken> genxcone_tokens;
      std::vector<std::vector<GenTopJet>> genxconeJets;

      std::vector<edm::EDGetToken> genxcone_tokens_dijet;
      std::vector<std::vector<GenTopJet>> genxconeJets_dijet;

      edm::EDGetTokenT<BXVector<GlobalAlgBlk>> l1GtToken_;
      edm::EDGetTokenT<BXVector<l1t::EGamma>> l1EGToken_;
      edm::EDGetTokenT<BXVector<l1t::Jet>> l1JetToken_;
      edm::EDGetTokenT<BXVector<l1t::Muon>> l1MuonToken_;

      std::vector<L1EGamma>  L1EG_seeds;
      std::vector<L1Jet> L1Jet_seeds;
      std::vector<L1Muon> L1Muon_seeds;


};

#endif
