#pragma once

#include "UHH2/core/plugins/NtupleWriterModule.h"
#include "DataFormats/PatCandidates/interface/Jet.h"


class GenericMVAJetTagComputer;

namespace uhh2 {

class NtupleWriterJets: public NtupleWriterModule {
public:

    static void fill_jet_info(const pat::Jet & pat_jet, Jet & jet);

    explicit NtupleWriterJets(Config & cfg, bool set_jets_member);

    virtual void process(const edm::Event &, uhh2::Event &);

    virtual ~NtupleWriterJets();
private:
    edm::EDGetToken src_token;
    float ptmin, etamax;
    Event::Handle<std::vector<Jet>> handle; // main handle to write output to
    boost::optional<Event::Handle<std::vector<Jet>>> jets_handle; // handle of name "jets" in case set_jets_member is true
};


class NtupleWriterTopJets: public NtupleWriterModule{
public:

    struct Config: public NtupleWriterModule::Config {
        using NtupleWriterModule::Config::Config;

        edm::InputTag substructure_variables_src; // a jet collection from where to take the subjet variables (after DeltaR-matching)
        std::string njettiness_src;
        std::string qjets_src;
    };

    explicit NtupleWriterTopJets(Config & cfg, bool set_jets_member);

    virtual void process(const edm::Event &, uhh2::Event &);

    virtual ~NtupleWriterTopJets();

private:
    float ptmin, etamax;
    edm::EDGetToken src_token, substructure_variables_src_token;
    std::string njettiness_src, qjets_src;

    Event::Handle<std::vector<TopJet>> handle;
    boost::optional<Event::Handle<std::vector<TopJet>>> topjets_handle;
};

}
