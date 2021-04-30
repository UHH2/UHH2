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
#include "SimDataFormats/JetMatching/interface/JetFlavourInfo.h"
#include "SimDataFormats/JetMatching/interface/JetFlavourInfoMatching.h"
#include "DataFormats/Common/interface/EDCollection.h"


#include "RecoBTau/JetTagComputer/interface/GenericMVAJetTagComputer.h"
#include "RecoBTau/JetTagComputer/interface/GenericMVAJetTagComputerWrapper.h"
#include "RecoBTau/JetTagComputer/interface/JetTagComputer.h"
#include "RecoBTau/JetTagComputer/interface/JetTagComputerRecord.h"


#include "TSystem.h"
#include "TFile.h"
#include "TH1.h"
#include "Compression.h"

using namespace std;

namespace{


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
    int compLevel = iConfig.getParameter<int>("compressionLevel");
    outfile = new TFile(fileName.c_str(), "RECREATE", "", compLevel);
    std::string compAlgo = iConfig.getParameter<std::string>("compressionAlgorithm");
    if (compAlgo == std::string("ZLIB")) {
      outfile->SetCompressionAlgorithm(ROOT::kZLIB);
    } else if (compAlgo == std::string("LZMA")) {
      outfile->SetCompressionAlgorithm(ROOT::kLZMA);
    } else if (compAlgo == std::string("Global")) {
      outfile->SetCompressionAlgorithm(0); // cos the enum ROOT::kUseGlobal doens't work for some reason
    } else if (compAlgo == std::string("LZ4")) {
      outfile->SetCompressionAlgorithm(ROOT::kLZ4);
    } else {
      throw cms::Exception("NtupleWriter")
          << "NtupleWriter configured with unknown compression algorithm '" << compAlgo << "'\n"
          << "Allowed compression algorithms are ZLIB, LZMA, Global, LZ4\n";
    }
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

  // Keep this early on, since we may need to modify things using it
  year = iConfig.getParameter<std::string>("year");

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
  doGenJetConstituentsNjets = iConfig.getParameter<unsigned>("doGenJetConstituentsNjets");
  doGenJetConstituentsMinJetPt = iConfig.getParameter<double>("doGenJetConstituentsMinJetPt");
  if(doGenJetConstituentsMinJetPt<1e-6) doGenJetConstituentsMinJetPt=2e6;

  doGenTopJets = iConfig.getParameter<bool>("doGenTopJets");
  doGenTopJetConstituentsNjets = iConfig.getParameter<unsigned>("doGenTopJetConstituentsNjets");
  doGenTopJetConstituentsMinJetPt = iConfig.getParameter<double>("doGenTopJetConstituentsMinJetPt");
  if(doGenTopJetConstituentsMinJetPt<1e-6) doGenTopJetConstituentsMinJetPt=2e6;

  doPFJetConstituentsNjets = iConfig.getParameter<unsigned>("doPFJetConstituentsNjets");
  doPFJetConstituentsMinJetPt = iConfig.getParameter<double>("doPFJetConstituentsMinJetPt");
  doPFJetConstituents = false;
  if(doPFJetConstituentsNjets>0 || doPFJetConstituentsMinJetPt>0)
    doPFJetConstituents=true;
  doPFTopJetConstituentsNjets = iConfig.getParameter<unsigned>("doPFTopJetConstituentsNjets");
  doPFTopJetConstituentsMinJetPt = iConfig.getParameter<double>("doPFTopJetConstituentsMinJetPt");
  doPFTopJetConstituents = false;
  if(doPFTopJetConstituentsNjets>0 || doPFTopJetConstituentsMinJetPt>0)
    doPFTopJetConstituents=true;

  doPhotons = iConfig.getParameter<bool>("doPhotons");
  doMET = iConfig.getParameter<bool>("doMET");
  doGenMET = iConfig.getParameter<bool>("doGenMET");
  doGenInfo = iConfig.getParameter<bool>("doGenInfo");
  doStableGenParticles = iConfig.getParameter<bool>("doStableGenParticles");

  doAllPFParticles = iConfig.getParameter<bool>("doAllPFParticles");

  // topjet configuration:
  bool doTopJets = iConfig.getParameter<bool>("doTopJets");

  doTrigger = iConfig.getParameter<bool>("doTrigger");
  doL1seed = iConfig.getParameter<bool>("doL1seed");
  doEcalBadCalib = iConfig.getParameter<bool>("doEcalBadCalib");
  doPrefire = iConfig.getParameter<bool>("doPrefire");


  doXCone = iConfig.getParameter<bool>("doXCone");
  doPFxconeJetConstituentsNjets = iConfig.getParameter<unsigned>("doPFxconeJetConstituentsNjets");
  doPFxconeJetConstituentsMinJetPt = iConfig.getParameter<double>("doPFxconeJetConstituentsMinJetPt");
  doPFxconeJetConstituents = false;
  if((doPFxconeJetConstituentsNjets>0 || doPFxconeJetConstituentsMinJetPt>0) && doXCone)
    doPFxconeJetConstituents=true;
  if(doPFxconeJetConstituentsMinJetPt<1e-6) doPFxconeJetConstituentsMinJetPt=2e6;

  doHOTVR = iConfig.getParameter<bool>("doHOTVR");
  doPFhotvrJetConstituentsNjets = iConfig.getParameter<unsigned>("doPFhotvrJetConstituentsNjets");
  doPFhotvrJetConstituentsMinJetPt = iConfig.getParameter<double>("doPFhotvrJetConstituentsMinJetPt");
  doPFhotvrJetConstituents = false;
  if((doPFhotvrJetConstituentsNjets>0 || doPFhotvrJetConstituentsMinJetPt>0) && doHOTVR)
    doPFhotvrJetConstituents=true;
  if(doPFhotvrJetConstituentsMinJetPt<1e-6) doPFhotvrJetConstituentsMinJetPt=2e6;


  doGenXCone = iConfig.getParameter<bool>("doGenXCone");
  doGenxconeJetConstituentsNjets = iConfig.getParameter<unsigned>("doGenxconeJetConstituentsNjets");
  doGenxconeJetConstituentsMinJetPt = iConfig.getParameter<double>("doGenxconeJetConstituentsMinJetPt");
  if(doGenxconeJetConstituentsMinJetPt<1e-6) doGenxconeJetConstituentsMinJetPt=2e6;

  doGenHOTVR = iConfig.getParameter<bool>("doGenHOTVR");
  doGenhotvrJetConstituentsNjets = iConfig.getParameter<unsigned>("doGenhotvrJetConstituentsNjets");
  doGenhotvrJetConstituentsMinJetPt = iConfig.getParameter<double>("doGenhotvrJetConstituentsMinJetPt");
  if(doGenhotvrJetConstituentsMinJetPt<1e-6) doGenhotvrJetConstituentsMinJetPt=2e6;

  doXCone_dijet = iConfig.getParameter<bool>("doXCone_dijet");
  doPFxconeDijetJetConstituentsNjets = iConfig.getParameter<unsigned>("doPFxconeDijetJetConstituentsNjets");
  doPFxconeDijetJetConstituentsMinJetPt = iConfig.getParameter<double>("doPFxconeDijetJetConstituentsMinJetPt");
  doPFxconeDijetJetConstituents = false;
  if((doPFxconeDijetJetConstituentsNjets>0 || doPFxconeDijetJetConstituentsMinJetPt>0) && doXCone_dijet)
    doPFxconeDijetJetConstituents=true;
  if(doPFxconeDijetJetConstituentsMinJetPt<1e-6) doPFxconeDijetJetConstituentsMinJetPt=2e6;

