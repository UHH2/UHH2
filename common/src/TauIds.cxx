#include "UHH2/common/include/TauIds.h"

using namespace uhh2;


bool TauIDMedium::operator()(const Tau & tau, const Event & event) const {
  if(!tau.decayModeFinding()) return false;
  if(!tau.againstElectronTightMVA5()) return false;
  if(!tau.againstMuonTight2()) return false;
  if(!tau.byMediumCombinedIsolationDeltaBetaCorr3Hits()) return false;
  double deltaRmin = 100;
  if(event.muons){
    for(unsigned int k=0; k<event.muons->size(); ++k)
      {
	Muon muon = event.muons->at(k);
	double deltaR = uhh2::deltaR(muon,tau);
	if (deltaR < deltaRmin) deltaRmin = deltaR;
      }
    if (deltaRmin < 0.5)
      {
	return false;
      }
  }
  else{
    return true;
  }
  return true;
}

