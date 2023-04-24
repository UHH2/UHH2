#include "UHH2/core/plugins/NtupleWriterJets.h"
#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Utils.h"
#include "FWCore/Framework/interface/one/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "RecoBTau/JetTagComputer/interface/GenericMVAJetTagComputer.h"
#include "RecoBTau/JetTagComputer/interface/GenericMVAJetTagComputerWrapper.h"
#include "RecoBTau/JetTagComputer/interface/JetTagComputer.h"
#include "RecoBTau/JetTagComputer/interface/JetTagComputerRecord.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/BTauReco/interface/HTTTopJetTagInfo.h"
#include "RecoBTag/SecondaryVertex/interface/CandidateBoostedDoubleSecondaryVertexComputer.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "RecoBTau/JetTagComputer/interface/JetTagComputerRecord.h"
#include "DataFormats/BTauReco/interface/CandIPTagInfo.h"
#include "DataFormats/BTauReco/interface/CandSecondaryVertexTagInfo.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "RecoVertex/VertexPrimitives/interface/ConvertToFromReco.h"
#include "RecoBTau/JetTagComputer/interface/JetTagComputer.h"
#include "TrackingTools/IPTools/interface/IPTools.h"
#include "fastjet/contrib/Njettiness.hh"
#include "DataFormats/BTauReco/interface/ParticleMasses.h"
#include "DataFormats/Math/interface/LorentzVector.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "RecoBTag/SecondaryVertex/interface/TrackKinematics.h"
#include "DataFormats/BTauReco/interface/BoostedDoubleSVTagInfo.h"
#include "DataFormats/Math/interface/deltaR.h"

#include <algorithm>

using namespace uhh2;
using namespace std;

bool btag_warning;

size_t uhh2::add_genpart(const reco::Candidate & jetgenp, vector<GenParticle> & genparts) {
   for(size_t j=0; j<genparts.size();j++){
     const GenParticle & sgenpart = genparts[j];
     auto r = reco::deltaR(jetgenp.eta(), jetgenp.phi(), sgenpart.eta(), sgenpart.phi());
     if (closeFloat(r, 0.0f) && closeFloat(jetgenp.pt(), sgenpart.pt())){
       return j;
     }
   }
   GenParticle genp;
   genp.set_charge(jetgenp.charge());
   genp.set_pt(jetgenp.p4().pt());
   genp.set_eta(jetgenp.p4().eta());
   genp.set_phi(jetgenp.p4().phi());
   genp.set_energy(jetgenp.p4().E());
   genp.set_index(genparts.size());
   genp.set_status(jetgenp.status());
   genp.set_pdgId(jetgenp.pdgId());

   genp.set_mother1(-1);
   genp.set_mother2(-1);
   genp.set_daughter1(-1);
   genp.set_daughter2(-1);

   genparts.push_back(genp);
   return genparts.size()-1;
}

size_t uhh2::add_pfpart(const reco::Candidate & pf, vector<PFParticle> & pfparts){
   for(size_t j=0; j<pfparts.size();j++){
     const PFParticle & spfcandart = pfparts[j];
     auto r = reco::deltaR(pf.eta(), pf.phi(), spfcandart.eta(), spfcandart.phi());
     if (closeFloat(r, 0.0f) && closeFloat(pf.pt(), spfcandart.pt())){
       return j;
     }
   }
   const pat::PackedCandidate* iter = dynamic_cast<const pat::PackedCandidate*>(&pf);
   PFParticle part;
   part.set_pt(pf.pt());
   part.set_eta(pf.eta());
   part.set_phi(pf.phi());
   part.set_energy(pf.energy());
   part.set_charge(pf.charge());
   part.set_puppiWeight(iter->puppiWeight());
   part.set_puppiWeightNoLep(iter->puppiWeightNoLep());
   PFParticle::EParticleID id = PFParticle::eX;
   reco::PFCandidate reco_pf;
   switch ( reco_pf.translatePdgIdToType(pf.pdgId()) ){
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

   pfparts.push_back(part);
   return pfparts.size()-1;
}

// Get userFloat entry, with some default return value if it doesn't exist
// TODO: template this?
float getPatJetUserFloat(const pat::Jet & jet, const std::string & key, float defaultValue=-9999.){
  return (jet.hasUserFloat(key) ? jet.userFloat(key) : defaultValue);
}

// Get userfloat by iterating through in reverse order until a label is found containing the desired key
// Returns default value otherwise
float getPatJetUserFloatMatching(const pat::Jet & jet, const std::string & key, float defaultValue=-9999.) {
  // Go reverse, as the latter entry is the latest one
  for (auto nitr=jet.userFloatNames().rbegin(); nitr != jet.userFloatNames().rend(); nitr++) {
    if (nitr->find(key) != std::string::npos) {
      return jet.userFloat(*nitr);
    }
  }
  return defaultValue;
}

// Figure out if "puppi" in name, case-independent
// Really hope the collection name doesn't have non-ASCII characters in it
bool isPuppiCollection(const std::string & name) {
  std::string nameLower = name;
  std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);
  return (nameLower.find("puppi") != std::string::npos);
}

NtupleWriterJets::NtupleWriterJets(Config & cfg, bool set_jets_member, unsigned int NPFJetwConstituents, double MinPtJetwConstituents){
    handle = cfg.ctx.declare_event_output<vector<Jet>>(cfg.dest_branchname, cfg.dest);

    ptmin = cfg.ptmin;
    etamax = cfg.etamax;
    if(set_jets_member){
        jets_handle = cfg.ctx.get_handle<vector<Jet>>("jets");
    }
    src = cfg.src;
    src_token = cfg.cc.consumes<std::vector<pat::Jet>>(cfg.src);
    l1jet_token = cfg.cc.consumes<BXVector<l1t::Jet>>(cfg.l1jet_src);
    btag_warning=true;
    save_lepton_keys_ = false;

    doPuppiSpecific = isPuppiCollection(src.label());

    h_muons.clear();
    h_elecs.clear();
    NPFJetwConstituents_ = NPFJetwConstituents;
    MinPtJetwConstituents_ = 2e6;
    if(MinPtJetwConstituents>0)
      MinPtJetwConstituents_ = MinPtJetwConstituents;
    //    auto h_pfcand = cfg.ctx.get_handle<vector<PFParticle>>("PFParticles"); h_pfcands.push_back(h_pfcand);
}

NtupleWriterJets::NtupleWriterJets(Config & cfg, bool set_jets_member, const std::vector<std::string>& muon_sources, const std::vector<std::string>& elec_sources, unsigned int NPFJetwConstituents, double MinPtJetwConstituents):
  NtupleWriterJets::NtupleWriterJets(cfg, set_jets_member, NPFJetwConstituents, MinPtJetwConstituents) {

    save_lepton_keys_ = true;

    for(const auto& muo_src : muon_sources){ auto h_muon = cfg.ctx.get_handle<std::vector<Muon>    >(muo_src); h_muons.push_back(h_muon); }
    for(const auto& ele_src : elec_sources){ auto h_elec = cfg.ctx.get_handle<std::vector<Electron>>(ele_src); h_elecs.push_back(h_elec); }
}

NtupleWriterJets::~NtupleWriterJets(){}

void NtupleWriterJets::process(const edm::Event & event, uhh2::Event & uevent,  const edm::EventSetup& iSetup){
    edm::Handle< std::vector<pat::Jet> > jet_handle;
    event.getByToken(src_token, jet_handle);
    const std::vector<pat::Jet> & pat_jets = *jet_handle;

    edm::Handle<BXVector<l1t::Jet>> l1jet_handle;
    event.getByToken(l1jet_token, l1jet_handle);

    /*--- lepton keys ---*/
    std::vector<long int> lepton_keys;
    if(save_lepton_keys_){

      for(const auto& h_muo : h_muons){

        const auto& muons = uevent.get(h_muo);

        for(const auto& muo : muons){
          for(const auto& sc : muo.source_candidates()){

            lepton_keys.push_back(sc.key);
          }
        }
      }

      for(const auto& h_ele : h_elecs){

        const auto& elecs = uevent.get(h_ele);

        for(const auto& ele : elecs){
          for(const auto& sc : ele.source_candidates()){

            lepton_keys.push_back(sc.key);
          }
        }
      }
    }
    /*-------------------*/

    std::vector<Jet> jets;
    for (unsigned int i = 0; i < pat_jets.size(); ++i) {
        const pat::Jet & pat_jet = pat_jets[i];
        if(pat_jet.pt() < ptmin) continue;
        if(fabs(pat_jet.eta()) > etamax) continue;
        jets.emplace_back();

        Jet& jet = jets.back();


        bool storePFcands = false;
        if(i<NPFJetwConstituents_ || pat_jet.pt()>MinPtJetwConstituents_) storePFcands = true;
        try {
          fill_jet_info(uevent,pat_jet, jet, true, doPuppiSpecific, storePFcands);
        }
        catch(runtime_error & ex){
          throw cms::Exception("fill_jet_info error", "Error in fill_jet_info NtupleWriterJets::process for jets with src = " + src.label());
        }

        // L1-reco matching: defaults to 10 if there's no L1 object to match
        for(const l1t::Jet & itL1 : *l1jet_handle){
          double dR_recoJet_l1Jet = reco::deltaR(pat_jet.eta(), pat_jet.phi(), itL1.p4().Eta(), itL1.p4().Phi());
          if(dR_recoJet_l1Jet < jet.minDeltaRToL1Jet()) jet.set_minDeltaRToL1Jet(dR_recoJet_l1Jet);
        }

        /*--- lepton keys ---*/
        if(save_lepton_keys_){

          const auto& jet_daughter_ptrs = pat_jet.daughterPtrVector();
          for(const auto & daughter_p : jet_daughter_ptrs){

            if(!daughter_p.isAvailable()) continue;

            const auto& key = daughter_p.key();

            if(std::find(lepton_keys.begin(), lepton_keys.end(), key) == lepton_keys.end()) continue;

            jet.add_lepton_key(key);
          }
        }
        /*-------------------*/
    }
    uevent.set(handle, move(jets));
    if(jets_handle){
        EventAccess_::set_unmanaged(uevent, *jets_handle, &uevent.get(handle));
    }
}


