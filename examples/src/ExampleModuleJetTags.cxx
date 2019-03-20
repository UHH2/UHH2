#include <iostream>
#include <memory>

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/Hists.h"

#include "TH1F.h"
#include "TH2F.h"

using namespace std;
using namespace uhh2;

/** \brief Example of how to use & check Jet IDs/discriminators via tags
 *
 * This brief example shows how to use a tag,
 * and produces a histogram to show how many jets passed each ID.
 * It also shows the distribution of each tag value for TopJets, which store
 * values and not IDs.
 */

namespace uhh2examples {

class ExampleJetTagsHists: public uhh2::Hists {
public:
    ExampleJetTagsHists(uhh2::Context & ctx, const std::string & dirname, const vector<string> & jetIDs_);

    virtual void fill(const uhh2::Event & ev) override;
    virtual ~ExampleJetTagsHists();
private:
    TH1F * hJetIDs;
    vector<string> jetIDs;
};

ExampleJetTagsHists::ExampleJetTagsHists(Context & ctx, const string & dirname, const vector<string> & jetIDs_):
Hists(ctx, dirname),
jetIDs(jetIDs_)
{
    hJetIDs = book<TH1F>("jet_ids", ";Jet ID;N", jetIDs.size(), 0, jetIDs.size());
    // set custom x axis labels
    for (uint i=1;i<=jetIDs.size();i++) {
        hJetIDs->GetXaxis()->SetBinLabel(i, jetIDs.at(i-1).c_str());
    }
}


void ExampleJetTagsHists::fill(const Event & event){
    // Loop over all IDs for all jets, and store which pass
    for (auto & jetItr : *event.jets) {
        for (uint i=0; i < jetIDs.size(); i++) {
            // Here we want the ID status from the ID string name
            // Use tagname2tag to convert string to a tag, then use get_tag
            if (jetItr.get_tag(jetItr.tagname2tag(jetIDs.at(i)))) {
                hJetIDs->Fill(i, event.weight);
            }
        }
    }

}

ExampleJetTagsHists::~ExampleJetTagsHists(){}


class ExampleTopJetTagsHists: public uhh2::Hists {
public:
    ExampleTopJetTagsHists(uhh2::Context & ctx, const std::string & dirname, const vector<string> & topjetTagNames_);

    virtual void fill(const uhh2::Event & ev) override;
    virtual ~ExampleTopJetTagsHists();
private:
    TH2F * hTopJetTags;
    vector<string> topjetTagNames;
};

ExampleTopJetTagsHists::ExampleTopJetTagsHists(Context & ctx, const string & dirname, const vector<string> & topjetTagNames_):
Hists(ctx, dirname),
topjetTagNames(topjetTagNames_)
{
    hTopJetTags = book<TH2F>("topjet_tags", ";TopJet Tag;Value", topjetTagNames.size(), 0, topjetTagNames.size(), 240, -60, 60);
    // set custom x axis labels
    for (uint i=1;i<=topjetTagNames.size();i++) {
        hTopJetTags->GetXaxis()->SetBinLabel(i, topjetTagNames.at(i-1).c_str());
    }
}


void ExampleTopJetTagsHists::fill(const Event & event){
    // Loop over all tags for all topjets, and store their values
    for (auto & jetItr : *event.topjets) {
        for (uint i=0; i < topjetTagNames.size(); i++) {
            // Here we want the tag from the tag string name
            // Use tagname2tag to convert string to a tag, then use get_tag
            if (jetItr.has_tag(jetItr.tagname2tag(topjetTagNames.at(i)))) {
                hTopJetTags->Fill(i, jetItr.get_tag(jetItr.tagname2tag(topjetTagNames.at(i))), event.weight);
            } else {
                hTopJetTags->Fill(i, -10, event.weight);
            }
        }
    }

}

ExampleTopJetTagsHists::~ExampleTopJetTagsHists(){}


class ExampleModuleJetTags: public AnalysisModule {
public:

    explicit ExampleModuleJetTags(Context & ctx);
    virtual bool process(Event & event) override;
private:
    vector<string> jetIDs =  {
        "pileup_loose",
        "pileup_medium",
        "pileup_tight"
    };
    unique_ptr<ExampleJetTagsHists> jetHists;
    vector<string> topjetTags = {
      "mass",
      "fRec",
      "Ropt",
      "RoptCalc",
      "ptForRoptCalc",
      "massRatioPassed",
      "z_ratio",
      "trackSipdSig_3",
      "trackSipdSig_2",
      "trackSipdSig_1",
      "trackSipdSig_0",
      "trackSipdSig_1_0",
      "trackSipdSig_0_0",
      "trackSipdSig_1_1",
      "trackSipdSig_0_1",
      "trackSip2dSigAboveCharm_0",
      "trackSip2dSigAboveBottom_0",
      "trackSip2dSigAboveBottom_1",
      "tau1_trackEtaRel_0",
      "tau1_trackEtaRel_1",
      "tau1_trackEtaRel_2",
      "tau0_trackEtaRel_0",
      "tau0_trackEtaRel_1",
      "tau0_trackEtaRel_2",
      "tau_vertexMass_0",
      "tau_vertexEnergyRatio_0",
      "tau_vertexDeltaR_0",
      "tau_flightDistance2dSig_0",
      "tau_vertexMass_1",
      "tau_vertexEnergyRatio_1",
      "tau_flightDistance2dSig_1",
      "jetNTracks",
      "nSV"
    };
    unique_ptr<ExampleTopJetTagsHists> topjetHists;
    int counter;
};


ExampleModuleJetTags::ExampleModuleJetTags(Context & ctx):
counter(0)
{
    cout << "Hello World from ExampleModuleJetTags!" << endl;
    jetHists.reset(new ExampleJetTagsHists(ctx, "jetID", jetIDs));
    topjetHists.reset(new ExampleTopJetTagsHists(ctx, "topjetTags", topjetTags));
}


bool ExampleModuleJetTags::process(Event & event) {
    if (counter < 10) { // only printout for first few events
        cout << " ** EVENT " << event.event << endl;
        for (const auto & jetItr : *event.jets) {
            // We can use the enum directly, this is the easiest way
            cout << "pileup_loose: " << jetItr.get_tag(Jet::pileup_loose) << endl;
        }
        for (const auto & jetItr : *event.topjets) {
            if (jetItr.has_tag(TopJet::tau0_trackEtaRel_0)) {
                cout << "tau0_trackEtaRel_0: " << jetItr.get_tag(TopJet::tau0_trackEtaRel_0) << endl;
            }
        }
    }
    jetHists->fill(event);
    topjetHists->fill(event);

    counter++;
    return true;
}

// as we want to run the ExampleCycleNew directly with AnalysisModuleRunner,
// make sure the ExampleModuleJetTags is found by class name. This is ensured by this macro:
UHH2_REGISTER_ANALYSIS_MODULE(ExampleModuleJetTags)

}
