#include "UHH2/common/include/BTagCalibrationStandalone.h"
#include "UHH2/common/include/MCWeight.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/Utils.h"
#include "UHH2/common/include/Utils.h"

#include "Riostream.h"
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

MCScaleVariation::MCScaleVariation(Context & ctx){

  auto dataset_type = ctx.get("dataset_type");
  bool is_mc = dataset_type == "MC";
  h_murmuf_weight_upup_     = ctx.declare_event_output<float>("weight_murmuf_upup");
  h_murmuf_weight_upnone_   = ctx.declare_event_output<float>("weight_murmuf_upnone");
  h_murmuf_weight_noneup_   = ctx.declare_event_output<float>("weight_murmuf_noneup");
  h_murmuf_weight_downdown_ = ctx.declare_event_output<float>("weight_murmuf_downdown");
  h_murmuf_weight_downnone_ = ctx.declare_event_output<float>("weight_murmuf_downnone");
  h_murmuf_weight_nonedown_ = ctx.declare_event_output<float>("weight_murmuf_nonedown");

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

  // Set weights to 1 for data
  if (event.isRealData) {
    event.set(h_murmuf_weight_upup_, 1.);
    event.set(h_murmuf_weight_upnone_, 1.);
    event.set(h_murmuf_weight_noneup_, 1.);
    event.set(h_murmuf_weight_downdown_, 1.);
    event.set(h_murmuf_weight_downnone_, 1.);
    event.set(h_murmuf_weight_nonedown_, 1.);
    return true;
  }

  try{
    // Set weights to 1 if no genInfo scale variation weights exist
    if(event.genInfo->systweights().size() < 9){
      event.set(h_murmuf_weight_upup_, 1.);
      event.set(h_murmuf_weight_upnone_, 1.);
      event.set(h_murmuf_weight_noneup_, 1.);
      event.set(h_murmuf_weight_downdown_, 1.);
      event.set(h_murmuf_weight_downnone_, 1.);
      event.set(h_murmuf_weight_nonedown_, 1.);
      return true;
    }

    // Set handles, written for all relevant cases irrespective of the values of mu_r and mu_f specified in the config file
    event.set(h_murmuf_weight_upup_, event.genInfo->systweights().at(4)/event.genInfo->originalXWGTUP());
    event.set(h_murmuf_weight_upnone_, event.genInfo->systweights().at(3)/event.genInfo->originalXWGTUP());
    event.set(h_murmuf_weight_noneup_, event.genInfo->systweights().at(1)/event.genInfo->originalXWGTUP());
    event.set(h_murmuf_weight_downdown_, event.genInfo->systweights().at(8)/event.genInfo->originalXWGTUP());
    event.set(h_murmuf_weight_downnone_, event.genInfo->systweights().at(6)/event.genInfo->originalXWGTUP());
    event.set(h_murmuf_weight_nonedown_, event.genInfo->systweights().at(2)/event.genInfo->originalXWGTUP());

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
    weight_up   *= w + err_tot;
    weight_down *= w - err_tot;

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
      weight_up   *= w + err_tot;
      weight_down *= w - err_tot;
    }

  }

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


