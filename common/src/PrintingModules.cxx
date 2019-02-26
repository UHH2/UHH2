#include "UHH2/common/include/PrintingModules.h"
#include "UHH2/core/include/Utils.h"

using namespace uhh2;
using namespace std;

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
    for(const Muon & mu : *event.muons){
        ++i;
        cout << " mu[" << i << "]: E=" << mu.energy() << "; pt=" << mu.pt() << ", eta=" << mu.eta() << "; phi=" << mu.phi() << "; reliso=" << mu.relIso()
             << "; global=" << mu.get_selector(Muon::Global) << "; pf=" << mu.get_selector(Muon::PF)
             << "; dxy=" << mu.dxy() << "; dz=" << mu.dz() << endl;
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
    const char * medium_id = "<N/A>";
//    if(ele.has_tag(Electron::eid_PHYS14_20x25_medium)){
//      if(ele.get_tag(Electron::eid_PHYS14_20x25_medium) > 0.5f){
//        medium_id = "pass";
//      }
//      else{
//        medium_id = "fail";
//      }
//    }
    out << "E=" << ele.energy() << "; pt=" << ele.pt() << ", eta=" << ele.eta() << "; phi=" << ele.phi() << "; reliso_db03=" << ele.relIsodb()
        << "; dEtaIn=" << ele.dEtaIn() << "; dPhiIn=" << ele.dPhiIn() << "; sigma_ieie=" << ele.sigmaIEtaIEta() << "; H/E=" << ele.HoverE()
        << "; d0=" << pv_d0 << "; dz=" << pv_dz << "; |1/E-1/p|=" << fabs_1oE_1op << "; n_misshits=" << ele.gsfTrack_trackerExpectedHitsInner_numberOfLostHits()
        << "; passes conv veto ? " << ele.passconversionveto() << "; medium id ? " << medium_id << endl;
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

bool GenJetPrinter::process(uhh2::Event & event) {
    if(!name.empty()){
        cout << "GenJetPrinter(" << name << "): ";
    }
    cout << "event has " << event.genjets->size() << " genjets" << endl;
    for(size_t i=0; i< event.genjets->size(); ++i){
        const auto & jet = (*event.genjets)[i];
        if(jet.pt() < ptmin) continue;
        cout << " jet[" << i << "]: pt=" << jet.pt() << "; eta=" << jet.eta() << "; phi=" << jet.phi() << endl;
    }
    return true;
}


bool TopJetPrinter::process(uhh2::Event & event) {
    if(!name.empty()){
        cout << "TopJetPrinter(" << name << "): ";
    }
    cout << "event has " << event.topjets->size() << " genjets" << endl;
    for(size_t i=0; i< event.topjets->size(); ++i){
        const auto & jet = (*event.topjets)[i];
        if(jet.pt() < ptmin) continue;
        cout << " jet[" << i << "]: pt=" << jet.pt() << "; eta=" << jet.eta() << "; phi=" << jet.phi() << endl;
    }
    return true;
}

bool GenTopJetPrinter::process(uhh2::Event & event) {
    if(!name.empty()){
        cout << "GenTopJetPrinter(" << name << "): ";
    }
    cout << "event has " << event.gentopjets->size() << " genjets" << endl;
    for(size_t i=0; i< event.gentopjets->size(); ++i){
        const auto & jet = (*event.gentopjets)[i];
        if(jet.pt() < ptmin) continue;
        cout << " jet[" << i << "]: pt=" << jet.pt() << "; eta=" << jet.eta() << "; phi=" << jet.phi() << endl;
    }
    return true;
}
