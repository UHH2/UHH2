
//set this flag to 1 when running in CMSSW_7_0_X, switch it to 0 for CMSSW_7_1_X and CMSSW_7_2_X
//#define CMSSW70 1

#include "FWCore/Utilities/interface/CPUTimer.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Common/interface/TriggerNames.h"

#include "UHH2/core/include/root-utils.h"
#include "UHH2/core/plugins/NtupleWriter.h"
#include "UHH2/core/plugins/JetProps.h"
#include "UHH2/core/plugins/NtupleWriterJets.h"
#include "UHH2/core/plugins/NtupleWriterLeptons.h"

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/PatCandidates/interface/PackedGenParticle.h"
#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/PdfInfo.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"

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
  bool doElectrons = iConfig.getParameter<bool>("doElectrons");
  bool doMuons = iConfig.getParameter<bool>("doMuons");
  bool doTaus = iConfig.getParameter<bool>("doTaus");
  bool doJets = iConfig.getParameter<bool>("doJets");
  bool doJetsConstituents = iConfig.getParameter<bool>("doJetsConstituents");

  doGenJets = iConfig.getParameter<bool>("doGenJets");
  doGenTopJets = iConfig.getParameter<bool>("doGenTopJets");
  doGenJetsWithParts = iConfig.getParameter<bool>("doGenJetsWithParts");
  doPhotons = iConfig.getParameter<bool>("doPhotons");
  doMET = iConfig.getParameter<bool>("doMET");
  doGenInfo = iConfig.getParameter<bool>("doGenInfo");
  doAllGenParticles = iConfig.getParameter<bool>("doAllGenParticles");

  // topjet configuration:
  bool doTopJets = iConfig.getParameter<bool>("doTopJets");
  bool doTopJetsConstituents = iConfig.getParameter<bool>("doTopJetsConstituents");
  SVComputer_  = iConfig.getUntrackedParameter<edm::InputTag>("svComputer",edm::InputTag("combinedSecondaryVertex"));
  bool doTagInfos = iConfig.getUntrackedParameter<bool>("doTagInfos",false);


  doTrigger = iConfig.getParameter<bool>("doTrigger");
  doPuppi = iConfig.getParameter<bool>("doPuppi");
  runOnMiniAOD = iConfig.getParameter<bool>("runOnMiniAOD");

  bool storePFsAroundLeptons = iConfig.getUntrackedParameter<bool>("storePFsAroundLeptons",false);
  bool doAllPFConstituents = iConfig.getParameter<bool>("doAllPFConstituents");

  if(storePFsAroundLeptons){
      throw runtime_error("storePFsAroundLeptons not supported any more");
  }
  if(doAllPFConstituents){
      throw runtime_error("doAllPFConstituents not supported any more (note: was buggy)");
  }

  // important: initialize first all module_writers, so that they can
  // inform the ges what they write to the uhh2::Event
  if(doTopJetsConstituents || doJetsConstituents){
      branch(tr, "PFParticles", "std::vector<PFParticle>", &pfparticles);
  }
  if(doPuppi){
      branch(tr, "PuppiParticles", "std::vector<PFParticle>", &puppiparticles);
  }
  if(doElectrons){
      using uhh2::NtupleWriterElectrons;
      auto electron_sources = iConfig.getParameter<std::vector<std::string> >("electron_sources");
      for(size_t i=0; i< electron_sources.size(); ++i){
          NtupleWriterElectrons::Config cfg(*context, consumesCollector(), electron_sources[i], electron_sources[i]);
          cfg.runOnMiniAOD = runOnMiniAOD;
          writer_modules.emplace_back(new NtupleWriterElectrons(cfg, i==0));
      }
  }
  if(doMuons){
      using uhh2::NtupleWriterMuons;
      auto muon_sources = iConfig.getParameter<std::vector<std::string> >("muon_sources");
      for(size_t i=0; i< muon_sources.size(); ++i){
          NtupleWriterMuons::Config cfg(*context, consumesCollector(), muon_sources[i], muon_sources[i]);
          cfg.runOnMiniAOD = runOnMiniAOD;
          writer_modules.emplace_back(new NtupleWriterMuons(cfg, i==0));
      }
  }
  if(doTaus){
      using uhh2::NtupleWriterTaus;
      auto tau_sources = iConfig.getParameter<std::vector<std::string> >("tau_sources");
      auto tau_ptmin = iConfig.getParameter<double> ("tau_ptmin");
      auto tau_etamax = iConfig.getParameter<double> ("tau_etamax");
      for(size_t i=0; i< tau_sources.size(); ++i){
          NtupleWriterTaus::Config cfg(*context, consumesCollector(), tau_sources[i], tau_sources[i]);
          cfg.runOnMiniAOD = runOnMiniAOD;
          cfg.ptmin = tau_ptmin;
          cfg.etamax = tau_etamax;
          writer_modules.emplace_back(new NtupleWriterTaus(cfg, i==0));
      }
  }
  if(doJets){
      using uhh2::NtupleWriterJets;
      auto jet_sources = iConfig.getParameter<std::vector<std::string> >("jet_sources");
      double jet_ptmin = iConfig.getParameter<double> ("jet_ptmin");
      double jet_etamax = iConfig.getParameter<double> ("jet_etamax");
      for(size_t i=0; i< jet_sources.size(); ++i){
          NtupleWriterJets::Config cfg(*context, consumesCollector(), jet_sources[i], jet_sources[i]);
          cfg.runOnMiniAOD = runOnMiniAOD;
          cfg.ptmin = jet_ptmin;
          cfg.etamax = jet_etamax;
          if(doJetsConstituents){
              cfg.pfparts = &pfparticles;
          }
          writer_modules.emplace_back(new NtupleWriterJets(cfg, i==0));
      }
  }
  if(doTopJets){
    using uhh2::NtupleWriterTopJets;
    auto topjet_sources = iConfig.getParameter<std::vector<std::string> >("topjet_sources");
    
    double topjet_ptmin = iConfig.getParameter<double> ("topjet_ptmin");
    double topjet_etamax = iConfig.getParameter<double> ("topjet_etamax");
    auto topjet_constituents_sources = iConfig.getParameter<std::vector<std::string> >("topjet_constituents_sources");
    auto njettiness_sources = iConfig.getParameter<std::vector<std::string> >("topjet_njettiness_sources");
    assert(topjet_constituents_sources.size() == njettiness_sources.size());
    for(size_t j=0; j< topjet_sources.size(); ++j){
        NtupleWriterTopJets::Config cfg(*context, consumesCollector(), topjet_sources[j], topjet_sources[j]);
        cfg.runOnMiniAOD = runOnMiniAOD;
        cfg.doTagInfos = doTagInfos;
        cfg.ptmin = topjet_ptmin;
        cfg.etamax = topjet_etamax;
        if(j < topjet_constituents_sources.size()){
            cfg.constituent_src = topjet_constituents_sources[j];
            cfg.njettiness_src = njettiness_sources[j];
            if(doTopJetsConstituents){
                bool is_puppi =  topjet_constituents_sources[j].find("Puppi") != string::npos;
                if(is_puppi){
                    cfg.pfparts = &puppiparticles;
                }
                else{
                    cfg.pfparts = &pfparticles;
                }
            }
        }
        topjet_modules.emplace_back(new NtupleWriterTopJets(cfg, j==0));
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

  if(doPV){
    branch(tr, "beamspot_x0",&event->beamspot_x0);
    branch(tr, "beamspot_y0",&event->beamspot_y0);
    branch(tr, "beamspot_z0",&event->beamspot_z0);
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
  if(doGenTopJets){
    auto gentopjet_sources = iConfig.getParameter<std::vector<std::string> >("gentopjet_sources");
    gentopjet_ptmin = iConfig.getParameter<double> ("gentopjet_ptmin");
    gentopjet_etamax = iConfig.getParameter<double> ("gentopjet_etamax");
    gentopjets.resize(gentopjet_sources.size());
    for(size_t j=0; j< gentopjet_sources.size(); ++j){
      gentopjet_tokens.push_back(consumes<reco::BasicJetCollection>(gentopjet_sources[j]));
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
   puppiparticles.clear();
   
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
       m->process(iEvent, *event);
   }

   if(doPuppi){
       edm::Handle< std::vector<reco::PFCandidate> > puppicand_handle;
       iEvent.getByLabel(edm::InputTag("puppi","Puppi"), puppicand_handle);
       for(const auto & pc : *puppicand_handle){
           uhh2::add_pfpart(pc, puppiparticles, false, false, false);
       }
   }

   // call topjet modules; they are special because
   // they need the JetTagComputer
   // TODO: check whether they need it for each event; maybe
   // once is enough?!
   if(!topjet_modules.empty()){
       const GenericMVAJetTagComputer *computer = 0;
       if(doTagInfos){
           edm::ESHandle<JetTagComputer> computerHandle;
           iSetup.get<JetTagComputerRecord>().get( SVComputer_.label(), computerHandle );
           computer = dynamic_cast<const GenericMVAJetTagComputer*>( computerHandle.product() );
       }
       for(auto & m : topjet_modules){
           m->set_computer(computer); // computer can be NULL, but that's ok
           m->process(iEvent, *event);
       }
   }
   

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
     auto & triggerResults = *event->get_triggerResults();
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
