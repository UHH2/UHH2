#pragma once

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/common/include/ObjectIdUtils.h"
#include "UHH2/common/include/Utils.h"
#include "UHH2/JetMETObjects/interface/JetCorrectionUncertainty.h"
#include "TRandom.h"
#include "TFormula.h"

#include <iostream>
#include <fstream>

class FactorizedJetCorrector;

/// namespace to define some useful filename constants to be used for jet energy corrections
namespace JERFiles {
  //Summer16_23Sep2016_V4_noRes needed for L2Res people
  extern const std::vector<std::string> Summer16_23Sep2016_V4_BCD_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_23Sep2016_V4_EF_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_23Sep2016_V4_G_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_23Sep2016_V4_H_L123_noRes_AK4PFchs_DATA;

  //Summer16_23Sep2016_V4 --> Official JEC recommendation for Moriond17
  extern const std::vector<std::string> Summer16_23Sep2016_V4_BCD_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_23Sep2016_V4_EF_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_23Sep2016_V4_G_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_23Sep2016_V4_H_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_23Sep2016_V4_BCD_L123_AK8PFchs_DATA;
  extern const std::vector<std::string> Summer16_23Sep2016_V4_EF_L123_AK8PFchs_DATA;
  extern const std::vector<std::string> Summer16_23Sep2016_V4_G_L123_AK8PFchs_DATA;
  extern const std::vector<std::string> Summer16_23Sep2016_V4_H_L123_AK8PFchs_DATA;
  extern const std::vector<std::string> Summer16_23Sep2016_V4_L123_AK4PFchs_MC;
  extern const std::vector<std::string> Summer16_23Sep2016_V4_L123_AK8PFchs_MC;
  extern const std::vector<std::string> Summer16_23Sep2016_V4_BCD_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_23Sep2016_V4_EF_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_23Sep2016_V4_G_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_23Sep2016_V4_H_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_23Sep2016_V4_L1RC_AK4PFchs_MC;

  //Summer16_23Sep2016_V4 --> PUPPI Jet Corrections
  extern const std::vector<std::string> Summer16_23Sep2016_V4_BCD_L123_AK4PFPuppi_DATA;
  extern const std::vector<std::string> Summer16_23Sep2016_V4_EF_L123_AK4PFPuppi_DATA;
  extern const std::vector<std::string> Summer16_23Sep2016_V4_G_L123_AK4PFPuppi_DATA;
  extern const std::vector<std::string> Summer16_23Sep2016_V4_H_L123_AK4PFPuppi_DATA;
  extern const std::vector<std::string> Summer16_23Sep2016_V4_BCD_L123_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Summer16_23Sep2016_V4_EF_L123_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Summer16_23Sep2016_V4_G_L123_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Summer16_23Sep2016_V4_H_L123_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Summer16_23Sep2016_V4_L123_AK4PFPuppi_MC;
  extern const std::vector<std::string> Summer16_23Sep2016_V4_L123_AK8PFPuppi_MC;
}


/// namespace to define some useful filename constants to be used for jet energy corrections
namespace JERFiles {
  //Summer16_03Feb2017_V3_noRes needed for L2Res people
  extern const std::vector<std::string> Summer16_03Feb2017_V3_BCD_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V3_EF_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V3_G_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V3_H_L123_noRes_AK4PFchs_DATA;

  //Summer16_03Feb2017_V3 --> Official JEC recommendation for Moriond17
  extern const std::vector<std::string> Summer16_03Feb2017_V3_BCD_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V3_EF_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V3_G_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V3_H_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V3_BCD_L123_AK8PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V3_EF_L123_AK8PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V3_G_L123_AK8PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V3_H_L123_AK8PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V1_L123_AK4PFchs_MC;
  extern const std::vector<std::string> Summer16_03Feb2017_V1_L123_AK8PFchs_MC;
  extern const std::vector<std::string> Summer16_03Feb2017_V3_BCD_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V3_EF_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V3_G_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V3_H_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V1_L1RC_AK4PFchs_MC;
}

