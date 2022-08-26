#include "UHH2/common/include/PSWeights.h"

using namespace std;
using namespace uhh2;

PSWeights::PSWeights(Context & ctx) {

  auto dataset_type = ctx.get("dataset_type");
  is_mc = dataset_type == "MC";

  // Inclusive Variations - up sqrt(2; down 1/sqrt(2)
  handle_isr_05sqrt2_dn = ctx.declare_event_output<float>("weight_isr_sqrt2_down");
  handle_isr_sqrt2_up = ctx.declare_event_output<float>("weight_isr_sqrt2_up");
  handle_fsr_05sqrt2_dn = ctx.declare_event_output<float>("weight_fsr_sqrt2_down");
  handle_fsr_sqrt2_up = ctx.declare_event_output<float>("weight_fsr_sqrt2_up");
  handle_isrfsr_05sqrt2_dn = ctx.declare_event_output<float>("weight_isrfsr_sqrt2_down");
  handle_isrfsr_sqrt2_up = ctx.declare_event_output<float>("weight_isrfsr_sqrt2_up");
  // Inclusive Variations - up 2.0; down 0.5
  handle_isr_05_dn = ctx.declare_event_output<float>("weight_isr_20_05_down");
  handle_isr_20_up = ctx.declare_event_output<float>("weight_isr_20_05_up");
  handle_fsr_05_dn = ctx.declare_event_output<float>("weight_fsr_20_05_down");
  handle_fsr_20_up = ctx.declare_event_output<float>("weight_fsr_20_05_up");
  handle_isrfsr_05_dn = ctx.declare_event_output<float>("weight_isrfsr_20_05_down");
  handle_isrfsr_20_up = ctx.declare_event_output<float>("weight_isrfsr_20_05_up");
  // Inclusive Variations - up 4.0; down 0.25
  handle_isr_025_dn = ctx.declare_event_output<float>("weight_isr_40_025_down");
  handle_isr_40_up = ctx.declare_event_output<float>("weight_isr_40_025_up");
  handle_fsr_025_dn = ctx.declare_event_output<float>("weight_fsr_40_025_down");
  handle_fsr_40_up = ctx.declare_event_output<float>("weight_fsr_40_025_up");
  handle_isrfsr_025_dn = ctx.declare_event_output<float>("weight_isrfsr_40_025_down");
  handle_isrfsr_40_up = ctx.declare_event_output<float>("weight_isrfsr_40_025_up");
  // Split Variations -- fsr
  handle_fsr_g2gg_up = ctx.declare_event_output<float>("weight_fsr_g2gg_up");
  handle_fsr_g2gg_dn = ctx.declare_event_output<float>("weight_fsr_g2gg_down");
  handle_fsr_g2qq_up = ctx.declare_event_output<float>("weight_fsr_g2qq_up");
  handle_fsr_g2qq_dn = ctx.declare_event_output<float>("weight_fsr_g2qq_down");
  handle_fsr_g2qg_up = ctx.declare_event_output<float>("weight_fsr_g2qg_up");
  handle_fsr_g2qg_dn = ctx.declare_event_output<float>("weight_fsr_g2qg_down");
  handle_fsr_g2xg_up = ctx.declare_event_output<float>("weight_fsr_g2xg_up");
  handle_fsr_g2xg_dn = ctx.declare_event_output<float>("weight_fsr_g2xg_down");
  // Split Variations -- isr
  handle_isr_g2gg_up = ctx.declare_event_output<float>("weight_isr_g2gg_up");
  handle_isr_g2gg_dn = ctx.declare_event_output<float>("weight_isr_g2gg_down");
  handle_isr_g2qq_up = ctx.declare_event_output<float>("weight_isr_g2qq_up");
  handle_isr_g2qq_dn = ctx.declare_event_output<float>("weight_isr_g2qq_down");
  handle_isr_g2qg_up = ctx.declare_event_output<float>("weight_isr_g2qg_up");
  handle_isr_g2qg_dn = ctx.declare_event_output<float>("weight_isr_g2qg_down");
  handle_isr_g2xg_up = ctx.declare_event_output<float>("weight_isr_g2xg_up");
  handle_isr_g2xg_dn = ctx.declare_event_output<float>("weight_isr_g2xg_down");

  if(!is_mc){
    cout << "Warning: MCScaleVariation will not have an effect on this non-MC sample (dataset_type = '" + dataset_type + "')" << endl;
    return;
  }
}


