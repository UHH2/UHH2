#pragma once
#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Selection.h"
#include "UHH2/core/include/Utils.h"

#include "UHH2/common/include/ObjectIdUtils.h"
#include "UHH2/common/include/NSelections.h"
#include "UHH2/common/include/JetIds.h"
#include "UHH2/common/include/JetCorrections.h"
#include "UHH2/common/include/CleaningModules.h"

/** \brief Run a configurable list commonly used modules
 *
 * This is a convenience class which runs a number of commonly-used
 * AnalysisModules in the right order. It is mainly intended
 * to get started quickly with a new analysis. It is configurable
 * to some extent by setting object ids or by disabling certain modules.
 *
 * The AnalysisModules run are (in this order):
 *  - good run selection (for data only) based on lumi_file defined in xml input
 *  - MCLumiWeight (for MC only; only has an effect if "use_sframe_weight" is set to false)
 *  - MCPileupReweight (for MC only)
 *  - JetCorrector using the latest PHYS14 corrections for MC
 *  - JetResolutionSmearer  (for MC only)
 *  - apply MET filters (all filters listed here: https://twiki.cern.ch/twiki/bin/viewauth/CMS/MissingETOptionalFiltersRun2#MiniAOD_76X_v2),
 *      at least one good primary vertex is only required if pvfilter is also active
 *  - Primary vertex cleaner (remove all non-good PVs from the list of primary vertices)
 *  - ElectronCleaner
 *  - Muon Cleaner
 *  - Tau Cleaner
 *  - PF ID JEt Cleaning
 *  - Jet Lepton Cleaning
 *  - JetCleaner
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
 *  ... more set_*_id or disable_* calls ...
 *  cm->init(context);
 *
 * // in AnalysisModule::process:
 *  bool pass_cm = cm->process(event);
 *  if(!pass_cm) return false;
 * \endcode
 *
 * In particular, call the 'set_*_id' methods in the constructor; call init after setting
 * all the object ids at the end of the constructor.
 */
class CommonModules: public uhh2::AnalysisModule {
public:
    CommonModules();
    // disable certain modules; see list above
    void disable_mclumiweight();
    void disable_mcpileupreweight();
    void disable_jec();
    void disable_jersmear();
    void disable_lumisel();
    void disable_metfilters();
    void disable_pvfilter();
    void disable_jetpfidfilter(){fail_if_init();jetpfidcleaner=false;};
    void switch_jetlepcleaner(bool status = true){fail_if_init();jetlepcleaner=status;}
    //void switch_topjetlepcleaner(bool status = true, double dR = 0.8){fail_if_init();topjetlepcleaner=status;topjetcleanerDR = dR;}
    void switch_jetPtSorter(bool status = true){fail_if_init();jetptsort=status;}
    void switch_metcorrection(bool status = true){fail_if_init();do_metcorrection=status;};
    void set_HTjetid(const boost::optional<JetId> & jetid = boost::none) {HT_jetid = jetid;}

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
    void change_pf_id(JetPFID::wp working_point_){
      fail_if_init();
      working_point=working_point_;
    }
    virtual bool process(uhh2::Event & event) override;
    void init(uhh2::Context & ctx, const std::string & SysType_PU = "central");

private:
    void fail_if_init() const;

    std::vector<std::unique_ptr<AnalysisModule>> modules;
    JetId jetid;
    ElectronId eleid;
    MuonId muid;
    TauId tauid;
    JetPFID::wp working_point;
    std::unique_ptr<JetCorrector> jet_corrector_MC, jet_corrector_B, jet_corrector_C, jet_corrector_DE, jet_corrector_F;
    std::unique_ptr<JetLeptonCleaner_by_KEYmatching> JLC_MC, JLC_B, JLC_C, JLC_DE, JLC_F;
    std::unique_ptr<JetResolutionSmearer> jet_resolution_smearer;
    std::unique_ptr<JetCleaner> jet_cleaner;
    const int runnr_B = 299329;
    const int runnr_C = 302029;
    const int runnr_DE = 304826;
    const int runnr_F = 306462;

    bool mclumiweight = true, mcpileupreweight = true, jersmear = true, jec = true, lumisel=true, jetlepcleaner = false, topjetlepcleaner =false, jetptsort = false, metfilters = true, pvfilter = true, jetpfidcleaner=true, do_metcorrection = false;

    double topjetcleanerDR;
    bool is_mc;
    bool init_done = false;

    std::unique_ptr<Selection> lumi_selection;
    std::unique_ptr<AndSelection> metfilters_selection;

    boost::optional<JetId> HT_jetid;
};
