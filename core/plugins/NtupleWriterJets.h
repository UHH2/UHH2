#pragma once

#include "UHH2/core/plugins/NtupleWriterModule.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "CommonTools/MVAUtils/interface/TMVAEvaluator.h"
#include "RecoBTau/JetTagComputer/interface/JetTagComputer.h"
#include "DataFormats/JetReco/interface/JetCollection.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/Candidate/interface/VertexCompositePtrCandidate.h"
#include "RecoBTag/SecondaryVertex/interface/TrackKinematics.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "TrackingTools/IPTools/interface/IPTools.h"
#include "fastjet/PseudoJet.hh"
#include "RecoBTag/SecondaryVertex/interface/TrackKinematics.h"
#include "DataFormats/L1TGlobal/interface/GlobalAlgBlk.h"
#include "DataFormats/L1Trigger/interface/Jet.h"


class GenericMVAJetTagComputer;

namespace uhh2 {

size_t add_genpart(const reco::Candidate & jetgenp, std::vector<GenParticle> & genparts);
size_t add_pfpart(const reco::Candidate & pf, std::vector<PFParticle> & pfparts);

class NtupleWriterJets: public NtupleWriterModule {
public:

    struct Config: public NtupleWriterModule::Config {
      edm::InputTag l1jet_src;

      // inherit constructor does not work yet :-(
      Config(uhh2::Context & ctx_, edm::ConsumesCollector && cc_, const edm::InputTag & src_,
             const std::string & dest_, const std::string & dest_branchname_ = ""):
        NtupleWriterModule::Config(ctx_, std::move(cc_), src_, dest_, dest_branchname_) {}
    };

    static void fill_jet_info(uhh2::Event & uevent, const pat::Jet & pat_jet, Jet & jet, bool do_btagging, bool doPuppiSpecific, bool fill_pfcand=false);

    explicit NtupleWriterJets(Config & cfg, bool set_jets_member, unsigned int NPFJetwConstituents, double MinPtJetwConstituents);
    explicit NtupleWriterJets(Config & cfg, bool set_jets_member, const std::vector<std::string>&, const std::vector<std::string>&, unsigned int NPFJetwConstituents, double MinPtJetwConstituents);

    virtual void process(const edm::Event &, uhh2::Event &,  const edm::EventSetup&);

    virtual ~NtupleWriterJets();
private:
    edm::InputTag src;
    edm::EDGetToken src_token;
    edm::EDGetToken src_higgs_token;
    edm::EDGetToken src_softdrop_token;
    edm::EDGetTokenT<BXVector<l1t::Jet>> l1jet_token;
    float ptmin, etamax;
    Event::Handle<std::vector<Jet>> handle; // main handle to write output to
    boost::optional<Event::Handle<std::vector<Jet>>> jets_handle; // handle of name "jets" in case set_jets_member is true
    //    boost::optional<Event::Handle<std::vector<PFParticle>>> pfcand_handle;//handle to PF constituences of jets
    bool doPuppiSpecific; // get puppi specifics from userFloats
    bool save_lepton_keys_;
    std::vector<Event::Handle<std::vector<Muon>    >> h_muons;
    std::vector<Event::Handle<std::vector<Electron>>> h_elecs;
    std::vector<Event::Handle<std::vector<PFParticle>>> h_pfcands;

    unsigned int NPFJetwConstituents_;
    double MinPtJetwConstituents_;
};


class NtupleWriterTopJets: public NtupleWriterModule{
public:

    struct Config: public NtupleWriterModule::Config {
        using NtupleWriterModule::Config::Config;

        bool do_btagging = true, do_btagging_subjets = true;

        edm::InputTag substructure_variables_src;// a jet collection from where to take the subjet variables (after DeltaR-matching)
        edm::InputTag substructure_groomed_variables_src;
        edm::InputTag SVComputer;
        std::string njettiness_src;
        std::string njettiness_groomed_src;
        std::string qjets_src;
        std::string subjet_src;
        std::string higgs_src;
        std::string higgs_name;
        std::string higgstaginfo_src;
        std::string softdrop_src;
        std::string toptagging_src;
        std::string ecf_beta1_src;
        std::string ecf_beta2_src;
    };

