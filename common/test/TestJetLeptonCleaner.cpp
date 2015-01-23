#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/common/include/JetCorrections.h"
#include "UHH2/common/include/PrintingModules.h"
#include "UHH2/common/include/CleaningModules.h"
#include "UHH2/common/include/MuonIds.h"
#include "UHH2/common/include/ElectronIds.h"

#include <iostream>

using namespace uhh2;
using namespace std;

class TestJetLeptonCleaner: public uhh2::AnalysisModule {
public:
    explicit TestJetLeptonCleaner(Context &) {
        jet_lepton_cleaner.reset(new JetLeptonCleaner(JERFiles::PHYS14_L123_MC));
        jets_before.reset(new JetPrinter("jets before", 30.0));
        jets_after.reset(new JetPrinter("jets after", 30.0));
        muons_before.reset(new MuonPrinter());
        electrons_before.reset(new ElectronPrinter("before"));
        electrons_after.reset(new ElectronPrinter("after"));
        muon_cleaner.reset(new MuonCleaner(AndId<Muon>(PtEtaCut(30., 2.4), MuonIDTight())));
        ele_cleaner.reset(new ElectronCleaner(AndId<Electron>(PtEtaCut(30.0, 2.4), &ElectronID_CSA14_50ns_medium)));
    }
    
    virtual bool process(Event & e) override{
        
        if(e.muons){
            // assume we test muons only (!)
            muon_cleaner->process(e);
            if(e.muons->empty()) return true;
            e.electrons->clear();
        }
        else{
            // assume we test electrons only:
            //electrons_before->process(e);
            ele_cleaner->process(e);
            electrons_after->process(e);
        }
        
        
        
        
        /*if(e.electrons->empty()) return true;
        e.muons->clear();
                          
        cout << endl;
        
        //muons_before->process(e);
        electrons_before->process(e);
        
        jets_before->process(e);
        jet_lepton_cleaner->process(e);
        jets_after->process(e);*/
        return true;
    }
    
private:
    std::unique_ptr<AnalysisModule> ele_cleaner, muon_cleaner, electrons_before, electrons_after, muons_before, jets_before, jets_after;
    std::unique_ptr<JetLeptonCleaner> jet_lepton_cleaner;
};


UHH2_REGISTER_ANALYSIS_MODULE(TestJetLeptonCleaner)

