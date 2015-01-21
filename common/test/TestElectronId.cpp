#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/common/include/PrintingModules.h"
#include "UHH2/common/include/CleaningModules.h"
#include "UHH2/common/include/ElectronIds.h"
#include "UHH2/common/include/ElectronHists.h"

#include <iostream>

using namespace uhh2;
using namespace std;

namespace {
    
union fi_union {
    uint32_t i;
    float f;
};
    
// bit = 0 means least significan
int extract_bit(float value, int bit){
    fi_union u;
    u.f = value;
    bool isset = (u.i & (uint32_t(1) << bit));
    return isset ? 1 : 0;
}

}



class TestElectronId: public uhh2::AnalysisModule {
public:
    explicit TestElectronId(Context & ctx): n_passing(4, 0){
        ele_printer.reset(new ElectronPrinter());
        ele_hists.reset(new ElectronHists(ctx, "ele"));
        ele_cleaner.reset(new ElectronCleaner(&ElectronID_PHYS14_25ns_medium));
    }
    
    virtual bool process(Event & e) override{
        assert(e.electrons);
        
        for(const auto & ele : *e.electrons){
            float id = ele.mvaTrigV0();
            assert(std::isnan(id));
            bool cmssw_medium = extract_bit(id, 2);
            bool sframe_medium = ElectronID_PHYS14_25ns_medium(ele, e);
            if(cmssw_medium ^ sframe_medium){
                cout << "Found weird electron: cmssw? " << cmssw_medium << "; sframe? " << sframe_medium
                     << endl << " electron variables: ";
                ElectronPrinter::print(cout, ele, e);
                cout << endl;
            }
            for(int i_id = 0; i_id < 4; ++i_id){
                if(extract_bit(id, i_id)){
                    ++n_passing[i_id];
                }
            }
        }
        ele_cleaner->process(e);
        ele_hists->fill(e);
        return true;
    }
    
    virtual ~TestElectronId(){
        cout << "~TestElectronId: Total electrons:" << endl;
        for(int i=0; i<4; ++i){
            cout << " N[" << i << "] = " << n_passing[i] << endl;
        }
    }
    
private:
    std::vector<int> n_passing;
    std::unique_ptr<AnalysisModule> ele_cleaner, ele_printer;
    std::unique_ptr<Hists> ele_hists;
};


UHH2_REGISTER_ANALYSIS_MODULE(TestElectronId)

// numbers from CMSSW:
// N[0] = 231
// N[1] = 204
// N[2] = 187
// N[3] = 164