void NtupleWriterJets::fill_jet_info(uhh2::Event & uevent, const pat::Jet & pat_jet, Jet & jet, bool do_btagging, bool doPuppiSpecific, bool fill_pfcand){
  jet.set_charge(pat_jet.charge());
  jet.set_pt(pat_jet.pt());
  jet.set_eta(pat_jet.eta());
  jet.set_phi(pat_jet.phi());
  jet.set_energy(pat_jet.energy());
  jet.set_partonFlavour(pat_jet.partonFlavour());
  jet.set_hadronFlavour(pat_jet.hadronFlavour());
  jet.set_numberOfDaughters (pat_jet.numberOfDaughters());
  jet.set_jetArea(pat_jet.jetArea());
  if(pat_jet.hasUserFloat("pileupJetId:fullDiscriminant")) {
    jet.set_pileupID(pat_jet.userFloat("pileupJetId:fullDiscriminant"));
    bool loose = bool(pat_jet.userInt("pileupJetId:fullId") & (1 << 2));
    jet.set_tag(jet.tagname2tag("pileup_loose"), loose);
    bool medium = bool(pat_jet.userInt("pileupJetId:fullId") & (1 << 1));
    jet.set_tag(jet.tagname2tag("pileup_medium"), medium);
    bool tight = bool(pat_jet.userInt("pileupJetId:fullId") & (1 << 0));
    jet.set_tag(jet.tagname2tag("pileup_tight"), tight);
  }

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
    if (doPuppiSpecific) {
      // Do PUPPI specifics
      // We do not know the name of the producer that gets enters into the userFloat name,
      // so we look for the last entry that ends with the desired string
      jet.set_puppiMultiplicity(getPatJetUserFloatMatching(pat_jet, ":puppiMultiplicity", -1.));
      jet.set_neutralPuppiMultiplicity(getPatJetUserFloatMatching(pat_jet, ":neutralPuppiMultiplicity", -1.));
      jet.set_neutralHadronPuppiMultiplicity(getPatJetUserFloatMatching(pat_jet, ":neutralHadronPuppiMultiplicity", -1.));
      jet.set_photonPuppiMultiplicity(getPatJetUserFloatMatching(pat_jet, ":photonPuppiMultiplicity", -1.));
      jet.set_HFHadronPuppiMultiplicity(getPatJetUserFloatMatching(pat_jet, ":HFHadronPuppiMultiplicity", -1.));
      jet.set_HFEMPuppiMultiplicity(getPatJetUserFloatMatching(pat_jet, ":HFEMPuppiMultiplicity", -1.));
    }
  }

  // ensures you can still store jets without any JEC applied
  if (pat_jet.jecSetsAvailable()) {
    jet.set_JEC_factor_raw(pat_jet.jecFactor("Uncorrected"));

    //L1 factor needed for JEC propagation to MET
    const std::vector< std::string > factors_jet = pat_jet.availableJECLevels();
    bool isL1 = false;
    for(unsigned int i=0;i<factors_jet.size();i++) {
      if(factors_jet[i]=="L1FastJet") {
        isL1 = true;
        break;
      }
    }
    if(isL1) jet.set_JEC_L1factor_raw(pat_jet.correctedJet("L1FastJet").pt() / pat_jet.correctedJet("Uncorrected").pt());
    else jet.set_JEC_L1factor_raw(1.);//PUPPI jets don't have L1 factor

  } else {
    jet.set_JEC_factor_raw(1.);
    jet.set_JEC_L1factor_raw(1.);
  }

  if(do_btagging){
    const auto & bdisc = pat_jet.getPairDiscri();
    bool deepcsv_b = false, deepcsv_bb = false;
    bool deepflavour_bb=false, deepflavour_b=false, deepflavour_lepb=false, deepflavour_c=false, deepflavour_uds=false, deepflavour_g=false;
    for(const auto & name_value : bdisc){
      const auto & name = name_value.first;
      const auto & value = name_value.second;
      if(name == "pfDeepCSVJetTags:probb"){
        jet.set_btag_DeepCSV_probb(value);
        deepcsv_b = true;
      }
      else if(name == "pfDeepCSVJetTags:probbb"){
        jet.set_btag_DeepCSV_probbb(value);
        deepcsv_bb = true;
      }
      else if(name=="pfDeepFlavourJetTags:probbb"){
        jet.set_btag_DeepFlavour_probbb(value);
        deepflavour_bb =true;
      }
      else if(name=="pfDeepFlavourJetTags:probb"){
        jet.set_btag_DeepFlavour_probb(value);
        deepflavour_b =true;
      }
      else if(name=="pfDeepFlavourJetTags:problepb"){
        jet.set_btag_DeepFlavour_problepb(value);
        deepflavour_lepb =true;
      }
      else if(name=="pfDeepFlavourJetTags:probc"){
        jet.set_btag_DeepFlavour_probc(value);
        deepflavour_c =true;
      }
      else if(name=="pfDeepFlavourJetTags:probuds"){
        jet.set_btag_DeepFlavour_probuds(value);
        deepflavour_uds =true;
      }
      else if(name=="pfDeepFlavourJetTags:probg"){
        jet.set_btag_DeepFlavour_probg(value);
        deepflavour_g =true;
      }
    }



    if(!deepcsv_b || !deepcsv_bb || !deepflavour_bb || !deepflavour_b || !deepflavour_lepb || !deepflavour_uds || !deepflavour_c || !deepflavour_g){
      if(btag_warning){
        std::string btag_list = "";
        for(const auto & name_value : bdisc){
          btag_list += name_value.first;
          btag_list += " ";
        }
        edm::LogWarning("NtupleWriterJets") << "Did not find all b-taggers! Available btaggers: " << btag_list;
        btag_warning = false;
      }
      // throw runtime_error("did not find all b-taggers; see output for details");
    }
  }

  if(fill_pfcand){//fill pf candidates list: add pf-candidate to the event list and store index in the jet container
    const auto& jet_daughter_ptrs = pat_jet.daughterPtrVector();
    for(const auto & daughter_p : jet_daughter_ptrs){
      size_t pfparticles_index = add_pfpart(*daughter_p, *uevent.pfparticles);
      jet.add_pfcand_index(pfparticles_index);
    }
  }
}


NtupleWriterTopJets::NtupleWriterTopJets(Config & cfg, bool set_jets_member, unsigned int NPFJetwConstituents, double MinPtJetwConstituents): ptmin(cfg.ptmin), etamax(cfg.etamax) {
    handle = cfg.ctx.declare_event_output<vector<TopJet>>(cfg.dest_branchname, cfg.dest);
    if(set_jets_member){
        topjets_handle = cfg.ctx.get_handle<vector<TopJet>>("topjets");
    }
    src_token = cfg.cc.consumes<std::vector<pat::Jet>>(cfg.src);
    njettiness_src = cfg.njettiness_src;
    src_njettiness1_token = cfg.cc.consumes<edm::ValueMap<float> >(edm::InputTag(njettiness_src, "tau1"));
    src_njettiness2_token = cfg.cc.consumes<edm::ValueMap<float> >(edm::InputTag(njettiness_src, "tau2"));
    src_njettiness3_token = cfg.cc.consumes<edm::ValueMap<float> >(edm::InputTag(njettiness_src, "tau3"));
    src_njettiness4_token = cfg.cc.consumes<edm::ValueMap<float> >(edm::InputTag(njettiness_src, "tau4"));

    njettiness_groomed_src = cfg.njettiness_groomed_src;
    src_njettiness1_groomed_token = cfg.cc.consumes<edm::ValueMap<float> >(edm::InputTag(njettiness_groomed_src, "tau1"));
    src_njettiness2_groomed_token = cfg.cc.consumes<edm::ValueMap<float> >(edm::InputTag(njettiness_groomed_src, "tau2"));
    src_njettiness3_groomed_token = cfg.cc.consumes<edm::ValueMap<float> >(edm::InputTag(njettiness_groomed_src, "tau3"));
    src_njettiness4_groomed_token = cfg.cc.consumes<edm::ValueMap<float> >(edm::InputTag(njettiness_groomed_src, "tau4"));

    qjets_src = cfg.qjets_src;
    src_qjets_token = cfg.cc.consumes<edm::ValueMap<float> >(edm::InputTag(qjets_src, "QjetsVolatility"));

    ecf_beta1_src = cfg.ecf_beta1_src;
    src_ecf_beta1_N2_token = cfg.cc.consumes<edm::ValueMap<float> >(edm::InputTag(ecf_beta1_src, "ecfN2"));
    src_ecf_beta1_N3_token = cfg.cc.consumes<edm::ValueMap<float> >(edm::InputTag(ecf_beta1_src, "ecfN3"));
    ecf_beta2_src = cfg.ecf_beta2_src;
    src_ecf_beta2_N2_token = cfg.cc.consumes<edm::ValueMap<float> >(edm::InputTag(ecf_beta2_src, "ecfN2"));
    src_ecf_beta2_N3_token = cfg.cc.consumes<edm::ValueMap<float> >(edm::InputTag(ecf_beta2_src, "ecfN3"));

    subjet_src = cfg.subjet_src;
    higgs_src= cfg.higgs_src;

    if (cfg.toptagging_src == "") {
      do_toptagging = false;
    } else {
      do_toptagging = true;
      src_hepTopTag_token = cfg.cc.consumes<edm::View<reco::HTTTopJetTagInfo> >(edm::InputTag(cfg.toptagging_src));
    }

    softdrop_src = cfg.softdrop_src;
    if(softdrop_src.find("Mass")==string::npos){
      src_softdrop_token = cfg.cc.consumes<std::vector<pat::Jet>>(cfg.softdrop_src);
    }

    src_higgs_token = cfg.cc.consumes<std::vector<pat::Jet>>(cfg.higgs_src);
    higgs_name=cfg.higgs_name;
    src = cfg.src;
    do_btagging = cfg.do_btagging;
    do_btagging_subjets = cfg.do_btagging_subjets;
    if(!njettiness_src.empty() || !qjets_src.empty()){
        substructure_variables_src_token = cfg.cc.consumes<reco::BasicJetCollection>(cfg.substructure_variables_src);
        substructure_variables_src_tokenreco = cfg.cc.consumes<reco::PFJetCollection>(cfg.substructure_variables_src);
    }
    if(!njettiness_groomed_src.empty()){
        substructure_groomed_variables_src_token = cfg.cc.consumes<reco::BasicJetCollection>(cfg.substructure_groomed_variables_src);
        substructure_groomed_variables_src_tokenreco = cfg.cc.consumes<reco::PFJetCollection>(cfg.substructure_groomed_variables_src);
    }
    btag_warning=true;
    topjet_collection = cfg.dest_branchname;

    doPuppiSpecific = isPuppiCollection(src.label());

    save_lepton_keys_ = false;

    h_muons.clear();
    h_elecs.clear();

    higgstaginfo_src = cfg.higgstaginfo_src;
    src_higgstaginfo_token =  cfg.cc.consumes<std::vector<reco::BoostedDoubleSVTagInfo> >(cfg.higgstaginfo_src);
    NPFJetwConstituents_ = NPFJetwConstituents;
    MinPtJetwConstituents_ = 2e6;
    if(MinPtJetwConstituents>0)
      MinPtJetwConstituents_ = MinPtJetwConstituents;
}

NtupleWriterTopJets::NtupleWriterTopJets(Config & cfg, bool set_jets_member, const std::vector<std::string>& muon_sources, const std::vector<std::string>& elec_sources, unsigned int NPFJetwConstituents, double MinPtJetwConstituents):
  NtupleWriterTopJets::NtupleWriterTopJets(cfg, set_jets_member, NPFJetwConstituents, MinPtJetwConstituents) {

    save_lepton_keys_ = true;

    for(const auto& muo_src : muon_sources){ auto h_muon = cfg.ctx.get_handle<std::vector<Muon>    >(muo_src); h_muons.push_back(h_muon); }
    for(const auto& ele_src : elec_sources){ auto h_elec = cfg.ctx.get_handle<std::vector<Electron>>(ele_src); h_elecs.push_back(h_elec); }
    NPFJetwConstituents_ = NPFJetwConstituents;
}

NtupleWriterTopJets::~NtupleWriterTopJets(){}


