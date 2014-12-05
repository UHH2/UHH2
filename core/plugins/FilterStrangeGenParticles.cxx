#include "FWCore/Framework/interface/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

using namespace std;

class FilterStrangeGenParticles: public edm::EDFilter {
   public:
      explicit FilterStrangeGenParticles(const edm::ParameterSet& cfg) {
          src = cfg.getParameter<edm::InputTag>("src");
          n_top_expected = cfg.getParameter<int>("n_top_expected");
      }

   private:
      virtual bool filter(edm::Event& event, const edm::EventSetup&) override;
      
      edm::InputTag src;
      int n_top_expected;
};


DEFINE_FWK_MODULE(FilterStrangeGenParticles);



bool FilterStrangeGenParticles::filter(edm::Event & event, const edm::EventSetup&){
    edm::Handle<reco::GenParticleCollection> h_genparticles;
    event.getByLabel(src, h_genparticles);
    int n_top = 0;
    for(const auto & gp : *h_genparticles){
        if(abs(gp.pdgId()) == 6000006){
            if(gp.numberOfDaughters() != 2){
                cout << "T' has " << gp.numberOfDaughters() << " daughters." << endl;
                return true;
            }
        }
        if(abs(gp.pdgId()) == 6){
            ++n_top;
            if(gp.numberOfDaughters() != 2){
                cout << "t has " << gp.numberOfDaughters() << " daughters." << endl;
                return true;
            }
            auto topd0 = gp.daughter(0);
            auto topd1 = gp.daughter(1);
            int pdgid_lower = abs(topd0->pdgId());
            int pdgid_higher = abs(topd1->pdgId());
            if(pdgid_lower > pdgid_higher){
                swap(pdgid_lower, pdgid_higher);
                swap(topd0, topd1);
            }
            if(pdgid_lower != 5){
                cout << "t does not have b as daughter" << endl;
                return true;
            }
            if(pdgid_higher != 24){
                cout << "t does not have W as daughter" << endl;
                return true;
            }
            if(topd1->numberOfDaughters() != 2){
                cout << "W from top has != 2 daughters" << endl;
                return true;
            }
        }
    }
    if(n_top_expected >= 0){
        if(n_top != n_top_expected){
            cout << "Found " << n_top << " top quarks in the event, but expected " << n_top_expected << endl;
            return true;
        }
    }
    return false;
}

