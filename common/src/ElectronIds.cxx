#include "UHH2/common/include/ElectronIds.h"

namespace {

// The values to cut on. Corresponds to the tables in the twiki:
// in the tables at https://twiki.cern.ch/twiki/bin/view/CMS/CutBasedElectronIdentificationRun2
// following the same order of variables as in the CSA14 section.
// The only difference to the twiki tables is the passes_conversion_rejection flag which replaces the 'Conversion rejection: vertex fit probability'
// and is treated as a boolean here.
  struct ele_cutvalues {
    float abs_dEtaIn, abs_dPhiIn, full5x5_sigmaIetaIeta, HoverE, fabs_d0, fabs_dz, fabs_1oE_1op, pfiso_dbeta_dr03, passes_conversion_rejection, cr_misshits;
  };

  ele_cutvalues cv_from_ele(const Electron& ele, const uhh2::Event& event){

    ele_cutvalues result;
    assert(event.pvs);
    if(event.pvs->size() == 0){
      result.abs_dEtaIn = std::numeric_limits<float>::infinity();
      return result;
    }

    const auto& pv = (*event.pvs)[0];

    result.abs_dEtaIn = std::abs(ele.dEtaIn());
    result.abs_dPhiIn = std::abs(ele.dPhiIn());
    result.full5x5_sigmaIetaIeta = ele.sigmaIEtaIEta();
    result.HoverE = ele.HoverE();
    result.fabs_d0 = std::abs(ele.gsfTrack_dxy_vertex(pv.x(), pv.y()));
    result.fabs_dz = std::abs(ele.gsfTrack_dz_vertex(pv.x(), pv.y(), pv.z()));
    result.fabs_1oE_1op = (ele.EcalEnergy() > 0. ? (std::abs(1.0f/ele.EcalEnergy() - ele.EoverPIn()/ele.EcalEnergy())) : std::numeric_limits<float>::infinity());
    result.pfiso_dbeta_dr03 = ele.relIsodb();
    result.cr_misshits = ele.gsfTrack_trackerExpectedHitsInner_numberOfLostHits();
    result.passes_conversion_rejection = ele.passconversionveto() ? 2.0f : 0.0f;

    return result;
  }

  bool passes_thresholds(const ele_cutvalues& vals, const ele_cutvalues& thresholds){

    if(vals.abs_dEtaIn >= thresholds.abs_dEtaIn) return false;
    if(vals.abs_dPhiIn >= thresholds.abs_dPhiIn) return false;
    if(vals.full5x5_sigmaIetaIeta >= thresholds.full5x5_sigmaIetaIeta) return false;
    if(vals.HoverE >= thresholds.HoverE) return false;
    if(vals.fabs_d0 >= thresholds.fabs_d0) return false;
    if(vals.fabs_dz >= thresholds.fabs_dz) return false;
    if(vals.fabs_1oE_1op >= thresholds.fabs_1oE_1op) return false;
    if(vals.pfiso_dbeta_dr03 >= thresholds.pfiso_dbeta_dr03) return false;
    if(vals.passes_conversion_rejection < thresholds.passes_conversion_rejection) return false; // note: for passes conversion rejection, comparison is inverted
    if(vals.cr_misshits > thresholds.cr_misshits) return false; // note for missing hits: >, not >=

    return true;
  }

  // check whether electron passes the given id, based on cut values given for barrel and endcap separately.
  bool passes_id(const Electron & ele, const uhh2::Event & event, const ele_cutvalues & thresh_barrel, const ele_cutvalues & thresh_endcap){

    bool pass(false);

    auto abs_eta = std::abs(ele.supercluster_eta());
    if                        (abs_eta <= 1.479) pass = passes_thresholds(cv_from_ele(ele, event), thresh_barrel);
    else if(1.479 < abs_eta && abs_eta <  2.5  ) pass = passes_thresholds(cv_from_ele(ele, event), thresh_endcap);

    return pass;
  }

}

bool ElectronID_PHYS14_25ns_veto(const Electron& electron, const uhh2::Event& event){

  static constexpr const auto thresholds_barrel = ele_cutvalues{
    .abs_dEtaIn                  = 0.013625,
    .abs_dPhiIn                  = 0.230374,
    .full5x5_sigmaIetaIeta       = 0.011586,
    .HoverE                      = 0.181130,
    .fabs_d0                     = 0.094095,
    .fabs_dz                     = 0.713070,
    .fabs_1oE_1op                = 0.295751,
    .pfiso_dbeta_dr03            = 0.158721,
    .passes_conversion_rejection = 1.      ,
    .cr_misshits                 = 2.      
  };

  static constexpr const auto thresholds_endcap = ele_cutvalues{
    .abs_dEtaIn                  = 0.011932,
    .abs_dPhiIn                  = 0.255450,
    .full5x5_sigmaIetaIeta       = 0.031849,
    .HoverE                      = 0.223870,
    .fabs_d0                     = 0.342293,
    .fabs_dz                     = 0.953461,
    .fabs_1oE_1op                = 0.155501,
    .pfiso_dbeta_dr03            = 0.177032,
    .passes_conversion_rejection = 1.      ,
    .cr_misshits                 = 3.      
  };

  return passes_id(electron, event, thresholds_barrel, thresholds_endcap);
}

