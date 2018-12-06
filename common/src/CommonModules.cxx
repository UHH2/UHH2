#include "UHH2/common/include/CommonModules.h"
#include "UHH2/common/include/JetCorrections.h"
#include "UHH2/common/include/MCWeight.h"
#include "UHH2/common/include/EventVariables.h"
#include "UHH2/common/include/CleaningModules.h"
#include "UHH2/common/include/EventVariables.h"
#include "UHH2/common/include/LumiSelection.h"
#include "UHH2/common/include/Utils.h"
#include "UHH2/common/include/TriggerSelection.h"

using namespace uhh2;
using namespace std;

void CommonModules::fail_if_init() const{
    if(init_done){
        throw invalid_argument("CommonModules::init already called!");
    }
}


CommonModules::CommonModules(){
  working_point = JetPFID::WP_TIGHT;
}


void CommonModules::init(Context & ctx, const std::string & SysType_PU){
    if(init_done){
        throw invalid_argument("CommonModules::init called twice!");
    }
    init_done = true;
    is_mc = ctx.get("dataset_type") == "MC";
    //set default PV id;
    PrimaryVertexId pvid=StandardPrimaryVertexId();
    if(pvfilter) modules.emplace_back(new PrimaryVertexCleaner(pvid));
    if(is_mc){
        if(mclumiweight)  modules.emplace_back(new MCLumiWeight(ctx));
        if(mcpileupreweight) modules.emplace_back(new MCPileupReweight(ctx,SysType_PU));
        if(jec){
	  jet_corrector_MC.reset(new JetCorrector(ctx, JERFiles::Fall17_17Nov2017_V32_L123_AK4PFchs_MC));
	}
        if(jersmear) jet_resolution_smearer.reset(new JetResolutionSmearer(ctx));
    }
    else{
       if(lumisel) lumi_selection.reset(new LumiSelection(ctx));
       if(jec){
	 jet_corrector_B.reset(new JetCorrector(ctx, JERFiles::Fall17_17Nov2017_V32_B_L123_AK4PFchs_DATA));
	 jet_corrector_C.reset(new JetCorrector(ctx, JERFiles::Fall17_17Nov2017_V32_C_L123_AK4PFchs_DATA));
	 jet_corrector_DE.reset(new JetCorrector(ctx, JERFiles::Fall17_17Nov2017_V32_DE_L123_AK4PFchs_DATA));
	 jet_corrector_F.reset(new JetCorrector(ctx, JERFiles::Fall17_17Nov2017_V32_F_L123_AK4PFchs_DATA));
       }
    }
    if(metfilters){
      // https://twiki.cern.ch/twiki/bin/viewauth/CMS/MissingETOptionalFiltersRun2
       metfilters_selection.reset(new AndSelection(ctx, "metfilters"));
       metfilters_selection->add<TriggerSelection>("HBHENoiseFilter", "Flag_HBHENoiseFilter");
       metfilters_selection->add<TriggerSelection>("HBHENoiseIsoFilter", "Flag_HBHENoiseIsoFilter");
       metfilters_selection->add<TriggerSelection>("globalSuperTightHalo2016Filter", "Flag_globalSuperTightHalo2016Filter");
       metfilters_selection->add<TriggerSelection>("EcalDeadCellTriggerPrimitiveFilter", "Flag_EcalDeadCellTriggerPrimitiveFilter");
       if (!is_mc) metfilters_selection->add<TriggerSelection>("eeBadScFilter", "Flag_eeBadScFilter");  // Not recommended for MC, but do check
       metfilters_selection->add<TriggerSelection>("BadChargedCandidateFilter", "Flag_BadChargedCandidateFilter");
       metfilters_selection->add<TriggerSelection>("BadPFMuonFilter", "Flag_BadPFMuonFilter");
       metfilters_selection->add<TriggerSelection>("goodVertices", "Flag_goodVertices");
       metfilters_selection->add<TriggerSelection>("ecalBadCalibFilter", "Flag_ecalBadCalibFilter");
       if(pvfilter) metfilters_selection->add<NPVSelection>("1 good PV",1,-1,pvid);
    }
    if(eleid) modules.emplace_back(new ElectronCleaner(eleid));
    if(muid)  modules.emplace_back(new MuonCleaner(muid));
    if(tauid) modules.emplace_back(new TauCleaner(tauid));
    if(jetpfidcleaner){
      modules.emplace_back(new JetCleaner(ctx, JetPFID(working_point)));
    }
    if(jetlepcleaner) {
      if(is_mc)	JLC_MC.reset(new JetLeptonCleaner_by_KEYmatching(ctx, JERFiles::Fall17_17Nov2017_V32_L123_AK4PFchs_MC));
      else{
	JLC_B.reset(new JetLeptonCleaner_by_KEYmatching(ctx, JERFiles::Fall17_17Nov2017_V32_B_L123_AK4PFchs_DATA));
	JLC_C.reset(new JetLeptonCleaner_by_KEYmatching(ctx, JERFiles::Fall17_17Nov2017_V32_C_L123_AK4PFchs_DATA));
	JLC_DE.reset(new JetLeptonCleaner_by_KEYmatching(ctx, JERFiles::Fall17_17Nov2017_V32_DE_L123_AK4PFchs_DATA));
	JLC_F.reset(new JetLeptonCleaner_by_KEYmatching(ctx, JERFiles::Fall17_17Nov2017_V32_F_L123_AK4PFchs_DATA));
      }
    }
    modules.emplace_back(new HTCalculator(ctx,HT_jetid));
    if(jetid) jet_cleaner.reset(new JetCleaner(ctx, jetid));
}

