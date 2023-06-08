#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Muon.h"

using namespace std;

class DumpMiniAOD: public edm::one::EDAnalyzer<> {
   public:
      explicit DumpMiniAOD(const edm::ParameterSet& cfg){
      }

   private:
       // to get available discriminators
      void dump_jets(const edm::Event & event){
          edm::Handle<pat::JetCollection> h_jets;
          event.getByLabel("slimmedJets", h_jets);
          for(const auto & j : *h_jets){
              for(const auto & nv : j.getPairDiscri()){
                  cout << nv.first << "=" << nv.second << endl;
              }
          }
      }
      
      // check whether pat_ele.pfIsolationVariables are the same as the variables saved directly.
      void dump_electrons(const edm::Event & event){
          edm::Handle<pat::ElectronCollection> h_electrons;
          event.getByLabel("slimmedElectrons", h_electrons);
          for(const auto & ele : *h_electrons){
              cout << "electron with pt = " << ele.pt() << ": " << endl;
              cout << " ch=" << ele.pfIsolationVariables().sumChargedHadronPt << "; direct=" << ele.chargedHadronIso() << endl;
              cout << " nh=" << ele.pfIsolationVariables().sumNeutralHadronEt << "; direct=" << ele.neutralHadronIso() << endl;
              cout << " ph=" << ele.pfIsolationVariables().sumPhotonEt << "; direct=" << ele.photonIso() << endl;
              cout << " pu=" << ele.pfIsolationVariables().sumPUPt << "; direct=" << ele.puChargedHadronIso() << endl;
          }
      }
      
      void dump_muons(const edm::Event & event){
          edm::Handle<std::vector<reco::Vertex>> pv_handle;
          event.getByLabel("offlineSlimmedPrimaryVertices", pv_handle);
          if(pv_handle->empty()){
             cout << "WARNING: no PVs found, not writing muons!" << endl;
             return;
          }
          const auto & PV = pv_handle->front();
          
          edm::Handle<std::vector<pat::Muon>> mu_handle;
          event.getByLabel("slimmedMuons", mu_handle);
          assert(mu_handle.isValid());
          
          //cout << endl;
          for (const pat::Muon & pat_mu : *mu_handle) {
            auto dz = pat_mu.muonBestTrack()->dz(PV.position());
            bool tight = pat_mu.isTightMuon(PV);
            cout << "Muon pt=" << pat_mu.pt() << "; eta=" << pat_mu.eta() << "; dz=" << dz << "; tight=" << tight << endl;
          }
      }
       
       
      virtual void analyze(const edm::Event& event, const edm::EventSetup&) override{
          //dump_electrons(event);
          dump_muons(event);
      }
};

DEFINE_FWK_MODULE(DumpMiniAOD);
