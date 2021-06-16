#include <algorithm>

#include "UHH2/core/plugins/NtupleWriterLeptons.h"
#include "UHH2/core/include/AnalysisModule.h"

#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Tau.h"

#include "DataFormats/MuonReco/interface/MuonSelectors.h"

using namespace uhh2;
using namespace std;

NtupleWriterElectrons::NtupleWriterElectrons(Config & cfg, bool set_electrons_member, const bool save_source_cands): save_source_candidates_(save_source_cands){
  handle = cfg.ctx.declare_event_output<vector<Electron>>(cfg.dest_branchname, cfg.dest);
  if(set_electrons_member) electrons_handle = cfg.ctx.get_handle<vector<Electron>>("electrons");
  src_token = cfg.cc.consumes<std::vector<pat::Electron>>(cfg.src);
  pv_token = cfg.cc.consumes<std::vector<reco::Vertex>>(cfg.pv_src);
  IDtag_keys = cfg.id_keys;
}

NtupleWriterElectrons::~NtupleWriterElectrons(){}

void NtupleWriterElectrons::process(const edm::Event & event, uhh2::Event & uevent, const edm::EventSetup& iSetup){
    edm::Handle< std::vector<pat::Electron> > ele_handle;
    event.getByToken(src_token, ele_handle);

    edm::Handle<std::vector<reco::Vertex>> pv_handle;
   event.getByToken(pv_token, pv_handle);
   if(pv_handle->empty()){
       edm::LogWarning("NtupleWriterElectrons") << "No PVs found, not writing electrons!";
       return;
   }
   const auto & PV = pv_handle->front();

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
        //	cout<<"pat_ele.pt() = "<<pat_ele.pt()<<endl;
        ele.set_ptError( pat_ele.gsfTrack()->ptError());
        ele.set_etaError( pat_ele.gsfTrack()->etaError());
        ele.set_phiError( pat_ele.gsfTrack()->phiError());
        //	ele.set_energyError( pat_ele.energyError());
        ele.set_supercluster_eta( pat_ele.superCluster()->eta() );
        ele.set_supercluster_phi( pat_ele.superCluster()->phi() );
        ele.set_dB(pat_ele.dB());
        const auto & pfiso = pat_ele.pfIsolationVariables();
        ele.set_neutralHadronIso(pfiso.sumNeutralHadronEt);
        ele.set_chargedHadronIso(pfiso.sumChargedHadronPt);
        ele.set_trackIso(pfiso.sumChargedParticlePt);
        ele.set_photonIso(pfiso.sumPhotonEt);
        ele.set_puChargedHadronIso(pfiso.sumPUPt);
        ele.set_gsfTrack_trackerExpectedHitsInner_numberOfLostHits(pat_ele.gsfTrack()->hitPattern().numberOfAllHits(reco::HitPattern::MISSING_INNER_HITS));
        ele.set_gsfTrack_px(pat_ele.gsfTrack()->px());
        ele.set_gsfTrack_py(pat_ele.gsfTrack()->py());
        ele.set_gsfTrack_pz(pat_ele.gsfTrack()->pz());
        ele.set_gsfTrack_vx(pat_ele.gsfTrack()->vx());
        ele.set_gsfTrack_vy(pat_ele.gsfTrack()->vy());
        ele.set_gsfTrack_vz(pat_ele.gsfTrack()->vz());
        ele.set_passconversionveto(pat_ele.passConversionVeto());
        ele.set_dEtaIn(pat_ele.deltaEtaSuperClusterTrackAtVtx());
        ele.set_dPhiIn(pat_ele.deltaPhiSuperClusterTrackAtVtx());
        ele.set_sigmaIEtaIEta(pat_ele.full5x5_sigmaIetaIeta());
        ele.set_HoverE(pat_ele.hadronicOverEm());
        ele.set_fbrem(std::max(pat_ele.fbrem(), -100.f)); // this is track frbem. default is -1e-30, making it impossible to validate on plots. -100 should be low enough to see detail around 0
        ele.set_EoverPIn(pat_ele.eSuperClusterOverP());
        ele.set_EcalEnergy(pat_ele.ecalEnergy());
        ele.set_hcalOverEcal    (pat_ele.hcalOverEcal());
        ele.set_ecalPFClusterIso(pat_ele.ecalPFClusterIso());
        ele.set_hcalPFClusterIso(pat_ele.hcalPFClusterIso());
        ele.set_dr03TkSumPt     (pat_ele.dr03TkSumPt());

        ele.set_mvaGeneralPurpose(pat_ele.hasUserFloat("mvaGeneralPurpose") ? pat_ele.userFloat("mvaGeneralPurpose") : -999.);
        ele.set_mvaHZZ(pat_ele.hasUserFloat("mvaHZZ") ? pat_ele.userFloat("mvaHZZ") : -999.);

        ele.set_mvaIso(pat_ele.hasUserFloat("ElectronMVAEstimatorIso") ? pat_ele.userFloat("ElectronMVAEstimatorIso") : -999.);
        ele.set_mvaNoIso(pat_ele.hasUserFloat("ElectronMVAEstimatorNoIso") ? pat_ele.userFloat("ElectronMVAEstimatorNoIso") : -999.);

        ele.set_effArea(pat_ele.hasUserFloat("EffArea") ? pat_ele.userFloat("EffArea") : -999.);

        ele.set_pfMINIIso_CH      (pat_ele.hasUserFloat("elPFMiniIsoValueCHSTAND") ? pat_ele.userFloat("elPFMiniIsoValueCHSTAND") : -999.);
        ele.set_pfMINIIso_NH      (pat_ele.hasUserFloat("elPFMiniIsoValueNHSTAND") ? pat_ele.userFloat("elPFMiniIsoValueNHSTAND") : -999.);
        ele.set_pfMINIIso_Ph      (pat_ele.hasUserFloat("elPFMiniIsoValuePhSTAND") ? pat_ele.userFloat("elPFMiniIsoValuePhSTAND") : -999.);
        ele.set_pfMINIIso_PU      (pat_ele.hasUserFloat("elPFMiniIsoValuePUSTAND") ? pat_ele.userFloat("elPFMiniIsoValuePUSTAND") : -999.);
        ele.set_pfMINIIso_NH_pfwgt(pat_ele.hasUserFloat("elPFMiniIsoValueNHPFWGT") ? pat_ele.userFloat("elPFMiniIsoValueNHPFWGT") : -999.);
        ele.set_pfMINIIso_Ph_pfwgt(pat_ele.hasUserFloat("elPFMiniIsoValuePhPFWGT") ? pat_ele.userFloat("elPFMiniIsoValuePhPFWGT") : -999.);

        ele.set_Nclusters(pat_ele.superCluster()->clusters().size());
        ele.set_Class(pat_ele.classification());

        ele.set_isEcalDriven(pat_ele.ecalDriven());
        ele.set_full5x5_e1x5(pat_ele.full5x5_e1x5());
        ele.set_full5x5_e2x5Max(pat_ele.full5x5_e2x5Max());
        ele.set_full5x5_e5x5(pat_ele.full5x5_e5x5());
        ele.set_dEtaInSeed(pat_ele.deltaEtaSeedClusterTrackAtVtx());

        ele.set_dxy(pat_ele.gsfTrack()->dxy(PV.position()));// correct for vertex postion

        for(const auto& tag_str : IDtag_keys){
          if(!pat_ele.isElectronIDAvailable(tag_str)) throw cms::Exception("Missing Electron ID", "ElectronID not found: "+tag_str);
          ele.set_tag(Electron::tagname2tag(tag_str), float(pat_ele.electronID(tag_str)));
        }

        /* source candidates */
        if(save_source_candidates_){

          for(unsigned int s=0; s<pat_ele.numberOfSourceCandidatePtrs(); ++s){

            if(!pat_ele.sourceCandidatePtr(s).isAvailable()) continue;

            source_candidate sc;
            sc.key = pat_ele.sourceCandidatePtr(s).key();
            sc.px  = pat_ele.sourceCandidatePtr(s)->px();
            sc.py  = pat_ele.sourceCandidatePtr(s)->py();
            sc.pz  = pat_ele.sourceCandidatePtr(s)->pz();
            sc.E   = pat_ele.sourceCandidatePtr(s)->energy();

            ele.add_source_candidate(std::move(sc));
          }
        }
        /*-------------------*/
    }

    uevent.set(handle, move(eles));
    if(electrons_handle){
        EventAccess_::set_unmanaged(uevent, *electrons_handle, &uevent.get(handle));
    }
}




