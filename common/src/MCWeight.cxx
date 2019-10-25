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



MCPileupReweight::MCPileupReweight(Context & ctx, const std::string & sysType):
    h_pu_weight_(ctx.declare_event_output<float>("weight_pu")),
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
        h_pu_weight_up_ = ctx.declare_event_output<float>("weight_pu_up");
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
        h_pu_weight_down_ = ctx.declare_event_output<float>("weight_pu_down");
    }
}

bool MCPileupReweight::process(Event &event){
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
      event.set(h_pu_weight_, 0.f);
      event.set(h_pu_weight_up_, 0.f);
      event.set(h_pu_weight_down_, 0.f);
      return false;
    } else if (event.genInfo->pileup_TrueNumInteractions() > h_npu_mc->GetXaxis()->GetXmax()) {
      cout << "WARNING trueNumInteractions = " << trueNumInteractions << " > upper edge of MC hist = " << h_npu_mc->GetXaxis()->GetXmax();
      cout << " Setting event weight_pu to 0" << endl;
      event.set(h_pu_weight_, 0.f);
      event.set(h_pu_weight_up_, 0.f);
      event.set(h_pu_weight_down_, 0.f);
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
      event.set(h_pu_weight_, 0.f);
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




MCMuonScaleFactor::MCMuonScaleFactor(uhh2::Context & ctx,
                                     const std::string & sf_file_path,
                                     const std::string & sf_name,
                                     float sys_error_percantage,
                                     const std::string & weight_postfix,
				     bool etaYaxis,
                                     const std::string & sys_uncert,
                                     const std::string & muons_handle_name):
  h_muons_            (ctx.get_handle<std::vector<Muon>>(muons_handle_name)),
  h_muon_weight_      (ctx.declare_event_output<float>("weight_sfmu_" + weight_postfix)),
  h_muon_weight_up_   (ctx.declare_event_output<float>("weight_sfmu_" + weight_postfix + "_up")),
  h_muon_weight_down_ (ctx.declare_event_output<float>("weight_sfmu_" + weight_postfix + "_down")),
  sys_error_factor_(sys_error_percantage/100.), etaYaxis_(etaYaxis)
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
    float eta = fabs(mu.eta());
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
  float eta = fabs(mu.eta());
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


MCMuonTrkScaleFactor::MCMuonTrkScaleFactor(uhh2::Context & ctx,
                                     const std::string & sf_file_path,
                                     float sys_error_percantage,
                                     const std::string & weight_postfix,
                                     const std::string & sys_uncert,
                                     const std::string & muons_handle_name):
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



MCElecScaleFactor::MCElecScaleFactor(uhh2::Context & ctx,
                                     const std::string & sf_file_path,
                                     float sys_error_percantage,
                                     const std::string & weight_postfix,
                                     const std::string & sys_uncert,
                                     const std::string & elecs_handle_name):
  h_elecs_            (ctx.get_handle<std::vector<Electron>>(elecs_handle_name)),
  h_elec_weight_      (ctx.declare_event_output<float>("weight_sfelec_" + weight_postfix)),
  h_elec_weight_up_   (ctx.declare_event_output<float>("weight_sfelec_" + weight_postfix + "_up")),
  h_elec_weight_down_ (ctx.declare_event_output<float>("weight_sfelec_" + weight_postfix + "_down")),
  sys_error_factor_(sys_error_percantage/100.)
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

  sf_hist_.reset((TH2*) sf_file.Get("EGamma_SF2D"));
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
    float eta = fabs(el.supercluster_eta());
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


MCBTagScaleFactor::MCBTagScaleFactor(uhh2::Context & ctx,
				     BTag::algo tagger,
				     BTag::wp wp,
				     const std::string & jets_handle_name,
                                     const std::string & sysType,
                                     const std::string & measType_bc,
                                     const std::string & measType_udsg,
                                     const std::string & xml_param_name,
				     const std::string & weights_name_postfix,
				     const std::string & xml_calib_name):
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


///// Copy from https://github.com/reimersa/UHH2/blob/RunII_94X_v1/common/src/MCWeight.cxx#L851
MCCSVv2ShapeSystematic::MCCSVv2ShapeSystematic(uhh2::Context & ctx,
                                               const std::string & jets_handle_name,
                                               const std::string & sysType,
                                               const std::string & measType,
                                               const std::string & weights_name_postfix,
                                               const std::string & xml_calib_name):
  h_jets_(ctx.get_handle<std::vector<Jet>>(jets_handle_name)),
  sysType_(sysType),
  h_weight_csv_central (ctx.declare_event_output<float>("weight_csv_central"+weights_name_postfix)),
  h_weight_csv_jesup (ctx.declare_event_output<float>("weight_csv_jesup"+weights_name_postfix)),
  h_weight_csv_jesdown (ctx.declare_event_output<float>("weight_csv_jesdown"+weights_name_postfix)),
  h_weight_csv_lfup (ctx.declare_event_output<float>("weight_csv_lfup"+weights_name_postfix)),
  h_weight_csv_lfdown (ctx.declare_event_output<float>("weight_csv_lfdown"+weights_name_postfix)),
  h_weight_csv_hfup (ctx.declare_event_output<float>("weight_csv_hfup"+weights_name_postfix)),
  h_weight_csv_hfdown (ctx.declare_event_output<float>("weight_csv_hfdown"+weights_name_postfix)),
  h_weight_csv_hfstats1up (ctx.declare_event_output<float>("weight_csv_hfstats1up"+weights_name_postfix)),
  h_weight_csv_hfstats1down (ctx.declare_event_output<float>("weight_csv_hfstats1down"+weights_name_postfix)),
  h_weight_csv_hfstats2up (ctx.declare_event_output<float>("weight_csv_hfstats2up"+weights_name_postfix)),
  h_weight_csv_hfstats2down (ctx.declare_event_output<float>("weight_csv_hfstats2down"+weights_name_postfix)),
  h_weight_csv_lfstats1up (ctx.declare_event_output<float>("weight_csv_lfstats1up"+weights_name_postfix)),
  h_weight_csv_lfstats1down (ctx.declare_event_output<float>("weight_csv_lfstats1down"+weights_name_postfix)),
  h_weight_csv_lfstats2up (ctx.declare_event_output<float>("weight_csv_lfstats2up"+weights_name_postfix)),
  h_weight_csv_lfstats2down (ctx.declare_event_output<float>("weight_csv_lfstats2down"+weights_name_postfix)),
  h_weight_csv_cferr1up (ctx.declare_event_output<float>("weight_csv_cferr1up"+weights_name_postfix)),
  h_weight_csv_cferr1down (ctx.declare_event_output<float>("weight_csv_cferr1down"+weights_name_postfix)),
  h_weight_csv_cferr2up (ctx.declare_event_output<float>("weight_csv_cferr2up"+weights_name_postfix)),
  h_weight_csv_cferr2down (ctx.declare_event_output<float>("weight_csv_cferr2down"+weights_name_postfix))
{
  auto dataset_type = ctx.get("dataset_type");
  bool is_mc = dataset_type == "MC";
  if (!is_mc) {
    cout << "Warning: MCCSVv2ShapeSystematic will not have an effect on "
         <<" this non-MC sample (dataset_type = '" + dataset_type + "')" << endl;
    return;
  }

  BTagCalibration calib_csvv2("csvv2", ctx.get(xml_calib_name));
  reader.reset(new BTagCalibrationReader(BTagEntry::OP_RESHAPING,"central",
                                         {"up_jes","down_jes",
                                             "up_lf","down_lf",
                                             "up_hf","down_hf",
                                             "up_hfstats1","down_hfstats1",
                                             "up_hfstats2","down_hfstats2",
                                             "up_lfstats1","down_lfstats1",
                                             "up_lfstats2","down_lfstats2",
                                             "up_cferr1","down_cferr1",
                                             "up_cferr2","down_cferr2"}));
  reader->load(calib_csvv2,BTagEntry::FLAV_B,measType);
  reader->load(calib_csvv2,BTagEntry::FLAV_C,measType);
  reader->load(calib_csvv2,BTagEntry::FLAV_UDSG,measType);

}

bool MCCSVv2ShapeSystematic::process(Event & event) {

  if (event.isRealData) {
    event.set(h_weight_csv_central,1.);
    event.set(h_weight_csv_jesup,1.);
    event.set(h_weight_csv_jesdown,1.);
    event.set(h_weight_csv_lfup,1.);
    event.set(h_weight_csv_lfdown,1.);
    event.set(h_weight_csv_hfup,1.);
    event.set(h_weight_csv_hfdown,1.);
    event.set(h_weight_csv_hfstats1up,1.);
    event.set(h_weight_csv_hfstats1down,1.);
    event.set(h_weight_csv_hfstats2up,1.);
    event.set(h_weight_csv_hfstats2down,1.);
    event.set(h_weight_csv_lfstats1up,1.);
    event.set(h_weight_csv_lfstats1down,1.);
    event.set(h_weight_csv_lfstats2up,1.);
    event.set(h_weight_csv_lfstats2down,1.);
    event.set(h_weight_csv_cferr1up,1.);
    event.set(h_weight_csv_cferr1down,1.);
    event.set(h_weight_csv_cferr2up,1.);
    event.set(h_weight_csv_cferr2down,1.);
    return true;
  }

  float weight_central = 1.0;
  float weight_jesup = 1.0;
  float weight_jesdown = 1.0;
  float weight_lfup = 1.0;
  float weight_lfdown = 1.0;
  float weight_hfup = 1.0;
  float weight_hfdown = 1.0;
  float weight_hfstats1up = 1.0;
  float weight_hfstats1down = 1.0;
  float weight_hfstats2up = 1.0;
  float weight_hfstats2down = 1.0;
  float weight_lfstats1up = 1.0;
  float weight_lfstats1down = 1.0;
  float weight_lfstats2up = 1.0;
  float weight_lfstats2down = 1.0;
  float weight_cferr1up = 1.0;
  float weight_cferr1down = 1.0;
  float weight_cferr2up = 1.0;
  float weight_cferr2down = 1.0;

  const auto & jets = event.get(h_jets_);
  for (size_t ijet=0; ijet < 2; ijet++) {
    Jet jet = jets.at(ijet);
    float jet_pt = jet.pt();
    float jet_eta = jet.eta();
    float jet_csv = jet.btag_combinedSecondaryVertex();
    if (jet_csv < 0.0) jet_csv = -0.05;
    if (jet_csv > 1.0) jet_csv = 1.0;

    BTagEntry::JetFlavor jet_flavor = BTagEntry::FLAV_UDSG;
    if (abs(jet.hadronFlavour()) == 5) jet_flavor = BTagEntry::FLAV_B;
    else if (abs(jet.hadronFlavour()) == 4) jet_flavor = BTagEntry::FLAV_C;

    if( jet_pt > 20.0 && fabs(jet_eta) < 2.4) {
      weight_central *= reader->eval_auto_bounds("central",jet_flavor, jet_eta, jet_pt, jet_csv);
      if (jet_flavor == BTagEntry::FLAV_B) {
        weight_jesup *= reader->eval_auto_bounds("up_jes",jet_flavor, jet_eta, jet_pt, jet_csv);
        weight_jesdown *= reader->eval_auto_bounds("down_jes",jet_flavor, jet_eta, jet_pt, jet_csv);
        weight_lfup *= reader->eval_auto_bounds("up_lf",jet_flavor, jet_eta, jet_pt, jet_csv);
        weight_lfdown *= reader->eval_auto_bounds("down_lf",jet_flavor, jet_eta, jet_pt, jet_csv);
        weight_hfstats1up *= reader->eval_auto_bounds("up_hfstats1",jet_flavor, jet_eta, jet_pt, jet_csv);
        weight_hfstats1down *= reader->eval_auto_bounds("down_hfstats1",jet_flavor, jet_eta, jet_pt, jet_csv);
        weight_hfstats2up *= reader->eval_auto_bounds("up_hfstats2",jet_flavor, jet_eta, jet_pt, jet_csv);
        weight_hfstats2down *= reader->eval_auto_bounds("down_hfstats2",jet_flavor, jet_eta, jet_pt, jet_csv);
      }
      if (jet_flavor == BTagEntry::FLAV_C) {
        weight_cferr1up *= reader->eval_auto_bounds("up_cferr1",jet_flavor, jet_eta, jet_pt, jet_csv);
        weight_cferr1down *= reader->eval_auto_bounds("down_cferr1",jet_flavor, jet_eta, jet_pt, jet_csv);
        weight_cferr2up *= reader->eval_auto_bounds("up_cferr2",jet_flavor, jet_eta, jet_pt, jet_csv);
        weight_cferr2down *= reader->eval_auto_bounds("down_cferr2",jet_flavor, jet_eta, jet_pt, jet_csv);
      }
      if (jet_flavor == BTagEntry::FLAV_UDSG) {
        weight_jesup *= reader->eval_auto_bounds("up_jes",jet_flavor, jet_eta, jet_pt, jet_csv);
        weight_jesdown *= reader->eval_auto_bounds("down_jes",jet_flavor, jet_eta, jet_pt, jet_csv);
        weight_hfup *= reader->eval_auto_bounds("up_hf",jet_flavor, jet_eta, jet_pt, jet_csv);
        weight_hfdown *= reader->eval_auto_bounds("down_hf",jet_flavor, jet_eta, jet_pt, jet_csv);
        weight_lfstats1up *= reader->eval_auto_bounds("up_lfstats1",jet_flavor, jet_eta, jet_pt, jet_csv);
        weight_lfstats1down *= reader->eval_auto_bounds("down_lfstats1",jet_flavor, jet_eta, jet_pt, jet_csv);
        weight_lfstats2up *= reader->eval_auto_bounds("up_lfstats2",jet_flavor, jet_eta, jet_pt, jet_csv);
        weight_lfstats2down *= reader->eval_auto_bounds("down_lfstats2",jet_flavor, jet_eta, jet_pt, jet_csv);
      }
    }
  }

  event.set(h_weight_csv_central, weight_central);
  event.set(h_weight_csv_jesup, weight_jesup);
  event.set(h_weight_csv_jesdown, weight_jesdown);
  event.set(h_weight_csv_lfup, weight_lfup);
  event.set(h_weight_csv_lfdown, weight_lfdown);
  event.set(h_weight_csv_hfup, weight_hfup);
  event.set(h_weight_csv_hfdown, weight_hfdown);
  event.set(h_weight_csv_hfstats1up, weight_hfstats1up);
  event.set(h_weight_csv_hfstats1down, weight_hfstats1down);
  event.set(h_weight_csv_hfstats2up, weight_hfstats2up);
  event.set(h_weight_csv_hfstats2down, weight_hfstats2down);
  event.set(h_weight_csv_lfstats1up, weight_lfstats1up);
  event.set(h_weight_csv_lfstats1down, weight_lfstats1down);
  event.set(h_weight_csv_lfstats2up, weight_lfstats2up);
  event.set(h_weight_csv_lfstats2down, weight_lfstats2down);
  event.set(h_weight_csv_cferr1up, weight_cferr1up);
  event.set(h_weight_csv_cferr1down, weight_cferr1down);
  event.set(h_weight_csv_cferr2up, weight_cferr2up);
  event.set(h_weight_csv_cferr2down, weight_cferr2down);

  if (sysType_ == "jesup") {event.weight *= weight_jesup;}
  else if (sysType_ == "jesdown") {event.weight *= weight_jesdown;}
  else if (sysType_ == "lfup") {event.weight *= weight_lfup;}
  else if (sysType_ == "lfdown") {event.weight *= weight_lfdown;}
  else if (sysType_ == "hfup") {event.weight *= weight_hfup;}
  else if (sysType_ == "hfdown") {event.weight *= weight_hfdown;}
  else if (sysType_ == "hfstats1up") {event.weight *= weight_hfstats1up;}
  else if (sysType_ == "hfstats1down") {event.weight *= weight_hfstats1down;}
  else if (sysType_ == "hfstats2up") {event.weight *= weight_hfstats2up;}
  else if (sysType_ == "hfstats2down") {event.weight *= weight_hfstats2down;}
  else if (sysType_ == "lfstats1up") {event.weight *= weight_lfstats1up;}
  else if (sysType_ == "lfstats1down") {event.weight *= weight_lfstats1down;}
  else if (sysType_ == "lfstats2up") {event.weight *= weight_lfstats2up;}
  else if (sysType_ == "lfstats2down") {event.weight *= weight_lfstats2down;}
  else if (sysType_ == "cferr1up") {event.weight *= weight_cferr1up;}
  else if (sysType_ == "cferr1down") {event.weight *= weight_cferr1down;}
  else if (sysType_ == "cferr2up") {event.weight *= weight_cferr2up;}
  else if (sysType_ == "cferr2down") {event.weight *= weight_cferr2down;}
  else {event.weight *= weight_central;}

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
