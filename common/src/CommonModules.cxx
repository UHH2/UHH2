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
  working_point = JetPFID::WP_LOOSE;
}


void CommonModules::init(Context & ctx){
    if(init_done){
        throw invalid_argument("CommonModules::init called twice!");
    }
    init_done = true;
    bool is_mc = ctx.get("dataset_type") == "MC";
    //set default PV id;
    PrimaryVertexId pvid=StandardPrimaryVertexId();
    if(pvfilter) modules.emplace_back(new PrimaryVertexCleaner(pvid));
    if(is_mc){
        if(mclumiweight)  modules.emplace_back(new MCLumiWeight(ctx));
        if(mcpileupreweight) modules.emplace_back(new MCPileupReweight(ctx));
        if(jec) modules.emplace_back(new JetCorrector(JERFiles::Summer15_25ns_L123_AK4PFchs_MC));
        if(jersmear) modules.emplace_back(new JetResolutionSmearer(ctx));
    }
    else{
       if(lumisel) lumi_selection.reset(new LumiSelection(ctx));
       if(jec) modules.emplace_back(new JetCorrector(JERFiles::Summer15_25ns_L123_AK4PFchs_DATA));
    }
    if(metfilters){
       metfilters_selection.reset(new AndSelection(ctx, "metfilters"));
       metfilters_selection->add<TriggerSelection>("HBHENoiseIsoFilter", "Flag_HBHENoiseIsoFilter");
       metfilters_selection->add<TriggerSelection>("CSCTightHalo", "Flag_CSCTightHaloFilter");
       metfilters_selection->add<TriggerSelection>("eeBadSc", "Flag_eeBadScFilter");
       if(pvfilter) metfilters_selection->add<NPVSelection>("1 good PV",1,-1,pvid);
    }
    if(eleid) modules.emplace_back(new ElectronCleaner(eleid));
    if(muid)  modules.emplace_back(new MuonCleaner(muid));
    if(tauid) modules.emplace_back(new TauCleaner(tauid));
    if(jetpfidcleaner){
      modules.emplace_back(new JetCleaner(JetPFID(working_point)));
    }
    if(jetlepcleaner) {
      if(is_mc) modules.emplace_back(new JetLeptonCleaner(JERFiles::Summer15_25ns_L123_AK4PFchs_MC));
      else modules.emplace_back(new JetLeptonCleaner(JERFiles::Summer15_25ns_L123_AK4PFchs_DATA));
    }
    if(jetid) modules.emplace_back(new JetCleaner(jetid));
    modules.emplace_back(new HTCalculator(ctx));
}

bool CommonModules::process(uhh2::Event & event){
    if(!init_done){
        throw runtime_error("CommonModules::init not called (has to be called in AnalysisModule constructor)");
    }
    if(event.isRealData && lumisel){
        if(!lumi_selection->passes(event)) return false;
    }
    if(metfilters){
        if(!metfilters_selection->passes(event)) return false;
    }
    for(auto & m : modules){
        m->process(event);
    }
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