// Photons ----------------
NtupleWriterPhotons::NtupleWriterPhotons(Config & cfg, bool set_photons_member, const bool save_source_cands): save_source_candidates_(save_source_cands){
  handle = cfg.ctx.declare_event_output<vector<Photon>>(cfg.dest_branchname, cfg.dest);
  if(set_photons_member) photons_handle = cfg.ctx.get_handle<vector<Photon>>("photons");
  src_token = cfg.cc.consumes<std::vector<pat::Photon>>(cfg.src);
  pv_token = cfg.cc.consumes<std::vector<reco::Vertex>>(cfg.pv_src);
  IDtag_keys = cfg.id_keys;
  doPuppiIso_ = cfg.doPuppiIso;
}

NtupleWriterPhotons::~NtupleWriterPhotons(){}

void NtupleWriterPhotons::process(const edm::Event & event, uhh2::Event & uevent, const edm::EventSetup& iSetup){
    edm::Handle< std::vector<pat::Photon> > pho_handle;
    event.getByToken(src_token, pho_handle);

   //  edm::Handle<std::vector<reco::Vertex>> pv_handle;
   // event.getByToken(pv_token, pv_handle);
   // if(pv_handle->empty()){
   //     edm::LogWarning("NtupleWriterPhotons") << "No PVs found, not writing phoctrons!";
   //     return;
   // }
   // const auto & PV = pv_handle->front();

    std::vector<Photon> phos;
    for (const pat::Photon & pat_pho : *pho_handle) {
        phos.emplace_back();
        Photon & pho = phos.back();
        pho.set_charge( pat_pho.charge());
        pho.set_pt( pat_pho.pt());
        pho.set_eta( pat_pho.eta());
        pho.set_phi( pat_pho.phi());
        pho.set_energy( pat_pho.energy());
        pho.set_vertex_x(pat_pho.vertex().x());
        pho.set_vertex_y(pat_pho.vertex().y());
        pho.set_vertex_z(pat_pho.vertex().z());
        if (doPuppiIso_){
          pho.set_puppiChargedHadronIso(pat_pho.puppiChargedHadronIso());
          pho.set_puppiNeutralHadronIso(pat_pho.puppiNeutralHadronIso());
          pho.set_puppiPhotonIso(pat_pho.puppiPhotonIso());
        }
        pho.set_supercluster_eta( pat_pho.superCluster()->eta() );
        pho.set_supercluster_phi( pat_pho.superCluster()->phi() );

        pho.set_trackIso(pat_pho.trackIso());
        pho.set_ecalIso(pat_pho.ecalIso());
        pho.set_hcalIso(pat_pho.hcalIso());
        pho.set_caloIso(pat_pho.caloIso());

        //	pho.set_patParticleIso(pat_pho.patParticleIso());
        pho.set_chargedHadronIso(pat_pho.chargedHadronIso());
        pho.set_neutralHadronIso(pat_pho.neutralHadronIso());
        pho.set_photonIso(pat_pho.photonIso());
        pho.set_puChargedHadronIso(pat_pho.puChargedHadronIso());

        for(const auto& tag_str : IDtag_keys){
          if(!pat_pho.isPhotonIDAvailable(tag_str)) throw cms::Exception("Missing Photon ID", "PhotonID not found: "+tag_str);
          pho.set_tag(Photon::tagname2tag(tag_str), float(pat_pho.photonID(tag_str)));
        }

        /* source candidates */
        if(save_source_candidates_){
          for(unsigned int s=0; s<pat_pho.numberOfSourceCandidatePtrs(); ++s){
            if(!pat_pho.sourceCandidatePtr(s).isAvailable()) continue;
            source_candidate sc;
            sc.key = pat_pho.sourceCandidatePtr(s).key();
            sc.px  = pat_pho.sourceCandidatePtr(s)->px();
            sc.py  = pat_pho.sourceCandidatePtr(s)->py();
            sc.pz  = pat_pho.sourceCandidatePtr(s)->pz();
            sc.E   = pat_pho.sourceCandidatePtr(s)->energy();
            pho.add_source_candidate(std::move(sc));
          }
        }
        /*-------------------*/
    }

    uevent.set(handle, move(phos));
    if(photons_handle){
        EventAccess_::set_unmanaged(uevent, *photons_handle, &uevent.get(handle));
    }
}