/// namespace to define some useful filename constants to be used for jet energy corrections
namespace JERFiles {
  //Summer16_03Feb2017_V4_noRes needed for L2Res people
  extern const std::vector<std::string> Summer16_03Feb2017_V4_BCD_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V4_EF_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V4_G_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V4_H_L123_noRes_AK4PFchs_DATA;

  //Summer16_03Feb2017_V4 --> Official JEC recommendation for Moriond17
  extern const std::vector<std::string> Summer16_03Feb2017_V4_BCD_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V4_EF_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V4_G_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V4_H_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V4_BCD_L123_AK8PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V4_EF_L123_AK8PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V4_G_L123_AK8PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V4_H_L123_AK8PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V4_BCD_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V4_EF_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V4_G_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V4_H_L1RC_AK4PFchs_DATA;
}

/// namespace to define some useful filename constants to be used for jet energy corrections
namespace JERFiles {
  //Summer16_03Feb2017_V5_noRes needed for L2Res people
  extern const std::vector<std::string> Summer16_03Feb2017_V5_BCD_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V5_EF_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V5_G_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V5_H_L123_noRes_AK4PFchs_DATA;

  //Summer16_03Feb2017_V5 --> Official JEC recommendation for Moriond17
  extern const std::vector<std::string> Summer16_03Feb2017_V5_BCD_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V5_EF_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V5_G_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V5_H_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V5_BCD_L123_AK8PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V5_EF_L123_AK8PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V5_G_L123_AK8PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V5_H_L123_AK8PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V5_BCD_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V5_EF_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V5_G_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V5_H_L1RC_AK4PFchs_DATA;
}

/// namespace to define some useful filename constants to be used for jet energy corrections
namespace JERFiles {
  //Summer16_03Feb2017_V6_noRes needed for L2Res people
  extern const std::vector<std::string> Summer16_03Feb2017_V6_BCD_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V6_EF_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V6_G_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V6_H_L123_noRes_AK4PFchs_DATA;

  //Summer16_03Feb2017_V6 --> Official JEC recommendation for Moriond17
  extern const std::vector<std::string> Summer16_03Feb2017_V6_BCD_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V6_EF_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V6_G_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V6_H_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V6_BCD_L123_AK8PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V6_EF_L123_AK8PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V6_G_L123_AK8PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V6_H_L123_AK8PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V6_L123_AK4PFchs_MC;
  extern const std::vector<std::string> Summer16_03Feb2017_V6_L123_AK8PFchs_MC;
  extern const std::vector<std::string> Summer16_03Feb2017_V6_BCD_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V6_EF_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V6_G_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V6_H_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Summer16_03Feb2017_V6_L1RC_AK4PFchs_MC;
}

//2017
namespace JERFiles{

  //Fall17_17Nov2017_V4
  extern const std::vector<std::string> Fall17_17Nov2017_V4_B_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V4_C_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V4_D_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V4_E_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V4_F_L123_AK4PFchs_DATA;

  extern const std::vector<std::string> Fall17_17Nov2017_V4_L123_AK4PFchs_MC;

  extern const std::vector<std::string> Fall17_17Nov2017_V4_B_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V4_C_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V4_D_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V4_E_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V4_F_L1RC_AK4PFchs_DATA;

  extern const std::vector<std::string> Fall17_17Nov2017_V4_L1RC_AK4PFchs_MC;

  //Fall17_17Nov2017_V5
  extern const std::vector<std::string> Fall17_17Nov2017_V5_B_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V5_C_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V5_D_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V5_E_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V5_F_L123_noRes_AK4PFchs_DATA;

  extern const std::vector<std::string> Fall17_17Nov2017_V5_B_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V5_C_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V5_D_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V5_E_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V5_F_L123_AK4PFchs_DATA;

  // extern const std::vector<std::string> Fall17_17Nov2017_V5_L123_AK4PFchs_MC;

  extern const std::vector<std::string> Fall17_17Nov2017_V5_B_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V5_C_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V5_D_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V5_E_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V5_F_L1RC_AK4PFchs_DATA;

  // extern const std::vector<std::string> Fall17_17Nov2017_V5_L1RC_AK4PFchs_MC;

