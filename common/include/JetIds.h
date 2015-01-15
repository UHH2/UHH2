#pragma once

#include "UHH2/core/include/Event.h"

// see comments in ElectronIds.h for general comments on IDs.

/** \brief The CSV V2 b-tag as jet id
 * 
 * Cuts on uhh2::Jet::btag_combinedSecondaryVertex, which (in CMSSW) is called "combinedInclusiveSecondaryVertexV2BJetTags"
 * (see core/plugins/NtupleWriterJets.cxx), also known as "CombinedSecondaryVertex v2 with IVF".
 * 
 * Uses the preliminary thresholds for that tagger recommended here:
 *  https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation53XReReco
 * 
 * Note that this will certainly need updates once 13TeV recommendations are available.
 */
class CSVBTag {
public:
    enum wp {WP_LOOSE, WP_MEDIUM, WP_TIGHT };
    
    explicit CSVBTag(wp working_point);
    
    bool operator()(const Jet & jet, const uhh2::Event & event) const;
private:
    float csv_threshold;
};

