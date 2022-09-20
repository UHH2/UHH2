#pragma once
#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"

// See this TWiki for explanation of all the different weights:
// https://twiki.cern.ch/twiki/bin/view/CMS/HowToPDF#Parton_shower_weights
// You have the option to turn off writing the splitted weights to your AnalysisTree if you don't need those
class PSWeights: public uhh2::AnalysisModule {
public:
  explicit PSWeights(uhh2::Context & ctx, const bool do_split_weights_ = true, const bool sample_without_ps_weights_ = false);
  virtual bool process(uhh2::Event & event) override;

private:

  // Inclusive Variations - up sqrt(2; down 1/sqrt(2)
  uhh2::Event::Handle<float> handle_isr_sqrt2_dn;
  uhh2::Event::Handle<float> handle_isr_sqrt2_up;
  uhh2::Event::Handle<float> handle_fsr_sqrt2_dn;
  uhh2::Event::Handle<float> handle_fsr_sqrt2_up;
  uhh2::Event::Handle<float> handle_isrfsr_sqrt2_dn;
  uhh2::Event::Handle<float> handle_isrfsr_sqrt2_up;

  // Inclusive Variations - up 2.0; down 0.5
  uhh2::Event::Handle<float> handle_isr_2_dn;
  uhh2::Event::Handle<float> handle_isr_2_up;
  uhh2::Event::Handle<float> handle_fsr_2_dn;
  uhh2::Event::Handle<float> handle_fsr_2_up;
  uhh2::Event::Handle<float> handle_isrfsr_2_dn;
  uhh2::Event::Handle<float> handle_isrfsr_2_up;

  // Inclusive Variations - up 4.0; down 0.25
  uhh2::Event::Handle<float> handle_isr_4_dn;
  uhh2::Event::Handle<float> handle_isr_4_up;
  uhh2::Event::Handle<float> handle_fsr_4_dn;
  uhh2::Event::Handle<float> handle_fsr_4_up;
  uhh2::Event::Handle<float> handle_isrfsr_4_dn;
  uhh2::Event::Handle<float> handle_isrfsr_4_up;

  // Split Variations -- fsr
  uhh2::Event::Handle<float> handle_fsr_g2gg_mur_dn;
  uhh2::Event::Handle<float> handle_fsr_g2gg_mur_up;
  uhh2::Event::Handle<float> handle_fsr_g2qq_mur_dn;
  uhh2::Event::Handle<float> handle_fsr_g2qq_mur_up;
  uhh2::Event::Handle<float> handle_fsr_q2qg_mur_dn;
  uhh2::Event::Handle<float> handle_fsr_q2qg_mur_up;
  uhh2::Event::Handle<float> handle_fsr_x2xg_mur_dn;
  uhh2::Event::Handle<float> handle_fsr_x2xg_mur_up;

  uhh2::Event::Handle<float> handle_fsr_g2gg_cns_dn;
  uhh2::Event::Handle<float> handle_fsr_g2gg_cns_up;
  uhh2::Event::Handle<float> handle_fsr_g2qq_cns_dn;
  uhh2::Event::Handle<float> handle_fsr_g2qq_cns_up;
  uhh2::Event::Handle<float> handle_fsr_q2qg_cns_dn;
  uhh2::Event::Handle<float> handle_fsr_q2qg_cns_up;
  uhh2::Event::Handle<float> handle_fsr_x2xg_cns_dn;
  uhh2::Event::Handle<float> handle_fsr_x2xg_cns_up;

  // Split Variations -- isr
  uhh2::Event::Handle<float> handle_isr_g2gg_mur_dn;
  uhh2::Event::Handle<float> handle_isr_g2gg_mur_up;
  uhh2::Event::Handle<float> handle_isr_g2qq_mur_dn;
  uhh2::Event::Handle<float> handle_isr_g2qq_mur_up;
  uhh2::Event::Handle<float> handle_isr_q2qg_mur_dn;
  uhh2::Event::Handle<float> handle_isr_q2qg_mur_up;
  uhh2::Event::Handle<float> handle_isr_x2xg_mur_dn;
  uhh2::Event::Handle<float> handle_isr_x2xg_mur_up;

  uhh2::Event::Handle<float> handle_isr_g2gg_cns_dn;
  uhh2::Event::Handle<float> handle_isr_g2gg_cns_up;
  uhh2::Event::Handle<float> handle_isr_g2qq_cns_dn;
  uhh2::Event::Handle<float> handle_isr_g2qq_cns_up;
  uhh2::Event::Handle<float> handle_isr_q2qg_cns_dn;
  uhh2::Event::Handle<float> handle_isr_q2qg_cns_up;
  uhh2::Event::Handle<float> handle_isr_x2xg_cns_dn;
  uhh2::Event::Handle<float> handle_isr_x2xg_cns_up;

  const bool do_split_weights;
  const bool sample_without_ps_weights;
  void initialise_handles(uhh2::Event & event);
  void write_inclusive_weights_from_geninfo(uhh2::Event & event);
  void write_split_weights_from_geninfo(uhh2::Event & event);
  bool is_mc;
};