MCBTagScaleFactor::MCBTagScaleFactor(
  uhh2::Context & ctx,
  BTag::algo tagger,
  BTag::wp wp,
  const std::string & jets_handle_name,
  const std::string & sysType,
  const std::string & measType_bc,
  const std::string & measType_udsg,
  const std::string & xml_param_name,
  const std::string & weights_name_postfix,
  const std::string & xml_calib_name
):
  btag_(BTag(tagger, wp)),
  h_jets_(ctx.get_handle<std::vector<Jet>>(jets_handle_name)),
  h_topjets_(ctx.get_handle<std::vector<TopJet>>(jets_handle_name)),
  sysType_(sysType),
  h_btag_weight_          (ctx.declare_event_output<float>("weight_btag"+weights_name_postfix)),
  h_btag_weight_up_       (ctx.declare_event_output<float>("weight_btag_up"+weights_name_postfix)),
  h_btag_weight_down_     (ctx.declare_event_output<float>("weight_btag_down"+weights_name_postfix)),
  h_btag_weight_bc_up_    (ctx.declare_event_output<float>("weight_btag_bc_up"+weights_name_postfix)),
  h_btag_weight_bc_down_  (ctx.declare_event_output<float>("weight_btag_bc_down"+weights_name_postfix)),
  h_btag_weight_udsg_up_  (ctx.declare_event_output<float>("weight_btag_udsg_up"+weights_name_postfix)),
  h_btag_weight_udsg_down_(ctx.declare_event_output<float>("weight_btag_udsg_down"+weights_name_postfix))
{
  auto dataset_type = ctx.get("dataset_type");
  bool is_mc = dataset_type == "MC";
  if (!is_mc) {
    cout << "Warning: MCBTagScaleFactor will not have an effect on "
    <<" this non-MC sample (dataset_type = '" + dataset_type + "')" << endl;
    return;
  }

  TFile eff_file(locate_file(ctx.get(xml_param_name)).c_str());
  if (eff_file.IsZombie()) {
    cout << "Warning: MCBTagScaleFactor will not have an effect because the root-file "
    << "with MC-efficiencies not found: " << ctx.get(xml_param_name) << endl;
    eff_file.Close();
    return;
  }
  eff_b_.reset((TH2*) eff_file.Get("BTagMCEffFlavBEff"));
  eff_c_.reset((TH2*) eff_file.Get("BTagMCEffFlavCEff"));
  eff_udsg_.reset((TH2*) eff_file.Get("BTagMCEffFlavUDSGEff"));
  eff_b_->SetDirectory(0);
  eff_c_->SetDirectory(0);
  eff_udsg_->SetDirectory(0);
  eff_file.Close();

  // https://twiki.cern.ch/twiki/bin/viewauth/CMS/BTagCalibration
  BTagCalibration calib_data(btag_.GetTagger(), ctx.get(xml_calib_name));
  BTagEntry::OperatingPoint op = (BTagEntry::OperatingPoint) btag_.GetWorkingPoint();

  calib_up_.reset(new BTagCalibrationReader(op, "up"));
  calib_.reset(new BTagCalibrationReader(op, "central"));
  calib_down_.reset(new BTagCalibrationReader(op, "down"));

  calib_up_->load(calib_data, BTagEntry::FLAV_B, measType_bc);
  calib_up_->load(calib_data, BTagEntry::FLAV_C, measType_bc);
  calib_up_->load(calib_data, BTagEntry::FLAV_UDSG, measType_udsg);
  calib_->load(calib_data, BTagEntry::FLAV_B, measType_bc);
  calib_->load(calib_data, BTagEntry::FLAV_C, measType_bc);
  calib_->load(calib_data, BTagEntry::FLAV_UDSG, measType_udsg);
  calib_down_->load(calib_data, BTagEntry::FLAV_B, measType_bc);
  calib_down_->load(calib_data, BTagEntry::FLAV_C, measType_bc);
  calib_down_->load(calib_data, BTagEntry::FLAV_UDSG, measType_udsg);
}

bool MCBTagScaleFactor::process(Event & event) {

  if (!calib_) {
    event.set(h_btag_weight_,           1.);
    event.set(h_btag_weight_up_,        1.);
    event.set(h_btag_weight_down_,      1.);
    event.set(h_btag_weight_bc_up_,     1.);
    event.set(h_btag_weight_bc_down_,   1.);
    event.set(h_btag_weight_udsg_up_,   1.);
    event.set(h_btag_weight_udsg_down_, 1.);
    return true;
  }

  float weight, weightErrBC, weightErrUDSG;
  if (event.is_valid(h_topjets_)) {
    std::tie(weight, weightErrBC, weightErrUDSG) = get_weight_btag(event.get(h_topjets_), event);
  } else {
    assert(event.is_valid(h_jets_));
    TopJet tj;
    tj.set_subjets(event.get(h_jets_));
    std::tie(weight, weightErrBC, weightErrUDSG) = get_weight_btag(vector<TopJet>({tj}), event);
  }
  float weightErr = sqrt(weightErrBC*weightErrBC + weightErrUDSG*weightErrUDSG);

  float weight_up        = weight + weightErr;
  float weight_down      = weight - weightErr;
  float weight_bc_up     = weight + weightErrBC;
  float weight_bc_down   = weight - weightErrBC;
  float weight_udsg_up   = weight + weightErrUDSG;
  float weight_udsg_down = weight - weightErrUDSG;

  event.set(h_btag_weight_,           weight);
  event.set(h_btag_weight_up_,        weight_up);
  event.set(h_btag_weight_down_,      weight_down);
  event.set(h_btag_weight_bc_up_,     weight_bc_up);
  event.set(h_btag_weight_bc_down_,   weight_bc_down);
  event.set(h_btag_weight_udsg_up_,   weight_udsg_up);
  event.set(h_btag_weight_udsg_down_, weight_udsg_down);

  if (sysType_ == "up")        {event.weight *= weight_up;}
  else if (sysType_ == "down")      {event.weight *= weight_down;}
  else if (sysType_ == "up_bc")     {event.weight *= weight_bc_up;}
  else if (sysType_ == "down_bc")   {event.weight *= weight_bc_down;}
  else if (sysType_ == "up_udsg")   {event.weight *= weight_udsg_up;}
  else if (sysType_ == "down_udsg") {event.weight *= weight_udsg_down;}
  else                              {event.weight *= weight;}

  return true;
}