void NtupleWriterTopJets::fill_btag_info(uhh2::Event & uevent, const pat::Jet & pat_jet, TopJet & jet){
  const auto & bdisc = pat_jet.getPairDiscri();
  bool doubleak8 = false, doubleca15 = false,
    decorrmass_deepboosted_bbvsLight=false,decorrmass_deepboosted_ccvsLight=false,decorrmass_deepboosted_TvsQCD=false,
    decorrmass_deepboosted_ZHccvsQCD=false,decorrmass_deepboosted_WvsQCD=false,decorrmass_deepboosted_ZHbbvsQCD=false,
    decorrmass_deepboosted_ZvsQCD=false,decorrmass_deepboosted_ZbbvsQCD=false,decorrmass_deepboosted_HbbvsQCD=false,
    decorrmass_deepboosted_H4qvsQCD=false,deepboosted_bbvsLight=false,deepboosted_ccvsLight=false,deepboosted_TvsQCD=false,
    deepboosted_ZHccvsQCD=false,deepboosted_WvsQCD=false,deepboosted_ZHbbvsQCD=false,
    deepboosted_ZvsQCD=false,deepboosted_ZbbvsQCD=false,deepboosted_HbbvsQCD=false,deepboosted_H4qvsQCD=false,
    deepboosted_probHbb=false,deepboosted_probQCDbb=false,deepboosted_probQCDc=false,deepboosted_probTbqq=false,
    deepboosted_probTbcq=false,deepboosted_probTbq=false,deepboosted_probQCDothers=false,deepboosted_probQCDb=false,
    deepboosted_probTbc=false,deepboosted_probWqq=false,deepboosted_probQCDcc=false,deepboosted_probHcc=false,
    deepboosted_probWcq=false,deepboosted_probZcc=false,deepboosted_probZqq=false,deepboosted_probHqqqq=false,
    deepboosted_probZbb=false, deepdouble_H=false,deepdouble_QCD=false, deepdouble_cl_H=false,deepdouble_cl_QCD=false,
    deepdouble_cb_H=false,deepdouble_cb_QCD=false, massinddeepdouble_H=false,massinddeepdouble_QCD=false,
    massinddeepdouble_cl_H=false,massinddeepdouble_cl_QCD=false, massinddeepdouble_cb_H=false,massinddeepdouble_cb_QCD=false,
    decorrmass_deepboosted_probHbb=false,decorrmass_deepboosted_probQCDbb=false,decorrmass_deepboosted_probQCDc=false,
    decorrmass_deepboosted_probTbqq=false,decorrmass_deepboosted_probTbcq=false,decorrmass_deepboosted_probTbq=false,
    decorrmass_deepboosted_probQCDothers=false,decorrmass_deepboosted_probQCDb=false,decorrmass_deepboosted_probTbc=false,
    decorrmass_deepboosted_probWqq=false,decorrmass_deepboosted_probQCDcc=false,decorrmass_deepboosted_probHcc=false,
    decorrmass_deepboosted_probWcq=false,decorrmass_deepboosted_probZcc=false,decorrmass_deepboosted_probZqq=false,
    decorrmass_deepboosted_probHqqqq=false,decorrmass_deepboosted_probZbb=false,
    particlenet_probTbcq=false,particlenet_probTbqq=false,particlenet_probTbc=false,particlenet_probTbq=false,
    particlenet_probTbel=false,particlenet_probTbmu=false,particlenet_probTbta=false,particlenet_probWcq=false,
    particlenet_probWqq=false,particlenet_probZbb=false,particlenet_probZcc=false,particlenet_probZqq=false,
    particlenet_probHbb=false,particlenet_probHcc=false,particlenet_probHqqqq=false,particlenet_probQCDbb=false,
    particlenet_probQCDcc=false,particlenet_probQCDb=false,particlenet_probQCDc=false,particlenet_probQCDothers=false,
    particlenet_TvsQCD=false,particlenet_WvsQCD=false,particlenet_ZvsQCD=false,particlenet_ZbbvsQCD=false,
    particlenet_HbbvsQCD=false,particlenet_HccvsQCD=false,particlenet_H4qvsQCD=false,
    decorrmass_particlenet_probXbb=false,decorrmass_particlenet_probXcc=false,decorrmass_particlenet_probXqq=false,decorrmass_particlenet_probQCDbb=false,
    decorrmass_particlenet_probQCDcc=false,decorrmass_particlenet_probQCDb=false,decorrmass_particlenet_probQCDc=false,decorrmass_particlenet_probQCDothers=false,
    decorrmass_particlenet_XbbvsQCD=false,decorrmass_particlenet_XccvsQCD=false,decorrmass_particlenet_XqqvsQCD=false,particlenet_mass=false;


    for(const auto & name_value : bdisc){
      const auto & name = name_value.first;
      const auto & value = name_value.second;
      if(name == "pfBoostedDoubleSecondaryVertexAK8BJetTags"){
        jet.set_btag_BoostedDoubleSecondaryVertexAK8(value);
        doubleak8 = true;
      }
      else if(name == "pfBoostedDoubleSecondaryVertexCA15BJetTags"){
        jet.set_btag_BoostedDoubleSecondaryVertexCA15(value);
        doubleca15 = true;
      }
      else if(name == "pfMassDecorrelatedDeepBoostedDiscriminatorsJetTags:bbvsLight"){
        jet.set_btag_MassDecorrelatedDeepBoosted_bbvsLight(value);
        decorrmass_deepboosted_bbvsLight=true;
      }
      else if(name == "pfMassDecorrelatedDeepBoostedDiscriminatorsJetTags:ccvsLight"){
        jet.set_btag_MassDecorrelatedDeepBoosted_ccvsLight(value);
        decorrmass_deepboosted_ccvsLight=true;
      }
      else if(name == "pfMassDecorrelatedDeepBoostedDiscriminatorsJetTags:TvsQCD"){
        jet.set_btag_MassDecorrelatedDeepBoosted_TvsQCD(value);
        decorrmass_deepboosted_TvsQCD=true;
      }
      else if(name == "pfMassDecorrelatedDeepBoostedDiscriminatorsJetTags:ZHccvsQCD"){
        jet.set_btag_MassDecorrelatedDeepBoosted_ZHccvsQCD(value);
        decorrmass_deepboosted_ZHccvsQCD=true;
      }
      else if(name == "pfMassDecorrelatedDeepBoostedDiscriminatorsJetTags:WvsQCD"){
        jet.set_btag_MassDecorrelatedDeepBoosted_WvsQCD(value);
        decorrmass_deepboosted_WvsQCD=true;
      }
      else if(name == "pfMassDecorrelatedDeepBoostedDiscriminatorsJetTags:ZHbbvsQCD"){
        jet.set_btag_MassDecorrelatedDeepBoosted_ZHbbvsQCD(value);
        decorrmass_deepboosted_ZHbbvsQCD=true;
      }
      else if(name == "pfMassDecorrelatedDeepBoostedDiscriminatorsJetTags:ZvsQCD"){
        jet.set_btag_MassDecorrelatedDeepBoosted_ZvsQCD(value);
        decorrmass_deepboosted_ZvsQCD=true;
      }
      else if(name == "pfMassDecorrelatedDeepBoostedDiscriminatorsJetTags:ZbbvsQCD"){
        jet.set_btag_MassDecorrelatedDeepBoosted_ZbbvsQCD(value);
        decorrmass_deepboosted_ZbbvsQCD=true;
      }
      else if(name == "pfMassDecorrelatedDeepBoostedDiscriminatorsJetTags:HbbvsQCD"){
        jet.set_btag_MassDecorrelatedDeepBoosted_HbbvsQCD(value);
        decorrmass_deepboosted_HbbvsQCD=true;
      }
      else if(name == "pfMassDecorrelatedDeepBoostedDiscriminatorsJetTags:H4qvsQCD"){
        jet.set_btag_MassDecorrelatedDeepBoosted_H4qvsQCD(value);
        decorrmass_deepboosted_H4qvsQCD=true;
      }
      else if(name == "pfMassDecorrelatedDeepBoostedJetTags:probHbb"){
        jet.set_btag_MassDecorrelatedDeepBoosted_probHbb(value);
        decorrmass_deepboosted_probHbb=true;
      }
      else if(name == "pfMassDecorrelatedDeepBoostedJetTags:probQCDc"){
        jet.set_btag_MassDecorrelatedDeepBoosted_probQCDc(value);
        decorrmass_deepboosted_probQCDc=true;
      }
      else if(name == "pfMassDecorrelatedDeepBoostedJetTags:probQCDbb"){
        jet.set_btag_MassDecorrelatedDeepBoosted_probQCDbb(value);
        decorrmass_deepboosted_probQCDbb=true;
      }
      else if(name == "pfMassDecorrelatedDeepBoostedJetTags:probTbqq"){
        jet.set_btag_MassDecorrelatedDeepBoosted_probTbqq(value);
        decorrmass_deepboosted_probTbqq=true;
      }
      else if(name == "pfMassDecorrelatedDeepBoostedJetTags:probTbcq"){
        jet.set_btag_MassDecorrelatedDeepBoosted_probTbcq(value);
        decorrmass_deepboosted_probTbcq=true;
      }
      else if(name == "pfMassDecorrelatedDeepBoostedJetTags:probTbq"){
        jet.set_btag_MassDecorrelatedDeepBoosted_probTbq(value);
        decorrmass_deepboosted_probTbq=true;
      }
      else if(name == "pfMassDecorrelatedDeepBoostedJetTags:probQCDothers"){
        jet.set_btag_MassDecorrelatedDeepBoosted_probQCDothers(value);
        decorrmass_deepboosted_probQCDothers=true;
      }
      else if(name == "pfMassDecorrelatedDeepBoostedJetTags:probQCDb"){
        jet.set_btag_MassDecorrelatedDeepBoosted_probQCDb(value);
        decorrmass_deepboosted_probQCDb=true;
      }
      else if(name == "pfMassDecorrelatedDeepBoostedJetTags:probTbc"){
        jet.set_btag_MassDecorrelatedDeepBoosted_probTbc(value);
        decorrmass_deepboosted_probTbc=true;
      }
      else if(name == "pfMassDecorrelatedDeepBoostedJetTags:probWqq"){
        jet.set_btag_MassDecorrelatedDeepBoosted_probWqq(value);
        decorrmass_deepboosted_probWqq=true;
      }
      else if(name == "pfMassDecorrelatedDeepBoostedJetTags:probQCDcc"){
        jet.set_btag_MassDecorrelatedDeepBoosted_probQCDcc(value);
        decorrmass_deepboosted_probQCDcc=true;
      }
      else if(name == "pfMassDecorrelatedDeepBoostedJetTags:probHbb"){
        jet.set_btag_MassDecorrelatedDeepBoosted_probHbb(value);
        decorrmass_deepboosted_probHbb=true;
      }
      else if(name == "pfMassDecorrelatedDeepBoostedJetTags:probHcc"){
        jet.set_btag_MassDecorrelatedDeepBoosted_probHcc(value);
        decorrmass_deepboosted_probHcc=true;
      }
      else if(name == "pfMassDecorrelatedDeepBoostedJetTags:probWcq"){
        jet.set_btag_MassDecorrelatedDeepBoosted_probWcq(value);
        decorrmass_deepboosted_probWcq=true;
      }
      else if(name == "pfMassDecorrelatedDeepBoostedJetTags:probZcc"){
        jet.set_btag_MassDecorrelatedDeepBoosted_probZcc(value);
        decorrmass_deepboosted_probZcc=true;
      }
      else if(name == "pfMassDecorrelatedDeepBoostedJetTags:probZqq"){
        jet.set_btag_MassDecorrelatedDeepBoosted_probZqq(value);
        decorrmass_deepboosted_probZqq=true;
      }
      else if(name == "pfMassDecorrelatedDeepBoostedJetTags:probHqqqq"){
        jet.set_btag_MassDecorrelatedDeepBoosted_probHqqqq(value);
        decorrmass_deepboosted_probHqqqq=true;
      }
      else if(name == "pfMassDecorrelatedDeepBoostedJetTags:probZbb"){
        jet.set_btag_MassDecorrelatedDeepBoosted_probZbb(value);
        decorrmass_deepboosted_probZbb=true;
      }
      else if(name=="pfDeepDoubleBvLJetTags:probHbb"){
        jet.set_btag_DeepDoubleBvLJet_probHbb(value);
        deepdouble_H = true;
      }
      else if(name=="pfDeepDoubleBvLJetTags:probQCD"){
        jet.set_btag_DeepDoubleBvLJet_probQCD(value);
        deepdouble_QCD = true;
      }
      else if(name=="pfDeepDoubleCvBJetTags:probHbb"){
        jet.set_btag_DeepDoubleCvBJet_probHbb(value);
        deepdouble_cb_H = true;
      }
      else if(name=="pfDeepDoubleCvBJetTags:probHcc"){
        jet.set_btag_DeepDoubleCvBJet_probHcc(value);
        deepdouble_cb_QCD = true;
      }
      else if(name=="pfDeepDoubleCvLJetTags:probHcc"){
        jet.set_btag_DeepDoubleCvLJet_probHcc(value);
        deepdouble_cl_H = true;
      }
      else if(name=="pfDeepDoubleCvLJetTags:probQCD"){
        jet.set_btag_DeepDoubleCvLJet_probQCD(value);
        deepdouble_cl_QCD = true;
      }
      else if(name=="pfMassIndependentDeepDoubleBvLJetTags:probHbb"){
        jet.set_btag_MassIndependentDeepDoubleBvLJet_probHbb(value);
        massinddeepdouble_H = true;
      }
      else if(name=="pfMassIndependentDeepDoubleBvLJetTags:probQCD"){
        jet.set_btag_MassIndependentDeepDoubleBvLJet_probQCD(value);
        massinddeepdouble_QCD = true;
      }
      else if(name=="pfMassIndependentDeepDoubleCvBJetTags:probHbb"){
        jet.set_btag_MassIndependentDeepDoubleCvBJet_probHbb(value);
        massinddeepdouble_cb_H = true;
      }
      else if(name=="pfMassIndependentDeepDoubleCvBJetTags:probHcc"){
        jet.set_btag_MassIndependentDeepDoubleCvBJet_probHcc(value);
        massinddeepdouble_cb_QCD = true;
      }
      else if(name=="pfMassIndependentDeepDoubleCvLJetTags:probHcc"){
        jet.set_btag_MassIndependentDeepDoubleCvLJet_probHcc(value);
        massinddeepdouble_cl_H = true;
      }
      else if(name=="pfMassIndependentDeepDoubleCvLJetTags:probQCD"){
        jet.set_btag_MassIndependentDeepDoubleCvLJet_probQCD(value);
        massinddeepdouble_cl_QCD = true;
      }
      else if(name == "pfDeepBoostedDiscriminatorsJetTags:TvsQCD"){
        jet.set_btag_DeepBoosted_TvsQCD(value);
        deepboosted_TvsQCD=true;
      }
      else if(name == "pfDeepBoostedDiscriminatorsJetTags:WvsQCD"){
        jet.set_btag_DeepBoosted_WvsQCD(value);
        deepboosted_WvsQCD=true;
      }
      else if(name == "pfDeepBoostedDiscriminatorsJetTags:ZvsQCD"){
        jet.set_btag_DeepBoosted_ZvsQCD(value);
        deepboosted_ZvsQCD=true;
      }
      else if(name == "pfDeepBoostedDiscriminatorsJetTags:ZbbvsQCD"){
        jet.set_btag_DeepBoosted_ZbbvsQCD(value);
        deepboosted_ZbbvsQCD=true;
      }
      else if(name == "pfDeepBoostedDiscriminatorsJetTags:HbbvsQCD"){
        jet.set_btag_DeepBoosted_HbbvsQCD(value);
        deepboosted_HbbvsQCD=true;
      }
      else if(name == "pfDeepBoostedDiscriminatorsJetTags:H4qvsQCD"){
        jet.set_btag_DeepBoosted_H4qvsQCD(value);
        deepboosted_H4qvsQCD=true;
      }
      else if(name == "pfDeepBoostedJetTags:probHbb"){
        jet.set_btag_DeepBoosted_probHbb(value);
        deepboosted_probHbb=true;
      }
      else if(name == "pfDeepBoostedJetTags:probQCDc"){
        jet.set_btag_DeepBoosted_probQCDc(value);
        deepboosted_probQCDc=true;
      }
      else if(name == "pfDeepBoostedJetTags:probQCDbb"){
        jet.set_btag_DeepBoosted_probQCDbb(value);
        deepboosted_probQCDbb=true;
      }
      else if(name == "pfDeepBoostedJetTags:probTbqq"){
        jet.set_btag_DeepBoosted_probTbqq(value);
        deepboosted_probTbqq=true;
      }
      else if(name == "pfDeepBoostedJetTags:probTbcq"){
        jet.set_btag_DeepBoosted_probTbcq(value);
        deepboosted_probTbcq=true;
      }
      else if(name == "pfDeepBoostedJetTags:probTbq"){
        jet.set_btag_DeepBoosted_probTbq(value);
        deepboosted_probTbq=true;
      }
      else if(name == "pfDeepBoostedJetTags:probQCDothers"){
        jet.set_btag_DeepBoosted_probQCDothers(value);
        deepboosted_probQCDothers=true;
      }
      else if(name == "pfDeepBoostedJetTags:probQCDb"){
        jet.set_btag_DeepBoosted_probQCDb(value);
        deepboosted_probQCDb=true;
      }
      else if(name == "pfDeepBoostedJetTags:probTbc"){
        jet.set_btag_DeepBoosted_probTbc(value);
        deepboosted_probTbc=true;
      }
      else if(name == "pfDeepBoostedJetTags:probWqq"){
        jet.set_btag_DeepBoosted_probWqq(value);
        deepboosted_probWqq=true;
      }
      else if(name == "pfDeepBoostedJetTags:probQCDcc"){
        jet.set_btag_DeepBoosted_probQCDcc(value);
        deepboosted_probQCDcc=true;
      }
      else if(name == "pfDeepBoostedJetTags:probHbb"){
        jet.set_btag_DeepBoosted_probHbb(value);
        deepboosted_probHbb=true;
      }
      else if(name == "pfDeepBoostedJetTags:probHcc"){
        jet.set_btag_DeepBoosted_probHcc(value);
        deepboosted_probHcc=true;
      }
      else if(name == "pfDeepBoostedJetTags:probWcq"){
        jet.set_btag_DeepBoosted_probWcq(value);
        deepboosted_probWcq=true;
      }
      else if(name == "pfDeepBoostedJetTags:probZcc"){
        jet.set_btag_DeepBoosted_probZcc(value);
        deepboosted_probZcc=true;
      }
      else if(name == "pfDeepBoostedJetTags:probZqq"){
        jet.set_btag_DeepBoosted_probZqq(value);
        deepboosted_probZqq=true;
      }
      else if(name == "pfDeepBoostedJetTags:probHqqqq"){
        jet.set_btag_DeepBoosted_probHqqqq(value);
        deepboosted_probHqqqq=true;
      }
      else if(name == "pfDeepBoostedJetTags:probZbb"){
        jet.set_btag_DeepBoosted_probZbb(value);
        deepboosted_probZbb=true;
      }
      else if(name == "pfParticleNetJetTags:probTbcq"){
        jet.set_btag_ParticleNetJetTags_probTbcq(value);
        particlenet_probTbcq = true;
      }
      else if(name == "pfParticleNetJetTags:probTbqq"){
        jet.set_btag_ParticleNetJetTags_probTbqq(value);
        particlenet_probTbqq = true;
      }
      else if(name == "pfParticleNetJetTags:probTbc"){
        jet.set_btag_ParticleNetJetTags_probTbc(value);
        particlenet_probTbc = true;
      }
      else if(name == "pfParticleNetJetTags:probTbq"){
        jet.set_btag_ParticleNetJetTags_probTbq(value);
        particlenet_probTbq = true;
      }
      else if(name == "pfParticleNetJetTags:probTbel"){
        jet.set_btag_ParticleNetJetTags_probTbel(value);
        particlenet_probTbel = true;
      }
      else if(name == "pfParticleNetJetTags:probTbmu"){
        jet.set_btag_ParticleNetJetTags_probTbmu(value);
        particlenet_probTbmu = true;
      }
      else if(name == "pfParticleNetJetTags:probTbta"){
        jet.set_btag_ParticleNetJetTags_probTbta(value);
        particlenet_probTbta = true;
      }
      else if(name == "pfParticleNetJetTags:probWcq"){
        jet.set_btag_ParticleNetJetTags_probWcq(value);
        particlenet_probWcq = true;
      }
      else if(name == "pfParticleNetJetTags:probWqq"){
        jet.set_btag_ParticleNetJetTags_probWqq(value);
        particlenet_probWqq = true;
      }
      else if(name == "pfParticleNetJetTags:probZbb"){
        jet.set_btag_ParticleNetJetTags_probZbb(value);
        particlenet_probZbb = true;
      }
      else if(name == "pfParticleNetJetTags:probZcc"){
        jet.set_btag_ParticleNetJetTags_probZcc(value);
        particlenet_probZcc = true;
      }
      else if(name == "pfParticleNetJetTags:probZqq"){
        jet.set_btag_ParticleNetJetTags_probZqq(value);
        particlenet_probZqq = true;
      }
      else if(name == "pfParticleNetJetTags:probHbb"){
        jet.set_btag_ParticleNetJetTags_probHbb(value);
        particlenet_probHbb = true;
      }
      else if(name == "pfParticleNetJetTags:probHcc"){
        jet.set_btag_ParticleNetJetTags_probHcc(value);
        particlenet_probHcc = true;
      }
      else if(name == "pfParticleNetJetTags:probHqqqq"){
        jet.set_btag_ParticleNetJetTags_probHqqqq(value);
        particlenet_probHqqqq = true;
      }
      else if(name == "pfParticleNetJetTags:probQCDbb"){
        jet.set_btag_ParticleNetJetTags_probQCDbb(value);
        particlenet_probQCDbb = true;
      }
      else if(name == "pfParticleNetJetTags:probQCDcc"){
        jet.set_btag_ParticleNetJetTags_probQCDcc(value);
        particlenet_probQCDcc = true;
      }
      else if(name == "pfParticleNetJetTags:probQCDb"){
        jet.set_btag_ParticleNetJetTags_probQCDb(value);
        particlenet_probQCDb = true;
      }
      else if(name == "pfParticleNetJetTags:probQCDc"){
        jet.set_btag_ParticleNetJetTags_probQCDc(value);
        particlenet_probQCDc = true;
      }
      else if(name == "pfParticleNetJetTags:probQCDothers"){
        jet.set_btag_ParticleNetJetTags_probQCDothers(value);
        particlenet_probQCDothers = true;
      }
      else if(name == "pfParticleNetDiscriminatorsJetTags:TvsQCD"){
        jet.set_btag_ParticleNetDiscriminatorsJetTags_TvsQCD(value);
        particlenet_TvsQCD = true;
      }
      else if(name == "pfParticleNetDiscriminatorsJetTags:WvsQCD"){
        jet.set_btag_ParticleNetDiscriminatorsJetTags_WvsQCD(value);
        particlenet_WvsQCD = true;
      }
      else if(name == "pfParticleNetDiscriminatorsJetTags:ZvsQCD"){
        jet.set_btag_ParticleNetDiscriminatorsJetTags_ZvsQCD(value);
        particlenet_ZvsQCD = true;
      }
      else if(name == "pfParticleNetDiscriminatorsJetTags:ZbbvsQCD"){
        jet.set_btag_ParticleNetDiscriminatorsJetTags_ZbbvsQCD(value);
        particlenet_ZbbvsQCD = true;
      }
      else if(name == "pfParticleNetDiscriminatorsJetTags:HbbvsQCD"){
        jet.set_btag_ParticleNetDiscriminatorsJetTags_HbbvsQCD(value);
        particlenet_HbbvsQCD = true;
      }
      else if(name == "pfParticleNetDiscriminatorsJetTags:HccvsQCD"){
        jet.set_btag_ParticleNetDiscriminatorsJetTags_HccvsQCD(value);
        particlenet_HccvsQCD = true;
      }
      else if(name == "pfParticleNetDiscriminatorsJetTags:H4qvsQCD"){
        jet.set_btag_ParticleNetDiscriminatorsJetTags_H4qvsQCD(value);
        particlenet_H4qvsQCD = true;
      }
      else if(name == "pfMassDecorrelatedParticleNetJetTags:probXbb"){
        jet.set_btag_MassDecorrelatedParticleNetJetTags_probXbb(value);
        decorrmass_particlenet_probXbb = true;
      }
      else if(name == "pfMassDecorrelatedParticleNetJetTags:probXcc"){
        jet.set_btag_MassDecorrelatedParticleNetJetTags_probXcc(value);
        decorrmass_particlenet_probXcc = true;
      }
      else if(name == "pfMassDecorrelatedParticleNetJetTags:probXqq"){
        jet.set_btag_MassDecorrelatedParticleNetJetTags_probXqq(value);
        decorrmass_particlenet_probXqq = true;
      }
      else if(name == "pfMassDecorrelatedParticleNetJetTags:probQCDbb"){
        jet.set_btag_MassDecorrelatedParticleNetJetTags_probQCDbb(value);
        decorrmass_particlenet_probQCDbb = true;
      }
      else if(name == "pfMassDecorrelatedParticleNetJetTags:probQCDcc"){
        jet.set_btag_MassDecorrelatedParticleNetJetTags_probQCDcc(value);
        decorrmass_particlenet_probQCDcc = true;
      }
      else if(name == "pfMassDecorrelatedParticleNetJetTags:probQCDb"){
        jet.set_btag_MassDecorrelatedParticleNetJetTags_probQCDb(value);
        decorrmass_particlenet_probQCDb = true;
      }
      else if(name == "pfMassDecorrelatedParticleNetJetTags:probQCDc"){
        jet.set_btag_MassDecorrelatedParticleNetJetTags_probQCDc(value);
        decorrmass_particlenet_probQCDc = true;
      }
      else if(name == "pfMassDecorrelatedParticleNetJetTags:probQCDothers"){
        jet.set_btag_MassDecorrelatedParticleNetJetTags_probQCDothers(value);
        decorrmass_particlenet_probQCDothers = true;
      }
      else if(name == "pfMassDecorrelatedParticleNetDiscriminatorsJetTags:XbbvsQCD"){
        jet.set_btag_MassDecorrelatedParticleNetDiscriminatorsJetTags_XbbvsQCD(value);
        decorrmass_particlenet_XbbvsQCD = true;
      }
      else if(name == "pfMassDecorrelatedParticleNetDiscriminatorsJetTags:XccvsQCD"){
        jet.set_btag_MassDecorrelatedParticleNetDiscriminatorsJetTags_XccvsQCD(value);
        decorrmass_particlenet_XccvsQCD = true;
      }
      else if(name == "pfMassDecorrelatedParticleNetDiscriminatorsJetTags:XqqvsQCD"){
        jet.set_btag_MassDecorrelatedParticleNetDiscriminatorsJetTags_XqqvsQCD(value);
        decorrmass_particlenet_XqqvsQCD = true;
      }
      else if(name == "pfParticleNetMassRegressionJetTags:mass"){
        jet.set_ParticleNetMassRegressionJetTags_mass(value);
        particlenet_mass=true;
      }

    }

    if(!doubleak8 || !doubleca15 || !decorrmass_deepboosted_bbvsLight || !decorrmass_deepboosted_ccvsLight
       || !decorrmass_deepboosted_TvsQCD || !decorrmass_deepboosted_ZHccvsQCD
       || !decorrmass_deepboosted_WvsQCD || !decorrmass_deepboosted_ZHbbvsQCD
       || !decorrmass_deepboosted_ZvsQCD || !decorrmass_deepboosted_ZbbvsQCD || !decorrmass_deepboosted_HbbvsQCD || !decorrmass_deepboosted_H4qvsQCD
       || !deepboosted_bbvsLight || !deepboosted_ccvsLight
       || !deepboosted_TvsQCD || !deepboosted_ZHccvsQCD
       || !deepboosted_WvsQCD || !deepboosted_ZHbbvsQCD
       || !deepboosted_ZvsQCD || !deepboosted_ZbbvsQCD || !deepboosted_HbbvsQCD || !deepboosted_H4qvsQCD
       || !deepboosted_probHbb || !deepboosted_probQCDbb || !deepboosted_probQCDc
       || !deepboosted_probTbqq || !deepboosted_probTbcq || !deepboosted_probTbq
       || !deepboosted_probQCDothers || !deepboosted_probQCDb || !deepboosted_probTbc
       || !deepboosted_probWqq || !deepboosted_probQCDcc || !deepboosted_probHcc
       || !deepboosted_probWcq || !deepboosted_probZcc || !deepboosted_probZqq
       || !deepboosted_probHqqqq || !deepboosted_probZbb
       || !deepdouble_H || !deepdouble_QCD || !deepdouble_cb_H || !deepdouble_cl_H || !deepdouble_cl_QCD  || !deepdouble_cb_QCD
       || !massinddeepdouble_QCD || !massinddeepdouble_H || !massinddeepdouble_cb_H || !massinddeepdouble_cl_H || !massinddeepdouble_cl_QCD  || !massinddeepdouble_cb_QCD
       || !decorrmass_deepboosted_probHbb || !decorrmass_deepboosted_probQCDbb
       || !decorrmass_deepboosted_probQCDc || !decorrmass_deepboosted_probTbqq
       || !decorrmass_deepboosted_probTbcq || !decorrmass_deepboosted_probTbq
       || !decorrmass_deepboosted_probQCDothers || !decorrmass_deepboosted_probQCDb
       || !decorrmass_deepboosted_probTbc || !decorrmass_deepboosted_probWqq
       || !decorrmass_deepboosted_probQCDcc || !decorrmass_deepboosted_probHcc
       || !decorrmass_deepboosted_probWcq || !decorrmass_deepboosted_probZcc
       || !decorrmass_deepboosted_probZqq || !decorrmass_deepboosted_probHqqqq
       || !decorrmass_deepboosted_probZbb
       || !particlenet_probTbcq || !particlenet_probTbqq || !particlenet_probTbc || !particlenet_probTbq
       || !particlenet_probTbel || !particlenet_probTbmu || !particlenet_probTbta || !particlenet_probWcq
       || !particlenet_probWqq || !particlenet_probZbb || !particlenet_probZcc || !particlenet_probZqq
       || !particlenet_probHbb || !particlenet_probHcc || !particlenet_probHqqqq || !particlenet_probQCDbb
       || !particlenet_probQCDcc || !particlenet_probQCDb || !particlenet_probQCDc || !particlenet_probQCDothers
       || !particlenet_TvsQCD || !particlenet_WvsQCD || !particlenet_ZvsQCD || !particlenet_ZbbvsQCD
       || !particlenet_HbbvsQCD || !particlenet_HccvsQCD || !particlenet_H4qvsQCD
       || !decorrmass_particlenet_probXbb || !decorrmass_particlenet_probXcc || !decorrmass_particlenet_probXqq || !decorrmass_particlenet_probQCDbb
       || !decorrmass_particlenet_probQCDcc || !decorrmass_particlenet_probQCDb || !decorrmass_particlenet_probQCDc || !decorrmass_particlenet_probQCDothers
       || !decorrmass_particlenet_XbbvsQCD || !decorrmass_particlenet_XccvsQCD || !decorrmass_particlenet_XqqvsQCD || !particlenet_mass){
      if(btag_warning){
        std::string btag_list = "";
        for(const auto & name_value : bdisc){
          btag_list += name_value.first;
          btag_list += " ";
        }
        edm::LogWarning("NtupleWriterTopJets") << "Did not find all b-taggers! Available btaggers: " << btag_list;
        btag_warning = false;
      }
    }

}