  //Fall17_17Nov2017_V6
  extern const std::vector<std::string> Fall17_17Nov2017_V6_B_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_C_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_D_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_E_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_F_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_B_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_C_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_D_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_E_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_F_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_L123_AK4PFchs_MC;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_B_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_C_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_D_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_E_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_F_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_L1RC_AK4PFchs_MC;


  extern const std::vector<std::string> Fall17_17Nov2017_V6_B_L123_noRes_AK4PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_C_L123_noRes_AK4PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_D_L123_noRes_AK4PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_E_L123_noRes_AK4PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_F_L123_noRes_AK4PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_B_L123_AK4PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_C_L123_AK4PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_D_L123_AK4PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_E_L123_AK4PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_F_L123_AK4PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_L123_AK4PFPuppi_MC;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_B_L1RC_AK4PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_C_L1RC_AK4PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_D_L1RC_AK4PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_E_L1RC_AK4PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_F_L1RC_AK4PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_L1RC_AK4PFPuppi_MC;

  extern const std::vector<std::string> Fall17_17Nov2017_V6_B_L123_noRes_AK8PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_C_L123_noRes_AK8PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_D_L123_noRes_AK8PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_E_L123_noRes_AK8PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_F_L123_noRes_AK8PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_B_L123_AK8PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_C_L123_AK8PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_D_L123_AK8PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_E_L123_AK8PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_F_L123_AK8PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_L123_AK8PFchs_MC;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_B_L1RC_AK8PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_C_L1RC_AK8PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_D_L1RC_AK8PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_E_L1RC_AK8PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_F_L1RC_AK8PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_L1RC_AK8PFchs_MC;


  extern const std::vector<std::string> Fall17_17Nov2017_V6_B_L123_noRes_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_C_L123_noRes_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_D_L123_noRes_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_E_L123_noRes_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_F_L123_noRes_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_B_L123_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_C_L123_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_D_L123_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_E_L123_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_F_L123_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_L123_AK8PFPuppi_MC;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_B_L1RC_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_C_L1RC_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_D_L1RC_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_E_L1RC_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_F_L1RC_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V6_L1RC_AK8PFPuppi_MC;


  //Fall17_17Nov2017_V7
  extern const std::vector<std::string> Fall17_17Nov2017_V7_B_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V7_C_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V7_D_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V7_E_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V7_F_L123_noRes_AK4PFchs_DATA;

  extern const std::vector<std::string> Fall17_17Nov2017_V7_B_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V7_C_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V7_D_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V7_E_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V7_F_L123_AK4PFchs_DATA;

  // extern const std::vector<std::string> Fall17_17Nov2017_V7_L123_AK4PFchs_MC;

  extern const std::vector<std::string> Fall17_17Nov2017_V7_B_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V7_C_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V7_D_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V7_E_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V7_F_L1RC_AK4PFchs_DATA;

  //Fall17_17Nov2017_V10
  extern const std::vector<std::string> Fall17_17Nov2017_V10_B_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V10_C_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V10_D_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V10_E_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V10_F_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V10_B_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V10_C_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V10_D_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V10_E_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V10_F_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V10_B_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V10_C_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V10_D_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V10_E_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V10_F_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V10_L123_AK4PFchs_MC;
  extern const std::vector<std::string> Fall17_17Nov2017_V10_L1RC_AK4PFchs_MC;

  extern const std::vector<std::string> Fall17_17Nov2017_V10_B_L123_noRes_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V10_C_L123_noRes_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V10_D_L123_noRes_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V10_E_L123_noRes_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V10_F_L123_noRes_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V10_B_L123_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V10_C_L123_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V10_D_L123_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V10_E_L123_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V10_F_L123_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V10_B_L1RC_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V10_C_L1RC_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V10_D_L1RC_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V10_E_L1RC_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V10_F_L1RC_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V10_L123_AK8PFPuppi_MC;

