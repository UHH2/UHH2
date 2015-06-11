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
    for(const auto & pname : cfg.id_sources.getParameterNames()){
        id_tags.push_back(Electron::tagname2tag(pname));
        id_src_tokens.push_back(cfg.cc.consumes<edm::ValueMap<float>>(cfg.id_sources.getParameter<edm::InputTag>(pname)));
    }
}

NtupleWriterElectrons::~NtupleWriterElectrons(){}

void NtupleWriterElectrons::process(const edm::Event & event, uhh2::Event & uevent){
    edm::Handle< std::vector<pat::Electron> > ele_handle;
    event.getByToken(src_token, ele_handle);
    std::vector<Electron> eles;
    
    std::vector<edm::Handle<edm::ValueMap<float>>> electron_ids(id_src_tokens.size());
    for(size_t i_id=0; i_id<id_src_tokens.size(); ++i_id){
        event.getByToken(id_src_tokens[i_id], electron_ids[i_id]);
    }
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
        ele.set_supercluster_eta(pat_ele.superCluster()->eta());
        ele.set_supercluster_phi(pat_ele.superCluster()->phi());
        ele.set_dB(pat_ele.dB());
        const auto & pfiso = pat_ele.pfIsolationVariables();
        ele.set_neutralHadronIso(pfiso.sumNeutralHadronEt);
        ele.set_chargedHadronIso(pfiso.sumChargedHadronPt);
        ele.set_trackIso(pfiso.sumChargedParticlePt);
        ele.set_photonIso(pfiso.sumPhotonEt);
        ele.set_puChargedHadronIso(pfiso.sumPUPt);
        ele.set_gsfTrack_trackerExpectedHitsInner_numberOfLostHits(pat_ele.gsfTrack()->hitPattern().numberOfHits(reco::HitPattern::MISSING_INNER_HITS));
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
        
        const edm::Ptr<pat::Electron> eleptr(ele_handle, i);
        for(size_t i_id=0; i_id<id_tags.size(); ++i_id){
            ele.set_tag(id_tags[i_id], (*electron_ids[i_id])[eleptr]);
        }
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
    pv_token = cfg.cc.consumes<std::vector<reco::Vertex>>(cfg.pv_src);
}

NtupleWriterMuons::~NtupleWriterMuons(){}

