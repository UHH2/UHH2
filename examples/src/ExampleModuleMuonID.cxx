#include <iostream>
#include <memory>
#include <numeric>
#include <algorithm>
#include <vector>

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/Hists.h"

#include "TH1F.h"
#include "TH2F.h"

using namespace std;
using namespace uhh2;

/** \brief Example of how to use & check Muon IDs via selectors
 *
 * This is the preferred, POG-approved method to check Muon ID, since it uses
 * stored values made by the official POG recipes.
 *
 * Note that there are a variety of selectors, some appropriate for 2016/80X,
 * and some appropriate for 2017 onwards. Whilst both might be filled, you should
 * know which one is actually sensible.
 *
 * There are manual versions in MuonIDs.cxx, but these may not be up to date.
 *
 * This brief example shows how to use a selector,
 * and produces a histogram to show how many muons passed each ID.
 *
 * For Electrons, the approach is similar, but they are called "tags"
 */

namespace uhh2examples {

class ExampleMuonIDHists: public uhh2::Hists {
public:
    ExampleMuonIDHists(uhh2::Context & ctx, const std::string & dirname);

    virtual void fill(const uhh2::Event & ev) override;
    virtual ~ExampleMuonIDHists();
private:
    TH1F * hMuonSelector;
    TH1F * hMuonSimType;
    TH1F * hMuonTrackType;
    vector<float> selectorBins;
    vector<float> simTypeBins = {-15, -13, -11, -3, -2, -1, 0, 1, 2, 3, 11, 13, 15, 999, 1100}; // last value for bin
    vector<float> trackTypeBins;
};

ExampleMuonIDHists::ExampleMuonIDHists(Context & ctx, const string & dirname):
Hists(ctx, dirname)
{
    selectorBins.resize(34); // should be 1 more than # selectors
    std::iota(selectorBins.begin(), selectorBins.end(), 0.);
    hMuonSelector = book<TH1F>("mu_selector", ";Muon Selector;N", selectorBins.size()-1, &selectorBins[0]);

    hMuonSimType = book<TH1F>("mu_simtype", ";Muon SimType;N", simTypeBins.size()-1, &simTypeBins[0]);

    trackTypeBins.resize(6); // should be 1 more than # track types
    std::iota(trackTypeBins.begin(), trackTypeBins.end(), 0.);
    hMuonTrackType = book<TH1F>("mu_tracktype", ";MuonTrackType;N", trackTypeBins.size()-1, &trackTypeBins[0]);
}


void ExampleMuonIDHists::fill(const Event & event){
    // Loop over all IDs for all muons, and store values
    for (auto & muItr : *event.muons) {
        // selectors
        for (auto selInd : selectorBins) {
            if (muItr.get_selector(static_cast<Muon::Selector>(selInd))) {
                hMuonSelector->Fill(selInd);
            }
        }
        // simTypes
        hMuonSimType->Fill(muItr.simType());

        // trackTypes
        hMuonTrackType->Fill(muItr.tunePTrackType());
    }

}

ExampleMuonIDHists::~ExampleMuonIDHists(){}


class ExampleModuleMuonID: public AnalysisModule {
public:

    explicit ExampleModuleMuonID(Context & ctx);
    virtual bool process(Event & event) override;
private:
    unique_ptr<ExampleMuonIDHists> hists;
    bool firstPrint;
};


ExampleModuleMuonID::ExampleModuleMuonID(Context & ctx):
firstPrint(false)
{
    cout << "Hello World from ExampleModuleMuonID!" << endl;
    hists.reset(new ExampleMuonIDHists(ctx, "MuonID"));
}


bool ExampleModuleMuonID::process(Event & event) {

    if (!firstPrint) {
        for (const auto & muItr : *event.muons) {
            cout << "Loose: " << muItr.get_selector(Muon::Loose) << endl;
            cout << "CutBasedIdLoose: " << muItr.get_selector(Muon::CutBasedIdLoose) << endl;
        }
        firstPrint = true;
    }

    hists->fill(event);

    return true;
}

// as we want to run the ExampleCycleNew directly with AnalysisModuleRunner,
// make sure the ExampleModuleMuonID is found by class name. This is ensured by this macro:
UHH2_REGISTER_ANALYSIS_MODULE(ExampleModuleMuonID)

}
