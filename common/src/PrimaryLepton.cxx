#include "UHH2/common/include/PrimaryLepton.h"
#include "UHH2/core/include/Utils.h"

using namespace uhh2;
using namespace std;

PrimaryLepton::PrimaryLepton(Context & ctx, 
                             const std::string & h_name,
                             float min_ele_pt,
                             float min_mu_pt) :
    h_primlep(ctx.get_handle<FlavorParticle>(h_name)),
    min_ele_pt_(min_ele_pt),
    min_mu_pt_(min_mu_pt)
{}

bool PrimaryLepton::process(uhh2::Event & event) {
    assert(event.muons || event.electrons);
    double ptmax = -infinity;
    FlavorParticle primlep;
    if(event.electrons) {
        for(const auto & ele : *event.electrons) {
            float ele_pt = ele.pt();
            if(ele_pt > min_ele_pt_ && ele_pt > ptmax) {
                ptmax = ele_pt;
                primlep = ele;
            }
        }
    }
    if(event.muons) {
        for(const auto & mu : *event.muons) {
            float mu_pt = mu.pt();
            if(mu_pt > min_mu_pt_ && mu_pt > ptmax) {
                ptmax = mu_pt;
                primlep = mu;
            }
        }
    }
    event.set(h_primlep, std::move(primlep));
    return true;
}

PrimaryLepton::~PrimaryLepton() {}