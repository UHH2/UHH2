#include "UHH2/core/plugins/NtupleWriterJets.h"
#include "UHH2/core/include/AnalysisModule.h"

#include "UHH2/core/plugins/JetProps.h"

#include "RecoBTau/JetTagComputer/interface/GenericMVAJetTagComputer.h"

using namespace uhh2;
using namespace std;


NtupleWriterJets::NtupleWriterJets(Config & cfg, bool set_jets_member) {
    handle = cfg.ctx.declare_event_output<vector<Jet>>(cfg.dest_branchname, cfg.dest);
    ptmin = cfg.ptmin;
    etamax = cfg.etamax;
    if(set_jets_member){
        jets_handle = cfg.ctx.get_handle<vector<Jet>>("jets");
    }
    src_token = cfg.cc.consumes<std::vector<pat::Jet>>(cfg.src);
    pfparts = cfg.pfparts;
    runOnMiniAOD = cfg.runOnMiniAOD;
}

NtupleWriterJets::~NtupleWriterJets(){}

void NtupleWriterJets::process(const edm::Event & event, uhh2::Event & uevent){
    edm::Handle< std::vector<pat::Jet> > jet_handle;
    event.getByToken(src_token, jet_handle);
    std::vector<Jet> jets;
    const std::vector<pat::Jet> & pat_jets = *jet_handle;
    for (unsigned int i = 0; i < pat_jets.size(); ++i) {
        const pat::Jet & pat_jet = pat_jets[i];
        if(pat_jet.pt() < ptmin) continue;
        if(fabs(pat_jet.eta()) > etamax) continue;
        jets.emplace_back();
        fill_jet_info(pat_jet, jets.back());
        if(pfparts){
            store_jet_constituents(pat_jet, jets.back(), *pfparts, runOnMiniAOD);
        }
    }
    uevent.set(handle, move(jets));
    if(jets_handle){
        EventAccess_::set_unmanaged(uevent, *jets_handle, &uevent.get(handle));
    }
}


void NtupleWriterJets::fill_jet_info(const pat::Jet & pat_jet, Jet & jet){
    jet.set_charge(pat_jet.charge());
    jet.set_pt(pat_jet.pt());
    jet.set_eta(pat_jet.eta());
    jet.set_phi(pat_jet.phi());
    jet.set_energy(pat_jet.energy());
    jet.set_flavor(pat_jet.partonFlavour());
    jet.set_numberOfDaughters (pat_jet.numberOfDaughters());
    const reco::TrackRefVector&  jettracks = pat_jet.associatedTracks();
    jet.set_nTracks ( jettracks.size());
    jet.set_jetArea(pat_jet.jetArea());
    if(pat_jet.isPFJet()){
        jet.set_neutralEmEnergyFraction (pat_jet.neutralEmEnergyFraction());
        jet.set_neutralHadronEnergyFraction (pat_jet.neutralHadronEnergyFraction());
        jet.set_chargedEmEnergyFraction (pat_jet.chargedEmEnergyFraction());
        jet.set_chargedHadronEnergyFraction (pat_jet.chargedHadronEnergyFraction());
        jet.set_muonEnergyFraction (pat_jet.muonEnergyFraction());
        jet.set_photonEnergyFraction (pat_jet.photonEnergyFraction());
        jet.set_chargedMultiplicity (pat_jet.chargedMultiplicity());
        jet.set_neutralMultiplicity (pat_jet.neutralMultiplicity());
        jet.set_muonMultiplicity (pat_jet.muonMultiplicity());
        jet.set_electronMultiplicity (pat_jet.electronMultiplicity());
        jet.set_photonMultiplicity (pat_jet.photonMultiplicity());
    }
    jet.set_JEC_factor_raw(pat_jet.jecFactor("Uncorrected"));
    jet.set_btag_simpleSecondaryVertexHighEff(pat_jet.bDiscriminator("simpleSecondaryVertexHighEffBJetTags"));
    jet.set_btag_simpleSecondaryVertexHighPur(pat_jet.bDiscriminator("simpleSecondaryVertexHighPurBJetTags"));
    jet.set_btag_combinedSecondaryVertex(pat_jet.bDiscriminator("combinedInclusiveSecondaryVertexV2BJetTags"));
    jet.set_btag_combinedSecondaryVertexMVA(pat_jet.bDiscriminator("combinedMVABJetTags"));
    jet.set_btag_jetBProbability(pat_jet.bDiscriminator("jetBProbabilityBJetTags"));
    jet.set_btag_jetProbability(pat_jet.bDiscriminator("jetProbabilityBJetTags"));
}


