#include "FWCore/Framework/interface/one/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

using namespace std;

class FilterStrangeGenParticles: public edm::one::EDFilter<edm::one::SharedResources> {
   public:
      explicit FilterStrangeGenParticles(const edm::ParameterSet& cfg) {
          src = cfg.getParameter<edm::InputTag>("src");
          n_top_expected = cfg.getUntrackedParameter<int>("n_top_expected", -1);
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
            auto tprime_d0 = gp.daughter(0);
            auto tprime_d1 = gp.daughter(1);
            
            int pdgid_lower = abs(tprime_d0->pdgId());
            int pdgid_higher = abs(tprime_d1->pdgId());
            if(pdgid_lower > pdgid_higher){
                swap(pdgid_lower, pdgid_higher);
                swap(tprime_d0, tprime_d1);
            }
            if(pdgid_lower == 6 && pdgid_higher == 25){// OK: tH
                continue;
            }
            //TODO: check all known decay channels!
            else{
                cout << "T' does not decay to top + Higgs" << endl;
                return true;
            }
        }
        else if(abs(gp.pdgId()) == 6000007){
            // single B'
            if(gp.numberOfDaughters() != 2){
                cout << "B' has " << gp.numberOfDaughters() << " daughters." << endl;
                return true;
            }
            auto bprime_d0 = gp.daughter(0);
            auto bprime_d1 = gp.daughter(1);
            
            int pdgid_lower = abs(bprime_d0->pdgId());
            int pdgid_higher = abs(bprime_d1->pdgId());
            
            if(pdgid_lower > pdgid_higher){
                swap(pdgid_lower, pdgid_higher);
                swap(bprime_d0, bprime_d1);
            }
            
            if(pdgid_lower != 6 || pdgid_higher != 24){
                cout << "B' decays: " << pdgid_lower << ", " << pdgid_higher << endl;
                return true;
            }
        }
        else if(abs(gp.pdgId()) == 6){
            ++n_top;
            if(gp.numberOfDaughters() < 2 || gp.numberOfDaughters() > 3){
                cout << "t has " << gp.numberOfDaughters() << " daughters." << endl;
                return true;
            }
            if(gp.numberOfDaughters() == 2){
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
            else{
                // intermediate W missing?
                auto topd0 = gp.daughter(0);
                auto topd1 = gp.daughter(1);
                auto topd2 = gp.daughter(2);
                std::set<int> pdgids;
                pdgids.insert(std::abs(topd0->pdgId()));
                pdgids.insert(std::abs(topd1->pdgId()));
                pdgids.insert(std::abs(topd2->pdgId()));
                
                if(pdgids.find(5)==pdgids.end()){
                    cout << "top has 3 daughters, but no b!" << endl;
                }
                // TODO: could check more, e.g. that the two others are consistent with coming from a W.
                //return true;
            }
        }
        else if(abs(gp.pdgId()) == 25){ // Higgs
            if(gp.numberOfDaughters() != 2){
                cout << "Higgs has != 2 daughters!" << endl;
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

