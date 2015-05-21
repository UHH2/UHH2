#include "UHH2/common/include/PartonHT.h"

using namespace uhh2;


PartonHT::PartonHT(const Event::Handle<double> & handle_): handle(handle_){
}

bool PartonHT::process(uhh2::Event & event) {
    assert(event.genparticles);
    event.set(handle, calculate(*event.genparticles));
    return true;
}

double PartonHT::calculate(const std::vector<GenParticle> & genparticles){
    constexpr const int invalid_daughter = (unsigned short)(-1);
    double result = 0.0;
    for(const auto & gp : genparticles){
        if(gp.daughter1() != invalid_daughter || gp.daughter2() != invalid_daughter) continue;
        // if we are here, it means we have a final state particle.
        // Add to HT in cas it is a parton (quark -- including b but not top -- or gluon).
        // Note that the exact HT definition depends on the madgraph configuration, but this
        // should cover the most common case.
        int id = abs(gp.pdgId());
        if((id >= 1 && id <= 5) || (id == 21)){
            result += gp.pt();
        }
    }
    return result;
}



DoubleSelection::DoubleSelection(const Event::Handle<double> & handle_, double min_value_, double max_value_): handle(handle_), min_value(min_value_), max_value(max_value_){
}


bool DoubleSelection::passes(const uhh2::Event & event){
    double value = event.get(handle);
    return value <= max_value && value >= min_value;
}

