#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "DataFormats/PatCandidates/interface/Jet.h"

using namespace std;

class DumpMiniAOD: public edm::EDAnalyzer {
   public:
      explicit DumpMiniAOD(const edm::ParameterSet& cfg){
      }

   private:
      virtual void analyze(const edm::Event& event, const edm::EventSetup&) override{
          edm::Handle<pat::JetCollection> h_jets;
          event.getByLabel("slimmedJets", h_jets);
          for(const auto & j : *h_jets){
              for(const auto & nv : j.getPairDiscri()){
                  cout << nv.first << "=" << nv.second << endl;
              }
          }
      }
};

DEFINE_FWK_MODULE(DumpMiniAOD);
