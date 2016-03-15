#pragma once

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/common/include/JetIds.h"
#include "TH2.h"

/** \brief Set the event weight according to the sample lumi and a target lumi
 *
 * This module only has an effect if "use_sframe_weight" is set to "false" to avoid
 * applying the same weight twice. Also, it will only run, if "dataset_type"
 * equals "MC".
 *
 * Interprets the following settings from the passed Context:
 *  - "dataset_lumi": the lumi of the current dataset, in inverse pb
 *  - "target_lumi": the data lumi to reweight to, in inverse pb
 * These are set automatically if running via SFrame by AnalysisModuleRunner.
 * 
 * The event weight is multiplied with the ratio of dataset_lumi and target_lumi, independent
 * of the maximum number of processed events. This is different from sframe which
 * takes into account the number of events to process.
 */
class MCLumiWeight: public uhh2::AnalysisModule {
public:
    explicit MCLumiWeight(uhh2::Context & ctx);

    virtual bool process(uhh2::Event & event) override;

private:
    double factor = 1.0;
    bool use_sframe_weight;
};


/** \brief Reweight MC based on simulated and actual pileup
 *
 * Interprets "pileup_directory" and "pileup_directory_data" from Context, which should
 * be set to the path to root files which contain the pileup information for each MC
 * sample and data. If an empty value is given, no pileup reweighting is done.
 * "pileup_directory_data_up" and "pileup_directory_data_down" may be set as well.
 * 
 * If sysType is set to "up" or "down", both path are needed: pileup_directory_data_<up/down>
 */
class MCPileupReweight: public uhh2::AnalysisModule {
public:
    explicit MCPileupReweight(uhh2::Context & ctx, const std::string & sysType="central");

    virtual bool process(uhh2::Event & event) override;

private:
   uhh2::Event::Handle<float> h_pu_weight_;
   uhh2::Event::Handle<float> h_pu_weight_up_;
   uhh2::Event::Handle<float> h_pu_weight_down_;
   TH1F * h_npu_mc;
   TH1F * h_npu_data;
   TH1F * h_npu_data_up;
   TH1F * h_npu_data_down;
   std::string sysType_;
};


/** \brief Vary renormalization and factorization scale up or down by factor 2
 *
 * Interprets "ScaleVariationMuR" and "ScaleVariationMuF" from Context, the values may be "up" or "down".
 * Any other value will result in no scale variation. This method also will have no effect on samples
 * where genInfo.systweight is not filled. 
 */
class MCScaleVariation: public uhh2::AnalysisModule {
 public:
  explicit MCScaleVariation(uhh2::Context & ctx);

  virtual bool process(uhh2::Event & event) override;

  private:
  double syst_weight;
  int i_mu_r = 0, i_mu_f = 0;

};



/** \brief Apply muon scale factors
 *
 * https://twiki.cern.ch/twiki/bin/viewauth/CMS/MuonReferenceEffsRun2#Results_for_2015_data
 *
 * - only for applying pt- _and_ eta-scale dependent scale factors
 *
 * - parameters:
 *   - sf_file_path: path the root file with the scale factors
 *   - sf_name: directory name in the root file
 *   - sys_error_percantage: e.g. "1." for 1% of systematic error
 *   - weight_postfix: handle name for weights, e.g. "trigger" will produce the
 *     handles "weight_sfmu_trigger", "weight_sfmu_trigger_up" and 
 *     "weight_sfmu_trigger_down"
 *   - sys_uncert: which uncertainty is applied to event.weight, can be "up", 
 *     "down" or "nominal".
 *   - muons_handle_name: handle to the muon collection (the default points to 
 *     event.muons)
 */
class MCMuonScaleFactor: public uhh2::AnalysisModule {
public:
  explicit MCMuonScaleFactor(uhh2::Context & ctx,
                             const std::string & sf_file_path,
                             const std::string & sf_name,
                             float sys_error_percantage,
                             const std::string & weight_postfix="",
                             const std::string & sys_uncert="nominal",
                             const std::string & muons_handle_name="muons"); 

  virtual bool process(uhh2::Event & event) override;

private:
  uhh2::Event::Handle<std::vector<Muon>> h_muons_;
  std::unique_ptr<TH2> sf_hist_;
  uhh2::Event::Handle<float> h_muon_weight_;
  uhh2::Event::Handle<float> h_muon_weight_up_;
  uhh2::Event::Handle<float> h_muon_weight_down_;
  float sys_error_factor_;
  float eta_min_, eta_max_, pt_min_, pt_max_;
  int sys_direction_;
};