void NtupleWriterTopJets::process(const edm::Event & event, uhh2::Event & uevent, const edm::EventSetup& iSetup){

    bool checkjettype =0;
    bool checkjettypegroomed =0;

    edm::Handle<pat::JetCollection> h_pat_topjets;
    event.getByToken(src_token, h_pat_topjets);
    const vector<pat::Jet> & pat_topjets = *h_pat_topjets;

    edm::Handle<edm::ValueMap<float>> h_njettiness1, h_njettiness2, h_njettiness3, h_njettiness4;
    edm::Handle<edm::ValueMap<float>> h_njettiness1_groomed, h_njettiness2_groomed, h_njettiness3_groomed, h_njettiness4_groomed;
    edm::Handle<edm::ValueMap<float>> h_qjets;
    edm::Handle<edm::ValueMap<float>> h_ecf_beta1_N2, h_ecf_beta1_N3, h_ecf_beta2_N2, h_ecf_beta2_N3;

    edm::Handle<reco::BasicJetCollection> topjets_with_cands;
    edm::Handle<reco::PFJetCollection> topjets_with_cands_reco;

    edm::Handle<reco::BasicJetCollection> topjets_groomed_with_cands;
    edm::Handle<reco::PFJetCollection> topjets_groomed_with_cands_reco;

    if(!njettiness_src.empty()){
        event.getByToken(src_njettiness1_token, h_njettiness1);
        event.getByToken(src_njettiness2_token, h_njettiness2);
        event.getByToken(src_njettiness3_token, h_njettiness3);
        event.getByToken(src_njettiness4_token, h_njettiness4);
    }
    if(!njettiness_groomed_src.empty()){
        event.getByToken(src_njettiness1_groomed_token, h_njettiness1_groomed);
        event.getByToken(src_njettiness2_groomed_token, h_njettiness2_groomed);
        event.getByToken(src_njettiness3_groomed_token, h_njettiness3_groomed);
        event.getByToken(src_njettiness4_groomed_token, h_njettiness4_groomed);
    }
    if(!qjets_src.empty()){
        event.getByToken(src_qjets_token, h_qjets);
    }
    if(!njettiness_src.empty() || !qjets_src.empty()){
      checkjettype = event.getByToken(substructure_variables_src_tokenreco, topjets_with_cands_reco);
      if(checkjettype){
        event.getByToken(substructure_variables_src_tokenreco, topjets_with_cands_reco);
      }
      else{
        event.getByToken(substructure_variables_src_token, topjets_with_cands);
      }
    }
    if(!njettiness_groomed_src.empty()){
      checkjettypegroomed = event.getByToken(substructure_groomed_variables_src_tokenreco, topjets_groomed_with_cands_reco);
      if(checkjettypegroomed){
        event.getByToken(substructure_groomed_variables_src_tokenreco, topjets_groomed_with_cands_reco);
      }
      else{
        event.getByToken(substructure_groomed_variables_src_token, topjets_groomed_with_cands);
      }
    }

    if (!ecf_beta1_src.empty()) {
      event.getByToken(src_ecf_beta1_N2_token, h_ecf_beta1_N2);
      event.getByToken(src_ecf_beta1_N3_token, h_ecf_beta1_N3);
    }
    if (!ecf_beta2_src.empty()) {
      event.getByToken(src_ecf_beta2_N2_token, h_ecf_beta2_N2);
      event.getByToken(src_ecf_beta2_N3_token, h_ecf_beta2_N3);
    }

    vector<TopJet> topjets;
    edm::Handle<edm::View<reco::HTTTopJetTagInfo>> top_jet_infos;
    if (do_toptagging) {
      event.getByToken(src_hepTopTag_token, top_jet_infos);
      assert(pat_topjets.size()==top_jet_infos->size());
    }

    /*--- lepton keys ---*/
    std::vector<long int> lepton_keys;
    if(save_lepton_keys_){

      for(const auto& h_muo : h_muons){

        const auto& muons = uevent.get(h_muo);

        for(const auto& muo : muons){
          for(const auto& sc : muo.source_candidates()){

            lepton_keys.push_back(sc.key);
          }
        }
      }

      for(const auto& h_ele : h_elecs){

        const auto& elecs = uevent.get(h_ele);

        for(const auto& ele : elecs){
          for(const auto& sc : ele.source_candidates()){

            lepton_keys.push_back(sc.key);
          }
        }
      }
    }
    /*-------------------*/

    for (unsigned int i = 0; i < pat_topjets.size(); i++) {
        const pat::Jet & pat_topjet =  pat_topjets[i];
        if(pat_topjet.pt() < ptmin) continue;
        if(fabs(pat_topjet.eta()) > etamax) continue;

        topjets.emplace_back();
        TopJet & topjet = topjets.back();
        bool storePFcands = false;
        if(i<NPFJetwConstituents_ || pat_topjet.pt()>MinPtJetwConstituents_) storePFcands = true;
        try{
          uhh2::NtupleWriterJets::fill_jet_info(uevent,pat_topjet, topjet, do_btagging, doPuppiSpecific, storePFcands);
        }catch(runtime_error &){
          throw cms::Exception("fill_jet_info error", "Error in fill_jet_info for topjets in NtupleWriterTopJets with src = " + src.label());
        }
        try{
          fill_btag_info(uevent,pat_topjet, topjet);
        }catch(runtime_error &){
          throw cms::Exception("fill_btag_info error", "Error in fill_btag_info for topjets in NtupleWriterTopJets with src = " + src.label());
        }

        /*--- lepton keys ---*/
        if(save_lepton_keys_){

          const auto& jet_daughter_ptrs = pat_topjet.daughterPtrVector();
          for(const auto & daughter_p : jet_daughter_ptrs){

            if(!daughter_p.isAvailable()) continue;

            const auto& key = daughter_p.key();

            if(std::find(lepton_keys.begin(), lepton_keys.end(), key) == lepton_keys.end()) continue;

            topjet.add_lepton_key(key);
          }
        }
        /*-------------------*/

        /*--- Njettiness/Qjets/ECFs ------*/

	// Take Njettiness/Qjets/ECFs from matched jet collection if respective source is specified.
        // Since these variables are stored in ValueMaps, we need to figure out the correct
        // reference Jet to access the value. We do this by deltaR matching

        // match a unpruned jet according to topjets_with_cands:
        int i_pat_topjet_wc = -1;
        if(!njettiness_src.empty() || !qjets_src.empty()){
          double drmin = numeric_limits<double>::infinity();
          if(checkjettype){
            for (size_t i_wc=0; i_wc < topjets_with_cands_reco->size(); ++i_wc) {
              auto dr = reco::deltaR((*topjets_with_cands_reco)[i_wc], pat_topjet);
              if(dr < drmin){
                i_pat_topjet_wc = i_wc;
                drmin = dr;
              }
            }
          }
          else{
            for (size_t i_wc=0; i_wc < topjets_with_cands->size(); ++i_wc) {
              auto dr = reco::deltaR((*topjets_with_cands)[i_wc], pat_topjet);
              if(dr < drmin){
                i_pat_topjet_wc = i_wc;
                drmin = dr;
              }
            }
          }
          if (i_pat_topjet_wc >= 0 && drmin < 1.0){ // be genereous: pruning can change jet axis quite a lot (esp. for DR=1.5 jets as in heptoptag)
            if(checkjettype){
              auto ref = edm::Ref<reco::PFJetCollection>(topjets_with_cands_reco, i_pat_topjet_wc);
              if(!njettiness_src.empty()){
                topjet.set_tau1((*h_njettiness1)[ref]);
                topjet.set_tau2((*h_njettiness2)[ref]);
                topjet.set_tau3((*h_njettiness3)[ref]);
                topjet.set_tau4((*h_njettiness4)[ref]);
              }
              if(!qjets_src.empty()){
                topjet.set_qjets_volatility((*h_qjets)[ref]);
              }
            }
            else{
              auto ref = edm::Ref<reco::BasicJetCollection>(topjets_with_cands, i_pat_topjet_wc);
              if(!njettiness_src.empty()){
                topjet.set_tau1((*h_njettiness1)[ref]);
                topjet.set_tau2((*h_njettiness2)[ref]);
                topjet.set_tau3((*h_njettiness3)[ref]);
                topjet.set_tau4((*h_njettiness4)[ref]);
              }
              if(!qjets_src.empty()){
                topjet.set_qjets_volatility((*h_qjets)[ref]);
              }
            }
          }
        }

        int i_pat_topjet_wc_groomed = -1;
        if(!njettiness_groomed_src.empty() || !ecf_beta1_src.empty() || !ecf_beta2_src.empty()){
          double drmin = numeric_limits<double>::infinity();
          if(checkjettypegroomed){
            for (size_t i_wc=0; i_wc < topjets_groomed_with_cands_reco->size(); ++i_wc) {
              auto dr = reco::deltaR((*topjets_groomed_with_cands_reco)[i_wc], pat_topjet);
              if(dr < drmin){
                i_pat_topjet_wc_groomed = i_wc;
                drmin = dr;
              }
            }
          }
          else{
            for (size_t i_wc=0; i_wc < topjets_groomed_with_cands->size(); ++i_wc) {
              auto dr = reco::deltaR((*topjets_groomed_with_cands)[i_wc], pat_topjet);
              if(dr < drmin){
                i_pat_topjet_wc_groomed = i_wc;
                drmin = dr;
              }
            }
          }
          if (i_pat_topjet_wc_groomed >= 0 && drmin < 1.0){ // be genereous: pruning can change jet axis quite a lot (esp. for DR=1.5 jets as in heptoptag)
            if(checkjettypegroomed){
              auto ref = edm::Ref<reco::PFJetCollection>(topjets_groomed_with_cands_reco, i_pat_topjet_wc_groomed);
              if(!njettiness_groomed_src.empty()){
                topjet.set_tau1_groomed((*h_njettiness1_groomed)[ref]);
                topjet.set_tau2_groomed((*h_njettiness2_groomed)[ref]);
                topjet.set_tau3_groomed((*h_njettiness3_groomed)[ref]);
                topjet.set_tau4_groomed((*h_njettiness4_groomed)[ref]);
              }
              // ECF are for groomed jets only
              if(!ecf_beta1_src.empty()){
                topjet.set_ecfN2_beta1((*h_ecf_beta1_N2)[ref]);
                topjet.set_ecfN3_beta1((*h_ecf_beta1_N3)[ref]);
              }
              if(!ecf_beta2_src.empty()){
                topjet.set_ecfN2_beta2((*h_ecf_beta2_N2)[ref]);
                topjet.set_ecfN3_beta2((*h_ecf_beta2_N3)[ref]);
              }

            }
            else{
              auto ref = edm::Ref<reco::BasicJetCollection>(topjets_groomed_with_cands, i_pat_topjet_wc_groomed);
              if(!njettiness_groomed_src.empty()){
                topjet.set_tau1_groomed((*h_njettiness1_groomed)[ref]);
                topjet.set_tau2_groomed((*h_njettiness2_groomed)[ref]);
                topjet.set_tau3_groomed((*h_njettiness3_groomed)[ref]);
                topjet.set_tau4_groomed((*h_njettiness4_groomed)[ref]);
              }
              if(!ecf_beta1_src.empty()){
                topjet.set_ecfN2_beta1((*h_ecf_beta1_N2)[ref]);
                topjet.set_ecfN3_beta1((*h_ecf_beta1_N3)[ref]);
              }
              if(!ecf_beta2_src.empty()){
                topjet.set_ecfN2_beta2((*h_ecf_beta2_N2)[ref]);
                topjet.set_ecfN3_beta2((*h_ecf_beta2_N3)[ref]);
              }
            }
          }
        }

        /*--- HEP Top Tagger variables -----*/

        if (do_toptagging)
        {
          // The HTTTopJetTagInfo vector is produced *not* sorted by jet pT.
          // The patJetProducer that makes our main TopJets *does* sort by pT.
          // Thus we lose which Jet matches with which HTTTopJetTagInfo object,
          // so here we rematch based on deltaR.
          // Note that we do not require == 0, since the fjPt/Eta/Phi is for the
          // initial fat jet, whilst the topjet is the Top jet candidate
          // i.e. sum of subjets 4-vectors. Thus we don't expect eta/phi to match.
          uint closest_ind(0);
          double minDR(999999);
          for (uint itt=0; itt < top_jet_infos->size(); itt++) {
            const reco::HTTTopJetTagInfo& jet_info = top_jet_infos->at(itt);
            double dr = reco::deltaR(topjet.eta(), topjet.phi(), jet_info.properties().fjEta, jet_info.properties().fjPhi);
            if (dr < minDR) {
              minDR = dr;
              closest_ind = itt;
            }
          }

          const reco::HTTTopJetTagInfo& jet_info = top_jet_infos->at(closest_ind);
          topjet.set_tag(TopJet::tagname2tag("fRec"), jet_info.properties().fRec);
          topjet.set_tag(TopJet::tagname2tag("Ropt"), jet_info.properties().ropt);
          topjet.set_tag(TopJet::tagname2tag("massRatioPassed"), jet_info.properties().massRatioPassed);
          topjet.set_tag(TopJet::tagname2tag("mass"),pat_topjet.mass());
          topjet.set_tag(TopJet::tagname2tag("RoptCalc"), jet_info.properties().roptCalc);
          topjet.set_tag(TopJet::tagname2tag("ptForRoptCalc"), jet_info.properties().ptForRoptCalc);
        }

        /*--- Njettiness ------*/
        if(njettiness_src.empty()){

          topjet.set_tau1(getPatJetUserFloat(pat_topjet, "NjettinessAK8Puppi:tau1", -1.));
          topjet.set_tau2(getPatJetUserFloat(pat_topjet, "NjettinessAK8Puppi:tau2", -1.));
          topjet.set_tau3(getPatJetUserFloat(pat_topjet, "NjettinessAK8Puppi:tau3", -1.));
          topjet.set_tau4(getPatJetUserFloat(pat_topjet, "NjettinessAK8Puppi:tau4", -1.));
        }
        if(njettiness_groomed_src.empty()){
          // as miniaod doesn't calculate groomed tau_i
          topjet.set_tau1_groomed(-1.);
          topjet.set_tau2_groomed(-1.);
          topjet.set_tau3_groomed(-1.);
          topjet.set_tau4_groomed(-1.);
        }
        /*---------------------*/

        /*--- energy correlation functions -----*/
        if (ecf_beta1_src.empty()) {
          topjet.set_ecfN2_beta1(getPatJetUserFloat(pat_topjet, "ak8PFJetsPuppiSoftDropValueMap:nb1AK8PuppiSoftDropN2", -1.));
          topjet.set_ecfN3_beta1(getPatJetUserFloat(pat_topjet, "ak8PFJetsPuppiSoftDropValueMap:nb1AK8PuppiSoftDropN3", -1.));
        }

        if (ecf_beta2_src.empty()) {
          topjet.set_ecfN2_beta2(getPatJetUserFloat(pat_topjet, "ak8PFJetsPuppiSoftDropValueMap:nb2AK8PuppiSoftDropN2", -1.));
          topjet.set_ecfN3_beta2(getPatJetUserFloat(pat_topjet, "ak8PFJetsPuppiSoftDropValueMap:nb2AK8PuppiSoftDropN3", -1.));
        }
        /*---------------------*/

        /*--- softdrop mass ---*/
        if(softdrop_src.find("Mass")!=string::npos){

          topjet.set_softdropmass(getPatJetUserFloat(pat_topjet, softdrop_src, -1.));
        }
        else if(softdrop_src!=""){//softdrop mass set through matching with softdrop-jet collection

          edm::Handle<pat::JetCollection> softdrop_pat_topjets;
          event.getByToken(src_softdrop_token, softdrop_pat_topjets);
          const vector<pat::Jet> & pat_softdropjets = *softdrop_pat_topjets;

          //match a jet from softdrop collection
          int i_pat_softdropjet = -1;
          double drmin = numeric_limits<double>::infinity();
          for (unsigned int ih = 0; ih < pat_softdropjets.size(); ih++) {

            const pat::Jet & softdrop_jet = pat_softdropjets[ih];
            auto dr = reco::deltaR(softdrop_jet, pat_topjet);
            if(dr < drmin){
              i_pat_softdropjet = ih;
              drmin = dr;
            }
          }

          if(i_pat_softdropjet >= 0 && drmin < 1.0){

            const pat::Jet & softdrop_jet = pat_softdropjets[i_pat_softdropjet];
            topjet.set_softdropmass(softdrop_jet.mass());
          }
        }
        /*---------------------*/

        /*--- Higgs tagging ---*/

	if(higgs_src!=""){

	  edm::Handle<pat::JetCollection> higgs_pat_topjets;
	  event.getByToken(src_higgs_token, higgs_pat_topjets);
	  const vector<pat::Jet> & pat_higgsjets = *higgs_pat_topjets;

	  //match a jet from "higgs" collection
	  int i_pat_higgsjet = -1;
	  double drmin = numeric_limits<double>::infinity();
	  for (unsigned int ih = 0; ih < pat_higgsjets.size(); ih++) {
	    const pat::Jet & higgs_jet = pat_higgsjets[ih];
	    auto dr = reco::deltaR(higgs_jet, pat_topjet);
	    if(dr < drmin){
	      i_pat_higgsjet = ih;
	      drmin = dr;
	    }
	  }


	  if (i_pat_higgsjet >= 0 && drmin < 1.0){
	    const pat::Jet & higgs_jet = pat_higgsjets[i_pat_higgsjet];
	    topjet.set_mvahiggsdiscr(higgs_jet.bDiscriminator(higgs_name.c_str()));

	  }


	}//higgs tag loop

        /*---------------------*/

        /*--- Higgs tagging commissioning ---*/

	//fill Higgs tagging info for commissioning
	if(higgstaginfo_src!=""){
	  edm::Handle<std::vector<reco::BoostedDoubleSVTagInfo> > svTagInfos;
	  event.getByToken(src_higgstaginfo_token, svTagInfos);

	  //find taginfo belonging to a jet closest to the studied jet
	  int i_svjet = -1;
	  double drmin = numeric_limits<double>::infinity();
	  for (unsigned int ih = 0; ih < svTagInfos->size(); ih++) {
	    const reco::BoostedDoubleSVTagInfo & svTagInfo = svTagInfos->at(ih);
	    const reco::JetBaseRef jet = svTagInfo.jet();
	    auto dr = reco::deltaR(jet->p4(), pat_topjet);
	    if(dr < drmin){
	      i_svjet = ih;
	      drmin = dr;
	    }
	  }

	  if(i_svjet>=0 && drmin<1.0){
	    const reco::BoostedDoubleSVTagInfo & svTagInfo = svTagInfos->at(i_svjet);
	    const reco::JetBaseRef jet = svTagInfo.jet();

	    topjet.set_tag(TopJet::tagname2tag("z_ratio"), svTagInfo.taggingVariables().get(reco::btau::TaggingVariableName::z_ratio));
	    topjet.set_tag(TopJet::tagname2tag("trackSipdSig_3"), svTagInfo.taggingVariables().get(reco::btau::TaggingVariableName::trackSip3dSig_3));
	    topjet.set_tag(TopJet::tagname2tag("trackSipdSig_2"), svTagInfo.taggingVariables().get(reco::btau::TaggingVariableName::trackSip3dSig_2));
	    topjet.set_tag(TopJet::tagname2tag("trackSipdSig_1"), svTagInfo.taggingVariables().get(reco::btau::TaggingVariableName::trackSip3dSig_1));
	    topjet.set_tag(TopJet::tagname2tag("trackSipdSig_0"), svTagInfo.taggingVariables().get(reco::btau::TaggingVariableName::trackSip3dSig_0));
	    topjet.set_tag(TopJet::tagname2tag("trackSipdSig_1_0"), svTagInfo.taggingVariables().get(reco::btau::TaggingVariableName::tau2_trackSip3dSig_0));
	    topjet.set_tag(TopJet::tagname2tag("trackSipdSig_0_0"), svTagInfo.taggingVariables().get(reco::btau::TaggingVariableName::tau1_trackSip3dSig_0));
	    topjet.set_tag(TopJet::tagname2tag("trackSipdSig_1_1"), svTagInfo.taggingVariables().get(reco::btau::TaggingVariableName::tau2_trackSip3dSig_1));
	    topjet.set_tag(TopJet::tagname2tag("trackSipdSig_0_1"), svTagInfo.taggingVariables().get(reco::btau::TaggingVariableName::tau1_trackSip3dSig_1));
	    topjet.set_tag(TopJet::tagname2tag("trackSip2dSigAboveCharm_0"), svTagInfo.taggingVariables().get(reco::btau::TaggingVariableName::trackSip2dSigAboveCharm));
	    topjet.set_tag(TopJet::tagname2tag("trackSip2dSigAboveBottom_0"), svTagInfo.taggingVariables().get(reco::btau::TaggingVariableName::trackSip2dSigAboveBottom_0));
	    topjet.set_tag(TopJet::tagname2tag("trackSip2dSigAboveBottom_1"), svTagInfo.taggingVariables().get(reco::btau::TaggingVariableName::trackSip2dSigAboveBottom_1));
	    topjet.set_tag(TopJet::tagname2tag("tau1_trackEtaRel_0"), svTagInfo.taggingVariables().get(reco::btau::TaggingVariableName::tau2_trackEtaRel_0));
	    topjet.set_tag(TopJet::tagname2tag("tau1_trackEtaRel_1"), svTagInfo.taggingVariables().get(reco::btau::TaggingVariableName::tau2_trackEtaRel_1));
	    topjet.set_tag(TopJet::tagname2tag("tau1_trackEtaRel_2"), svTagInfo.taggingVariables().get(reco::btau::TaggingVariableName::tau2_trackEtaRel_2));
	    topjet.set_tag(TopJet::tagname2tag("tau0_trackEtaRel_0"), svTagInfo.taggingVariables().get(reco::btau::TaggingVariableName::tau1_trackEtaRel_0));
	    topjet.set_tag(TopJet::tagname2tag("tau0_trackEtaRel_1"), svTagInfo.taggingVariables().get(reco::btau::TaggingVariableName::tau1_trackEtaRel_1));
	    topjet.set_tag(TopJet::tagname2tag("tau0_trackEtaRel_2"), svTagInfo.taggingVariables().get(reco::btau::TaggingVariableName::tau1_trackEtaRel_2));
	    topjet.set_tag(TopJet::tagname2tag("tau_vertexMass_0"), svTagInfo.taggingVariables().get(reco::btau::TaggingVariableName::tau1_vertexMass));
	    topjet.set_tag(TopJet::tagname2tag("tau_vertexEnergyRatio_0"), svTagInfo.taggingVariables().get(reco::btau::TaggingVariableName::tau1_vertexEnergyRatio));
	    topjet.set_tag(TopJet::tagname2tag("tau_vertexDeltaR_0"), svTagInfo.taggingVariables().get(reco::btau::TaggingVariableName::tau1_vertexDeltaR));
	    topjet.set_tag(TopJet::tagname2tag("tau_flightDistance2dSig_0"), svTagInfo.taggingVariables().get(reco::btau::TaggingVariableName::tau1_flightDistance2dSig));
	    topjet.set_tag(TopJet::tagname2tag("tau_vertexMass_1"), svTagInfo.taggingVariables().get(reco::btau::TaggingVariableName::tau2_vertexMass));
	    topjet.set_tag(TopJet::tagname2tag("tau_vertexEnergyRatio_1"), svTagInfo.taggingVariables().get(reco::btau::TaggingVariableName::tau2_vertexEnergyRatio));
	    topjet.set_tag(TopJet::tagname2tag("tau_flightDistance2dSig_1"), svTagInfo.taggingVariables().get(reco::btau::TaggingVariableName::tau2_flightDistance2dSig));
	    topjet.set_tag(TopJet::tagname2tag("jetNTracks"), svTagInfo.taggingVariables().get(reco::btau::TaggingVariableName::jetNTracks));
	    topjet.set_tag(TopJet::tagname2tag("nSV"), svTagInfo.taggingVariables().get(reco::btau::TaggingVariableName::jetNSecondaryVertices));

	  }

	}

        /*---------------------*/

        // loop over subjets to fill some more subjet info:
	//	bool storePFcands = false;
	//	if(i<NPFJetwConstituents_) storePFcands = true;

	if(subjet_src=="daughters"){
	  for (unsigned int k = 0; k < pat_topjet.numberOfDaughters(); k++) {
            Jet subjet;
            auto patsubjetd = dynamic_cast<const pat::Jet *>(pat_topjet.daughter(k));
            if (patsubjetd) {
	      try{
		NtupleWriterJets::fill_jet_info(uevent,*patsubjetd, subjet, do_btagging_subjets, doPuppiSpecific, storePFcands);
	      }catch(runtime_error &){
                throw cms::Exception("fill_jet_info error", "Error in fill_jet_info for daughters in NtupleWriterTopJets with src = " + src.label());
	      }
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
		throw cms::Exception("NullSubjet", "daughter is nullptr");
	      }
            }
            topjet.add_subjet(subjet);
	  }
	}//if label daughters
	//taking subjets from existing miniAOD collection
	else{
	  auto tSubjets = pat_topjet.subjets(subjet_src);
	  for( int sj = 0; sj < (int)tSubjets.size(); ++sj ){
	    Jet subjet;
	    auto tpatsubjet = dynamic_cast<const pat::Jet *>(tSubjets.at(sj).get());
            if (tpatsubjet) {
	      try{
		NtupleWriterJets::fill_jet_info(uevent,*tpatsubjet, subjet, do_btagging_subjets, doPuppiSpecific, storePFcands);
	      }catch(runtime_error &){
                throw cms::Exception("fill_jet_info error", "Error in fill_jet_info for subjets in NtupleWriterTopJets with src = " + src.label());
	      }

              /*--- lepton keys ---*/
              if(save_lepton_keys_){

                const auto& jet_daughter_ptrs = tpatsubjet->daughterPtrVector();
                for(const auto & daughter_p : jet_daughter_ptrs){

                  if(!daughter_p.isAvailable()) continue;

                  const auto& key = daughter_p.key();

                  if(std::find(lepton_keys.begin(), lepton_keys.end(), key) == lepton_keys.end()) continue;

                  subjet.add_lepton_key(key);
                }
              }
              /*-------------------*/
            }
	    else{
              throw cms::Exception("MissingSubjet", "Cannot get subjets from topjet src = " + src.label());
	    }
	    topjet.add_subjet(subjet);
	  }//loop over subjets
	}//if not daughters but subjets from miniAOD collection
    }// for topjets
    uevent.set(handle, move(topjets));
    if(topjets_handle){
        EventAccess_::set_unmanaged(uevent, *topjets_handle, &uevent.get(handle));
    }
}



