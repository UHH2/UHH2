#include "UHH2/common/include/CommonModules.h"
#include "UHH2/common/include/JetCorrections.h"
#include "UHH2/common/include/MCWeight.h"
#include "UHH2/common/include/EventVariables.h"
#include "UHH2/common/include/CleaningModules.h"
#include "UHH2/common/include/EventVariables.h"
#include "UHH2/common/include/LumiSelection.h"

using namespace uhh2;
using namespace std;

void CommonModules::fail_if_init() const{
    if(init_done){
        throw invalid_argument("CommonModules::init already called!");
    }
}

void CommonModules::init(Context & ctx){
    if(init_done){
        throw invalid_argument("CommonModules::init called twice!");
    }
    init_done = true;
    bool is_mc = ctx.get("dataset_type") == "MC";

    if(is_mc){
        if(mclumiweight)  modules.emplace_back(new MCLumiWeight(ctx));
        if(mcpileupreweight) modules.emplace_back(new MCPileupReweight(ctx));
        if(jec) modules.emplace_back(new JetCorrector(JERFiles::PHYS14_L123_MC));
        if(jersmear) modules.emplace_back(new JetResolutionSmearer(ctx));
    }
    else{
        if(lumisel) lumi_selection.reset(new LumiSelection(ctx));
        if(jec) modules.emplace_back(new JetCorrector(JERFiles::PHYS14_L123_DATA));
    }

    if(jetid){
        modules.emplace_back(new JetCleaner(jetid));
    }
    if(eleid){
        modules.emplace_back(new ElectronCleaner(eleid));
    }
    if(muid){
        modules.emplace_back(new MuonCleaner(muid));
    }
    if(tauid){
        modules.emplace_back(new TauCleaner(tauid));
    }
    modules.emplace_back(new HTCalculator(ctx));
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
        return common->process(event);
    }
private:
    std::unique_ptr<CommonModules> common;
};

UHH2_REGISTER_ANALYSIS_MODULE(TestCommonModules)


