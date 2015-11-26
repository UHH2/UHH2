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
 * Interprets "pileup_directory" from Context, which should be set to the
 * path to root files which contain the pileup information for each MC sample and data.
 * If an empty value is given, no pileup reweighting is done.
 * 
 * NOTE: currently, this is a dummy class; construction will fail with an excpetion, unless
 * an empty value is given for "pileup_directory".
 */
class MCPileupReweight: public uhh2::AnalysisModule {
public:
    explicit MCPileupReweight(uhh2::Context & ctx);

    virtual bool process(uhh2::Event & event) override;

private:
   TH1F *h_npu_mc;
   TH1F* h_npu_data;
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
                             std::string jets_handle_name="jets",
                             std::string sysType="central",
                             std::string measType_bc="mujets",
                             std::string measType_udsg="comb");

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
