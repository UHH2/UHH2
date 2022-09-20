#include "UHH2/common/include/PSWeights.h"

using namespace std;
using namespace uhh2;

PSWeights::PSWeights(Context & ctx, const bool do_split_weights_, const bool sample_without_ps_weights_):
  do_split_weights(do_split_weights_),
  sample_without_ps_weights(sample_without_ps_weights_)
{

  const string dataset_type = ctx.get("dataset_type");
  is_mc = dataset_type == "MC";

  // Inclusive Variations - up sqrt(2; down 1/sqrt(2)
  handle_isr_sqrt2_dn = ctx.declare_event_output<float>("weight_isr_sqrt2_down");
  handle_isr_sqrt2_up = ctx.declare_event_output<float>("weight_isr_sqrt2_up");
  handle_fsr_sqrt2_dn = ctx.declare_event_output<float>("weight_fsr_sqrt2_down");
  handle_fsr_sqrt2_up = ctx.declare_event_output<float>("weight_fsr_sqrt2_up");
  handle_isrfsr_sqrt2_dn = ctx.declare_event_output<float>("weight_isrfsr_sqrt2_down");
  handle_isrfsr_sqrt2_up = ctx.declare_event_output<float>("weight_isrfsr_sqrt2_up");
  // Inclusive Variations - up 2.0; down 0.5
  handle_isr_2_dn = ctx.declare_event_output<float>("weight_isr_2_down");
  handle_isr_2_up = ctx.declare_event_output<float>("weight_isr_2_up");
  handle_fsr_2_dn = ctx.declare_event_output<float>("weight_fsr_2_down");
  handle_fsr_2_up = ctx.declare_event_output<float>("weight_fsr_2_up");
  handle_isrfsr_2_dn = ctx.declare_event_output<float>("weight_isrfsr_2_down");
  handle_isrfsr_2_up = ctx.declare_event_output<float>("weight_isrfsr_2_up");
  // Inclusive Variations - up 4.0; down 0.25
  handle_isr_4_dn = ctx.declare_event_output<float>("weight_isr_4_down");
  handle_isr_4_up = ctx.declare_event_output<float>("weight_isr_4_up");
  handle_fsr_4_dn = ctx.declare_event_output<float>("weight_fsr_4_down");
  handle_fsr_4_up = ctx.declare_event_output<float>("weight_fsr_4_up");
  handle_isrfsr_4_dn = ctx.declare_event_output<float>("weight_isrfsr_4_down");
  handle_isrfsr_4_up = ctx.declare_event_output<float>("weight_isrfsr_4_up");

  if(do_split_weights) {
    // Split Variations -- fsr (muR)
    handle_fsr_g2gg_mur_dn = ctx.declare_event_output<float>("weight_fsr_g2gg_mur_down");
    handle_fsr_g2gg_mur_up = ctx.declare_event_output<float>("weight_fsr_g2gg_mur_up");
    handle_fsr_g2qq_mur_dn = ctx.declare_event_output<float>("weight_fsr_g2qq_mur_down");
    handle_fsr_g2qq_mur_up = ctx.declare_event_output<float>("weight_fsr_g2qq_mur_up");
    handle_fsr_q2qg_mur_dn = ctx.declare_event_output<float>("weight_fsr_q2qg_mur_down");
    handle_fsr_q2qg_mur_up = ctx.declare_event_output<float>("weight_fsr_q2qg_mur_up");
    handle_fsr_x2xg_mur_dn = ctx.declare_event_output<float>("weight_fsr_x2xg_mur_down");
    handle_fsr_x2xg_mur_up = ctx.declare_event_output<float>("weight_fsr_x2xg_mur_up");
    // Split Variations -- fsr (cNS)
    handle_fsr_g2gg_cns_dn = ctx.declare_event_output<float>("weight_fsr_g2gg_cns_down");
    handle_fsr_g2gg_cns_up = ctx.declare_event_output<float>("weight_fsr_g2gg_cns_up");
    handle_fsr_g2qq_cns_dn = ctx.declare_event_output<float>("weight_fsr_g2qq_cns_down");
    handle_fsr_g2qq_cns_up = ctx.declare_event_output<float>("weight_fsr_g2qq_cns_up");
    handle_fsr_q2qg_cns_dn = ctx.declare_event_output<float>("weight_fsr_q2qg_cns_down");
    handle_fsr_q2qg_cns_up = ctx.declare_event_output<float>("weight_fsr_q2qg_cns_up");
    handle_fsr_x2xg_cns_dn = ctx.declare_event_output<float>("weight_fsr_x2xg_cns_down");
    handle_fsr_x2xg_cns_up = ctx.declare_event_output<float>("weight_fsr_x2xg_cns_up");
    // Split Variations -- isr (muR)
    handle_isr_g2gg_mur_dn = ctx.declare_event_output<float>("weight_isr_g2gg_mur_down");
    handle_isr_g2gg_mur_up = ctx.declare_event_output<float>("weight_isr_g2gg_mur_up");
    handle_isr_g2qq_mur_dn = ctx.declare_event_output<float>("weight_isr_g2qq_mur_down");
    handle_isr_g2qq_mur_up = ctx.declare_event_output<float>("weight_isr_g2qq_mur_up");
    handle_isr_q2qg_mur_dn = ctx.declare_event_output<float>("weight_isr_q2qg_mur_down");
    handle_isr_q2qg_mur_up = ctx.declare_event_output<float>("weight_isr_q2qg_mur_up");
    handle_isr_x2xg_mur_dn = ctx.declare_event_output<float>("weight_isr_x2xg_mur_down");
    handle_isr_x2xg_mur_up = ctx.declare_event_output<float>("weight_isr_x2xg_mur_up");
    // Split Variations -- isr (cNS)
    handle_isr_g2gg_cns_dn = ctx.declare_event_output<float>("weight_isr_g2gg_cns_down");
    handle_isr_g2gg_cns_up = ctx.declare_event_output<float>("weight_isr_g2gg_cns_up");
    handle_isr_g2qq_cns_dn = ctx.declare_event_output<float>("weight_isr_g2qq_cns_down");
    handle_isr_g2qq_cns_up = ctx.declare_event_output<float>("weight_isr_g2qq_cns_up");
    handle_isr_q2qg_cns_dn = ctx.declare_event_output<float>("weight_isr_q2qg_cns_down");
    handle_isr_q2qg_cns_up = ctx.declare_event_output<float>("weight_isr_q2qg_cns_up");
    handle_isr_x2xg_cns_dn = ctx.declare_event_output<float>("weight_isr_x2xg_cns_down");
    handle_isr_x2xg_cns_up = ctx.declare_event_output<float>("weight_isr_x2xg_cns_up");
  }

  if(!is_mc){
    cout << "Warning: PSWeights will not have an effect on this non-MC sample (dataset_type = '" + dataset_type + "'). Will only write dummy weights" << endl;
    return;
  }
  if(sample_without_ps_weights){
    cout << "Warning: PSWeights will not have an effect on this sample since you specified that this sample has no PS weights stored. Will only write dummy weights" << endl;
    return;
  }
}