  //Fall17_17Nov2017_V11
  extern const std::vector<std::string> Fall17_17Nov2017_V11_B_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V11_C_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V11_D_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V11_E_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V11_F_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V11_B_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V11_C_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V11_D_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V11_E_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V11_F_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V11_B_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V11_C_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V11_D_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V11_E_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V11_F_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V11_L123_AK4PFchs_MC;
  extern const std::vector<std::string> Fall17_17Nov2017_V11_L1RC_AK4PFchs_MC;

  extern const std::vector<std::string> Fall17_17Nov2017_V11_B_L123_noRes_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V11_C_L123_noRes_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V11_D_L123_noRes_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V11_E_L123_noRes_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V11_F_L123_noRes_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V11_B_L123_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V11_C_L123_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V11_D_L123_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V11_E_L123_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V11_F_L123_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V11_B_L1RC_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V11_C_L1RC_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V11_D_L1RC_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V11_E_L1RC_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V11_F_L1RC_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V11_L123_AK8PFPuppi_MC;

  //Fall17_17Nov2017_V12
  extern const std::vector<std::string> Fall17_17Nov2017_V12_B_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V12_C_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V12_D_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V12_E_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V12_F_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V12_B_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V12_C_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V12_D_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V12_E_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V12_F_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V12_B_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V12_C_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V12_D_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V12_E_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V12_F_L1RC_AK4PFchs_DATA;

  extern const std::vector<std::string> Fall17_17Nov2017_V12_B_L123_noRes_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V12_C_L123_noRes_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V12_D_L123_noRes_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V12_E_L123_noRes_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V12_F_L123_noRes_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V12_B_L123_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V12_C_L123_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V12_D_L123_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V12_E_L123_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V12_F_L123_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V12_B_L1RC_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V12_C_L1RC_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V12_D_L1RC_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V12_E_L1RC_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V12_F_L1RC_AK8PFPuppi_DATA;

  //Fall17_17Nov2017_V13
  extern const std::vector<std::string> Fall17_17Nov2017_V13_B_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V13_C_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V13_D_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V13_E_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V13_F_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V13_B_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V13_C_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V13_D_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V13_E_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V13_F_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V13_B_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V13_C_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V13_D_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V13_E_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V13_F_L1RC_AK4PFchs_DATA;

  extern const std::vector<std::string> Fall17_17Nov2017_V13_B_L123_noRes_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V13_C_L123_noRes_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V13_D_L123_noRes_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V13_E_L123_noRes_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V13_F_L123_noRes_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V13_B_L123_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V13_C_L123_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V13_D_L123_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V13_E_L123_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V13_F_L123_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V13_B_L1RC_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V13_C_L1RC_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V13_D_L1RC_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V13_E_L1RC_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V13_F_L1RC_AK8PFPuppi_DATA;

  //Fall17_17Nov2017_V32
  extern const std::vector<std::string> Fall17_17Nov2017_V32_B_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_C_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_DE_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_F_L123_noRes_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_B_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_C_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_DE_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_F_L123_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_L123_AK4PFchs_MC;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_B_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_C_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_DE_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_F_L1RC_AK4PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_L1RC_AK4PFchs_MC;


  extern const std::vector<std::string> Fall17_17Nov2017_V32_B_L123_noRes_AK4PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_C_L123_noRes_AK4PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_DE_L123_noRes_AK4PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_F_L123_noRes_AK4PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_B_L123_AK4PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_C_L123_AK4PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_DE_L123_AK4PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_F_L123_AK4PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_L123_AK4PFPuppi_MC;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_B_L1RC_AK4PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_C_L1RC_AK4PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_DE_L1RC_AK4PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_F_L1RC_AK4PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_L1RC_AK4PFPuppi_MC;

  extern const std::vector<std::string> Fall17_17Nov2017_V32_B_L123_noRes_AK8PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_C_L123_noRes_AK8PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_DE_L123_noRes_AK8PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_F_L123_noRes_AK8PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_B_L123_AK8PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_C_L123_AK8PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_DE_L123_AK8PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_F_L123_AK8PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_L123_AK8PFchs_MC;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_B_L1RC_AK8PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_C_L1RC_AK8PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_DE_L1RC_AK8PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_F_L1RC_AK8PFchs_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_L1RC_AK8PFchs_MC;


