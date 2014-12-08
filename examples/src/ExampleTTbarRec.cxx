#include <iostream>
#include <memory>

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/Selection.h"
#include "UHH2/common/include/CleaningModules.h"
#include "UHH2/common/include/ObjectIdUtils.h"
#include "UHH2/common/include/JetIds.h"
#include "UHH2/common/include/MuonIds.h"
#include "UHH2/common/include/ElectronIds.h"
#include "UHH2/common/include/NSelections.h"
#include "UHH2/common/include/TTbarReconstruction.h"

using namespace std;
using namespace uhh2;

/** 
 * Example Module that performs a selection of two jets and one muon and fills the TTbar reconstruction hypotheses for selected events into the ouptut tree
 */
class ExampleTTbarRec: public AnalysisModule {
public:
    
    explicit ExampleTTbarRec(Context & ctx);
    virtual bool process(Event & event);

private:
  JetId jet_kinematic;
    TopJetId topjet_kinematic;
    MuonId muid;

    std::vector<std::unique_ptr<AnalysisModule>> cleanermodules;
    std::vector<std::unique_ptr<AnalysisModule>> recomodules;
    AndSelection selection;
};


ExampleTTbarRec::ExampleTTbarRec(Context & ctx): selection(ctx, "selection") {
    jet_kinematic = PtEtaCut(50.0, 2.4);
    topjet_kinematic = PtEtaCut(150.0,2.4);
    muid = AndId<Muon>(MuonIDTight(), PtEtaCut(20.0, 2.4));
    
    // clean the objects:
    cleanermodules.emplace_back(new JetCleaner(jet_kinematic));
    cleanermodules.emplace_back(new MuonCleaner(muid));
    
    // make the selection, step-by-step. Note that in most cases, no explicit
    // object id is passed, as the cleaners have removed the objects not passing the id already.
    selection.add<NMuonSelection>("nm >= 1", 1);
    selection.add<NJetSelection>("nj >= 2", 2);
    selection.add<NTopJetSelection>("nj >= 1", 1);

    //make reconstruction hyptotheses
    recomodules.emplace_back(new PrimaryLepton(ctx));
    recomodules.emplace_back(new HighMassTTbarReconstruction(ctx,NeutrinoReconstruction));
    recomodules.emplace_back(new CAJetsReconstruction(ctx,NeutrinoFitPolar));
    
}


bool ExampleTTbarRec::process(Event & event) {
    for(auto & m : cleanermodules){
        m->process(event);
    }
 
    if(selection.passes(event)){
      for(auto & m : recomodules){
        m->process(event);
      }
      return true;
    }
    return false;
}

// as we want to run the ExampleCycleNew directly with AnalysisModuleRunner,
// make sure the ExampleModule is found by class name. This is ensured by this macro:
UHH2_REGISTER_ANALYSIS_MODULE(ExampleTTbarRec)
