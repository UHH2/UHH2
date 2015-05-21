#include "UHH2/common/include/TauIds.h"

using namespace uhh2;

bool TauIDMedium::operator()(const Tau & tau, const Event & event) const {
    if(!tau.get_bool(Tau::decayModeFinding)) return false;
    if(!tau.get_bool(Tau::againstElectronTightMVA5)) return false;
    if(!tau.get_bool(Tau::againstMuonTight3)) return false;
    if(!tau.get_bool(Tau::byMediumCombinedIsolationDeltaBetaCorr3Hits)) return false;
    if(event.muons){
        for(auto & muon : *event.muons) {
            if (deltaR(muon,tau) < 0.4) return false;
        }
    }
    return true;
}

bool TauIDDecayModeFinding::operator()(const Tau & tau, const Event &) const {
    if(!tau.get_bool(Tau::decayModeFinding)) return false;
    return true;
}
