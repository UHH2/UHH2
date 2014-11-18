#include "UHH2/common/include/PrintingModules.h"

using namespace uhh2;
using namespace std;

bool MuonPrinter::process(Event & event){
    if(!event.muons){
        cout << "no muons read in" << endl;
        return true;
    }
    cout << "N_mu = " << event.muons->size() << endl;
    int i=-1;
    float pv0z = NAN;
    if(event.pvs && event.pvs->size() > 0){
        pv0z = event.pvs->at(0).z();
    }
    for(const Muon & mu : *event.muons){
        ++i;
        cout << "mu[" << i << "]: pt=" << mu.pt() << ", eta=" << mu.eta() << "; reliso=" << mu.relIso()
             << "; global=" << mu.isGlobalMuon() << "; pf=" << mu.isPFMuon() << "; nchi2=" << (mu.globalTrack_chi2() / mu.globalTrack_ndof())
             << "; muon hits=" << mu.globalTrack_numberOfValidMuonHits() << "; matched stations=" << mu.numberOfMatchedStations()
             << "; db=" << mu.dB() << "; |dz|=" << fabs(mu.vertex_z() - pv0z) << "; npix=" << mu.innerTrack_numberOfValidPixelHits() << "; ntrackl=" << mu.innerTrack_trackerLayersWithMeasurement() << endl;
    }
    return true;
}


bool GenParticlesPrinter::process(Event & event){
    if(!event.genparticles){
        cout << "no genparticles read in" << endl;
        return true;
    }
    cout << "N_gp = " << event.genparticles->size() << endl;
    int i=0;
    for(const auto & gp : *event.genparticles){
        ++i;
        cout << "gp[" << i << "]: id=" << gp.pdgId() << " pt=" << gp.pt() << ", eta=" << gp.eta() << "; index=" << gp.index()
             << "; status=" << gp.status() << "; mo1=" << gp.mother1() << "mo2 = " << gp.mother2() << endl;
    }
    return true;
}

UHH2_REGISTER_ANALYSIS_MODULE(GenParticlesPrinter)
