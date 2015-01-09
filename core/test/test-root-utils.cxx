#include "TFile.h"
#include "TTree.h"

#include "UHH2/core/include/root-utils.h"
#include <iostream>
#include <set>

using namespace uhh2;
using namespace std;

void create_testtree(const char * outname){
    TFile * f = new TFile(outname, "recreate");
    TTree * t = new TTree("AnalysisTree", "AnalysisTree");

    auto orun = obj::create<int>(1);
    auto oevent = obj::create<int>(1);
    auto ovec = obj::create<vector<int>>(vector<int>{1,2,3});
    
    OutTree otree(t);
    otree.create_branch("run", orun);
    otree.create_branch("event", oevent);
    otree.create_branch("vec", ovec);
    
    for(int run=1; run <= 5; ++run){
        for(int event=1; event<=100; ++event){
            orun->assign<int>(run);
            oevent->assign<int>(event);
            ovec->assign<vector<int>>(vector<int>{run, event, event+run});
            t->Fill();
        }
    }
    
    f->Write();
    f->Close();
    delete f;
}


void read_testtree(const char * inname){
    TFile * f = new TFile(inname, "read");
    assert(f->IsOpen());
    TTree * t = dynamic_cast<TTree*>(f->Get("AnalysisTree"));
    assert(t);
    
    InTree itree(t);
    
    // try reading with wrong type:
    bool except = false;
    try {
        itree.open_branch<float>("run");
    }
    catch(...){
        except = true;
    }
    assert(except);
    
    // enumerate all branches:
    set<string> branches;
    itree.visit_branches([&branches](const std::string & bname, const std::type_info &){ branches.insert(bname); });
    assert(branches.size() == 3);
    assert(*branches.begin() == "event");
    
    auto orun = itree.open_branch<int>("run");
    auto oevent = itree.open_branch<int>("event");
    auto ovec = itree.open_branch<vector<int>>("vec");
    
    assert(orun->type() == typeid(int));
    assert(oevent->type() == typeid(int));
    
    int n = t->GetEntries();
    assert(n == 5*100);
    int i = 0;
    for(int run=1; run <= 5; ++run){
        for(int event=1; event<=100; ++event){
            t->GetEntry(i++);
            assert(orun->get<int>() == run);
            assert(oevent->get<int>() == event);
            const auto & vec = ovec->get<vector<int>>();
            assert(vec.size() == 3);
            assert(vec[0] == run);
            assert(vec[1] == event);
            assert(vec[2] == event + run);
        }
    }
    delete f;
}






int main(){
    cout << "starting tests" << endl;
    create_testtree("test-root-utils-a.root");
    read_testtree("test-root-utils-a.root");
    cout << "tests complete" << endl;
}

