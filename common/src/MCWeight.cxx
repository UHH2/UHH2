// #include "UHH2/common/include/BTagCalibrationStandalone.h"
#include "UHH2/common/include/MCWeight.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/Utils.h"
#include "UHH2/common/include/Utils.h"

#include "Riostream.h"
#include "TFile.h"
#include "TH1F.h"
#include "TObjArray.h"
#include "TObjString.h"

#include <stdexcept>

using namespace uhh2;
using namespace std;

//____________________________________________________________________________________________________
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
    double dataset_lumi = abs(string2double(ctx.get("dataset_lumi")));
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

//____________________________________________________________________________________________________
MCPileupReweight::MCPileupReweight(Context & ctx, const std::string & sysType):
  h_pu_weight_(ctx.declare_event_output<float>("weight_pu")),
  h_pu_weight_up_(ctx.declare_event_output<float>("weight_pu_up")),
  h_pu_weight_down_(ctx.declare_event_output<float>("weight_pu_down")),
  h_npu_data_up(0),
  h_npu_data_down(0),
  sysType_(sysType)
{
  auto dataset_type = ctx.get("dataset_type");
  bool is_mc = dataset_type == "MC";
  if(!is_mc){
    cout << "Warning: MCPileupReweight will not have an effect on this non-MC sample (dataset_type = '" + dataset_type + "')" << endl;
    return;
  }

  //    h_pu_weight_ = ctx.declare_event_output<float>("weight_pu");

  // backward compatibility: (((no tag) is chosen over 25ns) is chosen over 50ns)
  std::string pileup_directory           = ctx.get("pileup_directory",
  ctx.get("pileup_directory_25ns",
  ctx.get("pileup_directory_50ns", "")));
  std::string pileup_directory_data      = ctx.get("pileup_directory_data");
  std::string pileup_directory_data_up   = ctx.get("pileup_directory_data_up", "");
  std::string pileup_directory_data_down = ctx.get("pileup_directory_data_down", "");

  if(pileup_directory_data == ""){
    throw runtime_error("MCPileupReweight: pileup_directory_data is needed!");
  }
  if(pileup_directory == ""){
    throw runtime_error("MCPileupReweight: pileup_directory is needed!");
  }
  if (sysType == "up" && !pileup_directory_data_up.length()) {
    throw runtime_error("MCPileupReweight: pileup_directory_data_up is needed!");
  }
  if (sysType == "down" && !pileup_directory_data_down.length()) {
    throw runtime_error("MCPileupReweight: pileup_directory_data_down is needed!");
  }
  if (sysType != "up" && sysType != "down") {
    sysType_ = "";  // this is checked first in the process function
  }

  TFile file_mc(locate_file(pileup_directory).c_str());
  h_npu_mc   = (TH1F*) file_mc.Get("input_Event/N_TrueInteractions");
  h_npu_mc->SetDirectory(0);
  h_npu_mc->Scale(1./h_npu_mc->Integral());

  TFile file_data(locate_file(pileup_directory_data).c_str());
  h_npu_data = (TH1F*) file_data.Get("pileup");
  h_npu_data->SetDirectory(0);
  h_npu_data->Scale(1./h_npu_data->Integral());

  if(h_npu_mc->GetNbinsX() != h_npu_data->GetNbinsX()){
    throw runtime_error("MCPileupReweight: pile-up histograms for data and MC have different numbers of bins");
  }
  if( (h_npu_mc->GetXaxis()->GetXmax() != h_npu_data->GetXaxis()->GetXmax())
  || (h_npu_mc->GetXaxis()->GetXmin() != h_npu_data->GetXaxis()->GetXmin()))
  {
    throw runtime_error("MCPileupReweight: pile-up histograms for data and MC have different axis ranges");
  }

  if (pileup_directory_data_up.length()) {
    TFile file_data_up(locate_file(pileup_directory_data_up).c_str());
    h_npu_data_up = (TH1F*) file_data_up.Get("pileup");
    h_npu_data_up->SetDirectory(0);
    h_npu_data_up->Scale(1./h_npu_data_up->Integral());
    if(h_npu_mc->GetNbinsX() != h_npu_data_up->GetNbinsX()){
      throw runtime_error("MCPileupReweight: pile-up histograms for data_up and MC have different numbers of bins");
    }
    if( (h_npu_mc->GetXaxis()->GetXmax() != h_npu_data_up->GetXaxis()->GetXmax())
    || (h_npu_mc->GetXaxis()->GetXmin() != h_npu_data_up->GetXaxis()->GetXmin()))
    {
      throw runtime_error("MCPileupReweight: pile-up histograms for data_up and MC have different axis ranges");
    }
  }
  if (pileup_directory_data_down.length()) {
    TFile file_data_down(locate_file(pileup_directory_data_down).c_str());
    h_npu_data_down = (TH1F*) file_data_down.Get("pileup");
    h_npu_data_down->SetDirectory(0);
    h_npu_data_down->Scale(1./h_npu_data_down->Integral());
    if(h_npu_mc->GetNbinsX() != h_npu_data_down->GetNbinsX()){
      throw runtime_error("MCPileupReweight: pile-up histograms for data_down and MC have different numbers of bins");
    }
    if( (h_npu_mc->GetXaxis()->GetXmax() != h_npu_data_down->GetXaxis()->GetXmax())
    || (h_npu_mc->GetXaxis()->GetXmin() != h_npu_data_down->GetXaxis()->GetXmin()))
    {
      throw runtime_error("MCPileupReweight: pile-up histograms for data_down and MC have different axis ranges");
    }
  }
}

bool MCPileupReweight::process(Event &event){
  event.set(h_pu_weight_, 0.f);
  event.set(h_pu_weight_up_, 0.f);
  event.set(h_pu_weight_down_, 0.f);
  if (event.isRealData) {
    event.set(h_pu_weight_, 1.f);
    return true;
  }

  double weight = 0., weight_up = 0., weight_down = 0., trueNumInteractions = 0.;
  // handle scenarios where events fall outside of our histograms
  // esp for 94X MC screwup where -ve # vertices exist

  try{
    trueNumInteractions = event.genInfo->pileup_TrueNumInteractions();
    if (event.genInfo->pileup_TrueNumInteractions() < h_npu_mc->GetXaxis()->GetXmin()) {
      cout << "WARNING trueNumInteractions = " << trueNumInteractions << " < lower edge of MC hist = " << h_npu_mc->GetXaxis()->GetXmin();
      cout << " Setting event weight_pu to 0" << endl;
      return false;
    } else if (event.genInfo->pileup_TrueNumInteractions() > h_npu_mc->GetXaxis()->GetXmax()) {
      cout << "WARNING trueNumInteractions = " << trueNumInteractions << " > upper edge of MC hist = " << h_npu_mc->GetXaxis()->GetXmax();
      cout << " Setting event weight_pu to 0" << endl;
      return false;
    }
  }
  catch(const std::runtime_error& error){
    std::cout<<"Problem with genInfo in MCWeight.cxx"<<std::endl;
    std::cout<<error.what();
  }

  int binnumber = h_npu_mc->GetXaxis()->FindBin(trueNumInteractions);
  auto mc_cont = h_npu_mc->GetBinContent(binnumber);
  if (mc_cont > 0) {
    weight = h_npu_data->GetBinContent(binnumber)/mc_cont;
    event.set(h_pu_weight_, weight);

    if (h_npu_data_up) {
      weight_up = h_npu_data_up->GetBinContent(binnumber)/mc_cont;
      event.set(h_pu_weight_up_, weight_up);
    }

    if (h_npu_data_down) {
      weight_down = h_npu_data_down->GetBinContent(binnumber)/mc_cont;
      event.set(h_pu_weight_down_, weight_down);
    }
  } else {
    cout << "WARNING no value in MC hist for trueNumInteractions = " << trueNumInteractions;
    cout << " Setting event weight_pu to 0" << endl;
    return false;
  }

  if (sysType_ == "") {
    event.weight *= weight;
  } else if (sysType_ == "down") {
    event.weight *= weight_down;
  } else {
    event.weight *= weight_up;
  }


  return true;
}