bool CommonModules::process(uhh2::Event & event){
    if(!init_done){
        throw runtime_error("CommonModules::init not called (has to be called in AnalysisModule constructor)");
    }
    if(event.isRealData && lumisel){
        if(!lumi_selection->passes(event)) return false;
    }
    for(auto & m : modules){
        m->process(event);
    }
    if(metfilters){
        if(!metfilters_selection->passes(event)) return false;
    }

    if(jetlepcleaner){
      if(is_mc) JLC_MC->process(event);
      else{
	if(event.run <= runnr_B)      JLC_B->process(event);
	else if(event.run <= runnr_C) JLC_C->process(event);
	else if(event.run <= runnr_DE) JLC_DE->process(event);
        else if(event.run <= runnr_F) JLC_F->process(event);
	else throw runtime_error("CommonModules.cxx: run number not covered by if-statements in process-routine.");
      }
    }

    if(jec){
      if(is_mc)jet_corrector_MC->process(event);
      else{
	if(event.run <= runnr_B)      jet_corrector_B->process(event);
	else if(event.run <= runnr_C) jet_corrector_C->process(event);
	else if(event.run <= runnr_DE) jet_corrector_DE->process(event);
	else if(event.run <= runnr_F) jet_corrector_F->process(event);
	else throw runtime_error("CommonModules.cxx: run number not covered by if-statements in process-routine.");
      }
    }

    if(jersmear && is_mc) jet_resolution_smearer->process(event);

    //set do_metcorrection = true in case you applied jet lepton cleaning by yourself and before calling common modules
    if((jetlepcleaner && jec) || (do_metcorrection && jec)){
      if(is_mc) jet_corrector_MC->correct_met(event);
      else{
	if(event.run <= runnr_B)      jet_corrector_B->correct_met(event);
	else if(event.run <= runnr_C) jet_corrector_C->correct_met(event);
	else if(event.run <= runnr_DE) jet_corrector_DE->correct_met(event);
	else if(event.run <= runnr_F) jet_corrector_F->correct_met(event);
	else throw runtime_error("CommonModules.cxx: run number not covered by if-statements in process-routine.");
      }
    }
    else if(jec || jetlepcleaner) cout <<"WARNING: You used CommonModules for either JEC or jet-lepton-cleaning but MET is not corrected. Please be aware of this." << endl;

    if(jetid) jet_cleaner->process(event);

    if(jetptsort){
      sort_by_pt(*event.jets);
    }
    return true;
}


void CommonModules::disable_mclumiweight(){
    fail_if_init();
    mclumiweight = false;
}

void CommonModules::disable_mcpileupreweight(){
    fail_if_init();
    mcpileupreweight = false;
}

void CommonModules::disable_jec(){
    fail_if_init();
    jec = false;
}

void CommonModules::disable_jersmear(){
    fail_if_init();
    jersmear = false;
}

void CommonModules::disable_lumisel(){
    fail_if_init();
    lumisel = false;
}

void CommonModules::disable_metfilters(){
    fail_if_init();
    metfilters = false;
}

void CommonModules::disable_pvfilter(){
    fail_if_init();
    pvfilter = false;
}

class TestCommonModules: public AnalysisModule {
public:
    TestCommonModules(uhh2::Context & ctx){
        common.reset(new CommonModules());
        // in a non-trivial usage, would call
        // common->set_*_id   and
        // common->disable_*  here.
	common->init(ctx);
    }

    virtual bool process(Event & event) override {
         bool pass_cm = common->process(event);
 	if(!pass_cm) std::cout << "Event rejected by common modules" << std::endl;
	return pass_cm;
    }
private:
    std::unique_ptr<CommonModules> common;
};

UHH2_REGISTER_ANALYSIS_MODULE(TestCommonModules)