bool ElectronID_PHYS14_25ns_loose(const Electron& electron, const uhh2::Event& event){

  static constexpr const auto thresholds_barrel = ele_cutvalues{
    .abs_dEtaIn                  = 0.009277,
    .abs_dPhiIn                  = 0.094739,
    .full5x5_sigmaIetaIeta       = 0.010331,
    .HoverE                      = 0.093068,
    .fabs_d0                     = 0.035904,
    .fabs_dz                     = 0.075496,
    .fabs_1oE_1op                = 0.189968,
    .pfiso_dbeta_dr03            = 0.130136,
    .passes_conversion_rejection = 1.      ,
    .cr_misshits                 = 1.      
  };

  static constexpr const auto thresholds_endcap = ele_cutvalues{
    .abs_dEtaIn                  = 0.009833,
    .abs_dPhiIn                  = 0.149934,
    .full5x5_sigmaIetaIeta       = 0.031838,
    .HoverE                      = 0.115754,
    .fabs_d0                     = 0.099266,
    .fabs_dz                     = 0.197897,
    .fabs_1oE_1op                = 0.140662,
    .pfiso_dbeta_dr03            = 0.163368,
    .passes_conversion_rejection = 1.      ,
    .cr_misshits                 = 1.      
  };

  return passes_id(electron, event, thresholds_barrel, thresholds_endcap);
}


bool ElectronID_PHYS14_25ns_medium(const Electron& electron, const uhh2::Event& event){

  static constexpr const auto thresholds_barrel = ele_cutvalues{
    .abs_dEtaIn                  = 0.008925,
    .abs_dPhiIn                  = 0.035973,
    .full5x5_sigmaIetaIeta       = 0.009996,
    .HoverE                      = 0.050537,
    .fabs_d0                     = 0.012235,
    .fabs_dz                     = 0.042020,
    .fabs_1oE_1op                = 0.091942,
    .pfiso_dbeta_dr03            = 0.107587,
    .passes_conversion_rejection = 1.      ,
    .cr_misshits                 = 1.      
  };

  static constexpr const auto thresholds_endcap = ele_cutvalues{
    .abs_dEtaIn                  = 0.007429,
    .abs_dPhiIn                  = 0.067879,
    .full5x5_sigmaIetaIeta       = 0.030135,
    .HoverE                      = 0.086782,
    .fabs_d0                     = 0.036719,
    .fabs_dz                     = 0.138142,
    .fabs_1oE_1op                = 0.100683,
    .pfiso_dbeta_dr03            = 0.113254,
    .passes_conversion_rejection = 1.      ,
    .cr_misshits                 = 1.      
  };

  return passes_id(electron, event, thresholds_barrel, thresholds_endcap);
}


bool ElectronID_PHYS14_25ns_tight(const Electron& electron, const uhh2::Event& event){

  static constexpr const auto thresholds_barrel = ele_cutvalues{
    .abs_dEtaIn                  = 0.006046,
    .abs_dPhiIn                  = 0.028092,
    .full5x5_sigmaIetaIeta       = 0.009947,
    .HoverE                      = 0.045772,
    .fabs_d0                     = 0.008790,
    .fabs_dz                     = 0.021226,
    .fabs_1oE_1op                = 0.020118,
    .pfiso_dbeta_dr03            = 0.069537,
    .passes_conversion_rejection = 1.      ,
    .cr_misshits                 = 1.      
  };

  static constexpr const auto thresholds_endcap = ele_cutvalues{
    .abs_dEtaIn                  = 0.007057,
    .abs_dPhiIn                  = 0.030159,
    .full5x5_sigmaIetaIeta       = 0.028237,
    .HoverE                      = 0.067778,
    .fabs_d0                     = 0.027984,
    .fabs_dz                     = 0.133431,
    .fabs_1oE_1op                = 0.098919,
    .pfiso_dbeta_dr03            = 0.078265,
    .passes_conversion_rejection = 1.      ,
    .cr_misshits                 = 1.      
  };

  return passes_id(electron, event, thresholds_barrel, thresholds_endcap);
}

// Electron Cut-Based ID without cut on PF (relative) isolation

bool ElectronID_PHYS14_25ns_veto_noIso(const Electron& electron, const uhh2::Event& event){

  static constexpr const auto thresholds_barrel = ele_cutvalues{
    .abs_dEtaIn                  = 0.013625,
    .abs_dPhiIn                  = 0.230374,
    .full5x5_sigmaIetaIeta       = 0.011586,
    .HoverE                      = 0.181130,
    .fabs_d0                     = 0.094095,
    .fabs_dz                     = 0.713070,
    .fabs_1oE_1op                = 0.295751,
    .pfiso_dbeta_dr03            = std::numeric_limits<float>::infinity(),
    .passes_conversion_rejection = 1.      ,
    .cr_misshits                 = 2.      
  };

  static constexpr const auto thresholds_endcap = ele_cutvalues{
    .abs_dEtaIn                  = 0.011932,
    .abs_dPhiIn                  = 0.255450,
    .full5x5_sigmaIetaIeta       = 0.031849,
    .HoverE                      = 0.223870,
    .fabs_d0                     = 0.342293,
    .fabs_dz                     = 0.953461,
    .fabs_1oE_1op                = 0.155501,
    .pfiso_dbeta_dr03            = std::numeric_limits<float>::infinity(),
    .passes_conversion_rejection = 1.      ,
    .cr_misshits                 = 3.      
  };

  return passes_id(electron, event, thresholds_barrel, thresholds_endcap);
}