//____________________________________________________________________________________________________
MCScaleVariation::MCScaleVariation(Context & ctx){

  auto dataset_type = ctx.get("dataset_type");
  bool is_mc = dataset_type == "MC";
  h_murmuf_weight_upup_     = ctx.declare_event_output<float>("weight_murmuf_upup");
  h_murmuf_weight_upnone_   = ctx.declare_event_output<float>("weight_murmuf_upnone");
  h_murmuf_weight_noneup_   = ctx.declare_event_output<float>("weight_murmuf_noneup");
  h_murmuf_weight_downdown_ = ctx.declare_event_output<float>("weight_murmuf_downdown");
  h_murmuf_weight_downnone_ = ctx.declare_event_output<float>("weight_murmuf_downnone");
  h_murmuf_weight_nonedown_ = ctx.declare_event_output<float>("weight_murmuf_nonedown");

  h_murmuf_dyn1_weight_upup_     = ctx.declare_event_output<float>("weight_murmuf_dyn1_upup");
  h_murmuf_dyn1_weight_upnone_   = ctx.declare_event_output<float>("weight_murmuf_dyn1_upnone");
  h_murmuf_dyn1_weight_noneup_   = ctx.declare_event_output<float>("weight_murmuf_dyn1_noneup");
  h_murmuf_dyn1_weight_downdown_ = ctx.declare_event_output<float>("weight_murmuf_dyn1_downdown");
  h_murmuf_dyn1_weight_downnone_ = ctx.declare_event_output<float>("weight_murmuf_dyn1_downnone");
  h_murmuf_dyn1_weight_nonedown_ = ctx.declare_event_output<float>("weight_murmuf_dyn1_nonedown");

  h_murmuf_dyn2_weight_upup_     = ctx.declare_event_output<float>("weight_murmuf_dyn2_upup");
  h_murmuf_dyn2_weight_upnone_   = ctx.declare_event_output<float>("weight_murmuf_dyn2_upnone");
  h_murmuf_dyn2_weight_noneup_   = ctx.declare_event_output<float>("weight_murmuf_dyn2_noneup");
  h_murmuf_dyn2_weight_downdown_ = ctx.declare_event_output<float>("weight_murmuf_dyn2_downdown");
  h_murmuf_dyn2_weight_downnone_ = ctx.declare_event_output<float>("weight_murmuf_dyn2_downnone");
  h_murmuf_dyn2_weight_nonedown_ = ctx.declare_event_output<float>("weight_murmuf_dyn2_nonedown");

  h_murmuf_dyn3_weight_upup_     = ctx.declare_event_output<float>("weight_murmuf_dyn3_upup");
  h_murmuf_dyn3_weight_upnone_   = ctx.declare_event_output<float>("weight_murmuf_dyn3_upnone");
  h_murmuf_dyn3_weight_noneup_   = ctx.declare_event_output<float>("weight_murmuf_dyn3_noneup");
  h_murmuf_dyn3_weight_downdown_ = ctx.declare_event_output<float>("weight_murmuf_dyn3_downdown");
  h_murmuf_dyn3_weight_downnone_ = ctx.declare_event_output<float>("weight_murmuf_dyn3_downnone");
  h_murmuf_dyn3_weight_nonedown_ = ctx.declare_event_output<float>("weight_murmuf_dyn3_nonedown");

  h_murmuf_dyn4_weight_upup_     = ctx.declare_event_output<float>("weight_murmuf_dyn4_upup");
  h_murmuf_dyn4_weight_upnone_   = ctx.declare_event_output<float>("weight_murmuf_dyn4_upnone");
  h_murmuf_dyn4_weight_noneup_   = ctx.declare_event_output<float>("weight_murmuf_dyn4_noneup");
  h_murmuf_dyn4_weight_downdown_ = ctx.declare_event_output<float>("weight_murmuf_dyn4_downdown");
  h_murmuf_dyn4_weight_downnone_ = ctx.declare_event_output<float>("weight_murmuf_dyn4_downnone");
  h_murmuf_dyn4_weight_nonedown_ = ctx.declare_event_output<float>("weight_murmuf_dyn4_nonedown");

  if(!is_mc){
    cout << "Warning: MCScaleVariation will not have an effect on this non-MC sample (dataset_type = '" + dataset_type + "')" << endl;
    return;
  }
  auto s_mu_r = ctx.get("ScaleVariationMuR");
  auto s_mu_f = ctx.get("ScaleVariationMuF");
  is_dy = ctx.get("dataset_version").find("DYJets") == 0;
  is_wjets = ctx.get("dataset_version").find("WJets") == 0;
  is_qcd_HTbinned = ctx.get("dataset_version").find("QCD_HT") == 0;
  is_alps = ctx.get("dataset_version").find("ALP") == 0;
  is_azh = (ctx.get("dataset_version").find("AZH") == 0) ||
           (ctx.get("dataset_version").find("AToZHToLLTTbar") == 0);
  is_htott_scalar = ctx.get("dataset_version").find("HscalarToTTTo") == 0;
  is_htott_pseudo = ctx.get("dataset_version").find("HpseudoToTTTo") == 0;
  is_zprimetott = ctx.get("dataset_version").find("ZPrimeToTT_") == 0;
  is_tstartstar = ctx.get("dataset_version").find("TstarTstar") != std::string::npos;

  if(s_mu_r == "up") {i_mu_r = 1;}
  else if(s_mu_r == "down"){i_mu_r = 2;}

  if(s_mu_f == "up"){i_mu_f = 1;}
  else if(s_mu_f == "down"){i_mu_f = 2;}

}

void MCScaleVariation::initialise_handles(Event & event){
  event.set(h_murmuf_weight_upup_, 1.);
  event.set(h_murmuf_weight_upnone_, 1.);
  event.set(h_murmuf_weight_noneup_, 1.);
  event.set(h_murmuf_weight_downdown_, 1.);
  event.set(h_murmuf_weight_downnone_, 1.);
  event.set(h_murmuf_weight_nonedown_, 1.);
  // DYN 1: Sum PT
  event.set(h_murmuf_dyn1_weight_upup_, 1.);
  event.set(h_murmuf_dyn1_weight_upnone_, 1.);
  event.set(h_murmuf_dyn1_weight_noneup_, 1.);
  event.set(h_murmuf_dyn1_weight_downdown_, 1.);
  event.set(h_murmuf_dyn1_weight_downnone_, 1.);
  event.set(h_murmuf_dyn1_weight_nonedown_, 1.);
  // DYN 2: HT
  event.set(h_murmuf_dyn2_weight_upup_, 1.);
  event.set(h_murmuf_dyn2_weight_upnone_, 1.);
  event.set(h_murmuf_dyn2_weight_noneup_, 1.);
  event.set(h_murmuf_dyn2_weight_downdown_, 1.);
  event.set(h_murmuf_dyn2_weight_downnone_, 1.);
  event.set(h_murmuf_dyn2_weight_nonedown_, 1.);
  // DYN 3: HT / 2
  event.set(h_murmuf_dyn3_weight_upup_, 1.);
  event.set(h_murmuf_dyn3_weight_upnone_, 1.);
  event.set(h_murmuf_dyn3_weight_noneup_, 1.);
  event.set(h_murmuf_dyn3_weight_downdown_, 1.);
  event.set(h_murmuf_dyn3_weight_downnone_, 1.);
  event.set(h_murmuf_dyn3_weight_nonedown_, 1.);
  // DYN 4: SQRTS
  event.set(h_murmuf_dyn4_weight_upup_, 1.);
  event.set(h_murmuf_dyn4_weight_upnone_, 1.);
  event.set(h_murmuf_dyn4_weight_noneup_, 1.);
  event.set(h_murmuf_dyn4_weight_downdown_, 1.);
  event.set(h_murmuf_dyn4_weight_downnone_, 1.);
  event.set(h_murmuf_dyn4_weight_nonedown_, 1.);
}

