#include "UHH2/common/include/MCWeight.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/Utils.h"

#include "TFile.h"
#include "TH1F.h"

#include <stdexcept>

using namespace uhh2;
using namespace std;

MCLumiWeight::MCLumiWeight(Context & ctx){
    use_sframe_weight = string2bool(ctx.get("use_sframe_weight", "true"));
    if(use_sframe_weight){
        return;
    }
    auto dataset_type = ctx.get("dataset_type");
    bool is_mc = dataset_type == "MC";
    if(!is_mc){
        cout << "Warning: MCLumiWeight will not have an effect on this non-MC sample (dataset_type = '" + dataset_type + "')" << endl;
        return;
    } else {
        double dataset_lumi = string2double(ctx.get("dataset_lumi"));
        double reweight_to_lumi = string2double(ctx.get("target_lumi"));
        if(reweight_to_lumi <= 0.0){
            throw runtime_error("MCLumiWeight: target_lumi <= 0.0 not allowed");
        }
        factor = reweight_to_lumi / dataset_lumi;
    }
}

bool MCLumiWeight::process(uhh2::Event & event){
    if(!use_sframe_weight){
        event.weight *= factor;
    }
    return true;
}



MCPileupReweight::MCPileupReweight(Context & ctx){

   auto dataset_type = ctx.get("dataset_type");
   bool is_mc = dataset_type == "MC";
   if(!is_mc){
       cout << "Warning: MCPileupReweight will not have an effect on this non-MC sample (dataset_type = '" + dataset_type + "')" << endl;
       return;
   }

   TString pileup_directory_data = ctx.get("pileup_directory_data");
   TString pileup_directory_50ns = ctx.get("pileup_directory_50ns");
   TString pileup_directory_25ns;
   if (ctx.get("dataset_version").find("25ns")!=std::string::npos) pileup_directory_25ns = ctx.get("pileup_directory_25ns");
   
   if(pileup_directory_data == ""){
      throw runtime_error("MCPileupReweight: not yet implemented!");
   }
   TFile *file_mc;
   if (ctx.get("dataset_version").find("25ns")!=std::string::npos) file_mc = new TFile(pileup_directory_25ns);
   else file_mc = new TFile(pileup_directory_50ns);
   TFile *file_data =  new TFile(pileup_directory_data);
   
   h_npu_mc=(TH1F*) file_mc->Get("input_Event/N_TrueInteractions");
   h_npu_data=(TH1F*) file_data->Get("pileup");
   
   
   if(h_npu_mc->GetNbinsX() != h_npu_data->GetNbinsX()){
      std::cerr << "ERROR: pile-up histograms for data and MC have different numbers of bins" <<std::endl;
      exit(-1);
   }
   if( (h_npu_mc->GetXaxis()->GetXmax() != h_npu_data->GetXaxis()->GetXmax()) || (h_npu_mc->GetXaxis()->GetXmin() != h_npu_data->GetXaxis()->GetXmin())){
      std::cerr << "ERROR: pile-up histograms for data and MC have different axis ranges" <<std::endl;
      exit(-1);
   }
   
   h_npu_mc->Scale(1./h_npu_mc->Integral());
   h_npu_data->Scale(1./h_npu_data->Integral());
}

bool MCPileupReweight::process(Event &event){

   if (event.isRealData) {
      return true;
   }

   double weight =0;
   int binnumber = h_npu_mc->GetXaxis()->FindBin(event.genInfo->pileup_TrueNumInteractions());
   
   if(h_npu_mc->GetBinContent(binnumber)!=0){
      weight = h_npu_data->GetBinContent(binnumber)/h_npu_mc->GetBinContent(binnumber);
   }
   
   event.weight *= weight;       
   return true;
}

MCScaleVariation::MCScaleVariation(Context & ctx){

  auto dataset_type = ctx.get("dataset_type");
  bool is_mc = dataset_type == "MC";
  if(!is_mc){
    cout << "Warning: MCScaleVariation will not have an effect on this non-MC sample (dataset_type = '" + dataset_type + "')" << endl;
    return;
  }
  auto s_mu_r = ctx.get("ScaleVariationMuR");
  auto s_mu_f = ctx.get("ScaleVariationMuF");

  if(s_mu_r == "up") {i_mu_r = 1;}
  else if(s_mu_r == "down"){i_mu_r = 2;}

  if(s_mu_f == "up"){i_mu_f = 1;}
  else if(s_mu_f == "down"){i_mu_f = 2;}

}

bool MCScaleVariation::process(Event & event){
  if (event.isRealData) {return true;}
  if(event.genInfo->systweights().size() == 0) return true;

  if(i_mu_r == 0 && i_mu_f == 0) return true;
  else if(i_mu_r == 0 && i_mu_f == 1) syst_weight = event.genInfo->systweights().at(1);
  else if(i_mu_r == 0 && i_mu_f == 2) syst_weight = event.genInfo->systweights().at(2);
                                      
  else if(i_mu_r == 1 && i_mu_f == 0) syst_weight = event.genInfo->systweights().at(3);
  else if(i_mu_r == 1 && i_mu_f == 1) syst_weight = event.genInfo->systweights().at(4);
  else if(i_mu_r == 1 && i_mu_f == 2) syst_weight = event.genInfo->systweights().at(5);
                                      
  else if(i_mu_r == 2 && i_mu_f == 0) syst_weight = event.genInfo->systweights().at(6);
  else if(i_mu_r == 2 && i_mu_f == 1) syst_weight = event.genInfo->systweights().at(7);
  else if(i_mu_r == 2 && i_mu_f == 2) syst_weight = event.genInfo->systweights().at(8);

  event.weight *= syst_weight/event.genInfo->originalXWGTUP();

  return true;
}

