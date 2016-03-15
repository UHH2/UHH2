#include "UHH2/common/include/TauUncerts.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/Utils.h"
#include "UHH2/common/include/Utils.h"

using namespace std;
using namespace uhh2;


TauEnergySmearing::TauEnergySmearing(Context & ctx){

  auto s_TauEnergy = ctx.get("TauEnergyVariation");

  if(s_TauEnergy == "up") {i_TauEnergy = 1;}
  if(s_TauEnergy == "down") {i_TauEnergy = 2;}

}

bool TauEnergySmearing::process(Event & event){
  if(i_TauEnergy == 0) {return true;}
  double factor = 1.;
  if(i_TauEnergy==1) factor = 1.03;
  if(i_TauEnergy==2) factor = 0.97;
  for(unsigned int i=0; i<event.taus->size(); ++i){
    LorentzVector vec = event.taus->at(i).v4();    
    LorentzVector svec;
    svec.SetPxPyPzE(vec.px()*factor, vec.py()*factor, vec.pz()*factor, vec.e()*factor);
    event.taus->at(i).set_v4(svec);
  }
  return true;
}

TauEnergyResolutionShifter::TauEnergyResolutionShifter(Context & ctx){

  auto s_TauEnergyRes = ctx.get("TauEnergyResolutionVariation");

  if(s_TauEnergyRes == "up") {i_TauEnergyRes = 1;}
  if(s_TauEnergyRes == "down") {i_TauEnergyRes = 2;}

}

bool TauEnergyResolutionShifter::process(Event & event){
  if (event.isRealData) {return true;}
  double factor = 0.0;
  if (i_TauEnergyRes==0) {factor = 0.0;}
  else{
    if (i_TauEnergyRes==1) {factor = +0.10;}
    if (i_TauEnergyRes==2) {factor = -0.10;}
  }

  double dR = 1000;
  for(unsigned int i=0; i<event.taus->size(); ++i) {
    Tau tau = event.taus->at(i);
    double genpt = 0;
    double recopt = 0;
    for(auto genp : *event.genparticles){
      if(abs(genp.pdgId())==15){
	double tmp = deltaR(tau,genp);
	if(tmp<dR){
	  dR = tmp;
	  genpt = genp.pt();
	  recopt = tau.pt();
	}
      }
    }
    if(dR<0.4){
      {
	LorentzVector tau_v4 =  event.taus->at(i).v4();
	double ptscale = std::max(0.0, 1 + factor * (recopt - genpt) / recopt);
	tau_v4*=ptscale;
	event.taus->at(i).set_v4(tau_v4);
      }
      sort_by_pt(*event.taus);
    }
  }

  return true;
}
