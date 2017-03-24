#include "FWCore/Utilities/interface/CPUTimer.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Common/interface/TriggerNames.h"

#include "UHH2/core/include/root-utils.h"
#include "UHH2/core/plugins/NtupleWriter.h"
#include "UHH2/core/plugins/NtupleWriterJets.h"
#include "UHH2/core/plugins/NtupleWriterLeptons.h"

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/PatCandidates/interface/PackedGenParticle.h"
#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/HLTReco/interface/TriggerTypeDefs.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHERunInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/PdfInfo.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/Common/interface/EDCollection.h"


#include "RecoBTau/JetTagComputer/interface/GenericMVAJetTagComputer.h"
#include "RecoBTau/JetTagComputer/interface/GenericMVAJetTagComputerWrapper.h"
#include "RecoBTau/JetTagComputer/interface/JetTagComputer.h"
#include "RecoBTau/JetTagComputer/interface/JetTagComputerRecord.h"

#include "TSystem.h"
#include "TFile.h"
#include "TH1.h"

using namespace std;

namespace{

    
size_t add_genpart(const reco::GenParticle & jetgenp, vector<GenParticle> & genparts){
   for(size_t j=0; j<genparts.size();j++){
     const GenParticle & sgenpart = genparts[j];
     auto r = fabs(static_cast<float>(jetgenp.eta()-sgenpart.eta()))+fabs(static_cast<float>(jetgenp.phi()-sgenpart.phi()));
     auto dpt = fabs(static_cast<float>(jetgenp.pt()-sgenpart.pt()));
     if (r == 0.0f && dpt == 0.0f){
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
            ptrs.push_back(EventAccess_::get(event, o.ti, o.handle, false, false));
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

    virtual void do_undeclare_event_output(const std::string & bname) override {
        throw runtime_error("undeclare_event_output not implemented in CMSSW!");
    }

    virtual void do_undeclare_all_event_output() override {
        throw runtime_error("undeclare_all_event_output not implemented in CMSSW!");
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
  else{
      context.reset(new uhh2::CMSSWContext(*ges, outfile, tr));
  }
  
  // TODO: cleanup the configuration by better grouping which
  // parameters are for which objects. Could even pass
  // a edm::ParameterSet for each NtupleWriterModule.
  doPV = iConfig.getParameter<bool>("doPV");
  doRho = iConfig.getUntrackedParameter<bool>("doRho",true);

  const bool save_lepton_keys = iConfig.exists("save_lepton_keys") ? iConfig.getParameter<bool>("save_lepton_keys") : false;

  bool doElectrons = iConfig.getParameter<bool>("doElectrons");
  
  bool doMuons = iConfig.getParameter<bool>("doMuons");
  bool doTaus = iConfig.getParameter<bool>("doTaus");
  bool doJets = iConfig.getParameter<bool>("doJets");

  doGenJets = iConfig.getParameter<bool>("doGenJets");
  doGenTopJets = iConfig.getParameter<bool>("doGenTopJets");
  doGenJetsWithParts = iConfig.getParameter<bool>("doGenJetsWithParts");
  doPhotons = iConfig.getParameter<bool>("doPhotons");
  doMET = iConfig.getParameter<bool>("doMET");
  doGenInfo = iConfig.getParameter<bool>("doGenInfo");
  doAllGenParticles = iConfig.getParameter<bool>("doAllGenParticles");
  doAllPFParticles = iConfig.getParameter<bool>("doAllPFParticles");

  // topjet configuration:
  bool doTopJets = iConfig.getParameter<bool>("doTopJets");

  doTrigger = iConfig.getParameter<bool>("doTrigger");
  //doTrigHTEmu = iConfig.getParameter<bool>("doTrigHTEmu");
 
  doHOTVR = iConfig.getParameter<bool>("doHOTVR");
  doXCone = iConfig.getParameter<bool>("doXCone");
  doGenHOTVR = iConfig.getParameter<bool>("doGenHOTVR");
  doGenXCone = iConfig.getParameter<bool>("doGenXCone");

  doEleAddVars = iConfig.getParameter<bool>("doEleAddVars");//Add variables to trace possible issues with the ECAL slew rate mitigation


  auto pv_sources = iConfig.getParameter<std::vector<std::string> >("pv_sources");

  // important: initialize first all module_writers, so that they can
  // inform the ges what they write to the uhh2::Event
  if(doElectrons){
      using uhh2::NtupleWriterElectrons;
      auto electron_source = iConfig.getParameter<edm::InputTag>("electron_source");
      
      NtupleWriterElectrons::Config cfg(*context, consumesCollector(), electron_source, electron_source.label());
      cfg.id_keys = iConfig.getParameter<std::vector<std::string>>("electron_IDtags");
      assert(pv_sources.size() > 0); // note: pvs are needed for electron id.
      cfg.pv_src = pv_sources[0];
      writer_modules.emplace_back(new NtupleWriterElectrons(cfg, true, save_lepton_keys));
  }
  if(doMuons){
      using uhh2::NtupleWriterMuons;
      auto muon_sources = iConfig.getParameter<std::vector<std::string> >("muon_sources");

      for(size_t i=0; i< muon_sources.size(); ++i){

        NtupleWriterMuons::Config cfg(*context, consumesCollector(), muon_sources[i], muon_sources[i]);
        assert(pv_sources.size() > 0); // note: pvs are required for muon id.

        cfg.pv_src = pv_sources[0];
        writer_modules.emplace_back(new NtupleWriterMuons(cfg, i==0, save_lepton_keys));
      }
  }
  if(doTaus){
      using uhh2::NtupleWriterTaus;
      auto tau_sources = iConfig.getParameter<std::vector<std::string> >("tau_sources");
      auto tau_ptmin = iConfig.getParameter<double> ("tau_ptmin");
      auto tau_etamax = iConfig.getParameter<double> ("tau_etamax");
      for(size_t i=0; i< tau_sources.size(); ++i){
          NtupleWriterTaus::Config cfg(*context, consumesCollector(), tau_sources[i], tau_sources[i]);
          cfg.ptmin = tau_ptmin;
          cfg.etamax = tau_etamax;
          writer_modules.emplace_back(new NtupleWriterTaus(cfg, i==0));
      }
  }
  if(doJets){
      using uhh2::NtupleWriterJets;
      auto jet_sources = iConfig.getParameter<std::vector<std::string> >("jet_sources");
      double jet_ptmin  = iConfig.getParameter<double>("jet_ptmin");
      double jet_etamax = iConfig.getParameter<double>("jet_etamax");

      /*--- lepton keys ---*/
      std::vector<std::string> muon_sources, elec_sources;
      if(save_lepton_keys){
        if(doMuons){

          muon_sources = iConfig.getParameter<std::vector<std::string>>("muon_sources");
	}
        if(doElectrons){

          auto elec_source = iConfig.getParameter<edm::InputTag>("electron_source");
          elec_sources.push_back(elec_source.label());
        }
      }
      /*-------------------*/

      for(size_t i=0; i<jet_sources.size(); ++i){
        NtupleWriterJets::Config cfg(*context, consumesCollector(), jet_sources[i], jet_sources[i]);
        cfg.ptmin = jet_ptmin;
        cfg.etamax = jet_etamax;
        writer_modules.emplace_back(new NtupleWriterJets(cfg, i==0, muon_sources, elec_sources));
      }
  }
  if(doTopJets){
    using uhh2::NtupleWriterTopJets;
    auto topjet_sources = iConfig.getParameter<std::vector<std::string> >("topjet_sources");
    auto subjet_sources = iConfig.getParameter<std::vector<std::string> >("subjet_sources");
    auto subjet_taginfos = iConfig.getParameter<std::vector<std::string> >("subjet_taginfos");
    auto higgstag_sources = iConfig.getParameter<std::vector<std::string> >("higgstag_sources");
    auto higgstag_names = iConfig.getParameter<std::vector<std::string> >("higgstag_names");
    auto topjet_prunedmass_sources   = iConfig.getParameter<std::vector<std::string> >("topjet_prunedmass_sources");
    auto topjet_softdropmass_sources = iConfig.getParameter<std::vector<std::string> >("topjet_softdropmass_sources");
    double topjet_ptmin = iConfig.getParameter<double> ("topjet_ptmin");
    double topjet_etamax = iConfig.getParameter<double> ("topjet_etamax");
    bool substructure_variables = false;
    bool substructure_groomed_variables = false;
    std::vector<std::string> qjets_sources, njettiness_sources, topjet_substructure_variables_sources, njettiness_groomed_sources, topjet_substructure_groomed_variables_sources;
    if(!iConfig.exists("subjet_sources")){
      cerr << "Exception: it is necessary to specify the subjets for each topjet collection" << endl;
      throw;
    }
    if(subjet_sources.size()!=topjet_sources.size()){
      cerr << "Exception: it is necessary to specify the subjets for each topjet collection" << endl;
      throw;
    }
    if(topjet_prunedmass_sources.size()!=topjet_sources.size()){
      cerr << "Exception: wrong size of topjet_prunedmass_sources" << endl;
      throw;
    }
    if(topjet_softdropmass_sources.size()!=topjet_sources.size()){
      cerr << "Exception: wrong size of topjet_softdropmass_sources" << endl;
      throw;
    }
    if(subjet_sources.size()!=subjet_taginfos.size()){
      cerr << "Exception: it is necessary to specify if you want to store taginfos for each subjet collection" << endl;
      throw;
    }
    if(subjet_sources.size()!=higgstag_sources.size()){
      cerr << "Exception: size of higgstag_sources is wrong" << endl;
      throw;
    }
    if(higgstag_names.size()!=higgstag_sources.size()){
      cerr << "Exception: size of higgstag_names is wrong" << endl;
      throw;
    }
    if(iConfig.exists("topjet_qjets_sources")){
        qjets_sources = iConfig.getParameter<std::vector<std::string> >("topjet_qjets_sources");
        substructure_variables = true;
    }
    if(iConfig.exists("topjet_njettiness_sources")){
        njettiness_sources = iConfig.getParameter<std::vector<std::string> >("topjet_njettiness_sources");
        substructure_variables = true;
    }
    if(substructure_variables){
        topjet_substructure_variables_sources = iConfig.getParameter<std::vector<std::string> >("topjet_substructure_variables_sources");
    }
    assert(qjets_sources.size() <= topjet_substructure_variables_sources.size());
    if(iConfig.exists("topjet_njettiness_groomed_sources")){
        njettiness_groomed_sources = iConfig.getParameter<std::vector<std::string> >("topjet_njettiness_groomed_sources");
        substructure_groomed_variables = true;
    }
    if(substructure_groomed_variables){
        topjet_substructure_groomed_variables_sources = iConfig.getParameter<std::vector<std::string> >("topjet_substructure_groomed_variables_sources");
    }

    /*--- lepton keys ---*/
    std::vector<std::string> muon_sources, elec_sources;
    if(save_lepton_keys){
      if(doMuons){

        muon_sources = iConfig.getParameter<std::vector<std::string>>("muon_sources");
      }
      if(doElectrons){

        auto elec_source = iConfig.getParameter<edm::InputTag>("electron_source");
        elec_sources.push_back(elec_source.label());
      }
    }
    /*-------------------*/

    for(size_t j=0; j< topjet_sources.size(); ++j){
        NtupleWriterTopJets::Config cfg(*context, consumesCollector(), topjet_sources[j], topjet_sources[j]);
        cfg.ptmin = topjet_ptmin;
        cfg.etamax = topjet_etamax;
	cfg.higgs_src = higgstag_sources[j];
	cfg.higgs_name = higgstag_names[j];
	cfg.pruned_src = topjet_prunedmass_sources[j];
        cfg.softdrop_src = topjet_softdropmass_sources[j];
	if(higgstag_sources[j]!=""&&higgstag_names[j]==""){
	  cerr << "Exception: higgstag source specified, but no higgstag discriminator name" << endl;
	  throw;
	}
	cfg.subjet_src = subjet_sources[j];
	if(subjet_taginfos[j]=="store"){
 	  cfg.do_taginfo_subjets = true;
 	}
 	else{
 	  cfg.do_taginfo_subjets = false;
 	}
        if(j < topjet_substructure_variables_sources.size()){
            cfg.substructure_variables_src = topjet_substructure_variables_sources[j];
        }
        if(j < njettiness_sources.size()){
            cfg.njettiness_src = njettiness_sources[j];
        }
	if(j < topjet_substructure_groomed_variables_sources.size()){
            cfg.substructure_groomed_variables_src = topjet_substructure_groomed_variables_sources[j];
        }
        if(j < njettiness_groomed_sources.size()){
            cfg.njettiness_groomed_src = njettiness_groomed_sources[j];
        }
        if(j < qjets_sources.size()){
            cfg.qjets_src = qjets_sources[j];
        }
	std::string topbranch=topjet_sources[j]+"_"+subjet_sources[j];
	cfg.dest_branchname = topbranch;
	cfg.dest = topbranch;
        writer_modules.emplace_back(new NtupleWriterTopJets(cfg, j==0, muon_sources, elec_sources));
/*
        // switch on lepton-keys storage for TopJet collections
        writer_modules.emplace_back(new NtupleWriterTopJets(cfg, i==0, muon_sources, elec_sources));
*/
    }
  }

  // initialization of tree variables
  event.reset(new uhh2::Event(*ges));

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

  branch(tr, "dupECALClusters",&event->dupECALClusters);
  branch(tr, "ishitsNotReplaced",&event->ishitsNotReplaced);
  if(doEleAddVars){
    auto dupECALClusters_source = iConfig.getParameter<edm::InputTag>("dupECALClusters_source");
    dupECALClusters_token = consumes<bool>(dupECALClusters_source);
    auto hitsNotReplaced_source = iConfig.getParameter<edm::InputTag>("hitsNotReplaced_source");
    hitsNotReplaced_token = consumes<edm::EDCollection<DetId>>(hitsNotReplaced_source);
  }
  else{
    event->dupECALClusters = false;
    event->ishitsNotReplaced = false;
  }

  //input tokens for objects with fixed names, not defined in the ntuplewriter.py script
  bs_token = consumes<reco::BeamSpot>( edm::InputTag("offlineBeamSpot"));
  generator_token = consumes<GenEventInfoProduct>( edm::InputTag("generator"));
  pus_token = consumes<std::vector<PileupSummaryInfo> > ( edm::InputTag("slimmedAddPileupInfo"));
  lhe_token = consumes<LHEEventProduct> ( edm::InputTag("externalLHEProducer"));

  //if(doPV){
  branch(tr, "beamspot_x0",&event->beamspot_x0);
  branch(tr, "beamspot_y0",&event->beamspot_y0);
  branch(tr, "beamspot_z0",&event->beamspot_z0);
  //}
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
  if(doGenTopJets){
    auto gentopjet_sources = iConfig.getParameter<std::vector<edm::InputTag> >("gentopjet_sources");
    gentopjet_ptmin = iConfig.getParameter<double> ("gentopjet_ptmin");
    gentopjet_etamax = iConfig.getParameter<double> ("gentopjet_etamax");
    gentopjets.resize(gentopjet_sources.size());
    for(size_t j=0; j< gentopjet_sources.size(); ++j){
      gentopjet_tokens.push_back(consumes<edm::View<reco::Jet> >(gentopjet_sources[j]));
      branch(tr, gentopjet_sources[j].encode().c_str(), "std::vector<GenTopJet>", &gentopjets[j]);
    }
    if(!gentopjet_sources.empty()){
        event->gentopjets = &gentopjets[0];
    }
    auto gentopjet_tau1 = iConfig.getParameter<std::vector<edm::InputTag> >("gentopjet_tau1");
    for(size_t j=0; j< gentopjet_tau1.size(); ++j){
      gentopjet_tau1_tokens.push_back(consumes<edm::ValueMap<float> >(gentopjet_tau1[j]));
    }
    auto gentopjet_tau2 = iConfig.getParameter<std::vector<edm::InputTag> >("gentopjet_tau2");
    for(size_t j=0; j< gentopjet_tau2.size(); ++j){
      gentopjet_tau2_tokens.push_back(consumes<edm::ValueMap<float> >(gentopjet_tau2[j]));
    }
    auto gentopjet_tau3 = iConfig.getParameter<std::vector<edm::InputTag> >("gentopjet_tau3");
    for(size_t j=0; j< gentopjet_tau3.size(); ++j){
      gentopjet_tau3_tokens.push_back(consumes<edm::ValueMap<float> >(gentopjet_tau3[j]));
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
      //      if (met_sources[j]=="slimmedMETsPuppi") puppi.push_back(true);
      if (met_sources[j]=="slimmedMETsPuppi" || met_sources[j]=="slMETsCHST1") puppi.push_back(true); //Puppi and CHS don't have METUncertainty
      else puppi.push_back(false);
    }
    if(!met_sources.empty()){
        event->met = &met[0];
    }
  }
  if(doPV){
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
    genparticle_token = consumes<reco::GenParticleCollection>(iConfig.getParameter<edm::InputTag>("genparticle_source"));
    if(doAllGenParticles) stablegenparticle_token = consumes<edm::View<pat::PackedGenParticle> >(iConfig.getParameter<edm::InputTag>("stablegenparticle_source"));
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
    metfilterBits_ = consumes<edm::TriggerResults>(iConfig.getParameter<edm::InputTag>("metfilter_bits"));
    triggerObjects_ = consumes<pat::TriggerObjectStandAloneCollection>(iConfig.getParameter<edm::InputTag>("trigger_objects"));
    triggerObjects_sources = iConfig.getParameter<std::vector<std::string> >("triggerObjects_sources");
    triggerObjects_out.resize(triggerObjects_sources.size());
    for (size_t j=0; j<triggerObjects_sources.size(); j++){
      TString name = "triggerObjects_";
      name += triggerObjects_sources[j].c_str();
      branch(tr, name, "std::vector<FlavorParticle>", &triggerObjects_out[j]);
    }

  }
  if(doAllPFParticles){
    event->pfparticles = new vector<PFParticle>;
    pf_collection_token = consumes<vector<pat::PackedCandidate>>(iConfig.getParameter<edm::InputTag>("pf_collection_source"));
    branch(tr, "PFParticles", "std::vector<PFParticle>", &event->pfparticles);
  }
  // HOTVR and XCone Jet Cluster - added by Alex and Dennis
  // PF Jets
  if(doHOTVR || doXCone)
    // HOTVR and XCone need full pf_collection for clustering
    {
      pf_collection_token = consumes<vector<pat::PackedCandidate>>(iConfig.getParameter<edm::InputTag>("pf_collection_source"));
      if(doHOTVR)
	{      
	  branch(tr, "HOTVRTopJets", "std::vector<TopJet>", &hotvrJets);
	}
      if(doXCone)
	{
	  branch(tr, "XConeTopJets", "std::vector<TopJet>", &xconeJets);
	}
    }
  // GenJets
  if(doGenHOTVR || doGenXCone)
    {
      stablegenparticle_token = consumes<edm::View<pat::PackedGenParticle> >(iConfig.getParameter<edm::InputTag>("stablegenparticle_source"));
     if(doGenHOTVR)
	{      
	  branch(tr, "genHOTVRTopJets", "std::vector<GenTopJet>", &genhotvrJets);
	}
      if(doGenXCone)
	{
	  branch(tr, "genXCone33TopJets", "std::vector<GenTopJet>", &genxcone33Jets);
	  branch(tr, "genXCone33TopJets_softdrop", "std::vector<GenTopJet>", &genxcone33Jets_softdrop);
	  branch(tr, "genXCone23TopJets", "std::vector<GenTopJet>", &genxcone23Jets);
	}
    }
  newrun = true;
}


NtupleWriter::~NtupleWriter() {}

namespace {
    inline void print_times(const edm::CPUTimer & timer, const char * point){
        // uncomment to study timing.
      //      cout << point << ": cpu = "<< (timer.cpuTime() * 1000.) << "ms; real = " << (timer.realTime() * 1000.) << "ms" << endl;
        
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

   if(doEleAddVars){
    edm::Handle<bool> m_dupECALClusters;
    iEvent.getByToken(dupECALClusters_token, m_dupECALClusters);
    event->dupECALClusters=*m_dupECALClusters;

    edm::Handle<edm::EDCollection<DetId>> m_hitsNotReplaced;
    iEvent.getByToken(hitsNotReplaced_token, m_hitsNotReplaced);
    const edm::EDCollection<DetId> & hitsNotReplaced = *m_hitsNotReplaced;
    //    std::cout<<"hitsNotReplaced.size() = "<<hitsNotReplaced.size()<<std::endl;
    if(hitsNotReplaced.size()>0)
      event->ishitsNotReplaced=true;
    else
      event->ishitsNotReplaced=false;
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
     iEvent.getByToken(bs_token, beamSpot);
     const reco::BeamSpot & bsp = *beamSpot;
     
     event->beamspot_x0 = bsp.x0();
     event->beamspot_y0 = bsp.y0();
     event->beamspot_z0 = bsp.z0();
   }
   else{
     event->beamspot_x0 = 0;
     event->beamspot_y0 = 0;
     event->beamspot_z0 = 0;
   }
   
   print_times(timer, "pv");

   // ------------- generator info -------------
   
   if(doGenInfo && event->genInfo){
     event->genInfo->clear_weights();
     event->genInfo->clear_systweights();  
     event->genInfo->clear_binningValues();
     event->genparticles->clear();

     edm::Handle<GenEventInfoProduct> genEventInfoProduct;
     iEvent.getByToken(generator_token, genEventInfoProduct);
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

     edm::Handle<LHEEventProduct> lhe;
     if(iEvent.getByToken(lhe_token,lhe)){
       event->genInfo->set_originalXWGTUP(lhe->originalXWGTUP());
       for(unsigned int k=0; k<lhe->weights().size(); k++){
	 event->genInfo->add_systweight(lhe->weights().at(k).wgt);
       }
     }

     edm::Handle<std::vector<PileupSummaryInfo> > pus;
     iEvent.getByToken(pus_token, pus);
     event->genInfo->set_pileup_NumInteractions_intime(0);
     event->genInfo->set_pileup_NumInteractions_ootbefore(0);
     event->genInfo->set_pileup_NumInteractions_ootafter(0);
     event->genInfo->set_PU_pT_hat_max(0);
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
	 const std::vector<float> PU_pT_hats = pus->at(i).getPU_pT_hats();
	 for(size_t j=0; j<PU_pT_hats.size(); ++j){
	   if(event->genInfo->PU_pT_hat_max()<PU_pT_hats[j]) 
	     event->genInfo->set_PU_pT_hat_max(PU_pT_hats[j]);
	 }
       }
     }
     //     std::cout<<"event->genInfo->PU_pT_hat_max() = "<<event->genInfo->PU_pT_hat_max()<<std::endl;

     edm::Handle<reco::GenParticleCollection> genPartColl;
     // use genPartColl for the Matrix-Element particles. Also use it for stable leptons
     // in case doAllGenParticles is false.
     iEvent.getByToken(genparticle_token, genPartColl);
     int index=-1;
     for(reco::GenParticleCollection::const_iterator iter = genPartColl->begin(); iter != genPartColl->end(); ++ iter){
       index++;

     // No checks here in order to keep mother-daughter relations intact.
     // For more filtering check the GEN PARTICLES section in the config

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
     
     //store stable gen particles from packed collection
     if(doAllGenParticles){
       edm::Handle<edm::View<pat::PackedGenParticle> > packed;
       // use packed particle collection for all STABLE (status 1) particles
       iEvent.getByToken(stablegenparticle_token,packed);

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
          int jet_charge = 0;
          size_t nd = gen_jet.numberOfSourceCandidatePtrs();
          for(size_t i=0; i<nd; ++i){
            auto constituent = gen_jet.sourceCandidatePtr(i);
            if(constituent.isNull()){
               continue;
            }
            // use constituent here
            jet_charge += constituent->charge();
          }
          jet.set_charge(jet_charge);
          genjets[j].push_back(jet);
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
   
   for(auto & m : writer_modules){
       m->process(iEvent, *event, iSetup);
   }

   // ------------- generator top jets -------------
   if(doGenTopJets){

     for(size_t j=0; j< gentopjet_tokens.size(); ++j){
       gentopjets[j].clear();
       edm::Handle<edm::View<reco::Jet> > reco_gentopjets;
       iEvent.getByToken(gentopjet_tokens[j], reco_gentopjets);
       edm::Handle<edm::ValueMap<float> > reco_gentopjets_tau1;
       if (j<gentopjet_tau1_tokens.size())
         iEvent.getByToken(gentopjet_tau1_tokens[j], reco_gentopjets_tau1);
       edm::Handle<edm::ValueMap<float> > reco_gentopjets_tau2;
       if (j<gentopjet_tau2_tokens.size())
         iEvent.getByToken(gentopjet_tau2_tokens[j], reco_gentopjets_tau2);
       edm::Handle<edm::ValueMap<float> > reco_gentopjets_tau3;
       if (j<gentopjet_tau3_tokens.size())
         iEvent.getByToken(gentopjet_tau3_tokens[j], reco_gentopjets_tau3);
       for (unsigned int i = 0; i < reco_gentopjets->size(); i++) {
         const reco::Jet & reco_gentopjet =  reco_gentopjets->at(i);
         if(reco_gentopjet.pt() < gentopjet_ptmin) continue;
         if(fabs(reco_gentopjet.eta()) > gentopjet_etamax) continue;

         GenTopJet gentopjet;
         gentopjet.set_charge(reco_gentopjet.charge());
         gentopjet.set_pt(reco_gentopjet.pt());
         gentopjet.set_eta(reco_gentopjet.eta());
         gentopjet.set_phi(reco_gentopjet.phi());
         gentopjet.set_energy(reco_gentopjet.energy());
	 const auto ptr = reco_gentopjets->ptrAt(i);
	 if(reco_gentopjets_tau1.isValid())
	   gentopjet.set_tau1((*reco_gentopjets_tau1)[ptr]);
	 if(reco_gentopjets_tau2.isValid())
	   gentopjet.set_tau2((*reco_gentopjets_tau2)[ptr]);
	 if(reco_gentopjets_tau3.isValid())
	   gentopjet.set_tau3((*reco_gentopjets_tau3)[ptr]);

	 std::vector<const reco::Candidate *> daughters;
	 if(dynamic_cast<const reco::GenJet *>(&reco_gentopjet)) { // This is a GenJet without subjets
            for (unsigned int l = 0; l < reco_gentopjet.numberOfDaughters(); l++) {
	      daughters.push_back(reco_gentopjet.daughter(l));
	    }
         } else { // This is a BasicJet with subjets
          for (unsigned int k = 0; k < reco_gentopjet.numberOfDaughters(); k++) {
            Particle subjet_v4;
            subjet_v4.set_pt(reco_gentopjet.daughter(k)->p4().pt());
            subjet_v4.set_eta(reco_gentopjet.daughter(k)->p4().eta());
            subjet_v4.set_phi(reco_gentopjet.daughter(k)->p4().phi()); 
            subjet_v4.set_energy(reco_gentopjet.daughter(k)->p4().E()); 
            gentopjet.add_subjet(subjet_v4);
            for (unsigned int l = 0; l < reco_gentopjet.daughter(k)->numberOfDaughters(); l++) {
	      daughters.push_back(reco_gentopjet.daughter(k)->daughter(l));
	    }
          }
	 }
         double chf = 0;
         double cef = 0;
         double nhf = 0;
         double nef = 0;
         for (unsigned int k = 0; k < daughters.size(); k++) {
           switch(abs(daughters[k]->pdgId())){
             case 11: //electron
               cef += daughters[k]->energy();
               break;
             case 211: //pi+-
             case 321: //K
             case 2212: //p
             case 3222: //Sigma+
             case 3112: //Sigma-
             case 3312: //Xi-
             case 3334: //Omega-
               chf += daughters[k]->energy();
               break;
             case 310: //KS0
             case 130: //KL0
             case 3122: //Lambda0
             case 3212: //Sigma0
             case 3322: //Xi0
             case 2112: //n0
               nhf += daughters[k]->energy();
               break;
             case 22: //photon
               nef += daughters[k]->energy();
               break;
           }
         }
         chf /= gentopjet.energy();
         cef /= gentopjet.energy();
         nhf /= gentopjet.energy();
         nef /= gentopjet.energy();
         gentopjet.set_chf(chf);
         gentopjet.set_cef(cef);
         gentopjet.set_nhf(nhf);
         gentopjet.set_nef(nef);
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
	 met[j].set_uncorr_pt(pat_met.uncorPt());
	 met[j].set_uncorr_phi(pat_met.uncorPhi());
	 //	 std::cout<<"MET uncorrPt = "<<pat_met.uncorPt()<<" uncorrPhi = "<<pat_met.uncorPhi()<<" corrPt = "<<pat_met.pt()<<" corrPhi = "<<pat_met.phi()<<std::endl;
         if(!puppi.at(j))
            {
               met[j].set_shiftedPx_JetEnUp(pat_met.shiftedPx(pat::MET::METUncertainty::JetEnUp, pat::MET::METCorrectionLevel::Type1));
               met[j].set_shiftedPx_JetEnDown(pat_met.shiftedPx(pat::MET::METUncertainty::JetEnDown, pat::MET::METCorrectionLevel::Type1));
               met[j].set_shiftedPx_JetResUp(pat_met.shiftedPx(pat::MET::METUncertainty::JetResUp, pat::MET::METCorrectionLevel::Type1));
               met[j].set_shiftedPx_JetResDown(pat_met.shiftedPx(pat::MET::METUncertainty::JetResDown, pat::MET::METCorrectionLevel::Type1));
               met[j].set_shiftedPx_UnclusteredEnUp(pat_met.shiftedPx(pat::MET::METUncertainty::UnclusteredEnUp, pat::MET::METCorrectionLevel::Type1));
               met[j].set_shiftedPx_UnclusteredEnDown(pat_met.shiftedPx(pat::MET::METUncertainty::UnclusteredEnDown, pat::MET::METCorrectionLevel::Type1));
               met[j].set_shiftedPx_ElectronEnUp(pat_met.shiftedPx(pat::MET::METUncertainty::ElectronEnUp, pat::MET::METCorrectionLevel::Type1)); 	
               met[j].set_shiftedPx_ElectronEnDown(pat_met.shiftedPx(pat::MET::METUncertainty::ElectronEnDown, pat::MET::METCorrectionLevel::Type1));
               met[j].set_shiftedPx_TauEnUp(pat_met.shiftedPx(pat::MET::METUncertainty::TauEnUp, pat::MET::METCorrectionLevel::Type1)); 	
               met[j].set_shiftedPx_TauEnDown(pat_met.shiftedPx(pat::MET::METUncertainty::TauEnDown, pat::MET::METCorrectionLevel::Type1));
               met[j].set_shiftedPx_MuonEnDown(pat_met.shiftedPx(pat::MET::METUncertainty::MuonEnDown, pat::MET::METCorrectionLevel::Type1));
               met[j].set_shiftedPx_MuonEnUp(pat_met.shiftedPx(pat::MET::METUncertainty::MuonEnUp, pat::MET::METCorrectionLevel::Type1));
               met[j].set_shiftedPy_JetEnUp(pat_met.shiftedPy(pat::MET::METUncertainty::JetEnUp, pat::MET::METCorrectionLevel::Type1));
               met[j].set_shiftedPy_JetEnDown(pat_met.shiftedPy(pat::MET::METUncertainty::JetEnDown, pat::MET::METCorrectionLevel::Type1));
               met[j].set_shiftedPy_JetResUp(pat_met.shiftedPy(pat::MET::METUncertainty::JetResUp, pat::MET::METCorrectionLevel::Type1));
               met[j].set_shiftedPy_JetResDown(pat_met.shiftedPy(pat::MET::METUncertainty::JetResDown, pat::MET::METCorrectionLevel::Type1));
               met[j].set_shiftedPy_UnclusteredEnUp(pat_met.shiftedPy(pat::MET::METUncertainty::UnclusteredEnUp, pat::MET::METCorrectionLevel::Type1));
               met[j].set_shiftedPy_UnclusteredEnDown(pat_met.shiftedPy(pat::MET::METUncertainty::UnclusteredEnDown, pat::MET::METCorrectionLevel::Type1));
               met[j].set_shiftedPy_ElectronEnUp(pat_met.shiftedPy(pat::MET::METUncertainty::ElectronEnUp, pat::MET::METCorrectionLevel::Type1)); 	
               met[j].set_shiftedPy_ElectronEnDown(pat_met.shiftedPy(pat::MET::METUncertainty::ElectronEnDown, pat::MET::METCorrectionLevel::Type1));
               met[j].set_shiftedPy_TauEnUp(pat_met.shiftedPy(pat::MET::METUncertainty::TauEnUp, pat::MET::METCorrectionLevel::Type1)); 	
               met[j].set_shiftedPy_TauEnDown(pat_met.shiftedPy(pat::MET::METUncertainty::TauEnDown, pat::MET::METCorrectionLevel::Type1));
               met[j].set_shiftedPy_MuonEnDown(pat_met.shiftedPy(pat::MET::METUncertainty::MuonEnDown, pat::MET::METCorrectionLevel::Type1));
               met[j].set_shiftedPy_MuonEnUp(pat_met.shiftedPy(pat::MET::METUncertainty::MuonEnUp, pat::MET::METCorrectionLevel::Type1));
            }
       }
      }
   }
   
   print_times(timer, "met");

   // ------------- PF constituents --------------

   if(doAllPFParticles){
     event->pfparticles->clear();
     edm::Handle<vector<pat::PackedCandidate> > pfColl_handle;
     iEvent.getByToken(pf_collection_token, pfColl_handle);

     const std::vector<pat::PackedCandidate>& pf_coll = *(pfColl_handle.product()); 

     for ( unsigned int j = 0; j<pf_coll.size(); ++j){
       const pat::PackedCandidate pf = pf_coll.at(j);
       
       PFParticle part;
       part.set_pt(pf.pt());
       part.set_eta(pf.eta());
       part.set_phi(pf.phi());
       part.set_energy(pf.energy());
       part.set_charge(pf.charge());
       part.set_puppiWeight(pf.puppiWeight());
       part.set_puppiWeightNoLep(pf.puppiWeightNoLep());
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
 
       event->pfparticles->push_back(part);
     }

   }
  
   // ------------- trigger -------------


   if(doTrigger){
     auto & triggerResults = *event->get_triggerResults();
     triggerResults.clear();
     triggerNames_outbranch.clear();

     //read trigger info from triggerBits (k=0) and from metfilterBits (k=1)
     for(int k=0;k<2; k++){
       edm::Handle<edm::TriggerResults> triggerBits;
       if(k==0) 
         iEvent.getByToken(triggerBits_, triggerBits);
       else
         iEvent.getByToken(metfilterBits_, triggerBits);

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

       edm::Handle<pat::TriggerObjectStandAloneCollection> triggerObjects;
       iEvent.getByToken(triggerObjects_, triggerObjects);

       if(k==0){
	 for(size_t j=0; j< triggerObjects_sources.size(); j++){
	   triggerObjects_out[j].clear();
	   for (pat::TriggerObjectStandAlone obj : *triggerObjects) { 
	     obj.unpackPathNames(names);
	     
	     for (unsigned h = 0; h < obj.filterIds().size(); ++h) {
	       if(obj.filterIds()[h]>=0){ // only take trigger objects with ID>0 (HLT trigger objects, see http://cmslxr.fnal.gov/source/DataFormats/HLTReco/interface/TriggerTypeDefs.h)
		 std::string trname = triggerObjects_sources[j].c_str();
		 std::vector<std::string> filters  = obj.filterLabels();
		 for(size_t l=0; l<filters.size(); l++){
		   if ( filters[l]== trname){
		     FlavorParticle p;
		     p.set_pt(obj.pt());
		     p.set_eta(obj.eta());
		     p.set_phi(obj.phi());
		     p.set_energy(obj.energy());
		     p.set_charge(obj.charge());
		     p.set_pdgId(obj.filterIds()[h]);
		     triggerObjects_out[j].push_back(p);
		   }
		 }
	       }
	     }
	   }
	 }
       }
       

       //PFHT800 emulation
       /*
       if(doTrigHTEmu && k==0){
	 if(newrun){
	   triggerNames_outbranch.push_back("HLT_PFHT800Emu_v1");
	 }

	 bool found=false;
	 for (unsigned int i = 0, n = triggerBits->size(); i < n; ++i){ 
	   if (names.triggerName(i).find("HLT_PFHTForMC")!=string::npos && triggerBits->accept(i)) {
	     for (pat::TriggerObjectStandAlone obj : *triggerObjects) {
	       obj.unpackPathNames(names);
	       for (unsigned h = 0; h < obj.filterIds().size(); ++h) {
		 if (obj.filterIds()[h]==trigger::TriggerTHT && obj.hasPathName( "HLT_PFHTForMC*", true, true )) {
		   triggerResults.push_back(obj.pt()>800.0);
		   found=true;   
		 }
	       }
	     }
	   }
	 }
	 if (!found) {triggerResults.push_back(false);}
      
       }//end PFHT800 emulation
       */
     }
     if(newrun){
         event->set_triggernames(triggerNames_outbranch);
     }
     newrun=false;
   }
   
   print_times(timer, "trigger");

   // ------------- HOTVR and XCone Jets  -------------
   if(doHOTVR || doXCone)
     {
       // get PFParticles
       edm::Handle<vector<pat::PackedCandidate>> pfColl_handle;
       iEvent.getByToken(pf_collection_token, pfColl_handle);

       const std::vector<pat::PackedCandidate>& pf_coll = *(pfColl_handle.product()); 
       std::vector<PFParticle> pfparticles;
       for ( unsigned int j = 0; j<pf_coll.size(); ++j){
	 const pat::PackedCandidate pf = pf_coll.at(j);
       
	 PFParticle part;
	 part.set_pt(pf.pt());
	 part.set_eta(pf.eta());
	 part.set_phi(pf.phi());
	 part.set_energy(pf.energy());
	 pfparticles.push_back(part);
       }
       print_times(timer, "HOTVR_loop_packedCands"); 

       UniversalJetCluster jetCluster(&pfparticles,doHOTVR,doXCone);
       print_times(timer, "HOTVR_jetCluster"); 
       if (doHOTVR)
	 {
       hotvrJets = jetCluster.GetHOTVRTopJets();
       print_times(timer, "HOTVR_GetHOTVRTopJets"); 
	 }
       if (doXCone)
	 {
	   xconeJets = jetCluster.GetXCone33Jets();
	 }
       print_times(timer, "HOTVR_end"); 
     }

  if(doGenHOTVR || doGenXCone)
     {
       edm::Handle<edm::View<pat::PackedGenParticle> > packed;
       // use packed particle collection for all STABLE (status 1) particles
       iEvent.getByToken(stablegenparticle_token,packed);
       vector<GenParticle> genparticles;
       for(size_t j=0; j<packed->size();j++){

	 const pat::PackedGenParticle* iter = &(*packed)[j];
	 if(iter->status()!=1) continue;

	 GenParticle genp;
	 genp.set_pt(iter->p4().pt());
	 genp.set_eta(iter->p4().eta());
	 genp.set_phi(iter->p4().phi());
	 genp.set_energy(iter->p4().E());
	 genp.set_status( iter->status());
	 genp.set_pdgId( iter->pdgId());

	 genparticles.push_back(genp);
       }

       UniversalGenJetCluster genjetCluster(&genparticles);
       if (doGenHOTVR)
	 {
	   genhotvrJets = genjetCluster.GetHOTVRTopJets();
	 }
       if (doGenXCone)
	 {
	   genxcone33Jets = genjetCluster.GetXCone33Jets();
	   genxcone33Jets_softdrop = genjetCluster.GetXCone33Jets_softdrop();
	   genxcone23Jets = genjetCluster.GetXCone23Jets();
	 }
       print_times(timer, "genHOTVR"); 
     }
   

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

  //print the LHE header to get the indices of the various systematic weights given on the sample
  /*
  edm::Handle<LHERunInfoProduct> run; 
  typedef std::vector<LHERunInfoProduct::Header>::const_iterator headers_const_iterator;
  
  iRun.getByLabel( "externalLHEProducer", run );
  LHERunInfoProduct myLHERunInfoProduct = *(run.product());
  
  for (headers_const_iterator iter=myLHERunInfoProduct.headers_begin(); iter!=myLHERunInfoProduct.headers_end(); iter++){
    std::cout << iter->tag() << std::endl;
    std::vector<std::string> lines = iter->lines();
    for (unsigned int iLine = 0; iLine<lines.size(); iLine++) {
      std::cout << lines.at(iLine);
    }
  }
  */
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void NtupleWriter::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}


void NtupleWriter::fill_genparticles_jet(const reco::GenJet& reco_genjet, GenJetWithParts& genjet)
{
  // loop over all jet constituents, fill into gen_particle collection
	 
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