  extern const std::vector<std::string> Fall17_17Nov2017_V32_B_L123_noRes_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_C_L123_noRes_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_DE_L123_noRes_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_F_L123_noRes_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_B_L123_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_C_L123_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_DE_L123_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_F_L123_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_L123_AK8PFPuppi_MC;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_B_L1RC_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_C_L1RC_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_DE_L1RC_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_F_L1RC_AK8PFPuppi_DATA;
  extern const std::vector<std::string> Fall17_17Nov2017_V32_L1RC_AK8PFPuppi_MC;

}


void correct_jet(FactorizedJetCorrector & corrector, Jet & jet, const uhh2::Event & event, JetCorrectionUncertainty* jec_unc = NULL, int jec_unc_direction=0);

/** \brief (Re-)Correct jets according to the corrections in the passed txt files
*
* txt files are available in JetMETObjects/data/; see README there for instructions how to produce
* updated files.
*
* For some standard jet energy corrections, you can use filenames defined in the JERFiles namespace.
*
* Options parsed from the given Context:
*  - "jecsmear_direction": either "nominal", "up", or "down" to apply nominal, +1sigma, -1sigma smearing resp.
*
* Please note that the JetCorrector does not sort the (re-)corrected jets by pt;
* you might want to do that before running algorithms / plotting which assume that.
*/
class JetCorrector: public uhh2::AnalysisModule {
public:
  explicit JetCorrector(uhh2::Context & ctx, const std::vector<std::string> & filenames, const std::vector<std::string> & filenames_L1RC = {});

  virtual bool process(uhh2::Event & event) override;
  virtual bool correct_met(uhh2::Event & event, const bool & isCHSmet = false, double pt_thresh = 15., double eta_thresh_low=0., double eta_thresh_high=5.5);

  virtual ~JetCorrector();

protected:
  std::unique_ptr<FactorizedJetCorrector> corrector;
  std::unique_ptr<FactorizedJetCorrector> corrector_L1RC;

  JetCorrectionUncertainty* jec_uncertainty;
  int direction = 0; // -1 = down, +1 = up, 0 = nominal
  bool propagate_to_met = false;
  bool used_ak4puppi = false;
  bool used_ak4chs = false;
  bool metprop_possible_ak8chs = false;
  bool metprop_possible_ak8puppi = false;
  bool used_slimmedmet = false;
  bool used_puppimet = false;
  bool used_chsmet = false;
  bool do_metL1RC = false;
};

class TopJetCorrector: public uhh2::AnalysisModule {
public:
  explicit TopJetCorrector(uhh2::Context & ctx, const std::vector<std::string> & filenames);

  virtual bool process(uhh2::Event & event) override;

  virtual ~TopJetCorrector();

private:
  std::unique_ptr<FactorizedJetCorrector> corrector;

  JetCorrectionUncertainty* jec_uncertainty;
  int direction = 0; // -1 = down, +1 = up, 0 = nominal
};

class SubJetCorrector: public uhh2::AnalysisModule {
public:
  explicit SubJetCorrector(uhh2::Context & ctx, const std::vector<std::string> & filenames);

  virtual bool process(uhh2::Event & event) override;

  virtual ~SubJetCorrector();

private:
  std::unique_ptr<FactorizedJetCorrector> corrector;
  JetCorrectionUncertainty* jec_uncertainty;
  int direction = 0; // -1 = down, +1 = up, 0 = nominal
};

class GenericJetCorrector: public uhh2::AnalysisModule {
public:
  explicit GenericJetCorrector(uhh2::Context & ctx, const std::vector<std::string> & filenames, const std::string & collectionname);

  virtual bool process(uhh2::Event & event) override;

  virtual ~GenericJetCorrector();

private:
  std::unique_ptr<FactorizedJetCorrector> corrector;
  uhh2::Event::Handle<std::vector<Jet> > h_jets;
  JetCorrectionUncertainty* jec_uncertainty;
  int direction = 0; // -1 = down, +1 = up, 0 = nominal
};

