#include "UHH2/core/plugins/NtupleWriterLeptons.h"
#include "UHH2/core/include/AnalysisModule.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Tau.h"

using namespace uhh2;
using namespace std;


NtupleWriterElectrons::NtupleWriterElectrons(Config & cfg, bool set_electrons_member){
    handle = cfg.ctx.declare_event_output<vector<Electron>>(cfg.dest_branchname, cfg.dest);
    if(set_electrons_member){
        electrons_handle = cfg.ctx.get_handle<vector<Electron>>("electrons");
    }
    src_token = cfg.cc.consumes<std::vector<pat::Electron>>(cfg.src);
}

NtupleWriterElectrons::~NtupleWriterElectrons(){}

void NtupleWriterElectrons::process(const edm::Event & event, uhh2::Event & uevent){
    edm::Handle< std::vector<pat::Electron> > ele_handle;
    event.getByToken(src_token, ele_handle);
    std::vector<Electron> eles;
    const size_t n_ele = ele_handle->size();
    for (size_t i=0; i<n_ele; ++i){
        const auto & pat_ele = (*ele_handle)[i];
        eles.emplace_back();
         Electron & ele = eles.back();
         ele.set_charge( pat_ele.charge());
         ele.set_pt( pat_ele.pt());
         ele.set_eta( pat_ele.eta());
         ele.set_phi( pat_ele.phi());
         ele.set_energy( pat_ele.energy());
         ele.set_vertex_x(pat_ele.vertex().x());
         ele.set_vertex_y(pat_ele.vertex().y());
         ele.set_vertex_z(pat_ele.vertex().z());
         ele.set_supercluster_eta(pat_ele.superCluster()->eta());
         ele.set_supercluster_phi(pat_ele.superCluster()->phi());
         ele.set_dB(pat_ele.dB());
         const auto & pfiso = pat_ele.pfIsolationVariables();
         ele.set_neutralHadronIso(pfiso.sumNeutralHadronEt);
         ele.set_chargedHadronIso(pfiso.sumChargedHadronPt);
         ele.set_trackIso(pfiso.sumChargedParticlePt);
         ele.set_photonIso(pfiso.sumPhotonEt);
         ele.set_puChargedHadronIso(pfiso.sumPUPt);
         ele.set_gsfTrack_trackerExpectedHitsInner_numberOfLostHits(pat_ele.gsfTrack()->hitPattern().numberOfLostTrackerHits(reco::HitPattern::MISSING_INNER_HITS));
         ele.set_gsfTrack_px( pat_ele.gsfTrack()->px());
         ele.set_gsfTrack_py( pat_ele.gsfTrack()->py());
         ele.set_gsfTrack_pz( pat_ele.gsfTrack()->pz());
         ele.set_gsfTrack_vx( pat_ele.gsfTrack()->vx());
         ele.set_gsfTrack_vy( pat_ele.gsfTrack()->vy());
         ele.set_gsfTrack_vz( pat_ele.gsfTrack()->vz());
         ele.set_passconversionveto(pat_ele.passConversionVeto());
         ele.set_dEtaIn(pat_ele.deltaEtaSuperClusterTrackAtVtx());
         ele.set_dPhiIn(pat_ele.deltaPhiSuperClusterTrackAtVtx());
         ele.set_sigmaIEtaIEta(pat_ele.full5x5_sigmaIetaIeta());
         ele.set_HoverE(pat_ele.hcalOverEcal());
         ele.set_fbrem(pat_ele.fbrem());
         ele.set_EoverPIn(pat_ele.eSuperClusterOverP());
         ele.set_EcalEnergy(pat_ele.ecalEnergy());
         //not in miniaod, maybe later ...
         //ele.set_mvaTrigV0(pat_ele.electronID("mvaTrigV0"));
         //ele.set_mvaNonTrigV0(pat_ele.electronID("mvaNonTrigV0"));
         ele.set_AEff(-1.0);
    }
    uevent.set(handle, move(eles));
    if(electrons_handle){
        EventAccess_::set_unmanaged(uevent, *electrons_handle, &uevent.get(handle));
    }
}


NtupleWriterMuons::NtupleWriterMuons(Config & cfg, bool set_muons_member){
    handle = cfg.ctx.declare_event_output<vector<Muon>>(cfg.dest_branchname, cfg.dest);
    if(set_muons_member){
        muons_handle = cfg.ctx.get_handle<vector<Muon>>("muons");
    }
    src_token = cfg.cc.consumes<std::vector<pat::Muon>>(cfg.src);
}

