#include <iostream>
#include <memory>

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/Hists.h"

#include "TH1F.h"
#include "TH2F.h"

using namespace std;
using namespace uhh2;

/** \brief Example of how to use & check Electron & Photon IDs via tags
 *
 * This is the preferred, POG-approved method to check IDs, since it uses
 * stored values made by the official POG recipes.
 * There are manual versions in ElectronIds.cxx, but these may not be up to date.
 *
 * This brief example shows how to use a tag,
 * and produces a histogram to show how many electrons passed each ID.
 * The same is done for photons
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
    TH1F * hElectronIDs;
    TH2F * hElectronMVAs;
    vector<string> electronIDs;
};

ExampleElectronIDHists::ExampleElectronIDHists(Context & ctx, const string & dirname, const vector<string> & electronIDs_):
Hists(ctx, dirname),
electronIDs(electronIDs_)
{
    hElectronIDs = book<TH1F>("ele_ids", ";Electron ID;N", electronIDs.size(), 0, electronIDs.size());
    // set custom x axis labels
    for (uint i=1;i<=electronIDs.size();i++) {
        hElectronIDs->GetXaxis()->SetBinLabel(i, electronIDs.at(i-1).c_str());
    }

    hElectronMVAs = book<TH2F>("ele_mvas", ";Electron MVA;MVA value", 4, 0, 4, 60, -1.2, 1.2);
    hElectronMVAs->GetXaxis()->SetBinLabel(1, "mvaGeneralPurpose");
    hElectronMVAs->GetXaxis()->SetBinLabel(2, "mvaHZZ");
    hElectronMVAs->GetXaxis()->SetBinLabel(3, "mvaIso");
    hElectronMVAs->GetXaxis()->SetBinLabel(4, "mvaNoIso");
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
        // Store MVA values, no easy way to automate this unless one uses std::bind
        hElectronMVAs->Fill(0.1, eleItr.mvaGeneralPurpose(), event.weight);
        hElectronMVAs->Fill(1.1, eleItr.mvaHZZ(), event.weight);
        hElectronMVAs->Fill(2.1, eleItr.mvaIso(), event.weight);
        hElectronMVAs->Fill(3.1, eleItr.mvaNoIso(), event.weight);
    }

}

ExampleElectronIDHists::~ExampleElectronIDHists(){}


class ExamplePhotonIDHists: public uhh2::Hists {
public:
    ExamplePhotonIDHists(uhh2::Context & ctx, const std::string & dirname, const vector<string> & plectronIDs_);

    virtual void fill(const uhh2::Event & ev) override;
    virtual ~ExamplePhotonIDHists();
private:
    TH1F * hPhotonIDs;
    vector<string> photonIDs;
};

ExamplePhotonIDHists::ExamplePhotonIDHists(Context & ctx, const string & dirname, const vector<string> & photonIDs_):
Hists(ctx, dirname),
photonIDs(photonIDs_)
{
    hPhotonIDs = book<TH1F>("photon_ids", ";Photon ID;N", photonIDs.size(), 0, photonIDs.size());
    // set custom x axis labels
    for (uint i=1;i<=photonIDs.size();i++) {
        hPhotonIDs->GetXaxis()->SetBinLabel(i, photonIDs.at(i-1).c_str());
    }

}


void ExamplePhotonIDHists::fill(const Event & event){
    // Loop over all IDs for all photons, and store which pass
    for (auto & phoItr : *event.photons) {
        for (uint i=0; i < photonIDs.size(); i++) {
            // Here we want the ID status from the ID string name
            // Use tagname2tag to convert string to a tag, then use get_tag
            if (phoItr.get_tag(phoItr.tagname2tag(photonIDs.at(i)))) {
                hPhotonIDs->Fill(i, event.weight);
            }
        }
    }

}

ExamplePhotonIDHists::~ExamplePhotonIDHists(){}


class ExampleModuleElectronID: public AnalysisModule {
public:

    explicit ExampleModuleElectronID(Context & ctx);
    virtual bool process(Event & event) override;
private:
    vector<string> electronIDs =  {
        "heepElectronID_HEEPV60",
        "cutBasedElectronID_Summer16_80X_V1_veto",
        "cutBasedElectronID_Summer16_80X_V1_loose",
        "cutBasedElectronID_Summer16_80X_V1_medium",
        "cutBasedElectronID_Summer16_80X_V1_tight",
        "cutBasedElectronHLTPreselection_Summer16_V1",
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
    unique_ptr<ExampleElectronIDHists> eleHists;

    vector<string> photonIDs =  {
        "cutBasedPhotonID_Spring16_V2p2_loose",
        "cutBasedPhotonID_Spring16_V2p2_medium",
        "cutBasedPhotonID_Spring16_V2p2_tight",
        "mvaPhoID_Spring16_nonTrig_V1_wp90",
        "mvaPhoID_Spring16_nonTrig_V1_wp80",
        "cutBasedPhotonID_Fall17_94X_V2_loose",
        "cutBasedPhotonID_Fall17_94X_V2_medium",
        "cutBasedPhotonID_Fall17_94X_V2_tight",
        "mvaPhoID_Fall17_iso_V2_wp90",
        "mvaPhoID_Fall17_iso_V2_wp80"
    };
    unique_ptr<ExamplePhotonIDHists> phoHists;
};


ExampleModuleElectronID::ExampleModuleElectronID(Context & ctx)
{
    cout << "Hello World from ExampleModuleElectronID!" << endl;
    eleHists.reset(new ExampleElectronIDHists(ctx, "electronID", electronIDs));
    phoHists.reset(new ExamplePhotonIDHists(ctx, "photonID", photonIDs));
}


bool ExampleModuleElectronID::process(Event & event) {
    cout << " *** event: " << event.event << endl;

    for (const auto & eleItr : *event.electrons) {
        // We can use the enum directly, this is the easiest way
        cout << "cutBasedElectronID_Summer16_80X_V1_veto: " << eleItr.get_tag(Electron::cutBasedElectronID_Summer16_80X_V1_veto) << endl;
        cout << "cutBasedElectronID_Fall17_94X_V2_veto: " << eleItr.get_tag(Electron::cutBasedElectronID_Fall17_94X_V2_veto) << endl;
    }
    for (const auto & phoItr : *event.photons) {
        // We can use the enum directly, this is the easiest way
        cout << "cutBasedPhotonID_Spring16_V2p2_loose: " << phoItr.get_tag(Photon::cutBasedPhotonID_Spring16_V2p2_loose) << endl;
        cout << "mvaPhoID_Fall17_iso_V2_wp80: " << phoItr.get_tag(Photon::mvaPhoID_Fall17_iso_V2_wp80) << endl;
    }

    eleHists->fill(event);
    phoHists->fill(event);

    return true;
}

// as we want to run the ExampleCycleNew directly with AnalysisModuleRunner,
// make sure the ExampleModuleElectronID is found by class name. This is ensured by this macro:
UHH2_REGISTER_ANALYSIS_MODULE(ExampleModuleElectronID)

}
