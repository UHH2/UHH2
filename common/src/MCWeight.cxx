#include "UHH2/common/include/MCWeight.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/Utils.h"

#include <stdexcept>

using namespace uhh2;
using namespace std;

MCLumiWeight::MCLumiWeight(Context & ctx){
    use_sframe_weight = string2bool(ctx.get("use_sframe_weight", "true"));
    if(use_sframe_weight){
        return;
    }
    auto dataset_type = ctx.get("dataset_type");
    bool is_mc = dataset_type  == "MC";
    if(!is_mc){
        throw runtime_error("MCLumiWeight created for a non-MC sample (dataset_type = '" + dataset_type + "'");
    }
    double dataset_lumi = string2double(ctx.get("dataset_lumi"));
    double reweight_to_lumi = string2double(ctx.get("target_lumi"));
    if(reweight_to_lumi <= 0.0){
        throw runtime_error("MCLumiWeight: target_lumi <= 0.0 not allowed");
    }
    factor = reweight_to_lumi / dataset_lumi;
}

bool MCLumiWeight::process(uhh2::Event & event){
    if(!use_sframe_weight){
        event.weight *= factor;
    }
    return true;
}



MCPileupReweight::MCPileupReweight(Context & ctx){
    auto pileup_directory = ctx.get("pileup_directory");
    if(pileup_directory != ""){
        throw runtime_error("MCPileupReweight: not yet implemented!");
    }
}

bool MCPileupReweight::process(Event &){
    // FIXME: implement
    return true;
}

