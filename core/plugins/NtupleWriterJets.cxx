#include "UHH2/core/plugins/NtupleWriterJets.h"
#include "UHH2/core/include/AnalysisModule.h"

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
    }
    uevent.set(handle, move(jets));
    if(jets_handle){
        uevent.set_unmanaged(*jets_handle, &uevent.get(handle));
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
    jet.set_btag_simpleSecondaryVertexHighEff(pat_jet.bDiscriminator("pfSimpleSecondaryVertexHighEffBJetTags"));
    jet.set_btag_simpleSecondaryVertexHighPur(pat_jet.bDiscriminator("pfSimpleSecondaryVertexHighPurBJetTags"));
    jet.set_btag_combinedSecondaryVertex(pat_jet.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags"));
    //jet.set_btag_combinedSecondaryVertexMVA(pat_jet.bDiscriminator("combinedMVABJetTags"));
    jet.set_btag_jetBProbability(pat_jet.bDiscriminator("pfJetBProbabilityBJetTags"));
    jet.set_btag_jetProbability(pat_jet.bDiscriminator("pfJetProbabilityBJetTags"));
}


NtupleWriterTopJets::NtupleWriterTopJets(Config & cfg, bool set_jets_member): runOnMiniAOD(cfg.runOnMiniAOD),
        ptmin(cfg.ptmin), etamax(cfg.etamax){
    handle = cfg.ctx.declare_event_output<vector<TopJet>>(cfg.dest_branchname, cfg.dest);
    if(set_jets_member){
        topjets_handle = cfg.ctx.get_handle<vector<TopJet>>("topjets");
    }
    src_token = cfg.cc.consumes<std::vector<pat::Jet>>(cfg.src);
    njettiness_src = cfg.njettiness_src;
    qjets_src = cfg.qjets_src;
    if(!njettiness_src.empty() || !qjets_src.empty()){
        substructure_variables_src_token = cfg.cc.consumes<std::vector<pat::Jet>>(cfg.substructure_variables_src);
    }
}

NtupleWriterTopJets::~NtupleWriterTopJets(){}


void NtupleWriterTopJets::process(const edm::Event & event, uhh2::Event & uevent){
    edm::Handle<pat::JetCollection> h_pat_topjets;
    event.getByToken(src_token, h_pat_topjets);
    const vector<pat::Jet> & pat_topjets = *h_pat_topjets;
    
    edm::Handle<edm::ValueMap<float>> h_njettiness1, h_njettiness2, h_njettiness3;
    edm::Handle<edm::ValueMap<float>> h_qjets;
    
    if(!njettiness_src.empty()){
        event.getByLabel(edm::InputTag(njettiness_src, "tau1"), h_njettiness1);
        event.getByLabel(edm::InputTag(njettiness_src, "tau2"), h_njettiness2);
        event.getByLabel(edm::InputTag(njettiness_src, "tau3"), h_njettiness3);
    }
    if(!qjets_src.empty()){
        event.getByLabel(edm::InputTag(qjets_src, "QjetsVolatility"), h_qjets);
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
        if(!njettiness_src.empty() || !qjets_src.empty()){
            event.getByToken(substructure_variables_src_token, pat_topjets_with_cands);
            for (size_t i_wc=0; i_wc < pat_topjets_with_cands->size(); ++i_wc) {
                const auto & cand = (*pat_topjets_with_cands)[i_wc];
                double dphi = reco::deltaPhi(cand.phi(), pat_topjet.phi());
                if (fabs(dphi)<0.5 && fabs(cand.eta()-pat_topjet.eta())<0.5){ // be generous: filtering, pruning... can change jet axis
                    i_pat_topjet_wc = i_wc;
                    break;
                }
            }
            if (i_pat_topjet_wc >= 0){
                auto ref = edm::Ref<pat::JetCollection>(pat_topjets_with_cands, i_pat_topjet_wc);
                if(!njettiness_src.empty()){
                    topjet.set_tau1((*h_njettiness1)[ref]);
                    topjet.set_tau2((*h_njettiness2)[ref]);
                    topjet.set_tau3((*h_njettiness3)[ref]);
                }
                if(!qjets_src.empty()){
                    topjet.set_qjets_volatility((*h_qjets)[ref]);
                }
            }
        }

        // loop over subjets to fill some more subjet info:
        for (unsigned int k = 0; k < pat_topjet.numberOfDaughters(); k++) {
            Jet subjet;
            auto patsubjetd = dynamic_cast<const pat::Jet *>(pat_topjet.daughter(k));
            if (patsubjetd) {
                NtupleWriterJets::fill_jet_info(*patsubjetd, subjet);
            }
            else {
                //filling only standard information in case the subjet has not been pat-tified during the pattuples production
                auto daughter = pat_topjet.daughter(k);
                if(daughter){
                    subjet.set_pt(daughter->pt());
                    subjet.set_eta(daughter->eta());
                    subjet.set_phi(daughter->phi());
                    subjet.set_energy(daughter->energy());
                }
                else{
                    throw runtime_error("subjet was nullptr");
                }
            }
            topjet.add_subjet(subjet);
        }
    }// for topjets
    uevent.set(handle, move(topjets));
    if(topjets_handle){
        uevent.set_unmanaged(*topjets_handle, &uevent.get(handle));
    }
}

