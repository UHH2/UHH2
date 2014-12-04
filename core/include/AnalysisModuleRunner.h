#pragma once


// NOTE: this file must be parsable by CINT, so it cannot use C++11 features.

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
 * It takes care of
 *  - preparing the input tree according to the configuration
 *  - constructing the AnalysisModule class
 *  - calling AnalysisModule::process for each event
 *  - writing the tree of selected events
 */
class AnalysisModuleRunner: public SCycleBase{
public:
    AnalysisModuleRunner();
    
    // called at the beginning of the cycle, but for proof only in one process!
    void BeginCycle() throw( SError ){}
    void EndCycle() throw( SError ){}

    // called at the beginning of the input data, on all proof nodes:
    void BeginInputData( const SInputData& ) throw( SError );
    void EndInputData  ( const SInputData& ) throw( SError ){}

    // called at the beginning of an input file, on the proof nodes:
    void BeginInputFile( const SInputData& ) throw( SError );
    void ExecuteEvent( const SInputData&, Double_t ) throw( SError );
    
    // called after processing the dataset, only on the proof master, not on the proof nodes:
    void EndMasterInputData(const SInputData &) throw (SError);
    
    virtual void Initialize( TXMLNode* node ) throw( SError );
    virtual void SetConfig(const SCycleConfig& config);
    
    virtual ~AnalysisModuleRunner();
        
    ClassDef(AnalysisModuleRunner, 0);
  
private:    
    class AnalysisModuleRunnerImpl;
    
    // use primpl-idiom to decouple implementation from
    // rest in order to enable ROOT to parse the header.
    std::auto_ptr<AnalysisModuleRunnerImpl> pimpl;
};

}