class BTagCalibrationReader;  // forward declaration
/** \brief apply btag scale factors
 *
 * jets_handle_name should point to a handle of type vector<Jet> _or_
 * vector<TopJet>, were in the latter case all of the subjets are used.
 *
 * measurementType and sysType are interpreted by the BTagCalibration
 * (check https://twiki.cern.ch/twiki/bin/viewauth/CMS/BTagCalibration).
 * Currently, sysType can be one of central, up, down, up_bc, down_bc, up_udsg, 
 * down_udsg.
 *
 * This module is designed to be used with the CSVBTag class
 * (see include/JetIds.h) and only one operating point!
 *
 * These values are taken from the context:
 *  - "MCBtagEfficiencies" should be a path-string pointing to a file with three
 *    histograms, that were made with the main selection of your analysis
 *    applied (TH2, x->pt, y->eta):
 *    - BTagMCEffFlavBEff
 *    - BTagMCEffFlavCEff
 *    - BTagMCEffFlavUDSGEff
 *  - "BTagCalibration" should be a path-string pointing to the .csv file
 *    containing scale factors.
 */
class MCBTagScaleFactor: public uhh2::AnalysisModule {
 public:
  explicit MCBTagScaleFactor(uhh2::Context & ctx,
                             const CSVBTag::wp & working_point,
                             const std::string & jets_handle_name="jets",
                             const std::string & sysType="central",
                             const std::string & measType_bc="mujets",
                             const std::string & measType_udsg="incl",
                             const std::string & xml_param_name="MCBtagEfficiencies",
                             const std::string & weights_name_postfix="");

  virtual bool process(uhh2::Event & event) override;

 protected:
  std::tuple<float, float, float> get_weight_btag(const std::vector<TopJet> &jets,
                                                  uhh2::Event & event);
  std::pair<float, float> get_SF_btag(float pt, float abs_eta, int flav);

  const CSVBTag btag_;
  std::unique_ptr<BTagCalibrationReader> calib_up_;
  std::unique_ptr<BTagCalibrationReader> calib_;
  std::unique_ptr<BTagCalibrationReader> calib_down_;
  std::unique_ptr<TH2> eff_b_;
  std::unique_ptr<TH2> eff_c_;
  std::unique_ptr<TH2> eff_udsg_;
  uhh2::Event::Handle<std::vector<Jet>> h_jets_;
  uhh2::Event::Handle<std::vector<TopJet>> h_topjets_;
  std::string sysType_;
  uhh2::Event::Handle<float> h_btag_weight_;
  uhh2::Event::Handle<float> h_btag_weight_up_;
  uhh2::Event::Handle<float> h_btag_weight_down_;
  uhh2::Event::Handle<float> h_btag_weight_bc_up_;
  uhh2::Event::Handle<float> h_btag_weight_bc_down_;
  uhh2::Event::Handle<float> h_btag_weight_udsg_up_;
  uhh2::Event::Handle<float> h_btag_weight_udsg_down_;
};

/** \brief Reweight MC based on DATA/MC descripancy for one particluar variable
 *
 * Interprets "reweight_file" from Context, which should be set to 
 * the root file (with correct path) which contain the reweighting information for MC samples
 * If an empty value is given, no reweighting is done.
 * 
 * NOTE: currently, this is a dummy class; construction will fail with an excpetion, unless
 * an empty value is given for "reweight_directory".
 */
class MCVarReweight: public uhh2::AnalysisModule {
public:
    explicit MCVarReweight(uhh2::Context & ctx);

    virtual bool process(uhh2::Event & event) override;

private:
   uhh2::Event::Handle<float> h_var_weight_;
   uhh2::Event::Handle<float> tt_x; // variable used to derive reweighting
   TF1 *h_reweight_mc; //reweighting function
   TF1 *h_reweight_mc_1,*h_reweight_mc_2,*h_reweight_mc_3; //several reweighting function, e.g for lep_eta variable
   TString reweight_var; //variable
   TString var_trans;//variable transformation
   TString reweight_file;
};

/** \brief Vary Tau efficiency
 *
 * https://twiki.cern.ch/twiki/bin/view/CMS/TauIDRecommendation13TeV#Tau_ID_efficiency
 *
 * Interprets "TauIdVariation" from Context, the values may be "up" or "down".
 * Any other value will result in no scale variation. 
 */
class TauEffVariation: public uhh2::AnalysisModule {
 public:
  explicit TauEffVariation(uhh2::Context & ctx);

  virtual bool process(uhh2::Event & event) override;

  private:
  int i_TauEff = 0;
};

/** \brief Vary Tau charge
 *
 * https://twiki.cern.ch/twiki/bin/view/CMS/TauIDRecommendation13TeV#Tau_charge_misidentification_rat
 *
 * Interprets "TauChargeVariation" from Context, the values may be "up" or "down".
 * Any other value will result in no scale variation. 
 */
class TauChargeVariation: public uhh2::AnalysisModule {
 public:
  explicit TauChargeVariation(uhh2::Context & ctx);

  virtual bool process(uhh2::Event & event) override;

  private:
  int i_TauCharge = 0;
};