void PSWeights::initialise_handles(Event & event){
  // Inclusive
  event.set(handle_isr_sqrt2_dn, 1.0);
  event.set(handle_isr_sqrt2_up, 1.0);
  event.set(handle_fsr_sqrt2_dn, 1.0);
  event.set(handle_fsr_sqrt2_up, 1.0);
  event.set(handle_isrfsr_sqrt2_dn, 1.0);
  event.set(handle_isrfsr_sqrt2_up, 1.0);

  event.set(handle_isr_2_dn, 1.0);
  event.set(handle_isr_2_up, 1.0);
  event.set(handle_fsr_2_dn, 1.0);
  event.set(handle_fsr_2_up, 1.0);
  event.set(handle_isrfsr_2_dn, 1.0);
  event.set(handle_isrfsr_2_up, 1.0);

  event.set(handle_isr_4_dn, 1.0);
  event.set(handle_isr_4_up, 1.0);
  event.set(handle_fsr_4_dn, 1.0);
  event.set(handle_fsr_4_up, 1.0);
  event.set(handle_isrfsr_4_dn, 1.0);
  event.set(handle_isrfsr_4_up, 1.0);

  // Split
  if(do_split_weights) {
    event.set(handle_fsr_g2gg_mur_dn, 1.0);
    event.set(handle_fsr_g2gg_mur_up, 1.0);
    event.set(handle_fsr_g2qq_mur_dn, 1.0);
    event.set(handle_fsr_g2qq_mur_up, 1.0);
    event.set(handle_fsr_q2qg_mur_dn, 1.0);
    event.set(handle_fsr_q2qg_mur_up, 1.0);
    event.set(handle_fsr_x2xg_mur_dn, 1.0);
    event.set(handle_fsr_x2xg_mur_up, 1.0);

    event.set(handle_fsr_g2gg_cns_dn, 1.0);
    event.set(handle_fsr_g2gg_cns_up, 1.0);
    event.set(handle_fsr_g2qq_cns_dn, 1.0);
    event.set(handle_fsr_g2qq_cns_up, 1.0);
    event.set(handle_fsr_q2qg_cns_dn, 1.0);
    event.set(handle_fsr_q2qg_cns_up, 1.0);
    event.set(handle_fsr_x2xg_cns_dn, 1.0);
    event.set(handle_fsr_x2xg_cns_up, 1.0);

    event.set(handle_isr_g2gg_mur_dn, 1.0);
    event.set(handle_isr_g2gg_mur_up, 1.0);
    event.set(handle_isr_g2qq_mur_dn, 1.0);
    event.set(handle_isr_g2qq_mur_up, 1.0);
    event.set(handle_isr_q2qg_mur_dn, 1.0);
    event.set(handle_isr_q2qg_mur_up, 1.0);
    event.set(handle_isr_x2xg_mur_dn, 1.0);
    event.set(handle_isr_x2xg_mur_up, 1.0);

    event.set(handle_isr_g2gg_cns_dn, 1.0);
    event.set(handle_isr_g2gg_cns_up, 1.0);
    event.set(handle_isr_g2qq_cns_dn, 1.0);
    event.set(handle_isr_g2qq_cns_up, 1.0);
    event.set(handle_isr_q2qg_cns_dn, 1.0);
    event.set(handle_isr_q2qg_cns_up, 1.0);
    event.set(handle_isr_x2xg_cns_dn, 1.0);
    event.set(handle_isr_x2xg_cns_up, 1.0);
  }
}


