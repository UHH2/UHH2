

// create a small test tree for testing the trigger reading and writing
// mechanism.


// In particular test cases in which:
// a. the first event in the file has the triggers (this will be the usual case)
// b. not the first event in the file has the triggers of the current run, but some event in the middle
// c. multiple runs with multiple triggers, events are not in any strict (run-increasing) order
//
//
// Only write the bare minimum for trees: runid, lumi, eventid, beamspot, with some known numbers,
// and then compare the result to the expected result.

#include "TFile.h"
#include "TTree.h"

#include "UHH2/core/include/Event.h"
#include "UHH2/core/test/common.h"

using namespace uhh2;

struct options {
    int runid_min, runid_max;
    int nevents_per_run;
};

// esel(eventno) determines whether to write the trigger names.
void create_testtree(const char * outname, const std::function<bool (int)> & esel, const options & opts){
    TFile * f = new TFile(outname, "recreate");
    TTree * t = new TTree("AnalysisTree", "AnalysisTree");
    int run, event, lumi;
    bool data = false;
    std::vector<bool> triggers;
    std::vector<std::string> triggernames_tree, triggernames;
    
    triggernames.push_back("trig1");
    triggernames.push_back("trig2");
    
    triggers.resize(2, true);
    
    t->Branch("run", &run);
    t->Branch("event", &event);
    t->Branch("luminosityBlock", &lumi);
    t->Branch("isRealData", &data);
    
    t->Branch("triggerNames", &triggernames_tree);
    t->Branch("triggerResults", &triggers);
    
    lumi = 0;
    
    for(run=opts.runid_min; run<=opts.runid_max; ++run){
        for(event=1; event<=opts.nevents_per_run; ++event){
            if(esel(event)){
                triggernames_tree = triggernames;
            }
            else{
                triggernames_tree.clear();
            }
            lumi = calc_lumi(run, event);
            triggers[0] = calc_trig1(run, event);
            triggers[1] = calc_trig2(run, event);
            t->Fill();
        }
    }
    
    f->Write();
    f->Close();
}

int main(){
    options opts{1, 2, 1000};
    create_testtree("testtree-a.root", [](int e){ return e == 1;}, opts);
    create_testtree("testtree-b.root", [](int e){ return e == 500;}, opts);
}

