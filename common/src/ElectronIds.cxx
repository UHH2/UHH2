#include "UHH2/common/include/ElectronIds.h"

using namespace uhh2;

namespace {

// the triggering MVA id, recommended for CMSSW 53X, see:
// https://twiki.cern.ch/twiki/bin/viewauth/CMS/MultivariateElectronIdentification#Triggering_MVA
bool eleid_trigmva53(const Electron & ele, const Event & event, double maxreliso = 0.15){
    if(!ele.passconversionveto()) return false;
    if(ele.gsfTrack_trackerExpectedHitsInner_numberOfLostHits() > 0) return false;
    if(ele.relIsorho(event.rho) > maxreliso) return false;
    double eeta = fabs(ele.supercluster_eta());
    if(ele.pt() >= 20.0) {
        if(eeta <= 0.8) {
            return ele.mvaTrigV0() > 0.94;
        } else if(eeta <= 1.479) {
            return ele.mvaTrigV0() > 0.85;
        } else if(eeta <= 2.5) {
            return ele.mvaTrigV0() > 0.92;
        }
        
    } else if(ele.pt() > 10.0) {
        if(eeta <= 0.8) {
            return ele.mvaTrigV0() > 0.00;
        } else if(eeta <= 1.479) {
            return ele.mvaTrigV0() > 0.10;
        } else if(eeta <= 2.5) {
            return ele.mvaTrigV0() > 0.62;
        }
    }
    return false;
}

}

ElectronID_TrigMVA53::ElectronID_TrigMVA53(double ptmin_, double etamax_, double maxreliso_): ptmin(ptmin_), etamax(etamax_), maxreliso(maxreliso_){}

bool ElectronID_TrigMVA53::operator()(const Electron & electron, const Event & event) const{
    if(electron.pt() < ptmin || fabs(electron.eta()) > etamax) return false;
    return eleid_trigmva53(electron, event, maxreliso);
}
    
