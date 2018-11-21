#include <iostream>
#include <memory>

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/Hists.h"

#include "TH1F.h"

using namespace std;
using namespace uhh2;

/** \brief Example of how to use & check Electron IDs via tags
 * 
 * This is the preferred, POG-approved method to check Electron ID, since it uses
 * stored values made by the official POG recipes.
 * There are manual versions in ElectronIds.cxx, but these may not be up to date.
 *
 * This brief example shows how to use a tag, 
 * and produces a histogram to show how many electrons passed each ID.
 *
 * For Muons, the approach is similar, but they are called "Selectors"
 */

namespace uhh2examples {

class ExampleElectronIDHists: public uhh2::Hists {
public:
    ExampleElectronIDHists(uhh2::Context & ctx, const std::string & dirname, const vector<string> & electronIDs_);

    virtual void fill(const uhh2::Event & ev) override;
    virtual ~ExampleElectronIDHists();
private:
    TH1I * hElectronIDs;
    vector<string> electronIDs;
};

ExampleElectronIDHists::ExampleElectronIDHists(Context & ctx, const string & dirname, const vector<string> & electronIDs_): 
Hists(ctx, dirname),
electronIDs(electronIDs_)
{
    hElectronIDs = book<TH1I>("ele_ids", ";Electron ID;N", electronIDs.size(), 0, electronIDs.size());
    // set custom x axis labels
    for (uint i=1;i<=electronIDs.size();i++) {
        hElectronIDs->GetXaxis()->SetBinLabel(i, electronIDs.at(i-1).c_str());
    }
}


void ExampleElectronIDHists::fill(const Event & event){
    // Loop over all IDs for all electrons, and store which pass
    for (auto & eleItr : *event.electrons) {
        for (uint i=0; i < electronIDs.size(); i++) {
            // Here we want the ID status from the ID string name
            // Use tagname2tag to convert string to a tag, then use get_tag
            if (eleItr.get_tag(eleItr.tagname2tag(electronIDs.at(i)))) {
                hElectronIDs->Fill(i, event.weight);
            }
        }
    }
}

ExampleElectronIDHists::~ExampleElectronIDHists(){}


class ExampleModuleElectronID: public AnalysisModule {
public:
    
    explicit ExampleModuleElectronID(Context & ctx);
    virtual bool process(Event & event) override;
private:
    vector<string> electronIDs =  {
        "cutBasedElectronID_Fall17_94X_V2_veto",
        "cutBasedElectronID_Fall17_94X_V2_loose",
        "cutBasedElectronID_Fall17_94X_V2_medium",
        "cutBasedElectronID_Fall17_94X_V2_tight",
        "heepElectronID_HEEPV70",
        "mvaEleID_Fall17_noIso_V2_wp90",
        "mvaEleID_Fall17_noIso_V2_wp80",
        "mvaEleID_Fall17_noIso_V2_wpLoose",
        "mvaEleID_Fall17_iso_V2_wp90",
        "mvaEleID_Fall17_iso_V2_wp80",
        "mvaEleID_Fall17_iso_V2_wpLoose"
    };
    unique_ptr<ExampleElectronIDHists> hists;
};


ExampleModuleElectronID::ExampleModuleElectronID(Context & ctx)
{
    cout << "Hello World from ExampleModuleElectronID!" << endl;
    hists.reset(new ExampleElectronIDHists(ctx, "electronID", electronIDs));
}


bool ExampleModuleElectronID::process(Event & event) {

    for (const auto & eleItr : *event.electrons) {
        // We can use the enum directly, this is the easiest way
        cout << "cutBasedElectronID_Fall17_94X_V2_veto: " << eleItr.get_tag(Electron::cutBasedElectronID_Fall17_94X_V2_veto) << endl;
    }
 
    hists->fill(event);

    return true;
}

// as we want to run the ExampleCycleNew directly with AnalysisModuleRunner,
// make sure the ExampleModuleElectronID is found by class name. This is ensured by this macro:
UHH2_REGISTER_ANALYSIS_MODULE(ExampleModuleElectronID)

}
