#pragma once

#include "UHH2/core/include/AnalysisModule.h"


/** \brief Set the event weight according to the sample lumi and a target lumi
 *
 * Only use for MC. Will throw an exception for data (i.e. if dataset_type != "MC").
 *
 * Interprets the following settings from the passed Context:
 *  - "dataset_lumi": the lumi of the current dataset, in inverse pb
 *  - "reweight_to_lumi": the data lumi to reweight to, in inverse pb
 */
class MCLumiWeight: public uhh2::AnalysisModule {
public:
    explicit MCLumiWeight(uhh2::Context & ctx);

    virtual bool process(uhh2::Event & event) override;

private:
    double factor;
};


/** \brief Reweight MC based on simulated and actual pileup
 *
 * Interprets "pileup_directory" from Context, which should be set to the
 * path to root files which contain the pileup information for each MC sample and data.
 * If an empty value is given, no pileup reweighting is done.
 * 
 * NOTE: currently, this is a dummy class; construction will fail with an excpetion, unless
 * an empty value is given for "pileup_directory".
 */
class MCPileupReweight: public uhh2::AnalysisModule {
public:
    explicit MCPileupReweight(uhh2::Context & ctx);

    virtual bool process(uhh2::Event & event) override;
};
