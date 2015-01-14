#pragma once

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/common/include/ObjectIdUtils.h"


/** \brief Run a configurable list commonly used modules
 *
 * This is a convenience class which runs a number of commonly-used
 * AnalysisModules in the right order. It is mainly intended
 * to get started quickly with a new analysis. It is configurable
 * to some extent by setting object ids or by disabling certain modules.
 *
 * The AnalysisModules run are (in this order):
 *  - MCLumiWeight (for MC only)
 *  - MCPileupReweight (for MC only)
 *  - JetCorrector using the latest PHYS14 corrections for MC
 *  - JetResolutionSmearer  (for MC only)
 *  - JetCleaner
 *  - ElectronCleaner
 *  - Muon Cleaner
 *  - Tau Cleaner
 *  - HTCalculator
 *
 * The cleaners are only run if an id was set via the according set methods, otherwise
 * no cleaner is run and all objects are kept.
 * All other modules are run as default; see the 'disable_*' methods below for disabling
 * certain modules, though.
 * 
 * Usage:
 * \code
 *  // in the AnalysisModule constructor:
 *  cm.reset(new CommonModules); // assuming a 'std::unique_ptr<CommonModules> cm' as member varibale
 *  cm->set_jet_id(...);
 *  ... other set_*_id calls ...
 *  cm->init(context);
 * 
 * // in AnalysisModule::process:
 * cm->process(event)
 * \endcode
 *
 * In particular, call the 'set_*_id' methods in the constructor; call init after setting
 * all the object ids at the end of the constructor.
 *
 * NOTE: currently, many modules used by this one are not yet fully
 * implemented. Therefore, init might fail with an exception.
 */
class CommonModules: public uhh2::AnalysisModule {
public:
    
    explicit CommonModules(uhh2::Context & ctx);
    
    // disable certain modules; see list above
    void disable_mclumiweight();
    void disable_mcpileupreweight();
    void disable_jec();
    void disable_jersmear();

    void set_jet_id(const JetId & jetid_){
        fail_if_init();
        jetid = jetid_;
    }
    void set_electron_id(const ElectronId & eleid_){
        fail_if_init();
        eleid = eleid_;
    }
    void set_muon_id(const MuonId & muid_){
        fail_if_init();
        muid = muid_;
    }
    void set_tau_id(const TauId & tauid_){
        fail_if_init();
        tauid = tauid_;
    }

    void init(uhh2::Context & ctx);

    virtual bool process(uhh2::Event & event) override;

private:
    void fail_if_init() const;

    std::vector<std::unique_ptr<AnalysisModule>> modules;
    JetId jetid;
    ElectronId eleid;
    MuonId muid;
    TauId tauid;
    
    bool mclumiweight = true, mcpileupreweight = true, jersmear = true, jec = true;

    bool init_done = false;
};

