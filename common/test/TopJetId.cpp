#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Utils.h"
#include "UHH2/common/include/CleaningModules.h"
#include "UHH2/common/include/TopJetIds.h"
#include "UHH2/common/include/TTbarGen.h"

#include "TH1D.h"


using namespace uhh2;
using namespace std;

class TestTopJetId: public uhh2::AnalysisModule {
public:
    explicit TestTopJetId(Context & ctx) {
        cleaner = make_unique<TopJetCleaner>(CMSTopTag());
        all_tops = new TH1D("all_tops", "all_tops", 100, 0, 1000.);
        tagged_tops = new TH1D("tagged_tops", "tagged_tops", 100, 0, 1000.);
        ctx.put("", all_tops);
        ctx.put("", tagged_tops);
    }
    
    virtual bool process(Event & e) override;
    
private:
    std::unique_ptr<AnalysisModule> cleaner;
    TH1D * all_tops;
    TH1D * tagged_tops;
};

bool TestTopJetId::process(Event & e){
    assert(e.topjets);
    assert(e.genparticles);
    TTbarGen ttgen(*e.genparticles, false);
    if(ttgen.DecayChannel() != TTbarGen::e_had){
        return true;
    }
    cleaner->process(e);
    all_tops->Fill(ttgen.Top().pt());
    all_tops->Fill(ttgen.Antitop().pt());
    // try to match remaining tops to tagged tops:
    for(const auto & topjet : *e.topjets){
        if(deltaR(topjet, ttgen.Top()) < 0.8){
            tagged_tops->Fill(ttgen.Top().pt());
        }
        else if(deltaR(topjet, ttgen.Antitop()) < 0.8){
            tagged_tops->Fill(ttgen.Antitop().pt());
        }
    }
    cout << "n_topjets = " << e.topjets->size() << endl;
    return true;
}

UHH2_REGISTER_ANALYSIS_MODULE(TestTopJetId)

