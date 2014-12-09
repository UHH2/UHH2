#pragma once

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/common/include/ObjectIdUtils.h"


/** \brief Run a configurable list commonly used modules
 *
 * This is a convenience class which runs a number of commonly-used
 * AnalysisModules in the right order. It is mainly intended
 * to get started quickly with a new analysis. It is configurable
 * to some extent in that modules can be disabled. For more
 * control, run the AnalysisModules yourself.
 *
 * The AnalysisModules run are (in this order):
 *  - MCLumiWeight (for MC only)
 *  - MCPileupReweight (for MC only)
 *  - JetCorrector using the PHYS14 corrections for MC
 *  - JetResolutionSmearer  (for MC only)
 *  - JetCleaner
 *  - ElectronCleaner
 *  - Muon Cleaner
 *  - Tau Cleaner
 *  - HTCalculator
 *
 *  The cleaners are only run if an id was set via the according set methods.
 *
 *  After setting the ids, call the init method.
 *
 *  NOTE: do not call a set-method after init, and do call init
 *  in the setup phase, i.e. before processing the first event (typically
 *  from a constructor of a derived class of AnalysisModule).
 *
 *  NOTE: currently, many modules used by this one are not yet fully
 *  implemented. Therefore, init might fail with an exception.
 */
class CommonModules: public uhh2::AnalysisModule {
public:

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
    boost::optional<JetId> jetid;
    boost::optional<ElectronId> eleid;
    boost::optional<MuonId> muid;
    boost::optional<TauId> tauid;

    bool init_done = false;
};

