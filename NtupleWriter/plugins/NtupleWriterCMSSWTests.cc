#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"

class NtupleWriterTestGenJets : public edm::EDAnalyzer {
   public:
      explicit NtupleWriterTestGenJets(const edm::ParameterSet&){}

   private:
      virtual void analyze(const edm::Event& event, const edm::EventSetup&) override{
          edm::Handle<reco::GenJetCollection> h_genjets;
          event.getByLabel("slimmedGenJets", h_genjets);
          for(const auto & gj : *h_genjets){
              //std::cout << "Genjet p4 = " << gj.p4() << std::endl;
              reco::Candidate::LorentzVector p4_constituent_sum;
              for(const auto & constituent : gj){
                  //p4_constituent_sum += constituent.p4();
                  //std::cout << "   constituent: " << constituent.p4() << std::endl;
              }
              //std::cout << " constituent sum p4 = " << p4_constituent_sum << std::endl;
          }
      }
};

DEFINE_FWK_MODULE(NtupleWriterTestGenJets);
