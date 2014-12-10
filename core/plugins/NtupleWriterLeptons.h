#pragma once

#include "UHH2/core/plugins/NtupleWriterModule.h"

namespace uhh2 {

class NtupleWriterElectrons: public NtupleWriterModule {
public:

    explicit NtupleWriterElectrons(Config & cfg, bool set_electrons_member);

    virtual void process(const edm::Event &, uhh2::Event &);

    virtual ~NtupleWriterElectrons();
private:
    edm::EDGetToken src_token;
    Event::Handle<std::vector<Electron>> handle; // main handle to write output to
    boost::optional<Event::Handle<std::vector<Electron>>> electrons_handle; // handle of name "electrons" in case set_electrons_member is true
};

class NtupleWriterMuons: public NtupleWriterModule {
public:

    explicit NtupleWriterMuons(Config & cfg, bool set_muons_member);

    virtual void process(const edm::Event &, uhh2::Event &);

    virtual ~NtupleWriterMuons();
private:
    edm::EDGetToken src_token;
    Event::Handle<std::vector<Muon>> handle;
    boost::optional<Event::Handle<std::vector<Muon>>> muons_handle;
};

class NtupleWriterTaus: public NtupleWriterModule {
public:

    explicit NtupleWriterTaus(Config & cfg, bool set_taus_member);

    virtual void process(const edm::Event &, uhh2::Event &);

    virtual ~NtupleWriterTaus();
private:
    double ptmin, etamax;
    edm::EDGetToken src_token;
    Event::Handle<std::vector<Tau>> handle;
    boost::optional<Event::Handle<std::vector<Tau>>> taus_handle;
};

}