NtupleWriterGenTopJets::NtupleWriterGenTopJets(Config & cfg, bool set_jets_member, unsigned int NGenJetwConstituents, double MinPtJetwConstituents):
ptmin(cfg.ptmin),etamax(cfg.etamax)
{
  handle = cfg.ctx.declare_event_output<vector<GenTopJet>>(cfg.dest_branchname, cfg.dest);
  if(set_jets_member){
    gentopjets_handle = cfg.ctx.get_handle<vector<GenTopJet>>("gentopjets");
  }
  src_token = cfg.cc.consumes<std::vector<reco::GenJet>>(cfg.src);

  subjet_src = cfg.subjet_src;
  subjet_src_token = cfg.cc.consumes<std::vector<reco::BasicJet>>(cfg.subjet_src);

  njettiness_src = cfg.njettiness_src;
  src_njettiness1_token = cfg.cc.consumes<edm::ValueMap<float> >(edm::InputTag(njettiness_src, "tau1"));
  src_njettiness2_token = cfg.cc.consumes<edm::ValueMap<float> >(edm::InputTag(njettiness_src, "tau2"));
  src_njettiness3_token = cfg.cc.consumes<edm::ValueMap<float> >(edm::InputTag(njettiness_src, "tau3"));
  src_njettiness4_token = cfg.cc.consumes<edm::ValueMap<float> >(edm::InputTag(njettiness_src, "tau4"));

  ecf_beta1_src = cfg.ecf_beta1_src;
  src_ecf_beta1_N2_token = cfg.cc.consumes<edm::ValueMap<float> >(edm::InputTag(ecf_beta1_src, "ecfN2"));
  src_ecf_beta1_N3_token = cfg.cc.consumes<edm::ValueMap<float> >(edm::InputTag(ecf_beta1_src, "ecfN3"));

  ecf_beta2_src = cfg.ecf_beta2_src;
  src_ecf_beta2_N2_token = cfg.cc.consumes<edm::ValueMap<float> >(edm::InputTag(ecf_beta2_src, "ecfN2"));
  src_ecf_beta2_N3_token = cfg.cc.consumes<edm::ValueMap<float> >(edm::InputTag(ecf_beta2_src, "ecfN3"));

  useSubstructureVar = false;
  if(!njettiness_src.empty() || !ecf_beta1_src.empty() || !ecf_beta2_src.empty()){
    useSubstructureVar = true;
    substructure_variables_src_token_basic = cfg.cc.consumes<reco::BasicJetCollection>(cfg.substructure_variables_src);
    substructure_variables_src_token_gen = cfg.cc.consumes<reco::GenJetCollection>(cfg.substructure_variables_src);
  }

  gentopjet_collection = cfg.dest_branchname;

  NGenJetwConstituents_ = NGenJetwConstituents;
  MinPtJetwConstituents_ = 2e4;
  if(MinPtJetwConstituents>0)
    MinPtJetwConstituents_ = MinPtJetwConstituents;
}


