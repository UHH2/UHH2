#include "UHH2/common/include/CommonModules.h"
#include "UHH2/common/include/JetCorrections.h"
#include "UHH2/common/include/MCWeight.h"
#include "UHH2/common/include/EventVariables.h"
#include "UHH2/common/include/CleaningModules.h"
#include "UHH2/common/include/EventVariables.h"

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
        modules.emplace_back(new MCLumiWeight(ctx));
        modules.emplace_back(new MCPileupReweight(ctx));
        modules.emplace_back(new JetCorrector(JERFiles::PHYS14_L123_MC));
        modules.emplace_back(new JetResolutionSmearer(ctx));
    }
    else{
        modules.emplace_back(new JetCorrector(JERFiles::PHYS14_L123_DATA));
    }

    if(jetid){
        modules.emplace_back(new JetCleaner(*jetid));
    }
    if(eleid){
        modules.emplace_back(new ElectronCleaner(*eleid));
    }
    if(muid){
        modules.emplace_back(new MuonCleaner(*muid));
    }
    if(tauid){
        modules.emplace_back(new TauCleaner(*tauid));
    }
    modules.emplace_back(new HTCalculator(ctx));
}

bool CommonModules::process(uhh2::Event & event){
    for(auto & m : modules){
        m->process(event);
    }
    return true;
}