bool MCScaleVariation::process(Event & event){


  initialise_handles(event);  // Set weights to 1 for data
  if (event.isRealData) return true;

  try{
    // Set weights to 1 if no genInfo scale variation weights exist
    if(event.genInfo->systweights().size() < 9) return true;

    // Set handles, written for all relevant cases irrespective of
    // the values of mu_r and mu_f specified in the config file
    if ( is_dy || is_wjets || is_qcd_HTbinned || is_alps || is_azh || is_htott_scalar || is_htott_pseudo || is_zprimetott || is_tstartstar ) {
      event.set(h_murmuf_weight_upup_, event.genInfo->systweights().at(20)/event.genInfo->originalXWGTUP());
      event.set(h_murmuf_dyn1_weight_upup_, event.genInfo->systweights().at(21)/event.genInfo->originalXWGTUP());
      event.set(h_murmuf_dyn2_weight_upup_, event.genInfo->systweights().at(22)/event.genInfo->originalXWGTUP());
      event.set(h_murmuf_dyn3_weight_upup_, event.genInfo->systweights().at(23)/event.genInfo->originalXWGTUP());
      event.set(h_murmuf_dyn4_weight_upup_, event.genInfo->systweights().at(24)/event.genInfo->originalXWGTUP());
      event.set(h_murmuf_weight_upnone_, event.genInfo->systweights().at(5)/event.genInfo->originalXWGTUP());
      event.set(h_murmuf_dyn1_weight_upnone_, event.genInfo->systweights().at(6)/event.genInfo->originalXWGTUP());
      event.set(h_murmuf_dyn2_weight_upnone_, event.genInfo->systweights().at(7)/event.genInfo->originalXWGTUP());
      event.set(h_murmuf_dyn3_weight_upnone_, event.genInfo->systweights().at(8)/event.genInfo->originalXWGTUP());
      event.set(h_murmuf_dyn4_weight_upnone_, event.genInfo->systweights().at(9)/event.genInfo->originalXWGTUP());
      event.set(h_murmuf_weight_noneup_, event.genInfo->systweights().at(15)/event.genInfo->originalXWGTUP());
      event.set(h_murmuf_dyn1_weight_noneup_, event.genInfo->systweights().at(16)/event.genInfo->originalXWGTUP());
      event.set(h_murmuf_dyn2_weight_noneup_, event.genInfo->systweights().at(17)/event.genInfo->originalXWGTUP());
      event.set(h_murmuf_dyn3_weight_noneup_, event.genInfo->systweights().at(18)/event.genInfo->originalXWGTUP());
      event.set(h_murmuf_dyn4_weight_noneup_, event.genInfo->systweights().at(19)/event.genInfo->originalXWGTUP());
      event.set(h_murmuf_weight_downdown_, event.genInfo->systweights().at(40)/event.genInfo->originalXWGTUP());
      event.set(h_murmuf_dyn4_weight_downdown_, event.genInfo->systweights().at(41)/event.genInfo->originalXWGTUP());
      event.set(h_murmuf_dyn4_weight_downdown_, event.genInfo->systweights().at(42)/event.genInfo->originalXWGTUP());
      event.set(h_murmuf_dyn4_weight_downdown_, event.genInfo->systweights().at(43)/event.genInfo->originalXWGTUP());
      event.set(h_murmuf_dyn4_weight_downdown_, event.genInfo->systweights().at(44)/event.genInfo->originalXWGTUP());
      event.set(h_murmuf_weight_downnone_, event.genInfo->systweights().at(10)/event.genInfo->originalXWGTUP());
      event.set(h_murmuf_dyn1_weight_downnone_, event.genInfo->systweights().at(11)/event.genInfo->originalXWGTUP());
      event.set(h_murmuf_dyn2_weight_downnone_, event.genInfo->systweights().at(12)/event.genInfo->originalXWGTUP());
      event.set(h_murmuf_dyn3_weight_downnone_, event.genInfo->systweights().at(13)/event.genInfo->originalXWGTUP());
      event.set(h_murmuf_dyn4_weight_downnone_, event.genInfo->systweights().at(14)/event.genInfo->originalXWGTUP());
      event.set(h_murmuf_weight_nonedown_, event.genInfo->systweights().at(30)/event.genInfo->originalXWGTUP());
      event.set(h_murmuf_dyn1_weight_nonedown_, event.genInfo->systweights().at(31)/event.genInfo->originalXWGTUP());
      event.set(h_murmuf_dyn2_weight_nonedown_, event.genInfo->systweights().at(32)/event.genInfo->originalXWGTUP());
      event.set(h_murmuf_dyn3_weight_nonedown_, event.genInfo->systweights().at(33)/event.genInfo->originalXWGTUP());
      event.set(h_murmuf_dyn4_weight_nonedown_, event.genInfo->systweights().at(34)/event.genInfo->originalXWGTUP());
    }
    else {
      event.set(h_murmuf_weight_upup_, event.genInfo->systweights().at(4)/event.genInfo->originalXWGTUP());
      event.set(h_murmuf_weight_upnone_, event.genInfo->systweights().at(1)/event.genInfo->originalXWGTUP());
      event.set(h_murmuf_weight_noneup_, event.genInfo->systweights().at(3)/event.genInfo->originalXWGTUP());
      event.set(h_murmuf_weight_downdown_, event.genInfo->systweights().at(8)/event.genInfo->originalXWGTUP());
      event.set(h_murmuf_weight_downnone_, event.genInfo->systweights().at(2)/event.genInfo->originalXWGTUP());
      event.set(h_murmuf_weight_nonedown_, event.genInfo->systweights().at(6)/event.genInfo->originalXWGTUP());
    }

    // Modify event weight according to scale variation specified in config file
    if (i_mu_r == 0 && i_mu_f == 0) return true;
    else if(i_mu_r == 0 && i_mu_f == 1) syst_weight = event.genInfo->systweights().at(1);
    else if(i_mu_r == 0 && i_mu_f == 2) syst_weight = event.genInfo->systweights().at(2);

    else if(i_mu_r == 1 && i_mu_f == 0) syst_weight = event.genInfo->systweights().at(3);
    else if(i_mu_r == 1 && i_mu_f == 1) syst_weight = event.genInfo->systweights().at(4);
    else if(i_mu_r == 1 && i_mu_f == 2) syst_weight = event.genInfo->systweights().at(5);

    else if(i_mu_r == 2 && i_mu_f == 0) syst_weight = event.genInfo->systweights().at(6);
    else if(i_mu_r == 2 && i_mu_f == 1) syst_weight = event.genInfo->systweights().at(7);
    else if(i_mu_r == 2 && i_mu_f == 2) syst_weight = event.genInfo->systweights().at(8);

    event.weight *= syst_weight/event.genInfo->originalXWGTUP();
  }
  catch(const std::runtime_error& error){
    std::cout<<"Problem with genInfo in MCWeight.cxx"<<std::endl;
    std::cout<<error.what();
  }

  return true;
}

//____________________________________________________________________________________________________
MCMuonScaleFactor::MCMuonScaleFactor(
  uhh2::Context & ctx,
  const std::string & sf_file_path,
  const std::string & sf_name,
  float sys_error_percantage,
  const std::string & weight_postfix,
  bool etaYaxis,
  const std::string & sys_uncert,
  const std::string & muons_handle_name,
  const bool absolute_eta
):
  h_muons_            (ctx.get_handle<std::vector<Muon>>(muons_handle_name)),
  h_muon_weight_      (ctx.declare_event_output<float>("weight_sfmu_" + weight_postfix)),
  h_muon_weight_up_   (ctx.declare_event_output<float>("weight_sfmu_" + weight_postfix + "_up")),
  h_muon_weight_down_ (ctx.declare_event_output<float>("weight_sfmu_" + weight_postfix + "_down")),
  sys_error_factor_(sys_error_percantage/100.), etaYaxis_(etaYaxis),
  fAbsoluteEta(absolute_eta)
{
  auto dataset_type = ctx.get("dataset_type");
  bool is_mc = dataset_type == "MC";
  if (!is_mc) {
    cout << "Warning: MCMuonScaleFactor will not have an effect on "
    <<" this non-MC sample (dataset_type = '" + dataset_type + "')" << endl;
    return;
  }


  TFile sf_file(locate_file(sf_file_path).c_str());
  if (sf_file.IsZombie()) {
    throw runtime_error("Scale factor file for muons not found: " + sf_file_path);
  }

  sf_hist_.reset((TH2*) sf_file.Get((sf_name + "/abseta_pt").c_str()));
  if (!sf_hist_.get()) {
    sf_hist_.reset((TH2*) sf_file.Get((sf_name + "/abseta_pt_ratio").c_str()));
    if (!sf_hist_.get()) {
      sf_hist_.reset((TH2*) sf_file.Get((sf_name + "/abseta_pair_newTuneP_probe_pt").c_str()));
      if (!sf_hist_.get()) {
        sf_hist_.reset((TH2*) sf_file.Get((sf_name).c_str()));
        if (!sf_hist_.get()) {
          throw runtime_error("Scale factor directory not found in file: " + sf_name);
        }
      }
    }
  }

  sf_hist_->SetDirectory(0);
  if(etaYaxis_){
    eta_min_ = sf_hist_->GetYaxis()->GetXmin();
    eta_max_ = sf_hist_->GetYaxis()->GetXmax();
    pt_min_  = sf_hist_->GetXaxis()->GetXmin();
    pt_max_  = sf_hist_->GetXaxis()->GetXmax();
  }
  else{
    eta_min_ = sf_hist_->GetXaxis()->GetXmin();
    eta_max_ = sf_hist_->GetXaxis()->GetXmax();
    pt_min_  = sf_hist_->GetYaxis()->GetXmin();
    pt_max_  = sf_hist_->GetYaxis()->GetXmax();
  }
  sys_direction_ = 0;
  if (sys_uncert == "up") {
    sys_direction_ = 1;
  } else if (sys_uncert == "down") {
    sys_direction_ = -1;
  }
}

