#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/core/test/common.h"

#include <iostream>

using namespace uhh2;
using namespace std;

class CoreTestA: public AnalysisModule {
public:
    CoreTestA(Context & ctx){
        ti_init = false;
    }
    
    virtual bool process(Event & e) override {
        if(!ti_init){
            ti1 = e.get_trigger_index("trig1");
            ti2 = e.get_trigger_index("trig2");
        }
        
        bool t1_actual = e.passes_trigger(ti1);
        bool t2_actual = e.passes_trigger(ti2);
        
        //cout << e.run << ":" << e.event << "; l=" << e.luminosityBlock << "; trig1=" << t1_actual << "; trig2=" << t2_actual << endl;
        
        int lumi_expected = calc_lumi(e.run, e.event);
        assert(e.luminosityBlock == lumi_expected);
        
        bool t1_expected = calc_trig1(e.run, e.event);
        bool t2_expected = calc_trig2(e.run, e.event);
        
        assert(t1_expected == t1_actual);
        assert(t2_expected == t2_actual);
        return true;
    }
    
private:
    bool ti_init;
    Event::TriggerIndex ti1, ti2;
    
};


UHH2_REGISTER_ANALYSIS_MODULE(CoreTestA)



class NoopModule: public AnalysisModule {
public:
    NoopModule(Context & ctx){
    }
    
    virtual bool process(Event & e) override {
        return true;
    }
};

UHH2_REGISTER_ANALYSIS_MODULE(NoopModule)
