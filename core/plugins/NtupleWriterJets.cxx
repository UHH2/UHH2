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

void NtupleWriterJets::process(const edm::Event & event, uhh2::Event & uevent){
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
    bool sv_he = false, sv_hp = false, csv = false, jetp = false, jetbp = false, doubleak8 = false, doubleca15 = false;
    for(const auto & name_value : bdisc){
      const auto & name = name_value.first;
      const auto & value = name_value.second;
      if(name == "pfSimpleSecondaryVertexHighEffBJetTags"){
	jet.set_btag_simpleSecondaryVertexHighEff(value);
	sv_he = true;
      }
      else if(name == "pfSimpleSecondaryVertexHighPurBJetTags"){
	jet.set_btag_simpleSecondaryVertexHighPur(value);
	sv_hp = true;
      }
      else if(name == "pfCombinedInclusiveSecondaryVertexV2BJetTags"){
	jet.set_btag_combinedSecondaryVertex(value);
	csv = true;
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
    if(!sv_he || !sv_hp || !csv || !jetp || !jetbp || !doubleak8 ||!doubleca15){
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


void NtupleWriterTopJets::process(const edm::Event & event, uhh2::Event & uevent){

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

