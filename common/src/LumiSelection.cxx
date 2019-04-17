#include "UHH2/common/include/LumiSelection.h"
#include "UHH2/common/include/Utils.h"

#include "TFile.h"
#include "TTree.h"

using namespace uhh2;
using namespace std;


LumiSelection::LumiSelection(uhh2::Context & ctx){

  string lumifile = locate_file(ctx.get("lumi_file"));
  std::unique_ptr<TFile> file(TFile::Open(lumifile.c_str(), "read"));
  TTree * tree = dynamic_cast<TTree*>(file->Get("AnalysisTree"));
  if(!tree){
    throw runtime_error("LuminosityHists: Did not find TTree 'AnalysisTree' in file ;" + lumifile + "'");
  }
  // only fetch branches we really need:
  TBranch * brun = tree->GetBranch("run");
  TBranch * blumiblock = tree->GetBranch("luminosityBlock");
  run_lumi rl;
  brun->SetAddress(&rl.run);
  blumiblock->SetAddress(&rl.lumiblock);

  auto ientries = tree->GetEntries();
  for(auto ientry = 0l; ientry < ientries; ientry++){
    for(auto b : {brun, blumiblock}){
      b->GetEntry(ientry);
    }
    rls.push_back(rl);
  }

}

bool LumiSelection::passes(const Event & event){

  for(size_t i=0; i< rls.size(); i++){
    if(rls[i].run==event.run && rls[i].lumiblock==event.luminosityBlock) return true;
  }

  return false;
}