void PSWeights::initialise_handles(Event & event){
  // Inclusive
  event.set(handle_isr_sqrt2_up, 1.0);
  event.set(handle_isr_05sqrt2_dn, 1.0);
  event.set(handle_fsr_sqrt2_up, 1.0);
  event.set(handle_fsr_05sqrt2_dn, 1.0);
  event.set(handle_isrfsr_sqrt2_up, 1.0);
  event.set(handle_isrfsr_05sqrt2_dn, 1.0);
  event.set(handle_isr_20_up, 1.0);
  event.set(handle_isr_05_dn, 1.0);
  event.set(handle_fsr_20_up, 1.0);
  event.set(handle_fsr_05_dn, 1.0);
  event.set(handle_isrfsr_20_up, 1.0);
  event.set(handle_isrfsr_05_dn, 1.0);
  event.set(handle_isr_40_up, 1.0);
  event.set(handle_isr_025_dn, 1.0);
  event.set(handle_fsr_40_up, 1.0);
  event.set(handle_fsr_025_dn, 1.0);
  event.set(handle_isrfsr_40_up, 1.0);
  event.set(handle_isrfsr_025_dn, 1.0);
  // Split
  event.set(handle_fsr_g2gg_dn, 1.0);
  event.set(handle_fsr_g2gg_up, 1.0);
  event.set(handle_fsr_g2qq_dn, 1.0);
  event.set(handle_fsr_g2qq_up, 1.0);
  event.set(handle_fsr_g2qg_dn, 1.0);
  event.set(handle_fsr_g2qg_up, 1.0);
  event.set(handle_fsr_g2xg_dn, 1.0);
  event.set(handle_fsr_g2xg_up, 1.0);
  event.set(handle_isr_g2gg_dn, 1.0);
  event.set(handle_isr_g2gg_up, 1.0);
  event.set(handle_isr_g2qq_dn, 1.0);
  event.set(handle_isr_g2qq_up, 1.0);
  event.set(handle_isr_g2qg_dn, 1.0);
  event.set(handle_isr_g2qg_up, 1.0);
  event.set(handle_isr_g2xg_dn, 1.0);
  event.set(handle_isr_g2xg_up, 1.0);
}


void PSWeights::write_inclusive_weights_from_geninfo(Event & event) {
  // The indexing is according to the documentation in:
  // https://twiki.cern.ch/twiki/bin/view/CMS/HowToPDF#Parton_shower_weights
  float nominal = event.genInfo->weights().at(0);

  // up sqrt(2); down 1/sqrt(2)
  float isr_05sqrt2_dn = event.genInfo->weights().at(24) / nominal;
  float isr_sqrt2_up = event.genInfo->weights().at(25) / nominal;
  float fsr_05sqrt2_dn = event.genInfo->weights().at(2) / nominal;
  float fsr_sqrt2_up = event.genInfo->weights().at(3) / nominal;
  event.set(handle_isr_sqrt2_up, isr_sqrt2_up);
  event.set(handle_isr_05sqrt2_dn, isr_05sqrt2_dn);
  event.set(handle_fsr_sqrt2_up, fsr_sqrt2_up);
  event.set(handle_fsr_05sqrt2_dn, fsr_05sqrt2_dn);
  event.set(handle_isrfsr_05sqrt2_dn, isr_05sqrt2_dn * fsr_05sqrt2_dn);
  event.set(handle_isrfsr_sqrt2_up, isr_sqrt2_up * fsr_sqrt2_up);

  // up 2.0; down 0.5
  float isr_05_dn = event.genInfo->weights().at(26) / nominal;
  float isr_20_up = event.genInfo->weights().at(27) / nominal;
  float fsr_05_dn = event.genInfo->weights().at(4) / nominal;
  float fsr_20_up = event.genInfo->weights().at(5) / nominal;
  event.set(handle_isr_20_up, isr_20_up);
  event.set(handle_isr_05_dn, isr_05_dn);
  event.set(handle_fsr_20_up, fsr_20_up);
  event.set(handle_fsr_05_dn, fsr_05_dn);
  event.set(handle_isrfsr_05_dn, isr_05_dn * fsr_05_dn);
  event.set(handle_isrfsr_20_up, isr_20_up * fsr_20_up);

  // up 4.0; down 0.25
  float isr_025_dn = event.genInfo->weights().at(28) / nominal;
  float isr_40_up = event.genInfo->weights().at(29) / nominal;
  float fsr_025_dn = event.genInfo->weights().at(6) / nominal;
  float fsr_40_up = event.genInfo->weights().at(7) / nominal;
  event.set(handle_isr_40_up, isr_40_up);
  event.set(handle_isr_025_dn, isr_025_dn);
  event.set(handle_fsr_40_up, fsr_40_up);
  event.set(handle_fsr_025_dn, fsr_025_dn);
  event.set(handle_isrfsr_025_dn, isr_025_dn * fsr_025_dn);
  event.set(handle_isrfsr_40_up, isr_40_up * fsr_40_up);
}


