#include "UHH2/core/include/Selection.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/AnalysisModule.h"
#include "TH1D.h"

using namespace uhh2;
using namespace std;

Selection::~Selection(){}

AndSelection::AndSelection(Context & ctx_, const std::string & cutflow_histo_name): ctx(ctx_), cutflow_hname(cutflow_histo_name), cutflow_raw(0), cutflow_weighted(0){
}

AndSelection::~AndSelection(){}

bool AndSelection::passes(const Event & event){
    bool result = true;
    // create histograms the first time the selection is evaluated.
    if(!cutflow_hname.empty() && cutflow_raw==NULL) create_histos();
    if(cutflow_raw){
        cutflow_raw->Fill(-1);
        cutflow_weighted->Fill(-1, event.weight);
    }
    for(size_t i=0; i<selections.size(); ++i){
        if(selections[i]->passes(event)){
            if(cutflow_raw){
                cutflow_raw->Fill(i);
                cutflow_weighted->Fill(i, event.weight);
            }
        }
        else{
            result = false;
            break;
        }
    }
    return result;
}

void AndSelection::create_histos(){
    cutflow_weighted = new TH1D(("cf_" + cutflow_hname).c_str(), ("Cutflow '" + cutflow_hname + "' using weights").c_str(), selections.size()+1, -1, selections.size());
    cutflow_raw = new TH1D(("cf_" + cutflow_hname + "_raw").c_str(), ("Cutflow '" + cutflow_hname + "' unweighted").c_str(), selections.size()+1, -1, selections.size());    
    for(TAxis * ax : {cutflow_raw->GetXaxis(), cutflow_weighted->GetXaxis()}){
        ax->SetBinLabel(1, "all");
        for(size_t i=0; i<selections.size(); ++i){
            ax->SetBinLabel(i+2, descriptions[i].c_str());
        }
    }
    ctx.put("", cutflow_raw);
    ctx.put("", cutflow_weighted);
}
