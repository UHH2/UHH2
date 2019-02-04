#include <iostream>
#include <memory>

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/Hists.h"

#include "TH1F.h"
#include "TH2F.h"

using namespace std;
using namespace uhh2;

/** \brief Example of how to use & check Jet IDs via tags
 *
 * This brief example shows how to use a tag,
 * and produces a histogram to show how many jets passed each ID.
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
    unique_ptr<ExampleJetTagsHists> hists;
    int counter;
};


ExampleModuleJetTags::ExampleModuleJetTags(Context & ctx):
counter(0)
{
    cout << "Hello World from ExampleModuleJetTags!" << endl;
    hists.reset(new ExampleJetTagsHists(ctx, "jetID", jetIDs));
}


bool ExampleModuleJetTags::process(Event & event) {
    if (counter < 10) { // only printout for first few events
        for (const auto & jetItr : *event.jets) {
            // We can use the enum directly, this is the easiest way
            cout << "pileup_loose: " << jetItr.get_tag(Jet::pileup_loose) << endl;
        }
    }
    hists->fill(event);

    counter++;
    return true;
}

// as we want to run the ExampleCycleNew directly with AnalysisModuleRunner,
// make sure the ExampleModuleJetTags is found by class name. This is ensured by this macro:
UHH2_REGISTER_ANALYSIS_MODULE(ExampleModuleJetTags)

}