// originally taken from https://twiki.cern.ch/twiki/pub/CMS/BTagSFMethods/Method1aExampleCode_CSVM.cc.txt
std::tuple<float, float, float> MCBTagScaleFactor::get_weight_btag(const vector<TopJet> &jets, Event & event) {

  float mcTag = 1.;
  float mcNoTag = 1.;
  float dataTag = 1.;
  float dataNoTag = 1.;

  float err1 = 0.;
  float err2 = 0.;
  float err3 = 0.;
  float err4 = 0.;

  //Here we loop over all selected jets
  for (const auto & topjet : jets) { for (const auto & jet : topjet.subjets()) {

    int hadronFlavor = abs(jet.hadronFlavour());
    float abs_eta = fabs(jet.eta());
    float pt = jet.pt();

    if(abs_eta > 2.4) {
      continue;
    }

    TH2* eff_hist;
    if (hadronFlavor == 5) {
      ///here one need to provide the pt/eta dependent efficiency for b-tag for "b jet"
      eff_hist = eff_b_.get();
    }else if( hadronFlavor==4){
      ///here one need to provide the pt/eta dependent efficiency for b-tag for "c jet"
      eff_hist = eff_c_.get();
    }else{
      ///here one need to provide the pt/eta dependent efficiency for b-tag for "light jet"
      eff_hist = eff_udsg_.get();
    }
    const auto eff_pt_axis = eff_hist->GetXaxis();
    float pt_low_edge = eff_pt_axis->GetBinLowEdge(eff_pt_axis->GetFirst());
    float pt_high_edge = eff_pt_axis->GetBinUpEdge(eff_pt_axis->GetLast());
    float pt_for_eff = (pt > pt_low_edge) ? pt : pt_low_edge + 1.;
    pt_for_eff = (pt_for_eff < pt_high_edge) ? pt_for_eff : pt_high_edge - 1.;
    float eff = eff_hist->GetBinContent(eff_hist->FindFixBin(pt_for_eff, abs_eta));

    float SF = 1., SFerr = 0.;
    std::tie(SF, SFerr) = get_SF_btag(pt, abs_eta, hadronFlavor);

    if (btag_(jet, event)) {
      mcTag *= eff;
      dataTag *= eff*SF;

      if(hadronFlavor==5 || hadronFlavor ==4)  err1 += SFerr/SF; ///correlated for b/c
      else err3 += SFerr/SF; //correlated for light

    }else{
      mcNoTag *= (1- eff);
      dataNoTag *= (1- eff*SF);

      if(hadronFlavor==5 || hadronFlavor ==4 ) err2 += (-eff*SFerr)/(1-eff*SF); /// /correlated for b/c
      else err4 +=  (-eff*SFerr)/(1-eff*SF);  ////correlated for light

    }

  }}

  float wtbtag = (dataNoTag * dataTag ) / ( mcNoTag * mcTag );
  float wtbtagErrBC = fabs(err1+err2) * wtbtag;
  float wtbtagErrUDSG = fabs(err3+err4) * wtbtag;

  string errStr = " is NaN in MCBTagScaleFactor::get_weight_btag. Please check that all efficiency-bins are greater than 0.";
  if (isnan(wtbtag)) {
    throw runtime_error("wtbtag" + errStr);
  }
  if (isnan(wtbtagErrBC)) {
    throw runtime_error("wtbtagErrBC" + errStr);
  }
  if (isnan(wtbtagErrUDSG)) {
    throw runtime_error("wtbtagErrUDSG" + errStr);
  }

  return std::make_tuple(wtbtag, wtbtagErrBC, wtbtagErrUDSG);
}