void NtupleWriterJets::store_jet_constituents(const pat::Jet & pat_jet, Jet& jet, std::vector<PFParticle> & pfparts, bool runOnMiniAOD){
    if(runOnMiniAOD){
        for (unsigned int i=0; i<pat_jet.numberOfDaughters(); ++i){
            pat::PackedCandidate* pf = ( pat::PackedCandidate*) pat_jet.daughter(i);
            size_t pfparticles_index = add_pfpart(*pf, pfparts, true, false, false);
            jet.add_pfconstituents_index(pfparticles_index);
        }
    }
    else{
        const std::vector<reco::PFCandidatePtr> jconstits = pat_jet.getPFConstituents();
        for (unsigned int i=0; i<jconstits.size(); ++i){
            const reco::PFCandidate* pf = jconstits[i].get();
            size_t pfparticles_index = add_pfpart(*pf, pfparts, true, false, false);
            jet.add_pfconstituents_index(pfparticles_index);
        }
    }
}


NtupleWriterTopJets::NtupleWriterTopJets(Config & cfg, bool set_jets_member): runOnMiniAOD(cfg.runOnMiniAOD),
        doTagInfos(cfg.doTagInfos), pfparts(cfg.pfparts), ptmin(cfg.ptmin), etamax(cfg.etamax){
    handle = cfg.ctx.declare_event_output<vector<TopJet>>(cfg.dest_branchname, cfg.dest);
    if(set_jets_member){
        topjets_handle = cfg.ctx.get_handle<vector<TopJet>>("topjets");
    }
    src_token = cfg.cc.consumes<std::vector<pat::Jet>>(cfg.src);
    njettiness_src = cfg.njettiness_src;
    if(cfg.pfparts || !cfg.njettiness_src.empty()){
        constituent_src_token = cfg.cc.consumes<std::vector<pat::Jet>>(cfg.constituent_src);
    }
}

NtupleWriterTopJets::~NtupleWriterTopJets(){}

