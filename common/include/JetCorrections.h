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
 * updates files.
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
