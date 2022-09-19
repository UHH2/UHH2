#pragma once
#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"


class PSWeights: public uhh2::AnalysisModule {
public:
  explicit PSWeights(uhh2::Context & ctx);
  virtual bool process(uhh2::Event & event) override;

  // Inclusive Variations - up sqrt(2; down 1/sqrt(2)
  uhh2::Event::Handle<float> handle_isr_05sqrt2_dn;
  uhh2::Event::Handle<float> handle_isr_sqrt2_up;
  uhh2::Event::Handle<float> handle_fsr_05sqrt2_dn;
  uhh2::Event::Handle<float> handle_fsr_sqrt2_up;
  uhh2::Event::Handle<float> handle_isrfsr_05sqrt2_dn;
  uhh2::Event::Handle<float> handle_isrfsr_sqrt2_up;
  // Inclusive Variations - up 2.0; down 0.5
  uhh2::Event::Handle<float> handle_isr_05_dn;
  uhh2::Event::Handle<float> handle_isr_20_up;
  uhh2::Event::Handle<float> handle_fsr_05_dn;
  uhh2::Event::Handle<float> handle_fsr_20_up;
  uhh2::Event::Handle<float> handle_isrfsr_05_dn;
  uhh2::Event::Handle<float> handle_isrfsr_20_up;
  // Inclusive Variations - up 4.0; down 0.25
  uhh2::Event::Handle<float> handle_isr_025_dn;
  uhh2::Event::Handle<float> handle_isr_40_up;
  uhh2::Event::Handle<float> handle_fsr_025_dn;
  uhh2::Event::Handle<float> handle_fsr_40_up;
  uhh2::Event::Handle<float> handle_isrfsr_025_dn;
  uhh2::Event::Handle<float> handle_isrfsr_40_up;
  // Split Variations -- fsr
  uhh2::Event::Handle<float> handle_fsr_g2gg_up;
  uhh2::Event::Handle<float> handle_fsr_g2gg_dn;
  uhh2::Event::Handle<float> handle_fsr_g2qq_up;
  uhh2::Event::Handle<float> handle_fsr_g2qq_dn;
  uhh2::Event::Handle<float> handle_fsr_g2qg_up;
  uhh2::Event::Handle<float> handle_fsr_g2qg_dn;
  uhh2::Event::Handle<float> handle_fsr_g2xg_up;
  uhh2::Event::Handle<float> handle_fsr_g2xg_dn;
  // Split Variations -- isr
  uhh2::Event::Handle<float> handle_isr_g2gg_up;
  uhh2::Event::Handle<float> handle_isr_g2gg_dn;
  uhh2::Event::Handle<float> handle_isr_g2qq_up;
  uhh2::Event::Handle<float> handle_isr_g2qq_dn;
  uhh2::Event::Handle<float> handle_isr_g2qg_up;
  uhh2::Event::Handle<float> handle_isr_g2qg_dn;
  uhh2::Event::Handle<float> handle_isr_g2xg_up;
  uhh2::Event::Handle<float> handle_isr_g2xg_dn;

private:
  void initialise_handles(uhh2::Event & event);
  void write_inclusive_weights_from_geninfo(uhh2::Event & event);
  void write_split_weights_from_geninfo(uhh2::Event & event);
  bool is_mc;
};
