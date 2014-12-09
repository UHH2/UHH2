#include "UHH2/common/include/EventVariables.h"

using namespace uhh2;

HTCalculator::HTCalculator(Context & ctx, const boost::optional<JetId> & jetid_, const std::string & name):
        jetid(jetid_){
    h_ht = ctx.get_handle<double>(name);
}


bool HTCalculator::process(Event & event){
    double ht = 0.0;
    for(const auto & jet : *event.jets){
        if(jetid && !(*jetid)(jet, event)) continue;
        ht += jet.pt();
    }
    event.set(h_ht, ht);
    return true;
}