bool ElectronID_PHYS14_25ns_loose_noIso(const Electron& electron, const uhh2::Event& event){

  static constexpr const auto thresholds_barrel = ele_cutvalues{
    .abs_dEtaIn                  = 0.009277,
    .abs_dPhiIn                  = 0.094739,
    .full5x5_sigmaIetaIeta       = 0.010331,
    .HoverE                      = 0.093068,
    .fabs_d0                     = 0.035904,
    .fabs_dz                     = 0.075496,
    .fabs_1oE_1op                = 0.189968,
    .pfiso_dbeta_dr03            = std::numeric_limits<float>::infinity(),
    .passes_conversion_rejection = 1.      ,
    .cr_misshits                 = 1.      
  };

  static constexpr const auto thresholds_endcap = ele_cutvalues{
    .abs_dEtaIn                  = 0.009833,
    .abs_dPhiIn                  = 0.149934,
    .full5x5_sigmaIetaIeta       = 0.031838,
    .HoverE                      = 0.115754,
    .fabs_d0                     = 0.099266,
    .fabs_dz                     = 0.197897,
    .fabs_1oE_1op                = 0.140662,
    .pfiso_dbeta_dr03            = std::numeric_limits<float>::infinity(),
    .passes_conversion_rejection = 1.      ,
    .cr_misshits                 = 1.      
  };

  return passes_id(electron, event, thresholds_barrel, thresholds_endcap);
}


bool ElectronID_PHYS14_25ns_medium_noIso(const Electron& electron, const uhh2::Event& event){

  static constexpr const auto thresholds_barrel = ele_cutvalues{
    .abs_dEtaIn                  = 0.008925,
    .abs_dPhiIn                  = 0.035973,
    .full5x5_sigmaIetaIeta       = 0.009996,
    .HoverE                      = 0.050537,
    .fabs_d0                     = 0.012235,
    .fabs_dz                     = 0.042020,
    .fabs_1oE_1op                = 0.091942,
    .pfiso_dbeta_dr03            = std::numeric_limits<float>::infinity(),
    .passes_conversion_rejection = 1.      ,
    .cr_misshits                 = 1.      
  };

  static constexpr const auto thresholds_endcap = ele_cutvalues{
    .abs_dEtaIn                  = 0.007429,
    .abs_dPhiIn                  = 0.067879,
    .full5x5_sigmaIetaIeta       = 0.030135,
    .HoverE                      = 0.086782,
    .fabs_d0                     = 0.036719,
    .fabs_dz                     = 0.138142,
    .fabs_1oE_1op                = 0.100683,
    .pfiso_dbeta_dr03            = std::numeric_limits<float>::infinity(),
    .passes_conversion_rejection = 1.      ,
    .cr_misshits                 = 1.      
  };

  return passes_id(electron, event, thresholds_barrel, thresholds_endcap);
}

bool ElectronID_PHYS14_25ns_tight_noIso(const Electron& electron, const uhh2::Event& event){

  static constexpr const auto thresholds_barrel = ele_cutvalues{
    .abs_dEtaIn                  = 0.006046,
    .abs_dPhiIn                  = 0.028092,
    .full5x5_sigmaIetaIeta       = 0.009947,
    .HoverE                      = 0.045772,
    .fabs_d0                     = 0.008790,
    .fabs_dz                     = 0.021226,
    .fabs_1oE_1op                = 0.020118,
    .pfiso_dbeta_dr03            = std::numeric_limits<float>::infinity(),
    .passes_conversion_rejection = 1.      ,
    .cr_misshits                 = 1.      
  };

  static constexpr const auto thresholds_endcap = ele_cutvalues{
    .abs_dEtaIn                  = 0.007057,
    .abs_dPhiIn                  = 0.030159,
    .full5x5_sigmaIetaIeta       = 0.028237,
    .HoverE                      = 0.067778,
    .fabs_d0                     = 0.027984,
    .fabs_dz                     = 0.133431,
    .fabs_1oE_1op                = 0.098919,
    .pfiso_dbeta_dr03            = std::numeric_limits<float>::infinity(),
    .passes_conversion_rejection = 1.      ,
    .cr_misshits                 = 1.      
  };

  return passes_id(electron, event, thresholds_barrel, thresholds_endcap);
}

