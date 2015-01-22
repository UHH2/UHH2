#pragma once

#include "UHH2/core/plugins/NtupleWriterModule.h"
#include "DataFormats/PatCandidates/interface/Jet.h"


class GenericMVAJetTagComputer;

namespace uhh2 {

class NtupleWriterJets: public NtupleWriterModule {
public:

    struct Config: public NtupleWriterModule::Config {
        using NtupleWriterModule::Config::Config;

        std::vector<PFParticle> * pfparts = NULL; // set to non-NULL to enable saving jet constituents
    };


    static void fill_jet_info(const pat::Jet & pat_jet, Jet & jet);

    // store constituents of the pat jet into pfparts and save indices in jet.
    static void store_jet_constituents(const pat::Jet & pat_jet, Jet& jet, std::vector<PFParticle> & pfparts, bool runOnMiniaod);

    explicit NtupleWriterJets(Config & cfg, bool set_jets_member);

    virtual void process(const edm::Event &, uhh2::Event &);

    virtual ~NtupleWriterJets();
private:
    bool runOnMiniAOD;
    std::vector<PFParticle> * pfparts;
    edm::EDGetToken src_token;
    float ptmin, etamax;
    Event::Handle<std::vector<Jet>> handle; // main handle to write output to
    boost::optional<Event::Handle<std::vector<Jet>>> jets_handle; // handle of name "jets" in case set_jets_member is true
};


class NtupleWriterTopJets: public NtupleWriterModule{
public:

    struct Config: public NtupleWriterModule::Config {
        using NtupleWriterModule::Config::Config;

        std::vector<PFParticle> * pfparts = NULL; // set to non-NULL to enable saving jet constituents
        edm::InputTag constituent_src; // a jet collection from where to take the constituents
        std::string njettiness_src;
        bool doTagInfos = false;
    };

    explicit NtupleWriterTopJets(Config & cfg, bool set_jets_member);

    void set_computer(const GenericMVAJetTagComputer *computer_){
        computer = computer_;
    }

    virtual void process(const edm::Event &, uhh2::Event &);

    virtual ~NtupleWriterTopJets();

private:
    bool runOnMiniAOD, doTagInfos;
    std::vector<PFParticle> * pfparts;
    float ptmin, etamax;
    edm::EDGetToken src_token, constituent_src_token;
    std::string njettiness_src;

    const GenericMVAJetTagComputer * computer = 0;

    Event::Handle<std::vector<TopJet>> handle;
    boost::optional<Event::Handle<std::vector<TopJet>>> topjets_handle;
};

}