//-------------------------

NtupleWriterMuons::NtupleWriterMuons(Config & cfg, bool set_muons_member, const bool save_source_cands): save_source_candidates_(save_source_cands){
  handle = cfg.ctx.declare_event_output<vector<Muon>>(cfg.dest_branchname, cfg.dest);
  if(set_muons_member) muons_handle = cfg.ctx.get_handle<vector<Muon>>("muons");
  src_token = cfg.cc.consumes<std::vector<pat::Muon>>(cfg.src);
  pv_token = cfg.cc.consumes<std::vector<reco::Vertex>>(cfg.pv_src);
}

NtupleWriterMuons::~NtupleWriterMuons(){}

void NtupleWriterMuons::process(const edm::Event & event, uhh2::Event & uevent,  const edm::EventSetup& iSetup){
   edm::Handle<std::vector<pat::Muon>> mu_handle;
   event.getByToken(src_token, mu_handle);

   edm::Handle<std::vector<reco::Vertex>> pv_handle;
   event.getByToken(pv_token, pv_handle);
   if(pv_handle->empty()){
       edm::LogWarning("NtupleWriterMuons") << "No PVs found, not writing muons!";
       return;
   }
   const auto & PV = pv_handle->front();

   std::vector<Muon> mus;
   for (const pat::Muon & pat_mu : *mu_handle) {
     mus.emplace_back();
     Muon & mu = mus.back();
     mu.set_charge( pat_mu.charge());
     mu.set_pt( pat_mu.pt());
     mu.set_eta( pat_mu.eta());
     mu.set_phi( pat_mu.phi());
     mu.set_energy( pat_mu.energy());
     // mu.set_ptError( pat_mu.ptError());
     // mu.set_etaError( pat_mu.etaError());
     // mu.set_phiError( pat_mu.phiError());
     // mu.set_energyError( pat_mu.energyError());

     mu.set_selector(Muon::Global    , pat_mu.isGlobalMuon());
     mu.set_selector(Muon::PF        , pat_mu.isPFMuon());
     mu.set_selector(Muon::Tracker   , pat_mu.isTrackerMuon());
     mu.set_selector(Muon::Standalone, pat_mu.isStandAloneMuon());

     // 2016v2 specific ones
     mu.set_selector(Muon::Soft  , pat_mu.isSoftMuon(PV));
     mu.set_selector(Muon::Loose , pat_mu.isLooseMuon());
     mu.set_selector(Muon::Medium, pat_mu.isMediumMuon());
     mu.set_selector(Muon::Tight , pat_mu.isTightMuon(PV));
     mu.set_selector(Muon::Highpt, pat_mu.isHighPtMuon(PV));

     // 2016v3 & 2017 onwards uses these instead
     mu.set_selector(Muon::CutBasedIdLoose       , pat_mu.passed(reco::Muon::CutBasedIdLoose));
     mu.set_selector(Muon::CutBasedIdMedium      , pat_mu.passed(reco::Muon::CutBasedIdMedium));
     mu.set_selector(Muon::CutBasedIdMediumPrompt, pat_mu.passed(reco::Muon::CutBasedIdMediumPrompt));
     mu.set_selector(Muon::CutBasedIdTight       , pat_mu.passed(reco::Muon::CutBasedIdTight));
     mu.set_selector(Muon::CutBasedIdGlobalHighPt, pat_mu.passed(reco::Muon::CutBasedIdGlobalHighPt));
     mu.set_selector(Muon::CutBasedIdTrkHighPt   , pat_mu.passed(reco::Muon::CutBasedIdTrkHighPt));
     mu.set_selector(Muon::SoftCutBasedId        , pat_mu.passed(reco::Muon::SoftCutBasedId));
     mu.set_selector(Muon::SoftMvaId             , pat_mu.passed(reco::Muon::SoftMvaId));
     mu.set_selector(Muon::MvaLoose              , pat_mu.passed(reco::Muon::MvaLoose));
     mu.set_selector(Muon::MvaMedium             , pat_mu.passed(reco::Muon::MvaMedium));
     mu.set_selector(Muon::MvaTight              , pat_mu.passed(reco::Muon::MvaTight));

     mu.set_selector(Muon::PFIsoVeryLoose  , pat_mu.passed(reco::Muon::PFIsoVeryLoose));
     mu.set_selector(Muon::PFIsoLoose      , pat_mu.passed(reco::Muon::PFIsoLoose));
     mu.set_selector(Muon::PFIsoMedium     , pat_mu.passed(reco::Muon::PFIsoMedium));
     mu.set_selector(Muon::PFIsoTight      , pat_mu.passed(reco::Muon::PFIsoTight));
     mu.set_selector(Muon::PFIsoVeryTight  , pat_mu.passed(reco::Muon::PFIsoVeryTight));
     mu.set_selector(Muon::TkIsoLoose      , pat_mu.passed(reco::Muon::TkIsoLoose));
     mu.set_selector(Muon::TkIsoTight      , pat_mu.passed(reco::Muon::TkIsoTight));
     mu.set_selector(Muon::MiniIsoLoose    , pat_mu.passed(reco::Muon::MiniIsoLoose));
     mu.set_selector(Muon::MiniIsoMedium   , pat_mu.passed(reco::Muon::MiniIsoMedium));
     mu.set_selector(Muon::MiniIsoTight    , pat_mu.passed(reco::Muon::MiniIsoTight));
     mu.set_selector(Muon::MiniIsoVeryTight, pat_mu.passed(reco::Muon::MiniIsoVeryTight));

     if(pat_mu.simType() == reco::NotMatched)                       mu.set_simType(Muon::NotMatched);
     else if(pat_mu.simType() == reco::MatchedPunchthrough)         mu.set_simType(Muon::MatchedPunchthrough);
     else if(pat_mu.simType() == reco::MatchedElectron)             mu.set_simType(Muon::MatchedElectron);
     else if(pat_mu.simType() == reco::MatchedPrimaryMuon)          mu.set_simType(Muon::MatchedPrimaryMuon);
     else if(pat_mu.simType() == reco::MatchedMuonFromLightFlavour) mu.set_simType(Muon::MatchedLightQuark);
     else if(pat_mu.simType() == reco::GhostPunchthrough)           mu.set_simType(Muon::GhostPunchthrough);
     else if(pat_mu.simType() == reco::GhostElectron)               mu.set_simType(Muon::GhostElectron);
     else if(pat_mu.simType() == reco::GhostPrimaryMuon)            mu.set_simType(Muon::GhostPrimaryMuon);
     else if(pat_mu.simType() == reco::GhostMuonFromLightFlavour)   mu.set_simType(Muon::GhostLightQuark);
     else if(pat_mu.simType() == reco::MatchedMuonFromHeavyFlavour){
       if(pat_mu.simExtType() == reco::MatchedMuonFromTau)          mu.set_simType(Muon::MatchedTau);
       else                                                         mu.set_simType(Muon::MatchedHeavyQuark);
     }
     else if(pat_mu.simType() == reco::GhostMuonFromHeavyFlavour){
       if(pat_mu.simExtType() == reco::GhostMuonFromTau)            mu.set_simType(Muon::GhostTau);
       else                                                         mu.set_simType(Muon::GhostHeavyQuark);
     }
     else                                                           mu.set_simType(Muon::Unknown);

     mu.set_simFlavor(pat_mu.simFlavour());
     mu.set_simPdgId(pat_mu.simPdgId());
     mu.set_simMotherPdgId(pat_mu.simMotherPdgId());
     mu.set_simHeaviestMotherFlavor(pat_mu.simHeaviestMotherFlavour());

     mu.set_dxy      (pat_mu.muonBestTrack()->dxy(PV.position()));
     mu.set_dxy_error(pat_mu.muonBestTrack()->dxyError());
     mu.set_dz       (pat_mu.muonBestTrack()->dz(PV.position()));
     mu.set_dz_error (pat_mu.muonBestTrack()->dzError());

     mu.set_globalTrack_normalizedChi2       ( pat_mu.globalTrack().isNonnull() ? pat_mu.globalTrack()->normalizedChi2() : -999.);
     mu.set_globalTrack_numberOfValidMuonHits( pat_mu.globalTrack().isNonnull() ? pat_mu.globalTrack()->hitPattern().numberOfValidMuonHits() : -1);

     mu.set_numberOfMatchedStations(pat_mu.numberOfMatchedStations());

     mu.set_innerTrack_trackerLayersWithMeasurement(pat_mu.innerTrack().isNonnull() ? pat_mu.innerTrack()->hitPattern().trackerLayersWithMeasurement() : -1);
     mu.set_innerTrack_numberOfValidPixelHits      (pat_mu.innerTrack().isNonnull() ? pat_mu.innerTrack()->hitPattern().numberOfValidPixelHits() : -1);
     mu.set_innerTrack_validFraction               (pat_mu.innerTrack().isNonnull() ? pat_mu.innerTrack()->validFraction() : -999.);

     mu.set_combinedQuality_chi2LocalPosition(pat_mu.combinedQuality().chi2LocalPosition);
     mu.set_combinedQuality_trkKink          (pat_mu.combinedQuality().trkKink);

     mu.set_segmentCompatibility(muon::segmentCompatibility(pat_mu));

     mu.set_sumChargedHadronPt(pat_mu.pfIsolationR04().sumChargedHadronPt);
     mu.set_sumNeutralHadronEt(pat_mu.pfIsolationR04().sumNeutralHadronEt);
     mu.set_sumPhotonEt       (pat_mu.pfIsolationR04().sumPhotonEt);
     mu.set_sumPUPt           (pat_mu.pfIsolationR04().sumPUPt);

     mu.set_pfMINIIso_CH      (pat_mu.hasUserFloat("muPFMiniIsoValueCHSTAND") ? pat_mu.userFloat("muPFMiniIsoValueCHSTAND") : -999.);
     mu.set_pfMINIIso_NH      (pat_mu.hasUserFloat("muPFMiniIsoValueNHSTAND") ? pat_mu.userFloat("muPFMiniIsoValueNHSTAND") : -999.);
     mu.set_pfMINIIso_Ph      (pat_mu.hasUserFloat("muPFMiniIsoValuePhSTAND") ? pat_mu.userFloat("muPFMiniIsoValuePhSTAND") : -999.);
     mu.set_pfMINIIso_PU      (pat_mu.hasUserFloat("muPFMiniIsoValuePUSTAND") ? pat_mu.userFloat("muPFMiniIsoValuePUSTAND") : -999.);
     mu.set_pfMINIIso_NH_pfwgt(pat_mu.hasUserFloat("muPFMiniIsoValueNHPFWGT") ? pat_mu.userFloat("muPFMiniIsoValueNHPFWGT") : -999.);
     mu.set_pfMINIIso_Ph_pfwgt(pat_mu.hasUserFloat("muPFMiniIsoValuePhPFWGT") ? pat_mu.userFloat("muPFMiniIsoValuePhPFWGT") : -999.);

     const auto & tunePTrack = pat_mu.tunePMuonBestTrack();
     mu.set_tunePTrackPt(tunePTrack->pt());
     mu.set_tunePTrackEta(tunePTrack->eta());
     mu.set_tunePTrackPhi(tunePTrack->phi());
     mu.set_tunePTrackType(static_cast<Muon::MuonTrackType>(pat_mu.tunePMuonBestTrackType()));

     /* source candidates */
     if(save_source_candidates_){

       for(unsigned int s=0; s<pat_mu.numberOfSourceCandidatePtrs(); ++s){

         if(!pat_mu.sourceCandidatePtr(s).isAvailable()) continue;

         source_candidate sc;
         sc.key = pat_mu.sourceCandidatePtr(s).key();
         sc.px  = pat_mu.sourceCandidatePtr(s)->px();
         sc.py  = pat_mu.sourceCandidatePtr(s)->py();
         sc.pz  = pat_mu.sourceCandidatePtr(s)->pz();
         sc.E   = pat_mu.sourceCandidatePtr(s)->energy();

         mu.add_source_candidate(std::move(sc));
       }
     }
     /*-------------------*/
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

void NtupleWriterTaus::process(const edm::Event & event, uhh2::Event & uevent,  const edm::EventSetup& iSetup){
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

         FILL_TAU_BIT(againstElectronVLooseMVA6);
         FILL_TAU_BIT(againstElectronLooseMVA6);
         FILL_TAU_BIT(againstElectronMediumMVA6);
         FILL_TAU_BIT(againstElectronTightMVA6);
         FILL_TAU_BIT(againstElectronVTightMVA6);
         FILL_TAU_BIT(againstMuonLoose3);
         FILL_TAU_BIT(againstMuonTight3);
         FILL_TAU_BIT(byLooseCombinedIsolationDeltaBetaCorr3Hits);
         FILL_TAU_BIT(byMediumCombinedIsolationDeltaBetaCorr3Hits);
         FILL_TAU_BIT(byTightCombinedIsolationDeltaBetaCorr3Hits);
	 // MVA based isolation discriminators not yet recommended for RunII (need to be retrained)
	 FILL_TAU_BIT(byVLooseIsolationMVArun2v1DBnewDMwLT);
         FILL_TAU_BIT(byLooseIsolationMVArun2v1DBnewDMwLT);
         FILL_TAU_BIT(byMediumIsolationMVArun2v1DBnewDMwLT);
         FILL_TAU_BIT(byTightIsolationMVArun2v1DBnewDMwLT);
         FILL_TAU_BIT(byVTightIsolationMVArun2v1DBnewDMwLT);
         FILL_TAU_BIT(byVVTightIsolationMVArun2v1DBnewDMwLT);

	 FILL_TAU_BIT(decayModeFinding);
         FILL_TAU_BIT(decayModeFindingNewDMs);



         #define FILL_TAU_FLOAT(name) tau.set_##name (pat_tau.tauID(#name))

         FILL_TAU_FLOAT(byCombinedIsolationDeltaBetaCorrRaw3Hits);
         FILL_TAU_FLOAT(byIsolationMVArun2v1DBnewDMwLTraw);
         FILL_TAU_FLOAT(chargedIsoPtSum);
         FILL_TAU_FLOAT(neutralIsoPtSum);
         FILL_TAU_FLOAT(puCorrPtSum);

	 // note: only available with dynamic strip reconstruction (included by default from CMSSW_7_4_14)
	 // FILL_TAU_BIT(byLoosePileupWeightedIsolation3Hits);
	 // FILL_TAU_BIT(byMediumPileupWeightedIsolation3Hits);
	 // FILL_TAU_BIT(byTightPileupWeightedIsolation3Hits);
	 // FILL_TAU_FLOAT(byPileupWeightedIsolationRaw3Hits);
	 FILL_TAU_FLOAT(neutralIsoPtSumWeight);
	 FILL_TAU_FLOAT(footprintCorrection);
	 FILL_TAU_FLOAT(photonPtSumOutsideSignalCone);

         #undef FILL_TAU_BIT
         #undef FILL_TAU_FLOAT

         tau.set_decayMode(pat_tau.decayMode());

    }
    uevent.set(handle, move(taus));
    if(taus_handle){
       EventAccess_::set_unmanaged(uevent, *taus_handle, &uevent.get(handle));
    }
}