// originally taken from https://twiki.cern.ch/twiki/pub/CMS/BTagSFMethods/Method1aExampleCode_CSVM.cc.txt
// (only the purpose of the function is the same, interface and implementation changed)
std::pair<float, float> MCBTagScaleFactor::get_SF_btag(float pt, float abs_eta, int flav) {

  auto btagentry_flav = flav == 5 ? BTagEntry::FLAV_B : (
    flav == 4 ? BTagEntry::FLAV_C :
    BTagEntry::FLAV_UDSG);

  auto sf_bounds = calib_->min_max_pt(btagentry_flav, abs_eta);

  float pt_for_eval = pt;
  bool is_out_of_bounds = false;
  if (pt < sf_bounds.first) {
    pt_for_eval = sf_bounds.first + 1e-5;
    is_out_of_bounds = true;
  } else if (pt > sf_bounds.second) {
    pt_for_eval = sf_bounds.second - 0.1;
    is_out_of_bounds = true;
  }

  float SF_up   = calib_up_->eval(btagentry_flav, abs_eta, pt_for_eval);
  float SF      = calib_->eval(btagentry_flav, abs_eta, pt_for_eval);
  float SF_down = calib_down_->eval(btagentry_flav, abs_eta, pt_for_eval);

  float SFerr_up_ = fabs(SF - SF_up);
  float SFerr_down_ = fabs(SF - SF_down);  // positive value!!

  float SFerr = SFerr_up_ > SFerr_down_ ? SFerr_up_ : SFerr_down_;

  if (is_out_of_bounds) {
    SFerr *= 2;
  }

  if (SF < 1e-10) {
    cout << "WARNING: SF vanishes! Will return SF = 1., SFerr = 0., Values: "
    << "(SF, SFerr, is_out_of_bounds, lowbound, highbound, pt, pt_for_eval, btagentry_flav): "
    << SF << ", " << SFerr << ", " << is_out_of_bounds << ", "
    << sf_bounds.first << ", " << sf_bounds.second << ", "
    << pt << ", " << pt_for_eval << ", " << btagentry_flav << endl;
    SF = 1.;
    SFerr = 0.;
  }
  return std::make_pair(SF, SFerr);
}