bool ElectronID_Spring15_25ns_veto(const Electron& electron, const uhh2::Event& event){

  static constexpr const auto thresholds_barrel = ele_cutvalues{
    .abs_dEtaIn                  = 0.0152,
    .abs_dPhiIn                  = 0.216,
    .full5x5_sigmaIetaIeta       = 0.0114,
    .HoverE                      = 0.181,
    .fabs_d0                     = 0.0564,
    .fabs_dz                     = 0.472,
    .fabs_1oE_1op                = 0.207,
    .pfiso_dbeta_dr03            = 0.126,
    .passes_conversion_rejection = 1.    ,
    .cr_misshits                 = 2.
  };

  static constexpr const auto thresholds_endcap = ele_cutvalues{
    .abs_dEtaIn                  = 0.0113,
    .abs_dPhiIn                  = 0.237,
    .full5x5_sigmaIetaIeta       = 0.0352,
    .HoverE                      = 0.116,
    .fabs_d0                     = 0.222,
    .fabs_dz                     = 0.921,
    .fabs_1oE_1op                = 0.174,
    .pfiso_dbeta_dr03            = 0.144,
    .passes_conversion_rejection = 1.      ,
    .cr_misshits                 = 3.
  };

  return passes_id(electron, event, thresholds_barrel, thresholds_endcap);
}

bool ElectronID_Spring15_25ns_loose(const Electron& electron, const uhh2::Event& event){

  static constexpr const auto thresholds_barrel = ele_cutvalues{
    .abs_dEtaIn                  = 0.0105,
    .abs_dPhiIn                  = 0.115,
    .full5x5_sigmaIetaIeta       = 0.0103,
    .HoverE                      = 0.104,
    .fabs_d0                     = 0.0261,
    .fabs_dz                     = 0.41,
    .fabs_1oE_1op                = 0.102,
    .pfiso_dbeta_dr03            = 0.0893,
    .passes_conversion_rejection = 1.    ,
    .cr_misshits                 = 2.
  };

  static constexpr const auto thresholds_endcap = ele_cutvalues{
    .abs_dEtaIn                  = 0.00814,
    .abs_dPhiIn                  = 0.182,
    .full5x5_sigmaIetaIeta       = 0.0301,
    .HoverE                      = 0.0897,
    .fabs_d0                     = 0.118,
    .fabs_dz                     = 0.822,
    .fabs_1oE_1op                = 0.126,
    .pfiso_dbeta_dr03            = 0.121,
    .passes_conversion_rejection = 1.      ,
    .cr_misshits                 = 1.
  };

  return passes_id(electron, event, thresholds_barrel, thresholds_endcap);
}

bool ElectronID_Spring15_25ns_medium(const Electron& electron, const uhh2::Event& event){

  static constexpr const auto thresholds_barrel = ele_cutvalues{
    .abs_dEtaIn                  = 0.0103,
    .abs_dPhiIn                  = 0.0336,
    .full5x5_sigmaIetaIeta       = 0.0101,
    .HoverE                      = 0.0876,
    .fabs_d0                     = 0.0118,
    .fabs_dz                     = 0.373,
    .fabs_1oE_1op                = 0.0174,
    .pfiso_dbeta_dr03            = 0.0766,
    .passes_conversion_rejection = 1.    ,
    .cr_misshits                 = 2.
  };

  static constexpr const auto thresholds_endcap = ele_cutvalues{
    .abs_dEtaIn                  = 0.00733,
    .abs_dPhiIn                  = 0.114,
    .full5x5_sigmaIetaIeta       = 0.0283,
    .HoverE                      = 0.0678,
    .fabs_d0                     = 0.0739,
    .fabs_dz                     = 0.602,
    .fabs_1oE_1op                = 0.0898,
    .pfiso_dbeta_dr03            = 0.0678,
    .passes_conversion_rejection = 1.      ,
    .cr_misshits                 = 1.
  };

  return passes_id(electron, event, thresholds_barrel, thresholds_endcap);
}

bool ElectronID_Spring15_25ns_tight(const Electron& electron, const uhh2::Event& event){

  static constexpr const auto thresholds_barrel = ele_cutvalues{
    .abs_dEtaIn                  = 0.00926,
    .abs_dPhiIn                  = 0.0336,
    .full5x5_sigmaIetaIeta       = 0.0101,
    .HoverE                      = 0.0597,
    .fabs_d0                     = 0.0111,
    .fabs_dz                     = 0.0466,
    .fabs_1oE_1op                = 0.012,
    .pfiso_dbeta_dr03            = 0.0354,
    .passes_conversion_rejection = 1.    ,
    .cr_misshits                 = 2.
  };

  static constexpr const auto thresholds_endcap = ele_cutvalues{
    .abs_dEtaIn                  = 0.00724,
    .abs_dPhiIn                  = 0.0918,
    .full5x5_sigmaIetaIeta       = 0.0279,
    .HoverE                      = 0.0615,
    .fabs_d0                     = 0.0351,
    .fabs_dz                     = 0.417,
    .fabs_1oE_1op                = 0.00999,
    .pfiso_dbeta_dr03            = 0.0646,
    .passes_conversion_rejection = 1.      ,
    .cr_misshits                 = 1.
  };

  return passes_id(electron, event, thresholds_barrel, thresholds_endcap);
}

