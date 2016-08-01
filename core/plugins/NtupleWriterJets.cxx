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
#include "DataFormats/JetReco/interface/HTTTopJetTagInfo.h"
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
using namespace uhh2;
using namespace std;

bool btag_warning;

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

          cerr << "Exception in fill_jet_info in NtupleWriterJets::process for jets with src=" << src << endl;
          throw;
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

  jet.set_JEC_factor_raw(pat_jet.jecFactor("Uncorrected"));
  //L1 factor needed for JEC propagation to MET
  const std::vector< std::string > factors_jet = pat_jet.availableJECLevels();
  bool isL1 = false;
  for(unsigned int i=0;i<factors_jet.size();i++)
    if(factors_jet[i]=="L1FastJet")
      isL1 = true;
  if(isL1) jet.set_JEC_L1factor_raw(pat_jet.jecFactor("L1FastJet"));
  else jet.set_JEC_L1factor_raw(1.);//PUPPI jets don't have L1 factor

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
    bool sv_he = false, csv = false, csvmva = false, jetp = false, jetbp = false, doubleak8 = false, doubleca15 = false;
    for(const auto & name_value : bdisc){
      const auto & name = name_value.first;
      const auto & value = name_value.second;
      if(name == "pfSimpleSecondaryVertexHighEffBJetTags"){
	jet.set_btag_simpleSecondaryVertexHighEff(value);
	sv_he = true;
      }
      else if(name == "pfCombinedInclusiveSecondaryVertexV2BJetTags"){
	jet.set_btag_combinedSecondaryVertex(value);
	csv = true;
      }
      else if(name == "pfCombinedMVAV2BJetTags"){                                                                                                            
        jet.set_btag_combinedSecondaryVertexMVA(value);                                                                                                                              
        csvmva = true;                                                                                                                                                               
      }   
      else if(name == "pfJetBProbabilityBJetTags"){
	jet.set_btag_jetBProbability(value);
	jetp = true;
      }
      else if(name == "pfJetProbabilityBJetTags"){
	jet.set_btag_jetProbability(value);
	jetbp = true;
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

    // NOTE: csvmva is NOT set.
    if(!sv_he || !csv || !csvmva || !jetp || !jetbp || !doubleak8 ||!doubleca15){
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

    qjets_src = cfg.qjets_src;
    src_qjets_token = cfg.cc.consumes<edm::ValueMap<float> >(edm::InputTag(qjets_src, "QjetsVolatility"));

    subjet_src = cfg.subjet_src;
    higgs_src= cfg.higgs_src;

    src_hepTopTagCHS_token = cfg.cc.consumes<edm::View<reco::HTTTopJetTagInfo> >(edm::InputTag("hepTopTagCHS"));
    src_hepTopTagPuppi_token = cfg.cc.consumes<edm::View<reco::HTTTopJetTagInfo> >(edm::InputTag("hepTopTagPuppi"));

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
    btag_warning=true;
    topjet_collection = cfg.dest_branchname;

    save_lepton_keys_ = false;

    h_muons.clear();
    h_elecs.clear();
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

    edm::Handle<pat::JetCollection> h_pat_topjets;
    event.getByToken(src_token, h_pat_topjets);
    const vector<pat::Jet> & pat_topjets = *h_pat_topjets;

    edm::Handle<edm::ValueMap<float>> h_njettiness1, h_njettiness2, h_njettiness3;
    edm::Handle<edm::ValueMap<float>> h_qjets;
    
    edm::Handle<reco::BasicJetCollection> topjets_with_cands;
    edm::Handle<reco::PFJetCollection> topjets_with_cands_reco;
    
    if(!njettiness_src.empty()){
        event.getByToken(src_njettiness1_token, h_njettiness1);
        event.getByToken(src_njettiness2_token, h_njettiness2);
        event.getByToken(src_njettiness3_token, h_njettiness3);
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
    vector<TopJet> topjets;
    edm::Handle<edm::View<reco::HTTTopJetTagInfo>> top_jet_infos;
    if (topjet_collection.find("CHS")!=string::npos) event.getByToken(src_hepTopTagCHS_token, top_jet_infos);
    if (topjet_collection.find("Puppi")!=string::npos) event.getByToken(src_hepTopTagPuppi_token, top_jet_infos); // Make sure both collections have the same size
    if (topjet_collection.find("Hep")!=string::npos) assert(pat_topjets.size()==top_jet_infos->size());

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
        try{
           uhh2::NtupleWriterJets::fill_jet_info(pat_topjet, topjet, do_btagging, false);
        }catch(runtime_error &){
           cerr << "Error in fill_jet_info for topjets in NtupleWriterTopJets with src = " << src << "." << endl;
           throw;
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
              }
              if(!qjets_src.empty()){
                topjet.set_qjets_volatility((*h_qjets)[ref]);
              }
            }
          }
        }
        if (topjet_collection.find("Hep")!=string::npos)
           {
              const reco::HTTTopJetTagInfo& jet_info = top_jet_infos->at(i);
              topjet.set_tag(TopJet::tagname2tag("fRec"), jet_info.properties().fRec);
              topjet.set_tag(TopJet::tagname2tag("Ropt"), jet_info.properties().Ropt);
              topjet.set_tag(TopJet::tagname2tag("massRatioPassed"), jet_info.properties().massRatioPassed);
              topjet.set_tag(TopJet::tagname2tag("mass"),pat_topjet.mass());
              topjet.set_tag(TopJet::tagname2tag("RoptCalc"), jet_info.properties().RoptCalc);
              topjet.set_tag(TopJet::tagname2tag("ptForRoptCalc"), jet_info.properties().ptForRoptCalc);
           }

        /*--- Njettiness ------*/
        if(njettiness_src.empty()){

          topjet.set_tau1(pat_topjet.userFloat("NjettinessAK8:tau1"));
          topjet.set_tau2(pat_topjet.userFloat("NjettinessAK8:tau2"));
          topjet.set_tau3(pat_topjet.userFloat("NjettinessAK8:tau3"));
        }
        /*---------------------*/

        /*--- pruned mass -----*/
        if(pruned_src.find("Mass")!=string::npos){

          topjet.set_prunedmass(pat_topjet.userFloat(pruned_src));
        }
        else{//pruned mass set through matching with pruned-jet collection

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

          topjet.set_softdropmass(pat_topjet.userFloat(softdrop_src));
        }
        else {//softdrop mass set through matching with softdrop-jet collection
          /* not implemented */
        }
        /*---------------------*/

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
	    //For matching higgs jet fill relevant variables for commissioning studies
	    
	    float z_ratio = dummyZ_ratio;
	    float trackSip3dSig_3 = dummyTrackSip3dSig, trackSip3dSig_2 = dummyTrackSip3dSig, trackSip3dSig_1 = dummyTrackSip3dSig, trackSip3dSig_0 = dummyTrackSip3dSig;
	    float tau2_trackSip3dSig_0 = dummyTrackSip3dSig, tau1_trackSip3dSig_0 = dummyTrackSip3dSig, tau2_trackSip3dSig_1 = dummyTrackSip3dSig, tau1_trackSip3dSig_1 = dummyTrackSip3dSig;
	    float trackSip2dSigAboveCharm_0 = dummyTrackSip2dSigAbove, trackSip2dSigAboveBottom_0 = dummyTrackSip2dSigAbove, trackSip2dSigAboveBottom_1 = dummyTrackSip2dSigAbove;
	    float tau1_trackEtaRel_0 = dummyTrackEtaRel, tau1_trackEtaRel_1 = dummyTrackEtaRel, tau1_trackEtaRel_2 = dummyTrackEtaRel;
	    float tau2_trackEtaRel_0 = dummyTrackEtaRel, tau2_trackEtaRel_1 = dummyTrackEtaRel, tau2_trackEtaRel_2 = dummyTrackEtaRel;
	    float tau1_vertexMass = dummyVertexMass, tau1_vertexEnergyRatio = dummyVertexEnergyRatio, tau1_vertexDeltaR = dummyVertexDeltaR, tau1_flightDistance2dSig = dummyFlightDistance2dSig;
	    float tau2_vertexMass = dummyVertexMass, tau2_vertexEnergyRatio = dummyVertexEnergyRatio, tau2_vertexDeltaR = dummyVertexDeltaR, tau2_flightDistance2dSig = dummyFlightDistance2dSig;
	    float jetNTracks = 0, nSV = 0, tau1_nSecondaryVertices = 0, tau2_nSecondaryVertices = 0;

	    const reco::CandIPTagInfo              & ipTagInfo = *higgs_jet.tagInfoCandIP("pfImpactParameter");
	    const reco::CandSecondaryVertexTagInfo & svTagInfo = *higgs_jet.tagInfoCandSecondaryVertex("pfInclusiveSecondaryVertexFinder");
	    
	    // get the jet reference
	    const reco::JetBaseRef jet = svTagInfo.jet();

	    std::vector<fastjet::PseudoJet> currentAxes;
	    float tau2, tau1;
	    // calculate N-subjettiness
	    recalcNsubjettiness(jet, tau1, tau2, currentAxes);
	 
	    const reco::VertexRef & vertexRef = ipTagInfo.primaryVertex();
	    GlobalPoint pv(0.,0.,0.);
	    if ( ipTagInfo.primaryVertex().isNonnull() )
	      pv = GlobalPoint(vertexRef->x(),vertexRef->y(),vertexRef->z());

	    const std::vector<reco::CandidatePtr> & selectedTracks = ipTagInfo.selectedTracks();
	    const std::vector<reco::btag::TrackIPData> & ipData = ipTagInfo.impactParameterData();
	    size_t trackSize = selectedTracks.size();


	    reco::TrackKinematics allKinematics;
	    std::vector<float> IP3Ds, IP3Ds_1, IP3Ds_2;
	    int contTrk=0;

	    // loop over tracks associated to the jet
	    for (size_t itt=0; itt < trackSize; ++itt)
	      {
		const reco::CandidatePtr ptrackRef = selectedTracks[itt];
		const reco::Track * ptrackPtr = reco::btag::toTrack(ptrackRef);
		const reco::Track & ptrack = *ptrackPtr;

		float track_PVweight = 0.;
		setTracksPV(ptrackRef, vertexRef, track_PVweight);
		if (track_PVweight>0.5) allKinematics.add(ptrack, track_PVweight);

		// check if the track is from V0
		bool isfromV0 = false;
		const reco::Track * trackPairV0Test[2];

		trackPairV0Test[0] = ptrackPtr;

		for (size_t jtt=0; jtt < trackSize; ++jtt)
		  {
		    if (itt == jtt) continue;

		    const reco::CandidatePtr pairTrackRef = selectedTracks[jtt];
		    const reco::Track * pairTrackPtr = reco::btag::toTrack(pairTrackRef);

		    trackPairV0Test[1] = pairTrackPtr;
		    if (!trackPairV0Filter(trackPairV0Test, 2))
		      {
			isfromV0 = true;
		      }
		    if (isfromV0)
		      break;
		  }

		if(!isfromV0 ) jetNTracks += 1.;

		
		iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder", trackBuilder);
		reco::TransientTrack transientTrack = trackBuilder->build(ptrack);
		GlobalVector direction(jet->px(), jet->py(), jet->pz());

		int index = 0;
		if (currentAxes.size() > 1 && reco::deltaR2(ptrack,currentAxes[1]) < reco::deltaR2(ptrack,currentAxes[0]))
		  index = 1;
		direction = GlobalVector(currentAxes[index].px(), currentAxes[index].py(), currentAxes[index].pz());

		// decay distance and track distance wrt to the closest tau axis
		float decayLengthTau=-1;
		float distTauAxis=-1;

		TrajectoryStateOnSurface closest = IPTools::closestApproachToJet(transientTrack.impactPointState(), *vertexRef , direction, transientTrack.field());
		if (closest.isValid())
		  decayLengthTau =  (closest.globalPosition() - RecoVertex::convertPos(vertexRef->position())).mag();

		distTauAxis = std::abs(IPTools::jetTrackDistance(transientTrack, direction, *vertexRef ).second.value());

		float IP3Dsig = ipTagInfo.impactParameterData()[itt].ip3d.significance();

		if( !isfromV0 && decayLengthTau<maxDecayLen_ && distTauAxis<maxDistToAxis_ )
		  {
		    IP3Ds.push_back( IP3Dsig<-50. ? -50. : IP3Dsig );
		    ++contTrk;
		    if (currentAxes.size() > 1)
		      {
			if (reco::deltaR2(ptrack,currentAxes[0]) < reco::deltaR2(ptrack,currentAxes[1]))
			  IP3Ds_1.push_back( IP3Dsig<-50. ? -50. : IP3Dsig );
			else
			  IP3Ds_2.push_back( IP3Dsig<-50. ? -50. : IP3Dsig );
		      }
		    else
		      IP3Ds_1.push_back( IP3Dsig<-50. ? -50. : IP3Dsig );
		  }
	      }

	    std::vector<size_t> indices = ipTagInfo.sortedIndexes(reco::btag::IP2DSig);
	    bool charmThreshSet = false;

	    reco::TrackKinematics kin;
	    for (size_t i=0; i<indices.size(); ++i)
	      {
		size_t idx = indices[i];
		const reco::btag::TrackIPData & data = ipData[idx];
		const reco::CandidatePtr ptrackRef = selectedTracks[idx];
		const reco::Track * ptrackPtr = reco::btag::toTrack(ptrackRef);
		const reco::Track & track = (*ptrackPtr);

		kin.add(track);

		if ( kin.vectorSum().M() > charmThreshold // charm cut
		     && !charmThreshSet )
		  {
		    trackSip2dSigAboveCharm_0 = data.ip2d.significance();

		    charmThreshSet = true;
		  }

		if ( kin.vectorSum().M() > bottomThreshold ) // bottom cut
		  {
		    trackSip2dSigAboveBottom_0 = data.ip2d.significance();
		    if ( (i+1)<indices.size() ) trackSip2dSigAboveBottom_1 = (ipData[indices[i+1]]).ip2d.significance();

		    break;
		  }
	      }

	    float dummyTrack = -50.;

	    std::sort( IP3Ds.begin(),IP3Ds.end(),std::greater<float>() );
	    std::sort( IP3Ds_1.begin(),IP3Ds_1.end(),std::greater<float>() );
	    std::sort( IP3Ds_2.begin(),IP3Ds_2.end(),std::greater<float>() );
	    int num_1 = IP3Ds_1.size();
	    int num_2 = IP3Ds_2.size();

	    switch(contTrk){
	    case 0:

	      trackSip3dSig_0 = dummyTrack;
	      trackSip3dSig_1 = dummyTrack;
	      trackSip3dSig_2 = dummyTrack;
	      trackSip3dSig_3 = dummyTrack;

	      break;

	    case 1:

	      trackSip3dSig_0 = IP3Ds.at(0);
	      trackSip3dSig_1 = dummyTrack;
	      trackSip3dSig_2 = dummyTrack;
	      trackSip3dSig_3 = dummyTrack;

	      break;

	    case 2:

	      trackSip3dSig_0 = IP3Ds.at(0);
	      trackSip3dSig_1 = IP3Ds.at(1);
	      trackSip3dSig_2 = dummyTrack;
	      trackSip3dSig_3 = dummyTrack;

	      break;

	    case 3:

	      trackSip3dSig_0 = IP3Ds.at(0);
	      trackSip3dSig_1 = IP3Ds.at(1);
	      trackSip3dSig_2 = IP3Ds.at(2);
	      trackSip3dSig_3 = dummyTrack;

	      break;

	    default:

	      trackSip3dSig_0 = IP3Ds.at(0);
	      trackSip3dSig_1 = IP3Ds.at(1);
	      trackSip3dSig_2 = IP3Ds.at(2);
	      trackSip3dSig_3 = IP3Ds.at(3);

	    }

	    switch(num_1){
	    case 0:

	      tau1_trackSip3dSig_0 = dummyTrack;
	      tau1_trackSip3dSig_1 = dummyTrack;

	      break;

	    case 1:

	      tau1_trackSip3dSig_0 = IP3Ds_1.at(0);
	      tau1_trackSip3dSig_1 = dummyTrack;

	      break;

	    default:

	      tau1_trackSip3dSig_0 = IP3Ds_1.at(0);
	      tau1_trackSip3dSig_1 = IP3Ds_1.at(1);

	    }

	    switch(num_2){
	    case 0:

	      tau2_trackSip3dSig_0 = dummyTrack;
	      tau2_trackSip3dSig_1 = dummyTrack;

	      break;

	    case 1:
	      tau2_trackSip3dSig_0 = IP3Ds_2.at(0);
	      tau2_trackSip3dSig_1 = dummyTrack;

	      break;

	    default:

	      tau2_trackSip3dSig_0 = IP3Ds_2.at(0);
	      tau2_trackSip3dSig_1 = IP3Ds_2.at(1);

	    }

	    math::XYZVector jetDir = jet->momentum().Unit();
	    reco::TrackKinematics tau1Kinematics;
	    reco::TrackKinematics tau2Kinematics;
	    std::vector<float> tau1_trackEtaRels, tau2_trackEtaRels;

	    std::map<double, size_t> VTXmap;
	    for (size_t vtx = 0; vtx < svTagInfo.nVertices(); ++vtx)
	      {
		reco::TrackKinematics vertexKinematic;

		// get the vertex kinematics
		const reco::VertexCompositePtrCandidate vertex = svTagInfo.secondaryVertex(vtx);
		vertexKinematics(vertex, vertexKinematic);

		if (currentAxes.size() > 1)
		  {
		    if (reco::deltaR2(svTagInfo.flightDirection(vtx),currentAxes[1]) < reco::deltaR2(svTagInfo.flightDirection(vtx),currentAxes[0]))
		      {
			tau2Kinematics = tau2Kinematics + vertexKinematic;
			if( tau2_flightDistance2dSig < 0 )
			  {
			    tau2_flightDistance2dSig = svTagInfo.flightDistance(vtx,true).significance();
			    tau2_vertexDeltaR = reco::deltaR(svTagInfo.flightDirection(vtx),currentAxes[1]);
			  }
			etaRelToTauAxis(vertex, currentAxes[1], tau2_trackEtaRels);
			tau2_nSecondaryVertices += 1.;
		      }
		    else
		      {
			tau1Kinematics = tau1Kinematics + vertexKinematic;
			if( tau1_flightDistance2dSig < 0 )
			  {
			    tau1_flightDistance2dSig =svTagInfo.flightDistance(vtx,true).significance();
			    tau1_vertexDeltaR = reco::deltaR(svTagInfo.flightDirection(vtx),currentAxes[0]);
			  }
			etaRelToTauAxis(vertex, currentAxes[0], tau1_trackEtaRels);
			tau1_nSecondaryVertices += 1.;
		      }

		  }
		else if (currentAxes.size() > 0)
		  {
		    tau1Kinematics = tau1Kinematics + vertexKinematic;
		    if( tau1_flightDistance2dSig < 0 )
		      {
			tau1_flightDistance2dSig =svTagInfo.flightDistance(vtx,true).significance();
			tau1_vertexDeltaR = reco::deltaR(svTagInfo.flightDirection(vtx),currentAxes[0]);
		      }
		    etaRelToTauAxis(vertex, currentAxes[1], tau1_trackEtaRels);
		    tau1_nSecondaryVertices += 1.;
		  }
		GlobalVector flightDir = svTagInfo.flightDirection(vtx);
		if (reco::deltaR2(flightDir, jetDir)<(maxSVDeltaRToJet_*maxSVDeltaRToJet_))
		  VTXmap[svTagInfo.flightDistance(vtx).error()]=vtx;
	      }
	    nSV = VTXmap.size();


	    math::XYZTLorentzVector allSum = allKinematics.weightedVectorSum() ;
	    if ( tau1_nSecondaryVertices > 0. )
	      {
		math::XYZTLorentzVector tau1_vertexSum = tau1Kinematics.weightedVectorSum();
		tau1_vertexEnergyRatio = tau1_vertexSum.E() / allSum.E();
		if ( tau1_vertexEnergyRatio > 50. ) tau1_vertexEnergyRatio = 50.;

		tau1_vertexMass = tau1_vertexSum.M();
	      }

	    if ( tau2_nSecondaryVertices > 0. )
	      {
		math::XYZTLorentzVector tau2_vertexSum = tau2Kinematics.weightedVectorSum();
		tau2_vertexEnergyRatio = tau2_vertexSum.E() / allSum.E();
		if ( tau2_vertexEnergyRatio > 50. ) tau2_vertexEnergyRatio = 50.;

		tau2_vertexMass= tau2_vertexSum.M();
	      }
	    
	    float dummyEtaRel = -1.;

	    std::sort( tau1_trackEtaRels.begin(),tau1_trackEtaRels.end() );
	    std::sort( tau2_trackEtaRels.begin(),tau2_trackEtaRels.end() );

	    switch(tau2_trackEtaRels.size()){
	    case 0:

	      tau2_trackEtaRel_0 = dummyEtaRel;
	      tau2_trackEtaRel_1 = dummyEtaRel;
	      tau2_trackEtaRel_2 = dummyEtaRel;

	      break;

	    case 1:

	      tau2_trackEtaRel_0 = tau2_trackEtaRels.at(0);
	      tau2_trackEtaRel_1 = dummyEtaRel;
	      tau2_trackEtaRel_2 = dummyEtaRel;

	      break;

	    case 2:

	      tau2_trackEtaRel_0 = tau2_trackEtaRels.at(0);
	      tau2_trackEtaRel_1 = tau2_trackEtaRels.at(1);
	      tau2_trackEtaRel_2 = dummyEtaRel;

	      break;

	    default:

	      tau2_trackEtaRel_0 = tau2_trackEtaRels.at(0);
	      tau2_trackEtaRel_1 = tau2_trackEtaRels.at(1);
	      tau2_trackEtaRel_2 = tau2_trackEtaRels.at(2);

	    }
	    
	    switch(tau1_trackEtaRels.size()){
	    case 0:

	      tau1_trackEtaRel_0 = dummyEtaRel;
	      tau1_trackEtaRel_1 = dummyEtaRel;
	      tau1_trackEtaRel_2 = dummyEtaRel;

	      break;

	    case 1:

	      tau1_trackEtaRel_0 = tau1_trackEtaRels.at(0);
	      tau1_trackEtaRel_1 = dummyEtaRel;
	      tau1_trackEtaRel_2 = dummyEtaRel;

	      break;

	    case 2:

	      tau1_trackEtaRel_0 = tau1_trackEtaRels.at(0);
	      tau1_trackEtaRel_1 = tau1_trackEtaRels.at(1);
	      tau1_trackEtaRel_2 = dummyEtaRel;

	      break;
	      
	    default:

	      tau1_trackEtaRel_0 = tau1_trackEtaRels.at(0);
	      tau1_trackEtaRel_1 = tau1_trackEtaRels.at(1);
	      tau1_trackEtaRel_2 = tau1_trackEtaRels.at(2);

	    }

	    int cont=0;
	    GlobalVector flightDir_0, flightDir_1;
	    reco::Candidate::LorentzVector SV_p4_0 , SV_p4_1;
	    double vtxMass = 0.;

	    for ( std::map<double, size_t>::iterator iVtx=VTXmap.begin(); iVtx!=VTXmap.end(); ++iVtx)
	      {
		++cont;
		const reco::VertexCompositePtrCandidate &vertex = svTagInfo.secondaryVertex(iVtx->second);
		if (cont==1)
		  {
		    flightDir_0 = svTagInfo.flightDirection(iVtx->second);
		    SV_p4_0 = vertex.p4();
		    vtxMass = SV_p4_0.mass();

		    if(vtxMass > 0.)
		      z_ratio = reco::deltaR(currentAxes[1],currentAxes[0])*SV_p4_0.pt()/vtxMass;
		  }
		if (cont==2)
		  {
		    flightDir_1 = svTagInfo.flightDirection(iVtx->second);
		    SV_p4_1 = vertex.p4();
		    vtxMass = (SV_p4_1+SV_p4_0).mass();

		    if(vtxMass > 0.)
		      z_ratio = reco::deltaR(flightDir_0,flightDir_1)*SV_p4_1.pt()/vtxMass;

		    break;
		  }
	      }

	    // when only one tau axis has SVs assigned, they are all assigned to the 1st tau axis
	    // in the special case below need to swap values
	    if( (tau1_vertexMass<0 && tau2_vertexMass>0) )
	      {
		float temp = tau1_trackEtaRel_0;
		tau1_trackEtaRel_0= tau2_trackEtaRel_0;
		tau2_trackEtaRel_0= temp;

		temp = tau1_trackEtaRel_1;
		tau1_trackEtaRel_1= tau2_trackEtaRel_1;
		tau2_trackEtaRel_1= temp;

		temp = tau1_trackEtaRel_2;
		tau1_trackEtaRel_2= tau2_trackEtaRel_2;
		tau2_trackEtaRel_2= temp;

		temp = tau1_flightDistance2dSig;
		tau1_flightDistance2dSig= tau2_flightDistance2dSig;
		tau2_flightDistance2dSig= temp;

		tau1_vertexDeltaR= tau2_vertexDeltaR;

		temp = tau1_vertexEnergyRatio;
		tau1_vertexEnergyRatio= tau2_vertexEnergyRatio;
		tau2_vertexEnergyRatio= temp;

		temp = tau1_vertexMass;
		tau1_vertexMass= tau2_vertexMass;
		tau2_vertexMass= temp;
	      }

	    topjet.set_tag(TopJet::tagname2tag("z_ratio"), z_ratio);
	    topjet.set_tag(TopJet::tagname2tag("trackSipdSig_3"), trackSip3dSig_3);
	    topjet.set_tag(TopJet::tagname2tag("trackSipdSig_2"), trackSip3dSig_2);
	    topjet.set_tag(TopJet::tagname2tag("trackSipdSig_1"), trackSip3dSig_1);
	    topjet.set_tag(TopJet::tagname2tag("trackSipdSig_0"), trackSip3dSig_0);
	    topjet.set_tag(TopJet::tagname2tag("trackSipdSig_1_0"), tau2_trackSip3dSig_0);
	    topjet.set_tag(TopJet::tagname2tag("trackSipdSig_0_0"), tau1_trackSip3dSig_0);
	    topjet.set_tag(TopJet::tagname2tag("trackSipdSig_1_1"), tau2_trackSip3dSig_1);
	    topjet.set_tag(TopJet::tagname2tag("trackSipdSig_0_1"), tau1_trackSip3dSig_1);
	    topjet.set_tag(TopJet::tagname2tag("trackSip2dSigAboveCharm_0"), trackSip2dSigAboveCharm_0);
	    topjet.set_tag(TopJet::tagname2tag("trackSip2dSigAboveBottom_0"), trackSip2dSigAboveBottom_0);
	    topjet.set_tag(TopJet::tagname2tag("trackSip2dSigAboveBottom_1"), trackSip2dSigAboveBottom_1);
	    topjet.set_tag(TopJet::tagname2tag("tau1_trackEtaRel_0"), tau2_trackEtaRel_0);
	    topjet.set_tag(TopJet::tagname2tag("tau1_trackEtaRel_1"), tau2_trackEtaRel_1);
	    topjet.set_tag(TopJet::tagname2tag("tau1_trackEtaRel_2"), tau2_trackEtaRel_2);
	    topjet.set_tag(TopJet::tagname2tag("tau0_trackEtaRel_0"), tau1_trackEtaRel_0);
	    topjet.set_tag(TopJet::tagname2tag("tau0_trackEtaRel_1"), tau1_trackEtaRel_1);
	    topjet.set_tag(TopJet::tagname2tag("tau0_trackEtaRel_2"), tau1_trackEtaRel_2);
	    topjet.set_tag(TopJet::tagname2tag("tau_vertexMass_0"), tau1_vertexMass);
	    topjet.set_tag(TopJet::tagname2tag("tau_vertexEnergyRatio_0"), tau1_vertexEnergyRatio);
	    topjet.set_tag(TopJet::tagname2tag("tau_vertexDeltaR_0"), tau1_vertexDeltaR);
	    topjet.set_tag(TopJet::tagname2tag("tau_flightDistance2dSig_0"), tau1_flightDistance2dSig);
	    topjet.set_tag(TopJet::tagname2tag("tau_vertexMass_1"), tau2_vertexMass);
	    topjet.set_tag(TopJet::tagname2tag("tau_vertexEnergyRatio_1"), tau2_vertexEnergyRatio);
	    topjet.set_tag(TopJet::tagname2tag("tau_flightDistance2dSig_1"), tau2_flightDistance2dSig);
	    topjet.set_tag(TopJet::tagname2tag("jetNTracks"), jetNTracks);
	    topjet.set_tag(TopJet::tagname2tag("nSV"), nSV);


	    //End Higgs jet commissioning

	  }
	  
	}//higgs tag loop

        // loop over subjets to fill some more subjet info:
	if(subjet_src=="daughters"){
	  for (unsigned int k = 0; k < pat_topjet.numberOfDaughters(); k++) {
            Jet subjet;
            auto patsubjetd = dynamic_cast<const pat::Jet *>(pat_topjet.daughter(k));
            if (patsubjetd) {
	      try{
		NtupleWriterJets::fill_jet_info(*patsubjetd, subjet, do_btagging_subjets, do_taginfo_subjets);
	      }catch(runtime_error &){
		cerr << "Error in fill_jet_info for subjets in NtupleWriterTopJets with src = " << src << "." << endl;
		throw;
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
		throw runtime_error("subjet was nullptr");
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
		cerr << "Error in fill_jet_info for subjets in NtupleWriterTopJets with src = " << src << "." << endl;
		throw;
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
	      cerr << "Error in fill_jet_info for subjets in NtupleWriterTopJets with src = " << src << "." << endl;
	      throw;
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

void NtupleWriterTopJets::recalcNsubjettiness(const reco::JetBaseRef & jet, float & tau1, float & tau2, std::vector<fastjet::PseudoJet> & currentAxes)
{
  std::vector<fastjet::PseudoJet> fjParticles;

  // loop over jet constituents and push them in the vector of FastJet constituents
  for(const reco::CandidatePtr & daughter : jet->daughterPtrVector())
    {
      if ( daughter.isNonnull() && daughter.isAvailable() )
	fjParticles.push_back( fastjet::PseudoJet( daughter->px(), daughter->py(), daughter->pz(), daughter->energy() ) );
      else
	edm::LogWarning("MissingJetConstituent") << "Jet constituent required for N-subjettiness computation is missing!";
    }

  // N-subjettiness calculator
  fastjet::contrib::Njettiness njettiness(fastjet::contrib::OnePass_KT_Axes(), fastjet::contrib::NormalizedMeasure(beta_,R0_));

  // calculate N-subjettiness
  tau1 = njettiness.getTau(1, fjParticles);
  tau2 = njettiness.getTau(2, fjParticles);
  currentAxes = njettiness.currentAxes();
}


void NtupleWriterTopJets::setTracksPVBase(const reco::TrackRef & trackRef, const reco::VertexRef & vertexRef, float & PVweight)
{
  PVweight = 0.;

  const reco::TrackBaseRef trackBaseRef( trackRef );

  typedef reco::Vertex::trackRef_iterator IT;

  const reco::Vertex & vtx = *(vertexRef);
  // loop over tracks in vertices
  for(IT it=vtx.tracks_begin(); it!=vtx.tracks_end(); ++it)
    {
      const reco::TrackBaseRef & baseRef = *it;
      // one of the tracks in the vertex is the same as the track considered in the function
      if( baseRef == trackBaseRef )
	{
	  PVweight = vtx.trackWeight(baseRef);
	  break;
	}
    }
}


void NtupleWriterTopJets::setTracksPV(const reco::CandidatePtr & trackRef, const reco::VertexRef & vertexRef, float & PVweight)
{
  PVweight = 0.;

  const pat::PackedCandidate * pcand = dynamic_cast<const pat::PackedCandidate *>(trackRef.get());

  if(pcand) // MiniAOD case
    {
      if( pcand->fromPV() == pat::PackedCandidate::PVUsedInFit )
	{
	  PVweight = 1.;
	}
    }
  else
    {
      const reco::PFCandidate * pfcand = dynamic_cast<const reco::PFCandidate *>(trackRef.get());

      setTracksPVBase(pfcand->trackRef(), vertexRef, PVweight);
    }
}


void NtupleWriterTopJets::vertexKinematics(const reco::VertexCompositePtrCandidate & vertex, reco::TrackKinematics & vtxKinematics)
{
  const std::vector<reco::CandidatePtr> & tracks = vertex.daughterPtrVector();

  for(std::vector<reco::CandidatePtr>::const_iterator track = tracks.begin(); track != tracks.end(); ++track) {
    const reco::Track& mytrack = *(*track)->bestTrack();
    vtxKinematics.add(mytrack, 1.0);
  }
}


void NtupleWriterTopJets::etaRelToTauAxis(const reco::VertexCompositePtrCandidate & vertex,
                                                                    fastjet::PseudoJet & tauAxis, std::vector<float> & tau_trackEtaRel)
{
  math::XYZVector direction(tauAxis.px(), tauAxis.py(), tauAxis.pz());
  const std::vector<reco::CandidatePtr> & tracks = vertex.daughterPtrVector();

  for(std::vector<reco::CandidatePtr>::const_iterator track = tracks.begin(); track != tracks.end(); ++track)
    tau_trackEtaRel.push_back(std::abs(reco::btau::etaRel(direction.Unit(), (*track)->momentum())));
}

bool NtupleWriterTopJets::trackPairV0Filter(const reco::Track *const *tracks, unsigned int n)
 {
   // only check for K0s for now

   if (n != 2)
     return true;

   if (tracks[0]->charge() * tracks[1]->charge() > 0)
     return true;

   ROOT::Math::LorentzVector<ROOT::Math::PxPyPzM4D<double> > vec1;
   ROOT::Math::LorentzVector<ROOT::Math::PxPyPzM4D<double> > vec2;

   vec1.SetPx(tracks[0]->px());
   vec1.SetPy(tracks[0]->py());
   vec1.SetPz(tracks[0]->pz());
   vec1.SetM(reco::ParticleMasses::piPlus);

   vec2.SetPx(tracks[1]->px());
   vec2.SetPy(tracks[1]->py());
   vec2.SetPz(tracks[1]->pz());
   vec2.SetM(reco::ParticleMasses::piPlus);

   double invariantMass = (vec1 + vec2).M();
   if (std::abs(invariantMass - reco::ParticleMasses::k0) < 0.03)
     return false;

   return true;
}
