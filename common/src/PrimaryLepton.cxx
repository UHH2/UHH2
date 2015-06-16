#include "UHH2/common/include/PrimaryLepton.h"
#include "UHH2/core/include/Utils.h"

using namespace uhh2;
using namespace std;

PrimaryLepton::PrimaryLepton(Context & ctx, const std::string & h_name) {
    h_primlep = ctx.get_handle<FlavorParticle>(h_name);
}

bool PrimaryLepton::process(uhh2::Event & event) {
    assert(event.muons || event.electrons);
    double ptmax = -infinity;
    FlavorParticle primlep;
    if(event.electrons) {
        for(const auto & ele : *event.electrons) {
            if(ele.pt() > ptmax) {
                ptmax = ele.pt();
                primlep = ele;
            }
        }
    }
    if(event.muons) {
        for(const auto & mu : *event.muons) {
            if(mu.pt() > ptmax) {
                ptmax = mu.pt();
                primlep = mu;
            }
        }
    }
    event.set(h_primlep, std::move(primlep));
    return true;
}

PrimaryLepton::~PrimaryLepton() {}