bool MCMuonScaleFactor::process(uhh2::Event & event) {

  if (!sf_hist_) {
    event.set(h_muon_weight_,       1.);
    event.set(h_muon_weight_up_,    1.);
    event.set(h_muon_weight_down_,  1.);
    return true;
  }

  const auto & muons = event.get(h_muons_);
  float weight = 1., weight_up = 1., weight_down = 1.;
  for (const auto & mu : muons) {
    float eta = fAbsoluteEta ? fabs(mu.eta()) : mu.eta();
    float pt = mu.pt();
    if (eta_min_ < eta && eta_max_ > eta){
      bool out_of_range = false;
      //take scale factor from the last measured pT bin in case of too large/small pT
      if(pt_min_ >= pt) {
        pt=pt_min_+0.0001;
        out_of_range = true;
      }
      if(pt_max_ <= pt) {
        pt=pt_max_-0.0001;
        out_of_range = true;
      }

      int bin;
      if(etaYaxis_) bin = sf_hist_->FindFixBin(pt, eta);
      else          bin = sf_hist_->FindFixBin(eta, pt);

      float w   = sf_hist_->GetBinContent(bin);
      float err = sf_hist_->GetBinError(bin);

      if (w==1 and err==0) {
        if (bin==1) bin +=1;
        else bin -= 1;
        w   = sf_hist_->GetBinContent(bin);
        err = sf_hist_->GetBinError(bin);
      }

      float err_tot = sqrt(err*err + pow(w*sys_error_factor_, 2));
      //take twice the uncertainty if the pT is outside the measured pT range
      if(out_of_range) err_tot*=2;

      weight      *= w;
      weight_up   *= w + err_tot;
      weight_down *= w - err_tot;
    }

  }


  event.set(h_muon_weight_,       weight);
  event.set(h_muon_weight_up_,    weight_up);
  event.set(h_muon_weight_down_,  weight_down);

  if (sys_direction_ == 1) {
    event.weight *= weight_up;
  } else if (sys_direction_ == -1) {
    event.weight *= weight_down;
  } else {
    event.weight *= weight;
  }

  return true;
}


bool MCMuonScaleFactor::process_onemuon(uhh2::Event & event, int i) {

  if (!sf_hist_) {
    event.set(h_muon_weight_,       1.);
    event.set(h_muon_weight_up_,    1.);
    event.set(h_muon_weight_down_,  1.);
    return true;
  }

  const auto & muons = event.get(h_muons_);
  float weight = 1., weight_up = 1., weight_down = 1.;
  float err_propagation = 0.;
  Muon mu = muons.at(i);
  float eta = fAbsoluteEta ? fabs(mu.eta()) : mu.eta();
  float pt = mu.pt();
  if (eta_min_ < eta && eta_max_ > eta){
    bool out_of_range = false;
    //take scale factor from the last measured pT bin in case of too large/small pT
    if(pt_min_ >= pt) {
      pt=pt_min_+0.0001;
      out_of_range = true;
    }
    if(pt_max_ <= pt) {
      pt=pt_max_-0.0001;
      out_of_range = true;
    }

    int bin;
    if(etaYaxis_)
    bin       = sf_hist_->FindFixBin(pt, eta);
    else
    bin       = sf_hist_->FindFixBin(eta, pt);

    float w       = sf_hist_->GetBinContent(bin);
    float err     = sf_hist_->GetBinError(bin);

    if (w==1 and err==0) {
      if (bin==1) bin +=1;
      else bin -= 1;
      w   = sf_hist_->GetBinContent(bin);
      err = sf_hist_->GetBinError(bin);
    }

    float err_tot = sqrt(err*err + pow(w*sys_error_factor_, 2));
    //take twice the uncertainty if the pT is outside the measured pT range
    if(out_of_range) err_tot*=2;

    weight      *= w;
    err_propagation += pow(err_tot/w, 2);

  }

  weight_up = weight + sqrt(err_propagation);
  weight_down = weight - sqrt(err_propagation);

  event.set(h_muon_weight_,       weight);
  event.set(h_muon_weight_up_,    weight_up);
  event.set(h_muon_weight_down_,  weight_down);

  if (sys_direction_ == 1) {
    event.weight *= weight_up;
  } else if (sys_direction_ == -1) {
    event.weight *= weight_down;
  } else {
    event.weight *= weight;
  }

  return true;
}

//____________________________________________________________________________________________________
MCMuonTrkScaleFactor::MCMuonTrkScaleFactor(
  uhh2::Context & ctx,
  const std::string & sf_file_path,
  float sys_error_percantage,
  const std::string & weight_postfix,
  const std::string & sys_uncert,
  const std::string & muons_handle_name
):
  h_muons_            (ctx.get_handle<std::vector<Muon>>(muons_handle_name)),
  h_muontrk_weight_      (ctx.declare_event_output<float>("weight_sfmu_" + weight_postfix)),
  h_muontrk_weight_up_   (ctx.declare_event_output<float>("weight_sfmu_" + weight_postfix + "_up")),
  h_muontrk_weight_down_ (ctx.declare_event_output<float>("weight_sfmu_" + weight_postfix + "_down")),
  sys_error_factor_(sys_error_percantage/100.)
{
  auto dataset_type = ctx.get("dataset_type");
  bool is_mc = dataset_type == "MC";
  if (!is_mc) {
    cout << "Warning: MCMuonTrkScaleFactor will not have an effect on "
    <<" this non-MC sample (dataset_type = '" + dataset_type + "')" << endl;
    return;
  }


  std::ifstream in;
  in.open(sf_file_path.c_str());
  Int_t nlines = 0;
  double eta_min,eta_max,eta_ave,factor, err_dn, err_up;

  eta_min_ = 5;
  eta_max_ = -5;
  while (1) {
    in >> eta_min >> eta_ave >> eta_max >> factor >> err_dn >> err_up;
    if (!in.good()) break;
    eta_.push_back(eta_max);
    SFs_.push_back(factor);
    SFs_err_dn_.push_back(err_dn);
    SFs_err_up_.push_back(err_up);
    if(eta_min_>eta_min) eta_min_ = eta_min;
    if(eta_max_<eta_max) eta_max_ = eta_max;
    nlines++;
  }
  in.close();
  //printf("MuonTrk SF: found %d points\n",nlines);

  sys_direction_ = 0;
  if (sys_uncert == "up") {
    sys_direction_ = 1;
  } else if (sys_uncert == "down") {
    sys_direction_ = -1;
  }
}

bool MCMuonTrkScaleFactor::process(uhh2::Event & event) {

  // if (!sf_hist_) {
  event.set(h_muontrk_weight_,       1.);
  event.set(h_muontrk_weight_up_,    1.);
  event.set(h_muontrk_weight_down_,  1.);
  //   return true;
  // }

  const auto & muons = event.get(h_muons_);
  float weight = 1., weight_up = 1., weight_down = 1.;
  for (const auto & mu : muons) {
    float eta = fabs(mu.eta());
    float pt = mu.pt();
    if(pt<10.){
      std::cout<<"For muons with pt<10 another input file with SFs should be used!"<<std::endl;
      return true;
    }

    if (eta_min_ < eta && eta_max_ > eta){
      //      bool out_of_range = false;
      for(unsigned int i=1;i<eta_.size();i++){
        if(eta<=eta_[i] && eta>=eta_[i-1]){
          float w = 0.01*SFs_[i];
          float err_dn =  0.01*SFs_err_dn_[i];
          float err_up =  0.01*SFs_err_up_[i];
          weight      *= w;
          weight_up   *= w + fabs(err_up);
          weight_down *= w - fabs(err_dn);
        }
      }
    }
  }
  event.set(h_muontrk_weight_,       weight);
  event.set(h_muontrk_weight_up_,    weight_up);
  event.set(h_muontrk_weight_down_,  weight_down);

  if (sys_direction_ == 1) {
    event.weight *= weight_up;
  } else if (sys_direction_ == -1) {
    event.weight *= weight_down;
  } else {
    event.weight *= weight;
  }

  return true;
}

//____________________________________________________________________________________________________
MCElecScaleFactor::MCElecScaleFactor(
  uhh2::Context & ctx,
  const std::string & sf_file_path,
  float sys_error_percantage,
  const std::string & weight_postfix,
  const std::string & sys_uncert,
  const std::string & elecs_handle_name,
  const std::string & sf_name,
  const bool absolute_eta
):
  h_elecs_            (ctx.get_handle<std::vector<Electron>>(elecs_handle_name)),
  h_elec_weight_      (ctx.declare_event_output<float>("weight_sfelec_" + weight_postfix)),
  h_elec_weight_up_   (ctx.declare_event_output<float>("weight_sfelec_" + weight_postfix + "_up")),
  h_elec_weight_down_ (ctx.declare_event_output<float>("weight_sfelec_" + weight_postfix + "_down")),
  sys_error_factor_(sys_error_percantage/100.),
  fAbsoluteEta(absolute_eta)
{
  auto dataset_type = ctx.get("dataset_type");
  bool is_mc = dataset_type == "MC";
  if (!is_mc) {
    cout << "Warning: MCElecScaleFactor will not have an effect on "
    <<" this non-MC sample (dataset_type = '" + dataset_type + "')" << endl;
    return;
  }

  TFile sf_file(locate_file(sf_file_path).c_str());
  if (sf_file.IsZombie()) {
    throw runtime_error("Scale factor file for electrons not found: " + sf_file_path);
  }

  sf_hist_.reset((TH2*) sf_file.Get(sf_name.c_str()));
  if (!sf_hist_.get()) {
    throw runtime_error("Electron scale factor histogram not found in file");
  }
  sf_hist_->SetDirectory(0);
  eta_min_ = sf_hist_->GetXaxis()->GetXmin();
  eta_max_ = sf_hist_->GetXaxis()->GetXmax();
  pt_min_  = sf_hist_->GetYaxis()->GetXmin();
  pt_max_  = sf_hist_->GetYaxis()->GetXmax();

  sys_direction_ = 0;
  if (sys_uncert == "up") {
    sys_direction_ = 1;
  } else if (sys_uncert == "down") {
    sys_direction_ = -1;
  }
}