void PSWeights::write_split_weights_from_geninfo(Event & event) {
  // The indexing is according to the documentation in:
  // https://twiki.cern.ch/twiki/bin/view/CMS/HowToPDF#Parton_shower_weights
  float nominal = event.genInfo->weights().at(0);

  float fsr_g2gg_dn = event.genInfo->weights().at(8) / nominal;
  float fsr_g2gg_up = event.genInfo->weights().at(9) / nominal;
  float fsr_g2qq_dn = event.genInfo->weights().at(10) / nominal;
  float fsr_g2qq_up = event.genInfo->weights().at(11) / nominal;
  float fsr_g2qg_dn = event.genInfo->weights().at(12) / nominal;
  float fsr_g2qg_up = event.genInfo->weights().at(13) / nominal;
  float fsr_g2xg_dn = event.genInfo->weights().at(14) / nominal;
  float fsr_g2xg_up = event.genInfo->weights().at(15) / nominal;

  float isr_g2gg_dn = event.genInfo->weights().at(30) / nominal;
  float isr_g2gg_up = event.genInfo->weights().at(31) / nominal;
  float isr_g2qq_dn = event.genInfo->weights().at(32) / nominal;
  float isr_g2qq_up = event.genInfo->weights().at(33) / nominal;
  float isr_g2qg_dn = event.genInfo->weights().at(34) / nominal;
  float isr_g2qg_up = event.genInfo->weights().at(35) / nominal;
  float isr_g2xg_dn = event.genInfo->weights().at(36) / nominal;
  float isr_g2xg_up = event.genInfo->weights().at(37) / nominal;

  event.set(handle_fsr_g2gg_dn, fsr_g2gg_dn);
  event.set(handle_fsr_g2gg_up, fsr_g2gg_up);
  event.set(handle_fsr_g2qq_dn, fsr_g2qq_dn);
  event.set(handle_fsr_g2qq_up, fsr_g2qq_up);
  event.set(handle_fsr_g2qg_dn, fsr_g2qg_dn);
  event.set(handle_fsr_g2qg_up, fsr_g2qg_up);
  event.set(handle_fsr_g2xg_dn, fsr_g2xg_dn);
  event.set(handle_fsr_g2xg_up, fsr_g2xg_up);

  event.set(handle_isr_g2gg_dn, isr_g2gg_dn);
  event.set(handle_isr_g2gg_up, isr_g2gg_up);
  event.set(handle_isr_g2qq_dn, isr_g2qq_dn);
  event.set(handle_isr_g2qq_up, isr_g2qq_up);
  event.set(handle_isr_g2qg_dn, isr_g2qg_dn);
  event.set(handle_isr_g2qg_up, isr_g2qg_up);
  event.set(handle_isr_g2xg_dn, isr_g2xg_dn);
  event.set(handle_isr_g2xg_up, isr_g2xg_up);
}


bool PSWeights::process(Event & event) {
  if(!is_mc){
    initialise_handles(event);
  }
  else{
    write_inclusive_weights_from_geninfo(event);
    write_split_weights_from_geninfo(event);
  }
  return true;
}
