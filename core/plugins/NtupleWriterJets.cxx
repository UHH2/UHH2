#include "UHH2/core/plugins/NtupleWriterJets.h"
#include "UHH2/core/include/AnalysisModule.h"
#include "FWCore/Framework/interface/EDProducer.h"
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
using namespace uhh2;
using namespace std;

bool btag_warning;

// Get userFloat entry, with some default return value if it doesn't exist
// TODO: template this?
float getPatJetUserFloat(const pat::Jet & jet, const std::string & key, float defaultValue=-9999.){
  return (jet.hasUserFloat(key) ? jet.userFloat(key) : defaultValue);
}


NtupleWriterJets::NtupleWriterJets(Config & cfg, bool set_jets_member){
    handle = cfg.ctx.declare_event_output<vector<Jet>>(cfg.dest_branchname, cfg.dest);
    ptmin = cfg.ptmin;
    etamax = cfg.etamax;
    if(set_jets_member){
        jets_handle = cfg.ctx.get_handle<vector<Jet>>("jets");
    }
    src = cfg.src;
    src_token = cfg.cc.consumes<std::vector<pat::Jet>>(cfg.src);
    btag_warning=true;

    save_lepton_keys_ = false;

    h_muons.clear();
    h_elecs.clear();
}

NtupleWriterJets::NtupleWriterJets(Config & cfg, bool set_jets_member, const std::vector<std::string>& muon_sources, const std::vector<std::string>& elec_sources):
  NtupleWriterJets::NtupleWriterJets(cfg, set_jets_member) {

    save_lepton_keys_ = true;

    for(const auto& muo_src : muon_sources){ auto h_muon = cfg.ctx.get_handle<std::vector<Muon>    >(muo_src); h_muons.push_back(h_muon); }
    for(const auto& ele_src : elec_sources){ auto h_elec = cfg.ctx.get_handle<std::vector<Electron>>(ele_src); h_elecs.push_back(h_elec); }
}

NtupleWriterJets::~NtupleWriterJets(){}

