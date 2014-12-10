#include "UHH2/core/plugins/NtupleWriterJets.h"
#include "UHH2/core/include/AnalysisModule.h"

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