void PSWeights::write_inclusive_weights_from_geninfo(Event & event) {
  // The indexing is according to the documentation in:
  // https://twiki.cern.ch/twiki/bin/view/CMS/HowToPDF#Parton_shower_weights
  const float nominal = event.genInfo->weights().at(0);

  // up sqrt(2); down 1/sqrt(2)
  const float isr_sqrt2_dn = event.genInfo->weights().at(24) / nominal;
  const float isr_sqrt2_up = event.genInfo->weights().at(25) / nominal;
  const float fsr_sqrt2_dn = event.genInfo->weights().at(2) / nominal;
  const float fsr_sqrt2_up = event.genInfo->weights().at(3) / nominal;
  event.set(handle_isr_sqrt2_dn, isr_sqrt2_dn);
  event.set(handle_isr_sqrt2_up, isr_sqrt2_up);
  event.set(handle_fsr_sqrt2_dn, fsr_sqrt2_dn);
  event.set(handle_fsr_sqrt2_up, fsr_sqrt2_up);
  event.set(handle_isrfsr_sqrt2_dn, isr_sqrt2_dn * fsr_sqrt2_dn);
  event.set(handle_isrfsr_sqrt2_up, isr_sqrt2_up * fsr_sqrt2_up);

  // up 2.0; down 0.5
  const float isr_2_dn = event.genInfo->weights().at(26) / nominal;
  const float isr_2_up = event.genInfo->weights().at(27) / nominal;
  const float fsr_2_dn = event.genInfo->weights().at(4) / nominal;
  const float fsr_2_up = event.genInfo->weights().at(5) / nominal;
  event.set(handle_isr_2_dn, isr_2_dn);
  event.set(handle_isr_2_up, isr_2_up);
  event.set(handle_fsr_2_dn, fsr_2_dn);
  event.set(handle_fsr_2_up, fsr_2_up);
  event.set(handle_isrfsr_2_dn, isr_2_dn * fsr_2_dn);
  event.set(handle_isrfsr_2_up, isr_2_up * fsr_2_up);

  // up 4.0; down 0.25
  const float isr_4_dn = event.genInfo->weights().at(28) / nominal;
  const float isr_4_up = event.genInfo->weights().at(29) / nominal;
  const float fsr_4_dn = event.genInfo->weights().at(6) / nominal;
  const float fsr_4_up = event.genInfo->weights().at(7) / nominal;
  event.set(handle_isr_4_dn, isr_4_dn);
  event.set(handle_isr_4_up, isr_4_up);
  event.set(handle_fsr_4_dn, fsr_4_dn);
  event.set(handle_fsr_4_up, fsr_4_up);
  event.set(handle_isrfsr_4_dn, isr_4_dn * fsr_4_dn);
  event.set(handle_isrfsr_4_up, isr_4_up * fsr_4_up);
}


