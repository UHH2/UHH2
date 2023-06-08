#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"


#include "TFile.h"
#include "TH1D.h"

using namespace std;

class NtupleWriterTestGenJets : public edm::one::EDAnalyzer<> {
   public:
      explicit NtupleWriterTestGenJets(const edm::ParameterSet& cfg): n_gj_total(0), n_gj_withnull(0){
          genjetsrc = cfg.getParameter<edm::InputTag>("genjetsrc");
          outfile = new TFile("genjets.root", "recreate");
          if(!genjetsrc.label().empty()){
            h_gj_eta = new TH1D("gj_eta", "gj_eta", 120, -6, 6);
            h_gj_pt = new TH1D("gj_pt", "gj_pt", 100, 0, 100);
            h_gj_eta_incomplete = new TH1D("gj_eta_incomplete", "gj_eta_incomplete", 120, -6, 6);
            h_gj_pt_incomplete = new TH1D("gj_pt_incomplete", "gj_pt_incomplete", 100, 0, 100);
            for(auto h : {h_gj_eta, h_gj_pt, h_gj_eta_incomplete, h_gj_pt_incomplete}){
              h->SetDirectory(outfile);
            }
          }
      }
      
      virtual ~NtupleWriterTestGenJets(){
          outfile->cd();
          outfile->Write();
          delete outfile;
      }

   private:
      void analyze_genjets(const edm::Event & event){
          edm::Handle<reco::GenJetCollection> h_genjets;
          event.getByLabel(genjetsrc, h_genjets);
          for(const auto & gj : *h_genjets){
              reco::Candidate::LorentzVector p4_constituent_sum;
              size_t nd = gj.numberOfSourceCandidatePtrs();
              cout << nd << endl;
              size_t n_null = 0;
              for(size_t i=0; i<nd; ++i){
                  auto constituent = gj.sourceCandidatePtr(i);
                  if(constituent.isNull()){
                      ++n_null;
                      continue;
                  }
                  p4_constituent_sum += constituent->p4();
                  if(fabs(constituent->y()) > 6.0){
                      cout << "constituent with y > 6!" << endl;
                  }
              }
              ++n_gj_total;
              if(n_null > 0){
                  ++n_gj_withnull;
                  h_gj_pt_incomplete->Fill(gj.pt());
                  h_gj_eta_incomplete->Fill(gj.eta());
              }
              h_gj_pt->Fill(gj.pt());
              h_gj_eta->Fill(gj.eta());
          }
      }
      
      virtual void analyze(const edm::Event& event, const edm::EventSetup&) override{
          analyze_genjets(event);
      }
      
      edm::InputTag genjetsrc;
      
      
      size_t n_gj_total, n_gj_withnull;
      
      TFile * outfile;
      // incomplete genjets:
      TH1D * h_gj_eta;
      TH1D * h_gj_pt;
      TH1D * h_gj_eta_incomplete;
      TH1D * h_gj_pt_incomplete;
};

DEFINE_FWK_MODULE(NtupleWriterTestGenJets);

// problematic event e.g. event 5593922 from QCD flat sample, lumi 56501
// in miniaod file 
// /store/mc/Phys14DR/QCD_Pt-15to3000_Tune4C_Flat_13TeV_pythia8/MINIAODSIM/PU20bx25_trkalmb_PHYS14_25_V1-v1/00000/1020E374-B26B-E411-8F91-E0CB4E29C513.root
// and in aod file
// /pnfs/desy.de/cms/tier2/store/mc/Phys14DR/QCD_Pt-15to3000_Tune4C_Flat_13TeV_pythia8/AODSIM/PU20bx25_trkalmb_PHYS14_25_V1-v1/00000/E6056E6A-AD6B-E411-889A-002590747E1A.root
// 