bool ElectronID_Spring15_25ns_veto_noIso(const Electron& electron, const uhh2::Event& event){

  static constexpr const auto thresholds_barrel = ele_cutvalues{
    .abs_dEtaIn                  = 0.0152,
    .abs_dPhiIn                  = 0.216,
    .full5x5_sigmaIetaIeta       = 0.0114,
    .HoverE                      = 0.181,
    .fabs_d0                     = 0.0564,
    .fabs_dz                     = 0.472,
    .fabs_1oE_1op                = 0.207,
    .pfiso_dbeta_dr03            = std::numeric_limits<float>::infinity(),
    .passes_conversion_rejection = 1.    ,
    .cr_misshits                 = 2.
  };

  static constexpr const auto thresholds_endcap = ele_cutvalues{
    .abs_dEtaIn                  = 0.0113,
    .abs_dPhiIn                  = 0.237,
    .full5x5_sigmaIetaIeta       = 0.0352,
    .HoverE                      = 0.116,
    .fabs_d0                     = 0.222,
    .fabs_dz                     = 0.921,
    .fabs_1oE_1op                = 0.174,
    .pfiso_dbeta_dr03            = std::numeric_limits<float>::infinity(),
    .passes_conversion_rejection = 1.      ,
    .cr_misshits                 = 3.
  };

  return passes_id(electron, event, thresholds_barrel, thresholds_endcap);
}

bool ElectronID_Spring15_25ns_loose_noIso(const Electron& electron, const uhh2::Event& event){

  static constexpr const auto thresholds_barrel = ele_cutvalues{
    .abs_dEtaIn                  = 0.0105,
    .abs_dPhiIn                  = 0.115,
    .full5x5_sigmaIetaIeta       = 0.0103,
    .HoverE                      = 0.104,
    .fabs_d0                     = 0.0261,
    .fabs_dz                     = 0.41,
    .fabs_1oE_1op                = 0.102,
    .pfiso_dbeta_dr03            = std::numeric_limits<float>::infinity(),
    .passes_conversion_rejection = 1.    ,
    .cr_misshits                 = 2.
  };

  static constexpr const auto thresholds_endcap = ele_cutvalues{
    .abs_dEtaIn                  = 0.00814,
    .abs_dPhiIn                  = 0.182,
    .full5x5_sigmaIetaIeta       = 0.0301,
    .HoverE                      = 0.0897,
    .fabs_d0                     = 0.118,
    .fabs_dz                     = 0.822,
    .fabs_1oE_1op                = 0.126,
    .pfiso_dbeta_dr03            = std::numeric_limits<float>::infinity(),
    .passes_conversion_rejection = 1.      ,
    .cr_misshits                 = 1.
  };

  return passes_id(electron, event, thresholds_barrel, thresholds_endcap);
}

bool ElectronID_Spring15_25ns_medium_noIso(const Electron& electron, const uhh2::Event& event){

  static constexpr const auto thresholds_barrel = ele_cutvalues{
    .abs_dEtaIn                  = 0.0103,
    .abs_dPhiIn                  = 0.0336,
    .full5x5_sigmaIetaIeta       = 0.0101,
    .HoverE                      = 0.0876,
    .fabs_d0                     = 0.0118,
    .fabs_dz                     = 0.373,
    .fabs_1oE_1op                = 0.0174,
    .pfiso_dbeta_dr03            = std::numeric_limits<float>::infinity(),
    .passes_conversion_rejection = 1.    ,
    .cr_misshits                 = 2.
  };

  static constexpr const auto thresholds_endcap = ele_cutvalues{
    .abs_dEtaIn                  = 0.00733,
    .abs_dPhiIn                  = 0.114,
    .full5x5_sigmaIetaIeta       = 0.0283,
    .HoverE                      = 0.0678,
    .fabs_d0                     = 0.0739,
    .fabs_dz                     = 0.602,
    .fabs_1oE_1op                = 0.0898,
    .pfiso_dbeta_dr03            = std::numeric_limits<float>::infinity(),
    .passes_conversion_rejection = 1.      ,
    .cr_misshits                 = 1.
  };

  return passes_id(electron, event, thresholds_barrel, thresholds_endcap);
}

bool ElectronID_Spring15_25ns_tight_noIso(const Electron& electron, const uhh2::Event& event){

  static constexpr const auto thresholds_barrel = ele_cutvalues{
    .abs_dEtaIn                  = 0.00926,
    .abs_dPhiIn                  = 0.0336,
    .full5x5_sigmaIetaIeta       = 0.0101,
    .HoverE                      = 0.0597,
    .fabs_d0                     = 0.0111,
    .fabs_dz                     = 0.0466,
    .fabs_1oE_1op                = 0.012,
    .pfiso_dbeta_dr03            = std::numeric_limits<float>::infinity(),
    .passes_conversion_rejection = 1.    ,
    .cr_misshits                 = 2.
  };

  static constexpr const auto thresholds_endcap = ele_cutvalues{
    .abs_dEtaIn                  = 0.00724,
    .abs_dPhiIn                  = 0.0918,
    .full5x5_sigmaIetaIeta       = 0.0279,
    .HoverE                      = 0.0615,
    .fabs_d0                     = 0.0351,
    .fabs_dz                     = 0.417,
    .fabs_1oE_1op                = 0.00999,
    .pfiso_dbeta_dr03            = std::numeric_limits<float>::infinity(),
    .passes_conversion_rejection = 1.      ,
    .cr_misshits                 = 1.
  };

  return passes_id(electron, event, thresholds_barrel, thresholds_endcap);
}

