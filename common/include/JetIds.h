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

/**
 * Jet Id following recomandations for 13 TeV:
 * https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetID
 *
 * For |eta|<=3.0 Apply
 * looseJetID = (NHF<0.99 && NEMF<0.99 && NumConst>1) && ((abs(eta)<=2.4 && CHF>0 && CHM>0 && CEMF<0.99) || abs(eta)>2.4) && abs(eta)<=3.0
 * tightJetID = (NHF<0.90 && NEMF<0.90 && NumConst>1) && ((abs(eta)<=2.4 && CHF>0 && CHM>0 && CEMF<0.99) || abs(eta)>2.4) && abs(eta)<=3.0
 * tightLepVetoJetID = (NHF<0.90 && NEMF<0.90 && NumConst>1 && MUF<0.8) && ((abs(eta)<=2.4 && CHF>0 && CHM>0 && CEMF<0.90) || abs(eta)>2.4) && abs(eta)<=3.0
 *
 * For |eta|> 3.0 Apply
 * looseJetID = (NEMF<0.90 && NumNeutralParticle>10 && abs(eta)>3.0 )
 * tightJetID = (NEMF<0.90 && NumNeutralParticle>10 && abs(eta)>3.0 ) 
 *
 *
 * once hf is understood may need to be changed!
 */


class JetPFID {
 public:
  enum wp {WP_LOOSE, WP_TIGHT, WP_TIGHT_LEPVETO};
  explicit JetPFID(wp working_point);
  bool operator()(const Jet&, const uhh2::Event&) const;
 private:
  wp m_working_point;
  bool looseID(const Jet & jet) const;
  bool tightID(const Jet & jet) const;
  bool tightLepVetoID(const Jet & jet) const;
};
