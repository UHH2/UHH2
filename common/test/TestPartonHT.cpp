#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/common/include/PartonHT.h"

#include "TH1F.h"

#include <iostream>

using namespace uhh2;
using namespace std;

class TestPartonHT: public uhh2::AnalysisModule {
public:
    explicit TestPartonHT(Context & ctx) {
        partonht = new TH1F("partonht", "partonht", 1000, 0, 1000);
        ctx.put("", partonht);
    }
    
    virtual bool process(Event & e) override{
        assert(e.genparticles);
        auto pht = PartonHT::calculate(*e.genparticles);
        //cout << pht << endl;
        partonht->Fill(pht);
        return true;
    }
    
private:
    TH1F * partonht;
};


UHH2_REGISTER_ANALYSIS_MODULE(TestPartonHT)