bool ElectronID_Spring15_50ns_veto(const Electron& electron, const uhh2::Event& event){

  static constexpr const auto thresholds_barrel = ele_cutvalues{
    .abs_dEtaIn                  = 0.0126,
    .abs_dPhiIn                  = 0.107,
    .full5x5_sigmaIetaIeta       = 0.012,
    .HoverE                      = 0.186,
    .fabs_d0                     = 0.0621,
    .fabs_dz                     = 0.613,
    .fabs_1oE_1op                = 0.239,
    .pfiso_dbeta_dr03            = 0.161,
    .passes_conversion_rejection = 1.    ,
    .cr_misshits                 = 2.
  };

  static constexpr const auto thresholds_endcap = ele_cutvalues{
    .abs_dEtaIn                  = 0.0109,
    .abs_dPhiIn                  = 0.219,
    .full5x5_sigmaIetaIeta       = 0.0339,
    .HoverE                      = 0.0962,
    .fabs_d0                     = 0.279,
    .fabs_dz                     = 0.947,
    .fabs_1oE_1op                = 0.141,
    .pfiso_dbeta_dr03            = 0.193,
    .passes_conversion_rejection = 1.      ,
    .cr_misshits                 = 3.
  };

  return passes_id(electron, event, thresholds_barrel, thresholds_endcap);
}


bool ElectronID_Spring15_50ns_loose(const Electron& electron, const uhh2::Event& event){

  static constexpr const auto thresholds_barrel = ele_cutvalues{
    .abs_dEtaIn                  = 0.00976,
    .abs_dPhiIn                  = 0.0929,
    .full5x5_sigmaIetaIeta       = 0.0105,
    .HoverE                      = 0.0765,
    .fabs_d0                     = 0.0227,
    .fabs_dz                     = 0.379,
    .fabs_1oE_1op                = 0.184,
    .pfiso_dbeta_dr03            = 0.118,
    .passes_conversion_rejection = 1.    ,
    .cr_misshits                 = 2.
  };

  static constexpr const auto thresholds_endcap = ele_cutvalues{
    .abs_dEtaIn                  = 0.00952,
    .abs_dPhiIn                  = 0.181,
    .full5x5_sigmaIetaIeta       = 0.0318,
    .HoverE                      = 0.0824,
    .fabs_d0                     = 0.242,
    .fabs_dz                     = 0.921,
    .fabs_1oE_1op                = 0.125,
    .pfiso_dbeta_dr03            = 0.118,
    .passes_conversion_rejection = 1.      ,
    .cr_misshits                 = 1.
  };

  return passes_id(electron, event, thresholds_barrel, thresholds_endcap);
}

bool ElectronID_Spring15_50ns_medium(const Electron& electron, const uhh2::Event& event){

  static constexpr const auto thresholds_barrel = ele_cutvalues{
    .abs_dEtaIn                  = 0.0094,
    .abs_dPhiIn                  = 0.0296,
    .full5x5_sigmaIetaIeta       = 0.0101,
    .HoverE                      = 0.0372,
    .fabs_d0                     = 0.0151,
    .fabs_dz                     = 0.238,
    .fabs_1oE_1op                = 0.118,
    .pfiso_dbeta_dr03            = 0.0987,
    .passes_conversion_rejection = 1.    ,
    .cr_misshits                 = 2.
  };

  static constexpr const auto thresholds_endcap = ele_cutvalues{
    .abs_dEtaIn                  = 0.00773,
    .abs_dPhiIn                  = 0.148,
    .full5x5_sigmaIetaIeta       = 0.0287,
    .HoverE                      = 0.0546,
    .fabs_d0                     = 0.0535,
    .fabs_dz                     = 0.572,
    .fabs_1oE_1op                = 0.104,
    .pfiso_dbeta_dr03            = 0.0902,
    .passes_conversion_rejection = 1.      ,
    .cr_misshits                 = 1.
  };

  return passes_id(electron, event, thresholds_barrel, thresholds_endcap);
}

bool ElectronID_Spring15_50ns_tight(const Electron& electron, const uhh2::Event& event){

  static constexpr const auto thresholds_barrel = ele_cutvalues{
    .abs_dEtaIn                  = 0.0095,
    .abs_dPhiIn                  = 0.0291,
    .full5x5_sigmaIetaIeta       = 0.0101,
    .HoverE                      = 0.0372,
    .fabs_d0                     = 0.0144,
    .fabs_dz                     = 0.323,
    .fabs_1oE_1op                = 0.0174,
    .pfiso_dbeta_dr03            = 0.0468,
    .passes_conversion_rejection = 1.    ,
    .cr_misshits                 = 2.
  };

  static constexpr const auto thresholds_endcap = ele_cutvalues{
    .abs_dEtaIn                  = 0.00762,
    .abs_dPhiIn                  = 0.0439,
    .full5x5_sigmaIetaIeta       = 0.0287,
    .HoverE                      = 0.0544,
    .fabs_d0                     = 0.0377,
    .fabs_dz                     = 0.571,
    .fabs_1oE_1op                = 0.01,
    .pfiso_dbeta_dr03            = 0.0759,
    .passes_conversion_rejection = 1.      ,
    .cr_misshits                 = 1.
  };

  return passes_id(electron, event, thresholds_barrel, thresholds_endcap);
}