  doGenXCone_dijet = iConfig.getParameter<bool>("doGenXCone_dijet");
  doGenxconeDijetJetConstituentsNjets = iConfig.getParameter<unsigned>("doGenxconeDijetJetConstituentsNjets");
  doGenxconeDijetJetConstituentsMinJetPt = iConfig.getParameter<double>("doGenxconeDijetJetConstituentsMinJetPt");
  if(doGenxconeDijetJetConstituentsMinJetPt<1e-6) doGenxconeDijetJetConstituentsMinJetPt=2e6;

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
  if(doPhotons){
      using uhh2::NtupleWriterPhotons;
      auto photon_source = iConfig.getParameter<edm::InputTag>("photon_sources");
      NtupleWriterPhotons::Config cfg(*context, consumesCollector(), photon_source, photon_source.label());
      cfg.id_keys = iConfig.getParameter<std::vector<std::string>>("photon_IDtags");
      assert(pv_sources.size() > 0); // note: pvs are needed for electron id.
      cfg.pv_src = pv_sources[0];
      cfg.doPuppiIso = true;
      if (year == "2016v2") { cfg.doPuppiIso = false; } // PUPPI isolation doens't exist in 80X
      writer_modules.emplace_back(new NtupleWriterPhotons(cfg, true, false));
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
        writer_modules.emplace_back(new NtupleWriterJets(cfg, i==0, muon_sources, elec_sources,doPFJetConstituentsNjets,doPFJetConstituentsMinJetPt));
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
        writer_modules.emplace_back(new NtupleWriterTopJets(cfg, j==0, muon_sources, elec_sources,doPFTopJetConstituentsNjets,doPFTopJetConstituentsMinJetPt));

      }
    }
  }

  if (doGenTopJets) {
    using uhh2::NtupleWriterGenTopJets;
    if(iConfig.exists("GenTopJets")){
      double topjet_ptmin = iConfig.getParameter<double> ("gentopjet_ptmin");
      double topjet_etamax = iConfig.getParameter<double> ("gentopjet_etamax");

      edm::VParameterSet gentopjets_list = iConfig.getParameterSetVector("GenTopJets");

      for(unsigned int j=0; j<gentopjets_list.size(); ++j){
        if(!gentopjets_list[j].exists("gentopjet_source")){
          throw cms::Exception("MissingSourceName", "It is necessary to specify the source of the topjet collection");
        }
        std::string gentopjet_source = gentopjets_list[j].getParameter<std::string>("gentopjet_source");
        if(!gentopjets_list[j].exists("subjet_source")){
          throw cms::Exception("MissingSourceName", "It is necessary to specify the subjets for each topjet collection");
        }
        std::string subjet_source = gentopjets_list[j].getParameter<std::string>("subjet_source");

        NtupleWriterGenTopJets::Config cfg(*context, consumesCollector(), gentopjet_source, gentopjet_source);
        cfg.ptmin = topjet_ptmin;
        cfg.etamax = topjet_etamax;
        cfg.subjet_src = subjet_source;

        bool substructure_variables = false;

        if(gentopjets_list[j].exists("njettiness_source")){
          cfg.njettiness_src = gentopjets_list[j].getParameter<std::string>("njettiness_source");
          substructure_variables = true;
        }

        if (gentopjets_list[j].exists("ecf_beta1_source")) {
          cfg.ecf_beta1_src = gentopjets_list[j].getParameter<std::string>("ecf_beta1_source");
          substructure_variables = true;
        }
        if (gentopjets_list[j].exists("ecf_beta2_source")) {
          cfg.ecf_beta2_src = gentopjets_list[j].getParameter<std::string>("ecf_beta2_source");
          substructure_variables = true;
        }

        if (substructure_variables) {
          if(!gentopjets_list[j].exists("substructure_variables_source")){
            throw cms::Exception("MissingSourceName", "njettiness or ecf sources defined without definition of substructure_variables_source");
          }
          cfg.substructure_variables_src = gentopjets_list[j].getParameter<std::string>("substructure_variables_source");
        }

        std::string topbranch = gentopjet_source;
        cfg.dest_branchname = topbranch;
        cfg.dest = topbranch;
        writer_modules.emplace_back(new NtupleWriterGenTopJets(cfg, j==0, doGenTopJetConstituentsNjets, doGenTopJetConstituentsMinJetPt));
      }
    }
  }

  // initialization of tree variables
  event.reset(new uhh2::Event(*ges));

  branch(tr, "run", &event->run);
  branch(tr, "event", &event->event);
  branch(tr, "luminosityBlock", &event->luminosityBlock);
  branch(tr, "isRealData", &event->isRealData);
  branch(tr, "year", &event->year);
  branch(tr, "rho", &event->rho);
  //always create rho branch, as some SFrame modules rely on it being present; only fill it
  // if doRho is true.
  if(doRho){
     auto rho_source = iConfig.getParameter<edm::InputTag>("rho_source");
     rho_token = consumes<double>(rho_source);
  }
  else{
    event->rho = -1;
  }

  if(doGenJets){
    auto genjet_sources = iConfig.getParameter<std::vector<std::string> >("genjet_sources");
    genjet_ptmin = iConfig.getParameter<double> ("genjet_ptmin");
    genjet_etamax = iConfig.getParameter<double> ("genjet_etamax");
    genjets.resize(genjet_sources.size());
    for(size_t j=0; j< genjet_sources.size(); ++j){
      genjet_tokens.push_back(consumes<vector<reco::GenJet>>(genjet_sources[j]));
      branch(tr, genjet_sources[j].c_str(), "std::vector<GenJet>", &genjets[j]);
    }
    if(!genjet_sources.empty()){
        event->genjets = &genjets[0];
    }
    genjetflavor_token = consumes<reco::JetFlavourInfoMatchingCollection > ( edm::InputTag("slimmedGenJetsFlavourInfos"));
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
    bs_token = consumes<reco::BeamSpot>( edm::InputTag("offlineBeamSpot"));
  }
  // these have to be written anyway, since they are needed in EventHelper
  branch(tr, "beamspot_x0",&event->beamspot_x0);
  branch(tr, "beamspot_y0",&event->beamspot_y0);
  branch(tr, "beamspot_z0",&event->beamspot_z0);

  if(doGenInfo){
    genparticle_token = consumes<reco::GenParticleCollection>(iConfig.getParameter<edm::InputTag>("genparticle_source"));
    generator_token = consumes<GenEventInfoProduct>( edm::InputTag("generator"));
    lhe_token = consumes<LHEEventProduct> ( edm::InputTag("externalLHEProducer"));
    // this one is necessary to read the LHERunInfoProduct:
    consumes<LHERunInfoProduct, edm::InRun>({"externalLHEProducer"});
    pus_token = consumes<std::vector<PileupSummaryInfo> > ( edm::InputTag("slimmedAddPileupInfo"));
    if(doStableGenParticles) stablegenparticle_token = consumes<edm::View<reco::Candidate> >(iConfig.getParameter<edm::InputTag>("stablegenparticle_source"));
    event->genInfo = new GenInfo();
    event->genparticles = new vector<GenParticle>();
    branch(tr, "genInfo","GenInfo", event->genInfo);
    branch(tr, "GenParticles","std::vector<GenParticle>", event->genparticles);
  }
  if(doPFJetConstituents || doPFTopJetConstituents || doPFxconeJetConstituents || doPFhotvrJetConstituents || doPFxconeDijetJetConstituents){
    event->pfparticles = new vector<PFParticle>();
    branch(tr, "PFParticles","std::vector<PFParticle>", event->pfparticles);
  }

  if(doTrigger){
    trigger_prefixes = iConfig.getParameter<std::vector<std::string> >("trigger_prefixes");
    event->get_triggerResults() = new vector<bool>();
    event->get_triggerPrescales() = new vector<int>();
    event->get_triggerPrescalesL1min() = new vector<int>();
    event->get_triggerPrescalesL1max() = new vector<int>();
    branch(tr, "triggerNames", "std::vector<std::string>", &triggerNames_outbranch);
    branch(tr, "triggerResults", "std::vector<bool>", event->get_triggerResults());
    branch(tr, "triggerPrescales", "std::vector<int>", event->get_triggerPrescales());
    branch(tr, "triggerPrescalesL1min", "std::vector<int>", event->get_triggerPrescalesL1min());
    branch(tr, "triggerPrescalesL1max", "std::vector<int>", event->get_triggerPrescalesL1max());
    triggerBits_ = consumes<edm::TriggerResults>(iConfig.getParameter<edm::InputTag>("trigger_bits"));
    edm::InputTag triggerPrescalesTag("patTrigger");
    triggerPrescales_ = consumes<pat::PackedTriggerPrescales>(triggerPrescalesTag);
    edm::InputTag triggerPrescalesL1minTag("patTrigger:l1min");
    triggerPrescalesL1min_ = consumes<pat::PackedTriggerPrescales>(triggerPrescalesL1minTag);
    edm::InputTag triggerPrescalesL1maxTag("patTrigger:l1max");
    triggerPrescalesL1max_ = consumes<pat::PackedTriggerPrescales>(triggerPrescalesL1maxTag);
    metfilterBits_ = consumes<edm::TriggerResults>(iConfig.getParameter<edm::InputTag>("metfilter_bits"));
    triggerObjects_ = consumes<pat::TriggerObjectStandAloneCollection>(iConfig.getParameter<edm::InputTag>("trigger_objects"));
    triggerObjects_sources = iConfig.getParameter<std::vector<std::string> >("triggerObjects_sources");
    triggerObjects_out.resize(triggerObjects_sources.size());
    for (size_t j=0; j<triggerObjects_sources.size(); j++){
      TString name = "triggerObjects_";
      name += triggerObjects_sources[j].c_str();
      branch(tr, name, "std::vector<FlavorParticle>", &triggerObjects_out[j]);
    }
    auto extraTriggers_sources = iConfig.getParameter<std::vector<edm::InputTag> >("extra_trigger_bits");
    for (const auto & etItr : extraTriggers_sources){
      extraTriggers_names.push_back(etItr.encode().c_str());
      extraTriggers_tokens.push_back(consumes<bool>(etItr));
    }
  }

  branch(tr, "passEcalBadCalib", &event->passEcalBadCalib);
  if(doEcalBadCalib) {
    ecalBadCalibFilterUpdate_token = consumes<bool>(iConfig.getParameter<edm::InputTag>("ecalBadCalib_source"));
  }

  branch(tr, "prefiringWeight", &event->prefiringWeight);
  branch(tr, "prefiringWeightUp", &event->prefiringWeightUp);
  branch(tr, "prefiringWeightDown", &event->prefiringWeightDown);
  if(doPrefire){
    std::string prefire_source = iConfig.getParameter<std::string>("prefire_source");
    prefweight_token = consumes<double>(edm::InputTag(prefire_source, "nonPrefiringProb"));
    prefweightup_token = consumes<double>(edm::InputTag(prefire_source, "nonPrefiringProbUp"));
    prefweightdown_token = consumes<double>(edm::InputTag(prefire_source, "nonPrefiringProbDown"));
  }
  if(doL1seed){
    l1GtToken_ = consumes<BXVector<GlobalAlgBlk>>(iConfig.getParameter<edm::InputTag>("l1GtSrc"));
    l1EGToken_ = consumes<BXVector<l1t::EGamma>>(iConfig.getParameter<edm::InputTag>("l1EGSrc"));
    l1JetToken_ = consumes<BXVector<l1t::Jet>>(iConfig.getParameter<edm::InputTag>("l1JetSrc"));
    l1MuonToken_ = consumes<BXVector<l1t::Muon>>(iConfig.getParameter<edm::InputTag>("l1MuonSrc"));
    l1EtSumToken_ = consumes<BXVector<l1t::EtSum>>(iConfig.getParameter<edm::InputTag>("l1EtSumSrc"));
    l1TauToken_ = consumes<BXVector<l1t::Tau>>(iConfig.getParameter<edm::InputTag>("l1TauSrc"));

    branch(tr,"L1EGamma_seeds","std::vector<L1EGamma>",&L1EG_seeds);
    branch(tr,"L1Jet_seeds","std::vector<L1Jet>",&L1Jet_seeds);
    branch(tr,"L1Muon_seeds","std::vector<L1Muon>",&L1Muon_seeds);
    branch(tr,"L1EtSum_seeds","std::vector<L1EtSum>",&L1EtSum_seeds);
    branch(tr,"L1Tau_seeds","std::vector<L1Tau>",&L1Tau_seeds);
  }

  if(doAllPFParticles){
    pf_collection_token = consumes<vector<pat::PackedCandidate>>(iConfig.getParameter<edm::InputTag>("pf_collection_source"));
    if(!doPFJetConstituents && !doPFTopJetConstituents && !doPFxconeJetConstituents && !doPFhotvrJetConstituents && !doPFxconeDijetJetConstituents){
      event->pfparticles = new vector<PFParticle>;
      branch(tr, "PFParticles", "std::vector<PFParticle>", &event->pfparticles);
    }
  }

  // HOTVR and XCone Jet Cluster - added by Alex and Dennis

  if (doHOTVR) {
    auto hotvr_sources = iConfig.getParameter<std::vector<edm::InputTag> >("HOTVR_sources");
    hotvrJets.resize(hotvr_sources.size());
    for (size_t j=0; j<hotvr_sources.size(); ++j) {
      hotvr_tokens.push_back(consumes<pat::JetCollection>(hotvr_sources[j]));
      branch(tr, hotvr_sources[j].encode().c_str(), "std::vector<TopJet>", &hotvrJets[j]);
    }
  }

  if (doXCone) {
    auto xcone_sources = iConfig.getParameter<std::vector<edm::InputTag> >("XCone_sources");
    xconeJets.resize(xcone_sources.size());
    for (size_t j=0; j<xcone_sources.size(); ++j) {
      xcone_tokens.push_back(consumes<pat::JetCollection>(xcone_sources[j]));
      branch(tr, xcone_sources[j].encode().c_str(), "std::vector<TopJet>", &xconeJets[j]);
    }
  }

  if (doXCone_dijet) {
    auto xcone_sources_dijet = iConfig.getParameter<std::vector<edm::InputTag> >("XCone_dijet_sources");
    xconeJets_dijet.resize(xcone_sources_dijet.size());
    for (size_t j=0; j<xcone_sources_dijet.size(); ++j) {
      xcone_tokens_dijet.push_back(consumes<pat::JetCollection>(xcone_sources_dijet[j]));
      branch(tr, xcone_sources_dijet[j].encode().c_str(), "std::vector<TopJet>", &xconeJets_dijet[j]);
    }
  }


  if (doGenHOTVR) {
    auto genhotvr_sources = iConfig.getParameter<std::vector<edm::InputTag> >("GenHOTVR_sources");
    genhotvrJets.resize(genhotvr_sources.size());
    for (size_t j=0; j<genhotvr_sources.size(); ++j) {
      genhotvr_tokens.push_back(consumes<pat::JetCollection>(genhotvr_sources[j]));
      branch(tr, genhotvr_sources[j].encode().c_str(), "std::vector<GenTopJet>", &genhotvrJets[j]);
    }
  }

  if (doGenXCone) {
    auto genxcone_sources = iConfig.getParameter<std::vector<edm::InputTag> >("GenXCone_sources");
    genxconeJets.resize(genxcone_sources.size());
    for (size_t j=0; j<genxcone_sources.size(); ++j) {
      genxcone_tokens.push_back(consumes<pat::JetCollection>(genxcone_sources[j]));
      branch(tr, genxcone_sources[j].encode().c_str(), "std::vector<GenTopJet>", &genxconeJets[j]);
    }
  }
  if (doGenXCone_dijet) {
    auto genxcone_sources_dijet = iConfig.getParameter<std::vector<edm::InputTag> >("GenXCone_dijet_sources");
    genxconeJets_dijet.resize(genxcone_sources_dijet.size());
    for (size_t j=0; j<genxcone_sources_dijet.size(); ++j) {
      genxcone_tokens_dijet.push_back(consumes<pat::JetCollection>(genxcone_sources_dijet[j]));
      branch(tr, genxcone_sources_dijet[j].encode().c_str(), "std::vector<GenTopJet>", &genxconeJets_dijet[j]);
    }
  }
  newrun = true;
  firstEvent = true;
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
   event->year = year;

   if(doRho){
      edm::Handle<double> m_rho;
      iEvent.getByToken(rho_token, m_rho);
      event->rho=*m_rho;
   }


   print_times(timer, "rho");

   // ------------- PF constituents --------------
   // must do before handling jet collections, since they may store constituents,
   // which lookup against event->pfparticles
   if (event->pfparticles != nullptr) event->pfparticles->clear();
   if(doAllPFParticles){
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
     // in case doStableGenParticles is false.
     bool hasGPs = iEvent.getByToken(genparticle_token, genPartColl);
     int index=-1;
     if (hasGPs) {
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
     }

     //store stable gen particles from packed collection
     if(doStableGenParticles){
       edm::Handle<edm::View<reco::Candidate> > packed;
       // use packed particle collection for all STABLE (status 1) particles
       iEvent.getByToken(stablegenparticle_token,packed);

       for(size_t j=0; j<packed->size();j++){
         const pat::PackedGenParticle* iter = dynamic_cast<const pat::PackedGenParticle*>(&(packed->at(j)));

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

         event->genparticles->push_back(genp);
       }
     }

   }

   print_times(timer, "geninfo");

   //-------------- gen jets -------------

   if(doGenJets){
     edm::Handle<reco::JetFlavourInfoMatchingCollection> genjetflavorMatching;
     bool hasFlavInfo = iEvent.getByToken(genjetflavor_token, genjetflavorMatching);

     for(size_t j=0; j< genjet_tokens.size(); ++j){
       genjets[j].clear();
       edm::Handle< std::vector<reco::GenJet> > genjet_handle;
       iEvent.getByToken(genjet_tokens[j], genjet_handle);
       const std::vector<reco::GenJet>& gen_jets = *genjet_handle;
       for (unsigned int i = 0; i < gen_jets.size(); ++i) {
         const reco::GenJet & gen_jet = gen_jets[i];
         if(gen_jet.pt() < genjet_ptmin) continue;
         if(fabs(gen_jet.eta()) > genjet_etamax) continue;

         GenJet jet;
         jet.set_charge(gen_jet.charge());
         jet.set_pt(gen_jet.pt());
         jet.set_eta(gen_jet.eta());
         jet.set_phi(gen_jet.phi());
         jet.set_pt(gen_jet.pt());
         jet.set_energy(gen_jet.energy());
         if (hasFlavInfo) {
           for ( reco::JetFlavourInfoMatchingCollection::const_iterator j  = genjetflavorMatching->begin(); j != genjetflavorMatching->end(); ++j ) {
             const reco::Jet *aJet = (*j).first.get();
             if(&gen_jet == aJet){
               reco::JetFlavourInfo aInfo = (*j).second;
               jet.set_partonFlavour(aInfo.getPartonFlavour());
               jet.set_hadronFlavour(aInfo.getHadronFlavour());
             }
           }
         }
         bool add_genparts=false;
         if(genjets[j].size()<doGenJetConstituentsNjets || gen_jet.pt()>doGenJetConstituentsMinJetPt) add_genparts=true;
         fill_geninfo_recojet(gen_jet,jet,add_genparts);
         genjets[j].push_back(jet);
       } // end loop over gen_jets
     } // end loop over genjet_tokens
   } // end if doGenJets
   print_times(timer, "genjets");


   for(auto & m : writer_modules){
       m->process(iEvent, *event, iSetup);
   }

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
         met[j].set_sumEt(pat_met.sumEt());
         met[j].set_mEtSignificance(pat_met.metSignificance());
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
               met[j].set_rawCHS_px((year != "2016v2" ? pat_met.corPx(pat::MET::METCorrectionLevel::RawChs) : pat_met.px()));  // for 2016v2, rawCHS wasn't stored,
               met[j].set_rawCHS_py((year != "2016v2" ? pat_met.corPy(pat::MET::METCorrectionLevel::RawChs) : pat_met.py()));  // so while it "works", it just returns junk
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
         genmet[j].set_sumEt(pat_genmet.genMET()->sumEt());
         // Calculate the met significance ourselves, since it isn't stored
         // this doesn't actually work for genMET - significane matrix probably needs calculating
         // genmet[j].set_mEtSignificance(metsig::METSignificance::getSignificance(pat_genmet.genMET()->getSignificanceMatrix(), *(pat_genmet.genMET())));
         // Instead just store MET/sqrt(sumEt)
         genmet[j].set_mEtSignificance(pat_genmet.genMET()->mEtSig());
         //uncorrected MET is equal to normal MET for GenMET
         genmet[j].set_uncorr_pt(pat_genmet.genMET()->pt());
         genmet[j].set_uncorr_phi(pat_genmet.genMET()->phi());
       }
     }
   }

   print_times(timer, "met");

   // ------------- trigger -------------


   if(doTrigger){
     auto & triggerResults = *event->get_triggerResults();
     auto & triggerPrescales = *event->get_triggerPrescales();
     auto & triggerPrescalesL1min = *event->get_triggerPrescalesL1min();
     auto & triggerPrescalesL1max = *event->get_triggerPrescalesL1max();
     triggerResults.clear();
     triggerPrescales.clear();
     triggerNames_outbranch.clear();
     triggerPrescalesL1min.clear();
     triggerPrescalesL1max.clear();

     //read trigger info from triggerBits (k=0) and from metfilterBits (k=1)
     for(int k=0;k<2; k++){
       edm::Handle<edm::TriggerResults> triggerBits;
       edm::Handle<pat::PackedTriggerPrescales> packedTriggerPrescales;
       edm::Handle<pat::PackedTriggerPrescales> packedTriggerPrescalesL1min;
       edm::Handle<pat::PackedTriggerPrescales> packedTriggerPrescalesL1max;
       if(k==0)
         iEvent.getByToken(triggerBits_, triggerBits);
       else
         iEvent.getByToken(metfilterBits_, triggerBits);

       if(iEvent.isRealData()){
         iEvent.getByToken(triggerPrescales_, packedTriggerPrescales);
         iEvent.getByToken(triggerPrescalesL1min_, packedTriggerPrescalesL1min);
         iEvent.getByToken(triggerPrescalesL1min_, packedTriggerPrescalesL1max);
       }
       const edm::TriggerNames &names = iEvent.triggerNames(*triggerBits);

       for (unsigned int i = 0, n = triggerBits->size(); i < n; ++i) {
         std::vector<std::string>::const_iterator it = trigger_prefixes.begin();
         for(; it!=trigger_prefixes.end(); ++it){
           if(names.triggerName(i).substr(0, it->size()) == *it)break;
         }
         if(it==trigger_prefixes.end()) continue;
         triggerResults.push_back(triggerBits->accept(i));

         if(iEvent.isRealData()){
           triggerPrescales.push_back(packedTriggerPrescales->getPrescaleForIndex(i));
           triggerPrescalesL1min.push_back(packedTriggerPrescalesL1min->getPrescaleForIndex(i));
           triggerPrescalesL1max.push_back(packedTriggerPrescalesL1max->getPrescaleForIndex(i));
         }
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
     }
     // store extra trigger bits
     for (uint iET=0; iET<extraTriggers_names.size(); iET++) {
       if(newrun) {
        std::string newTrigName = "Extra_"+extraTriggers_names[iET];
        triggerNames_outbranch.push_back(newTrigName);
       }
       edm::Handle<bool> extraTrigHandle;
       iEvent.getByToken(extraTriggers_tokens[iET], extraTrigHandle);
       triggerResults.push_back(*extraTrigHandle);
       if(iEvent.isRealData()){
         // need to fill as checked when asking for any trigger prescale
         triggerPrescales.push_back(1);
         triggerPrescalesL1min.push_back(1);
         triggerPrescalesL1max.push_back(1);
       }
     }

     if(newrun){
         event->set_triggernames(triggerNames_outbranch);
     }
     newrun=false;
   } // end doTrigger

   if (doEcalBadCalib) {
     edm::Handle<bool> passEcalBadCalibFilterUpdate;
     iEvent.getByToken(ecalBadCalibFilterUpdate_token, passEcalBadCalibFilterUpdate);
     event->passEcalBadCalib = (*passEcalBadCalibFilterUpdate);
   }

   if (doPrefire) {
     edm::Handle<double> theprefweight;
     iEvent.getByToken(prefweight_token, theprefweight);
     float prefiringWeight = (*theprefweight);
     event->prefiringWeight = prefiringWeight;

     edm::Handle<double> theprefweightup;
     iEvent.getByToken(prefweightup_token, theprefweightup);
     float prefiringWeightUp = (*theprefweightup);
     event->prefiringWeightUp = prefiringWeightUp;

     edm::Handle<double> theprefweightdown;
     iEvent.getByToken(prefweightdown_token, theprefweightdown);
     float prefiringWeightDown = (*theprefweightdown);
     event->prefiringWeightDown = prefiringWeightDown;

   }

   print_times(timer, "trigger+prefire");


   if(doL1seed){
     if(iEvent.isRealData()) {
       auto & triggerResults = *event->get_triggerResults();
       auto & triggerPrescales = *event->get_triggerPrescales();
       auto & triggerPrescalesL1min = *event->get_triggerPrescalesL1min();
       auto & triggerPrescalesL1max = *event->get_triggerPrescalesL1max();

       //muGT
       edm::Handle<BXVector<GlobalAlgBlk>> l1GtHandle;
       iEvent.getByToken(l1GtToken_, l1GtHandle);
       bool prefire = l1GtHandle->begin(-1)->getFinalOR();
       triggerNames_outbranch.push_back("muGT_BX_minus_1__prefire");
       triggerResults.push_back(prefire);
       triggerPrescales.push_back(1);
       triggerPrescalesL1min.push_back(1);
       triggerPrescalesL1max.push_back(1);

       bool postfire = l1GtHandle->begin(1)->getFinalOR();
       triggerNames_outbranch.push_back("muGT_BX_plus_1");
       triggerResults.push_back(postfire);
       triggerPrescales.push_back(1);
       triggerPrescalesL1min.push_back(1);
       triggerPrescalesL1max.push_back(1);

       bool fire = l1GtHandle->begin(0)->getFinalOR();
       triggerNames_outbranch.push_back("muGT_BX_plus_0");
       triggerResults.push_back(fire);
       triggerPrescales.push_back(1);
       triggerPrescalesL1min.push_back(1);
       triggerPrescalesL1max.push_back(1);
     }

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
      	 l1eg.set_Shape(itL1->shape());
      	 l1eg.set_iso(itL1->hwIso());
      	 L1EG_seeds.push_back(l1eg);
       }
     };
     readBx(*l1EGHandle, 0);
     if(iEvent.isRealData()) {
       readBx(*l1EGHandle, -2);
       readBx(*l1EGHandle, -1);
       readBx(*l1EGHandle, +1);
       readBx(*l1EGHandle, +2);
     }

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
      	 l1jet.set_puEt(itL1->puEt());
      	 l1jet.set_seedEt(itL1->seedEt());
      	 l1jet.set_rawEt(itL1->rawEt());
      	 L1Jet_seeds.push_back(l1jet);
       }
     };
     readBxjet(*l1JetHandle, 0);
     if(iEvent.isRealData()) {
       readBxjet(*l1JetHandle, -2);
       readBxjet(*l1JetHandle, -1);
       readBxjet(*l1JetHandle, +1);
       readBxjet(*l1JetHandle, +2);
     }

     //L1Muon
     edm::Handle<BXVector<l1t::Muon>> l1MuonHandle;
     iEvent.getByToken(l1MuonToken_, l1MuonHandle);
     L1Muon_seeds.clear();
     auto readBxmuon = [&] (const BXVector<l1t::Muon>& egVect, int bx) {
       for (auto itL1=l1MuonHandle->begin(bx); itL1!=l1MuonHandle->end(bx); ++itL1) {
      	 L1Muon l1muon;
      	 l1muon.set_pt(itL1->p4().Pt());
      	 l1muon.set_eta(itL1->p4().Eta());
      	 l1muon.set_phi(itL1->p4().Phi());
      	 l1muon.set_energy(itL1->p4().energy());
         l1muon.set_charge(itL1->charge());

         l1muon.set_bx(bx);

         l1muon.set_hwPt(itL1->hwPt());
         l1muon.set_hwEta(itL1->hwEta());
         l1muon.set_hwPhi(itL1->hwPhi());
         l1muon.set_hwQual(itL1->hwQual());
         l1muon.set_hwIso(itL1->hwIso());

         l1muon.set_hwCharge(itL1->hwCharge());
         l1muon.set_hwChargeValid(itL1->hwChargeValid());
         l1muon.set_tfMuonIndex(itL1->tfMuonIndex());
         l1muon.set_hwTag(itL1->hwTag());

         l1muon.set_hwEtaAtVtx(itL1->hwEtaAtVtx());
         l1muon.set_hwPhiAtVtx(itL1->hwPhiAtVtx());
         l1muon.set_etaAtVtx(itL1->etaAtVtx());
         l1muon.set_phiAtVtx(itL1->phiAtVtx());

         l1muon.set_hwIsoSum(itL1->hwIsoSum());
         l1muon.set_hwDPhiExtra(itL1->hwDPhiExtra());
         l1muon.set_hwDEtaExtra(itL1->hwDEtaExtra());
         l1muon.set_hwRank(itL1->hwRank());

         l1muon.set_debug(itL1->debug());

         L1Muon_seeds.push_back(l1muon);
       }
     };
     readBxmuon(*l1MuonHandle, 0);
     if(iEvent.isRealData()) {
       readBxmuon(*l1MuonHandle, -2);
       readBxmuon(*l1MuonHandle, -1);
       readBxmuon(*l1MuonHandle, +1);
       readBxmuon(*l1MuonHandle, +2);
     }

     //l1EtSum
     edm::Handle<BXVector<l1t::EtSum>> l1EtSumHandle;
     iEvent.getByToken(l1EtSumToken_, l1EtSumHandle);
     L1EtSum_seeds.clear();
     auto readBxetsum = [&] (const BXVector<l1t::EtSum>& egVect, int bx) {
       for (auto itL1=l1EtSumHandle->begin(bx); itL1!=l1EtSumHandle->end(bx); ++itL1) {
         L1EtSum l1EtSum;

         // checking and setting type
         if(itL1->getType() == l1t::EtSum::EtSumType::kTotalEt) {
           l1EtSum.set_type(L1EtSum::EtSumType::kTotalEt);
         }
         else if(itL1->getType() == l1t::EtSum::EtSumType::kTotalHt) {
           l1EtSum.set_type(L1EtSum::EtSumType::kTotalHt);
         }
         else if(itL1->getType() == l1t::EtSum::EtSumType::kMissingEt) {
           l1EtSum.set_type(L1EtSum::EtSumType::kMissingEt);
         }
         else if(itL1->getType() == l1t::EtSum::EtSumType::kMissingHt) {
           l1EtSum.set_type(L1EtSum::EtSumType::kMissingHt);
         }
         else if(itL1->getType() == l1t::EtSum::EtSumType::kTotalEtx) {
           l1EtSum.set_type(L1EtSum::EtSumType::kTotalEtx);
         }
         else if(itL1->getType() == l1t::EtSum::EtSumType::kTotalEty) {
           l1EtSum.set_type(L1EtSum::EtSumType::kTotalEty);
         }
         else if(itL1->getType() == l1t::EtSum::EtSumType::kTotalHtx) {
           l1EtSum.set_type(L1EtSum::EtSumType::kTotalEtx);
         }
         else if(itL1->getType() == l1t::EtSum::EtSumType::kTotalHty) {
           l1EtSum.set_type(L1EtSum::EtSumType::kTotalEty);
         }
         else continue; // only keeping a selection of types for the moment

      	 l1EtSum.set_pt(itL1->p4().Pt());
      	 l1EtSum.set_eta(itL1->p4().Eta());
      	 l1EtSum.set_phi(itL1->p4().Phi());
      	 l1EtSum.set_energy(itL1->p4().energy());
         l1EtSum.set_charge(itL1->charge());

         l1EtSum.set_bx(bx);

         l1EtSum.set_hwPt(itL1->hwPt());
         l1EtSum.set_hwEta(itL1->hwEta());
         l1EtSum.set_hwPhi(itL1->hwPhi());
         l1EtSum.set_hwQual(itL1->hwQual());
         l1EtSum.set_hwIso(itL1->hwIso());

         L1EtSum_seeds.push_back(l1EtSum);
       }
     };
     readBxetsum(*l1EtSumHandle, 0);
     if(iEvent.isRealData()) {
       readBxetsum(*l1EtSumHandle, -2);
       readBxetsum(*l1EtSumHandle, -1);
       readBxetsum(*l1EtSumHandle, +1);
       readBxetsum(*l1EtSumHandle, +2);
     }


     //L1Tau
     edm::Handle<BXVector<l1t::Tau>> l1TauHandle;
     iEvent.getByToken(l1TauToken_, l1TauHandle);
     L1Tau_seeds.clear();
     auto readBxtau = [&] (const BXVector<l1t::Tau>& egVect, int bx) {
       for (auto itL1=l1TauHandle->begin(bx); itL1!=l1TauHandle->end(bx); ++itL1) {
         L1Tau l1tau;
         l1tau.set_pt(itL1->p4().Pt());
         l1tau.set_eta(itL1->p4().Eta());
         l1tau.set_phi(itL1->p4().Phi());
         l1tau.set_energy(itL1->p4().energy());
         l1tau.set_charge(itL1->charge());

         l1tau.set_bx(bx);

         l1tau.set_hwPt(itL1->hwPt());
         l1tau.set_hwEta(itL1->hwEta());
         l1tau.set_hwPhi(itL1->hwPhi());
         l1tau.set_hwQual(itL1->hwQual());
         l1tau.set_hwIso(itL1->hwIso());

         l1tau.set_towerIEta(itL1->towerIEta());
         l1tau.set_towerIPhi(itL1->towerIPhi());
         l1tau.set_rawEt(itL1->rawEt());
         l1tau.set_isoEt(itL1->isoEt());
         l1tau.set_nTT(itL1->nTT());
         l1tau.set_hasEM(itL1->hasEM());
         l1tau.set_isMerged(itL1->isMerged());

         L1Tau_seeds.push_back(l1tau);
       }
     };
     readBxtau(*l1TauHandle, 0);
     if(iEvent.isRealData()) {
       readBxtau(*l1TauHandle, -2);
       readBxtau(*l1TauHandle, -1);
       readBxtau(*l1TauHandle, +1);
       readBxtau(*l1TauHandle, +2);
     }

   }


   // ------------- HOTVR and XCone Jets  -------------
   if (doHOTVR) {
    for (size_t j=0; j < hotvr_tokens.size(); ++j){
      hotvrJets[j].clear();
      edm::Handle<pat::JetCollection> hotvr_patjets;
      iEvent.getByToken(hotvr_tokens[j], hotvr_patjets);

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
	thisJet.set_numberOfDaughters(patJet.numberOfDaughters());
	thisJet.set_partonFlavour(patJet.partonFlavour());
	thisJet.set_hadronFlavour(patJet.hadronFlavour());
	thisJet.set_charge(patJet.charge());
	if(patJet.isPFJet()){
	  thisJet.set_neutralEmEnergyFraction (patJet.neutralEmEnergyFraction());
	  thisJet.set_neutralHadronEnergyFraction (patJet.neutralHadronEnergyFraction());
	  thisJet.set_chargedEmEnergyFraction (patJet.chargedEmEnergyFraction());
	  thisJet.set_chargedHadronEnergyFraction (patJet.chargedHadronEnergyFraction());
	  thisJet.set_muonEnergyFraction (patJet.muonEnergyFraction());
	  thisJet.set_photonEnergyFraction (patJet.photonEnergyFraction());
	  thisJet.set_chargedMultiplicity (patJet.chargedMultiplicity());
	  thisJet.set_neutralMultiplicity (patJet.neutralMultiplicity());
	  thisJet.set_muonMultiplicity (patJet.muonMultiplicity());
	  thisJet.set_electronMultiplicity (patJet.electronMultiplicity());
	  thisJet.set_photonMultiplicity (patJet.photonMultiplicity());
	}
	thisJet.set_JEC_factor_raw(1.);
	thisJet.set_JEC_L1factor_raw(1.);
	bool storePFparts = (hotvrJets[j].size()<doPFhotvrJetConstituentsNjets || thisJet.pt()>doPFhotvrJetConstituentsMinJetPt);
	if(storePFparts){
	  const auto& jet_daughter_ptrs = patJet.daughterPtrVector();
	  for(const auto & daughter_p : jet_daughter_ptrs){
	    size_t pfparticles_index = uhh2::add_pfpart(*daughter_p,*event->pfparticles);
	    thisJet.add_pfcand_index(pfparticles_index);
	  }
	}

        for (const auto & subItr : patJet.subjets()) {
          Jet subjet;
          subjet.set_pt(subItr->p4().pt());
          subjet.set_eta(subItr->p4().eta());
          subjet.set_phi(subItr->p4().phi());
          subjet.set_energy(subItr->p4().E());
          subjet.set_jetArea(subItr->jetArea());
	  subjet.set_numberOfDaughters(subItr->numberOfDaughters());
	  subjet.set_partonFlavour(subItr->partonFlavour());
	  subjet.set_hadronFlavour(subItr->hadronFlavour());
	  subjet.set_charge(subItr->charge());
	  if(subItr->isPFJet()){
	    subjet.set_neutralEmEnergyFraction (subItr->neutralEmEnergyFraction());
	    subjet.set_neutralHadronEnergyFraction (subItr->neutralHadronEnergyFraction());
	    subjet.set_chargedEmEnergyFraction (subItr->chargedEmEnergyFraction());
	    subjet.set_chargedHadronEnergyFraction (subItr->chargedHadronEnergyFraction());
	    subjet.set_muonEnergyFraction (subItr->muonEnergyFraction());
	    subjet.set_photonEnergyFraction (subItr->photonEnergyFraction());
	    subjet.set_chargedMultiplicity (subItr->chargedMultiplicity());
	    subjet.set_neutralMultiplicity (subItr->neutralMultiplicity());
	    subjet.set_muonMultiplicity (subItr->muonMultiplicity());
	    subjet.set_electronMultiplicity (subItr->electronMultiplicity());
	    subjet.set_photonMultiplicity (subItr->photonMultiplicity());
	  }
	  subjet.set_JEC_factor_raw(1.);
	  subjet.set_JEC_L1factor_raw(1.);
	  if(storePFparts){
	    const auto& jet_daughter_ptrs = subItr->daughterPtrVector();
	    for(const auto & daughter_p : jet_daughter_ptrs){
	      size_t pfparticles_index = uhh2::add_pfpart(*daughter_p,*event->pfparticles);
	      subjet.add_pfcand_index(pfparticles_index);
	    }
	  }
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

      // Convert from pat::Jet to TopJet, with special userFloats, and with subjets
      for (const auto & patJet : *xcone_patjets) {
        TopJet thisJet;
        thisJet.set_pt(patJet.p4().pt());
        thisJet.set_eta(patJet.p4().eta());
        thisJet.set_phi(patJet.p4().phi());
        thisJet.set_energy(patJet.p4().E());
        thisJet.set_jetArea(patJet.jetArea());
        thisJet.set_softdropmass(patJet.userFloat("softdropmass"));
	thisJet.set_numberOfDaughters(patJet.numberOfDaughters());
	thisJet.set_partonFlavour(patJet.partonFlavour());
	thisJet.set_hadronFlavour(patJet.hadronFlavour());
	thisJet.set_charge(patJet.charge());
	if(patJet.isPFJet()){
	  thisJet.set_neutralEmEnergyFraction (patJet.neutralEmEnergyFraction());
	  thisJet.set_neutralHadronEnergyFraction (patJet.neutralHadronEnergyFraction());
	  thisJet.set_chargedEmEnergyFraction (patJet.chargedEmEnergyFraction());
	  thisJet.set_chargedHadronEnergyFraction (patJet.chargedHadronEnergyFraction());
	  thisJet.set_muonEnergyFraction (patJet.muonEnergyFraction());
	  thisJet.set_photonEnergyFraction (patJet.photonEnergyFraction());
	  thisJet.set_chargedMultiplicity (patJet.chargedMultiplicity());
	  thisJet.set_neutralMultiplicity (patJet.neutralMultiplicity());
	  thisJet.set_muonMultiplicity (patJet.muonMultiplicity());
	  thisJet.set_electronMultiplicity (patJet.electronMultiplicity());
	  thisJet.set_photonMultiplicity (patJet.photonMultiplicity());
	}
	thisJet.set_JEC_factor_raw(1.);
	thisJet.set_JEC_L1factor_raw(1.);
	bool storePFparts = (xconeJets[j].size()<doPFxconeJetConstituentsNjets || thisJet.pt()>doPFxconeJetConstituentsMinJetPt);
	if(storePFparts){
	  const auto& jet_daughter_ptrs = patJet.daughterPtrVector();
	  for(const auto & daughter_p : jet_daughter_ptrs){
	    size_t pfparticles_index = uhh2::add_pfpart(*daughter_p,*event->pfparticles);
	    thisJet.add_pfcand_index(pfparticles_index);
	  }
	}

        for (const auto & subItr : patJet.subjets()) {
          Jet subjet;
          subjet.set_pt(subItr->p4().pt());
          subjet.set_eta(subItr->p4().eta());
          subjet.set_phi(subItr->p4().phi());
          subjet.set_energy(subItr->p4().E());
          subjet.set_jetArea(subItr->jetArea());
	  subjet.set_numberOfDaughters(subItr->numberOfDaughters());
	  subjet.set_partonFlavour(subItr->partonFlavour());
	  subjet.set_hadronFlavour(subItr->hadronFlavour());
	  subjet.set_charge(subItr->charge());
	  if(subItr->isPFJet()){
	    subjet.set_neutralEmEnergyFraction (subItr->neutralEmEnergyFraction());
	    subjet.set_neutralHadronEnergyFraction (subItr->neutralHadronEnergyFraction());
	    subjet.set_chargedEmEnergyFraction (subItr->chargedEmEnergyFraction());
	    subjet.set_chargedHadronEnergyFraction (subItr->chargedHadronEnergyFraction());
	    subjet.set_muonEnergyFraction (subItr->muonEnergyFraction());
	    subjet.set_photonEnergyFraction (subItr->photonEnergyFraction());
	    subjet.set_chargedMultiplicity (subItr->chargedMultiplicity());
	    subjet.set_neutralMultiplicity (subItr->neutralMultiplicity());
	    subjet.set_muonMultiplicity (subItr->muonMultiplicity());
	    subjet.set_electronMultiplicity (subItr->electronMultiplicity());
	    subjet.set_photonMultiplicity (subItr->photonMultiplicity());
	  }
	  subjet.set_JEC_factor_raw(1.);
	  subjet.set_JEC_L1factor_raw(1.);
	  if(storePFparts){
	    const auto& jet_daughter_ptrs = subItr->daughterPtrVector();
	    for(const auto & daughter_p : jet_daughter_ptrs){
	      size_t pfparticles_index = uhh2::add_pfpart(*daughter_p,*event->pfparticles);
	      subjet.add_pfcand_index(pfparticles_index);
	    }
	  }
          thisJet.add_subjet(subjet);
        }
        xconeJets[j].push_back(thisJet);
      }
    }
   }

   //dijet XCONE
   if (doXCone_dijet) {
    for (size_t j=0; j < xcone_tokens_dijet.size(); ++j){
      xconeJets_dijet[j].clear();
      edm::Handle<pat::JetCollection> xcone_patjets_dijet;
      iEvent.getByToken(xcone_tokens_dijet[j], xcone_patjets_dijet);

      // Convert from pat::Jet to TopJet, with special userFloats, and with subjets
      for (const auto & patJet : *xcone_patjets_dijet) {
        TopJet thisJet;
        thisJet.set_pt(patJet.p4().pt());
        thisJet.set_eta(patJet.p4().eta());
        thisJet.set_phi(patJet.p4().phi());
        thisJet.set_energy(patJet.p4().E());
        thisJet.set_jetArea(patJet.jetArea());
        thisJet.set_softdropmass(patJet.userFloat("softdropmass"));
	thisJet.set_numberOfDaughters(patJet.numberOfDaughters());
	thisJet.set_partonFlavour(patJet.partonFlavour());
	thisJet.set_hadronFlavour(patJet.hadronFlavour());
	thisJet.set_charge(patJet.charge());
	if(patJet.isPFJet()){
	  thisJet.set_neutralEmEnergyFraction (patJet.neutralEmEnergyFraction());
	  thisJet.set_neutralHadronEnergyFraction (patJet.neutralHadronEnergyFraction());
	  thisJet.set_chargedEmEnergyFraction (patJet.chargedEmEnergyFraction());
	  thisJet.set_chargedHadronEnergyFraction (patJet.chargedHadronEnergyFraction());
	  thisJet.set_muonEnergyFraction (patJet.muonEnergyFraction());
	  thisJet.set_photonEnergyFraction (patJet.photonEnergyFraction());
	  thisJet.set_chargedMultiplicity (patJet.chargedMultiplicity());
	  thisJet.set_neutralMultiplicity (patJet.neutralMultiplicity());
	  thisJet.set_muonMultiplicity (patJet.muonMultiplicity());
	  thisJet.set_electronMultiplicity (patJet.electronMultiplicity());
	  thisJet.set_photonMultiplicity (patJet.photonMultiplicity());
	}
	thisJet.set_JEC_factor_raw(1.);
	thisJet.set_JEC_L1factor_raw(1.);
	bool storePFparts = (xconeJets_dijet[j].size()<doPFxconeDijetJetConstituentsNjets || thisJet.pt()>doPFxconeDijetJetConstituentsMinJetPt);
	if(storePFparts){
	  const auto& jet_daughter_ptrs = patJet.daughterPtrVector();
	  for(const auto & daughter_p : jet_daughter_ptrs){
	    size_t pfparticles_index = uhh2::add_pfpart(*daughter_p,*event->pfparticles);
	    thisJet.add_pfcand_index(pfparticles_index);
	  }
	}

        for (const auto & subItr : patJet.subjets()) {
          Jet subjet;
          subjet.set_pt(subItr->p4().pt());
          subjet.set_eta(subItr->p4().eta());
          subjet.set_phi(subItr->p4().phi());
          subjet.set_energy(subItr->p4().E());
          subjet.set_jetArea(subItr->jetArea());
	  subjet.set_numberOfDaughters(subItr->numberOfDaughters());
	  subjet.set_partonFlavour(subItr->partonFlavour());
	  subjet.set_hadronFlavour(subItr->hadronFlavour());
	  subjet.set_charge(subItr->charge());
	  if(subItr->isPFJet()){
	    subjet.set_neutralEmEnergyFraction (subItr->neutralEmEnergyFraction());
	    subjet.set_neutralHadronEnergyFraction (subItr->neutralHadronEnergyFraction());
	    subjet.set_chargedEmEnergyFraction (subItr->chargedEmEnergyFraction());
	    subjet.set_chargedHadronEnergyFraction (subItr->chargedHadronEnergyFraction());
	    subjet.set_muonEnergyFraction (subItr->muonEnergyFraction());
	    subjet.set_photonEnergyFraction (subItr->photonEnergyFraction());
	    subjet.set_chargedMultiplicity (subItr->chargedMultiplicity());
	    subjet.set_neutralMultiplicity (subItr->neutralMultiplicity());
	    subjet.set_muonMultiplicity (subItr->muonMultiplicity());
	    subjet.set_electronMultiplicity (subItr->electronMultiplicity());
	    subjet.set_photonMultiplicity (subItr->photonMultiplicity());
	  }
	  subjet.set_JEC_factor_raw(1.);
	  subjet.set_JEC_L1factor_raw(1.);
	  if(storePFparts){
	    const auto& jet_daughter_ptrs = subItr->daughterPtrVector();
	    for(const auto & daughter_p : jet_daughter_ptrs){
	      size_t pfparticles_index = uhh2::add_pfpart(*daughter_p,*event->pfparticles);
	      subjet.add_pfcand_index(pfparticles_index);
	    }
	  }
          thisJet.add_subjet(subjet);
        }

        xconeJets_dijet[j].push_back(thisJet);
      }
    }
   }

   if (doGenHOTVR) {
    for (size_t j=0; j < genhotvr_tokens.size(); ++j){
      genhotvrJets[j].clear();
      edm::Handle<pat::JetCollection> genhotvr_patjets;
      iEvent.getByToken(genhotvr_tokens[j], genhotvr_patjets);

      // Convert from pat::Jet to GenTopJet with subjets
      for (const auto & patJet : *genhotvr_patjets) {
        GenTopJet thisJet;
        thisJet.set_pt(patJet.p4().pt());
        thisJet.set_eta(patJet.p4().eta());
        thisJet.set_phi(patJet.p4().phi());
        thisJet.set_energy(patJet.p4().E());
	thisJet.set_partonFlavour(patJet.partonFlavour());
	thisJet.set_hadronFlavour(patJet.hadronFlavour());
	bool add_genparts=false;
	if(genhotvrJets[j].size()<doGenhotvrJetConstituentsNjets || thisJet.pt()>doGenhotvrJetConstituentsMinJetPt) add_genparts=true;
        for (const auto & subItr : patJet.subjets()) {
          GenJet subjet;
          subjet.set_pt(subItr->p4().pt());
          subjet.set_eta(subItr->p4().eta());
          subjet.set_phi(subItr->p4().phi());
          subjet.set_energy(subItr->p4().E());
          subjet.set_partonFlavour(subItr->partonFlavour());
          subjet.set_hadronFlavour(subItr->hadronFlavour());
	  fill_geninfo_patjet(subItr,subjet,add_genparts);
          thisJet.add_subjet(subjet);
        }
	fill_geninfo_patjet(patJet,thisJet,add_genparts);
        genhotvrJets[j].push_back(thisJet);
      }
    }
   }

  if (doGenXCone) {
    for (size_t j=0; j < genxcone_tokens.size(); ++j){
      genxconeJets[j].clear();
      edm::Handle<pat::JetCollection> genxcone_patjets;
      iEvent.getByToken(genxcone_tokens[j], genxcone_patjets);

      // Convert from pat::Jet to GenTopJet with subjets
      for (const auto & patJet : *genxcone_patjets) {
        GenTopJet thisJet;
        thisJet.set_pt(patJet.p4().pt());
        thisJet.set_eta(patJet.p4().eta());
        thisJet.set_phi(patJet.p4().phi());
        thisJet.set_energy(patJet.p4().E());
	thisJet.set_partonFlavour(patJet.partonFlavour());
	thisJet.set_hadronFlavour(patJet.hadronFlavour());
	bool add_genparts=false;
	if(genxconeJets[j].size()<doGenxconeJetConstituentsNjets || thisJet.pt()>doGenxconeJetConstituentsMinJetPt) add_genparts=true;
        for (const auto & subItr : patJet.subjets()) {
          GenJet subjet;
          subjet.set_pt(subItr->p4().pt());
          subjet.set_eta(subItr->p4().eta());
          subjet.set_phi(subItr->p4().phi());
          subjet.set_energy(subItr->p4().E());
          subjet.set_partonFlavour(subItr->partonFlavour());
          subjet.set_hadronFlavour(subItr->hadronFlavour());
	  fill_geninfo_patjet(subItr,subjet,add_genparts);
          thisJet.add_subjet(subjet);
        }
	fill_geninfo_patjet(patJet,thisJet,add_genparts);
        genxconeJets[j].push_back(thisJet);
      }
    }
   }

  //XCONE GEN dijet
  if (doGenXCone_dijet) {
    for (size_t j=0; j < genxcone_tokens_dijet.size(); ++j){
      genxconeJets_dijet[j].clear();
      edm::Handle<pat::JetCollection> genxcone_patjets_dijet;
      iEvent.getByToken(genxcone_tokens_dijet[j], genxcone_patjets_dijet);

      // Convert from pat::Jet to GenTopJet with subjets
      for (const auto & patJet : *genxcone_patjets_dijet) {
        GenTopJet thisJet;
        thisJet.set_pt(patJet.p4().pt());
        thisJet.set_eta(patJet.p4().eta());
        thisJet.set_phi(patJet.p4().phi());
        thisJet.set_energy(patJet.p4().E());
	thisJet.set_partonFlavour(patJet.partonFlavour());
	thisJet.set_hadronFlavour(patJet.hadronFlavour());
	bool add_genparts=false;
	if(genxconeJets_dijet[j].size()<doGenxconeDijetJetConstituentsNjets || thisJet.pt()>doGenxconeDijetJetConstituentsMinJetPt) add_genparts=true;
        for (const auto & subItr : patJet.subjets()) {
          GenJet subjet;
          subjet.set_pt(subItr->p4().pt());
          subjet.set_eta(subItr->p4().eta());
          subjet.set_phi(subItr->p4().phi());
          subjet.set_energy(subItr->p4().E());
          subjet.set_partonFlavour(subItr->partonFlavour());
          subjet.set_hadronFlavour(subItr->hadronFlavour());
	  fill_geninfo_patjet(subItr,subjet,add_genparts);
          thisJet.add_subjet(subjet);
        }
	fill_geninfo_patjet(patJet,thisJet,add_genparts);
        genxconeJets_dijet[j].push_back(thisJet);
      }
    }
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
   firstEvent = false;
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


void NtupleWriter::fill_geninfo_recojet(const reco::Jet& reco_genjet, GenJet& genjet, bool& add_genparts)
{
  pat::Jet pat_jet(reco_genjet);
  fill_geninfo_patjet(pat_jet, genjet, add_genparts);
}


void NtupleWriter::fill_geninfo_patjet(const pat::Jet& pat_genjet, GenJet& genjet, bool& add_genparts)
{

  // recalculate the jet charge.
  int jet_charge = 0;
  double chf = 0; double cef = 0;
  double nhf = 0; double nef = 0;
  double muf = 0;

  int chMult = 0;
  int nMult = 0;
  int muMult = 0;
  int elMult = 0;
  int phMult = 0;
  // loop over all jet constituents, fill PF fractions & multiplicity info
  try {
    for(unsigned int l = 0; l<pat_genjet.numberOfSourceCandidatePtrs(); ++l){
      const reco::Candidate* constituent =  pat_genjet.daughter(l);
      jet_charge += constituent->charge();
      if(add_genparts){
        size_t genparticles_index = uhh2::add_genpart(*constituent, *event->genparticles);
        genjet.add_genparticles_index(genparticles_index);
      }

      if(abs(constituent->pdgId())==11) {
        cef += constituent->energy();
        elMult++;
        chMult++;
      } else{
        if(abs(constituent->pdgId())==22) {
          nef += constituent->energy();
          phMult++;
          nMult++;
        } else{
          if(abs(constituent->pdgId())==13) {
            muf += constituent->energy();
            muMult++;
            chMult++;
          } else{
           if(abs(constituent->charge())>0.1) {
             chf += constituent->energy();
             chMult++;
           } else {
             nhf += constituent->energy();
             nMult++;
           }
          }
        }
      }
    }
  } catch ( exception & e) {
    if (firstEvent) {
      e.what();
      cout << "Cannot get pat_genjet daughters - collection probably missing from input file." << endl;
      cout << "All energy fractions & multiplicities will be 0" << endl;
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
 // cout<<"N constituens ="<<pat_genjet.numberOfSourceCandidatePtrs()<<" with charge = "<<jet_charge<<endl;
}


//define this as a plug-in
DEFINE_FWK_MODULE(NtupleWriter);