bool MCElecScaleFactor::process(uhh2::Event & event) {

  if (!sf_hist_) {
    event.set(h_elec_weight_,       1.);
    event.set(h_elec_weight_up_,    1.);
    event.set(h_elec_weight_down_,  1.);
    return true;
  }

  const auto & elecs = event.get(h_elecs_);
  float weight = 1., weight_up = 1., weight_down = 1.;
  float err_propagation = 0.;
  for (const auto & el : elecs) {
    float eta = fAbsoluteEta ? fabs(el.supercluster_eta()) : el.supercluster_eta();
    float pt = el.pt();
    if (eta_min_ < eta && eta_max_ > eta){
      bool out_of_range = false;
      //take scale factor from the last measured pT bin in case of too large/small pT
      if(pt_min_ >= pt) {
        pt=pt_min_+0.0001;
        out_of_range = true;
      }
      if(pt_max_ <= pt) {
        pt=pt_max_-0.0001;
        out_of_range = true;
      }
      int bin       = sf_hist_->FindFixBin(eta, pt);
      float w       = sf_hist_->GetBinContent(bin);
      float err     = sf_hist_->GetBinError(bin);

      if (w==1 and err==0) {
        if (bin==1) bin +=1;
        else bin -= 1;
        w   = sf_hist_->GetBinContent(bin);
        err = sf_hist_->GetBinError(bin);
      }

      float err_tot = sqrt(err*err + pow(w*sys_error_factor_, 2));
      //take twice the uncertainty if the pT is outside the measured pT range
      if(out_of_range) err_tot*=2;

      weight      *= w;
      err_propagation += pow(err_tot/w, 2);
    }

  }

  weight_up = weight + sqrt(err_propagation);
  weight_down = weight - sqrt(err_propagation);

  event.set(h_elec_weight_,       weight);
  event.set(h_elec_weight_up_,    weight_up);
  event.set(h_elec_weight_down_,  weight_down);

  if (sys_direction_ == 1) {
    event.weight *= weight_up;
  } else if (sys_direction_ == -1) {
    event.weight *= weight_down;
  } else {
    event.weight *= weight;
  }

  return true;
}

//____________________________________________________________________________________________________
MCBTagScaleFactor::MCBTagScaleFactor(
  uhh2::Context & ctx,
  const BTag::algo algo,
  const BTag::wp wp,
  const std::string & jets_handle_name,
  const std::string & measType_bc,
  const std::string & measType_udsg,
  const std::string & xml_config_eff_file,
  const std::string & weights_name_postfix,
  const std::string & sf_version
):
  fYear(extract_year(ctx)),
  fBTagID(BTag(algo, wp)),
  fHandle_jets(ctx.get_handle<std::vector<Jet>>(jets_handle_name)),
  fHandle_topjets(ctx.get_handle<std::vector<TopJet>>(jets_handle_name))
{
  fUncerts[FlavType::BC] = fUncerts_per_measType.at(measType_bc);
  fUncerts[FlavType::LIGHT] = fUncerts_per_measType.at(measType_udsg);

  const string weights_name = "weight_btag"+(weights_name_postfix.empty() ? "_" : "_"+weights_name_postfix+"_");
  fHandle_weight_central = ctx.declare_event_output<float>(weights_name+"central");
  for(const auto & ft : kFlavTypes) {
    for(const string & uncert : fUncerts.at(ft.first)) {
      fHandles_weights[ft.first][uncert] = ctx.declare_event_output<float>(weights_name+ft.second.name+"_"+uncert);
    }
  }

  bool invalid_variation = false;
  const string sysType = ctx.get(fSystDirectionConfigName, "central");
  const auto tokens = ((TString)sysType).Tokenize("_");
  for(const auto & ft : kFlavTypes) {
    if(((TObjString*)tokens->First())->GetString() == ft.second.name) fSysType_ft = ft.first;
  }
  if(fSysType_ft != FlavType::NONE) {
    fSysType_uncert = ((TObjString*)(tokens->At(1)))->GetString();
    for(Int_t i = 2; i <= tokens->GetLast(); ++i) fSysType_uncert += "_"+((TObjString*)(tokens->At(i)))->GetString();
    const auto & v = fUncerts.at(fSysType_ft);
    if(std::find(v.begin(), v.end(), fSysType_uncert) == v.end()) invalid_variation = true;
  }
  else {
    if(sysType != "central" && sysType != "nominal") invalid_variation = true;
  }
  if(invalid_variation) throw invalid_argument("MCBTagScaleFactor: The variation '"+sysType+"' that you request for application is invalid");

  const string dataset_type = ctx.get("dataset_type");
  const bool is_mc = dataset_type == "MC";
  if(!is_mc) {
    cout << "Warning: MCBTagScaleFactor will only setup dummy weights for this non-MC sample (dataset_type = '" + dataset_type + "')" << endl;
    return;
  }

  string sf_file_path = (string)getenv("CMSSW_BASE")+"/src/UHH2/common/UHH2-data/btagging_SFs_UL/";
  switch(algo) {
    case BTag::DEEPCSV :
    switch(fYear) {
      case Year::isUL16preVFP :
      sf_file_path += "UL16preVFP/wp_deepCSV_"+sf_version+".csv";
      break;
      case Year::isUL16postVFP :
      sf_file_path += "UL16postVFP/wp_deepCSV_"+sf_version+".csv";
      break;
      case Year::isUL17 :
      sf_file_path += "UL17/wp_deepCSV_"+sf_version+".csv";
      break;
      case Year::isUL18 :
      sf_file_path += "UL18/wp_deepCSV_"+sf_version+".csv";
      break;
      default :
      throw invalid_argument("MCBTagScaleFactor: Invalid year");
    }
    break;
    case BTag::DEEPJET :
    switch(fYear) {
      case Year::isUL16preVFP :
      sf_file_path += "UL16preVFP/wp_deepJet_"+sf_version+".csv";
      break;
      case Year::isUL16postVFP :
      sf_file_path += "UL16postVFP/wp_deepJet_"+sf_version+".csv";
      break;
      case Year::isUL17 :
      sf_file_path += "UL17/wp_deepJet_"+sf_version+".csv";
      break;
      case Year::isUL18 :
      sf_file_path += "UL18/wp_deepJet_"+sf_version+".csv";
      break;
      default :
      throw invalid_argument("MCBTagScaleFactor: Invalid year");
    }
    break;
    default :
    throw invalid_argument("MCBTagScaleFactor: Invalid b-tagging algorithm. This module only works with DeepJet or DeepCSV");
  }
  if(ctx.has(fCalibrationFileConfigName)) sf_file_path = ctx.get(fCalibrationFileConfigName);

  BTagCalib::OperatingPoint op = BTagCalib::OperatingPoint::UNDEFINED;
  switch(fBTagID.GetWorkingPoint()) {
    case BTag::WP_LOOSE :
    op = BTagCalib::OperatingPoint::LOOSE;
    break;
    case BTag::WP_MEDIUM :
    op = BTagCalib::OperatingPoint::MEDIUM;
    break;
    case BTag::WP_TIGHT :
    op = BTagCalib::OperatingPoint::TIGHT;
    break;
    default :
    throw invalid_argument("MCBTagScaleFactor: Invalid working point");
  }
  fReaders[FlavType::BC].reset(new BTagCalib::Reader(locate_file(sf_file_path), op, measType_bc, fUncerts.at(FlavType::BC)));
  fReaders[FlavType::LIGHT].reset(new BTagCalib::Reader(locate_file(sf_file_path), op, measType_udsg, fUncerts.at(FlavType::LIGHT)));

  TFile eff_file(locate_file(ctx.get(xml_config_eff_file)).c_str());
  if(eff_file.IsZombie()) {
    throw runtime_error("MCBTagScaleFactor: MC-efficiencies ROOT file '" + dataset_type + "' is zombie");
  }
  // You need to provide a ROOT file with three histograms with these names containing MC b-tagging efficiencies:
  fEffHists[BTagCalib::JetFlavor::FLAV_B].reset((TH2*)eff_file.Get("BTagMCEffFlavBEff"));
  fEffHists[BTagCalib::JetFlavor::FLAV_C].reset((TH2*)eff_file.Get("BTagMCEffFlavCEff"));
  fEffHists[BTagCalib::JetFlavor::FLAV_UDSG].reset((TH2*)eff_file.Get("BTagMCEffFlavUDSGEff"));
  for(auto & hist : fEffHists) hist.second->SetDirectory(0);
  eff_file.Close();
}

