#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/common/include/PrintingModules.h"
#include "UHH2/common/include/CleaningModules.h"
#include "UHH2/common/include/ElectronIds.h"
#include "UHH2/common/include/ElectronHists.h"

#include <iostream>

using namespace uhh2;
using namespace std;

class TestElectronId: public uhh2::AnalysisModule {
public:
    explicit TestElectronId(Context & ctx) {
        ele_printer.reset(new ElectronPrinter());
        ele_hists.reset(new ElectronHists(ctx, "ele"));
        ele_cleaner.reset(new ElectronCleaner(&ElectronID_PHYS14_25ns_medium));
    }
    
    virtual bool process(Event & e) override{
        assert(e.electrons);
        
        for(const auto & ele : *e.electrons){
            bool cmssw_medium = ele.get_tag(Electron::eid_PHYS14_20x25_medium);
            bool sframe_medium = ElectronID_PHYS14_25ns_medium(ele, e);
            if(cmssw_medium ^ sframe_medium){
                cout << "Found weird electron: cmssw? " << cmssw_medium << "; sframe? " << sframe_medium
                     << endl << " electron variables: ";
                ElectronPrinter::print(cout, ele, e);
                cout << endl;
            }
            if(sframe_medium){
                ++n_passing;
            }
        }
        ele_cleaner->process(e);
        ele_hists->fill(e);
        return true;
    }
    
    virtual ~TestElectronId(){
        cout << "~TestElectronId: Total electrons passing sframe medium id:" << n_passing<< endl;
    }
    
private:
    int n_passing = 0;
    std::unique_ptr<AnalysisModule> ele_cleaner, ele_printer;
    std::unique_ptr<Hists> ele_hists;
};


UHH2_REGISTER_ANALYSIS_MODULE(TestElectronId)

// numbers from CMSSW:
// N[0] = 231
// N[1] = 204
// N[2] = 187
// N[3] = 164
