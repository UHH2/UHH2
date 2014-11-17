#include "UHH2/common/include/NSelections.h"
#include "UHH2/core/include/Event.h"

using namespace uhh2;

    
NMuonSelection::NMuonSelection(int nmin_, int nmax_): nmin(nmin_), nmax(nmax_){
}

bool NMuonSelection::passes(const Event & event){
    int nmuons = event.muons->size();
    return nmuons >= nmin && (nmax < 0 || nmuons <= nmax);
}


NElectronSelection::NElectronSelection(int nmin_, int nmax_): nmin(nmin_), nmax(nmax_){
}

bool NElectronSelection::passes(const Event & event){
    int ne = event.electrons->size();
    return ne >= nmin && (nmax < 0 || ne <= nmax);
}

NJetSelection::NJetSelection(int nmin_, int nmax_): nmin(nmin_), nmax(nmax_){}

bool NJetSelection::passes(const Event & event){
    int nj = event.jets->size();
    return nj >= nmin && (nmax < 0 || nj <= nmax);
}