MCBTagDiscriminantReweighting::MCBTagDiscriminantReweighting(
  uhh2::Context & ctx,
  BTag::algo algorithm,
  const std::string & sysType,
  const std::string & jets_handle_name,
  const std::string & weights_name_postfix,
  const std::string & measType
):
  fAlgorithm(algorithm),
  fYear(extract_year(ctx)),
  h_jets(ctx.get_handle<std::vector<Jet>>(jets_handle_name)),
  h_weight_central (ctx.declare_event_output<float>("weight_btagdisc_"+weights_name_postfix+"_central")),
  h_weight_cferr1_up (ctx.declare_event_output<float>("weight_btagdisc_"+weights_name_postfix+"_cferr1_up")),
  h_weight_cferr1_down (ctx.declare_event_output<float>("weight_btagdisc_"+weights_name_postfix+"_cferr1_down")),
  h_weight_cferr2_up (ctx.declare_event_output<float>("weight_btagdisc_"+weights_name_postfix+"_cferr2_up")),
  h_weight_cferr2_down (ctx.declare_event_output<float>("weight_btagdisc_"+weights_name_postfix+"_cferr2_down")),
  h_weight_lf_up (ctx.declare_event_output<float>("weight_btagdisc_"+weights_name_postfix+"_lf_up")),
  h_weight_lf_down (ctx.declare_event_output<float>("weight_btagdisc_"+weights_name_postfix+"_lf_down")),
  h_weight_lfstats1_up (ctx.declare_event_output<float>("weight_btagdisc_"+weights_name_postfix+"_lfstats1_up")),
  h_weight_lfstats1_down (ctx.declare_event_output<float>("weight_btagdisc_"+weights_name_postfix+"_lfstats1_down")),
  h_weight_lfstats2_up (ctx.declare_event_output<float>("weight_btagdisc_"+weights_name_postfix+"_lfstats2_up")),
  h_weight_lfstats2_down (ctx.declare_event_output<float>("weight_btagdisc_"+weights_name_postfix+"_lfstats2_down")),
  h_weight_hf_up (ctx.declare_event_output<float>("weight_btagdisc_"+weights_name_postfix+"_hf_up")),
  h_weight_hf_down (ctx.declare_event_output<float>("weight_btagdisc_"+weights_name_postfix+"_hf_down")),
  h_weight_hfstats1_up (ctx.declare_event_output<float>("weight_btagdisc_"+weights_name_postfix+"_hfstats1_up")),
  h_weight_hfstats1_down (ctx.declare_event_output<float>("weight_btagdisc_"+weights_name_postfix+"_hfstats1_down")),
  h_weight_hfstats2_up (ctx.declare_event_output<float>("weight_btagdisc_"+weights_name_postfix+"_hfstats2_up")),
  h_weight_hfstats2_down (ctx.declare_event_output<float>("weight_btagdisc_"+weights_name_postfix+"_hfstats2_down"))
{
  const string dataset_type = ctx.get("dataset_type");
  const bool is_mc = dataset_type == "MC";
  if(!is_mc) {
    cout << "Warning: MCBTagDiscriminantReweighting will not have an effect on this non-MC sample (dataset_type = '" + dataset_type + "')" << endl;
    return;
  }

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

  vector<string> to_be_loaded = {
    "up_cferr1", "down_cferr1",
    "up_cferr2", "down_cferr2",
    "up_lf", "down_lf",
    "up_lfstats1", "down_lfstats1",
    "up_lfstats2", "down_lfstats2",
    "up_hf", "down_hf",
    "up_hfstats1", "down_hfstats1",
    "up_hfstats2", "down_hfstats2",
  };

  const string jecsmear_direction = ctx.get("jecsmear_direction", "nominal");
  const string jecsmear_source = ctx.get("jecsmear_source", "Total");
  if(jecsmear_direction == "nominal") {
    fCentralOrJES = "central";
  }
  // If JEC variation is applied, we do not apply the central b-tagging SF but the corresponding "up/down_jes..." SF.
  // If you specify a JEC uncertainty sub-source via "jecsmear_source" in your config XML, the correct corresponding b-tagging SF will be used.
  // In order to get an overview of which splitted JEC uncertainty sources are available, you should look into the UncertaintySources JEC files and the b-tagging SF files.
  // If you specify a JEC uncertainty source that is not supported, i.e. not given in the b-tagging SF files, BTagCalibrationReader will hopefully fail loudly.
  // By default, the "Total" JEC uncertainty is used.
  else if(jecsmear_direction == "up") {
    fCentralOrJES = "up_jes";
    if(jecsmear_source != "Total") fCentralOrJES += jecsmear_source;
    to_be_loaded.push_back(fCentralOrJES);
  }
  else if(jecsmear_direction == "down") {
    fCentralOrJES = "down_jes";
    if(jecsmear_source != "Total") fCentralOrJES += jecsmear_source;
    to_be_loaded.push_back(fCentralOrJES);
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
      sf_file_path += "reshaping_deepCSV_106XUL16preVFP_v2.csv";
      break;
      case Year::isUL16postVFP :
      sf_file_path += "reshaping_deepCSV_106XUL16postVFP_v3.csv";
      break;
      case Year::isUL17 :
      sf_file_path += "reshaping_deepCSV_106XUL17_v3.csv";
      break;
      case Year::isUL18 :
      sf_file_path += "reshaping_deepCSV_106XUL18_v2.csv";
      break;
      default :
      throw invalid_argument("MCBTagDiscriminantReweighting: Invalid year");
    }
    break;
    case BTag::DEEPJET :
    switch(fYear) {
      case Year::isUL16preVFP :
      sf_file_path += "reshaping_deepJet_106XUL16preVFP_v2.csv";
      break;
      case Year::isUL16postVFP :
      sf_file_path += "reshaping_deepJet_106XUL16postVFP_v3.csv";
      break;
      case Year::isUL17 :
      sf_file_path += "reshaping_deepJet_106XUL17_v3.csv";
      break;
      case Year::isUL18 :
      sf_file_path += "reshaping_deepJet_106XUL18_v2.csv";
      break;
      default :
      throw invalid_argument("MCBTagDiscriminantReweighting: Invalid year");
    }
    break;
    default :
    throw invalid_argument("MCBTagDiscriminantReweighting: Invalid b-tagging algorithm. This module only works with DeepJet or DeepCSV");
  }

  BTagCalibration calibration("tagger", sf_file_path); // CHECK: the first std::string argument here should not be relevant
  reader.reset(new BTagCalibrationReader(BTagEntry::OP_RESHAPING, "central", to_be_loaded));
  reader->load(calibration, BTagEntry::FLAV_B, measType);
  reader->load(calibration, BTagEntry::FLAV_C, measType);
  reader->load(calibration, BTagEntry::FLAV_UDSG, measType);
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

    BTagEntry::JetFlavor jet_flavor = BTagEntry::FLAV_UDSG;
    if(abs(jet.hadronFlavour()) == 5) jet_flavor = BTagEntry::FLAV_B;
    else if(abs(jet.hadronFlavour()) == 4) jet_flavor = BTagEntry::FLAV_C;

    if(jet_pt > 20.0 && fabs(jet_eta) < 2.5) { // UL: b-tagging possible for eta up to 2.5

      if(jet_flavor == BTagEntry::FLAV_C) {
        weight_central *= reader->eval_auto_bounds("central", jet_flavor, jet_eta, jet_pt, jet_btagdisc);

        weight_cferr1_up *= reader->eval_auto_bounds("up_cferr1", jet_flavor, jet_eta, jet_pt, jet_btagdisc);
        weight_cferr1_down *= reader->eval_auto_bounds("down_cferr1", jet_flavor, jet_eta, jet_pt, jet_btagdisc);
        weight_cferr2_up *= reader->eval_auto_bounds("up_cferr2", jet_flavor, jet_eta, jet_pt, jet_btagdisc);
        weight_cferr2_down *= reader->eval_auto_bounds("down_cferr2", jet_flavor, jet_eta, jet_pt, jet_btagdisc);
      }

      else if(jet_flavor == BTagEntry::FLAV_B || jet_flavor == BTagEntry::FLAV_UDSG) {
        weight_central *= reader->eval_auto_bounds(fCentralOrJES, jet_flavor, jet_eta, jet_pt, jet_btagdisc);

        // lf(stats1/2) and hf(stats1/2) variatons must be applied to both FLAV_B and FLAV_UDSG jets!
        weight_lf_up *= reader->eval_auto_bounds("up_lf", jet_flavor, jet_eta, jet_pt, jet_btagdisc);
        weight_lf_down *= reader->eval_auto_bounds("down_lf", jet_flavor, jet_eta, jet_pt, jet_btagdisc);
        weight_lfstats1_up *= reader->eval_auto_bounds("up_lfstats1", jet_flavor, jet_eta, jet_pt, jet_btagdisc);
        weight_lfstats1_down *= reader->eval_auto_bounds("down_lfstats1", jet_flavor, jet_eta, jet_pt, jet_btagdisc);
        weight_lfstats2_up *= reader->eval_auto_bounds("up_lfstats2", jet_flavor, jet_eta, jet_pt, jet_btagdisc);
        weight_lfstats2_down *= reader->eval_auto_bounds("down_lfstats2", jet_flavor, jet_eta, jet_pt, jet_btagdisc);

        weight_hf_up *= reader->eval_auto_bounds("up_hf", jet_flavor, jet_eta, jet_pt, jet_btagdisc);
        weight_hf_down *= reader->eval_auto_bounds("down_hf", jet_flavor, jet_eta, jet_pt, jet_btagdisc);
        weight_hfstats1_up *= reader->eval_auto_bounds("up_hfstats1", jet_flavor, jet_eta, jet_pt, jet_btagdisc);
        weight_hfstats1_down *= reader->eval_auto_bounds("down_hfstats1", jet_flavor, jet_eta, jet_pt, jet_btagdisc);
        weight_hfstats2_up *= reader->eval_auto_bounds("up_hfstats2", jet_flavor, jet_eta, jet_pt, jet_btagdisc);
        weight_hfstats2_down *= reader->eval_auto_bounds("down_hfstats2", jet_flavor, jet_eta, jet_pt, jet_btagdisc);
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
