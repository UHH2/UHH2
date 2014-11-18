
//set this flag to 1 when running in CMSSW_7_0_X, switch it to 0 for CMSSW_7_1_X and CMSSW_7_2_X
//#define CMSSW70 1

#include "FWCore/Utilities/interface/CPUTimer.h"

#include "UHH2/core/include/root-utils.h"
#include "UHH2/core/plugins/NtupleWriter.h"
#include "UHH2/core/plugins/JetProps.h"
#include "RecoBTau/JetTagComputer/interface/GenericMVAJetTagComputer.h"
#include "RecoBTau/JetTagComputer/interface/GenericMVAJetTagComputerWrapper.h"
#include "RecoBTau/JetTagComputer/interface/JetTagComputer.h"
#include "RecoBTau/JetTagComputer/interface/JetTagComputerRecord.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "TSystem.h"

namespace{
    
/// Use info from pat_jet to fill info in jet
void fill_jet_info(const pat::Jet & pat_jet, Jet & jet)
{
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
    jet.set_btag_combinedSecondaryVertex(pat_jet.bDiscriminator("combinedSecondaryVertexBJetTags"));
    jet.set_btag_combinedSecondaryVertexMVA(pat_jet.bDiscriminator("combinedSecondaryVertexMVABJetTags"));
    jet.set_btag_jetBProbability(pat_jet.bDiscriminator("jetBProbabilityBJetTags"));
    jet.set_btag_jetProbability(pat_jet.bDiscriminator("jetProbabilityBJetTags"));
}

}

