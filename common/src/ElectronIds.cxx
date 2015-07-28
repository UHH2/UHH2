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

bool ElectronID_CSA14_50ns_loose(const Electron & ele, const uhh2::Event & event){
    static const auto thresholds_barrel = ele_cutvalues{0.016f, 0.080f, 0.012f, 0.15f, 0.019f, 0.036f, 0.11f, 0.18f, 1.0f, 1.0f};
    static const auto thresholds_endcap = ele_cutvalues{0.025f, 0.097f, 0.032f, 0.12f, 0.099f, 0.880f, 0.11f, 0.21f, 1.0f, 1.0f};
    return passes_id(ele, event, thresholds_barrel, thresholds_endcap);
}

bool ElectronID_CSA14_50ns_medium(const Electron & ele, const uhh2::Event & event){
    static const auto thresholds_barrel = ele_cutvalues{0.015f, 0.051f, 0.010f, 0.100f, 0.012f, 0.030f, 0.053f, 0.14f, 1.0f, 1.0f};
    static const auto thresholds_endcap = ele_cutvalues{0.023f, 0.056f, 0.030f, 0.099f, 0.068f, 0.780f, 0.110f, 0.15f, 1.0f, 1.0f};
    return passes_id(ele, event, thresholds_barrel, thresholds_endcap);
}

bool ElectronID_CSA14_50ns_tight(const Electron & ele, const uhh2::Event & event){
    static const auto thresholds_barrel = ele_cutvalues{0.012f, 0.024f, 0.010f, 0.074f, 0.0091f, 0.017f, 0.026f, 0.10f, 1.0f, 1.0f};
    static const auto thresholds_endcap = ele_cutvalues{0.019f, 0.043f, 0.029f, 0.080f, 0.0370f, 0.065f, 0.076f, 0.14f, 1.0f, 1.0f};
    return passes_id(ele, event, thresholds_barrel, thresholds_endcap);
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
