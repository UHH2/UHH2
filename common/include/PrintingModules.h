#pragma once


/* This file define 'printing' modules, which print event content on cout.
 * They are probably only useful for debugging, e.g. by dumping muon collections
 * before and after running a cleaner, etc.
 */

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"

class MuonPrinter: public uhh2::AnalysisModule {
public:
    virtual bool process(uhh2::Event & event) override;
};

class GenParticlesPrinter: public uhh2::AnalysisModule {
public:
    GenParticlesPrinter(uhh2::Context &){}
    virtual bool process(uhh2::Event & event) override;
};

class JetPrinter: public uhh2::AnalysisModule {
public:
    explicit JetPrinter(const std::string & name_, double ptmin_): name(name_), ptmin(ptmin_){}
    virtual bool process(uhh2::Event & event) override;
private:
    std::string name;
    double ptmin;
};
