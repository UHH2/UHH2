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

using namespace std;
using namespace uhh2;

namespace uhh2examples {

/** \brief Another analysis example of an AnalysisModule
 * 
 * In contrast to ExampleModule, this class uses more code from 'common' to achieve
 * similar effects.
 * 
 * It implements a preselection of
 *   - at least one lepton (electron or muon) with pt > 20, |eta| < 2.4 (electrons: |eta| < 2.5), using the tight id for
 *     muons and the medium cut-based id for electrons.
 *   - at least two jets with pt > 30, eta < 2.4
 *   - at least one b-tagged jet with above kinematics with loose csv b-tag
 * 
 * In the output, only leptons passing the id and jets with pt > 30 GeV and eta < 2.4 are kept.
 */
class ExampleModule2: public AnalysisModule {
public:
    
    explicit ExampleModule2(Context & ctx);
    virtual bool process(Event & event);

private:
    JetId jet_kinematic, btag;
    MuonId muid;
    ElectronId eleid;
    
    std::vector<std::unique_ptr<AnalysisModule>> modules;
    
    AndSelection ele_selection, mu_selection;
};


ExampleModule2::ExampleModule2(Context & ctx): ele_selection(ctx, "ele"), mu_selection(ctx, "mu") {
    jet_kinematic = PtEtaCut(30.0, 2.4);
    btag = CSVBTag(CSVBTag::WP_LOOSE);
    muid = AndId<Muon>(MuonIDTight(), PtEtaCut(20.0, 2.4));
    eleid = AndId<Electron>(ElectronID_PHYS14_25ns_medium, PtEtaCut(20.0, 2.5));
    
    // clean the objects:
    modules.emplace_back(new JetCleaner(jet_kinematic));
    modules.emplace_back(new MuonCleaner(muid));
    modules.emplace_back(new ElectronCleaner(eleid));
    
    // make the selection, step-by-step. Note that in most cases, no explicit
    // object id is passed, as the cleaners have removed the objects not passing the id already.
    ele_selection.add<NElectronSelection>("ne >= 1", 1);
    ele_selection.add<NJetSelection>("nj >= 2", 2);
    ele_selection.add<NJetSelection>("nb >= 1", 1, -1, btag);
    
    mu_selection.add<NMuonSelection>("nm >= 1", 1);
    mu_selection.add<NJetSelection>("nj >= 2", 2);
    mu_selection.add<NJetSelection>("nb >= 1", 1, -1, btag);
}


bool ExampleModule2::process(Event & event) {
    for(auto & m : modules){
        m->process(event);
    }
    
    // note that always both selections are tested, even if we could
    // short-circuit after mu_selection is passed. This is done
    // to ensure that both cutflow histograms are filled.
    bool keep_mu = mu_selection.passes(event);
    bool keep_ele = ele_selection.passes(event);
    return keep_mu || keep_ele;
}

// as we want to run the ExampleCycleNew directly with AnalysisModuleRunner,
// make sure the ExampleModule is found by class name. This is ensured by this macro:
UHH2_REGISTER_ANALYSIS_MODULE(ExampleModule2)

}