bool ElectronID_Spring15_50ns_veto_noIso(const Electron& electron, const uhh2::Event& event){

  static constexpr const auto thresholds_barrel = ele_cutvalues{
    .abs_dEtaIn                  = 0.0126,
    .abs_dPhiIn                  = 0.107,
    .full5x5_sigmaIetaIeta       = 0.012,
    .HoverE                      = 0.186,
    .fabs_d0                     = 0.0621,
    .fabs_dz                     = 0.613,
    .fabs_1oE_1op                = 0.239,
    .pfiso_dbeta_dr03            = std::numeric_limits<float>::infinity(),
    .passes_conversion_rejection = 1.    ,
    .cr_misshits                 = 2.
  };

  static constexpr const auto thresholds_endcap = ele_cutvalues{
    .abs_dEtaIn                  = 0.0109,
    .abs_dPhiIn                  = 0.219,
    .full5x5_sigmaIetaIeta       = 0.0339,
    .HoverE                      = 0.0962,
    .fabs_d0                     = 0.279,
    .fabs_dz                     = 0.947,
    .fabs_1oE_1op                = 0.141,
    .pfiso_dbeta_dr03            = std::numeric_limits<float>::infinity(),
    .passes_conversion_rejection = 1.      ,
    .cr_misshits                 = 3.
  };

  return passes_id(electron, event, thresholds_barrel, thresholds_endcap);
}


bool ElectronID_Spring15_50ns_loose_noIso(const Electron& electron, const uhh2::Event& event){

  static constexpr const auto thresholds_barrel = ele_cutvalues{
    .abs_dEtaIn                  = 0.00976,
    .abs_dPhiIn                  = 0.0929,
    .full5x5_sigmaIetaIeta       = 0.0105,
    .HoverE                      = 0.0765,
    .fabs_d0                     = 0.0227,
    .fabs_dz                     = 0.379,
    .fabs_1oE_1op                = 0.184,
    .pfiso_dbeta_dr03            = std::numeric_limits<float>::infinity(),
    .passes_conversion_rejection = 1.    ,
    .cr_misshits                 = 2.
  };

  static constexpr const auto thresholds_endcap = ele_cutvalues{
    .abs_dEtaIn                  = 0.00952,
    .abs_dPhiIn                  = 0.181,
    .full5x5_sigmaIetaIeta       = 0.0318,
    .HoverE                      = 0.0824,
    .fabs_d0                     = 0.242,
    .fabs_dz                     = 0.921,
    .fabs_1oE_1op                = 0.125,
    .pfiso_dbeta_dr03            = std::numeric_limits<float>::infinity(),
    .passes_conversion_rejection = 1.      ,
    .cr_misshits                 = 1.
  };

  return passes_id(electron, event, thresholds_barrel, thresholds_endcap);
}

bool ElectronID_Spring15_50ns_medium_noIso(const Electron& electron, const uhh2::Event& event){

  static constexpr const auto thresholds_barrel = ele_cutvalues{
    .abs_dEtaIn                  = 0.0094,
    .abs_dPhiIn                  = 0.0296,
    .full5x5_sigmaIetaIeta       = 0.0101,
    .HoverE                      = 0.0372,
    .fabs_d0                     = 0.0151,
    .fabs_dz                     = 0.238,
    .fabs_1oE_1op                = 0.118,
    .pfiso_dbeta_dr03            = std::numeric_limits<float>::infinity(),
    .passes_conversion_rejection = 1.    ,
    .cr_misshits                 = 2.
  };

  static constexpr const auto thresholds_endcap = ele_cutvalues{
    .abs_dEtaIn                  = 0.00773,
    .abs_dPhiIn                  = 0.148,
    .full5x5_sigmaIetaIeta       = 0.0287,
    .HoverE                      = 0.0546,
    .fabs_d0                     = 0.0535,
    .fabs_dz                     = 0.572,
    .fabs_1oE_1op                = 0.104,
    .pfiso_dbeta_dr03            = std::numeric_limits<float>::infinity(),
    .passes_conversion_rejection = 1.      ,
    .cr_misshits                 = 1.
  };

  return passes_id(electron, event, thresholds_barrel, thresholds_endcap);
}

bool ElectronID_Spring15_50ns_tight_noIso(const Electron& electron, const uhh2::Event& event){

  static constexpr const auto thresholds_barrel = ele_cutvalues{
    .abs_dEtaIn                  = 0.0095,
    .abs_dPhiIn                  = 0.0291,
    .full5x5_sigmaIetaIeta       = 0.0101,
    .HoverE                      = 0.0372,
    .fabs_d0                     = 0.0144,
    .fabs_dz                     = 0.323,
    .fabs_1oE_1op                = 0.0174,
    .pfiso_dbeta_dr03            = std::numeric_limits<float>::infinity(),
    .passes_conversion_rejection = 1.    ,
    .cr_misshits                 = 2.
  };

  static constexpr const auto thresholds_endcap = ele_cutvalues{
    .abs_dEtaIn                  = 0.00762,
    .abs_dPhiIn                  = 0.0439,
    .full5x5_sigmaIetaIeta       = 0.0287,
    .HoverE                      = 0.0544,
    .fabs_d0                     = 0.0377,
    .fabs_dz                     = 0.571,
    .fabs_1oE_1op                = 0.01,
    .pfiso_dbeta_dr03            = std::numeric_limits<float>::infinity(),
    .passes_conversion_rejection = 1.      ,
    .cr_misshits                 = 1.
  };

  return passes_id(electron, event, thresholds_barrel, thresholds_endcap);
}

