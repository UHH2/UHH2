#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Electron.h"

using namespace std;

class DumpMiniAOD: public edm::EDAnalyzer {
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
       
       
      virtual void analyze(const edm::Event& event, const edm::EventSetup&) override{
          dump_electrons(event);
      }
};

DEFINE_FWK_MODULE(DumpMiniAOD);