void NtupleWriterJets::process(const edm::Event & event, uhh2::Event & uevent,  const edm::EventSetup& iSetup){
    edm::Handle< std::vector<pat::Jet> > jet_handle;
    event.getByToken(src_token, jet_handle);
    const std::vector<pat::Jet> & pat_jets = *jet_handle;

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

        try {

          fill_jet_info(pat_jet, jet, true, false);
        }
        catch(runtime_error & ex){
          throw cms::Exception("fill_jet_info error", "Error in fill_jet_info NtupleWriterJets::process for jets with src = " + src.label());
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


void NtupleWriterJets::fill_jet_info(const pat::Jet & pat_jet, Jet & jet, bool do_btagging, bool do_taginfo){
  jet.set_charge(pat_jet.charge());
  jet.set_pt(pat_jet.pt());
  jet.set_eta(pat_jet.eta());
  jet.set_phi(pat_jet.phi());
  jet.set_energy(pat_jet.energy());
  jet.set_flavor(pat_jet.partonFlavour());
  jet.set_hadronFlavor(pat_jet.hadronFlavour());
  jet.set_numberOfDaughters (pat_jet.numberOfDaughters());
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

  // ensures you can still store jets without any JEC applied
  if (pat_jet.jecSetsAvailable()) {
    jet.set_JEC_factor_raw(pat_jet.jecFactor("Uncorrected"));

    //L1 factor needed for JEC propagation to MET
    const std::vector< std::string > factors_jet = pat_jet.availableJECLevels();
    bool isL1 = false;
    for(unsigned int i=0;i<factors_jet.size();i++)
      if(factors_jet[i]=="L1FastJet")
        isL1 = true;
    if(isL1) jet.set_JEC_L1factor_raw(pat_jet.jecFactor("L1FastJet"));
    else jet.set_JEC_L1factor_raw(1.);//PUPPI jets don't have L1 factor

  } else {
    jet.set_JEC_factor_raw(1.);
    jet.set_JEC_L1factor_raw(1.);
  }


  if(do_taginfo){
    JetBTagInfo jetbtaginfo;
    //ip tag info
    reco::TaggingVariableList tvlIP=pat_jet.tagInfoCandIP("pfImpactParameter")->taggingVariables();
    jetbtaginfo.set_TrackMomentum(tvlIP.getList(reco::btau::trackMomentum,false));
    jetbtaginfo.set_TrackEta(tvlIP.getList(reco::btau::trackEta,false));
    jetbtaginfo.set_TrackEtaRel(tvlIP.getList(reco::btau::trackEtaRel,false));
    jetbtaginfo.set_TrackDeltaR(tvlIP.getList(reco::btau::trackDeltaR,false));
    jetbtaginfo.set_TrackSip3dVal(tvlIP.getList(reco::btau::trackSip3dVal,false));
    jetbtaginfo.set_TrackSip3dSig(tvlIP.getList(reco::btau::trackSip3dSig,false));
    jetbtaginfo.set_TrackSip2dVal(tvlIP.getList(reco::btau::trackSip2dVal,false));
    jetbtaginfo.set_TrackSip2dSig(tvlIP.getList(reco::btau::trackSip2dSig,false));
    jetbtaginfo.set_TrackDecayLenVal(tvlIP.getList(reco::btau::trackDecayLenVal,false));
    jetbtaginfo.set_TrackChi2(tvlIP.getList(reco::btau::trackChi2,false));
    jetbtaginfo.set_TrackNTotalHits(tvlIP.getList(reco::btau::trackNTotalHits,false));
    jetbtaginfo.set_TrackNPixelHits(tvlIP.getList(reco::btau::trackNPixelHits,false));     
    jetbtaginfo.set_TrackPtRel(tvlIP.getList(reco::btau::trackPtRel,false));
    jetbtaginfo.set_TrackPPar(tvlIP.getList(reco::btau::trackPPar,false));
    jetbtaginfo.set_TrackPtRatio(tvlIP.getList(reco::btau::trackPtRatio,false));
    jetbtaginfo.set_TrackPParRatio(tvlIP.getList(reco::btau::trackPParRatio,false));
    jetbtaginfo.set_TrackJetDistVal(tvlIP.getList(reco::btau::trackJetDistVal,false));
    jetbtaginfo.set_TrackJetDistSig(tvlIP.getList(reco::btau::trackJetDistSig,false));
    jetbtaginfo.set_TrackGhostTrackDistVal(tvlIP.getList(reco::btau::trackGhostTrackDistVal,false));
    jetbtaginfo.set_TrackGhostTrackDistSig(tvlIP.getList(reco::btau::trackGhostTrackDistSig,false));
    jetbtaginfo.set_TrackGhostTrackWeight(tvlIP.getList(reco::btau::trackGhostTrackWeight,false));
    //sv tag info
    reco::TaggingVariableList tvlSV=pat_jet.tagInfoCandSecondaryVertex("pfInclusiveSecondaryVertexFinder")->taggingVariables();
    jetbtaginfo.set_FlightDistance2dVal(tvlSV.getList(reco::btau::flightDistance2dVal,false));
    jetbtaginfo.set_FlightDistance2dSig(tvlSV.getList(reco::btau::flightDistance2dSig,false));
    jetbtaginfo.set_FlightDistance3dVal(tvlSV.getList(reco::btau::flightDistance3dVal,false));
    jetbtaginfo.set_FlightDistance3dSig(tvlSV.getList(reco::btau::flightDistance3dSig,false));
    jetbtaginfo.set_VertexJetDeltaR(tvlSV.getList(reco::btau::vertexJetDeltaR,false));
    jetbtaginfo.set_JetNSecondaryVertices(pat_jet.tagInfoCandSecondaryVertex("pfInclusiveSecondaryVertexFinder")->nVertices());
    std::vector<TLorentzVector> vp4; vp4.clear();
    std::vector<float> vchi2; vchi2.clear();
    std::vector<float> vndof; vndof.clear();
    std::vector<float> vchi2ndof; vchi2ndof.clear();
    std::vector<int> sizetracks; sizetracks.clear();
    for(unsigned int i = 0; i < pat_jet.tagInfoCandSecondaryVertex("pfInclusiveSecondaryVertexFinder")->nVertices(); ++i) {
      reco::VertexCompositePtrCandidate sv=pat_jet.tagInfoCandSecondaryVertex("pfInclusiveSecondaryVertexFinder")->secondaryVertex(i);
      sizetracks.push_back(sv.numberOfSourceCandidatePtrs());
      ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > p4 = sv.p4();
      vp4.push_back(TLorentzVector(p4.px(),p4.py(),p4.pz(),p4.e()));
      vchi2.push_back(sv.vertexChi2());  
      vndof.push_back(sv.vertexNdof());  
      vchi2ndof.push_back(sv.vertexNormalizedChi2());  
    }
    jetbtaginfo.set_SecondaryVertex(vp4);
    jetbtaginfo.set_VertexChi2(vchi2);
    jetbtaginfo.set_VertexNdof(vndof);
    jetbtaginfo.set_VertexNormalizedChi2(vchi2ndof);
    jetbtaginfo.set_VertexNTracks(sizetracks);
    //try computer: currently not implemented! Only draft code for future use
    /*const GenericMVAJetTagComputer *computer ;
      edm::ESHandle<JetTagComputer> computerHandle;
      std::string SVComputer_ = "candidateCombinedSecondaryVertexV2Computer";
      iSetup.get<JetTagComputerRecord>().get(SVComputer_.c_str(), computerHandle );
      computer = dynamic_cast<const GenericMVAJetTagComputer*>( computerHandle.product() );
      if(computer)
      {
      std::vector<const reco::BaseTagInfo*>  baseTagInfos;
      baseTagInfos.push_back(pat_jet.tagInfoTrackIP("pfImpactParameter") );
      baseTagInfos.push_back(pat_jet.tagInfoSecondaryVertex("pfInclusiveSecondaryVertexFinder") );      
      JetTagComputer::TagInfoHelper helper(baseTagInfos);
      reco::TaggingVariableList vars = computer->taggingVariables(helper);
      jetbtaginfo.set_VertexMassJTC(vars.get(reco::btau::vertexMass,-9999));
      jetbtaginfo.set_VertexCategoryJTC(vars.get(reco::btau::vertexCategory,-9999));
      jetbtaginfo.set_VertexEnergyRatioJTC(vars.get(reco::btau::vertexEnergyRatio,-9999));
      jetbtaginfo.set_TrackSip3dSigAboveCharmJTC(vars.get(reco::btau::trackSip3dSigAboveCharm,-9999));
      }*/
    jet.set_btaginfo(jetbtaginfo);
    JetBTagInfo postcheck=jet.btaginfo();
  }//do taginfos
  if(do_btagging){
    const auto & bdisc = pat_jet.getPairDiscri();
    bool csv = false, csvmva = false, doubleak8 = false, doubleca15 = false, deepcsv_b = false, deepcsv_bb = false;
    for(const auto & name_value : bdisc){
      const auto & name = name_value.first;
      const auto & value = name_value.second;
      if(name == "pfCombinedInclusiveSecondaryVertexV2BJetTags"){
	jet.set_btag_combinedSecondaryVertex(value);
	csv = true;
      }
      else if(name == "pfCombinedMVAV2BJetTags"){                                                                                                            
        jet.set_btag_combinedSecondaryVertexMVA(value);                                                                                                                              
        csvmva = true;                                                                                                                                                               
      }   
      else if(name == "pfDeepCSVJetTags:probb"){                                                                                                            
        jet.set_btag_DeepCSV_probb(value);                                                                                                                              
        deepcsv_b = true;                                                                                                                                                               
      }   
      else if(name == "pfDeepCSVJetTags:probbb"){                                                                                                            
        jet.set_btag_DeepCSV_probbb(value);                                                                                                                              
        deepcsv_bb = true;                                                                                                                                                               
      }   
      else if(name == "pfBoostedDoubleSecondaryVertexAK8BJetTags"){
	jet.set_btag_BoostedDoubleSecondaryVertexAK8(value);
	doubleak8 = true;
      }
      else if(name == "pfBoostedDoubleSecondaryVertexCA15BJetTags"){
	jet.set_btag_BoostedDoubleSecondaryVertexCA15(value);
	doubleca15 = true;
      }
    }

    if(!csv || !csvmva || !doubleak8 || !doubleca15 || !deepcsv_b || !deepcsv_bb){
      if(btag_warning){
	cout << "Warning in NtupleWriterJets: did not find all b-taggers! Available btaggers: ";
	for(const auto & name_value : bdisc){
	  cout << name_value.first << " ";
	}
	cout << endl;
	btag_warning = false;
      }
      // throw runtime_error("did not find all b-taggers; see output for details");
    }
  }
}


NtupleWriterTopJets::NtupleWriterTopJets(Config & cfg, bool set_jets_member): ptmin(cfg.ptmin), etamax(cfg.etamax) {
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

    pruned_src = cfg.pruned_src;
    if(pruned_src.find("Mass")==string::npos){
      src_pruned_token = cfg.cc.consumes<std::vector<pat::Jet>>(cfg.pruned_src);
    }

    softdrop_src = cfg.softdrop_src;
    if(softdrop_src.find("Mass")==string::npos){
      src_softdrop_token = cfg.cc.consumes<std::vector<pat::Jet>>(cfg.softdrop_src);
    }

    src_higgs_token = cfg.cc.consumes<std::vector<pat::Jet>>(cfg.higgs_src);
    higgs_name=cfg.higgs_name;
    do_taginfo_subjets = cfg.do_taginfo_subjets;
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

    save_lepton_keys_ = false;

    h_muons.clear();
    h_elecs.clear();

    higgstaginfo_src = cfg.higgstaginfo_src;
    src_higgstaginfo_token =  cfg.cc.consumes<std::vector<reco::BoostedDoubleSVTagInfo> >(cfg.higgstaginfo_src);
}

NtupleWriterTopJets::NtupleWriterTopJets(Config & cfg, bool set_jets_member, const std::vector<std::string>& muon_sources, const std::vector<std::string>& elec_sources):
  NtupleWriterTopJets::NtupleWriterTopJets(cfg, set_jets_member) {

    save_lepton_keys_ = true;

    for(const auto& muo_src : muon_sources){ auto h_muon = cfg.ctx.get_handle<std::vector<Muon>    >(muo_src); h_muons.push_back(h_muon); }
    for(const auto& ele_src : elec_sources){ auto h_elec = cfg.ctx.get_handle<std::vector<Electron>>(ele_src); h_elecs.push_back(h_elec); }

}

NtupleWriterTopJets::~NtupleWriterTopJets(){}


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
        //use CHS jet momentum in case of CHS subjet collection (see https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookMiniAOD2016#Jets)
        if(subjet_src.find("CHS")!=string::npos){
          TLorentzVector puppi_v4;
          if (!pat_topjet.hasUserFloat("ak8PFJetsCHSValueMap:pt")) {
            throw cms::Exception("Missing userFloat", "You wanted CHS subjets but no ak8PFJetsCHSValueMap entries in the ValueMap");
          }
          puppi_v4.SetPtEtaPhiM(pat_topjet.userFloat("ak8PFJetsCHSValueMap:pt"),
            pat_topjet.userFloat("ak8PFJetsCHSValueMap:eta"),
            pat_topjet.userFloat("ak8PFJetsCHSValueMap:phi"),
            pat_topjet.userFloat("ak8PFJetsCHSValueMap:mass"));
          //skip jets with incredibly high pT (99999 seems to be a default value in MINIAOD if the puppi jet is not defined)
          if(puppi_v4.Pt()>=99999) continue;
          if(puppi_v4.Pt() < ptmin) continue;
          if(fabs(puppi_v4.Eta()) > etamax) continue;
        }
        else{
          if(pat_topjet.pt() < ptmin) continue;
          if(fabs(pat_topjet.eta()) > etamax) continue;
        }
        
        topjets.emplace_back();
        TopJet & topjet = topjets.back();
        try{
           uhh2::NtupleWriterJets::fill_jet_info(pat_topjet, topjet, do_btagging, false);
	   if(subjet_src.find("CHS")!=string::npos){
	     TLorentzVector puppi_v4;
	     puppi_v4.SetPtEtaPhiM(pat_topjet.userFloat("ak8PFJetsCHSValueMap:pt"),
				   pat_topjet.userFloat("ak8PFJetsCHSValueMap:eta"),
				   pat_topjet.userFloat("ak8PFJetsCHSValueMap:phi"),
				   pat_topjet.userFloat("ak8PFJetsCHSValueMap:mass"));
	     topjet.set_pt(puppi_v4.Pt());
	     topjet.set_eta(puppi_v4.Eta());
	     topjet.set_phi(puppi_v4.Phi());
	     topjet.set_energy(puppi_v4.E());
	}

        }catch(runtime_error &){
          throw cms::Exception("fill_jet_info error", "Error in fill_jet_info for topjets in NtupleWriterTopJets with src = " + src.label());
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

        /*--- pruned mass -----*/
        if(pruned_src.find("Mass")!=string::npos){

          topjet.set_prunedmass(getPatJetUserFloat(pat_topjet, pruned_src, -1.));
        }
        else if(pruned_src!=""){//pruned mass set through matching with pruned-jet collection

          edm::Handle<pat::JetCollection> pruned_pat_topjets;
          event.getByToken(src_pruned_token, pruned_pat_topjets);
          const vector<pat::Jet> & pat_prunedjets = *pruned_pat_topjets;

          //match a jet from pruned collection
          int i_pat_prunedjet = -1;
          double drmin = numeric_limits<double>::infinity();
          for (unsigned int ih = 0; ih < pat_prunedjets.size(); ih++) {

            const pat::Jet & pruned_jet = pat_prunedjets[ih];
            auto dr = reco::deltaR(pruned_jet, pat_topjet);
            if(dr < drmin){
              i_pat_prunedjet = ih;
              drmin = dr;
            }
          }

          if(i_pat_prunedjet >= 0 && drmin < 1.0){

            const pat::Jet & pruned_jet = pat_prunedjets[i_pat_prunedjet];
            topjet.set_prunedmass(pruned_jet.mass());
          }
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
	if(subjet_src=="daughters"){
	  for (unsigned int k = 0; k < pat_topjet.numberOfDaughters(); k++) {
            Jet subjet;
            auto patsubjetd = dynamic_cast<const pat::Jet *>(pat_topjet.daughter(k));
            if (patsubjetd) {
	      try{
		NtupleWriterJets::fill_jet_info(*patsubjetd, subjet, do_btagging_subjets, do_taginfo_subjets);
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
		NtupleWriterJets::fill_jet_info(*tpatsubjet, subjet, do_btagging_subjets, do_taginfo_subjets);
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