void NtupleWriterMuons::process(const edm::Event & event, uhh2::Event & uevent){
   edm::Handle<std::vector<pat::Muon>> mu_handle;
   event.getByToken(src_token, mu_handle);
   
   edm::Handle<std::vector<reco::Vertex>> pv_handle;
   event.getByToken(pv_token, pv_handle);
   if(pv_handle->empty()){
       cout << "WARNING: no PVs found, not writing muons!" << endl;
       return;
   }
   const auto & PV = pv_handle->front();
   
   vector<Muon> mus;
   for (const pat::Muon & pat_mu : *mu_handle) {
     mus.emplace_back();
     Muon & mu = mus.back();
     mu.set_charge( pat_mu.charge());
     mu.set_pt( pat_mu.pt());
     mu.set_eta( pat_mu.eta());
     mu.set_phi( pat_mu.phi());
     mu.set_energy( pat_mu.energy());
     
     mu.set_dxy(pat_mu.muonBestTrack()->dxy(PV.position()));
     mu.set_dxy_error(pat_mu.muonBestTrack()->dxyError());
     mu.set_dz(pat_mu.muonBestTrack()->dz(PV.position()));
     mu.set_dz_error(pat_mu.muonBestTrack()->dzError());
     
     mu.set_bool(Muon::global, pat_mu.isGlobalMuon());
     mu.set_bool(Muon::pf, pat_mu.isPFMuon());
     mu.set_bool(Muon::tracker, pat_mu.isTrackerMuon());
     mu.set_bool(Muon::standalone, pat_mu.isStandAloneMuon());
     
     mu.set_bool(Muon::soft, pat_mu.isSoftMuon(PV));
     mu.set_bool(Muon::tight, pat_mu.isTightMuon(PV));
     mu.set_bool(Muon::highpt, pat_mu.isHighPtMuon(PV));
     
     mu.set_sumChargedHadronPt(pat_mu.pfIsolationR04().sumChargedHadronPt);
     mu.set_sumNeutralHadronEt(pat_mu.pfIsolationR04().sumNeutralHadronEt);
     mu.set_sumPhotonEt(pat_mu.pfIsolationR04().sumPhotonEt);
     mu.set_sumPUPt(pat_mu.pfIsolationR04().sumPUPt);
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
         
         // use the macro to avoid typos: using this macro assures that the enum name
         // used in the same as the string used for the pat tauID.
         #define FILL_TAU_BIT(tauidname) tau.set_bool(Tau:: tauidname, pat_tau.tauID(#tauidname) > 0.5)
        
         FILL_TAU_BIT(againstElectronLoose);         //  0
         FILL_TAU_BIT(againstElectronMedium);
         FILL_TAU_BIT(againstElectronTight);
         FILL_TAU_BIT(againstElectronVLooseMVA5);
         FILL_TAU_BIT(againstElectronLooseMVA5);
         FILL_TAU_BIT(againstElectronMediumMVA5);
         FILL_TAU_BIT(againstElectronTightMVA5);
         FILL_TAU_BIT(againstElectronVTightMVA5);
         FILL_TAU_BIT(againstMuonLoose);
         FILL_TAU_BIT(againstMuonMedium);
         FILL_TAU_BIT(againstMuonTight);            // 10
         FILL_TAU_BIT(againstMuonLoose3);
         FILL_TAU_BIT(againstMuonTight3);
         FILL_TAU_BIT(againstMuonLooseMVA);
         FILL_TAU_BIT(againstMuonMediumMVA);
         FILL_TAU_BIT(againstMuonTightMVA);
         FILL_TAU_BIT(decayModeFinding);
         FILL_TAU_BIT(byLooseCombinedIsolationDeltaBetaCorr3Hits);
         FILL_TAU_BIT(byMediumCombinedIsolationDeltaBetaCorr3Hits);
         FILL_TAU_BIT(byTightCombinedIsolationDeltaBetaCorr3Hits);
         FILL_TAU_BIT(byVLooseIsolationMVA3newDMwoLT);  // 20
         FILL_TAU_BIT(byLooseIsolationMVA3newDMwoLT);
         FILL_TAU_BIT(byMediumIsolationMVA3newDMwoLT);
         FILL_TAU_BIT(byTightIsolationMVA3newDMwoLT);
         FILL_TAU_BIT(byVTightIsolationMVA3newDMwoLT);
         FILL_TAU_BIT(byVVTightIsolationMVA3newDMwoLT);
         FILL_TAU_BIT(byVLooseIsolationMVA3newDMwLT);
         FILL_TAU_BIT(byLooseIsolationMVA3newDMwLT);
         FILL_TAU_BIT(byMediumIsolationMVA3newDMwLT);
         FILL_TAU_BIT(byTightIsolationMVA3newDMwLT);
         FILL_TAU_BIT(byVTightIsolationMVA3newDMwLT);  // 30
         FILL_TAU_BIT(byVVTightIsolationMVA3newDMwLT); // 31
         FILL_TAU_BIT(decayModeFindingNewDMs);
         
         #undef FILL_TAU_BIT
         #define FILL_TAU_FLOAT(name) tau.set_##name (pat_tau.tauID(#name))
         
         FILL_TAU_FLOAT(againstElectronMVA5raw);
         FILL_TAU_FLOAT(againstMuonMVAraw);
         FILL_TAU_FLOAT(byCombinedIsolationDeltaBetaCorrRaw3Hits);
         FILL_TAU_FLOAT(byIsolationMVA3newDMwoLTraw);
         FILL_TAU_FLOAT(byIsolationMVA3newDMwLTraw);
         FILL_TAU_FLOAT(chargedIsoPtSum);
         FILL_TAU_FLOAT(neutralIsoPtSum);
         FILL_TAU_FLOAT(puCorrPtSum);
         
         #undef FILL_TAU_FLOAT

         tau.set_decayMode(pat_tau.decayMode());

    }
    uevent.set(handle, move(taus));
    if(taus_handle){
       EventAccess_::set_unmanaged(uevent, *taus_handle, &uevent.get(handle));
    }
 }