namespace {

void fill_subjet_taginfos(TopJet & topjet, const pat::Jet * subjet){
    reco::TaggingVariableList tvlIP = subjet->tagInfoTrackIP("impactParameter")->taggingVariables();
    topjet.add_subTrackMomentum(tvlIP.getList(reco::btau::trackMomentum, false));
    topjet.add_subTrackEta(tvlIP.getList(reco::btau::trackEta, false));
    topjet.add_subTrackEtaRel(tvlIP.getList(reco::btau::trackEtaRel, false));
    topjet.add_subTrackDeltaR(tvlIP.getList(reco::btau::trackDeltaR, false));
    topjet.add_subTrackSip3dVal(tvlIP.getList(reco::btau::trackSip3dVal, false));
    topjet.add_subTrackSip3dSig(tvlIP.getList(reco::btau::trackSip3dSig, false));
    topjet.add_subTrackSip2dVal(tvlIP.getList(reco::btau::trackSip2dVal, false));
    topjet.add_subTrackSip2dSig(tvlIP.getList(reco::btau::trackSip2dSig, false));
    topjet.add_subTrackDecayLenVal(tvlIP.getList(reco::btau::trackDecayLenVal, false));
    topjet.add_subTrackChi2(tvlIP.getList(reco::btau::trackChi2, false));
    topjet.add_subTrackNTotalHits(tvlIP.getList(reco::btau::trackNTotalHits, false));
    topjet.add_subTrackNPixelHits(tvlIP.getList(reco::btau::trackNPixelHits, false));
    topjet.add_subTrackPtRel(tvlIP.getList(reco::btau::trackPtRel, false));
    topjet.add_subTrackPPar(tvlIP.getList(reco::btau::trackPPar, false));
    topjet.add_subTrackPtRatio(tvlIP.getList(reco::btau::trackPtRatio, false));
    topjet.add_subTrackPParRatio(tvlIP.getList(reco::btau::trackPParRatio, false));
    topjet.add_subTrackJetDistVal(tvlIP.getList(reco::btau::trackJetDistVal, false));
    topjet.add_subTrackJetDistSig(tvlIP.getList(reco::btau::trackJetDistSig, false));
    topjet.add_subTrackGhostTrackDistVal(tvlIP.getList(reco::btau::trackGhostTrackDistVal, false));
    topjet.add_subTrackGhostTrackDistSig(tvlIP.getList(reco::btau::trackGhostTrackDistSig, false));
    topjet.add_subTrackGhostTrackWeight(tvlIP.getList(reco::btau::trackGhostTrackWeight, false));
    //sv tag info
    reco::TaggingVariableList tvlSV = subjet->tagInfoSecondaryVertex("secondaryVertex")->taggingVariables();
    topjet.add_subFlightDistance2dVal(tvlSV.getList(reco::btau::flightDistance2dVal, false));
    topjet.add_subFlightDistance2dSig(tvlSV.getList(reco::btau::flightDistance2dSig, false));
    topjet.add_subFlightDistance3dVal(tvlSV.getList(reco::btau::flightDistance3dVal, false));
    topjet.add_subFlightDistance3dSig(tvlSV.getList(reco::btau::flightDistance3dSig, false));
    topjet.add_subVertexJetDeltaR(tvlSV.getList(reco::btau::vertexJetDeltaR, false));
    topjet.add_subJetNSecondaryVertices(tvlSV.get(reco::btau::jetNSecondaryVertices, -9999));
    topjet.add_subVertexNTracks(tvlSV.get(reco::btau::vertexNTracks, -9999));
    std::vector<TLorentzVector> vp4;
    std::vector<float> vchi2;
    std::vector<float> vndof;
    std::vector<float> vchi2ndof;
    std::vector<float> vtrsize;
    auto sv = subjet->tagInfoSecondaryVertex("secondaryVertex");
    unsigned int nv = sv->nVertices();
    for (unsigned int i = 0; i < nv; i++) {
        const auto & sv_i = sv->secondaryVertex(i);
        ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > p4 = sv_i.p4();
        vp4.push_back(TLorentzVector(p4.px(), p4.py(), p4.pz(), p4.e()));
        vchi2.push_back(sv_i.chi2());
        vndof.push_back(sv_i.ndof());
        vchi2ndof.push_back(sv_i.normalizedChi2());
        vtrsize.push_back(sv_i.tracksSize());
    }
    topjet.add_subSecondaryVertex(vp4);
    topjet.add_subVertexChi2(vchi2);
    topjet.add_subVertexNdof(vndof);
    topjet.add_subVertexNormalizedChi2(vchi2ndof);
    topjet.add_subVertexTracksSize(vtrsize);
}

}