bool MCBTagScaleFactor::process(Event & event) {

  float weight_central = 1.f;

  if(event.isRealData) {
    event.set(fHandle_weight_central, weight_central);
    for(const auto & ft : kFlavTypes) {
      for(const string & uncert : fUncerts.at(ft.first)) {
        event.set(fHandles_weights[ft.first][uncert], weight_central);
      }
    }
    return true;
  }

  map<FlavType, float> weights_central;
  for(const auto & ft : kFlavTypes) {
    const float w = get_btag_weight(event, ft.first, "central");
    weights_central[ft.first] = w;
    weight_central *= w;
  }
  event.set(fHandle_weight_central, weight_central);
  for(const auto & ft : kFlavTypes) {
    for(const string & uncert : fUncerts.at(ft.first)) {
      event.set(fHandles_weights[ft.first][uncert], weight_central / weights_central.at(ft.first) * get_btag_weight(event, ft.first, uncert));
    }
  }

  if(fSysType_ft == FlavType::NONE) event.weight *= weight_central;
  else {
    event.weight *= event.get(fHandles_weights.at(fSysType_ft).at(fSysType_uncert));
  }
  return true;
}

float MCBTagScaleFactor::get_btag_weight(const Event & event, const FlavType & ft, const string & sys) const {
  float mcTag = 1.f;
  float dataTag = 1.f;
  float mcNoTag = 1.f;
  float dataNoTag = 1.f;

  vector<Jet> jets;
  if(event.is_valid(fHandle_jets)) jets = event.get(fHandle_jets);
  else {
    assert(event.is_valid(fHandle_topjets));
    for(const TopJet & tj : event.get(fHandle_topjets)) {
      for(const Jet & sj : tj.subjets()) jets.push_back(sj);
    }
  }

  for(size_t ijet = 0; ijet < jets.size(); ijet++) {
    const Jet jet = jets.at(ijet);
    const float jet_pt = jet.pt();
    const float jet_eta = jet.eta();
    if(jet_pt <= 20.f || fabs(jet_eta) >= 2.5f) continue; // UL: b-tagging possible for eta up to 2.5 // ">=" is correct, not ">"!

    BTagCalib::JetFlavor jet_flavor = BTagCalib::JetFlavor::FLAV_UDSG;
    if(abs(jet.hadronFlavour()) == 5) jet_flavor = BTagCalib::JetFlavor::FLAV_B;
    else if(abs(jet.hadronFlavour()) == 4) jet_flavor = BTagCalib::JetFlavor::FLAV_C;
    if(ft == FlavType::BC && jet_flavor == BTagCalib::JetFlavor::FLAV_UDSG) continue;
    else if(ft == FlavType::LIGHT && jet_flavor != BTagCalib::JetFlavor::FLAV_UDSG) continue;

    const TH2 *eff_hist = fEffHists.at(jet_flavor).get();
    const auto eff_pt_axis = eff_hist->GetXaxis();
    const float pt_low_edge = eff_pt_axis->GetBinLowEdge(eff_pt_axis->GetFirst());
    const float pt_high_edge = eff_pt_axis->GetBinUpEdge(eff_pt_axis->GetLast());
    const float pt_for_eff = max(pt_low_edge+0.001f, min(pt_high_edge-0.001f, jet_pt));
    const float eff = eff_hist->GetBinContent(eff_hist->FindFixBin(pt_for_eff, fabs(jet_eta)));

    const float sf = fReaders.at(ft)->Evaluate(sys, jet_flavor, jet_eta, jet_pt);

    if(fBTagID(jet, event)) {
      mcTag *= eff;
      dataTag *= eff*sf;
    }
    else {
      mcNoTag *= (1.f - eff);
      dataNoTag *= (1.f - eff*sf);
    }
  }

  // Formula as given in https://twiki.cern.ch/twiki/bin/view/CMS/BTagSFMethods#1a_Event_reweighting_using_scale
  const float w = (dataNoTag * dataTag) / (mcNoTag * mcTag);
  if(isnan(w)) throw runtime_error("MCBTagScaleFactor::get_btag_weight(): Weight is NaN. Possible cause: Content of MC efficiency bin is 0 or exactly 1 and causes division by zero in weight formula");
  return w;
}

//____________________________________________________________________________________________________
#define WEIGHTS_NAME_BTAGDISC "weight_btagdisc_"+(weights_name_postfix.empty() ? "" : weights_name_postfix+"_")