NtupleWriterGenTopJets::~NtupleWriterGenTopJets(){}

void NtupleWriterGenTopJets::process(const edm::Event & event, uhh2::Event & uevent, const edm::EventSetup& iSetup){


  edm::Handle<reco::GenJetCollection> h_reco_gentopjets;
  event.getByToken(src_token, h_reco_gentopjets);
  const vector<reco::GenJet> & reco_gentopjets = *h_reco_gentopjets;

  edm::Handle<reco::BasicJetCollection> h_reco_basicgenjets;
  if (!subjet_src.empty()) {
    event.getByToken(subjet_src_token, h_reco_basicgenjets);
  }

  edm::Handle<edm::ValueMap<float>> h_njettiness1, h_njettiness2, h_njettiness3, h_njettiness4;
  edm::Handle<edm::ValueMap<float>> h_ecf_beta1_N2, h_ecf_beta1_N3, h_ecf_beta2_N2, h_ecf_beta2_N3;

  edm::Handle<reco::BasicJetCollection> gentopjets_with_cands_basic;
  edm::Handle<reco::GenJetCollection> gentopjets_with_cands_gen;

  bool jetTypeIsGenJet = false;
  if (useSubstructureVar) {
    jetTypeIsGenJet = event.getByToken(substructure_variables_src_token_gen, gentopjets_with_cands_gen);
    if (!jetTypeIsGenJet) {
      event.getByToken(substructure_variables_src_token_basic, gentopjets_with_cands_basic);
    }
  }

  if (!njettiness_src.empty()) {
    event.getByToken(src_njettiness1_token, h_njettiness1);
    event.getByToken(src_njettiness2_token, h_njettiness2);
    event.getByToken(src_njettiness3_token, h_njettiness3);
    event.getByToken(src_njettiness4_token, h_njettiness4);
  }

  if (!ecf_beta1_src.empty()) {
    event.getByToken(src_ecf_beta1_N2_token, h_ecf_beta1_N2);
    event.getByToken(src_ecf_beta1_N3_token, h_ecf_beta1_N3);
  }
  if (!ecf_beta2_src.empty()) {
    event.getByToken(src_ecf_beta2_N2_token, h_ecf_beta2_N2);
    event.getByToken(src_ecf_beta2_N3_token, h_ecf_beta2_N3);
  }

  vector<GenTopJet> gentopjets;

  for (unsigned int i = 0; i < reco_gentopjets.size(); i++) {
    const reco::GenJet & reco_gentopjet = reco_gentopjets[i];
    if (reco_gentopjet.pt() < ptmin) continue;
    if (fabs(reco_gentopjet.eta()) > etamax) continue;

    gentopjets.emplace_back();
    GenTopJet & gentopjet = gentopjets.back();

    bool storeGenParticles = (i<NGenJetwConstituents_ || reco_gentopjet.pt()>MinPtJetwConstituents_);
    try {
      uhh2::NtupleWriterGenTopJets::fill_genjet_info(uevent, reco_gentopjet, gentopjet, storeGenParticles);
    } catch(runtime_error &){
      throw cms::Exception("fill_jgenet_info error", "Error in fill_genjet_info for gentopjets in NtupleWriterGenTopJets with src = " + src.label());
    }

    /*--- Njettiness/ECFs/other things from ValueMaps ------*/

    // Take substructure variables (Njettiness/ECFs) from matched jet collection
    // if respective source is specified.
    // Since these variables are stored in ValueMaps, we need to figure out the correct
    // reference Jet to access the value. We do this by deltaR matching

    // match a unpruned jet according to topjets_with_cands:
    int i_reco_gentopjet_wc = -1;
    if (useSubstructureVar) {
      double drmin = numeric_limits<double>::infinity();
      if (jetTypeIsGenJet) {
        for (size_t i_wc=0; i_wc < gentopjets_with_cands_gen->size(); ++i_wc) {
          auto dr = reco::deltaR((*gentopjets_with_cands_gen)[i_wc], reco_gentopjet);
          if(dr < drmin){
            i_reco_gentopjet_wc = i_wc;
            drmin = dr;
          }
        }
      } else {
        for (size_t i_wc=0; i_wc < gentopjets_with_cands_basic->size(); ++i_wc) {
          auto dr = reco::deltaR((*gentopjets_with_cands_basic)[i_wc], reco_gentopjet);
          if(dr < drmin){
            i_reco_gentopjet_wc = i_wc;
            drmin = dr;
          }
        }
      }

      if (i_reco_gentopjet_wc >= 0 && drmin < 1.0) { // be genereous: grooming can change jet axis quite a lot (esp. for DR=1.5 jets as in heptoptag)
        if (jetTypeIsGenJet) {
          auto ref = edm::Ref<reco::GenJetCollection>(gentopjets_with_cands_gen, i_reco_gentopjet_wc);
          if (!njettiness_src.empty()) {
            gentopjet.set_tau1((*h_njettiness1)[ref]);
            gentopjet.set_tau2((*h_njettiness2)[ref]);
            gentopjet.set_tau3((*h_njettiness3)[ref]);
            gentopjet.set_tau4((*h_njettiness4)[ref]);
          }
          if (!ecf_beta1_src.empty()) {
            gentopjet.set_ecfN2_beta1((*h_ecf_beta1_N2)[ref]);
            gentopjet.set_ecfN3_beta1((*h_ecf_beta1_N3)[ref]);
          }
          if (!ecf_beta2_src.empty()) {
            gentopjet.set_ecfN2_beta2((*h_ecf_beta2_N2)[ref]);
            gentopjet.set_ecfN3_beta2((*h_ecf_beta2_N3)[ref]);
          }
        } else {
          auto ref = edm::Ref<reco::BasicJetCollection>(gentopjets_with_cands_basic, i_reco_gentopjet_wc);
          if (!njettiness_src.empty()) {
            gentopjet.set_tau1((*h_njettiness1)[ref]);
            gentopjet.set_tau2((*h_njettiness2)[ref]);
            gentopjet.set_tau3((*h_njettiness3)[ref]);
            gentopjet.set_tau4((*h_njettiness4)[ref]);
          }
          if (!ecf_beta1_src.empty()) {
            gentopjet.set_ecfN2_beta1((*h_ecf_beta1_N2)[ref]);
            gentopjet.set_ecfN3_beta1((*h_ecf_beta1_N3)[ref]);
          }
          if (!ecf_beta2_src.empty()) {
            gentopjet.set_ecfN2_beta2((*h_ecf_beta2_N2)[ref]);
            gentopjet.set_ecfN3_beta2((*h_ecf_beta2_N3)[ref]);
          }
        }
      }
    } // end storing substructure vars

    /*---- Handle subjets ----*/
    // we find the corresponding basicjet in the other collection,
    // and this has daughters that are actually the subjets if the user has set it up correctly
    if (!subjet_src.empty()) {
      int i_reco_basic_match = -1;
      double drmin = numeric_limits<double>::infinity();
      for (uint i_wc=0; i_wc<h_reco_basicgenjets->size(); i_wc++) {
        auto dr = reco::deltaR((*h_reco_basicgenjets)[i_wc], reco_gentopjet);
        if(dr < drmin){
          i_reco_basic_match = i_wc;
          drmin = dr;
        }
      }

      if (i_reco_basic_match >= 0 && drmin < 0.01) { // should be an almost identical match!
        const reco::BasicJet & matchBasicJet = h_reco_basicgenjets->at(i_reco_basic_match);
        for (uint dInd=0; dInd < matchBasicJet.numberOfDaughters(); dInd++) {
          GenJet subjet;
          uhh2::NtupleWriterGenTopJets::fill_genjet_info(uevent, *(matchBasicJet.daughter(dInd)), subjet, storeGenParticles);
          gentopjet.add_subjet(subjet);
        }
      }
    }

  } // end for gentopjets loop

  uevent.set(handle, move(gentopjets));
  if(gentopjets_handle){
    EventAccess_::set_unmanaged(uevent, *gentopjets_handle, &uevent.get(handle));
  }
}

