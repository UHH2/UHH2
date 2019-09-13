#pragma once

#include "UHH2/core/include/Selection.h"
#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"


/**
 * Module to remove MC events with overly large weights.
 * These are often due to picking up a too energetic jet e.g. from PU,
 * or just a freak accident of large statistics
 */
class MCLargeWeightKiller: public uhh2::Selection {
public:

    explicit MCLargeWeightKiller(uhh2::Context & ctx,
                                 float recoJetGenHTRatioMax, // maximum allowed ratio of leading reco jet pT / generator HT
                                 float genJetGenHTRatioMax, // maximum allowed ratio of leading gen jet pT / generator HT
                                 float recoJetQScaleRatioMax, // maximum allowed ratio of leading reco jet pT / Q scale
                                 float PUpTHatGenHTRatioMax, // maximum allowed ratio of PU maximum pTHat / gen HT (ensures scale of PU < scale of hard interaction)
                                 float recoJetpTHatRatioMax, // maximum allowed ratio of leading reco jet pT / pTHat
                                 float genJetpTHatRatioMax, // maximum allowed ratio of leading gen jet pT / pTHat
                                 const std::string jetsName="jets", // name of jet collection to be used
                                 const std::string genjetsName="genjets" // name of genjet collection to be used
                                 );
    virtual bool passes(const uhh2::Event & event) override;

private:
    float recoJetGenHTRatioMax_, genJetGenHTRatioMax_;
    float recoJetQScaleRatioMax_;
    float PUpTHatGenHTRatioMax_;
    float recoJetpTHatRatioMax_, genJetpTHatRatioMax_;
    uhh2::Event::Handle<std::vector<Jet>> jetsHandle_;
    uhh2::Event::Handle<std::vector<GenJet>> genjetsHandle_;
};
