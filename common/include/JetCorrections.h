#pragma once

#include "UHH2/core/include/AnalysisModule.h"

class FactorizedJetCorrector;

/// namespace to define some useful filename constants to be used for jet energy corrections
namespace JERFiles {
    extern const std::vector<std::string> PHYS14_L123_MC;
    extern const std::vector<std::string> PHYS14_L123_DATA;
}

/** \brief (Re-)Correct jets according to the corrections in the passed txt files
 * 
 * txt files are available in JetMETObjects/data/; see README there for instructions how to produce
 * updated files.
 * 
 * For some standard jet energy corrections, you can use filenames defined in the JERFiles namespace.
 */
class JetCorrector: public uhh2::AnalysisModule {
public:
    explicit JetCorrector(const std::vector<std::string> & filenames);
    
    virtual bool process(uhh2::Event & event) override;
    
    virtual ~JetCorrector();
    
private:
    std::unique_ptr<FactorizedJetCorrector> corrector;
};

/** \brief Smear the jet four-momenta in MC to match the resolution in data
 *
 * The corrections applied correspond to the values listed here:
 * https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetResolution
 * for 8TeV data (r36 from 2014-08-28) using the method to scale
 * the genjet pt - reco pt difference.
 *
 * Run this *after* the jet energy corrections.
 *
 * IMPORTANT: do NOT run the module twice, as then, the jets will be smeared twice, which
 * is too much.
 *
 * Options parsed from the given Context:
 *  - "jersmear_smear_met": if "true", propagate the jet resolution smearing to MET. Default is false.
 *  - "jersmear_direction": either "nominal", "up", or "down" to apply nominal, +1sigma, -1sigma smearing resp.
 */
class JetResolutionSmearer: public uhh2::AnalysisModule {
public:
    explicit JetResolutionSmearer(uhh2::Context & ctx);

    virtual bool process(uhh2::Event & event) override;

    virtual ~JetResolutionSmearer();
private:
    void set_jer_applied(bool v);
    
    bool smear_met;
    int direction = 0; // -1 = down, +1 = up, 0 = nominal
    bool first_event = true;
    uhh2::Context::MetadataObject<bool> jer_applied;
};
