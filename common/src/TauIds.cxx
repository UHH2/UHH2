#include "UHH2/common/include/TauIds.h"

using namespace uhh2;

bool TauIDMedium::operator()(const Tau & tau, const Event & event) const {
    if(!tau.decayModeFinding()) return false;
    if(!tau.againstElectronTightMVA5()) return false;
    if(!tau.againstMuonTight2()) return false;
    if(!tau.byMediumCombinedIsolationDeltaBetaCorr3Hits()) return false;
    if(event.muons){
        for(auto & muon : *event.muons) {
            if (deltaR(muon,tau) < 0.5) return false;
        }
    }
    return true;
}
