#pragma once

#include "UHH2/core/include/AnalysisModule.h"


/** \brief Set the event weight according to the sample lumi and a target lumi
 *
 * Only use for MC. Will throw an exception for data (i.e. if dataset_type != "MC").
 * 
 * This module only has an effect if "use_sframe_weight" is set to "false" to avoid
 * applying the same weight twice.
 *
 * Interprets the following settings from the passed Context:
 *  - "dataset_lumi": the lumi of the current dataset, in inverse pb
 *  - "target_lumi": the data lumi to reweight to, in inverse pb
 * These are set automatically if running via SFrame by AnalysisModuleRunner.
 * 
 * The event weight is multiplied with the ratio of dataset_lumi and target_lumi, independent
 * of the maximum number of processed events. This is different from sframe which
 * takes into account the number of events to process.
 */
class MCLumiWeight: public uhh2::AnalysisModule {
public:
    explicit MCLumiWeight(uhh2::Context & ctx);

    virtual bool process(uhh2::Event & event) override;

private:
    double factor = 1.0;
    bool use_sframe_weight;
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