class GenericTopJetCorrector: public uhh2::AnalysisModule {
public:
  explicit GenericTopJetCorrector(uhh2::Context & ctx, const std::vector<std::string> & filenames, const std::string & collectionname);

  virtual bool process(uhh2::Event & event) override;

  virtual ~GenericTopJetCorrector();

private:
  std::unique_ptr<FactorizedJetCorrector> corrector;
  uhh2::Event::Handle<std::vector<TopJet> > h_jets;
  JetCorrectionUncertainty* jec_uncertainty;
  int direction = 0; // -1 = down, +1 = up, 0 = nominal
};

class GenericSubJetCorrector: public uhh2::AnalysisModule {
public:
  explicit GenericSubJetCorrector(uhh2::Context & ctx, const std::vector<std::string> & filenames, const std::string & collectionname);

  virtual bool process(uhh2::Event & event) override;

  virtual ~GenericSubJetCorrector();

private:
  std::unique_ptr<FactorizedJetCorrector> corrector;
  uhh2::Event::Handle<std::vector<TopJet> > h_jets;
  JetCorrectionUncertainty* jec_uncertainty;
  int direction = 0; // -1 = down, +1 = up, 0 = nominal
};

/** \brief Cross-clean lepton and jets by subtracting lepton four momenta from nearby jets
*
* Leptons are subtracted from the jets' raw four-momentum if:
*  - DR(jet, lepton) < drmax (default: 0.4) and
*  - electron/muon multiplicity is greater than 0 and
*  - electron energy / muon energy of jet is compatible with lepton to subtract
*
* Only implemented for muons and electrons, not for taus. As default, all muons
* and electrons are used. To not consider all electrons/muons either:
*   - run an appropriate cleaning module before this one or
*   - set an explicit id via the set_electron_id / set_muon_id.
*
* Note that the cleaning works well if using a muon or electron id which is stricly a subset of the
* particle-flow id, because only particle-flow muons/electrons are considered in the muon/electron
* energy fraction stored in the jet which is used to decide whether or not to subtract it.
* So if you use non-PF muons or non-PF electrons, you might need to re-write the
* JetLeptonCleaner for that case.
*
* Please note that the JetLeptonCleaner does not sort the (re-)corrected jets by pt;
* you might want to do that before running algorithms / plotting which assume that.
*
*/
class JetLeptonCleaner: public uhh2::AnalysisModule {
public:
  // jec_filenames is teh same as for the JetCorrector.
  explicit JetLeptonCleaner(uhh2::Context & ctx, const std::vector<std::string> & jec_filenames);

  void set_muon_id(const MuonId & mu_id_){
    mu_id = mu_id_;
  }

  void set_electron_id(const ElectronId & ele_id_){
    ele_id = ele_id_;
  }

  void set_drmax(double drmax_){
    drmax = drmax_;
  }

  virtual bool process(uhh2::Event & event) override;

  virtual ~JetLeptonCleaner();

private:
  std::unique_ptr<FactorizedJetCorrector> corrector;
  MuonId mu_id;
  ElectronId ele_id;
  double drmax = 0.4;
  JetCorrectionUncertainty* jec_uncertainty;
  int direction = 0; // -1 = down, +1 = up, 0 = nominal
};



/** \brief JetLeptonCleaner using the matching of candidates' keys
*
* Can now run on TopJet Collection as well; the class will first check whether a Jet collection with
* the given label name exists, if not, it will look for a TopJet collection with this name.
*
* Default is 'jets' so it will run over the standard Ak4 jet collection.
*
* DISCLAIMER: In case of Ak8 jets, this JetLeptonCleaner only runs over the fat jet itself, not
* the subjets of the Ak8 jets! Whether this is necessary and has any effects on subjet-related
* quantities might need to be tested in the future.
*/
class JetLeptonCleaner_by_KEYmatching: public uhh2::AnalysisModule {

public:
  explicit JetLeptonCleaner_by_KEYmatching(uhh2::Context&, const std::vector<std::string>&, const std::string& jet_label="jets");
  virtual ~JetLeptonCleaner_by_KEYmatching();