void PSWeights::write_split_weights_from_geninfo(Event & event) {
  // The indexing is according to the documentation in:
  // https://twiki.cern.ch/twiki/bin/view/CMS/HowToPDF#Parton_shower_weights
  const float nominal = event.genInfo->weights().at(0);

  const float fsr_g2gg_mur_dn = event.genInfo->weights().at(8) / nominal;
  const float fsr_g2gg_mur_up = event.genInfo->weights().at(9) / nominal;
  const float fsr_g2qq_mur_dn = event.genInfo->weights().at(10) / nominal;
  const float fsr_g2qq_mur_up = event.genInfo->weights().at(11) / nominal;
  const float fsr_q2qg_mur_dn = event.genInfo->weights().at(12) / nominal;
  const float fsr_q2qg_mur_up = event.genInfo->weights().at(13) / nominal;
  const float fsr_x2xg_mur_dn = event.genInfo->weights().at(14) / nominal;
  const float fsr_x2xg_mur_up = event.genInfo->weights().at(15) / nominal;

  const float fsr_g2gg_cns_dn = event.genInfo->weights().at(16) / nominal;
  const float fsr_g2gg_cns_up = event.genInfo->weights().at(17) / nominal;
  const float fsr_g2qq_cns_dn = event.genInfo->weights().at(18) / nominal;
  const float fsr_g2qq_cns_up = event.genInfo->weights().at(19) / nominal;
  const float fsr_q2qg_cns_dn = event.genInfo->weights().at(20) / nominal;
  const float fsr_q2qg_cns_up = event.genInfo->weights().at(21) / nominal;
  const float fsr_x2xg_cns_dn = event.genInfo->weights().at(22) / nominal;
  const float fsr_x2xg_cns_up = event.genInfo->weights().at(23) / nominal;

  const float isr_g2gg_mur_dn = event.genInfo->weights().at(30) / nominal;
  const float isr_g2gg_mur_up = event.genInfo->weights().at(31) / nominal;
  const float isr_g2qq_mur_dn = event.genInfo->weights().at(32) / nominal;
  const float isr_g2qq_mur_up = event.genInfo->weights().at(33) / nominal;
  const float isr_q2qg_mur_dn = event.genInfo->weights().at(34) / nominal;
  const float isr_q2qg_mur_up = event.genInfo->weights().at(35) / nominal;
  const float isr_x2xg_mur_dn = event.genInfo->weights().at(36) / nominal;
  const float isr_x2xg_mur_up = event.genInfo->weights().at(37) / nominal;

  const float isr_g2gg_cns_dn = event.genInfo->weights().at(38) / nominal;
  const float isr_g2gg_cns_up = event.genInfo->weights().at(39) / nominal;
  const float isr_g2qq_cns_dn = event.genInfo->weights().at(40) / nominal;
  const float isr_g2qq_cns_up = event.genInfo->weights().at(41) / nominal;
  const float isr_q2qg_cns_dn = event.genInfo->weights().at(42) / nominal;
  const float isr_q2qg_cns_up = event.genInfo->weights().at(43) / nominal;
  const float isr_x2xg_cns_dn = event.genInfo->weights().at(44) / nominal;
  const float isr_x2xg_cns_up = event.genInfo->weights().at(45) / nominal;

  event.set(handle_fsr_g2gg_mur_dn, fsr_g2gg_mur_dn);
  event.set(handle_fsr_g2gg_mur_up, fsr_g2gg_mur_up);
  event.set(handle_fsr_g2qq_mur_dn, fsr_g2qq_mur_dn);
  event.set(handle_fsr_g2qq_mur_up, fsr_g2qq_mur_up);
  event.set(handle_fsr_q2qg_mur_dn, fsr_q2qg_mur_dn);
  event.set(handle_fsr_q2qg_mur_up, fsr_q2qg_mur_up);
  event.set(handle_fsr_x2xg_mur_dn, fsr_x2xg_mur_dn);
  event.set(handle_fsr_x2xg_mur_up, fsr_x2xg_mur_up);

  event.set(handle_fsr_g2gg_cns_dn, fsr_g2gg_cns_dn);
  event.set(handle_fsr_g2gg_cns_up, fsr_g2gg_cns_up);
  event.set(handle_fsr_g2qq_cns_dn, fsr_g2qq_cns_dn);
  event.set(handle_fsr_g2qq_cns_up, fsr_g2qq_cns_up);
  event.set(handle_fsr_q2qg_cns_dn, fsr_q2qg_cns_dn);
  event.set(handle_fsr_q2qg_cns_up, fsr_q2qg_cns_up);
  event.set(handle_fsr_x2xg_cns_dn, fsr_x2xg_cns_dn);
  event.set(handle_fsr_x2xg_cns_up, fsr_x2xg_cns_up);

  event.set(handle_isr_g2gg_mur_dn, isr_g2gg_mur_dn);
  event.set(handle_isr_g2gg_mur_up, isr_g2gg_mur_up);
  event.set(handle_isr_g2qq_mur_dn, isr_g2qq_mur_dn);
  event.set(handle_isr_g2qq_mur_up, isr_g2qq_mur_up);
  event.set(handle_isr_q2qg_mur_dn, isr_q2qg_mur_dn);
  event.set(handle_isr_q2qg_mur_up, isr_q2qg_mur_up);
  event.set(handle_isr_x2xg_mur_dn, isr_x2xg_mur_dn);
  event.set(handle_isr_x2xg_mur_up, isr_x2xg_mur_up);

  event.set(handle_isr_g2gg_cns_dn, isr_g2gg_cns_dn);
  event.set(handle_isr_g2gg_cns_up, isr_g2gg_cns_up);
  event.set(handle_isr_g2qq_cns_dn, isr_g2qq_cns_dn);
  event.set(handle_isr_g2qq_cns_up, isr_g2qq_cns_up);
  event.set(handle_isr_q2qg_cns_dn, isr_q2qg_cns_dn);
  event.set(handle_isr_q2qg_cns_up, isr_q2qg_cns_up);
  event.set(handle_isr_x2xg_cns_dn, isr_x2xg_cns_dn);
  event.set(handle_isr_x2xg_cns_up, isr_x2xg_cns_up);
}


bool PSWeights::process(Event & event) {
  if(!is_mc || sample_without_ps_weights){
    initialise_handles(event);
  }
  else{
    write_inclusive_weights_from_geninfo(event);
    if(do_split_weights) write_split_weights_from_geninfo(event);
  }
  return true;
}
