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
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/HLTReco/interface/TriggerTypeDefs.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHERunInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/PdfInfo.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/Common/interface/EDCollection.h"

#include "DataFormats/L1TGlobal/interface/GlobalAlgBlk.h"

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
            throw cms::Exception("FailedMkDir", "Could not create directory '" + path + "' in outfile!");
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
        throw cms::Exception("NotImplementedError" "declare_event_input not implemented in CMSSW!");
    }

    virtual void do_declare_event_output(const std::type_info & ti, const std::string & bname, const std::string & mname) override {
        if(!outfile || !outtree) return;
        outputs.emplace_back(event_output{ti, bname, ges.get_raw_handle(ti, mname)});
    }

    virtual void do_undeclare_event_output(const std::string & bname) override {
        throw cms::Exception("NotImplementedError", "undeclare_event_output not implemented in CMSSW!");
    }

    virtual void do_undeclare_all_event_output() override {
        throw cms::Exception("NotImplementedError", "undeclare_all_event_output not implemented in CMSSW!");
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
      edm::LogInfo("NtupleWriter") << "Trying to build AnalysisModule '" << modulename << "' from library '" << libname << "'";
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
  doGenMET = iConfig.getParameter<bool>("doGenMET");
  doGenInfo = iConfig.getParameter<bool>("doGenInfo");
  doAllGenParticles = iConfig.getParameter<bool>("doAllGenParticles");
  doAllGenParticlesPythia8  = iConfig.getParameter<bool>("doAllGenParticlesPythia8");
  doAllPFParticles = iConfig.getParameter<bool>("doAllPFParticles");

  // topjet configuration:
  bool doTopJets = iConfig.getParameter<bool>("doTopJets");

  doTrigger = iConfig.getParameter<bool>("doTrigger");
  doL1seed = iConfig.getParameter<bool>("doL1seed");
  //doTrigHTEmu = iConfig.getParameter<bool>("doTrigHTEmu");

  doHOTVR = iConfig.getParameter<bool>("doHOTVR");
  doXCone = iConfig.getParameter<bool>("doXCone");
  doGenHOTVR = iConfig.getParameter<bool>("doGenHOTVR");
  doGenXCone = iConfig.getParameter<bool>("doGenXCone");


  auto pv_sources = iConfig.getParameter<std::vector<std::string> >("pv_sources");

  // important: initialize first all module_writers, so that they can
  // inform the ges what they write to the uhh2::Event
  if(doElectrons){
      using uhh2::NtupleWriterElectrons;
      auto electron_source = iConfig.getParameter<edm::InputTag>("electron_source");
      //      auto electron_sources = iConfig.getParameter<std::vector<std::string> >("electron_sources");
      //      foar(size_t i=0; i< electron_sources.size(); ++i){

        NtupleWriterElectrons::Config cfg(*context, consumesCollector(), electron_source, electron_source.label());
        //      NtupleWriterElectrons::Config cfg(*context, consumesCollector(), electron_sources[i], electron_source[i].label());
        cfg.id_keys = iConfig.getParameter<std::vector<std::string>>("electron_IDtags");
        assert(pv_sources.size() > 0); // note: pvs are needed for electron id.
        cfg.pv_src = pv_sources[0];
        writer_modules.emplace_back(new NtupleWriterElectrons(cfg, true, save_lepton_keys));
        //}

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
    if(iConfig.exists("TopJets")){
      double topjet_ptmin = iConfig.getParameter<double> ("topjet_ptmin");
      double topjet_etamax = iConfig.getParameter<double> ("topjet_etamax");

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

      edm::VParameterSet topjets_list = iConfig.getParameterSetVector("TopJets");

      for(unsigned int j=0; j<topjets_list.size(); ++j){
        if(!topjets_list[j].exists("topjet_source")){
          throw cms::Exception("MissingSourceName", "It is necessary to specify the source of the topjet collection");
        }
        std::string topjet_source = topjets_list[j].getParameter<std::string>("topjet_source");
        if(!topjets_list[j].exists("subjet_source")){
          throw cms::Exception("MissingSourceName", "It is necessary to specify the subjets for each topjet collection");
        }
        std::string subjet_source = topjets_list[j].getParameter<std::string>("subjet_source");

        NtupleWriterTopJets::Config cfg(*context, consumesCollector(), topjet_source, topjet_source);
        cfg.ptmin = topjet_ptmin;
        cfg.etamax = topjet_etamax;
        cfg.subjet_src = subjet_source;
        if(topjets_list[j].exists("do_subjet_taginfo")){
          cfg.do_taginfo_subjets = topjets_list[j].getParameter<bool>("do_subjet_taginfo");
        }
        else{
          cfg.do_taginfo_subjets = false;
        }

        if(topjets_list[j].exists("prunedmass_source")){
          cfg.pruned_src = topjets_list[j].getParameter<std::string>("prunedmass_source");
        }
        if(topjets_list[j].exists("softdropmass_source")){
          cfg.softdrop_src = topjets_list[j].getParameter<std::string>("softdropmass_source");
        }
        if(topjets_list[j].exists("higgstag_source")){
          cfg.higgs_src = topjets_list[j].getParameter<std::string>("higgstag_source");
        }
        if(topjets_list[j].exists("higgstag_name")){
          cfg.higgs_name = topjets_list[j].getParameter<std::string>("higgstag_name");
        }
        if(cfg.higgs_src!="" && cfg.higgs_name==""){
          throw cms::Exception("MissingSourceName", "higgstag source specified, but no higgstag discriminator name");
        }
        if(topjets_list[j].exists("higgstaginfo_source")){
          cfg.higgstaginfo_src = topjets_list[j].getParameter<std::string>("higgstaginfo_source");
        }

        if (topjets_list[j].exists("toptagging_source")){
          cfg.toptagging_src = topjets_list[j].getParameter<std::string>("toptagging_source");
        }

        bool substructure_variables = false;
        bool substructure_groomed_variables = false;
        if(topjets_list[j].exists("njettiness_source")){
          cfg.njettiness_src = topjets_list[j].getParameter<std::string>("njettiness_source");
          substructure_variables = true;
        }
        if(topjets_list[j].exists("qjets_source")){
          cfg.qjets_src = topjets_list[j].getParameter<std::string>("qjets_source");
          substructure_variables = true;
        }
        if(substructure_variables){
          if(!topjets_list[j].exists("substructure_variables_source")){
            throw cms::Exception("MissingSourceName", "njettiness or qjets sources defined without definition of substructure_variables_source");
          }
          cfg.substructure_variables_src = topjets_list[j].getParameter<std::string>("substructure_variables_source");
        }

        if(topjets_list[j].exists("njettiness_groomed_source")){
          cfg.njettiness_groomed_src = topjets_list[j].getParameter<std::string>("njettiness_groomed_source");
          substructure_groomed_variables = true;
        }
        if (topjets_list[j].exists("ecf_beta1_source")) {
          cfg.ecf_beta1_src = topjets_list[j].getParameter<std::string>("ecf_beta1_source");
          substructure_groomed_variables = true;
        }
        if (topjets_list[j].exists("ecf_beta2_source")) {
          cfg.ecf_beta2_src = topjets_list[j].getParameter<std::string>("ecf_beta2_source");
          substructure_groomed_variables = true;
        }
        if(substructure_groomed_variables){
          if(!topjets_list[j].exists("substructure_groomed_variables_source")){
            throw cms::Exception("MissingSourceName", "Groomed njettiness or ECF sources defined without definition of substructure_groomed_variables_source for topjets " + topjet_source);
          }
          cfg.substructure_groomed_variables_src = topjets_list[j].getParameter<std::string>("substructure_groomed_variables_source");
        }

        std::string topbranch=topjet_source+"_"+subjet_source;
        cfg.dest_branchname = topbranch;
        cfg.dest = topbranch;
        writer_modules.emplace_back(new NtupleWriterTopJets(cfg, j==0, muon_sources, elec_sources));

      }
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
    /*    auto ph_source = iConfig.getParameter<std::vector<std::string> >("photon_sources");
    std::vector<std::string> photon_sources;
    photon_sources.push_back(ph_source); */

    phs.resize(photon_sources.size());
    for(size_t j=0; j< photon_sources.size(); ++j){
      //      photon_tokens.push_back(consumes<vector<pat::PhotonCollection>>(photon_sources[j]));
      photon_tokens.push_back(consumes<vector<pat::Photon>>(photon_sources[j]));
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
      if (met_sources[j]=="slimmedMETsPuppi") skipMETUncertainties.push_back(true);  // Puppi doesn't have METUncertainty
      else skipMETUncertainties.push_back(false);
    }
    if(!met_sources.empty()){
        event->met = &met[0];
    }
  }
  if(doGenMET){
    auto genmet_sources = iConfig.getParameter<std::vector<std::string> >("genmet_sources");
    genmet.resize(genmet_sources.size());
    for(size_t j=0; j< genmet_sources.size(); ++j){
      genmet_tokens.push_back(consumes<vector<pat::MET>>(genmet_sources[j]));
      branch(tr, (genmet_sources[j]+"_GenMET").c_str(), "MET", &genmet[j]);
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
    if(doAllGenParticles) stablegenparticle_token = consumes<edm::View<reco::Candidate> >(iConfig.getParameter<edm::InputTag>("stablegenparticle_source"));
    event->genInfo = new GenInfo();
    event->genparticles = new vector<GenParticle>();
    branch(tr, "genInfo","GenInfo", event->genInfo);
    branch(tr, "GenParticles","std::vector<GenParticle>", event->genparticles);
  }
  if(doTrigger){
    trigger_prefixes = iConfig.getParameter<std::vector<std::string> >("trigger_prefixes");
    event->get_triggerResults() = new vector<bool>();
    event->get_triggerPrescales() = new vector<int>();
    branch(tr, "triggerNames", "std::vector<std::string>", &triggerNames_outbranch);
    branch(tr, "triggerResults", "std::vector<bool>", event->get_triggerResults());
    branch(tr, "triggerPrescales", "std::vector<int>", event->get_triggerPrescales());
    triggerBits_ = consumes<edm::TriggerResults>(iConfig.getParameter<edm::InputTag>("trigger_bits"));
    edm::InputTag triggerPrescalesTag("patTrigger");
    triggerPrescales_ = consumes<pat::PackedTriggerPrescales>(triggerPrescalesTag);
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
  if(doL1seed){
    l1GtToken_ = consumes<BXVector<GlobalAlgBlk>>(iConfig.getParameter<edm::InputTag>("l1GtSrc"));
    l1EGToken_ = consumes<BXVector<l1t::EGamma>>(iConfig.getParameter<edm::InputTag>("l1EGSrc"));
    l1JetToken_ = consumes<BXVector<l1t::Jet>>(iConfig.getParameter<edm::InputTag>("l1JetSrc"));

    branch(tr,"L1EGamma_seeds","std::vector<L1EGamma>",&L1EG_seeds);
    branch(tr,"L1Jet_seeds","std::vector<L1Jet>",&L1Jet_seeds);

  }

  if(doAllPFParticles){
    event->pfparticles = new vector<PFParticle>;
    pf_collection_token = consumes<vector<pat::PackedCandidate>>(iConfig.getParameter<edm::InputTag>("pf_collection_source"));
    branch(tr, "PFParticles", "std::vector<PFParticle>", &event->pfparticles);
  }

  // HOTVR and XCone Jet Cluster - added by Alex and Dennis

  if (doHOTVR) {
    auto hotvr_sources = iConfig.getParameter<std::vector<edm::InputTag> >("HOTVR_sources");
    hotvrJets.resize(hotvr_sources.size());
    for (size_t j=0; j<hotvr_sources.size(); ++j) {
      hotvr_tokens.push_back(consumes<pat::JetCollection>(hotvr_sources[j]));
      hotvr_subjet_tokens.push_back(consumes<pat::JetCollection>(edm::InputTag(hotvr_sources[j].label(), "SubJets")));
      branch(tr, hotvr_sources[j].encode().c_str(), "std::vector<TopJet>", &hotvrJets[j]);
    }
  }

  if (doXCone) {
    auto xcone_sources = iConfig.getParameter<std::vector<edm::InputTag> >("XCone_sources");
    xconeJets.resize(xcone_sources.size());
    for (size_t j=0; j<xcone_sources.size(); ++j) {
      xcone_tokens.push_back(consumes<pat::JetCollection>(xcone_sources[j]));
      xcone_subjet_tokens.push_back(consumes<pat::JetCollection>(edm::InputTag(xcone_sources[j].label(), "SubJets")));
      branch(tr, xcone_sources[j].encode().c_str(), "std::vector<TopJet>", &xconeJets[j]);
    }
  }

  // GenJets
  if(doGenHOTVR || doGenXCone)
    {
      stablegenparticle_token = consumes<edm::View<reco::Candidate> >(iConfig.getParameter<edm::InputTag>("stablegenparticle_source"));
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
       edm::Handle<edm::View<reco::Candidate> > packed;
       // use packed particle collection for all STABLE (status 1) particles
       iEvent.getByToken(stablegenparticle_token,packed);

       for(size_t j=0; j<packed->size();j++){
         bool skip_particle = false;
         const pat::PackedGenParticle* iter = dynamic_cast<const pat::PackedGenParticle*>(&(packed->at(j)));
         //      if(iter->status()!=1) cout<<"iter->status() = "<<iter->status()<<endl;
         if(doAllGenParticlesPythia8){//for pythia8: store particles with status code, see http://home.thep.lu.se/~torbjorn/pythia81html/ParticleProperties.html
           if(iter->status()<2)
             skip_particle = true;
         }
         else{
           if(iter->status()!=1)
             skip_particle = true;
         }
         // if(!doAllGenParticlesPythia8 && iter->status()!=1) //not pythia8: store all stable particles
         //   skip_particle = true;
         // else
         //   if(doAllGenParticlesPythia8 && iter->status()<2)  //
         //     skip_particle = true;

         if(skip_particle) continue;

         //      cout<<doAllGenParticlesPythia8<<" "<<doAllGenParticles<<" Particle stored!, iter->status() = "<<iter->status()<<endl;
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

         // int nm=iter->numberOfMothers();
         // int nd=iter->numberOfDaughters();

         // if (nm>0) genp.set_mother1( iter->motherRef(0).key());
         // if (nm>1) genp.set_mother2( iter->motherRef(1).key());
         // if (nd>0) genp.set_daughter1( iter->daughterRef(0).key());
         // if (nd>1) genp.set_daughter2( iter->daughterRef(1).key());


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
         edm::LogWarning("NtupleWriter") << "WARNING: number of METs = " << pat_mets.size() <<", should be 1";
       }
       else{
         pat::MET pat_met = pat_mets[0];
         met[j].set_pt(pat_met.pt());
         met[j].set_phi(pat_met.phi());
         met[j].set_mEtSig(pat_met.mEtSig());
         met[j].set_uncorr_pt(pat_met.uncorPt());
         met[j].set_uncorr_phi(pat_met.uncorPhi());
         // std::cout<<"MET uncorrPt = "<<pat_met.uncorPt()<<" uncorrPhi = "<<pat_met.uncorPhi()<<" corrPt = "<<pat_met.pt()<<" corrPhi = "<<pat_met.phi()<<std::endl;
         if(!skipMETUncertainties.at(j))
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
               met[j].set_rawCHS_px(pat_met.corPx(pat::MET::METCorrectionLevel::RawChs));
               met[j].set_rawCHS_py(pat_met.corPy(pat::MET::METCorrectionLevel::RawChs));
            }
       }
      }
   }

   if(doGenMET){
     for(size_t j=0; j< genmet_tokens.size(); ++j){
       edm::Handle< std::vector<pat::MET> > genmet_handle;
       iEvent.getByToken(genmet_tokens[j], genmet_handle);
       const std::vector<pat::MET>& pat_genmets = *genmet_handle;
       if(pat_genmets.size()!=1){
         edm::LogWarning("NtupleWriter") << "WARNING: number of GenMETs = " << pat_genmets.size() <<", should be 1";
       }
       else{
         pat::MET pat_genmet = pat_genmets[0];
         genmet[j].set_pt(pat_genmet.genMET()->pt());
         genmet[j].set_phi(pat_genmet.genMET()->phi());
         genmet[j].set_mEtSig(pat_genmet.genMET()->mEtSig());
         //uncorrected MET is equal to normal MET for GenMET
         genmet[j].set_uncorr_pt(pat_genmet.genMET()->pt());
         genmet[j].set_uncorr_phi(pat_genmet.genMET()->phi());
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
     auto & triggerPrescales = *event->get_triggerPrescales();
     triggerResults.clear();
     triggerPrescales.clear();
     triggerNames_outbranch.clear();

     //read trigger info from triggerBits (k=0) and from metfilterBits (k=1)
     for(int k=0;k<2; k++){
       edm::Handle<edm::TriggerResults> triggerBits;
       edm::Handle<pat::PackedTriggerPrescales> packedTriggerPrescales;
       if(k==0)
         iEvent.getByToken(triggerBits_, triggerBits);
       else
         iEvent.getByToken(metfilterBits_, triggerBits);

       if(iEvent.isRealData())
         iEvent.getByToken(triggerPrescales_, packedTriggerPrescales);

       const edm::TriggerNames &names = iEvent.triggerNames(*triggerBits);

       for (unsigned int i = 0, n = triggerBits->size(); i < n; ++i) {
         std::vector<std::string>::const_iterator it = trigger_prefixes.begin();
         for(; it!=trigger_prefixes.end(); ++it){
           if(names.triggerName(i).substr(0, it->size()) == *it)break;
         }
         if(it==trigger_prefixes.end()) continue;
         triggerResults.push_back(triggerBits->accept(i));

         if(iEvent.isRealData())
           triggerPrescales.push_back(packedTriggerPrescales->getPrescaleForIndex(i));

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
             obj.unpackFilterLabels(iEvent, *triggerBits);
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

  if(doL1seed){
    auto & triggerResults = *event->get_triggerResults();   
    //muGT 
    edm::Handle<BXVector<GlobalAlgBlk>> l1GtHandle;
    iEvent.getByToken(l1GtToken_, l1GtHandle);
    bool prefire = l1GtHandle->begin(-1)->getFinalOR();
    triggerNames_outbranch.push_back("muGT_BX_minus_1__prefire");
    triggerResults.push_back(prefire);

    bool postfire = l1GtHandle->begin(1)->getFinalOR();
    triggerNames_outbranch.push_back("muGT_BX_plus_1");
    triggerResults.push_back(postfire);

    bool fire = l1GtHandle->begin(0)->getFinalOR();
    triggerNames_outbranch.push_back("muGT_BX_plus_0");
    triggerResults.push_back(fire);

    //L1EG

    edm::Handle<BXVector<l1t::EGamma>> l1EGHandle;
    iEvent.getByToken(l1EGToken_, l1EGHandle);
    L1EG_seeds.clear();
    auto readBx = [&] (const BXVector<l1t::EGamma>& egVect, int bx) {
      for (auto itL1=l1EGHandle->begin(bx); itL1!=l1EGHandle->end(bx); ++itL1) {
	L1EGamma l1eg;
	l1eg.set_bx(bx);
	l1eg.set_pt(itL1->p4().Pt());
	l1eg.set_eta(itL1->p4().Eta());
	l1eg.set_phi(itL1->p4().Phi());
	l1eg.set_energy(itL1->p4().energy());
	l1eg.set_towerIeta(itL1->towerIEta());
	l1eg.set_towerIphi(itL1->towerIPhi());
	l1eg.set_nTT(itL1->nTT());
	l1eg.set_Shape(itL1->shape());
	l1eg.set_iso(itL1->hwIso());
	L1EG_seeds.push_back(l1eg);
	//	cout<<"bx = "<<bx<<" itL1->p4() = "<<itL1->p4()<<" itL1->nTT() = "<<itL1->nTT()<<endl;
      }
    };
    readBx(*l1EGHandle, -2);
    readBx(*l1EGHandle, -1);
    readBx(*l1EGHandle, 0);
    readBx(*l1EGHandle, +1);
    readBx(*l1EGHandle, +2);

    //L1Jet
    edm::Handle<BXVector<l1t::Jet>> l1JetHandle;
    iEvent.getByToken(l1JetToken_, l1JetHandle);
    L1Jet_seeds.clear();
    auto readBxjet = [&] (const BXVector<l1t::Jet>& egVect, int bx) {
      for (auto itL1=l1JetHandle->begin(bx); itL1!=l1JetHandle->end(bx); ++itL1) {
	L1Jet l1jet;
	l1jet.set_bx(bx);
	l1jet.set_pt(itL1->p4().Pt());
	l1jet.set_eta(itL1->p4().Eta());
	l1jet.set_phi(itL1->p4().Phi());
	l1jet.set_energy(itL1->p4().energy());
	l1jet.set_towerIeta(itL1->towerIEta());
	l1jet.set_towerIphi(itL1->towerIPhi());
	l1jet.set_puEt(itL1->puEt());
	l1jet.set_seedEt(itL1->seedEt());
	l1jet.set_rawEt(itL1->rawEt());
	L1Jet_seeds.push_back(l1jet);
	//	cout<<"bx = "<<bx<<" itL1->p4() = "<<itL1->p4()<<" itL1->puEt() = "<<itL1->puEt()<<endl;
      }
    };
    readBxjet(*l1JetHandle, -2);
    readBxjet(*l1JetHandle, -1);
    readBxjet(*l1JetHandle, 0);
    readBxjet(*l1JetHandle, +1);
    readBxjet(*l1JetHandle, +2);


    // //L1Jet
    // edm::Handle<BXVector<l1t::Jet>> l1JetHandle;
    // iEvent.getByToken(l1JetToken_, l1JetHandle);

    // prefire = l1JetHandle->begin(-1)->getFinalOR();
    // triggerNames_outbranch.push_back("L1Jet_BX_minus_1__prefire");
    // triggerResults.push_back(prefire);

    // postfire = l1JetHandle->begin(1)->getFinalOR();
    // triggerNames_outbranch.push_back("L1Jet_BX_plus_1");
    // triggerResults.push_back(postfire);

    // fire = l1JetHandle->begin(0)->getFinalOR();
    // triggerNames_outbranch.push_back("L1Jet_BX_plus_0");
    // triggerResults.push_back(fire);
  }


   // ------------- HOTVR and XCone Jets  -------------
   if (doHOTVR) {
    for (size_t j=0; j < hotvr_tokens.size(); ++j){
      hotvrJets[j].clear();
      edm::Handle<pat::JetCollection> hotvr_patjets;
      iEvent.getByToken(hotvr_tokens[j], hotvr_patjets);
      edm::Handle<pat::JetCollection> hotvr_subjet_patjets;
      iEvent.getByToken(hotvr_subjet_tokens[j], hotvr_subjet_patjets);

      // Convert from pat::Jet to TopJet, with special userFloats, and with subjets
      for (const auto & patJet : *hotvr_patjets) {
        TopJet thisJet;
        thisJet.set_pt(patJet.p4().pt());
        thisJet.set_eta(patJet.p4().eta());
        thisJet.set_phi(patJet.p4().phi());
        thisJet.set_energy(patJet.p4().E());
        thisJet.set_jetArea(patJet.jetArea());
        thisJet.set_tau1_groomed(patJet.userFloat("tau1"));
        thisJet.set_tau2_groomed(patJet.userFloat("tau2"));
        thisJet.set_tau3_groomed(patJet.userFloat("tau3"));
        for (const auto & subItr : patJet.subjets()) {
          Jet subjet;
          subjet.set_pt(subItr->p4().pt());
          subjet.set_eta(subItr->p4().eta());
          subjet.set_phi(subItr->p4().phi());
          subjet.set_energy(subItr->p4().E());
          subjet.set_jetArea(subItr->jetArea());
          thisJet.add_subjet(subjet);
        }

        hotvrJets[j].push_back(thisJet);
      }
    }
   }

   if (doXCone) {
    for (size_t j=0; j < xcone_tokens.size(); ++j){
      xconeJets[j].clear();
      edm::Handle<pat::JetCollection> xcone_patjets;
      iEvent.getByToken(xcone_tokens[j], xcone_patjets);
      edm::Handle<pat::JetCollection> xcone_subjet_patjets;
      iEvent.getByToken(xcone_subjet_tokens[j], xcone_subjet_patjets);

      // Convert from pat::Jet to TopJet, with special userFloats, and with subjets
      for (const auto & patJet : *xcone_patjets) {
        TopJet thisJet;
        thisJet.set_pt(patJet.p4().pt());
        thisJet.set_eta(patJet.p4().eta());
        thisJet.set_phi(patJet.p4().phi());
        thisJet.set_energy(patJet.p4().E());
        thisJet.set_jetArea(patJet.jetArea());
        thisJet.set_softdropmass(patJet.userFloat("softdropmass"));

        for (const auto & subItr : patJet.subjets()) {
          Jet subjet;
          subjet.set_pt(subItr->p4().pt());
          subjet.set_eta(subItr->p4().eta());
          subjet.set_phi(subItr->p4().phi());
          subjet.set_energy(subItr->p4().E());
          subjet.set_jetArea(subItr->jetArea());
          thisJet.add_subjet(subjet);
        }

        xconeJets[j].push_back(thisJet);
      }
    }
   }

  if(doGenHOTVR || doGenXCone)
     {
       edm::Handle<edm::View<reco::Candidate> > packed;
       // use packed particle collection for all STABLE (status 1) particles
       iEvent.getByToken(stablegenparticle_token,packed);
       vector<GenParticle> genparticles;
       for(size_t j=0; j<packed->size();j++){

         const pat::PackedGenParticle* iter = dynamic_cast<const pat::PackedGenParticle*>(&(packed->at(j)));
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

void NtupleWriter::endRun(edm::Run const& iRun, edm::EventSetup const&  iSetup){}

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