void NtupleWriterGenTopJets::fill_genjet_info(uhh2::Event & event, const reco::Candidate & reco_genjet, GenJet & genjet, bool add_genparts) {
  genjet.set_pt(reco_genjet.pt());
  genjet.set_eta(reco_genjet.eta());
  genjet.set_phi(reco_genjet.phi());
  genjet.set_energy(reco_genjet.energy());

  int jet_charge = 0;
  double chf = 0; double cef = 0;
  double nhf = 0; double nef = 0;
  double muf = 0;

  int chMult = 0;
  int nMult = 0;
  int muMult = 0;
  int elMult = 0;
  int phMult = 0;
  for (unsigned int l = 0; l<reco_genjet.numberOfSourceCandidatePtrs(); ++l) {
    const reco::Candidate* constituent = reco_genjet.daughter(l);
    jet_charge += constituent->charge();
    if(add_genparts){
      size_t genparticles_index = add_genpart(*constituent, *(event.genparticles));
      genjet.add_genparticles_index(genparticles_index);
    }

    if(abs(constituent->pdgId())==11) {
      cef += constituent->energy();
      elMult++;
      chMult++;
    } else if(abs(constituent->pdgId())==22) {
      nef += constituent->energy();
      phMult++;
      nMult++;
    } else if(abs(constituent->pdgId())==13) {
      muf += constituent->energy();
      muMult++;
      chMult++;
    } else {
      if(abs(constituent->charge())>0.1) {
        chf += constituent->energy();
        chMult++;
      } else {
        nhf += constituent->energy();
        nMult++;
      }
    }
  }
  chf /= genjet.energy();
  cef /= genjet.energy();
  nhf /= genjet.energy();
  nef /= genjet.energy();
  muf /= genjet.energy();
  genjet.set_chf(chf);
  genjet.set_cef(cef);
  genjet.set_nhf(nhf);
  genjet.set_nef(nef);
  genjet.set_muf(muf);
  genjet.set_charge(jet_charge);
  genjet.set_chargedMultiplicity(chMult);
  genjet.set_neutralMultiplicity(nMult);
  genjet.set_muonMultiplicity(muMult);
  genjet.set_electronMultiplicity(elMult);
  genjet.set_photonMultiplicity(phMult);
}
