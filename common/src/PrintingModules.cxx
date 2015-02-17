#include "UHH2/common/include/PrintingModules.h"
#include "UHH2/core/include/Utils.h"

using namespace uhh2;
using namespace std;

namespace {
    
union fi_union {
    uint32_t i;
    float f;
};
    
// bit = 0 means least significan
int extract_bit(float value, int bit){
    fi_union u;
    u.f = value;
    bool isset = (u.i & (uint32_t(1) << bit));
    return isset ? 1 : 0;
}

}

MuonPrinter::MuonPrinter(const std::string & name_): name(name_){
}

bool MuonPrinter::process(Event & event){
    if(!name.empty()){
        cout << "MuonPrinter(" << name << "): ";
    }
    if(!event.muons){
        cout << "no muons read in" << endl;
        return true;
    }
    cout << " event has " << event.muons->size() << " muons" << endl;
    int i=-1;
    float pv0z = NAN;
    if(event.pvs && event.pvs->size() > 0){
        pv0z = event.pvs->at(0).z();
    }
    for(const Muon & mu : *event.muons){
        ++i;
        cout << " mu[" << i << "]: E=" << mu.energy() << "; pt=" << mu.pt() << ", eta=" << mu.eta() << "; phi=" << mu.phi() << "; reliso=" << mu.relIso()
             << "; global=" << mu.isGlobalMuon() << "; pf=" << mu.isPFMuon() << "; nchi2=" << (mu.globalTrack_chi2() / mu.globalTrack_ndof())
             << "; muon hits=" << mu.globalTrack_numberOfValidMuonHits() << "; matched stations=" << mu.numberOfMatchedStations()
             << "; db=" << mu.dB() << "; |dz|=" << fabs(mu.vertex_z() - pv0z) << "; npix=" << mu.innerTrack_numberOfValidPixelHits() << "; ntrackl=" << mu.innerTrack_trackerLayersWithMeasurement() << endl;
    }
    return true;
}



ElectronPrinter::ElectronPrinter(const std::string & name_): name(name_){
}

void ElectronPrinter::print(std::ostream & out, const Electron & ele, const Event & event){
    float pv_d0 = NAN, pv_dz = NAN;
    float fabs_1oE_1op = NAN;
    if(event.pvs && event.pvs->size() > 0){
        const auto & pv = event.pvs->at(0);
        pv_d0 = ele.gsfTrack_dxy_vertex(pv.x(), pv.y());
        pv_dz = ele.gsfTrack_dz_vertex(pv.x(), pv.y(), pv.z());
    }
    if(ele.EcalEnergy() > 0.0){
        fabs_1oE_1op = std::abs(1.0f/ele.EcalEnergy() - ele.EoverPIn()/ele.EcalEnergy());
    }
    float id = ele.mvaTrigV0();
    out << "E=" << ele.energy() << "; pt=" << ele.pt() << ", eta=" << ele.eta() << "; phi=" << ele.phi() << "; reliso_db03=" << ele.relIsodb()
        << "; id=" << id << "; medium=" << extract_bit(id, 2)
        << "; dEtaIn=" << ele.dEtaIn() << "; dPhiIn=" << ele.dPhiIn() << "; sigma_ieie=" << ele.sigmaIEtaIEta() << "; H/E=" << ele.HoverE()
        << "; d0=" << pv_d0 << "; dz=" << pv_dz << "; |1/E-1/p|=" << fabs_1oE_1op << "; n_misshits=" << ele.gsfTrack_trackerExpectedHitsInner_numberOfLostHits()
        << "; passes conv veto ? " << ele.passconversionveto() << endl;
}

bool ElectronPrinter::process(Event & event){
    if(!name.empty()){
        cout << "ElectronPrinter(" << name << "): ";
    }
    if(!event.electrons){
        cout << "no electrons read in" << endl;
        return true;
    }
    cout << " event has " << event.electrons->size() << " electrons" << endl;
    int i=-1;
    for(const auto & ele : *event.electrons){
        ++i;
        cout << " ele[" << i << "]: ";
        print(cout, ele, event);
    }
    return true;
}



bool GenParticlesPrinter::process(Event & event){
    if(!event.genparticles){
        cout << "no genparticles read in" << endl;
        return true;
    }
    cout << "N_gp = " << event.genparticles->size() << endl;
    TableOutput to({"id", "ind", "d1", "d2", "mo1", "mo2", "stat", "pt", "eta"});
    for(const auto & gp : *event.genparticles){
        to.add_row({ to_string(gp.pdgId()), to_string(gp.index()), to_string(gp.daughter1()), to_string(gp.daughter2()),
                   to_string(gp.mother1()), to_string(gp.mother2()),
                   to_string(gp.status()), double2string(gp.pt(), 4), double2string(gp.eta(), 2)});
    }
    to.print(cout);
    return true;
}

UHH2_REGISTER_ANALYSIS_MODULE(GenParticlesPrinter)



bool JetPrinter::process(uhh2::Event & event) {
    if(!name.empty()){
        cout << "JetPrinter(" << name << "): ";
    }
    cout << "event has " << event.jets->size() << " jets" << endl;
    for(size_t i=0; i< event.jets->size(); ++i){
        const auto & jet = (*event.jets)[i];
        if(jet.pt() < ptmin) continue;
        cout << " jet[" << i << "]: pt=" << jet.pt() << "; eta=" << jet.eta() << "; phi=" << jet.phi() << " raw pt=" << (jet.pt() * jet.JEC_factor_raw()) << "; muE="
             << jet.JEC_factor_raw() * jet.muonEnergyFraction() * jet.energy() << endl;
    }
    return true;
}
