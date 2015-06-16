#pragma once

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"

/** \brief Search the highest-pt lepton (electron or muon)
 *
 * The found lepton is written to the event as FlavourParticle
 * with name "PrimaryLepton". Should be called after proper cleaning
 * modules for electron and muon ID.
 *
 */
class PrimaryLepton: public uhh2::AnalysisModule {
public:
    explicit PrimaryLepton(uhh2::Context & ctx, const std::string & h_name="PrimaryLepton");

    virtual bool process(uhh2::Event & event) override;

    virtual ~PrimaryLepton();

private:
    uhh2::Event::Handle<FlavorParticle> h_primlep;
};