MCBTagDiscriminantReweighting::MCBTagDiscriminantReweighting(
  uhh2::Context & ctx,
  BTag::algo algorithm,
  const std::string & jets_handle_name,
  const std::string & weights_name_postfix,
  const std::string & measType,
  const std::string & sf_version
):
  fAlgorithm(algorithm),
  fYear(extract_year(ctx)),
  h_jets(ctx.get_handle<std::vector<Jet>>(jets_handle_name)),
  h_weight_central (ctx.declare_event_output<float>(WEIGHTS_NAME_BTAGDISC+"central")),
  h_weight_cferr1_up (ctx.declare_event_output<float>(WEIGHTS_NAME_BTAGDISC+"cferr1_up")),
  h_weight_cferr1_down (ctx.declare_event_output<float>(WEIGHTS_NAME_BTAGDISC+"cferr1_down")),
  h_weight_cferr2_up (ctx.declare_event_output<float>(WEIGHTS_NAME_BTAGDISC+"cferr2_up")),
  h_weight_cferr2_down (ctx.declare_event_output<float>(WEIGHTS_NAME_BTAGDISC+"cferr2_down")),
  h_weight_lf_up (ctx.declare_event_output<float>(WEIGHTS_NAME_BTAGDISC+"lf_up")),
  h_weight_lf_down (ctx.declare_event_output<float>(WEIGHTS_NAME_BTAGDISC+"lf_down")),
  h_weight_lfstats1_up (ctx.declare_event_output<float>(WEIGHTS_NAME_BTAGDISC+"lfstats1_up")),
  h_weight_lfstats1_down (ctx.declare_event_output<float>(WEIGHTS_NAME_BTAGDISC+"lfstats1_down")),
  h_weight_lfstats2_up (ctx.declare_event_output<float>(WEIGHTS_NAME_BTAGDISC+"lfstats2_up")),
  h_weight_lfstats2_down (ctx.declare_event_output<float>(WEIGHTS_NAME_BTAGDISC+"lfstats2_down")),
  h_weight_hf_up (ctx.declare_event_output<float>(WEIGHTS_NAME_BTAGDISC+"hf_up")),
  h_weight_hf_down (ctx.declare_event_output<float>(WEIGHTS_NAME_BTAGDISC+"hf_down")),
  h_weight_hfstats1_up (ctx.declare_event_output<float>(WEIGHTS_NAME_BTAGDISC+"hfstats1_up")),
  h_weight_hfstats1_down (ctx.declare_event_output<float>(WEIGHTS_NAME_BTAGDISC+"hfstats1_down")),
  h_weight_hfstats2_up (ctx.declare_event_output<float>(WEIGHTS_NAME_BTAGDISC+"hfstats2_up")),
  h_weight_hfstats2_down (ctx.declare_event_output<float>(WEIGHTS_NAME_BTAGDISC+"hfstats2_down"))
{
  const string dataset_type = ctx.get("dataset_type");
  const bool is_mc = dataset_type == "MC";
  if(!is_mc) {
    cout << "Warning: MCBTagDiscriminantReweighting will only setup dummy weights for this non-MC sample (dataset_type = '" + dataset_type + "')" << endl;
    return;
  }

  const string sysType = ctx.get(fSystDirectionConfigName, "central");
  if(sysType == "central" || sysType == "nominal") fSysType = SysType::central;
  else if(sysType == "cferr1_up") fSysType = SysType::cferr1_up;
  else if(sysType == "cferr1_down") fSysType = SysType::cferr1_down;
  else if(sysType == "cferr2_up") fSysType = SysType::cferr2_up;
  else if(sysType == "cferr2_down") fSysType = SysType::cferr2_down;
  else if(sysType == "lf_up") fSysType = SysType::lf_up;
  else if(sysType == "lf_down") fSysType = SysType::lf_down;
  else if(sysType == "lfstats1_up") fSysType = SysType::lfstats1_up;
  else if(sysType == "lfstats1_down") fSysType = SysType::lfstats1_down;
  else if(sysType == "lfstats2_up") fSysType = SysType::lfstats2_up;
  else if(sysType == "lfstats2_down") fSysType = SysType::lfstats2_down;
  else if(sysType == "hf_up") fSysType = SysType::hf_up;
  else if(sysType == "hf_down") fSysType = SysType::hf_down;
  else if(sysType == "hfstats1_up") fSysType = SysType::hfstats1_up;
  else if(sysType == "hfstats1_down") fSysType = SysType::hfstats1_down;
  else if(sysType == "hfstats2_up") fSysType = SysType::hfstats2_up;
  else if(sysType == "hfstats2_down") fSysType = SysType::hfstats2_down;
  else if(sysType.find("jes") != string::npos) throw invalid_argument((string)"MCBTagDiscriminantReweighting: You specified a JES uncertainty source ('"+sysType+"') as systematic variation. The JES-varied b-tagging SFs are supposed to be applied jointly with the corresponding JEC variation and not as separate SF systematic!");
  else throw invalid_argument((string)"MCBTagDiscriminantReweighting: Invalid sysType '"+sysType+"'");

  set<string> to_be_loaded = {
    "up_cferr1", "down_cferr1",
    "up_cferr2", "down_cferr2",
    "up_lf", "down_lf",
    "up_lfstats1", "down_lfstats1",
    "up_lfstats2", "down_lfstats2",
    "up_hf", "down_hf",
    "up_hfstats1", "down_hfstats1",
    "up_hfstats2", "down_hfstats2",
  };

  const string jecsmear_direction = ctx.get("jecsmear_direction");
  const string jecsmear_source = ctx.get("jecsmear_source", "Total");
  if(jecsmear_direction == "nominal") {
    fCentralOrJES = "central";
  }
  // If JEC variation is applied, we do not apply the central b-tagging SF but the corresponding "up/down_jes..." SF.
  // If you specify a JEC uncertainty sub-source via "jecsmear_source" in your config XML, the correct corresponding b-tagging SF will be used.
  // In order to get an overview of which splitted JEC uncertainty sources are available, you should look into the UncertaintySources JEC files and the b-tagging SF files.
  // If you specify a JEC uncertainty source that is not supported, i.e. not given in the b-tagging SF files or just not loaded, BTagCalib::Reader::Evaluate will fail loudly.
  // By default, the "Total" JEC uncertainty is used.
  else if(jecsmear_direction == "up") {
    fCentralOrJES = "up_jes";
    if(jecsmear_source != "Total") fCentralOrJES += jecsmear_source;
    to_be_loaded.insert(fCentralOrJES);
  }
  else if(jecsmear_direction == "down") {
    fCentralOrJES = "down_jes";
    if(jecsmear_source != "Total") fCentralOrJES += jecsmear_source;
    to_be_loaded.insert(fCentralOrJES);
  }
  else {
    throw invalid_argument("MCBTagDiscriminantReweighting: Invalid jecsmear_direction '"+jecsmear_direction+"'");
  }
  if(jecsmear_direction != "nominal" && fSysType != SysType::central) throw runtime_error("MCBTagDiscriminantReweighting: Not allowed to vary JES and vary b-tagging SFs simultaneously");

  string sf_file_path = (string)getenv("CMSSW_BASE")+"/src/UHH2/common/UHH2-data/btagging_SFs_UL/";
  switch(fAlgorithm) {
    case BTag::DEEPCSV :
    switch(fYear) {
      case Year::isUL16preVFP :
      sf_file_path += "UL16preVFP/reshaping_deepCSV_"+sf_version+".csv";
      break;
      case Year::isUL16postVFP :
      sf_file_path += "UL16postVFP/reshaping_deepCSV_"+sf_version+".csv";
      break;
      case Year::isUL17 :
      sf_file_path += "UL17/reshaping_deepCSV_"+sf_version+".csv";
      break;
      case Year::isUL18 :
      sf_file_path += "UL18/reshaping_deepCSV_"+sf_version+".csv";
      break;
      default :
      throw invalid_argument("MCBTagDiscriminantReweighting: Invalid year");
    }
    break;
    case BTag::DEEPJET :
    switch(fYear) {
      case Year::isUL16preVFP :
      sf_file_path += "UL16preVFP/reshaping_deepJet_"+sf_version+".csv";
      break;
      case Year::isUL16postVFP :
      sf_file_path += "UL16postVFP/reshaping_deepJet_"+sf_version+".csv";
      break;
      case Year::isUL17 :
      sf_file_path += "UL17/reshaping_deepJet_"+sf_version+".csv";
      break;
      case Year::isUL18 :
      sf_file_path += "UL18/reshaping_deepJet_"+sf_version+".csv";
      break;
      default :
      throw invalid_argument("MCBTagDiscriminantReweighting: Invalid year");
    }
    break;
    default :
    throw invalid_argument("MCBTagDiscriminantReweighting: Invalid b-tagging algorithm. This module only works with DeepJet or DeepCSV");
  }
  if(ctx.has(fCalibrationFileConfigName)) sf_file_path = ctx.get(fCalibrationFileConfigName);

  fReader.reset(new BTagCalib::Reader(locate_file(sf_file_path), BTagCalib::OperatingPoint::SHAPE, measType, to_be_loaded));
}

