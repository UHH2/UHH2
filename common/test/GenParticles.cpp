#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Utils.h"
#include "UHH2/common/include/PrintingModules.h"


using namespace uhh2;
using namespace std;

class TestGenParticles: public uhh2::AnalysisModule {
public:
    explicit TestGenParticles(Context & ctx) {
      printer = std::make_unique<GenParticlesPrinter>(ctx);
    }
    
    virtual bool process(Event & e) override;
    
    virtual ~TestGenParticles(){
        cout << "  n_z/h = " << n_z_h  << " / " << n_total << endl;
    }
    
private:
    std::unique_ptr<AnalysisModule> printer;
    int n_z_h = 0;
    int n_total = 0;
};

bool TestGenParticles::process(Event & e){
    assert(e.genparticles);
    //printer->process(e);
    ++n_total;
    for(const auto & gp : *e.genparticles){
        int id = abs(gp.pdgId());
        if(id == 23 || id == 25){
            ++n_z_h;
        }
    }
    return true;
}

UHH2_REGISTER_ANALYSIS_MODULE(TestGenParticles)


// test for git issue #36
class TestGenParticleLinks: public uhh2::AnalysisModule {
public:
    explicit TestGenParticleLinks(Context & ctx){
      printer = std::make_unique<GenParticlesPrinter>(ctx);
    }
   
    virtual bool process(Event & e) override;
    
    ~TestGenParticleLinks(){
        cout << "TestGenParticles: total number of bad events concerning mother/daughter links of Tprime: " << n_bad << endl;
        cout << "TestGenParticles: total number of bad events concerning missing W daughters: " << n_bad_w << endl;
        cout << "TestGenParticles: total number of bad events concerning missing H daughters: " << n_bad_h << endl;
    }
    
private:
    std::unique_ptr<AnalysisModule> printer;
    int n_bad = 0;
    int n_bad_w = 0;
    int n_bad_h = 0;
};

bool TestGenParticleLinks::process(Event & e){
    assert(e.genparticles);
    //printer->process(e);
    for(const auto & gp : *e.genparticles){
        if(std::abs(gp.pdgId()) == 6000006){ // Tprime. Ask for mothers:
            auto mother1 = gp.mother(e.genparticles, 1);
            auto mother2 = gp.mother(e.genparticles, 2);
            if(!mother1 && !mother2){
                cout << "no mother for Tprime found!" << endl;
                printer->process(e);
                ++n_bad;
            }
            for(auto mother: {mother1, mother2}){
                if(mother){
                    // cross-check that the tprime is found as daughter:
                    auto d1 = mother->daughter(e.genparticles, 1);
                    auto d2 = mother->daughter(e.genparticles, 2);
                    if(d1 != &gp && d2 != &gp){
                        cout << "did not find Tprime as daughter of its mother!" << endl;
                        printer->process(e);
                        ++n_bad;
                    }
                }
            }
        }
        else if(std::abs(gp.pdgId()) == 24){// W
            auto d1 = gp.daughter(e.genparticles, 1);
            auto d2 = gp.daughter(e.genparticles, 2);
            if(d1==nullptr || d2==nullptr){
                cout << "Did not find daughters of W" << endl;
                printer->process(e);
                ++n_bad_w;
            }
        }
        else if(std::abs(gp.pdgId()) == 25){ // H
            auto d1 = gp.daughter(e.genparticles, 1);
            auto d2 = gp.daughter(e.genparticles, 2);
            if(d1==nullptr || d2==nullptr){
                cout << "Did not find daughters of H" << endl;
                printer->process(e);
                ++n_bad_h;
            }
        }
    }
    return true;
}


UHH2_REGISTER_ANALYSIS_MODULE(TestGenParticleLinks)

