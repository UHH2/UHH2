#include "UHH2/common/include/AdditionalSelections.h"
#include "UHH2/core/include/AnalysisModule.h"

#include <stdexcept>

using namespace uhh2;
using namespace std;


GenParticleIdSelection::GenParticleIdSelection(const GenParticleId & id, int n_min, int n_max): 
    id_(id), n_min_(n_min), n_max_(n_max) {}

bool GenParticleIdSelection::passes(const Event & event)
{
    int n_passes = 0;
    for (const auto & genp : *event.genparticles )
    {
        if (id_(genp, event))
            n_passes++;
    }
    return n_passes >= n_min_ && (n_max_ < 0 || n_passes <= n_max_);
}

BoolSelection::BoolSelection(Event::Handle<bool> const & hndl) : hndl_(hndl) {}

bool BoolSelection::passes(const Event & event)
{
    bool pass = event.get(hndl_);
    return pass;
}

VetoSelection::VetoSelection(std::shared_ptr<Selection> sel) : sel_(sel) {}

bool VetoSelection::passes(const Event & event)
{
    bool pass = sel_->passes(event);
    return !pass;
}

bool OrSelection::passes(const Event & event)
{
    for(size_t i=0; i<selections.size(); ++i){
        if(selections[i]->passes(event)){
            return true;
        }
    }
    return false;
}