bool MCBTagDiscriminantReweighting::process(Event & event) {

  float weight_central = 1.0;
  float weight_cferr1_up = 1.0;
  float weight_cferr1_down = 1.0;
  float weight_cferr2_up = 1.0;
  float weight_cferr2_down = 1.0;
  float weight_lf_up = 1.0;
  float weight_lf_down = 1.0;
  float weight_lfstats1_up = 1.0;
  float weight_lfstats1_down = 1.0;
  float weight_lfstats2_up = 1.0;
  float weight_lfstats2_down = 1.0;
  float weight_hf_up = 1.0;
  float weight_hf_down = 1.0;
  float weight_hfstats1_up = 1.0;
  float weight_hfstats1_down = 1.0;
  float weight_hfstats2_up = 1.0;
  float weight_hfstats2_down = 1.0;

  if(event.isRealData) {
    event.set(h_weight_central, weight_central);
    event.set(h_weight_cferr1_up, weight_cferr1_up);
    event.set(h_weight_cferr1_down, weight_cferr1_down);
    event.set(h_weight_cferr2_up, weight_cferr2_up);
    event.set(h_weight_cferr2_down, weight_cferr2_down);
    event.set(h_weight_lf_up, weight_lf_up);
    event.set(h_weight_lf_down, weight_lf_down);
    event.set(h_weight_lfstats1_up, weight_lfstats1_up);
    event.set(h_weight_lfstats1_down, weight_lfstats1_down);
    event.set(h_weight_lfstats2_up, weight_lfstats2_up);
    event.set(h_weight_lfstats2_down, weight_lfstats2_down);
    event.set(h_weight_hf_up, weight_hf_up);
    event.set(h_weight_hf_down, weight_hf_down);
    event.set(h_weight_hfstats1_up, weight_hfstats1_up);
    event.set(h_weight_hfstats1_down, weight_hfstats1_down);
    event.set(h_weight_hfstats2_up, weight_hfstats2_up);
    event.set(h_weight_hfstats2_down, weight_hfstats2_down);
    return true;
  }

  const auto & jets = event.get(h_jets);
  for(size_t ijet = 0; ijet < jets.size(); ijet++) {
    const Jet jet = jets.at(ijet);
    const float jet_pt = jet.pt();
    const float jet_eta = jet.eta();

    float jet_btagdisc;
    if(fAlgorithm == BTag::DEEPCSV) jet_btagdisc = jet.btag_DeepCSV();
    else if(fAlgorithm == BTag::DEEPJET) jet_btagdisc = jet.btag_DeepJet();
    else throw runtime_error("MCBTagDiscriminantReweighting::process(): Invalid b-tagging algorithm. This module only works with DeepJet or DeepCSV");

    if(jet_btagdisc < 0.0) jet_btagdisc = -0.05;
    else if(jet_btagdisc > 1.0) jet_btagdisc = 1.0;

    BTagCalib::JetFlavor jet_flavor = BTagCalib::JetFlavor::FLAV_UDSG;
    if(abs(jet.hadronFlavour()) == 5) jet_flavor = BTagCalib::JetFlavor::FLAV_B;
    else if(abs(jet.hadronFlavour()) == 4) jet_flavor = BTagCalib::JetFlavor::FLAV_C;

    if(jet_pt > 20.0 && fabs(jet_eta) < 2.5) { // UL: b-tagging possible for eta up to 2.5

      if(jet_flavor == BTagCalib::JetFlavor::FLAV_C) {
        // For c jets, no dedicated JES SFs are available and the "central" SF is always used for the "central" weight.
        const float w_central = fReader->Evaluate("central", jet_flavor, jet_eta, jet_pt, jet_btagdisc);

        weight_central *= w_central;

        weight_cferr1_up *= fReader->Evaluate("up_cferr1", jet_flavor, jet_eta, jet_pt, jet_btagdisc);
        weight_cferr1_down *= fReader->Evaluate("down_cferr1", jet_flavor, jet_eta, jet_pt, jet_btagdisc);
        weight_cferr2_up *= fReader->Evaluate("up_cferr2", jet_flavor, jet_eta, jet_pt, jet_btagdisc);
        weight_cferr2_down *= fReader->Evaluate("down_cferr2", jet_flavor, jet_eta, jet_pt, jet_btagdisc);

        weight_lf_up *= w_central;
        weight_lf_down *= w_central;
        weight_lfstats1_up *= w_central;
        weight_lfstats1_down *= w_central;
        weight_lfstats2_up *= w_central;
        weight_lfstats2_down *= w_central;

        weight_hf_up *= w_central;
        weight_hf_down *= w_central;
        weight_hfstats1_up *= w_central;
        weight_hfstats1_down *= w_central;
        weight_hfstats2_up *= w_central;
        weight_hfstats2_down *= w_central;
      }

      else if(jet_flavor == BTagCalib::JetFlavor::FLAV_B || jet_flavor == BTagCalib::JetFlavor::FLAV_UDSG) {
        // For b and light jets, the "central" weight is either the "central" SF or a dedicated JES variation SF (if jecsmear_direction != nominal).
        const float w_central = fReader->Evaluate(fCentralOrJES, jet_flavor, jet_eta, jet_pt, jet_btagdisc);

        weight_central *= w_central;

        weight_cferr1_up *= w_central;
        weight_cferr1_down *= w_central;
        weight_cferr2_up *= w_central;
        weight_cferr2_down *= w_central;

        // lf(stats1/2) and hf(stats1/2) variatons must be applied to both FLAV_B and FLAV_UDSG jets!
        weight_lf_up *= fReader->Evaluate("up_lf", jet_flavor, jet_eta, jet_pt, jet_btagdisc);
        weight_lf_down *= fReader->Evaluate("down_lf", jet_flavor, jet_eta, jet_pt, jet_btagdisc);
        weight_lfstats1_up *= fReader->Evaluate("up_lfstats1", jet_flavor, jet_eta, jet_pt, jet_btagdisc);
        weight_lfstats1_down *= fReader->Evaluate("down_lfstats1", jet_flavor, jet_eta, jet_pt, jet_btagdisc);
        weight_lfstats2_up *= fReader->Evaluate("up_lfstats2", jet_flavor, jet_eta, jet_pt, jet_btagdisc);
        weight_lfstats2_down *= fReader->Evaluate("down_lfstats2", jet_flavor, jet_eta, jet_pt, jet_btagdisc);

        weight_hf_up *= fReader->Evaluate("up_hf", jet_flavor, jet_eta, jet_pt, jet_btagdisc);
        weight_hf_down *= fReader->Evaluate("down_hf", jet_flavor, jet_eta, jet_pt, jet_btagdisc);
        weight_hfstats1_up *= fReader->Evaluate("up_hfstats1", jet_flavor, jet_eta, jet_pt, jet_btagdisc);
        weight_hfstats1_down *= fReader->Evaluate("down_hfstats1", jet_flavor, jet_eta, jet_pt, jet_btagdisc);
        weight_hfstats2_up *= fReader->Evaluate("up_hfstats2", jet_flavor, jet_eta, jet_pt, jet_btagdisc);
        weight_hfstats2_down *= fReader->Evaluate("down_hfstats2", jet_flavor, jet_eta, jet_pt, jet_btagdisc);
      }
    }
  }

  event.set(h_weight_central, weight_central);
  event.set(h_weight_cferr1_up, weight_cferr1_up);
  event.set(h_weight_cferr1_down, weight_cferr1_down);
  event.set(h_weight_cferr2_up, weight_cferr2_up);
  event.set(h_weight_cferr2_down, weight_cferr2_down);
  event.set(h_weight_lf_up, weight_lf_up);
  event.set(h_weight_lf_down, weight_lf_down);
  event.set(h_weight_lfstats1_up, weight_lfstats1_up);
  event.set(h_weight_lfstats1_down, weight_lfstats1_down);
  event.set(h_weight_lfstats2_up, weight_lfstats2_up);
  event.set(h_weight_lfstats2_down, weight_lfstats2_down);
  event.set(h_weight_hf_up, weight_hf_up);
  event.set(h_weight_hf_down, weight_hf_down);
  event.set(h_weight_hfstats1_up, weight_hfstats1_up);
  event.set(h_weight_hfstats1_down, weight_hfstats1_down);
  event.set(h_weight_hfstats2_up, weight_hfstats2_up);
  event.set(h_weight_hfstats2_down, weight_hfstats2_down);

  if(fSysType == SysType::cferr1_up) event.weight *= weight_cferr1_up;
  else if(fSysType == SysType::cferr1_down) event.weight *= weight_cferr1_down;
  else if(fSysType == SysType::cferr2_up) event.weight *= weight_cferr2_up;
  else if(fSysType == SysType::cferr2_down) event.weight *= weight_cferr2_down;
  else if(fSysType == SysType::lf_up) event.weight *= weight_lf_up;
  else if(fSysType == SysType::lf_down) event.weight *= weight_lf_down;
  else if(fSysType == SysType::lfstats1_up) event.weight *= weight_lfstats1_up;
  else if(fSysType == SysType::lfstats1_down) event.weight *= weight_lfstats1_down;
  else if(fSysType == SysType::lfstats2_up) event.weight *= weight_lfstats2_up;
  else if(fSysType == SysType::lfstats2_down) event.weight *= weight_lfstats2_down;
  else if(fSysType == SysType::hf_up) event.weight *= weight_hf_up;
  else if(fSysType == SysType::hf_down) event.weight *= weight_hf_down;
  else if(fSysType == SysType::hfstats1_up) event.weight *= weight_hfstats1_up;
  else if(fSysType == SysType::hfstats1_down) event.weight *= weight_hfstats1_down;
  else if(fSysType == SysType::hfstats2_up) event.weight *= weight_hfstats2_up;
  else if(fSysType == SysType::hfstats2_down) event.weight *= weight_hfstats2_down;
  else event.weight *= weight_central;

  return true;
}

//____________________________________________________________________________________________________
TauEffVariation::TauEffVariation(Context & ctx){

  auto dataset_type = ctx.get("dataset_type");
  bool is_mc = dataset_type == "MC";
  if(!is_mc){
    cout << "Warning: TauIdVariation will not have an effect on this non-MC sample (dataset_type = '" + dataset_type + "')" << endl;
    return;
  }
  auto s_TauEff = ctx.get("TauIdVariation");

  if(s_TauEff == "up") {i_TauEff = 1;}
  else if(s_TauEff == "down"){i_TauEff = 2;}

}
bool TauEffVariation::process(Event & event){
  if (event.isRealData) return true;

  std::vector<Tau> real_taus;
  for(unsigned int j=0; j<event.taus->size(); ++j)
  {
    bool real = false;
    Tau tau = event.taus->at(j);
    try{
      for(unsigned int i=0; i<event.genparticles->size(); ++i)
      {
        GenParticle genp = event.genparticles->at(i);
        double dr = deltaR(genp,tau);
        if (dr < 0.4 && abs(genp.pdgId())==15) real = true;
      }
    }
    catch(const std::runtime_error& error){
      std::cout<<"Problem with genparticles in MCWeight.cxx"<<std::endl;
      std::cout<<error.what();
    }
    if(real) real_taus.push_back(tau);
  }
  for(unsigned int i=0; i<real_taus.size(); ++i)
  {
    Tau realtau = real_taus.at(i);
    if (i_TauEff==0)
    {
      event.weight *= SF_TauId;
    }
    if (i_TauEff==1)
    {
      event.weight *= (SF_TauId+0.10)+0.2*realtau.pt()/1000;
    }
    if (i_TauEff==2)
    {
      event.weight *= (SF_TauId-0.10)-0.2*realtau.pt()/1000;
    }
  }
  return true;
}

//____________________________________________________________________________________________________
TauChargeVariation::TauChargeVariation(Context & ctx){
  auto dataset_type = ctx.get("dataset_type");
  bool is_mc = dataset_type == "MC";
  if(!is_mc){
    cout << "Warning: TauChargeVariation will not have an effect on this non-MC sample (dataset_type = '" + dataset_type + "')" << endl;
    return;
  }
  auto s_TauCharge = ctx.get("TauChargeVariation");

  if(s_TauCharge == "up") {i_TauCharge = 1;}
  else if(s_TauCharge == "down"){i_TauCharge = 2;}

}
bool TauChargeVariation::process(Event & event){
  if (event.isRealData) return true;

  for(unsigned int j=0; j<event.taus->size(); ++j)
  {
    Tau tau = event.taus->at(j);
    try{
      for(unsigned int i=0; i<event.genparticles->size(); ++i)
      {
        GenParticle genp = event.genparticles->at(i);
        double dr = deltaR(genp,tau);
        if (dr < 0.4 && abs(genp.pdgId())==15){
          if(tau.charge()!=genp.charge()){
            if(i_TauCharge==0) {return true;}
            if(i_TauCharge==1){
              event.weight *= 1.02;
            }
            if(i_TauCharge==2){
              event.weight *= 0.98;
            }
          }
        }
      }
    }
    catch(const std::runtime_error& error){
      std::cout<<"Problem with genInfo in MCWeight.cxx"<<std::endl;
      std::cout<<error.what();
    }
  }
  return true;
}
