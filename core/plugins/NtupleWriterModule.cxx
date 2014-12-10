#include "UHH2/core/plugins/NtupleWriterModule.h"

uhh2::NtupleWriterModule::~NtupleWriterModule(){}



PFParticle uhh2::PFCandidate2PFParticle(const pat::PackedCandidate & pf, bool fromjet, bool fromiso, bool frompuiso){
    PFParticle part;
    part.set_pt(pf.pt());
    part.set_eta(pf.eta());
    part.set_phi(pf.phi());
    part.set_energy(pf.energy());
    part.set_charge(pf.charge());
    // part.set_ecal_en(pf.ecalEnergy());
    // part.set_hcal_en(pf.hcalEnergy());
    part.set_isJetParticle(fromjet);
    part.set_isIsoParticle(fromiso);
    part.set_isPUIsoParticle(frompuiso);
    if(pf.fromPV()>=2) part.set_fromPV(true);
    else part.set_fromPV(false);
    reco::Track trackref = pf.pseudoTrack();
    //if (!trackref.isNull()){
    //part.set_track_mom(trackref.p());
    //}

    //electron ID not fully supported when taken from miniAOD
    PFParticle::EParticleID id = PFParticle::eX;
    // if(pf.isGlobalMuon() || pf.isStandAloneMuon() ) id = PFParticle::eMu;
    // else if(pf.charge()==0) id = PFParticle::eH0;
    // else if(pf.fromPV() >= 2) id = PFParticle::eH;

    reco::PFCandidate pfp;
    pfp.setCharge(pf.charge());
    switch ( pfp.translatePdgIdToType(pf.pdgId()) ){
         case reco::PFCandidate::X : id = PFParticle::eX; break;
         case reco::PFCandidate::h : id = PFParticle::eH; break;
         case reco::PFCandidate::e : id = PFParticle::eE; break;
         case reco::PFCandidate::mu : id = PFParticle::eMu; break;
         case reco::PFCandidate::gamma : id = PFParticle::eGamma; break;
         case reco::PFCandidate::h0 : id = PFParticle::eH0; break;
         case reco::PFCandidate::h_HF : id = PFParticle::eH_HF; break;
         case reco::PFCandidate::egamma_HF : id = PFParticle::eEgamma_HF; break;
     }
    part.set_particleID(id);
    return part;
}


PFParticle uhh2::PFCandidate2PFParticle(const reco::PFCandidate & pf, bool fromjet, bool fromiso, bool frompuiso){
    PFParticle part;
    part.set_pt(pf.pt());
    part.set_eta(pf.eta());
    part.set_phi(pf.phi());
    part.set_energy(pf.energy());
    part.set_charge(pf.charge());
    part.set_ecal_en(pf.ecalEnergy());
    part.set_hcal_en(pf.hcalEnergy());
    part.set_isJetParticle(fromjet);
    part.set_isIsoParticle(fromiso);
    part.set_isPUIsoParticle(frompuiso);
    reco::TrackRef trackref = pf.trackRef();
    if (!trackref.isNull()){
      part.set_track_mom(trackref->p());
    }
    PFParticle::EParticleID id = PFParticle::eX;
    switch ( pf.translatePdgIdToType(pf.pdgId()) ){
        case reco::PFCandidate::X : id = PFParticle::eX; break;
        case reco::PFCandidate::h : id = PFParticle::eH; break;
        case reco::PFCandidate::e : id = PFParticle::eE; break;
        case reco::PFCandidate::mu : id = PFParticle::eMu; break;
        case reco::PFCandidate::gamma : id = PFParticle::eGamma; break;
        case reco::PFCandidate::h0 : id = PFParticle::eH0; break;
        case reco::PFCandidate::h_HF : id = PFParticle::eH_HF; break;
        case reco::PFCandidate::egamma_HF : id = PFParticle::eEgamma_HF; break;
    }
    part.set_particleID(id);
    return part;
}

namespace {
template<typename T>
size_t add_pfpartT(const T & pf, std::vector<PFParticle> & pfs, bool fromjet, bool fromiso, bool frompuiso){
   for(size_t j=0; j<pfs.size(); ++j){
     const PFParticle & spf = pfs[j];
     // note: static_cast to float is to ensure the comparison is done with the same precision as these quantities
     // have been stored in spf. Otherwise, there could be a non-zero difference just because of conversion loss
     // from double to float.
     float r = fabsf(static_cast<float>(pf.eta()) - spf.eta()) + fabsf(static_cast<float>(pf.phi()) - spf.phi());
     float dpt = fabsf(static_cast<float>(pf.pt()) - spf.pt());
     if (r == 0.0f && dpt == 0.0f){
         if(fromjet) pfs[j].set_isJetParticle(true);
         if(fromiso) pfs[j].set_isIsoParticle(true);
         if(frompuiso) pfs[j].set_isPUIsoParticle(true);
         return j;
     }
   }
   pfs.push_back(uhh2::PFCandidate2PFParticle(pf, fromjet, fromiso, frompuiso));
   return pfs.size()-1;
}
}

size_t uhh2::add_pfpart(const pat::PackedCandidate & pf, std::vector<PFParticle> & pfs, bool fromjet, bool fromiso, bool frompuiso){
    return add_pfpartT(pf, pfs, fromjet, fromiso, frompuiso);
}


size_t uhh2::add_pfpart(const reco::PFCandidate & pf, std::vector<PFParticle> & pfs, bool fromjet, bool fromiso, bool frompuiso){
    return add_pfpartT(pf, pfs, fromjet, fromiso, frompuiso);
}