namespace{

PFParticle PFCandidate2PFParticle_packed(const pat::PackedCandidate & pf, bool fromjet, bool fromiso, bool frompuiso){
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

PFParticle PFCandidate2PFParticle(const reco::PFCandidate & pf, bool fromjet, bool fromiso, bool frompuiso){
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

// add pf to pfs, ensuring there is no duplication. Retuns the index
// of pf in pfs. 
 size_t add_pfpart_packed(const pat::PackedCandidate & pf, vector<PFParticle> & pfs, bool fromjet, bool fromiso, bool frompuiso){
    for(size_t j=0; j<pfs.size(); ++j){
      PFParticle spf = pfs[j];
      // note: static_cast to float is to ensure the comparison is done with the same precision as these quantities
      // have been stored in spf. Otherwise, there could be a non-zero difference just because of conversion loss
      // from double to float.
      double r = fabs(static_cast<float>(pf.eta()) - spf.eta()) + fabs(static_cast<float>(pf.phi()) - spf.phi());
      double dpt = fabs(static_cast<float>(pf.pt()) - spf.pt());
      if (r == 0.0 && dpt == 0.0){
	if(fromjet) pfs[j].set_isJetParticle(true);
	if(fromiso) pfs[j].set_isIsoParticle(true);	
	if(frompuiso) pfs[j].set_isPUIsoParticle(true);
        return j;
      }            
    }
    pfs.push_back(PFCandidate2PFParticle_packed(pf, fromjet, fromiso, frompuiso));
    return pfs.size()-1;
}

size_t add_pfpart(const reco::PFCandidate & pf, vector<PFParticle> & pfs, bool fromjet, bool fromiso, bool frompuiso){
    for(size_t j=0; j<pfs.size(); ++j){
      PFParticle spf = pfs[j];
      // note: static_cast to float is to ensure the comparison is done with the same precision as these quantities
      // have been stored in spf. Otherwise, there could be a non-zero difference just because of conversion loss
      // from double to float.
      double r = fabs(static_cast<float>(pf.eta()) - spf.eta()) + fabs(static_cast<float>(pf.phi()) - spf.phi());
      double dpt = fabs(static_cast<float>(pf.pt()) - spf.pt());
      if (r == 0.0 && dpt == 0.0){
	if(fromjet) pfs[j].set_isJetParticle(true);
	if(fromiso) pfs[j].set_isIsoParticle(true);	
	if(frompuiso) pfs[j].set_isPUIsoParticle(true);
	return j;
      }
    }
    pfs.push_back(PFCandidate2PFParticle(pf, fromjet, fromiso, frompuiso));
    return pfs.size()-1;
}
    
size_t add_genpart(const reco::GenParticle & jetgenp, vector<GenParticle> & genparts){
   for(size_t j=0; j<genparts.size();j++){
     GenParticle sgenpart = genparts[j];
     double r = fabs(static_cast<float>(jetgenp.eta()-sgenpart.eta()))+fabs(static_cast<float>(jetgenp.phi()-sgenpart.phi()));
     double dpt = fabs(static_cast<float>(jetgenp.pt()-sgenpart.pt()));
     if (r == 0.0 && dpt == 0.0){
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
	 
   int nm=jetgenp.numberOfMothers();
   int nd=jetgenp.numberOfDaughters();

   if (nm>0) genp.set_mother1( jetgenp.motherRef(0).key());
   if (nm>1) genp.set_mother2( jetgenp.motherRef(1).key());
   if (nd>0) genp.set_daughter1( jetgenp.daughterRef(0).key());
   if (nd>1) genp.set_daughter2( jetgenp.daughterRef(1).key());
	 

   genparts.push_back(genp);
 
   return genparts.size()-1;
}

template<typename T>
void branch(TTree * tree, const char * bname, T t){
    if(tree){
        tree->Branch(bname, t);
    }
}

template<typename T>
void branch(TTree * tree, const char * bname, const char * type, T t){
    if(tree){
        tree->Branch(bname, type, t);
    }
}

}

namespace uhh2 {


class CMSSWContext: public uhh2::Context {
public:
    // does not take ownership of outfile
    CMSSWContext(GenericEventStructure & ges_, TFile * outfile_, TTree * outtree_): uhh2::Context(ges_), outfile(outfile_), outtree(outtree_){
    }
    
    /*virtual void write_entry(const std::string & treename){
        throw runtime_error("write_entry not implemented in CMSSW!");
    }*/
    
    virtual void put(const std::string & path, TH1 * h) override {
        if(!outfile) return;
        TDirectory * dir = outfile->GetDirectory(path.c_str());
        if(!dir){
            dir = outfile->mkdir(path.c_str());
            dir = outfile->GetDirectory(path.c_str());
        }
        if(!dir){
            throw runtime_error("Could not create directory '" + path + "' in outfile!");
        }
        h->SetDirectory(dir);
    }
    
    void setup_output_branches(Event & event){
        for(const auto & o : outputs){
            ptrs.push_back(event.get(o.ti, o.handle, false));
            tree_branch(outtree, o.bname, ptrs.back(), &ptrs.back(), o.ti);
        }
    }
    
private:
    virtual void do_declare_event_input(const std::type_info & ti, const std::string & bname, const std::string & mname) override {
        throw runtime_error("declare_event_input not implemented in CMSSW!");
    }
    
    virtual void do_declare_event_output(const std::type_info & ti, const std::string & bname, const std::string & mname) override {
        if(!outfile || !outtree) return;
        outputs.emplace_back(event_output{ti, bname, ges.get_raw_handle(ti, mname)});
    }
    
    TFile * outfile;
    TTree * outtree;
    
    struct event_output {
        const std::type_info & ti;
        std::string bname;
        Event::RawHandle handle;
    };
    std::vector<event_output> outputs;
    std::list<void*> ptrs;
};

}


NtupleWriter::NtupleWriter(const edm::ParameterSet& iConfig): outfile(0), tr(0), setup_output_branches_done(false) {
  fileName = iConfig.getParameter<std::string>("fileName");
  if(!fileName.empty()){
    outfile = new TFile(fileName.c_str(), "RECREATE");
    outfile->cd();
    tr = new TTree("AnalysisTree","AnalysisTree");
  }
  ges.reset(new uhh2::GenericEventStructure());
  if(iConfig.exists("AnalysisModule")){
      edm::ParameterSet module_pset = iConfig.getParameter<edm::ParameterSet>("AnalysisModule");
      string modulename = module_pset.getParameter<std::string>("name");
      string libname = module_pset.getParameter<std::string>("library");
      cout << "Trying to build AnalysisModule '" << modulename << "' from library '" << libname << "'" << endl;
      gSystem->Load(libname.c_str());
      context.reset(new uhh2::CMSSWContext(*ges, outfile, tr));
      auto names = module_pset.getParameterNamesForType<string>();
      for(const auto & name : names){
          context->set(name, module_pset.getParameter<std::string>(name));
      }
      module = uhh2::AnalysisModuleRegistry::build(modulename, *context);
  }
  event.reset(new uhh2::Event(*ges));
  
  std::cout<<"begin"<<std::endl;
  doElectrons = iConfig.getParameter<bool>("doElectrons");
  doMuons = iConfig.getParameter<bool>("doMuons");
  doTaus = iConfig.getParameter<bool>("doTaus"); 
  doJets = iConfig.getParameter<bool>("doJets");
  doGenJets = iConfig.getParameter<bool>("doGenJets");
  doGenTopJets = iConfig.getParameter<bool>("doGenTopJets");
  doGenJetsWithParts = iConfig.getParameter<bool>("doGenJetsWithParts");
  doPhotons = iConfig.getParameter<bool>("doPhotons");
  doMET = iConfig.getParameter<bool>("doMET");
  doGenInfo = iConfig.getParameter<bool>("doGenInfo");
  doAllGenParticles = iConfig.getParameter<bool>("doAllGenParticles");
  doPV = iConfig.getParameter<bool>("doPV");
  doTopJets = iConfig.getParameter<bool>("doTopJets");
  doTopJetsConstituents = iConfig.getParameter<bool>("doTopJetsConstituents");
  doJetsConstituents = iConfig.getParameter<bool>("doJetsConstituents");
  doTrigger = iConfig.getParameter<bool>("doTrigger");
  SVComputer_  = iConfig.getUntrackedParameter<edm::InputTag>("svComputer",edm::InputTag("combinedSecondaryVertex"));
  doTagInfos = iConfig.getUntrackedParameter<bool>("doTagInfos",false);
  doRho = iConfig.getUntrackedParameter<bool>("doRho",true);
  storePFsAroundLeptons = iConfig.getUntrackedParameter<bool>("storePFsAroundLeptons",false);
  doAllPFConstituents = iConfig.getParameter<bool>("doAllPFConstituents");
  runOnMiniAOD = iConfig.getParameter<bool>("runOnMiniAOD");

  // initialization of tree variables

  branch(tr, "run",&event->run);
  branch(tr, "event",&event->event);
  branch(tr, "luminosityBlock",&event->luminosityBlock);
  branch(tr, "isRealData",&event->isRealData);
  branch(tr, "rho",&event->rho);
  //always create rho branch, as some SFrame modules rely on it being present; only fill it
  // if doRho is true.
  if(doRho){
     auto rho_source = iConfig.getParameter<edm::InputTag>("rho_source");
     rho_token = consumes<double>(rho_source);
  }
  else{
    event->rho = -1;
  }

  if(doPV){
    branch(tr, "beamspot_x0",&event->beamspot_x0);
    branch(tr, "beamspot_y0",&event->beamspot_y0);
    branch(tr, "beamspot_z0",&event->beamspot_z0);
  }
  if(doElectrons){
    auto electron_sources = iConfig.getParameter<std::vector<std::string> >("electron_sources");
    eles.resize(electron_sources.size());
    for(size_t j=0; j< electron_sources.size(); ++j){  
      electron_tokens.emplace_back(consumes<pat::ElectronCollection>(electron_sources[j]));
      branch(tr, electron_sources[j].c_str(), "std::vector<Electron>", &eles[j]); 
    }
    if(!electron_sources.empty()){
        event->electrons = &eles[0];
    }
  }
  if(doMuons){
    auto muon_sources = iConfig.getParameter<std::vector<std::string> >("muon_sources");
    mus.resize(muon_sources.size());
    for(size_t j=0; j< muon_sources.size(); ++j){
      muon_tokens.emplace_back(consumes<pat::MuonCollection>(muon_sources[j]));
      branch(tr, muon_sources[j].c_str(), "std::vector<Muon>", &mus[j]);
    }
    if(!muon_sources.empty()){
        event->muons = &mus[0];
    }
  }
  if(doTaus){
    auto tau_sources = iConfig.getParameter<std::vector<std::string> >("tau_sources");
    tau_ptmin = iConfig.getParameter<double> ("tau_ptmin");
    tau_etamax = iConfig.getParameter<double> ("tau_etamax");
    taus.resize(tau_sources.size());
    for(size_t j=0; j< tau_sources.size(); ++j){
       tau_tokens.emplace_back(consumes<pat::TauCollection>(tau_sources[j]));
       branch(tr, tau_sources[j].c_str(), "std::vector<Tau>", &taus[j]);
    }
    if(!tau_sources.empty()){
        event->taus = &taus[0];
    }
  }
  if(doJets){
    auto jet_sources = iConfig.getParameter<std::vector<std::string> >("jet_sources");
    jet_ptmin = iConfig.getParameter<double> ("jet_ptmin");
    jet_etamax = iConfig.getParameter<double> ("jet_etamax");
    jets.resize(jet_sources.size());
    for(size_t j=0; j< jet_sources.size(); ++j){
        jet_tokens.push_back(consumes<vector<pat::Jet>>(jet_sources[j]));
        branch(tr, jet_sources[j].c_str(), "std::vector<Jet>", &jets[j]);
    }
    if(!jet_sources.empty()){
        event->jets = &jets[0];
    }
  }
  if(doGenJets){
    auto genjet_sources = iConfig.getParameter<std::vector<std::string> >("genjet_sources");
    genjet_ptmin = iConfig.getParameter<double> ("genjet_ptmin");
    genjet_etamax = iConfig.getParameter<double> ("genjet_etamax");
    genjets.resize(genjet_sources.size());
    for(size_t j=0; j< genjet_sources.size(); ++j){
        genjet_tokens.push_back(consumes<vector<reco::GenJet>>(genjet_sources[j]));
        branch(tr, genjet_sources[j].c_str(), "std::vector<Particle>", &genjets[j]);
    }
    if(!genjet_sources.empty()){
        event->genjets = &genjets[0];
    }
  }
  if(doTopJets){
    auto topjet_sources = iConfig.getParameter<std::vector<std::string> >("topjet_sources");
    topjet_ptmin = iConfig.getParameter<double> ("topjet_ptmin");
    topjet_etamax = iConfig.getParameter<double> ("topjet_etamax");
    topjets.resize(topjet_sources.size());
    for(size_t j=0; j< topjet_sources.size(); ++j){
      topjet_tokens.push_back(consumes<vector<pat::Jet>>(topjet_sources[j]));
      branch(tr, topjet_sources[j].c_str(), "std::vector<TopJet>", &topjets[j]);
    }
    if(!topjet_sources.empty()){
        event->topjets = &topjets[0];
    }
  }
  if(doTopJetsConstituents || doJetsConstituents || doAllPFConstituents || storePFsAroundLeptons){
      // ... but only create branch if configured to do so:
      branch(tr, "PFParticles", "std::vector<PFParticle>", &pfparticles);
  }
  if(doTopJetsConstituents) topjet_constituents_sources = iConfig.getParameter<std::vector<std::string> >("topjet_constituents_sources");
  if(doAllPFConstituents) pf_constituents_sources = iConfig.getParameter<std::vector<std::string> >("pf_constituents_sources");
  if(doGenTopJets){
    auto gentopjet_sources = iConfig.getParameter<std::vector<std::string> >("gentopjet_sources");
    gentopjet_ptmin = iConfig.getParameter<double> ("gentopjet_ptmin");
    gentopjet_etamax = iConfig.getParameter<double> ("gentopjet_etamax");
    gentopjets.resize(gentopjet_sources.size());
    for(size_t j=0; j< gentopjet_sources.size(); ++j){
      gentopjet_tokens.push_back(consumes<vector<reco::BasicJetCollection>>(gentopjet_sources[j]));
      branch(tr, gentopjet_sources[j].c_str(), "std::vector<GenTopJet>", &gentopjets[j]);
    }
    if(!gentopjet_sources.empty()){
        event->gentopjets = &gentopjets[0];
    }
  }
  if(doGenJetsWithParts){
    auto genjetwithparts_sources = iConfig.getParameter<std::vector<std::string> >("genjetwithparts_sources");
    genjetwithparts_ptmin = iConfig.getParameter<double> ("genjetwithparts_ptmin");
    genjetwithparts_etamax = iConfig.getParameter<double> ("genjetwithparts_etamax");
    genjetwithparts.resize(genjetwithparts_sources.size());
    for(size_t j=0; j< genjetwithparts_sources.size(); ++j){
      genjetwithparts_tokens.push_back(consumes<vector<reco::GenJet>>(genjetwithparts_sources[j]));
      branch(tr, genjetwithparts_sources[j].c_str(), "std::vector<GenJetWithParts>", &genjetwithparts[j]);
    }
  }

  if(doPhotons){
    auto photon_sources = iConfig.getParameter<std::vector<std::string> >("photon_sources");
    phs.resize(photon_sources.size());
    for(size_t j=0; j< photon_sources.size(); ++j){
      photon_tokens.push_back(consumes<vector<pat::PhotonCollection>>(photon_sources[j]));
      branch(tr, photon_sources[j].c_str(), "std::vector<Photon>", &phs[j]);
    }
    if(!photon_sources.empty()){
        event->photons = &phs[0];
    }
  }
  if(doMET){
    auto met_sources = iConfig.getParameter<std::vector<std::string> >("met_sources");
    met.resize(met_sources.size());
    for(size_t j=0; j< met_sources.size(); ++j){  
      met_tokens.push_back(consumes<vector<pat::MET>>(met_sources[j]));
      branch(tr, met_sources[j].c_str(), "MET", &met[j]);
    }
    if(!met_sources.empty()){
        event->met = &met[0];
    }
  }
  if(doPV){
    auto pv_sources = iConfig.getParameter<std::vector<std::string> >("pv_sources");
    pvs.resize(pv_sources.size());
    for(size_t j=0; j< pv_sources.size(); ++j){  
      pv_tokens.push_back(consumes<vector<reco::Vertex> >(pv_sources[j]));
      branch(tr, pv_sources[j].c_str(), "std::vector<PrimaryVertex>", &pvs[j]);
    }
    if(!pv_sources.empty()){
        event->pvs = &pvs[0];
    }
  }
  if(doGenInfo){
    genparticle_source= iConfig.getParameter<edm::InputTag>("genparticle_source");
    if(runOnMiniAOD && doAllGenParticles) stablegenparticle_source = iConfig.getParameter<edm::InputTag>("stablegenparticle_source");
    event->genInfo = new GenInfo();
    event->genparticles = new vector<GenParticle>();
    branch(tr, "genInfo","GenInfo", event->genInfo);
    branch(tr, "GenParticles","std::vector<GenParticle>", event->genparticles);
  }
  if(doTrigger){
    trigger_prefixes = iConfig.getParameter<std::vector<std::string> >("trigger_prefixes");
    event->get_triggerResults() = new vector<bool>();
    branch(tr, "triggerNames", "std::vector<std::string>", &triggerNames_outbranch);
    branch(tr, "triggerResults", "std::vector<bool>", event->get_triggerResults());
    triggerBits_ = consumes<edm::TriggerResults>(iConfig.getParameter<edm::InputTag>("trigger_bits"));
  }
  if (storePFsAroundLeptons){
    pf_around_leptons_sources = iConfig.getParameter<std::vector<std::string> >("pf_around_leptons_sources");
  }
  newrun = true;
}


NtupleWriter::~NtupleWriter() {}

namespace {
    inline void print_times(const edm::CPUTimer & timer, const char * point){
        // uncomment to study timing.
        //cout << point << ": cpu = "<< (timer.cpuTime() * 1000.) << "ms; real = " << (timer.realTime() * 1000.) << "ms" << endl;
        
        // To interpret timing results, keep in mind that event.getByLabel in general
        // triggers reading the product from the input file (which needs some time), while for event.getByToken
        // hardly requires any time, as the framework pre-fetches these objects (it can do that because it knows via
        // consumes<> what to pre-fetch). So superficially, it might look as if performance improves
        // when switching from getByLabel to getByToken, but actually, the time to read the product is just spent elsewhere,
        // and the overall performance actually hardly changes.
    }
}

// ------------ method called for each event  ------------
bool NtupleWriter::filter(edm::Event& iEvent, const edm::EventSetup& iSetup) {
   edm::CPUTimer timer;
   timer.start();
   pfparticles.clear();
   
   event->weight = 1.0;
   event->run = iEvent.id().run();
   event->event = iEvent.id().event();
   event->luminosityBlock = iEvent.luminosityBlock();
   event->isRealData      = iEvent.isRealData();

   if(doRho){
      edm::Handle<double> m_rho;
      iEvent.getByToken(rho_token, m_rho);
      event->rho=*m_rho;
   }
   
   print_times(timer, "rho");

   // ------------- primary vertices and beamspot  -------------

   if(doPV){
     for(size_t j=0; j< pv_tokens.size(); ++j){
       pvs[j].clear();
       edm::Handle< std::vector<reco::Vertex> > pv_handle;
       iEvent.getByToken(pv_tokens[j], pv_handle);
       const std::vector<reco::Vertex>& reco_pvs = *pv_handle;
       for (unsigned int i = 0; i <  reco_pvs.size(); ++i) {
	 reco::Vertex reco_pv = reco_pvs[i];
	 PrimaryVertex pv;
	 pv.set_x( reco_pv.x());
	 pv.set_y( reco_pv.y());
	 pv.set_z( reco_pv.z());
	 pv.set_nTracks( reco_pv.nTracks());
	 //pv.set_isValid( reco_pv.isValid());
	 pv.set_chi2( reco_pv.chi2());
	 pv.set_ndof( reco_pv.ndof());	 

	 pvs[j].push_back(pv);
       }
     }
      
     edm::Handle<reco::BeamSpot> beamSpot;
     iEvent.getByLabel(edm::InputTag("offlineBeamSpot"), beamSpot);
     const reco::BeamSpot & bsp = *beamSpot;
     
     event->beamspot_x0 = bsp.x0();
     event->beamspot_y0 = bsp.y0();
     event->beamspot_z0 = bsp.z0();
   }
   
   print_times(timer, "pv");

   // ------------- generator info -------------
   
   if(doGenInfo && event->genInfo){
     event->genInfo->clear_weights();
     event->genInfo->clear_binningValues();
     event->genparticles->clear();

     edm::Handle<GenEventInfoProduct> genEventInfoProduct;
     iEvent.getByLabel("generator", genEventInfoProduct);
     const GenEventInfoProduct& genEventInfo = *(genEventInfoProduct.product());
  
     for(unsigned int k=0; k<genEventInfo.binningValues().size();++k){
       event->genInfo->add_binningValue(genEventInfo.binningValues().at(k));
     }
     for(unsigned int k=0; k<genEventInfo.weights().size();++k){
       event->genInfo->add_weight(genEventInfo.weights().at(k));
     }
     event->genInfo->set_alphaQCD(genEventInfo.alphaQCD());
     event->genInfo->set_alphaQED(genEventInfo.alphaQED());
     event->genInfo->set_qScale(genEventInfo.qScale());
     
     const gen::PdfInfo* pdf = genEventInfo.pdf();
     if(pdf){
       event->genInfo->set_pdf_id1(pdf->id.first);
       event->genInfo->set_pdf_id2(pdf->id.second); 
       event->genInfo->set_pdf_x1(pdf->x.first);
       event->genInfo->set_pdf_x2(pdf->x.second);
       event->genInfo->set_pdf_scalePDF(pdf->scalePDF);
       event->genInfo->set_pdf_xPDF1(pdf->xPDF.first);
       event->genInfo->set_pdf_xPDF2(pdf->xPDF.second); 
     }
     else{
       event->genInfo->set_pdf_id1(-999);
       event->genInfo->set_pdf_id2(-999); 
       event->genInfo->set_pdf_x1(-999);
       event->genInfo->set_pdf_x2(-999);
       event->genInfo->set_pdf_scalePDF(-999);
       event->genInfo->set_pdf_xPDF1(-999);
       event->genInfo->set_pdf_xPDF2(-999);
     }

     edm::Handle<std::vector<PileupSummaryInfo> > pus;
     iEvent.getByLabel(edm::InputTag("addPileupInfo"), pus);
     event->genInfo->set_pileup_NumInteractions_intime(0);
     event->genInfo->set_pileup_NumInteractions_ootbefore(0);
     event->genInfo->set_pileup_NumInteractions_ootafter(0);
     if(pus.isValid()){
       event->genInfo->set_pileup_TrueNumInteractions ( (float) pus->at(0).getTrueNumInteractions());
       for(size_t i=0; i<pus->size(); ++i){
	 if(pus->at(i).getBunchCrossing() == 0) // intime pileup
	   event->genInfo->set_pileup_NumInteractions_intime( event->genInfo->pileup_NumInteractions_intime() + pus->at(i).getPU_NumInteractions());
	 else if(pus->at(i).getBunchCrossing() == -1){ // oot pileup before
	   event->genInfo->set_pileup_NumInteractions_ootbefore( event->genInfo->pileup_NumInteractions_ootbefore() + pus->at(i).getPU_NumInteractions());
	 }
	 else if(pus->at(i).getBunchCrossing() == +1){ // oot pileup before
	   event->genInfo->set_pileup_NumInteractions_ootafter( event->genInfo->pileup_NumInteractions_ootafter() + pus->at(i).getPU_NumInteractions());
	 }
       }
     }

     edm::Handle<reco::GenParticleCollection> genPartColl;
     // use genPartColl for the Matrix-Element particles. Also use it for stable leptons
     // in case doAllGenParticles is false.
     iEvent.getByLabel(genparticle_source, genPartColl);
     int index=-1;
     for(reco::GenParticleCollection::const_iterator iter = genPartColl->begin(); iter != genPartColl->end(); ++ iter){
       index++;
       //write out only leptons and matrix-element particles (for doAllGenParticles, write all)
       bool islepton = iter->status()==1 && abs(iter->pdgId())>=11 && abs(iter->pdgId())<=16 ;
       bool is_me = iter->status()==3 || (iter->status()>=20 && iter->status()<=30); // 3 for MadGraph, 20--30 for Pythia8
       if(is_me || islepton ||  doAllGenParticles){
	 //in case we store all stable particles later anyway: do not store here to avoid duplication.
	 if(doAllGenParticles && runOnMiniAOD && iter->status()==1){
             continue;
         }

	 GenParticle genp;
	 genp.set_charge(iter->charge());
	 genp.set_pt(iter->p4().pt());
	 genp.set_eta(iter->p4().eta());
	 genp.set_phi(iter->p4().phi());
	 genp.set_energy(iter->p4().E());
	 genp.set_index(index);
	 genp.set_status( iter->status());
	 genp.set_pdgId( iter->pdgId());

	 genp.set_mother1(-1);
	 genp.set_mother2(-1);
	 genp.set_daughter1(-1);
	 genp.set_daughter2(-1);
	 
	 int nm=iter->numberOfMothers();
	 int nd=iter->numberOfDaughters();

	 
	 if (nm>0) genp.set_mother1( iter->motherRef(0).key());
	 if (nm>1) genp.set_mother2( iter->motherRef(1).key());
	 if (nd>0) genp.set_daughter1( iter->daughterRef(0).key());
	 if (nd>1) genp.set_daughter2( iter->daughterRef(1).key());

	 event->genparticles->push_back(genp);

       }
     }
     
     //store stable gen particles from packed collection
     if(doAllGenParticles && runOnMiniAOD){
       edm::Handle<edm::View<pat::PackedGenParticle> > packed;
       // use packed particle collection for all STABLE (status 1) particles
       iEvent.getByLabel(stablegenparticle_source,packed);

       for(size_t j=0; j<packed->size();j++){

	 const pat::PackedGenParticle* iter = &(*packed)[j];
	 if(iter->status()!=1) continue;

	 index++;

	 GenParticle genp;
	 genp.set_charge(iter->charge());
	 genp.set_pt(iter->p4().pt());
	 genp.set_eta(iter->p4().eta());
	 genp.set_phi(iter->p4().phi());
	 genp.set_energy(iter->p4().E());
	 genp.set_index(index);
	 genp.set_status( iter->status());
	 genp.set_pdgId( iter->pdgId());

	 genp.set_mother1(-1);
	 genp.set_mother2(-1);
	 genp.set_daughter1(-1);
	 genp.set_daughter2(-1);

	 bool islepton = abs(iter->pdgId())>=11 && abs(iter->pdgId())<=16 ;
	  //check, if particle has already been filled in previous routine from reco::GenParticleCollection 
	 // bool fill=true;
	 // for(unsigned int i=0; i< genps.size(); ++i){
	 //    if(genps[i].status()==1 && genps[i].pdgId()==genp.pdgId() && fabs(genps[i].pt()-genp.pt())<0.1 && fabs(genps[i].eta()-genp.eta())<0.1 && islepton){
	 //      std::cout << "Doppelt: " << genps[i].status() << "  " << genps[i].pt() << "  " << genp.pt() <<"    "<<  genps[i].eta() << "  " <<   genp.eta() << "  " <<  genps[i].pdgId() << "  " << genps[i].mother1() << "  " << genps[i].mother2()<< std::endl;
	 //      fill=false;
	 //      break;
	 //    }
	 //  }
	 //  if(fill) {

	 //    genps.push_back(genp);

	 //    if (islepton) std::cout << "Nicht Doppelt: " << genp.status() << "  " << genp.pt() << "  " << genp.eta() << "  " << genp.pdgId() << std::endl;
	 // }

	 if(!islepton) {
             event->genparticles->push_back(genp);
         }
       }
     }
 
   }
   
   print_times(timer, "geninfo");

   // ------------- full PF collection -------------   

   if(doAllPFConstituents){
     if(runOnMiniAOD){
       for(unsigned int k=0; k<pf_constituents_sources.size(); ++k){
	 edm::Handle< std::vector<pat::PackedCandidate> > pfcand_handle;
	 iEvent.getByLabel(pf_around_leptons_sources[k], pfcand_handle);  
	 const std::vector<pat::PackedCandidate>& pf_cands = *(pfcand_handle.product());
	 
	 for (unsigned int i=0; i<pf_cands.size(); ++i){
	   const pat::PackedCandidate & pf = pf_cands[i];
	   add_pfpart_packed(pf, pfparticles, false, false, false);    
	 }
       }
     }
     else{
       for(unsigned int k=0; k<pf_constituents_sources.size(); ++k){
	 edm::Handle< std::vector<reco::PFCandidate> > pfcand_handle;
	 iEvent.getByLabel(pf_around_leptons_sources[k], pfcand_handle);  
	 const std::vector<reco::PFCandidate>& pf_cands = *(pfcand_handle.product());
	 
	 for (unsigned int i=0; i<pf_cands.size(); ++i){
	   const reco::PFCandidate & pf = pf_cands[i];
	   add_pfpart(pf, pfparticles, false, false, false);    
	 }
       }
     }
   }


   // ------------- electrons -------------   
   if(doElectrons){
     for(size_t j=0; j< electron_tokens.size(); ++j){
       eles[j].clear();
       edm::Handle< std::vector<pat::Electron> > ele_handle;
       iEvent.getByToken(electron_tokens[j], ele_handle);
       const std::vector<pat::Electron>& pat_electrons = *ele_handle;
       
       for (unsigned int i = 0; i < pat_electrons.size(); ++i) {
	 const pat::Electron & pat_ele = pat_electrons[i];
         eles[j].emplace_back();
	 Electron & ele = eles[j].back();
	 
	 ele.set_charge( pat_ele.charge());
	 ele.set_pt( pat_ele.pt());
	 ele.set_eta( pat_ele.eta());
	 ele.set_phi( pat_ele.phi());
	 ele.set_energy( pat_ele.energy());
	 ele.set_vertex_x(pat_ele.vertex().x());
	 ele.set_vertex_y(pat_ele.vertex().y());
	 ele.set_vertex_z(pat_ele.vertex().z());
	 ele.set_supercluster_eta(pat_ele.superCluster()->eta());
	 ele.set_supercluster_phi(pat_ele.superCluster()->phi());
	 ele.set_dB(pat_ele.dB());
	 //ele.set_particleIso(pat_ele.particleIso());
	 ele.set_neutralHadronIso(pat_ele.neutralHadronIso());
	 ele.set_chargedHadronIso(pat_ele.chargedHadronIso());
	 ele.set_trackIso(pat_ele.trackIso());
	 ele.set_photonIso(pat_ele.photonIso());
	 ele.set_puChargedHadronIso(pat_ele.puChargedHadronIso());
#if CMSSW70 == 1
	 ele.set_gsfTrack_trackerExpectedHitsInner_numberOfLostHits(pat_ele.gsfTrack()->trackerExpectedHitsInner().numberOfLostHits());
#else
	 ele.set_gsfTrack_trackerExpectedHitsInner_numberOfLostHits(pat_ele.gsfTrack()->hitPattern().numberOfLostTrackerHits(reco::HitPattern::MISSING_INNER_HITS));
#endif
	 ele.set_gsfTrack_px( pat_ele.gsfTrack()->px());
	 ele.set_gsfTrack_py( pat_ele.gsfTrack()->py());
	 ele.set_gsfTrack_pz( pat_ele.gsfTrack()->pz());
	 ele.set_gsfTrack_vx( pat_ele.gsfTrack()->vx());
	 ele.set_gsfTrack_vy( pat_ele.gsfTrack()->vy());
	 ele.set_gsfTrack_vz( pat_ele.gsfTrack()->vz());
	 ele.set_passconversionveto(pat_ele.passConversionVeto());
	 ele.set_dEtaIn(pat_ele.deltaEtaSuperClusterTrackAtVtx());
	 ele.set_dPhiIn(pat_ele.deltaPhiSuperClusterTrackAtVtx());
	 ele.set_sigmaIEtaIEta(pat_ele.sigmaIetaIeta());
	 ele.set_HoverE(pat_ele.hadronicOverEm());
	 ele.set_fbrem(pat_ele.fbrem());
	 ele.set_EoverPIn(pat_ele.eSuperClusterOverP());
	 ele.set_EcalEnergy(pat_ele.ecalEnergy());
	 //not in miniaod
	 //ele.set_mvaTrigV0(pat_ele.electronID("mvaTrigV0"));
	 //ele.set_mvaNonTrigV0(pat_ele.electronID("mvaNonTrigV0"));
	 float AEff03 = 0.00;
	 if(event->isRealData){
	   AEff03 = ElectronEffectiveArea::GetElectronEffectiveArea(ElectronEffectiveArea::kEleGammaAndNeutralHadronIso03, pat_ele.superCluster()->eta(), ElectronEffectiveArea::kEleEAData2011);
	 }else{
	   AEff03 = ElectronEffectiveArea::GetElectronEffectiveArea(ElectronEffectiveArea::kEleGammaAndNeutralHadronIso03, pat_ele.superCluster()->eta(), ElectronEffectiveArea::kEleEAFall11MC);
	 }
	 ele.set_AEff(AEff03);
	 
	 if (storePFsAroundLeptons){
	   if(runOnMiniAOD){
	     for(unsigned int k=0; k<pf_around_leptons_sources.size(); ++k){
	       edm::Handle< std::vector<pat::PackedCandidate> > pfcand_handle;
	       iEvent.getByLabel(pf_around_leptons_sources[k], pfcand_handle);  
	       const std::vector<pat::PackedCandidate>& pf_cands = *(pfcand_handle.product());
	       StorePFCandsInCone_packed(&ele, pf_cands, 0.4);
	     }
	   }
	   else{
	     for(unsigned int k=0; k<pf_around_leptons_sources.size(); ++k){
	       edm::Handle< std::vector<reco::PFCandidate> > pfcand_handle;
	       iEvent.getByLabel(pf_around_leptons_sources[k], pfcand_handle);
	       const std::vector<reco::PFCandidate>& pf_cands = *(pfcand_handle.product());
	       bool frompu = true;
	       if( pf_around_leptons_sources[k].find("NoPileUp") < pf_around_leptons_sources[k].size()) frompu=false;
	       StorePFCandsInCone(&ele, pf_cands, 0.4, frompu);
	     }
	   }
	 }

       }
     }
   }
   
   print_times(timer, "electrons");

   // ------------- muons ------------- 
   if(doMuons){
     for(size_t j=0; j< muon_tokens.size(); ++j){
       mus[j].clear();
       
       edm::Handle< std::vector<pat::Muon> > mu_handle;
       iEvent.getByToken(muon_tokens[j], mu_handle);
       const std::vector<pat::Muon>& pat_muons = *mu_handle;
       
       for (unsigned int i = 0; i <pat_muons.size() ; ++i) {
	 const pat::Muon & pat_mu = pat_muons[i];
         mus[j].emplace_back();
	 Muon & mu = mus[j].back();
	 mu.set_charge( pat_mu.charge());
	 mu.set_pt( pat_mu.pt());
	 mu.set_eta( pat_mu.eta());
	 mu.set_phi( pat_mu.phi());
	 mu.set_energy( pat_mu.energy());
	 mu.set_vertex_x ( pat_mu.vertex().x());
	 mu.set_vertex_y ( pat_mu.vertex().y());
	 mu.set_vertex_z ( pat_mu.vertex().z());
	 mu.set_dB ( pat_mu.dB());
	 //mu.particleIso ( pat_mu.particleIso());
	 mu.set_neutralHadronIso ( pat_mu.neutralHadronIso());
	 mu.set_chargedHadronIso ( pat_mu.chargedHadronIso());
	 mu.set_trackIso ( pat_mu.trackIso());
	 mu.set_photonIso ( pat_mu.photonIso());
	 mu.set_puChargedHadronIso ( pat_mu.puChargedHadronIso());
	 mu.set_isGlobalMuon ( pat_mu.isGlobalMuon());
	 mu.set_isPFMuon ( pat_mu.isPFMuon());
	 mu.set_isStandAloneMuon ( pat_mu.isStandAloneMuon());
	 mu.set_isTrackerMuon ( pat_mu.isTrackerMuon());
	 mu.set_numberOfMatchedStations ( pat_mu.numberOfMatchedStations());
	 reco::TrackRef globalTrack = pat_mu.globalTrack();
	 if(!globalTrack.isNull()){
	   mu.set_globalTrack_chi2 ( globalTrack->chi2());
	   mu.set_globalTrack_ndof ( globalTrack->ndof());
	   mu.set_globalTrack_d0 ( globalTrack->d0());	 
	   mu.set_globalTrack_d0Error ( globalTrack->d0Error());
	   mu.set_globalTrack_numberOfValidHits ( globalTrack->numberOfValidHits());
	   mu.set_globalTrack_numberOfLostHits ( globalTrack->numberOfLostHits());
	   mu.set_globalTrack_numberOfValidMuonHits(globalTrack->hitPattern().numberOfValidMuonHits() );
	 }
	 else{
	   mu.set_globalTrack_chi2 ( 0);
	   mu.set_globalTrack_ndof ( 0);
	   mu.set_globalTrack_d0 ( 0);
	   mu.set_globalTrack_d0Error ( 0);
	   mu.set_globalTrack_numberOfValidHits ( 0);
	   mu.set_globalTrack_numberOfLostHits ( 0);
	 }
	 reco::TrackRef innerTrack = pat_mu.innerTrack();
	 if(!innerTrack.isNull()){
	   mu.set_innerTrack_chi2 ( innerTrack->chi2());
	   mu.set_innerTrack_ndof ( innerTrack->ndof());
	   mu.set_innerTrack_d0 ( innerTrack->d0());	 
	   mu.set_innerTrack_d0Error ( innerTrack->d0Error());
	   mu.set_innerTrack_numberOfValidHits ( innerTrack->numberOfValidHits());
	   mu.set_innerTrack_numberOfLostHits ( innerTrack->numberOfLostHits());
	   mu.set_innerTrack_trackerLayersWithMeasurement ( innerTrack->hitPattern().trackerLayersWithMeasurement());
	   mu.set_innerTrack_numberOfValidPixelHits ( innerTrack->hitPattern().numberOfValidPixelHits());
	 }
	 else{
	   mu.set_innerTrack_chi2 ( 0);
	   mu.set_innerTrack_ndof ( 0);
	   mu.set_innerTrack_d0 ( 0);
	   mu.set_innerTrack_d0Error ( 0);
	   mu.set_innerTrack_numberOfValidHits ( 0);
	   mu.set_innerTrack_numberOfLostHits ( 0);
	   mu.set_innerTrack_trackerLayersWithMeasurement ( 0);
	   mu.set_innerTrack_numberOfValidPixelHits ( 0);
	 }
	 reco::TrackRef outerTrack = pat_mu.outerTrack();
	 if(!outerTrack.isNull()){
	   mu.set_outerTrack_chi2 ( outerTrack->chi2());
	   mu.set_outerTrack_ndof ( outerTrack->ndof());
	   mu.set_outerTrack_d0 ( outerTrack->d0());	 
	   mu.set_outerTrack_d0Error ( outerTrack->d0Error());
	   mu.set_outerTrack_numberOfValidHits ( outerTrack->numberOfValidHits());
	   mu.set_outerTrack_numberOfLostHits ( outerTrack->numberOfLostHits());
	 } 
	 else{
	   mu.set_outerTrack_chi2 ( 0);
	   mu.set_outerTrack_ndof ( 0);
	   mu.set_outerTrack_d0 ( 0);
	   mu.set_outerTrack_d0Error ( 0);
	   mu.set_outerTrack_numberOfValidHits ( 0);
	   mu.set_outerTrack_numberOfLostHits ( 0);
	 }

	 if (storePFsAroundLeptons){
	   if(runOnMiniAOD){
	     for(unsigned int k=0; k<pf_around_leptons_sources.size(); ++k){
	       edm::Handle< std::vector<pat::PackedCandidate> > pfcand_handle;
	       iEvent.getByLabel(pf_around_leptons_sources[k], pfcand_handle);  
	       const std::vector<pat::PackedCandidate>& pf_cands = *(pfcand_handle.product());
	       StorePFCandsInCone_packed(&mu, pf_cands, 0.4);
	     }
	   }
	   else{
	     for(unsigned int k=0; k<pf_around_leptons_sources.size(); ++k){
	       edm::Handle< std::vector<reco::PFCandidate> > pfcand_handle;
	       iEvent.getByLabel(pf_around_leptons_sources[k], pfcand_handle);
	       const std::vector<reco::PFCandidate>& pf_cands = *(pfcand_handle.product());
	       bool frompu = true;
	       if( pf_around_leptons_sources[k].find("NoPileUp") < pf_around_leptons_sources[k].size()) frompu=false;
	       StorePFCandsInCone(&mu, pf_cands, 0.4, frompu);
	     }
	   }
	 }

       }
     }
   }
   
   print_times(timer, "muons");
   // ------------- taus ------------- 

   if(doTaus){
     for(size_t j=0; j< tau_tokens.size(); ++j){
       taus[j].clear();
       
       edm::Handle< std::vector<pat::Tau> > tau_handle;
       iEvent.getByToken(tau_tokens[j], tau_handle);
       const std::vector<pat::Tau>& pat_taus = *tau_handle;
       
       for (unsigned int i = 0; i < pat_taus.size(); ++i) {
	 const pat::Tau & pat_tau = pat_taus[i];
	 if(pat_tau.pt() < tau_ptmin) continue;
	 if(fabs(pat_tau.eta()) > tau_etamax) continue;

         taus[j].emplace_back();
         Tau & tau = taus[j].back();
         
	 tau.set_charge( pat_tau.charge());
	 tau.set_pt( pat_tau.pt());
	 tau.set_eta( pat_tau.eta());
	 tau.set_phi( pat_tau.phi());
	 tau.set_energy( pat_tau.energy());
	 tau.set_decayModeFinding ( pat_tau.tauID("decayModeFinding")>0.5); 
	 tau.set_decayModeFindingNewDMs ( pat_tau.tauID("decayModeFindingNewDMs")>0.5); 

	 tau.set_againstElectronLoose ( pat_tau.tauID("againstElectronLoose"));
	 tau.set_againstElectronLooseMVA5 ( pat_tau.tauID("againstElectronLooseMVA5"));
	 tau.set_againstElectronMVA5category ( pat_tau.tauID("againstElectronMVA5category"));
	 tau.set_againstElectronMVA5raw ( pat_tau.tauID("againstElectronMVA5raw"));
	 tau.set_againstElectronMedium ( pat_tau.tauID("againstElectronMedium"));
	 tau.set_againstElectronMediumMVA5 ( pat_tau.tauID("againstElectronMediumMVA5"));
	 tau.set_againstElectronTight ( pat_tau.tauID("againstElectronTight"));
	 tau.set_againstElectronTightMVA5 ( pat_tau.tauID("againstElectronTightMVA5"));
	 tau.set_againstElectronVLooseMVA5 ( pat_tau.tauID("againstElectronVLooseMVA5"));
	 tau.set_againstElectronVTightMVA5 ( pat_tau.tauID("againstElectronVTightMVA5"));
	 tau.set_againstMuonLoose ( pat_tau.tauID("againstMuonLoose"));
	 tau.set_againstMuonLoose2 ( pat_tau.tauID("againstMuonLoose2"));
	 tau.set_againstMuonLoose3 ( pat_tau.tauID("againstMuonLoose3"));
	 tau.set_againstMuonLooseMVA ( pat_tau.tauID("againstMuonLooseMVA"));
	 tau.set_againstMuonMVAraw ( pat_tau.tauID("againstMuonMVAraw"));
	 tau.set_againstMuonMedium ( pat_tau.tauID("againstMuonMedium"));
	 tau.set_againstMuonMedium2 ( pat_tau.tauID("againstMuonMedium2"));
	 tau.set_againstMuonMediumMVA ( pat_tau.tauID("againstMuonMediumMVA"));
	 tau.set_againstMuonTight ( pat_tau.tauID("againstMuonTight"));
	 tau.set_againstMuonTight2 ( pat_tau.tauID("againstMuonTight2"));
	 tau.set_againstMuonTight3 ( pat_tau.tauID("againstMuonTight3"));
	 tau.set_againstMuonTightMVA ( pat_tau.tauID("againstMuonTightMVA"));
	 tau.set_byCombinedIsolationDeltaBetaCorrRaw3Hits ( pat_tau.tauID("byCombinedIsolationDeltaBetaCorrRaw3Hits"));
	 tau.set_byIsolationMVA3newDMwLTraw ( pat_tau.tauID("byIsolationMVA3newDMwLTraw"));
	 tau.set_byIsolationMVA3newDMwoLTraw ( pat_tau.tauID("byIsolationMVA3newDMwoLTraw"));
	 tau.set_byIsolationMVA3oldDMwLTraw ( pat_tau.tauID("byIsolationMVA3oldDMwLTraw"));
	 tau.set_byIsolationMVA3oldDMwoLTraw ( pat_tau.tauID("byIsolationMVA3oldDMwoLTraw"));
	 tau.set_byLooseCombinedIsolationDeltaBetaCorr3Hits ( pat_tau.tauID("byLooseCombinedIsolationDeltaBetaCorr3Hits"));
	 tau.set_byLooseIsolationMVA3newDMwLT ( pat_tau.tauID("byLooseIsolationMVA3newDMwLT"));
	 tau.set_byLooseIsolationMVA3newDMwoLT ( pat_tau.tauID("byLooseIsolationMVA3newDMwoLT"));
	 tau.set_byLooseIsolationMVA3oldDMwLT ( pat_tau.tauID("byLooseIsolationMVA3oldDMwLT"));
	 tau.set_byLooseIsolationMVA3oldDMwoLT ( pat_tau.tauID("byLooseIsolationMVA3oldDMwoLT"));
	 tau.set_byMediumCombinedIsolationDeltaBetaCorr3Hits ( pat_tau.tauID("byMediumCombinedIsolationDeltaBetaCorr3Hits"));
	 tau.set_byMediumIsolationMVA3newDMwLT ( pat_tau.tauID("byMediumIsolationMVA3newDMwLT"));
	 tau.set_byMediumIsolationMVA3newDMwoLT ( pat_tau.tauID("byMediumIsolationMVA3newDMwoLT"));
	 tau.set_byMediumIsolationMVA3oldDMwLT ( pat_tau.tauID("byMediumIsolationMVA3oldDMwLT"));
	 tau.set_byMediumIsolationMVA3oldDMwoLT ( pat_tau.tauID("byMediumIsolationMVA3oldDMwoLT"));
	 tau.set_byTightCombinedIsolationDeltaBetaCorr3Hits ( pat_tau.tauID("byTightCombinedIsolationDeltaBetaCorr3Hits"));
	 tau.set_byTightIsolationMVA3newDMwLT ( pat_tau.tauID("byTightIsolationMVA3newDMwLT"));
	 tau.set_byTightIsolationMVA3newDMwoLT ( pat_tau.tauID("byTightIsolationMVA3newDMwoLT"));
	 tau.set_byTightIsolationMVA3oldDMwLT ( pat_tau.tauID("byTightIsolationMVA3oldDMwLT"));
	 tau.set_byTightIsolationMVA3oldDMwoLT ( pat_tau.tauID("byTightIsolationMVA3oldDMwoLT"));
	 tau.set_byVLooseIsolationMVA3newDMwLT ( pat_tau.tauID("byVLooseIsolationMVA3newDMwLT"));
	 tau.set_byVLooseIsolationMVA3newDMwoLT ( pat_tau.tauID("byVLooseIsolationMVA3newDMwoLT"));
	 tau.set_byVLooseIsolationMVA3oldDMwLT ( pat_tau.tauID("byVLooseIsolationMVA3oldDMwLT"));
	 tau.set_byVLooseIsolationMVA3oldDMwoLT ( pat_tau.tauID("byVLooseIsolationMVA3oldDMwoLT"));
	 tau.set_byVTightIsolationMVA3newDMwLT ( pat_tau.tauID("byVTightIsolationMVA3newDMwLT"));
	 tau.set_byVTightIsolationMVA3newDMwoLT ( pat_tau.tauID("byVTightIsolationMVA3newDMwoLT"));
	 tau.set_byVTightIsolationMVA3oldDMwLT ( pat_tau.tauID("byVTightIsolationMVA3oldDMwLT"));
	 tau.set_byVTightIsolationMVA3oldDMwoLT ( pat_tau.tauID("byVTightIsolationMVA3oldDMwoLT"));
	 tau.set_byVVTightIsolationMVA3newDMwLT ( pat_tau.tauID("byVVTightIsolationMVA3newDMwLT"));
	 tau.set_byVVTightIsolationMVA3newDMwoLT ( pat_tau.tauID("byVVTightIsolationMVA3newDMwoLT"));
	 tau.set_byVVTightIsolationMVA3oldDMwLT ( pat_tau.tauID("byVVTightIsolationMVA3oldDMwLT"));
	 tau.set_byVVTightIsolationMVA3oldDMwoLT ( pat_tau.tauID("byVVTightIsolationMVA3oldDMwoLT"));

	 tau.set_decayMode( pat_tau.decayMode() );

//       std::cout << pat_tau.tauID("byCombinedIsolationDeltaBetaCorrRaw3Hits") << std::endl;
	 
// 	 reco::PFCandidateRef leadPFCand = pat_tau.leadPFCand();
// 	 if(!leadPFCand.isNull()){
// 	   tau.set_leadPFCand_px ( leadPFCand->px());
// 	   tau.set_leadPFCand_py ( leadPFCand->py());
// 	   tau.set_leadPFCand_pz ( leadPFCand->pz());
// 	 }
// 	 else{
// 	   tau.set_leadPFCand_px ( 0);
// 	   tau.set_leadPFCand_py ( 0);
// 	   tau.set_leadPFCand_pz ( 0);
// 	 }

	 // store isolation info only for identified taus
	 //if (pat_tau.tauID("decayModeFinding")>0.5){
	 //  bool storepfs = (pat_tau.tauID("byLooseCombinedIsolationDeltaBetaCorr3Hits")>0.5) || (pat_tau.tauID("byLooseIsolationMVA3newDMwoLT")>0.5); 	   
	   
	 if (storePFsAroundLeptons){
	   if(runOnMiniAOD){
	     for(unsigned int k=0; k<pf_around_leptons_sources.size(); ++k){
	       edm::Handle< std::vector<pat::PackedCandidate> > pfcand_handle;
	       iEvent.getByLabel(pf_around_leptons_sources[k], pfcand_handle);  
	       const std::vector<pat::PackedCandidate>& pf_cands = *(pfcand_handle.product());
	       StorePFCandsInCone_packed(&tau, pf_cands, 0.4);
	     }
	   }
	   else{
	     for(unsigned int k=0; k<pf_around_leptons_sources.size(); ++k){
	       edm::Handle< std::vector<reco::PFCandidate> > pfcand_handle;
	       iEvent.getByLabel(pf_around_leptons_sources[k], pfcand_handle);
	       const std::vector<reco::PFCandidate>& pf_cands = *(pfcand_handle.product());
	       bool frompu = true;
	       if( pf_around_leptons_sources[k].find("NoPileUp") < pf_around_leptons_sources[k].size()) frompu=false;
	       StorePFCandsInCone(&tau, pf_cands, 0.4, frompu);
	     }
	   }
	 }
       }
     }
   }
   
   print_times(timer, "taus");

   //-------------- gen jets -------------

   if(doGenJets){
     for(size_t j=0; j< genjet_tokens.size(); ++j){
       genjets[j].clear();
       edm::Handle< std::vector<reco::GenJet> > genjet_handle;
       iEvent.getByToken(genjet_tokens[j], genjet_handle);
       const std::vector<reco::GenJet>& gen_jets = *genjet_handle;
       for (unsigned int i = 0; i < gen_jets.size(); ++i) {
         const reco::GenJet & gen_jet = gen_jets[i];
         if(gen_jet.pt() < genjet_ptmin) continue;
         if(fabs(gen_jet.eta()) > genjet_etamax) continue;

          Particle jet;
          jet.set_charge(gen_jet.charge());
          jet.set_pt(gen_jet.pt());
          jet.set_eta(gen_jet.eta());
          jet.set_phi(gen_jet.phi());
          jet.set_energy(gen_jet.energy());

          // recalculate the jet charge.
          // Currently not done due to an issue with miniAOD:
          // https://hypernews.cern.ch/HyperNews/CMS/get/physTools/3274.html
          /*cout << "Jet charge genjets " << j << ":" << i << endl;
          int jet_charge = 0;
          for(const auto & constituent : gen_jet){
              jet_charge += constituent.charge();
          }
          jet.set_charge(jet_charge);
          genjets[j].push_back(jet);*/
       }
     }
   }
   
   print_times(timer, "genjets");
   
   //--------------gen jets with parts-----------------------
   if(doGenJetsWithParts){
     for(size_t j=0; j < genjetwithparts_tokens.size();j++){
       genjetwithparts[j].clear();
       edm::Handle< std::vector<reco::GenJet> > genjet_handle;
       iEvent.getByToken(genjetwithparts_tokens[j], genjet_handle);
       const std::vector<reco::GenJet>& gen_jets = *genjet_handle;
       for(unsigned int i=0; i < gen_jets.size(); i++){
	 const reco::GenJet* gen_jet = &gen_jets[i];
	 if(gen_jet->pt() < genjetwithparts_ptmin) continue;
	 if(fabs(gen_jet->eta()) > genjetwithparts_etamax) continue;

	 GenJetWithParts genjet;
	 genjet.set_charge(gen_jet->charge());
	 genjet.set_pt(gen_jet->pt());
	 genjet.set_eta(gen_jet->eta());
	 genjet.set_phi(gen_jet->phi());
	 genjet.set_energy(gen_jet->energy());

	 // recalculate the jet charge
	 int jet_charge = 0;
	 std::vector<const reco::GenParticle * > jetgenps = gen_jet->getGenConstituents();
	 for(unsigned int l = 0; l<jetgenps.size(); ++l){
	   jet_charge +=  jetgenps[l]->charge();
	 }

    	 genjet.set_charge(jet_charge);

	 fill_genparticles_jet(gen_jets[i], genjet);
	 genjetwithparts[j].push_back(genjet);
       }
     }
   }
   
   print_times(timer, "genjetswithparts");
   
   // ------------- jets -------------
   if(doJets){
     for(size_t j=0; j< jet_tokens.size(); ++j){
       jets[j].clear();
       edm::Handle< std::vector<pat::Jet> > jet_handle;
       iEvent.getByToken(jet_tokens[j], jet_handle);
       const std::vector<pat::Jet> & pat_jets = *jet_handle;
       for (unsigned int i = 0; i < pat_jets.size(); ++i) {
         const pat::Jet & pat_jet = pat_jets[i];
         if(pat_jet.pt() < jet_ptmin) continue;
         if(fabs(pat_jet.eta()) > jet_etamax) continue;
         jets[j].emplace_back();
         fill_jet_info(pat_jet, jets[j].back());
         /*const reco::GenJet *genj = pat_jet.genJet();
         if(genj){
           for(unsigned int k=0; k<genjets->size(); ++k){
             if(genj->pt()==genjets->at(k).pt() && genj->eta()==genjets->at(k).eta()){
                jets[j].back().set_genjet_index(k);
             }
           }
         }*/
         if(doJetsConstituents){
            StoreJetConstituents(pat_jet, jets[j].back());
         }
       }
     }
   }
   
   print_times(timer, "after all jets");

   // ------------- top jets -------------
   if(doTopJets){
     const GenericMVAJetTagComputer *computer = 0;
     if(doTagInfos){
        edm::ESHandle<JetTagComputer> computerHandle;
        iSetup.get<JetTagComputerRecord>().get( SVComputer_.label(), computerHandle );
        computer = dynamic_cast<const GenericMVAJetTagComputer*>( computerHandle.product() );
     }

     for(size_t j=0; j< topjet_tokens.size(); ++j){
       print_times(timer, "topjets begin");
       topjets[j].clear();

       edm::Handle<pat::JetCollection> h_pat_topjets;
       iEvent.getByToken(topjet_tokens[j], h_pat_topjets);
       const vector<pat::Jet> & pat_topjets = *h_pat_topjets;

       for (unsigned int i = 0; i < pat_topjets.size(); i++) {
	 const pat::Jet & pat_topjet =  pat_topjets[i];
	 if(pat_topjet.pt() < topjet_ptmin) continue;
	 if(fabs(pat_topjet.eta()) > topjet_etamax) continue;

	 TopJet topjet;
         fill_jet_info(pat_topjet, topjet);
         
	 /*
	 const reco::GenJet *genj = pat_topjet.genJet();
	 if(genj){
	   topjet.set_genjet_pt ( genj->pt());
	   topjet.set_genjet_eta ( genj->eta());
	   topjet.set_genjet_phi ( genj->phi());
	   topjet.set_genjet_energy ( genj->energy());
	   if(doAllGenParticles){
	     std::vector<const reco::GenParticle * > jetgenps = genj->getGenConstituents();
	     for(unsigned int l = 0; l<jetgenps.size(); ++l){
	       for(unsigned int k=0; k< genps.size(); ++k){
		 if(jetgenps[l]->pt() == genps[k].pt() && jetgenps[l]->pdgId() == genps[k].pdgId()){
		   topjet.add_genparticles_index(genps[k].index());
		   break;
		 }
	       }
	     }
	     if(topjet.genparticles_indices().size()!= jetgenps.size())
	       std::cout << "WARNING: Found only " << topjet.genparticles_indices().size() << " from " << jetgenps.size() << " gen particles of this topjet"<<std::endl;
	   }
	 }
	 */

	 // add constituents to the jet, if requested
	 if (doTopJetsConstituents){
	   if (topjet_constituents_sources.size()>j){ //only add constituents if they are defined
	     edm::Handle<pat::JetCollection> pat_topjets_with_cands;
	     iEvent.getByLabel(topjet_constituents_sources[j], pat_topjets_with_cands);
	     const pat::Jet* pat_topjet_wc = NULL;

	     for (unsigned int it = 0; it < pat_topjets_with_cands->size(); it++) {
	       const pat::Jet* cand =  dynamic_cast<pat::Jet const *>(&pat_topjets_with_cands->at(it));
               assert(cand);
	       double dphi = deltaPhi(cand->phi(), pat_topjet.phi());   
	       if (fabs(dphi)<0.5 && fabs(cand->eta()-pat_topjet.eta())<0.5){ // be generous: filtering, pruning... can change jet axis
		 pat_topjet_wc = cand;
		 break;
	       }
	     }

	     if (pat_topjet_wc){
	       //if (pat_topjet.pt()>150) 
	       StoreJetConstituents(*pat_topjet_wc, topjet);
	       // now run substructure information
	       JetProps substructure(&topjet);
	       substructure.set_pf_cands(&pfparticles);
	       double tau1 = substructure.GetNsubjettiness(1, Njettiness::onepass_kt_axes, 1., 2.0);
	       double tau2 = substructure.GetNsubjettiness(2, Njettiness::onepass_kt_axes, 1., 2.0);
	       double tau3 = substructure.GetNsubjettiness(3, Njettiness::onepass_kt_axes, 1., 2.0);
	       double qjets = substructure.GetQjetVolatility(iEvent.id().event(), 2.0);
	       topjet.set_tau1(tau1);
	       topjet.set_tau2(tau2);
	       topjet.set_tau3(tau3);
	       topjet.set_qjets_volatility(qjets);
	     }
	   }
	 }


	 for (unsigned int k = 0; k < pat_topjet.numberOfDaughters(); k++) {
	   Particle subjet_v4;

	   reco::Candidate const * subjetd =  pat_topjet.daughter(k);
           pat::Jet const * patsubjetd = dynamic_cast<pat::Jet const *>(subjetd);
	   if(patsubjetd) {
              auto p4 = patsubjetd->correctedP4(0);
	      subjet_v4.set_pt(p4.pt());
              subjet_v4.set_eta(p4.eta());
              subjet_v4.set_phi(p4.phi()); 
              subjet_v4.set_energy(p4.E());
              topjet.add_subjet(subjet_v4);
	      //btag info
              topjet.add_subFlavour(patsubjetd->partonFlavour());
              topjet.add_subCSV(patsubjetd->bDiscriminator("combinedSecondaryVertexBJetTags"));
	      //info for subjet JEC correction
	      topjet.add_subArea(patsubjetd->jetArea());
	      topjet.add_subJEC_raw(patsubjetd->jecFactor("Uncorrected"));
	      if (doTagInfos)
		{
		  //ip tag info
		  reco::TaggingVariableList tvlIP=patsubjetd->tagInfoTrackIP("impactParameter")->taggingVariables();
		  topjet.add_subTrackMomentum(tvlIP.getList(reco::btau::trackMomentum,false));
		  topjet.add_subTrackEta(tvlIP.getList(reco::btau::trackEta,false));
		  topjet.add_subTrackEtaRel(tvlIP.getList(reco::btau::trackEtaRel,false));
		  topjet.add_subTrackDeltaR(tvlIP.getList(reco::btau::trackDeltaR,false));
		  topjet.add_subTrackSip3dVal(tvlIP.getList(reco::btau::trackSip3dVal,false));
		  topjet.add_subTrackSip3dSig(tvlIP.getList(reco::btau::trackSip3dSig,false));
		  topjet.add_subTrackSip2dVal(tvlIP.getList(reco::btau::trackSip2dVal,false));
		  topjet.add_subTrackSip2dSig(tvlIP.getList(reco::btau::trackSip2dSig,false));
		  topjet.add_subTrackDecayLenVal(tvlIP.getList(reco::btau::trackDecayLenVal,false));
		  topjet.add_subTrackChi2(tvlIP.getList(reco::btau::trackChi2,false));
		  topjet.add_subTrackNTotalHits(tvlIP.getList(reco::btau::trackNTotalHits,false));
		  topjet.add_subTrackNPixelHits(tvlIP.getList(reco::btau::trackNPixelHits,false));     
		  topjet.add_subTrackPtRel(tvlIP.getList(reco::btau::trackPtRel,false));
		  topjet.add_subTrackPPar(tvlIP.getList(reco::btau::trackPPar,false));
		  topjet.add_subTrackPtRatio(tvlIP.getList(reco::btau::trackPtRatio,false));
		  topjet.add_subTrackPParRatio(tvlIP.getList(reco::btau::trackPParRatio,false));
		  topjet.add_subTrackJetDistVal(tvlIP.getList(reco::btau::trackJetDistVal,false));
		  topjet.add_subTrackJetDistSig(tvlIP.getList(reco::btau::trackJetDistSig,false));
		  topjet.add_subTrackGhostTrackDistVal(tvlIP.getList(reco::btau::trackGhostTrackDistVal,false));
		  topjet.add_subTrackGhostTrackDistSig(tvlIP.getList(reco::btau::trackGhostTrackDistSig,false));
		  topjet.add_subTrackGhostTrackWeight(tvlIP.getList(reco::btau::trackGhostTrackWeight,false));
		  //sv tag info
		  reco::TaggingVariableList tvlSV=patsubjetd->tagInfoSecondaryVertex("secondaryVertex")->taggingVariables();
		  topjet.add_subFlightDistance2dVal(tvlSV.getList(reco::btau::flightDistance2dVal,false));
		  topjet.add_subFlightDistance2dSig(tvlSV.getList(reco::btau::flightDistance2dSig,false));
		  topjet.add_subFlightDistance3dVal(tvlSV.getList(reco::btau::flightDistance3dVal,false));
		  topjet.add_subFlightDistance3dSig(tvlSV.getList(reco::btau::flightDistance3dSig,false));
		  topjet.add_subVertexJetDeltaR(tvlSV.getList(reco::btau::vertexJetDeltaR,false));
		  topjet.add_subJetNSecondaryVertices(tvlSV.get(reco::btau::jetNSecondaryVertices,-9999));
		  topjet.add_subVertexNTracks(tvlSV.get(reco::btau::vertexNTracks,-9999));
		  std::vector<TLorentzVector> vp4;
		  std::vector<float> vchi2;
		  std::vector<float> vndof;
		  std::vector<float> vchi2ndof;
		  std::vector<float> vtrsize;
                  unsigned int nv = patsubjetd->tagInfoSecondaryVertex("secondaryVertex")->nVertices();
		  for(unsigned int i=0; i<nv; i++){
                      const auto & sv_i = patsubjetd->tagInfoSecondaryVertex("secondaryVertex")->secondaryVertex(i);
		      ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > p4 = sv_i.p4();
		      vp4.push_back(TLorentzVector(p4.px(),p4.py(),p4.pz(),p4.e()));
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
		  //try computer
		  if(computer)
		    {
#if CMSSW70 == 1
		      computer->passEventSetup(iSetup);
#endif
		      std::vector<const reco::BaseTagInfo*>  baseTagInfos;
		      baseTagInfos.push_back(patsubjetd->tagInfoTrackIP("impactParameter") );
		      baseTagInfos.push_back(patsubjetd->tagInfoSecondaryVertex("secondaryVertex") );      
		      JetTagComputer::TagInfoHelper helper(baseTagInfos);
		      reco::TaggingVariableList vars = computer->taggingVariables(helper);
		      topjet.add_subVertexMassJTC(vars.get(reco::btau::vertexMass,-9999));
		      topjet.add_subVertexCategoryJTC(vars.get(reco::btau::vertexCategory,-9999));
		      topjet.add_subVertexEnergyRatioJTC(vars.get(reco::btau::vertexEnergyRatio,-9999));
		      topjet.add_subTrackSip3dSigAboveCharmJTC(vars.get(reco::btau::trackSip3dSigAboveCharm,-9999));
		    }
		}
	   }
	   else
	     {
	       //filling only standard information in case the subjet has not been pat-tified during the pattuples production
	       subjet_v4.set_pt(pat_topjet.daughter(k)->p4().pt());
	       subjet_v4.set_eta(pat_topjet.daughter(k)->p4().eta());
	       subjet_v4.set_phi(pat_topjet.daughter(k)->p4().phi());
	       subjet_v4.set_energy(pat_topjet.daughter(k)->p4().E());
	       topjet.add_subjet(subjet_v4);
	     }
	   
	   
	 }
	 topjets[j].push_back(topjet);
       }
     }
   }
   
   print_times(timer, "topjets");

   
   // ------------- generator top jets -------------
   if(doGenTopJets){
     for(size_t j=0; j< gentopjet_tokens.size(); ++j){
       gentopjets[j].clear();
       edm::Handle<reco::BasicJetCollection> reco_gentopjets;
       iEvent.getByToken(gentopjet_tokens[j], reco_gentopjets);
       for (unsigned int i = 0; i < reco_gentopjets->size(); i++) {
         const reco::BasicJet & reco_gentopjet =  reco_gentopjets->at(i);
         if(reco_gentopjet.pt() < gentopjet_ptmin) continue;
         if(fabs(reco_gentopjet.eta()) > gentopjet_etamax) continue;

         GenTopJet gentopjet;
         gentopjet.set_charge(reco_gentopjet.charge());
         gentopjet.set_pt(reco_gentopjet.pt());
         gentopjet.set_eta(reco_gentopjet.eta());
         gentopjet.set_phi(reco_gentopjet.phi());
         gentopjet.set_energy(reco_gentopjet.energy());

         for (unsigned int k = 0; k < reco_gentopjet.numberOfDaughters(); k++) {
            Particle subjet_v4;
            subjet_v4.set_pt(reco_gentopjet.daughter(k)->p4().pt());
            subjet_v4.set_eta(reco_gentopjet.daughter(k)->p4().eta());
            subjet_v4.set_phi(reco_gentopjet.daughter(k)->p4().phi()); 
            subjet_v4.set_energy(reco_gentopjet.daughter(k)->p4().E()); 
            gentopjet.add_subjet(subjet_v4);
         }
         gentopjets[j].push_back(gentopjet);
       }
     }
   }
   
   print_times(timer, "gentopjets");


   // ------------- photons ------------- 
   if(doPhotons){
     for(size_t j=0; j< photon_tokens.size(); ++j){
       phs[j].clear();
       edm::Handle< std::vector<pat::Photon> > photon_handle;
       iEvent.getByToken(photon_tokens[j], photon_handle);
       const std::vector<pat::Photon>& pat_photons = *photon_handle;
       for (unsigned int i = 0; i < pat_photons.size(); ++i) {
         const pat::Photon & pat_photon = pat_photons[i];
         Photon ph;
         ph.set_charge(0);
         ph.set_pt( pat_photon.pt());
         ph.set_eta( pat_photon.eta());
         ph.set_phi( pat_photon.phi());
         ph.set_energy( pat_photon.energy());
         ph.set_vertex_x(pat_photon.vertex().x());
         ph.set_vertex_y(pat_photon.vertex().y());
         ph.set_vertex_z(pat_photon.vertex().z());
         ph.set_supercluster_eta(pat_photon.superCluster()->eta());
         ph.set_supercluster_phi(pat_photon.superCluster()->phi());
         ph.set_trackIso(pat_photon.trackIso());
         phs[j].push_back(ph);
       }
     }
   }
   
   print_times(timer, "photons");

   // ------------- MET -------------
   if(doMET){
     for(size_t j=0; j< met_tokens.size(); ++j){
       edm::Handle< std::vector<pat::MET> > met_handle;
       iEvent.getByToken(met_tokens[j], met_handle);
       const std::vector<pat::MET>& pat_mets = *met_handle;
       if(pat_mets.size()!=1){
         std::cout<< "WARNING: number of METs = " << pat_mets.size() <<", should be 1" << std::endl;
       }
       else{
         pat::MET pat_met = pat_mets[0];
         met[j].set_pt(pat_met.pt());
         met[j].set_phi(pat_met.phi());
         met[j].set_mEtSig(pat_met.mEtSig());
       }
     }
   }
   
   print_times(timer, "met");
  
   // ------------- trigger -------------


   if(doTrigger){
     edm::Handle<edm::TriggerResults> triggerBits;
     iEvent.getByToken(triggerBits_, triggerBits);
     triggerResults.clear();
     triggerNames_outbranch.clear();
     const edm::TriggerNames &names = iEvent.triggerNames(*triggerBits);
     for (unsigned int i = 0, n = triggerBits->size(); i < n; ++i) {
       std::vector<std::string>::const_iterator it = trigger_prefixes.begin();
       for(; it!=trigger_prefixes.end(); ++it){
            if(names.triggerName(i).substr(0, it->size()) == *it)break;
       }
       if(it==trigger_prefixes.end()) continue;
       triggerResults.push_back(triggerBits->accept(i));
       if(newrun){
           triggerNames_outbranch.push_back(names.triggerName(i));
       }
     }
     if(newrun){
         event->set_triggernames(triggerNames_outbranch);
     }
     newrun=false;
   }
   
   print_times(timer, "trigger");
   
   // * done filling the event; call the AnalysisModule if configured:
   bool keep = true;
   if(module){ // if no AnalysisModule is configured: always keep event
       keep = module->process(*event);
   }
   
   if(tr && keep){
       if(!setup_output_branches_done && context){
          context->setup_output_branches(*event);
          setup_output_branches_done = true;
       }
       tr->Fill();
   }   
   print_times(timer, "end");
   return keep;
}

void NtupleWriter::endJob(){
    if(outfile && tr){
      outfile->cd();
      outfile->Write();
      outfile->Close();
    }
}


void NtupleWriter::beginRun(edm::Run const& iRun, edm::EventSetup const&  iSetup){
  if(doTrigger){
    //bool setup_changed = false;
    //hlt_cfg.init(iRun, iSetup, "HLT", setup_changed);
    newrun=true;
  }

}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void NtupleWriter::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}


void NtupleWriter::StorePFCandsInCone_packed(Particle* inpart, const std::vector<pat::PackedCandidate>& pf_cands, double R0)
{
  for (unsigned int i=0; i<pf_cands.size(); ++i){
    const pat::PackedCandidate & pf = pf_cands[i];
    double dr = deltaR(*inpart, pf);
    if (dr>R0) continue;
    // don't store particle itself:
    double dpt = fabs( inpart->pt() - pf.pt() );
    if (dr<1e-10 && dpt<1e-10){ 
      continue;
    }
    bool frompu=true;
    if(pf.charge()==0 || pf.fromPV() >=2) frompu=false;

    add_pfpart_packed(pf, pfparticles, false, !frompu, frompu);    
  }
}

void NtupleWriter::StoreJetConstituents(const pat::Jet& pat_jet, Jet& jet)
{
  if(runOnMiniAOD){
    for (unsigned int i=0; i<pat_jet.numberOfDaughters(); ++i){
      pat::PackedCandidate* pf = ( pat::PackedCandidate*) pat_jet.daughter(i);
      size_t pfparticles_index = add_pfpart_packed(*pf, pfparticles, true, false, false);
      jet.add_pfconstituents_index(pfparticles_index);    
    }
  }
  else{
    const std::vector<reco::PFCandidatePtr> jconstits = pat_jet.getPFConstituents();
    for (unsigned int i=0; i<jconstits.size(); ++i){
      const reco::PFCandidate* pf = jconstits[i].get();
      size_t pfparticles_index = add_pfpart(*pf, pfparticles, true, false, false);
      jet.add_pfconstituents_index(pfparticles_index);
    }
  }

}

void NtupleWriter::StorePFCandsInCone(Particle* inpart, const std::vector<reco::PFCandidate>& pf_cands, double R0, bool frompu)
{
  for (unsigned int i=0; i<pf_cands.size(); ++i){
    const reco::PFCandidate & pf = pf_cands[i];
    double dr = deltaR(*inpart, pf);
    if (dr>R0) continue;
    // don't store particle itself:
    double dpt = fabs( inpart->pt() - pf.pt() );
    if (dr<1e-10 && dpt<1e-10){
      continue;
    }
    add_pfpart(pf, pfparticles, false, !frompu, frompu);
  }
}



void NtupleWriter::fill_genparticles_jet(const reco::GenJet& reco_genjet, GenJetWithParts& genjet)
{
  // loop over all jet consituents, fill into gen_particle collection
	 
  std::vector<const reco::GenParticle*> jetgenps = reco_genjet.getGenConstituents();
  for(unsigned int l = 0; l<jetgenps.size(); ++l){
    const reco::GenParticle* jetgenp =  jetgenps[l];
    size_t genparticles_index = add_genpart(*jetgenp, *event->genparticles);
    genjet.add_genparticles_index(genparticles_index);
  }

  //  if(topjet.genparticles_indices().size()!= jetgenps.size())
  //  std::cout << "WARNING: Found only " << topjet.genparticles_indices().size() << " from " << jetgenps.size() << " gen particles of this topjet"<<std::endl;
  

}

//define this as a plug-in
DEFINE_FWK_MODULE(NtupleWriter);