////

bool ElectronID_MVAnotrig_PHYS14_loose(const Electron& electron, const uhh2::Event&){

  bool pass(false);

  const float MVA(electron.mvaNonTrigV0()), pt(electron.pt()), abs_etaSC(std::abs(electron.supercluster_eta()));

  if(5. < pt && pt <= 10.){

    if                         (abs_etaSC < 0.8)   pass = (MVA > -0.483);
    else if(0.8 <= abs_etaSC && abs_etaSC < 1.479) pass = (MVA > -0.267);
    else if                    (abs_etaSC < 2.5)   pass = (MVA > -0.323);
  }
  else if(pt > 10.){

    if                         (abs_etaSC < 0.8)   pass = (MVA >  0.933);
    else if(0.8 <= abs_etaSC && abs_etaSC < 1.479) pass = (MVA >  0.825);
    else if                    (abs_etaSC < 2.5)   pass = (MVA >  0.337);
  }

  return pass;
}

bool ElectronID_MVAnotrig_PHYS14_tight(const Electron& electron, const uhh2::Event&){

  bool pass(false);

  const float MVA(electron.mvaNonTrigV0()), pt(electron.pt()), abs_etaSC(std::abs(electron.supercluster_eta()));

  if(5. < pt && pt <= 10.){

    if                         (abs_etaSC < 0.8)   pass = (MVA > -0.253);
    else if(0.8 <= abs_etaSC && abs_etaSC < 1.479) pass = (MVA >  0.081);
    else if                    (abs_etaSC < 2.5)   pass = (MVA > -0.081);
  }
  else if(pt > 10.){

    if                         (abs_etaSC < 0.8)   pass = (MVA >  0.965);
    else if(0.8 <= abs_etaSC && abs_etaSC < 1.479) pass = (MVA >  0.917);
    else if                    (abs_etaSC < 2.5)   pass = (MVA >  0.683);
  }

  return pass;
}

bool ElectronID_MVAnotrig_Spring15_25ns_loose(const Electron& electron, const uhh2::Event&){

  bool pass(false);

  const float MVA(electron.mvaNonTrigV0()), pt(electron.pt()), abs_etaSC(std::abs(electron.supercluster_eta()));

  if(5. < pt && pt <= 10.){

    if                         (abs_etaSC < 0.8)   pass = (MVA > -0.083313);
    else if(0.8 <= abs_etaSC && abs_etaSC < 1.479) pass = (MVA > -0.235222);
    else if                    (abs_etaSC < 2.5)   pass = (MVA > -0.670990);
  }
  else if(pt > 10.){

    if                         (abs_etaSC < 0.8)   pass = (MVA >  0.913286);
    else if(0.8 <= abs_etaSC && abs_etaSC < 1.479) pass = (MVA >  0.805013);
    else if                    (abs_etaSC < 2.5)   pass = (MVA >  0.358969);
  }

  return pass;
}

bool ElectronID_MVAnotrig_Spring15_25ns_tight(const Electron& electron, const uhh2::Event&){

  bool pass(false);

  const float MVA(electron.mvaNonTrigV0()), pt(electron.pt()), abs_etaSC(std::abs(electron.supercluster_eta()));

  if(5. < pt && pt <= 10.){

    if                         (abs_etaSC < 0.8)   pass = (MVA >  0.287435);
    else if(0.8 <= abs_etaSC && abs_etaSC < 1.479) pass = (MVA >  0.221846);
    else if                    (abs_etaSC < 2.5)   pass = (MVA > -0.303263);
  }
  else if(pt > 10.){

    if                         (abs_etaSC < 0.8)   pass = (MVA >  0.967083);
    else if(0.8 <= abs_etaSC && abs_etaSC < 1.479) pass = (MVA >  0.929117);
    else if                    (abs_etaSC < 2.5)   pass = (MVA >  0.726311);
  }

  return pass;
}
///

bool Electron_MINIIso::operator()(const Electron& ele, const uhh2::Event&) const {

  float iso(-1.);

  if(!ele.pt()) throw std::runtime_error("Electron_MINIIso::operator() -- null electron transverse momentum: failed to calculate relative MINI-Isolation");

  if     (iso_key_ == "uncorrected") iso = (ele.pfMINIIso_CH() + ele.pfMINIIso_NH() + ele.pfMINIIso_Ph())/ele.pt();
  else if(iso_key_ == "delta-beta")  iso = (ele.pfMINIIso_CH() + std::max(0., ele.pfMINIIso_NH() + ele.pfMINIIso_Ph() - .5*ele.pfMINIIso_PU()))/ele.pt();
  else if(iso_key_ == "pf-weight")   iso = (ele.pfMINIIso_CH() + ele.pfMINIIso_NH_pfwgt() + ele.pfMINIIso_Ph_pfwgt())/ele.pt();
  else throw std::runtime_error("Electron_MINIIso::operator() -- invalid key for MINI-Isolation pileup correction: "+iso_key_);

  return (iso < iso_cut_);
}
