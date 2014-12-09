#include "UHH2/common/include/MCWeight.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/Utils.h"

#include <stdexcept>

using namespace uhh2;
using namespace std;

MCLumiWeight::MCLumiWeight(Context & ctx){
    auto dataset_type = ctx.get("dataset_type");
    bool is_mc = dataset_type  == "MC";
    if(!is_mc){
        throw runtime_error("MCLumiWeight created for a non-MC sample (dataset_type = '" + dataset_type + "'");
    }
    double dataset_lumi = string2double(ctx.get("dataset_lumi"));
    double reweight_to_lumi = string2double(ctx.get("reweight_to_lumi"));
    if(reweight_to_lumi <= 0.0){
        throw runtime_error("MCLumiWeight: reweight_to_lumi was <= 0.0");
    }
    factor = reweight_to_lumi / dataset_lumi;
}

bool MCLumiWeight::process(uhh2::Event & event){
    event.weight *= factor;
    return true;
}



MCPileupReweight::MCPileupReweight(Context & ctx){
    throw runtime_error("MCPileupReweight: not yet implemented!");
}

bool MCPileupReweight::process(Event & event){
    // FIXME: implement
    return true;
}