void NtupleWriterTopJets::process(const edm::Event & event, uhh2::Event & uevent){
    edm::Handle<pat::JetCollection> h_pat_topjets;
    event.getByToken(src_token, h_pat_topjets);
    const vector<pat::Jet> & pat_topjets = *h_pat_topjets;
    
    edm::Handle<edm::ValueMap<float> > h_njettiness1, h_njettiness2, h_njettiness3;
    
    if(!njettiness_src.empty()){
        event.getByLabel(edm::InputTag(njettiness_src, "tau1"), h_njettiness1);
        event.getByLabel(edm::InputTag(njettiness_src, "tau2"), h_njettiness2);
        event.getByLabel(edm::InputTag(njettiness_src, "tau3"), h_njettiness3);
    }

    vector<TopJet> topjets;

    for (unsigned int i = 0; i < pat_topjets.size(); i++) {
        const pat::Jet & pat_topjet =  pat_topjets[i];
        if(pat_topjet.pt() < ptmin) continue;
        if(fabs(pat_topjet.eta()) > etamax) continue;

        topjets.emplace_back();
        TopJet & topjet = topjets.back();
        uhh2::NtupleWriterJets::fill_jet_info(pat_topjet, topjet);
        
        // match a unpruned jet according to topjets_with_cands:
        int i_pat_topjet_wc = -1;
        edm::Handle<pat::JetCollection> pat_topjets_with_cands;
        if(pfparts || !njettiness_src.empty()){
            event.getByToken(constituent_src_token, pat_topjets_with_cands);
            for (size_t i_wc=0; i_wc < pat_topjets_with_cands->size(); ++i_wc) {
                const auto & cand = (*pat_topjets_with_cands)[i_wc];
                double dphi = reco::deltaPhi(cand.phi(), pat_topjet.phi());
                if (fabs(dphi)<0.5 && fabs(cand.eta()-pat_topjet.eta())<0.5){ // be generous: filtering, pruning... can change jet axis
                    i_pat_topjet_wc = i_wc;
                    break;
                }
            }
        }

        if (pfparts && i_pat_topjet_wc >= 0){
            uhh2::NtupleWriterJets::store_jet_constituents(pat_topjets_with_cands->at(i_pat_topjet_wc), topjet, *pfparts, runOnMiniAOD);
            // now run substructure information
            JetProps substructure(&topjet);
            substructure.set_pf_cands(pfparts);
            double tau1 = substructure.GetNsubjettiness(1, Njettiness::onepass_kt_axes, 1., 2.0);
            double tau2 = substructure.GetNsubjettiness(2, Njettiness::onepass_kt_axes, 1., 2.0);
            double tau3 = substructure.GetNsubjettiness(3, Njettiness::onepass_kt_axes, 1., 2.0);
            double qjets = substructure.GetQjetVolatility(event.id().event(), 2.0);
            topjet.set_tau1(tau1);
            topjet.set_tau2(tau2);
            topjet.set_tau3(tau3);
            topjet.set_qjets_volatility(qjets);
        }
        
        if (i_pat_topjet_wc >= 0 && !njettiness_src.empty()){
            auto ref = edm::Ref<pat::JetCollection>(pat_topjets_with_cands, i_pat_topjet_wc);
            topjet.set_tau1((*h_njettiness1)[ref]);
            topjet.set_tau2((*h_njettiness2)[ref]);
            topjet.set_tau3((*h_njettiness3)[ref]);
        }

        // loop over subjets to fill some more subjet info:
        for (unsigned int k = 0; k < pat_topjet.numberOfDaughters(); k++) {
            Particle subjet_v4;
            auto patsubjetd = dynamic_cast<const pat::Jet *>(pat_topjet.daughter(k));
            if (patsubjetd) {
                auto p4 = patsubjetd->correctedP4(0);
                subjet_v4.set_pt(p4.pt());
                subjet_v4.set_eta(p4.eta());
                subjet_v4.set_phi(p4.phi());
                subjet_v4.set_energy(p4.E());
                topjet.add_subjet(subjet_v4);
                //btag info
                topjet.add_subFlavour(patsubjetd->partonFlavour());
                topjet.add_subCSV(patsubjetd->bDiscriminator("combinedInclusiveSecondaryVertexV2BJetTags"));
                //info for subjet JEC correction
                topjet.add_subArea(patsubjetd->jetArea());
                topjet.add_subJEC_raw(patsubjetd->jecFactor("Uncorrected"));
                if (doTagInfos) {
                    //ip tag info
                    fill_subjet_taginfos(topjet, patsubjetd);
                    if (computer) {
                        std::vector<const reco::BaseTagInfo*> baseTagInfos;
                        baseTagInfos.push_back(patsubjetd->tagInfoTrackIP("impactParameter"));
                        baseTagInfos.push_back(patsubjetd->tagInfoSecondaryVertex("secondaryVertex"));
                        JetTagComputer::TagInfoHelper helper(baseTagInfos);
                        reco::TaggingVariableList vars = computer->taggingVariables(helper);
                        topjet.add_subVertexMassJTC(vars.get(reco::btau::vertexMass, -9999));
                        topjet.add_subVertexCategoryJTC(vars.get(reco::btau::vertexCategory, -9999));
                        topjet.add_subVertexEnergyRatioJTC(vars.get(reco::btau::vertexEnergyRatio, -9999));
                        topjet.add_subTrackSip3dSigAboveCharmJTC(vars.get(reco::btau::trackSip3dSigAboveCharm, -9999));
                    }
                } // doTagInfos
            } // if(patsubjetd)
            else {
                //filling only standard information in case the subjet has not been pat-tified during the pattuples production
                subjet_v4.set_pt(pat_topjet.daughter(k)->p4().pt());
                subjet_v4.set_eta(pat_topjet.daughter(k)->p4().eta());
                subjet_v4.set_phi(pat_topjet.daughter(k)->p4().phi());
                subjet_v4.set_energy(pat_topjet.daughter(k)->p4().E());
                topjet.add_subjet(subjet_v4);
            }
        }
    }// for topjets
    uevent.set(handle, move(topjets));
    if(topjets_handle){
        EventAccess_::set_unmanaged(uevent, *topjets_handle, &uevent.get(handle));
    }
}

