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
    void setup_output();
    void FillTriggerNames();
    
    std::string m_JetCollection, m_GenJetCollection, m_ElectronCollection, m_MuonCollection, 
      m_TauCollection, m_PhotonCollection, m_PrimaryVertexCollection, m_METName, m_TopJetCollection, m_TopJetCollectionGen,
      m_GenParticleCollection;
    bool m_readCommonInfo, m_addGenInfo, m_readTrigger;
    
    // for reading trigger names:
    int m_runid_triggernames; // the run id which corresponds to the currently filled "actual" trigger names in m_bcc
    std::vector<std::string> m_triggerNames; // of the run m_runid_triggernames; should usually match current run
    std::vector<std::string> * m_input_triggerNames; // as in the input branch
    
    // trigger name output is special: we only write a non-empty list the first time
    // we see this runid. So save the runid for which we already saved the triggerNames
    // and write an empty list in ther cases:
    std::vector<std::string> m_output_triggerNames;
    int m_output_triggerNames_runid;
    
    std::auto_ptr<GenericEventStructure> ges;
    std::auto_ptr<Event> event;
    bool first_event_inputdata;
    
    std::auto_ptr<SFrameContext> context;
       
    // the actual analysis module to run:
    std::auto_ptr<AnalysisModule> analysis;
    
    std::map<std::string, std::string> dummyConfigVars;
};

}

