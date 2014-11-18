#include "UHH2/common/include/PrintingModules.h"
#include "UHH2/core/include/Utils.h"

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
    TableOutput to({"id", "ind", "mo1", "mo2", "stat", "pt", "eta"});
    for(const auto & gp : *event.genparticles){
        to.add_row({ int2string(gp.pdgId()), int2string(gp.index()), int2string(gp.mother1()), int2string(gp.mother2()),
                   int2string(gp.status()), double2string(gp.pt(), 4), double2string(gp.eta(), 2)});
    }
    to.print(cout);
    return true;
}

UHH2_REGISTER_ANALYSIS_MODULE(GenParticlesPrinter)