    explicit NtupleWriterTopJets(Config & cfg, bool set_jets_member, unsigned int NPFJetwConstituents, double MinPtJetwConstituents);
    explicit NtupleWriterTopJets(Config & cfg, bool set_jets_member, const std::vector<std::string>&, const std::vector<std::string>&, unsigned int NPFJetwConstituents, double MinPtJetwConstituents);

    virtual void process(const edm::Event &, uhh2::Event &,  const edm::EventSetup&);
    static void fill_btag_info(uhh2::Event & uevent, const pat::Jet & pat_jet, TopJet & jet);
    virtual ~NtupleWriterTopJets();

private:
    edm::InputTag src;
    float ptmin, etamax;
    bool do_btagging, do_btagging_subjets, do_toptagging;
    edm::EDGetToken src_token, src_higgs_token, src_softdrop_token, substructure_variables_src_token, substructure_variables_src_tokenreco, substructure_groomed_variables_src_token, substructure_groomed_variables_src_tokenreco;
    edm::EDGetToken src_njettiness1_token, src_njettiness2_token, src_njettiness3_token, src_njettiness4_token, src_qjets_token;
    edm::EDGetToken src_njettiness1_groomed_token, src_njettiness2_groomed_token, src_njettiness3_groomed_token, src_njettiness4_groomed_token;
    edm::EDGetToken src_ecf_beta1_N2_token, src_ecf_beta1_N3_token, src_ecf_beta2_N2_token, src_ecf_beta2_N3_token;
    edm::EDGetToken src_hepTopTag_token;
    edm::EDGetToken src_higgstaginfo_token;
    std::string njettiness_src, njettiness_groomed_src, qjets_src, ecf_beta1_src, ecf_beta2_src, subjet_src, higgs_src, higgs_name, higgstaginfo_src, softdrop_src, topjet_collection;
    bool doPuppiSpecific;
    Event::Handle<std::vector<TopJet>> handle;
    boost::optional<Event::Handle<std::vector<TopJet>>> topjets_handle;
    //    boost::optional<Event::Handle<std::vector<PFParticle>>> pfcand_handle;
    std::vector<TopJet::tag> id_tags;

    bool save_lepton_keys_;
    unsigned int NPFJetwConstituents_;
    double MinPtJetwConstituents_;
    std::vector<Event::Handle<std::vector<Muon>    >> h_muons;
    std::vector<Event::Handle<std::vector<Electron>>> h_elecs;
    std::vector<Event::Handle<std::vector<PFParticle>>> h_pfcands;
};

class NtupleWriterGenTopJets: public NtupleWriterModule{
public:

    struct Config: public NtupleWriterModule::Config {
        using NtupleWriterModule::Config::Config;

        edm::InputTag substructure_variables_src;// a jet collection from where to take the subjet variables (after DeltaR-matching), may or may not be the same as the main jet collection
        std::string subjet_src;
        std::string njettiness_src;
        std::string ecf_beta1_src;
        std::string ecf_beta2_src;
    };

    explicit NtupleWriterGenTopJets(Config & cfg, bool set_jets_member, unsigned int NGenJetwConstituents, double MinPtJetwConstituents);
    static void fill_genjet_info(uhh2::Event & event, const reco::Candidate & reco_genjet, GenJet & jet, bool add_genparts=false);
    virtual void process(const edm::Event &, uhh2::Event &,  const edm::EventSetup&);
    virtual ~NtupleWriterGenTopJets();

private:
    edm::InputTag src;
    float ptmin, etamax;
    edm::EDGetToken src_token, subjet_src_token;
    edm::EDGetToken substructure_variables_src_token_basic, substructure_variables_src_token_gen;
    edm::EDGetToken src_njettiness1_token, src_njettiness2_token, src_njettiness3_token, src_njettiness4_token;
    edm::EDGetToken src_ecf_beta1_N2_token, src_ecf_beta1_N3_token, src_ecf_beta2_N2_token, src_ecf_beta2_N3_token;
    std::string subjet_src, njettiness_src, ecf_beta1_src, ecf_beta2_src, gentopjet_collection;
    bool useSubstructureVar;

    Event::Handle<std::vector<GenTopJet>> handle;
    boost::optional<Event::Handle<std::vector<GenTopJet>>> gentopjets_handle;

    unsigned int NGenJetwConstituents_;
    double MinPtJetwConstituents_;
};

}
