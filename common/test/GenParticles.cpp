#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Utils.h"
#include "UHH2/common/include/PrintingModules.h"


using namespace uhh2;
using namespace std;

class TestGenParticles: public uhh2::AnalysisModule {
public:
    explicit TestGenParticles(Context & ctx) {
        printer = make_unique<GenParticlesPrinter>(ctx);
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

