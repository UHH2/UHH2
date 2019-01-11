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
    explicit CSVBTag(float float_point);

    bool operator()(const Jet & jet, const uhh2::Event & event) const;
private:
    float csv_threshold;
};

class DeepCSVBTag {
public:
    enum wp {WP_LOOSE, WP_MEDIUM, WP_TIGHT };

    explicit DeepCSVBTag(wp working_point);
    explicit DeepCSVBTag(float float_point);

    bool operator()(const Jet & jet, const uhh2::Event & event) const;
private:
    float deepcsv_threshold;
};


/**
 * Jet Id following recomandations for 2017 13 TeV:
 * https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetID13TeVRun2017
 */
class JetPFID {
 public:
  enum wp {WP_TIGHT, WP_TIGHT_LEPVETO, WP_TIGHT_PUPPI, WP_TIGHT_LEPVETO_PUPPI};
  explicit JetPFID(wp working_point);
  bool operator()(const Jet&, const uhh2::Event&) const;
 private:
  wp m_working_point;
  bool tightID(const Jet & jet) const;
  bool tightLepVetoID(const Jet & jet) const;
  bool tightID_PUPPI(const Jet & jet) const;
  bool tightLepVetoID_PUPPI(const Jet & jet) const;
};
