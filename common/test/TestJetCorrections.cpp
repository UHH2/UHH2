#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/common/include/PrintingModules.h"
#include "UHH2/common/include/JetCorrections.h"

using namespace uhh2;

class TestJetCorrections: public uhh2::AnalysisModule {
public:
    explicit TestJetCorrections(Context & ctx){
        modules.emplace_back(new JetPrinter("before correction", 10.0));
        modules.emplace_back(new JetCorrector(JERFiles::PHYS14_L123_MC));
        modules.emplace_back(new JetPrinter("after correction", 10.0));
    }
    
    virtual bool process(Event & e) override;
    
private:
    std::vector<std::unique_ptr<AnalysisModule>> modules;
};


bool TestJetCorrections::process(Event & e){
    for(auto & m : modules){
        m->process(e);
    }
    return true;
}

UHH2_REGISTER_ANALYSIS_MODULE(TestJetCorrections)
