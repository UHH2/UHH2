

// test reading the vector<bool> triggerResults from the AnalysisTree with plain
// ROOT.

#include "TFile.h"
#include "TTree.h"

#include <memory>
#include <cassert>
#include <iostream>

using namespace std;

int main(){
    unique_ptr<TFile> infile(TFile::Open("/afs/desy.de/user/o/ottjoc/xxl-af-cms/CMSSW_7_2_1_patch4/src/UHH2/core/Ntuple.root", "read"));
    TTree * tree = dynamic_cast<TTree*>(infile->Get("AnalysisTree")); // belongs to infile
    assert(tree);
    
    std::vector<bool> triggerResults;
    auto pTriggerResults = &triggerResults;
    TBranch * b = tree->GetBranch("triggerResults");
    assert(b);
    b->SetAddress(&pTriggerResults);
    
    for(int i=0; i<10; ++i){
        b->GetEntry(i);
        cout << "entry " << i << " has " << triggerResults.size() << " triggers" << endl;
    }
}