  virtual bool process(uhh2::Event & event) override;
  bool do_cleaning(Jet & jet, uhh2::Event & event);

  void set_muon_id(const MuonId& muo_id_){ muo_id = muo_id_; }

  void set_electron_id(const ElectronId& ele_id_){ ele_id = ele_id_; }

private:
  uhh2::Event::Handle<std::vector<Jet> > h_jets_;
  uhh2::Event::Handle<std::vector<TopJet> > h_topjets_;
  std::unique_ptr<FactorizedJetCorrector> corrector;
  MuonId     muo_id;
  ElectronId ele_id;
  JetCorrectionUncertainty* jec_uncertainty;
  int direction = 0; // -1 = down, +1 = up, 0 = nominal
};


//// ----- modules for Jet Energy Resolution data/MC corrections -----

namespace JERSmearing {

  typedef std::vector<std::array<float, 4> > SFtype1;

  extern const SFtype1 SF_13TeV_2016;
  extern const SFtype1 SF_13TeV_2016_03Feb2017;
  extern const SFtype1 SF_13TeV_Summer16_25nsV1;

  extern const SFtype1 SF_13TeV_Fall17;
  extern const SFtype1 SF_13TeV_Fall17_V3;
}



////

/** \brief generalization of JetResolutionSmearer (see the latter for additional info)
*         to apply jet-energy-resolution smearing on non-default jet collections
*
*  options parsed from Context:
*   - "jersmear_direction": either "nominal", "up", or "down" to apply nominal, +1sigma, -1sigma smearing correction
*
*/
class GenericJetResolutionSmearer : public uhh2::AnalysisModule {

public:
  explicit GenericJetResolutionSmearer(uhh2::Context&, const std::string& recj="jets", const std::string& genj="genjets",
  const JERSmearing::SFtype1& JER_sf=JERSmearing::SF_13TeV_Fall17_V3, const TString ResolutionFileName="Fall17_V3_MC_PtResolution_AK4PFchs.txt");
  virtual ~GenericJetResolutionSmearer() {m_resfile.close();}

  virtual bool process(uhh2::Event&) override;

  template<typename RJ, typename GJ> void apply_JER_smearing(std::vector<RJ>&, const std::vector<GJ>&, float radius, float rho);

private:
  uhh2::Event::Handle<std::vector<Jet> >       h_recjets_;
  uhh2::Event::Handle<std::vector<Particle> >  h_genjets_;
  uhh2::Event::Handle<std::vector<TopJet> >    h_rectopjets_;
  uhh2::Event::Handle<std::vector<GenTopJet> > h_gentopjets_;

  int direction = 0; // -1 = down, +1 = up, 0 = nominal
  JERSmearing::SFtype1 JER_SFs_;
  TString m_ResolutionFileName;

  std::ifstream m_resfile;

  float getResolution(float eta, float rho, float pt);
  TFormula* res_formula;
};



/** \brief Smear the jet four-momenta in MC to match the resolution in data
*
* The corrections applied correspond to the values listed here:
* https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetResolution
* for 8TeV data (r36 from 2014-08-28) using the method to scale
* the genjet pt - reco pt difference.
*
* Run this *after* the jet energy corrections.
*
* IMPORTANT: do NOT run the module twice, as then, the jets will be smeared twice, which
* is too much.
*
* Options parsed from the given Context:
*  - "jersmear_direction": either "nominal", "up", or "down" to apply nominal, +1sigma, -1sigma smearing resp.
*
* Please note that the JetResolutionSmearer does not sort the (re-)corrected jets by pt;
* you might want to do that before running algorithms / plotting which assume that.
*/
class JetResolutionSmearer: public uhh2::AnalysisModule{
public:
  explicit JetResolutionSmearer(uhh2::Context & ctx, const JERSmearing::SFtype1& JER_sf=JERSmearing::SF_13TeV_Fall17_V3);

  virtual bool process(uhh2::Event & event) override;

  virtual ~JetResolutionSmearer();
private:

  GenericJetResolutionSmearer* m_gjrs;
};

//// -----------------------------------------------------------------