NtupleWriterMuons::~NtupleWriterMuons(){}

void NtupleWriterMuons::process(const edm::Event & event, uhh2::Event & uevent){
   edm::Handle<std::vector<pat::Muon>> mu_handle;
   event.getByToken(src_token, mu_handle);
   vector<Muon> mus;
   for (const pat::Muon & pat_mu : *mu_handle) {
     mus.emplace_back();
     Muon & mu = mus.back();
     mu.set_charge( pat_mu.charge());
     mu.set_pt( pat_mu.pt());
     mu.set_eta( pat_mu.eta());
     mu.set_phi( pat_mu.phi());
     mu.set_energy( pat_mu.energy());
     mu.set_vertex_x ( pat_mu.vertex().x());
     mu.set_vertex_y ( pat_mu.vertex().y());
     mu.set_vertex_z ( pat_mu.vertex().z());
     mu.set_dB ( pat_mu.dB());
     //mu.particleIso ( pat_mu.particleIso());
     mu.set_neutralHadronIso ( pat_mu.neutralHadronIso());
     mu.set_chargedHadronIso ( pat_mu.chargedHadronIso());
     mu.set_trackIso ( pat_mu.trackIso());
     mu.set_photonIso ( pat_mu.photonIso());
     mu.set_puChargedHadronIso ( pat_mu.puChargedHadronIso());
     mu.set_isGlobalMuon ( pat_mu.isGlobalMuon());
     mu.set_isPFMuon ( pat_mu.isPFMuon());
     mu.set_isStandAloneMuon ( pat_mu.isStandAloneMuon());
     mu.set_isTrackerMuon ( pat_mu.isTrackerMuon());
     mu.set_numberOfMatchedStations ( pat_mu.numberOfMatchedStations());
     reco::TrackRef globalTrack = pat_mu.globalTrack();
     if(!globalTrack.isNull()){
       mu.set_globalTrack_chi2 ( globalTrack->chi2());
       mu.set_globalTrack_ndof ( globalTrack->ndof());
       mu.set_globalTrack_d0 ( globalTrack->d0());
       mu.set_globalTrack_d0Error ( globalTrack->d0Error());
       mu.set_globalTrack_numberOfValidHits ( globalTrack->numberOfValidHits());
       mu.set_globalTrack_numberOfLostHits ( globalTrack->numberOfLostHits());
       mu.set_globalTrack_numberOfValidMuonHits(globalTrack->hitPattern().numberOfValidMuonHits() );
     } // otherwise; keep them at 0
     reco::TrackRef innerTrack = pat_mu.innerTrack();
     if(!innerTrack.isNull()){
       mu.set_innerTrack_chi2 ( innerTrack->chi2());
       mu.set_innerTrack_ndof ( innerTrack->ndof());
       mu.set_innerTrack_d0 ( innerTrack->d0());
       mu.set_innerTrack_d0Error ( innerTrack->d0Error());
       mu.set_innerTrack_numberOfValidHits ( innerTrack->numberOfValidHits());
       mu.set_innerTrack_numberOfLostHits ( innerTrack->numberOfLostHits());
       mu.set_innerTrack_trackerLayersWithMeasurement ( innerTrack->hitPattern().trackerLayersWithMeasurement());
       mu.set_innerTrack_numberOfValidPixelHits ( innerTrack->hitPattern().numberOfValidPixelHits());
     } // otherwise; keep them at 0
     reco::TrackRef outerTrack = pat_mu.outerTrack();
     if(!outerTrack.isNull()){
       mu.set_outerTrack_chi2 ( outerTrack->chi2());
       mu.set_outerTrack_ndof ( outerTrack->ndof());
       mu.set_outerTrack_d0 ( outerTrack->d0());
       mu.set_outerTrack_d0Error ( outerTrack->d0Error());
       mu.set_outerTrack_numberOfValidHits ( outerTrack->numberOfValidHits());
       mu.set_outerTrack_numberOfLostHits ( outerTrack->numberOfLostHits());
     } // otherwise; keep them at 0
   }
   uevent.set(handle, move(mus));
   if(muons_handle){
       EventAccess_::set_unmanaged(uevent, *muons_handle, &uevent.get(handle));
   }
}



