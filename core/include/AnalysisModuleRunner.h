#pragma once


// NOTE: this file must be parsable by CINT, so it cannot use C++11 features. Therefore,
// keep it small and use pimpl-idiom for actual implementation.

#include "core/include/SCycleBase.h"
#include "UHH2/core/include/fwd.h"

#include <memory>
#include <string>
#include <list>
#include <map>

namespace uhh2 {

class SFrameContext;

/** \brief The SFrame cycle used to run an AnalysisModule
 *
 * This is the component which runs within the SFrame framework and provides
 * the bridge to the AnalysisModule-based framework.
 *
 * It takes care of
 *  - preparing the input tree according to the configuration
 *  - constructing the AnalysisModule class
 *  - calling AnalysisModule::process for each event
 *  - writing the tree of selected events
 *
 * For details on the configuration, refer to the example xml files, in particular
 * examples/config/Example.xml which lists all configuration options.
 */
class AnalysisModuleRunner: public SCycleBase{
public:
    // make GetOutputFile public for SFrameContext;
    using SCycleBase::GetOutputFile;

    AnalysisModuleRunner();

    // called at the beginning of the cycle, but for proof only in one process!
    void BeginCycle() override {}
    void EndCycle() override {}

    // called at the beginning of the input data, on all proof nodes:
    void BeginInputData( const SInputData& ) override;
    void EndInputData  ( const SInputData& ) override {};

    // called at the beginning of an input file, on the proof nodes:
    void BeginInputFile( const SInputData& ) override;
    void ExecuteEvent( const SInputData&, Double_t ) override;

    // called after processing the dataset, only on the proof master, not on the proof nodes:
    void EndMasterInputData(const SInputData &) override;

    void CloseOutputFile() override;
    virtual void Initialize( TXMLNode* node ) override;
    virtual void SetConfig(const SCycleConfig& config) override;

    virtual ~AnalysisModuleRunner();

    ClassDefOverride(AnalysisModuleRunner, 0);


    class AnalysisModuleRunnerImpl;
    std::unique_ptr<AnalysisModuleRunnerImpl> pimpl;
};

}