NtupleWriterTaus::NtupleWriterTaus(Config & cfg, bool set_taus_member){
    handle = cfg.ctx.declare_event_output<vector<Tau>>(cfg.dest_branchname, cfg.dest);
    if(set_taus_member){
        taus_handle = cfg.ctx.get_handle<vector<Tau>>("taus");
    }
    src_token = cfg.cc.consumes<std::vector<pat::Tau>>(cfg.src);
    ptmin = cfg.ptmin;
    etamax = cfg.etamax;
}

NtupleWriterTaus::~NtupleWriterTaus(){}

void NtupleWriterTaus::process(const edm::Event & event, uhh2::Event & uevent){
    edm::Handle< std::vector<pat::Tau> > tau_handle;
    event.getByToken(src_token, tau_handle);
    std::vector<Tau> taus;
    for (const auto & pat_tau : *tau_handle) {
         if(pat_tau.pt() < ptmin) continue;
         if(fabs(pat_tau.eta()) > etamax) continue;
         taus.emplace_back();
         Tau & tau = taus.back();

         tau.set_charge( pat_tau.charge());
         tau.set_pt( pat_tau.pt());
         tau.set_eta( pat_tau.eta());
         tau.set_phi( pat_tau.phi());
         tau.set_energy( pat_tau.energy());
         tau.set_decayModeFinding ( pat_tau.tauID("decayModeFinding")>0.5);
         tau.set_decayModeFindingNewDMs ( pat_tau.tauID("decayModeFindingNewDMs")>0.5);

         tau.set_againstElectronLoose ( pat_tau.tauID("againstElectronLoose"));
         tau.set_againstElectronLooseMVA5 ( pat_tau.tauID("againstElectronLooseMVA5"));
         tau.set_againstElectronMVA5category ( pat_tau.tauID("againstElectronMVA5category"));
         tau.set_againstElectronMVA5raw ( pat_tau.tauID("againstElectronMVA5raw"));
         tau.set_againstElectronMedium ( pat_tau.tauID("againstElectronMedium"));
         tau.set_againstElectronMediumMVA5 ( pat_tau.tauID("againstElectronMediumMVA5"));
         tau.set_againstElectronTight ( pat_tau.tauID("againstElectronTight"));
         tau.set_againstElectronTightMVA5 ( pat_tau.tauID("againstElectronTightMVA5"));
         tau.set_againstElectronVLooseMVA5 ( pat_tau.tauID("againstElectronVLooseMVA5"));
         tau.set_againstElectronVTightMVA5 ( pat_tau.tauID("againstElectronVTightMVA5"));
         tau.set_againstMuonLoose ( pat_tau.tauID("againstMuonLoose"));
         tau.set_againstMuonLoose2 ( pat_tau.tauID("againstMuonLoose2"));
         tau.set_againstMuonLoose3 ( pat_tau.tauID("againstMuonLoose3"));
         tau.set_againstMuonLooseMVA ( pat_tau.tauID("againstMuonLooseMVA"));
         tau.set_againstMuonMVAraw ( pat_tau.tauID("againstMuonMVAraw"));
         tau.set_againstMuonMedium ( pat_tau.tauID("againstMuonMedium"));
         tau.set_againstMuonMedium2 ( pat_tau.tauID("againstMuonMedium2"));
         tau.set_againstMuonMediumMVA ( pat_tau.tauID("againstMuonMediumMVA"));
         tau.set_againstMuonTight ( pat_tau.tauID("againstMuonTight"));
         tau.set_againstMuonTight2 ( pat_tau.tauID("againstMuonTight2"));
         tau.set_againstMuonTight3 ( pat_tau.tauID("againstMuonTight3"));
         tau.set_againstMuonTightMVA ( pat_tau.tauID("againstMuonTightMVA"));
         tau.set_byCombinedIsolationDeltaBetaCorrRaw3Hits ( pat_tau.tauID("byCombinedIsolationDeltaBetaCorrRaw3Hits"));
         tau.set_byIsolationMVA3newDMwLTraw ( pat_tau.tauID("byIsolationMVA3newDMwLTraw"));
         tau.set_byIsolationMVA3newDMwoLTraw ( pat_tau.tauID("byIsolationMVA3newDMwoLTraw"));
         tau.set_byIsolationMVA3oldDMwLTraw ( pat_tau.tauID("byIsolationMVA3oldDMwLTraw"));
         tau.set_byIsolationMVA3oldDMwoLTraw ( pat_tau.tauID("byIsolationMVA3oldDMwoLTraw"));
         tau.set_byLooseCombinedIsolationDeltaBetaCorr3Hits ( pat_tau.tauID("byLooseCombinedIsolationDeltaBetaCorr3Hits"));
         tau.set_byLooseIsolationMVA3newDMwLT ( pat_tau.tauID("byLooseIsolationMVA3newDMwLT"));
         tau.set_byLooseIsolationMVA3newDMwoLT ( pat_tau.tauID("byLooseIsolationMVA3newDMwoLT"));
         tau.set_byLooseIsolationMVA3oldDMwLT ( pat_tau.tauID("byLooseIsolationMVA3oldDMwLT"));
         tau.set_byLooseIsolationMVA3oldDMwoLT ( pat_tau.tauID("byLooseIsolationMVA3oldDMwoLT"));
         tau.set_byMediumCombinedIsolationDeltaBetaCorr3Hits ( pat_tau.tauID("byMediumCombinedIsolationDeltaBetaCorr3Hits"));
         tau.set_byMediumIsolationMVA3newDMwLT ( pat_tau.tauID("byMediumIsolationMVA3newDMwLT"));
         tau.set_byMediumIsolationMVA3newDMwoLT ( pat_tau.tauID("byMediumIsolationMVA3newDMwoLT"));
         tau.set_byMediumIsolationMVA3oldDMwLT ( pat_tau.tauID("byMediumIsolationMVA3oldDMwLT"));
         tau.set_byMediumIsolationMVA3oldDMwoLT ( pat_tau.tauID("byMediumIsolationMVA3oldDMwoLT"));
         tau.set_byTightCombinedIsolationDeltaBetaCorr3Hits ( pat_tau.tauID("byTightCombinedIsolationDeltaBetaCorr3Hits"));
         tau.set_byTightIsolationMVA3newDMwLT ( pat_tau.tauID("byTightIsolationMVA3newDMwLT"));
         tau.set_byTightIsolationMVA3newDMwoLT ( pat_tau.tauID("byTightIsolationMVA3newDMwoLT"));
         tau.set_byTightIsolationMVA3oldDMwLT ( pat_tau.tauID("byTightIsolationMVA3oldDMwLT"));
         tau.set_byTightIsolationMVA3oldDMwoLT ( pat_tau.tauID("byTightIsolationMVA3oldDMwoLT"));
         tau.set_byVLooseIsolationMVA3newDMwLT ( pat_tau.tauID("byVLooseIsolationMVA3newDMwLT"));
         tau.set_byVLooseIsolationMVA3newDMwoLT ( pat_tau.tauID("byVLooseIsolationMVA3newDMwoLT"));
         tau.set_byVLooseIsolationMVA3oldDMwLT ( pat_tau.tauID("byVLooseIsolationMVA3oldDMwLT"));
         tau.set_byVLooseIsolationMVA3oldDMwoLT ( pat_tau.tauID("byVLooseIsolationMVA3oldDMwoLT"));
         tau.set_byVTightIsolationMVA3newDMwLT ( pat_tau.tauID("byVTightIsolationMVA3newDMwLT"));
         tau.set_byVTightIsolationMVA3newDMwoLT ( pat_tau.tauID("byVTightIsolationMVA3newDMwoLT"));
         tau.set_byVTightIsolationMVA3oldDMwLT ( pat_tau.tauID("byVTightIsolationMVA3oldDMwLT"));
         tau.set_byVTightIsolationMVA3oldDMwoLT ( pat_tau.tauID("byVTightIsolationMVA3oldDMwoLT"));
         tau.set_byVVTightIsolationMVA3newDMwLT ( pat_tau.tauID("byVVTightIsolationMVA3newDMwLT"));
         tau.set_byVVTightIsolationMVA3newDMwoLT ( pat_tau.tauID("byVVTightIsolationMVA3newDMwoLT"));
         tau.set_byVVTightIsolationMVA3oldDMwLT ( pat_tau.tauID("byVVTightIsolationMVA3oldDMwLT"));
         tau.set_byVVTightIsolationMVA3oldDMwoLT ( pat_tau.tauID("byVVTightIsolationMVA3oldDMwoLT"));

         tau.set_decayMode( pat_tau.decayMode() );
    }
    uevent.set(handle, move(taus));
    if(taus_handle){
       EventAccess_::set_unmanaged(uevent, *taus_handle, &uevent.get(handle));
    }
 }

