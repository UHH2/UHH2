#include "UHH2/common/include/JetCorrections.h"
#include "UHH2/common/include/Utils.h"
#include "UHH2/core/include/Utils.h"

#include "UHH2/JetMETObjects/interface/FactorizedJetCorrector.h"
#include "UHH2/JetMETObjects/interface/JetCorrectorParameters.h"

#include <string>

using namespace std;
using namespace uhh2;

//see https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookJetEnergyCorrections#GetTxtFiles how to get the txt files with jet energy corrections from the database


/* ++++++++++++++ Summer16_23Sep2016_V4_noRes needed for L2Res people +++++++++++++++ */
const std::vector<std::string> JERFiles::Summer16_23Sep2016_V4_BCD_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_23Sep2016BCDV4_DATA/Summer16_23Sep2016BCDV4_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016BCDV4_DATA/Summer16_23Sep2016BCDV4_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016BCDV4_DATA/Summer16_23Sep2016BCDV4_DATA_L3Absolute_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_23Sep2016_V4_EF_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_23Sep2016EFV4_DATA/Summer16_23Sep2016EFV4_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016EFV4_DATA/Summer16_23Sep2016EFV4_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016EFV4_DATA/Summer16_23Sep2016EFV4_DATA_L3Absolute_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_23Sep2016_V4_G_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_23Sep2016GV4_DATA/Summer16_23Sep2016GV4_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016GV4_DATA/Summer16_23Sep2016GV4_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016GV4_DATA/Summer16_23Sep2016GV4_DATA_L3Absolute_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_23Sep2016_V4_H_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_23Sep2016HV4_DATA/Summer16_23Sep2016HV4_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016HV4_DATA/Summer16_23Sep2016HV4_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016HV4_DATA/Summer16_23Sep2016HV4_DATA_L3Absolute_AK4PFchs.txt",
};


/***************** Summer16_23Sep2016_V4 --> Official JEC recommendation for Moriond17 **********************************/
const std::vector<std::string> JERFiles::Summer16_23Sep2016_V4_BCD_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_23Sep2016BCDV4_DATA/Summer16_23Sep2016BCDV4_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016BCDV4_DATA/Summer16_23Sep2016BCDV4_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016BCDV4_DATA/Summer16_23Sep2016BCDV4_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016BCDV4_DATA/Summer16_23Sep2016BCDV4_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_23Sep2016_V4_EF_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_23Sep2016EFV4_DATA/Summer16_23Sep2016EFV4_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016EFV4_DATA/Summer16_23Sep2016EFV4_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016EFV4_DATA/Summer16_23Sep2016EFV4_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016EFV4_DATA/Summer16_23Sep2016EFV4_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_23Sep2016_V4_G_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_23Sep2016GV4_DATA/Summer16_23Sep2016GV4_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016GV4_DATA/Summer16_23Sep2016GV4_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016GV4_DATA/Summer16_23Sep2016GV4_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016GV4_DATA/Summer16_23Sep2016GV4_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_23Sep2016_V4_H_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_23Sep2016HV4_DATA/Summer16_23Sep2016HV4_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016HV4_DATA/Summer16_23Sep2016HV4_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016HV4_DATA/Summer16_23Sep2016HV4_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016HV4_DATA/Summer16_23Sep2016HV4_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_23Sep2016_V4_L123_AK4PFchs_MC = {
  "JECDatabase/textFiles/Summer16_23Sep2016V4_MC/Summer16_23Sep2016V4_MC_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016V4_MC/Summer16_23Sep2016V4_MC_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016V4_MC/Summer16_23Sep2016V4_MC_L3Absolute_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_23Sep2016_V4_L123_AK8PFchs_MC = {
  "JECDatabase/textFiles/Summer16_23Sep2016V4_MC/Summer16_23Sep2016V4_MC_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016V4_MC/Summer16_23Sep2016V4_MC_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016V4_MC/Summer16_23Sep2016V4_MC_L3Absolute_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_23Sep2016_V4_BCD_L123_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_23Sep2016BCDV4_DATA/Summer16_23Sep2016BCDV4_DATA_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016BCDV4_DATA/Summer16_23Sep2016BCDV4_DATA_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016BCDV4_DATA/Summer16_23Sep2016BCDV4_DATA_L3Absolute_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016BCDV4_DATA/Summer16_23Sep2016BCDV4_DATA_L2L3Residual_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_23Sep2016_V4_EF_L123_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_23Sep2016EFV4_DATA/Summer16_23Sep2016EFV4_DATA_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016EFV4_DATA/Summer16_23Sep2016EFV4_DATA_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016EFV4_DATA/Summer16_23Sep2016EFV4_DATA_L3Absolute_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016EFV4_DATA/Summer16_23Sep2016EFV4_DATA_L2L3Residual_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_23Sep2016_V4_G_L123_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_23Sep2016GV4_DATA/Summer16_23Sep2016GV4_DATA_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016GV4_DATA/Summer16_23Sep2016GV4_DATA_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016GV4_DATA/Summer16_23Sep2016GV4_DATA_L3Absolute_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016GV4_DATA/Summer16_23Sep2016GV4_DATA_L2L3Residual_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_23Sep2016_V4_H_L123_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_23Sep2016HV4_DATA/Summer16_23Sep2016HV4_DATA_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016HV4_DATA/Summer16_23Sep2016HV4_DATA_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016HV4_DATA/Summer16_23Sep2016HV4_DATA_L3Absolute_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016HV4_DATA/Summer16_23Sep2016HV4_DATA_L2L3Residual_AK8PFchs.txt",
};

/***************** Summer16_23Sep2016_V4 --> PUPPI Jet Corrections  **********************************/
const std::vector<std::string> JERFiles::Summer16_23Sep2016_V4_BCD_L123_AK4PFPuppi_DATA = {
  "JECDatabase/textFiles/Summer16_23Sep2016BCDV4_DATA/Summer16_23Sep2016BCDV4_DATA_L1FastJet_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016BCDV4_DATA/Summer16_23Sep2016BCDV4_DATA_L2Relative_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016BCDV4_DATA/Summer16_23Sep2016BCDV4_DATA_L3Absolute_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016BCDV4_DATA/Summer16_23Sep2016BCDV4_DATA_L2L3Residual_AK4PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Summer16_23Sep2016_V4_EF_L123_AK4PFPuppi_DATA = {
  "JECDatabase/textFiles/Summer16_23Sep2016EFV4_DATA/Summer16_23Sep2016EFV4_DATA_L1FastJet_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016EFV4_DATA/Summer16_23Sep2016EFV4_DATA_L2Relative_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016EFV4_DATA/Summer16_23Sep2016EFV4_DATA_L3Absolute_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016EFV4_DATA/Summer16_23Sep2016EFV4_DATA_L2L3Residual_AK4PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Summer16_23Sep2016_V4_G_L123_AK4PFPuppi_DATA = {
  "JECDatabase/textFiles/Summer16_23Sep2016GV4_DATA/Summer16_23Sep2016GV4_DATA_L1FastJet_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016GV4_DATA/Summer16_23Sep2016GV4_DATA_L2Relative_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016GV4_DATA/Summer16_23Sep2016GV4_DATA_L3Absolute_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016GV4_DATA/Summer16_23Sep2016GV4_DATA_L2L3Residual_AK4PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Summer16_23Sep2016_V4_H_L123_AK4PFPuppi_DATA = {
  "JECDatabase/textFiles/Summer16_23Sep2016HV4_DATA/Summer16_23Sep2016HV4_DATA_L1FastJet_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016HV4_DATA/Summer16_23Sep2016HV4_DATA_L2Relative_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016HV4_DATA/Summer16_23Sep2016HV4_DATA_L3Absolute_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016HV4_DATA/Summer16_23Sep2016HV4_DATA_L2L3Residual_AK4PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Summer16_23Sep2016_V4_L123_AK4PFPuppi_MC = {
  "JECDatabase/textFiles/Summer16_23Sep2016V4_MC/Summer16_23Sep2016V4_MC_L1FastJet_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016V4_MC/Summer16_23Sep2016V4_MC_L2Relative_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016V4_MC/Summer16_23Sep2016V4_MC_L3Absolute_AK4PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Summer16_23Sep2016_V4_L123_AK8PFPuppi_MC = {
  "JECDatabase/textFiles/Summer16_23Sep2016V4_MC/Summer16_23Sep2016V4_MC_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016V4_MC/Summer16_23Sep2016V4_MC_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016V4_MC/Summer16_23Sep2016V4_MC_L3Absolute_AK8PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Summer16_23Sep2016_V4_BCD_L123_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Summer16_23Sep2016BCDV4_DATA/Summer16_23Sep2016BCDV4_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016BCDV4_DATA/Summer16_23Sep2016BCDV4_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016BCDV4_DATA/Summer16_23Sep2016BCDV4_DATA_L3Absolute_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016BCDV4_DATA/Summer16_23Sep2016BCDV4_DATA_L2L3Residual_AK8PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Summer16_23Sep2016_V4_EF_L123_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Summer16_23Sep2016EFV4_DATA/Summer16_23Sep2016EFV4_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016EFV4_DATA/Summer16_23Sep2016EFV4_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016EFV4_DATA/Summer16_23Sep2016EFV4_DATA_L3Absolute_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016EFV4_DATA/Summer16_23Sep2016EFV4_DATA_L2L3Residual_AK8PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Summer16_23Sep2016_V4_G_L123_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Summer16_23Sep2016GV4_DATA/Summer16_23Sep2016GV4_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016GV4_DATA/Summer16_23Sep2016GV4_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016GV4_DATA/Summer16_23Sep2016GV4_DATA_L3Absolute_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016GV4_DATA/Summer16_23Sep2016GV4_DATA_L2L3Residual_AK8PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Summer16_23Sep2016_V4_H_L123_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Summer16_23Sep2016HV4_DATA/Summer16_23Sep2016HV4_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016HV4_DATA/Summer16_23Sep2016HV4_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016HV4_DATA/Summer16_23Sep2016HV4_DATA_L3Absolute_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Summer16_23Sep2016HV4_DATA/Summer16_23Sep2016HV4_DATA_L2L3Residual_AK8PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Summer16_23Sep2016_V4_BCD_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_23Sep2016BCDV4_DATA/Summer16_23Sep2016BCDV4_DATA_L1RC_AK4PFchs.txt"
};

const std::vector<std::string> JERFiles::Summer16_23Sep2016_V4_EF_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_23Sep2016EFV4_DATA/Summer16_23Sep2016EFV4_DATA_L1RC_AK4PFchs.txt"
};

const std::vector<std::string> JERFiles::Summer16_23Sep2016_V4_G_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_23Sep2016GV4_DATA/Summer16_23Sep2016GV4_DATA_L1RC_AK4PFchs.txt"
};

const std::vector<std::string> JERFiles::Summer16_23Sep2016_V4_H_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_23Sep2016HV4_DATA/Summer16_23Sep2016HV4_DATA_L1RC_AK4PFchs.txt"
};

const std::vector<std::string> JERFiles::Summer16_23Sep2016_V4_L1RC_AK4PFchs_MC = {
  "JECDatabase/textFiles/Summer16_23Sep2016V4_MC/Summer16_23Sep2016V4_MC_L1RC_AK4PFchs.txt"
};




/* ++++++++++++++ Summer16_03Feb2017_V3_noRes needed for L2Res people +++++++++++++++ */
const std::vector<std::string> JERFiles::Summer16_03Feb2017_V3_BCD_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017BCDV3_DATA/Summer16_03Feb2017BCDV3_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017BCDV3_DATA/Summer16_03Feb2017BCDV3_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017BCDV3_DATA/Summer16_03Feb2017BCDV3_DATA_L3Absolute_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V3_EF_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017EFV3_DATA/Summer16_03Feb2017EFV3_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017EFV3_DATA/Summer16_03Feb2017EFV3_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017EFV3_DATA/Summer16_03Feb2017EFV3_DATA_L3Absolute_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V3_G_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017GV3_DATA/Summer16_03Feb2017GV3_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017GV3_DATA/Summer16_03Feb2017GV3_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017GV3_DATA/Summer16_03Feb2017GV3_DATA_L3Absolute_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V3_H_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017HV3_DATA/Summer16_03Feb2017HV3_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017HV3_DATA/Summer16_03Feb2017HV3_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017HV3_DATA/Summer16_03Feb2017HV3_DATA_L3Absolute_AK4PFchs.txt",
};


/***************** Summer16_03Feb2017_V3 **********************************/
const std::vector<std::string> JERFiles::Summer16_03Feb2017_V3_BCD_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017BCDV3_DATA/Summer16_03Feb2017BCDV3_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017BCDV3_DATA/Summer16_03Feb2017BCDV3_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017BCDV3_DATA/Summer16_03Feb2017BCDV3_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017BCDV3_DATA/Summer16_03Feb2017BCDV3_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V3_EF_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017EFV3_DATA/Summer16_03Feb2017EFV3_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017EFV3_DATA/Summer16_03Feb2017EFV3_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017EFV3_DATA/Summer16_03Feb2017EFV3_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017EFV3_DATA/Summer16_03Feb2017EFV3_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V3_G_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017GV3_DATA/Summer16_03Feb2017GV3_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017GV3_DATA/Summer16_03Feb2017GV3_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017GV3_DATA/Summer16_03Feb2017GV3_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017GV3_DATA/Summer16_03Feb2017GV3_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V3_H_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017HV3_DATA/Summer16_03Feb2017HV3_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017HV3_DATA/Summer16_03Feb2017HV3_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017HV3_DATA/Summer16_03Feb2017HV3_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017HV3_DATA/Summer16_03Feb2017HV3_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V1_L123_AK4PFchs_MC = {
  "JECDatabase/textFiles/Summer16_03Feb2017_V1_MC/Summer16_03Feb2017_V1_MC_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017_V1_MC/Summer16_03Feb2017_V1_MC_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017_V1_MC/Summer16_03Feb2017_V1_MC_L3Absolute_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V1_L123_AK8PFchs_MC = {
  "JECDatabase/textFiles/Summer16_03Feb2017_V1_MC/Summer16_03Feb2017_V1_MC_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017_V1_MC/Summer16_03Feb2017_V1_MC_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017_V1_MC/Summer16_03Feb2017_V1_MC_L3Absolute_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V7_L123_AK4PFpuppi_MC = {
  "JECDatabase/textFiles/Summer16_03Feb2017_V7_MC/Summer16_03Feb2017_V7_MC_L1FastJet_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017_V7_MC/Summer16_03Feb2017_V7_MC_L2Relative_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017_V7_MC/Summer16_03Feb2017_V7_MC_L3Absolute_AK4PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V3_BCD_L123_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017BCDV3_DATA/Summer16_03Feb2017BCDV3_DATA_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017BCDV3_DATA/Summer16_03Feb2017BCDV3_DATA_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017BCDV3_DATA/Summer16_03Feb2017BCDV3_DATA_L3Absolute_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017BCDV3_DATA/Summer16_03Feb2017BCDV3_DATA_L2L3Residual_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V3_EF_L123_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017EFV3_DATA/Summer16_03Feb2017EFV3_DATA_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017EFV3_DATA/Summer16_03Feb2017EFV3_DATA_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017EFV3_DATA/Summer16_03Feb2017EFV3_DATA_L3Absolute_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017EFV3_DATA/Summer16_03Feb2017EFV3_DATA_L2L3Residual_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V3_G_L123_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017GV3_DATA/Summer16_03Feb2017GV3_DATA_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017GV3_DATA/Summer16_03Feb2017GV3_DATA_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017GV3_DATA/Summer16_03Feb2017GV3_DATA_L3Absolute_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017GV3_DATA/Summer16_03Feb2017GV3_DATA_L2L3Residual_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V3_H_L123_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017HV3_DATA/Summer16_03Feb2017HV3_DATA_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017HV3_DATA/Summer16_03Feb2017HV3_DATA_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017HV3_DATA/Summer16_03Feb2017HV3_DATA_L3Absolute_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017HV3_DATA/Summer16_03Feb2017HV3_DATA_L2L3Residual_AK8PFchs.txt",
};


const std::vector<std::string> JERFiles::Summer16_03Feb2017_V3_BCD_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017BCDV3_DATA/Summer16_03Feb2017BCDV3_DATA_L1RC_AK4PFchs.txt"
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V3_EF_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017EFV3_DATA/Summer16_03Feb2017EFV3_DATA_L1RC_AK4PFchs.txt"
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V3_G_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017GV3_DATA/Summer16_03Feb2017GV3_DATA_L1RC_AK4PFchs.txt"
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V3_H_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017HV3_DATA/Summer16_03Feb2017HV3_DATA_L1RC_AK4PFchs.txt"
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V1_L1RC_AK4PFchs_MC = {
  "JECDatabase/textFiles/Summer16_03Feb2017_V1_MC/Summer16_03Feb2017_V1_MC_L1RC_AK4PFchs.txt"
};

/* ++++++++++++++ Summer16_03Feb2017_V4_noRes needed for L2Res people +++++++++++++++ */
const std::vector<std::string> JERFiles::Summer16_03Feb2017_V4_BCD_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V4_DATA/Summer16_03Feb2017BCD_V4_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V4_DATA/Summer16_03Feb2017BCD_V4_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V4_DATA/Summer16_03Feb2017BCD_V4_DATA_L3Absolute_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V4_EF_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017EF_V4_DATA/Summer16_03Feb2017EF_V4_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017EF_V4_DATA/Summer16_03Feb2017EF_V4_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017EF_V4_DATA/Summer16_03Feb2017EF_V4_DATA_L3Absolute_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V4_G_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017G_V4_DATA/Summer16_03Feb2017G_V4_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017G_V4_DATA/Summer16_03Feb2017G_V4_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017G_V4_DATA/Summer16_03Feb2017G_V4_DATA_L3Absolute_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V4_H_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017H_V4_DATA/Summer16_03Feb2017H_V4_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017H_V4_DATA/Summer16_03Feb2017H_V4_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017H_V4_DATA/Summer16_03Feb2017H_V4_DATA_L3Absolute_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V4_BCD_L123_noRes_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V4_DATA/Summer16_03Feb2017BCD_V4_DATA_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V4_DATA/Summer16_03Feb2017BCD_V4_DATA_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V4_DATA/Summer16_03Feb2017BCD_V4_DATA_L3Absolute_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V4_EF_L123_noRes_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017EF_V4_DATA/Summer16_03Feb2017EF_V4_DATA_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017EF_V4_DATA/Summer16_03Feb2017EF_V4_DATA_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017EF_V4_DATA/Summer16_03Feb2017EF_V4_DATA_L3Absolute_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V4_G_L123_noRes_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017G_V4_DATA/Summer16_03Feb2017G_V4_DATA_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017G_V4_DATA/Summer16_03Feb2017G_V4_DATA_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017G_V4_DATA/Summer16_03Feb2017G_V4_DATA_L3Absolute_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V4_H_L123_noRes_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017H_V4_DATA/Summer16_03Feb2017H_V4_DATA_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017H_V4_DATA/Summer16_03Feb2017H_V4_DATA_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017H_V4_DATA/Summer16_03Feb2017H_V4_DATA_L3Absolute_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V4_BCD_L123_noRes_AK4PFpuppi_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V4_DATA/Summer16_03Feb2017BCD_V4_DATA_L1FastJet_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V4_DATA/Summer16_03Feb2017BCD_V4_DATA_L2Relative_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V4_DATA/Summer16_03Feb2017BCD_V4_DATA_L3Absolute_AK4PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V4_EF_L123_noRes_AK4PFpuppi_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017EF_V4_DATA/Summer16_03Feb2017EF_V4_DATA_L1FastJet_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017EF_V4_DATA/Summer16_03Feb2017EF_V4_DATA_L2Relative_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017EF_V4_DATA/Summer16_03Feb2017EF_V4_DATA_L3Absolute_AK4PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V4_G_L123_noRes_AK4PFpuppi_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017G_V4_DATA/Summer16_03Feb2017G_V4_DATA_L1FastJet_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017G_V4_DATA/Summer16_03Feb2017G_V4_DATA_L2Relative_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017G_V4_DATA/Summer16_03Feb2017G_V4_DATA_L3Absolute_AK4PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V4_H_L123_noRes_AK4PFpuppi_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017H_V4_DATA/Summer16_03Feb2017H_V4_DATA_L1FastJet_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017H_V4_DATA/Summer16_03Feb2017H_V4_DATA_L2Relative_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017H_V4_DATA/Summer16_03Feb2017H_V4_DATA_L3Absolute_AK4PFPuppi.txt",
};


/***************** Summer16_03Feb2017_V4 **********************************/
const std::vector<std::string> JERFiles::Summer16_03Feb2017_V4_BCD_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V4_DATA/Summer16_03Feb2017BCD_V4_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V4_DATA/Summer16_03Feb2017BCD_V4_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V4_DATA/Summer16_03Feb2017BCD_V4_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V4_DATA/Summer16_03Feb2017BCD_V4_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V4_EF_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017EFV4_DATA/Summer16_03Feb2017EF_V4_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017EFV4_DATA/Summer16_03Feb2017EF_V4_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017EFV4_DATA/Summer16_03Feb2017EF_V4_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017EFV4_DATA/Summer16_03Feb2017EF_V4_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V4_G_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017G_V4_DATA/Summer16_03Feb2017G_V4_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017G_V4_DATA/Summer16_03Feb2017G_V4_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017G_V4_DATA/Summer16_03Feb2017G_V4_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017G_V4_DATA/Summer16_03Feb2017G_V4_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V4_H_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017HV4_DATA/Summer16_03Feb2017H_V4_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017HV4_DATA/Summer16_03Feb2017H_V4_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017HV4_DATA/Summer16_03Feb2017H_V4_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017HV4_DATA/Summer16_03Feb2017H_V4_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V4_BCD_L123_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V4_DATA/Summer16_03Feb2017BCDV4_DATA_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V4_DATA/Summer16_03Feb2017BCDV4_DATA_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V4_DATA/Summer16_03Feb2017BCDV4_DATA_L3Absolute_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V4_DATA/Summer16_03Feb2017BCDV4_DATA_L2L3Residual_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V4_EF_L123_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017EF_V4_DATA/Summer16_03Feb2017EF_V4_DATA_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017EF_V4_DATA/Summer16_03Feb2017EF_V4_DATA_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017EF_V4_DATA/Summer16_03Feb2017EF_V4_DATA_L3Absolute_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017EF_V4_DATA/Summer16_03Feb2017EF_V4_DATA_L2L3Residual_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V4_G_L123_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017G_V4_DATA/Summer16_03Feb2017G_V4_DATA_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017G_V4_DATA/Summer16_03Feb2017G_V4_DATA_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017G_V4_DATA/Summer16_03Feb2017G_V4_DATA_L3Absolute_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017G_V4_DATA/Summer16_03Feb2017G_V4_DATA_L2L3Residual_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V4_H_L123_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017H_V4_DATA/Summer16_03Feb2017HV4_DATA_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017H_V4_DATA/Summer16_03Feb2017HV4_DATA_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017H_V4_DATA/Summer16_03Feb2017HV4_DATA_L3Absolute_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017H_V4_DATA/Summer16_03Feb2017HV4_DATA_L2L3Residual_AK8PFchs.txt",
};


const std::vector<std::string> JERFiles::Summer16_03Feb2017_V4_BCD_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V4_DATA/Summer16_03Feb2017BCD_V4_DATA_L1RC_AK4PFchs.txt"
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V4_EF_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017EF_V4_DATA/Summer16_03Feb2017EF_V4_DATA_L1RC_AK4PFchs.txt"
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V4_G_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017G_V4_DATA/Summer16_03Feb2017G_V4_DATA_L1RC_AK4PFchs.txt"
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V4_H_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017H_V4_DATA/Summer16_03Feb2017H_V4_DATA_L1RC_AK4PFchs.txt"
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V4_BCD_L1RC_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V4_DATA/Summer16_03Feb2017BCD_V4_DATA_L1RC_AK8PFchs.txt"
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V4_EF_L1RC_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017EF_V4_DATA/Summer16_03Feb2017EF_V4_DATA_L1RC_AK8PFchs.txt"
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V4_G_L1RC_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017G_V4_DATA/Summer16_03Feb2017G_V4_DATA_L1RC_AK8PFchs.txt"
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V4_H_L1RC_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017H_V4_DATA/Summer16_03Feb2017H_V4_DATA_L1RC_AK8PFchs.txt"
};


const std::vector<std::string> JERFiles::Summer16_03Feb2017_V4_BCD_L123_AK4PFpuppi_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V4_DATA/Summer16_03Feb2017BCD_V4_DATA_L1FastJet_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V4_DATA/Summer16_03Feb2017BCD_V4_DATA_L2Relative_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V4_DATA/Summer16_03Feb2017BCD_V4_DATA_L3Absolute_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V4_DATA/Summer16_03Feb2017BCD_V4_DATA_L2L3Residual_AK4PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V4_EF_L123_AK4PFpuppi_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017EF_V4_DATA/Summer16_03Feb2017EF_V4_DATA_L1FastJet_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017EF_V4_DATA/Summer16_03Feb2017EF_V4_DATA_L2Relative_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017EF_V4_DATA/Summer16_03Feb2017EF_V4_DATA_L3Absolute_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017EF_V4_DATA/Summer16_03Feb2017EF_V4_DATA_L2L3Residual_AK4PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V4_G_L123_AK4PFpuppi_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017G_V4_DATA/Summer16_03Feb2017G_V4_DATA_L1FastJet_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017G_V4_DATA/Summer16_03Feb2017G_V4_DATA_L2Relative_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017G_V4_DATA/Summer16_03Feb2017G_V4_DATA_L3Absolute_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017G_V4_DATA/Summer16_03Feb2017G_V4_DATA_L2L3Residual_AK4PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V4_H_L123_AK4PFpuppi_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017H_V4_DATA/Summer16_03Feb2017H_V4_DATA_L1FastJet_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017H_V4_DATA/Summer16_03Feb2017H_V4_DATA_L2Relative_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017H_V4_DATA/Summer16_03Feb2017H_V4_DATA_L3Absolute_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017H_V4_DATA/Summer16_03Feb2017H_V4_DATA_L2L3Residual_AK4PFPuppi.txt",
};


/* ++++++++++++++ Summer16_03Feb2017_V5_noRes needed for L2Res people +++++++++++++++ */
const std::vector<std::string> JERFiles::Summer16_03Feb2017_V5_BCD_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V5_DATA/Summer16_03Feb2017BCD_V5_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V5_DATA/Summer16_03Feb2017BCD_V5_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V5_DATA/Summer16_03Feb2017BCD_V5_DATA_L3Absolute_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V5_EF_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017EF_V5_DATA/Summer16_03Feb2017EF_V5_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017EF_V5_DATA/Summer16_03Feb2017EF_V5_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017EF_V5_DATA/Summer16_03Feb2017EF_V5_DATA_L3Absolute_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V5_G_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017G_V5_DATA/Summer16_03Feb2017G_V5_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017G_V5_DATA/Summer16_03Feb2017G_V5_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017G_V5_DATA/Summer16_03Feb2017G_V5_DATA_L3Absolute_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V5_H_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017H_V5_DATA/Summer16_03Feb2017H_V5_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017H_V5_DATA/Summer16_03Feb2017H_V5_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017H_V5_DATA/Summer16_03Feb2017H_V5_DATA_L3Absolute_AK4PFchs.txt",
};


/***************** Summer16_03Feb2017_V5 --> Official JEC recommendation for Moriond17 **********************************/
const std::vector<std::string> JERFiles::Summer16_03Feb2017_V5_BCD_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V5_DATA/Summer16_03Feb2017BCD_V5_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V5_DATA/Summer16_03Feb2017BCD_V5_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V5_DATA/Summer16_03Feb2017BCD_V5_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V5_DATA/Summer16_03Feb2017BCD_V5_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V5_EF_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017EF_V5_DATA/Summer16_03Feb2017EF_V5_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017EF_V5_DATA/Summer16_03Feb2017EF_V5_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017EF_V5_DATA/Summer16_03Feb2017EF_V5_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017EF_V5_DATA/Summer16_03Feb2017EF_V5_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V5_G_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017G_V5_DATA/Summer16_03Feb2017G_V5_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017G_V5_DATA/Summer16_03Feb2017G_V5_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017G_V5_DATA/Summer16_03Feb2017G_V5_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017G_V5_DATA/Summer16_03Feb2017G_V5_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V5_H_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017H_V5_DATA/Summer16_03Feb2017H_V5_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017H_V5_DATA/Summer16_03Feb2017H_V5_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017H_V5_DATA/Summer16_03Feb2017H_V5_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017H_V5_DATA/Summer16_03Feb2017H_V5_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V5_BCD_L123_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V5_DATA/Summer16_03Feb2017BCD_V5_DATA_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V5_DATA/Summer16_03Feb2017BCD_V5_DATA_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V5_DATA/Summer16_03Feb2017BCD_V5_DATA_L3Absolute_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V5_DATA/Summer16_03Feb2017BCD_V5_DATA_L2L3Residual_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V5_EF_L123_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017EFV5_DATA/Summer16_03Feb2017EF_V5_DATA_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017EFV5_DATA/Summer16_03Feb2017EF_V5_DATA_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017EFV5_DATA/Summer16_03Feb2017EF_V5_DATA_L3Absolute_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017EFV5_DATA/Summer16_03Feb2017EF_V5_DATA_L2L3Residual_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V5_G_L123_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017G_V5_DATA/Summer16_03Feb2017G_V5_DATA_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017G_V5_DATA/Summer16_03Feb2017G_V5_DATA_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017G_V5_DATA/Summer16_03Feb2017G_V5_DATA_L3Absolute_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017G_V5_DATA/Summer16_03Feb2017G_V5_DATA_L2L3Residual_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V5_H_L123_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017H_V5_DATA/Summer16_03Feb2017H_V5_DATA_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017H_V5_DATA/Summer16_03Feb2017H_V5_DATA_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017H_V5_DATA/Summer16_03Feb2017H_V5_DATA_L3Absolute_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017H_V5_DATA/Summer16_03Feb2017H_V5_DATA_L2L3Residual_AK8PFchs.txt",
};


const std::vector<std::string> JERFiles::Summer16_03Feb2017_V5_BCD_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V5_DATA/Summer16_03Feb2017BCD_V5_DATA_L1RC_AK4PFchs.txt"
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V5_EF_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017EF_V5_DATA/Summer16_03Feb2017EF_V5_DATA_L1RC_AK4PFchs.txt"
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V5_G_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017G_V5_DATA/Summer16_03Feb2017G_V5_DATA_L1RC_AK4PFchs.txt"
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V5_H_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017H_V5_DATA/Summer16_03Feb2017H_V5_DATA_L1RC_AK4PFchs.txt"
};



/* ++++++++++++++ Summer16_03Feb2017_V6_noRes needed for L2Res people +++++++++++++++ */
const std::vector<std::string> JERFiles::Summer16_03Feb2017_V6_BCD_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V6_DATA/Summer16_03Feb2017BCD_V6_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V6_DATA/Summer16_03Feb2017BCD_V6_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V6_DATA/Summer16_03Feb2017BCD_V6_DATA_L3Absolute_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V6_EF_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017EF_V6_DATA/Summer16_03Feb2017EF_V6_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017EF_V6_DATA/Summer16_03Feb2017EF_V6_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017EF_V6_DATA/Summer16_03Feb2017EF_V6_DATA_L3Absolute_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V6_G_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017G_V6_DATA/Summer16_03Feb2017G_V6_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017G_V6_DATA/Summer16_03Feb2017G_V6_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017G_V6_DATA/Summer16_03Feb2017G_V6_DATA_L3Absolute_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V6_H_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017H_V6_DATA/Summer16_03Feb2017H_V6_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017H_V6_DATA/Summer16_03Feb2017H_V6_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017H_V6_DATA/Summer16_03Feb2017H_V6_DATA_L3Absolute_AK4PFchs.txt",
};


/***************** Summer16_03Feb2017_V6 --> Official JEC recommendation for Moriond17 **********************************/
const std::vector<std::string> JERFiles::Summer16_03Feb2017_V6_BCD_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V6_DATA/Summer16_03Feb2017BCD_V6_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V6_DATA/Summer16_03Feb2017BCD_V6_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V6_DATA/Summer16_03Feb2017BCD_V6_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V6_DATA/Summer16_03Feb2017BCD_V6_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V6_EF_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017EF_V6_DATA/Summer16_03Feb2017EF_V6_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017EF_V6_DATA/Summer16_03Feb2017EF_V6_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017EF_V6_DATA/Summer16_03Feb2017EF_V6_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017EF_V6_DATA/Summer16_03Feb2017EF_V6_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V6_G_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017G_V6_DATA/Summer16_03Feb2017G_V6_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017G_V6_DATA/Summer16_03Feb2017G_V6_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017G_V6_DATA/Summer16_03Feb2017G_V6_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017G_V6_DATA/Summer16_03Feb2017G_V6_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V6_H_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017H_V6_DATA/Summer16_03Feb2017H_V6_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017H_V6_DATA/Summer16_03Feb2017H_V6_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017H_V6_DATA/Summer16_03Feb2017H_V6_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017H_V6_DATA/Summer16_03Feb2017H_V6_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V6_BCD_L123_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V6_DATA/Summer16_03Feb2017BCD_V6_DATA_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V6_DATA/Summer16_03Feb2017BCD_V6_DATA_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V6_DATA/Summer16_03Feb2017BCD_V6_DATA_L3Absolute_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V6_DATA/Summer16_03Feb2017BCD_V6_DATA_L2L3Residual_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V7_L123_AK4PFchs_MC = {
  "JECDatabase/textFiles/Summer16_03Feb2017_V7_MC/Summer16_03Feb2017_V7_MC_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017_V7_MC/Summer16_03Feb2017_V7_MC_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017_V7_MC/Summer16_03Feb2017_V7_MC_L3Absolute_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V7_L123_AK8PFchs_MC = {
  "JECDatabase/textFiles/Summer16_03Feb2017_V7_MC/Summer16_03Feb2017_V7_MC_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017_V7_MC/Summer16_03Feb2017_V7_MC_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017_V7_MC/Summer16_03Feb2017_V7_MC_L3Absolute_AK8PFchs.txt",
};


const std::vector<std::string> JERFiles::Summer16_03Feb2017_V6_EF_L123_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017EF_V6_DATA/Summer16_03Feb2017EF_V6_DATA_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017EF_V6_DATA/Summer16_03Feb2017EF_V6_DATA_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017EF_V6_DATA/Summer16_03Feb2017EF_V6_DATA_L3Absolute_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017EF_V6_DATA/Summer16_03Feb2017EF_V6_DATA_L2L3Residual_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V6_G_L123_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017G_V6_DATA/Summer16_03Feb2017G_V6_DATA_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017G_V6_DATA/Summer16_03Feb2017G_V6_DATA_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017G_V6_DATA/Summer16_03Feb2017G_V6_DATA_L3Absolute_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017G_V6_DATA/Summer16_03Feb2017G_V6_DATA_L2L3Residual_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V6_H_L123_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017H_V6_DATA/Summer16_03Feb2017H_V6_DATA_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017H_V6_DATA/Summer16_03Feb2017H_V6_DATA_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017H_V6_DATA/Summer16_03Feb2017H_V6_DATA_L3Absolute_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017H_V6_DATA/Summer16_03Feb2017H_V6_DATA_L2L3Residual_AK8PFchs.txt",
};


const std::vector<std::string> JERFiles::Summer16_03Feb2017_V6_BCD_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V6_DATA/Summer16_03Feb2017BCD_V6_DATA_L1RC_AK4PFchs.txt"
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V6_EF_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017EF_V6_DATA/Summer16_03Feb2017EF_V6_DATA_L1RC_AK4PFchs.txt"
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V6_G_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017G_V6_DATA/Summer16_03Feb2017G_V6_DATA_L1RC_AK4PFchs.txt"
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V6_H_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017H_V6_DATA/Summer16_03Feb2017H_V6_DATA_L1RC_AK4PFchs.txt"
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V7_L1RC_AK4PFchs_MC = {
  "JECDatabase/textFiles/Summer16_03Feb2017_V7_MC/Summer16_03Feb2017_V7_MC_L1RC_AK4PFchs.txt"
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V7_L1RC_AK8PFchs_MC = {
  "JECDatabase/textFiles/Summer16_03Feb2017_V7_MC/Summer16_03Feb2017_V7_MC_L1RC_AK8PFchs.txt"
};

/* ++++++++++++++ Summer16_07Aug2017_V1_noRes needed for L2Res people +++++++++++++++ */
const std::vector<std::string> JERFiles::Summer16_07Aug2017_V1_BCD_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_07Aug2017BCD_V1_DATA/Summer16_07Aug2017BCD_V1_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_07Aug2017BCD_V1_DATA/Summer16_07Aug2017BCD_V1_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_07Aug2017BCD_V1_DATA/Summer16_07Aug2017BCD_V1_DATA_L3Absolute_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_07Aug2017_V1_EF_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_07Aug2017EF_V1_DATA/Summer16_07Aug2017EF_V1_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_07Aug2017EF_V1_DATA/Summer16_07Aug2017EF_V1_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_07Aug2017EF_V1_DATA/Summer16_07Aug2017EF_V1_DATA_L3Absolute_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_07Aug2017_V1_G_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_07Aug2017G_V1_DATA/Summer16_07Aug2017G_V1_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_07Aug2017G_V1_DATA/Summer16_07Aug2017G_V1_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_07Aug2017G_V1_DATA/Summer16_07Aug2017G_V1_DATA_L3Absolute_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_07Aug2017_V1_H_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_07Aug2017H_V1_DATA/Summer16_07Aug2017H_V1_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_07Aug2017H_V1_DATA/Summer16_07Aug2017H_V1_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_07Aug2017H_V1_DATA/Summer16_07Aug2017H_V1_DATA_L3Absolute_AK4PFchs.txt",
};


/*****************Summer16_07Aug2017_V1 --> Official JEC recommendation forLegacy2016 **********************************/
const std::vector<std::string> JERFiles::Summer16_07Aug2017_V1_BCD_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_07Aug2017BCD_V1_DATA/Summer16_07Aug2017BCD_V1_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_07Aug2017BCD_V1_DATA/Summer16_07Aug2017BCD_V1_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_07Aug2017BCD_V1_DATA/Summer16_07Aug2017BCD_V1_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_07Aug2017BCD_V1_DATA/Summer16_07Aug2017BCD_V1_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_07Aug2017_V1_EF_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_07Aug2017EF_V1_DATA/Summer16_07Aug2017EF_V1_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_07Aug2017EF_V1_DATA/Summer16_07Aug2017EF_V1_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_07Aug2017EF_V1_DATA/Summer16_07Aug2017EF_V1_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_07Aug2017EF_V1_DATA/Summer16_07Aug2017EF_V1_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_07Aug2017_V1_G_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_07Aug2017G_V1_DATA/Summer16_07Aug2017G_V1_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_07Aug2017G_V1_DATA/Summer16_07Aug2017G_V1_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_07Aug2017G_V1_DATA/Summer16_07Aug2017G_V1_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_07Aug2017G_V1_DATA/Summer16_07Aug2017G_V1_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_07Aug2017_V1_H_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_07Aug2017H_V1_DATA/Summer16_07Aug2017H_V1_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_07Aug2017H_V1_DATA/Summer16_07Aug2017H_V1_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_07Aug2017H_V1_DATA/Summer16_07Aug2017H_V1_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_07Aug2017H_V1_DATA/Summer16_07Aug2017H_V1_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_07Aug2017_V1_BCD_L123_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_07Aug2017BCD_V1_DATA/Summer16_07Aug2017BCD_V1_DATA_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_07Aug2017BCD_V1_DATA/Summer16_07Aug2017BCD_V1_DATA_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_07Aug2017BCD_V1_DATA/Summer16_07Aug2017BCD_V1_DATA_L3Absolute_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_07Aug2017BCD_V1_DATA/Summer16_07Aug2017BCD_V1_DATA_L2L3Residual_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_07Aug2017_V1_L123_AK4PFchs_MC = {
  "JECDatabase/textFiles/Summer16_07Aug2017_V1_MC/Summer16_07Aug2017_V1_MC_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_07Aug2017_V1_MC/Summer16_07Aug2017_V1_MC_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_07Aug2017_V1_MC/Summer16_07Aug2017_V1_MC_L3Absolute_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_07Aug2017_V1_L123_AK8PFchs_MC = {
  "JECDatabase/textFiles/Summer16_07Aug2017_V1_MC/Summer16_07Aug2017_V1_MC_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_07Aug2017_V1_MC/Summer16_07Aug2017_V1_MC_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_07Aug2017_V1_MC/Summer16_07Aug2017_V1_MC_L3Absolute_AK8PFchs.txt",
};


const std::vector<std::string> JERFiles::Summer16_07Aug2017_V1_EF_L123_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_07Aug2017EF_V1_DATA/Summer16_07Aug2017EF_V1_DATA_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_07Aug2017EF_V1_DATA/Summer16_07Aug2017EF_V1_DATA_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_07Aug2017EF_V1_DATA/Summer16_07Aug2017EF_V1_DATA_L3Absolute_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_07Aug2017EF_V1_DATA/Summer16_07Aug2017EF_V1_DATA_L2L3Residual_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_07Aug2017_V1_G_L123_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_07Aug2017G_V1_DATA/Summer16_07Aug2017G_V1_DATA_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_07Aug2017G_V1_DATA/Summer16_07Aug2017G_V1_DATA_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_07Aug2017G_V1_DATA/Summer16_07Aug2017G_V1_DATA_L3Absolute_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_07Aug2017G_V1_DATA/Summer16_07Aug2017G_V1_DATA_L2L3Residual_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_07Aug2017_V1_H_L123_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_07Aug2017H_V1_DATA/Summer16_07Aug2017H_V1_DATA_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_07Aug2017H_V1_DATA/Summer16_07Aug2017H_V1_DATA_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_07Aug2017H_V1_DATA/Summer16_07Aug2017H_V1_DATA_L3Absolute_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_07Aug2017H_V1_DATA/Summer16_07Aug2017H_V1_DATA_L2L3Residual_AK8PFchs.txt",
};


const std::vector<std::string> JERFiles::Summer16_07Aug2017_V1_BCD_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_07Aug2017BCD_V1_DATA/Summer16_07Aug2017BCD_V1_DATA_L1RC_AK4PFchs.txt"
};

const std::vector<std::string> JERFiles::Summer16_07Aug2017_V1_EF_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_07Aug2017EF_V1_DATA/Summer16_07Aug2017EF_V1_DATA_L1RC_AK4PFchs.txt"
};

const std::vector<std::string> JERFiles::Summer16_07Aug2017_V1_G_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_07Aug2017G_V1_DATA/Summer16_07Aug2017G_V1_DATA_L1RC_AK4PFchs.txt"
};

const std::vector<std::string> JERFiles::Summer16_07Aug2017_V1_H_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_07Aug2017H_V1_DATA/Summer16_07Aug2017H_V1_DATA_L1RC_AK4PFchs.txt"
};

const std::vector<std::string> JERFiles::Summer16_07Aug2017_V1_L1RC_AK4PFchs_MC = {
  "JECDatabase/textFiles/Summer16_07Aug2017_V1_MC/Summer16_07Aug2017_V1_MC_L1RC_AK4PFchs.txt"
};



namespace {
    
// to share some code between JetCorrector and JetLeptonCleaner, provide some methods
// dealing with jet energy corrections here:
std::unique_ptr<FactorizedJetCorrector> build_corrector(const std::vector<std::string> & filenames){
    std::vector<JetCorrectorParameters> pars;
    for(const auto & filename : filenames){
        pars.emplace_back(locate_file(filename));
    }
    return uhh2::make_unique<FactorizedJetCorrector>(pars);
}

  void correct_jet(FactorizedJetCorrector & corrector, Jet & jet, const Event & event, JetCorrectionUncertainty* jec_unc = NULL, int jec_unc_direction=0){
    auto factor_raw = jet.JEC_factor_raw();
    corrector.setJetPt(jet.pt() * factor_raw);
    corrector.setJetEta(jet.eta());
    corrector.setJetE(jet.energy() * factor_raw);
    corrector.setJetA(jet.jetArea());
    corrector.setRho(event.rho);
    auto correctionfactors = corrector.getSubCorrections();
    auto correctionfactor_L1  = correctionfactors.front();
    auto correctionfactor = correctionfactors.back();
    //    cout<<"correctionfactor = "<<correctionfactor<<endl;
    LorentzVector jet_v4_corrected = jet.v4() * (factor_raw *correctionfactor);
   
    if(jec_unc_direction!=0){
      if (jec_unc==NULL){
	std::cerr << "JEC variation should be applied, but JEC uncertainty object is NULL! Abort." << std::endl;
	exit(EXIT_FAILURE);
      }
      // ignore jets with very low pt or high eta, avoiding a crash from the JESUncertainty tool
      double pt = jet_v4_corrected.Pt();
      double eta = jet_v4_corrected.Eta();
      if (!(pt<5. || fabs(eta)>5.)) {
      
	jec_unc->setJetEta(eta);
	jec_unc->setJetPt(pt);
	
	double unc = 0.;	  
	if (jec_unc_direction == 1){
	  unc = jec_unc->getUncertainty(1);
	  correctionfactor *= (1 + fabs(unc));
	} else if (jec_unc_direction == -1){
	  unc = jec_unc->getUncertainty(-1);
	  correctionfactor *= (1 - fabs(unc));
	}
	jet_v4_corrected = jet.v4() * (factor_raw *correctionfactor);
      }
    }

  
    jet.set_v4(jet_v4_corrected);
    jet.set_JEC_factor_raw(1. / correctionfactor);
    jet.set_JEC_L1factor_raw(correctionfactor_L1);

  }

  
  //propagate to MET
  //apply type1 MET correction to RAW MET
  //NB: jet with substracted muon Pt should be used

  void correct_MET(const Event & event, const bool & do_L1corr, const bool do_L1RCcorr, FactorizedJetCorrector & corrector_L1RC){

    //we start from raw MET
    LorentzVector metv4= event.met->uncorr_v4();

    for(auto & jet : *event.jets){
      //thresholds on the corrected jets: pt > 15, EM fraction < 0.9
      if(jet.v4().Pt() > 15 && (jet.neutralEmEnergyFraction()+jet.chargedEmEnergyFraction())<0.9){
	auto factor_raw = jet.JEC_factor_raw();
	auto L1factor_raw = jet.JEC_L1factor_raw();
	corrector_L1RC.setJetPt(jet.pt() * factor_raw);
	corrector_L1RC.setJetEta(jet.eta());
	corrector_L1RC.setJetE(jet.energy() * factor_raw);
	corrector_L1RC.setJetA(jet.jetArea());
	corrector_L1RC.setRho(event.rho);
	auto correctionfactors_L1RC = corrector_L1RC.getSubCorrections();
	auto correctionfactor_L1RC  = correctionfactors_L1RC.back();


	LorentzVector L1corr =   (L1factor_raw*factor_raw)*jet.v4();            //L1 corrected jets
	LorentzVector L1RCcorr = (correctionfactor_L1RC*factor_raw)*jet.v4();   //L1RC corrected jets
	LorentzVector L123corr = jet.v4();                                      //L123 corrected jets (L23 in case of puppi)

	metv4 -=  L123corr;

	//For Puppi jets, no L1 correction is needed
	if(do_L1corr){
	  //	  std::cout<<"do_L1corr"<<std::endl;
	  //slimmed MET is corrected by L1FastJet, CHS MET is corrected by L1RC
	  if(!do_L1RCcorr) metv4 += L1corr;
	  else             metv4 += L1RCcorr;
	}
      }
    }

    event.met->set_pt(metv4.Pt());
    event.met->set_phi(metv4.Phi());
  }

  

JetCorrectionUncertainty* corrector_uncertainty(uhh2::Context & ctx, const std::vector<std::string> & filenames, int &direction){
    
    auto dir = ctx.get("jecsmear_direction", "nominal");
    if(dir == "up"){
        direction = 1;
    }
    else if(dir == "down"){
        direction = -1;
    }
    else if(dir != "nominal"){
        // direction = 0 is default
        throw runtime_error("JetCorrector: invalid value jecsmear_direction='" + dir + "' (valid: 'nominal', 'up', 'down')");
    }

    //initialize JetCorrectionUncertainty if shift direction is not "nominal", else return NULL pointer
    if(direction!=0){
      //take name from the L1FastJet correction (0th element of filenames) and replace "L1FastJet" by "Uncertainty" to get the proper name of the uncertainty file
      TString unc_file = locate_file(filenames[0]);
      if (unc_file.Contains("L1FastJet")) {
        unc_file.ReplaceAll("L1FastJet","Uncertainty");
      }
      else if (unc_file.Contains("L2Relative")) {
        unc_file.ReplaceAll("L2Relative","Uncertainty");
      }
      else {
        throw runtime_error("WARNING No JEC Uncertainty File found!");
      }
      JetCorrectionUncertainty* jec_uncertainty = new JetCorrectionUncertainty(unc_file.Data());
      return jec_uncertainty;
    }
    return NULL;
    
}

}

JetCorrector::JetCorrector(uhh2::Context & ctx, const std::vector<std::string> & filenames, const std::vector<std::string> & filenames_L1RC){
    corrector = build_corrector(filenames);
    direction = 0;

    //MET should only be corrected using AK8 jets, iff there is no AK4 collection that could be used for this because the calculation of our raw MET is based on AK4 jets
    used_ak4chs = ctx.get("JetCollection")=="slimmedJets";
    used_ak4puppi = ctx.get("JetCollection")=="slimmedJetsPuppi";
    metprop_possible_ak8chs = ctx.get("JetCollection")=="patJetsAK8PFCHS";
    metprop_possible_ak8puppi = ctx.get("JetCollection")=="patJetsAK8PFPUPPI";

    //MET is always corrected using the jet collection stated in the "JetCollection" Item in the context and only in case one of the stated jet collections is used. 
    //Particularly, only one of these two AK8 collections should be used.
    propagate_to_met = used_ak4chs || used_ak4puppi || metprop_possible_ak8chs || metprop_possible_ak8puppi;
    if(!propagate_to_met) cout << "WARNING in JetCorrections.cxx: You specified a jet collection in the 'JetCollection' item in the config file that is not suited to correct MET. You should change that if zou are using MET." << endl;

    //The first two collections are standard MET collections. The third one is only used for derivation of JECs
    used_slimmedmet = ctx.get("METName")=="slimmedMETs";
    used_puppimet = ctx.get("METName")=="slimmedMETsPuppi";
    used_chsmet = ctx.get("METName")=="slMETsCHS";
    if((used_slimmedmet || used_chsmet) && !(used_ak4chs || metprop_possible_ak8chs)) throw runtime_error("In JetCorrections.cxx: You are using slimmedMET or slMETchs but neither AK4CHS nor AK8CHS jets. You need to correct MET using one of those two.");
    else if(used_puppimet && !(used_ak4puppi || metprop_possible_ak8puppi)) throw runtime_error("In JetCorrections.cxx: You are using slimmedMETpuppi but neither AK4Puppi nor AK8Puppi jets. You need to correct MET using one of those two.");

    //if CHS MET is used, the correction is based on the (L123 - L1RC) scheme, else it is based on the standard (L123-L1).
    //See also: https://twiki.cern.ch/twiki/bin/viewauth/CMS/METType1Type2Formulae
    do_metL1RC = propagate_to_met && used_chsmet;
    
    //If using CHS MET and therefore going for (L123 - L1RC), the L1RC corrections have to be provided in a separate const std::vector<std::string>. This must only contain the L1RC correction.
    if(do_metL1RC && (filenames_L1RC.size() == 1)) corrector_L1RC = build_corrector(filenames_L1RC);
    else if(do_metL1RC && (filenames_L1RC.size() != 1)) throw runtime_error("In JetCorrections.cxx: CHS MET should be corrected via (L123 - L1RC) but the JERFile for L1RC is empty or too large (should have one entry).");
    //create dummy if L1RC is not needed. It is not applied anyway
    else corrector_L1RC = build_corrector(filenames);

    jec_uncertainty = corrector_uncertainty(ctx, filenames, direction) ;
}
    
bool JetCorrector::process(uhh2::Event & event){
    assert(event.jets);

    //apply jet corrections
    for(auto & jet : *event.jets){
      correct_jet(*corrector, jet, event, jec_uncertainty, direction);
    }
 
    /*
    //propagate jet corrections to MET
    bool correct_with_chs = used_ak4chs || metprop_possible_ak8chs;
    correct_MET(event, correct_with_chs, do_metL1RC, *corrector_L1RC); 
    */	

    return true;
}

bool JetCorrector::correct_met(uhh2::Event & event){
  assert(event.jets);

  //propagate jet corrections to MET
  bool correct_with_chs = used_ak4chs || metprop_possible_ak8chs;
  correct_MET(event, correct_with_chs, do_metL1RC, *corrector_L1RC); 
  
  return true;
}

// note: implement here because only here (and not in the header file), the destructor of FactorizedJetCorrector is known
JetCorrector::~JetCorrector(){}


TopJetCorrector::TopJetCorrector(uhh2::Context & ctx, const std::vector<std::string> & filenames){
    corrector = build_corrector(filenames);
    direction = 0;
    jec_uncertainty = corrector_uncertainty(ctx, filenames, direction) ;
}
    
bool TopJetCorrector::process(uhh2::Event & event){
    assert(event.topjets);
    for(auto & jet : *event.topjets){
        correct_jet(*corrector, jet, event, jec_uncertainty, direction);
    }
    return true;
}

// note: implement here because only here (and not in the header file), the destructor of FactorizedJetCorrector is known
TopJetCorrector::~TopJetCorrector(){}

SubJetCorrector::SubJetCorrector(uhh2::Context & ctx, const std::vector<std::string> & filenames){
    corrector = build_corrector(filenames);
    direction = 0;
    jec_uncertainty = corrector_uncertainty(ctx, filenames, direction) ;
}
    
bool SubJetCorrector::process(uhh2::Event & event){
    assert(event.topjets);
    for(auto & topjet : *event.topjets){
        auto subjets = topjet.subjets();
        for (auto & subjet : subjets) { 
            correct_jet(*corrector, subjet, event, jec_uncertainty, direction);
        }
        topjet.set_subjets(move(subjets));
    }
    return true;
}

// note: implement here because only here (and not in the header file), the destructor of FactorizedJetCorrector is known
SubJetCorrector::~SubJetCorrector(){}

GenericJetCorrector::GenericJetCorrector(uhh2::Context & ctx, const std::vector<std::string> & filenames, const std::string & collectionname){
    corrector = build_corrector(filenames);
    direction = 0;
    jec_uncertainty = corrector_uncertainty(ctx, filenames, direction) ;
    h_jets = ctx.get_handle<std::vector<Jet> >(collectionname);
}
    
bool GenericJetCorrector::process(uhh2::Event & event){

    const auto jets = &event.get(h_jets);
    assert(jets);
    for(auto & jet : *jets){
        correct_jet(*corrector, jet, event, jec_uncertainty, direction);
    }
    return true;
}

// note: implement here because only here (and not in the header file), the destructor of FactorizedJetCorrector is known
GenericJetCorrector::~GenericJetCorrector(){}

GenericTopJetCorrector::GenericTopJetCorrector(uhh2::Context & ctx, const std::vector<std::string> & filenames, const std::string & collectionname){
    corrector = build_corrector(filenames);
    direction = 0;
    jec_uncertainty = corrector_uncertainty(ctx, filenames, direction) ;
    h_jets = ctx.get_handle<std::vector<TopJet> >(collectionname);
}
    
bool GenericTopJetCorrector::process(uhh2::Event & event){

    const auto jets = &event.get(h_jets);
    assert(jets);
    for(auto & jet : *jets){
        correct_jet(*corrector, jet, event, jec_uncertainty, direction);
    }
    return true;
}

// note: implement here because only here (and not in the header file), the destructor of FactorizedJetCorrector is known
GenericTopJetCorrector::~GenericTopJetCorrector(){}

GenericSubJetCorrector::GenericSubJetCorrector(uhh2::Context & ctx, const std::vector<std::string> & filenames, const std::string & collectionname){
    corrector = build_corrector(filenames);
    direction = 0;
    jec_uncertainty = corrector_uncertainty(ctx, filenames, direction) ;
    h_jets = ctx.get_handle<std::vector<TopJet> >(collectionname);
}
    
bool GenericSubJetCorrector::process(uhh2::Event & event){

    const auto topjets = &event.get(h_jets);
    assert(topjets);
    for(auto & topjet : *topjets){
        auto subjets = topjet.subjets();
        for (auto & subjet : subjets) { 
            correct_jet(*corrector, subjet, event, jec_uncertainty, direction);
        }
        topjet.set_subjets(move(subjets));
    }
    return true;
}

// note: implement here because only here (and not in the header file), the destructor of FactorizedJetCorrector is known
GenericSubJetCorrector::~GenericSubJetCorrector(){}

// ** JetLeptonCleaner

JetLeptonCleaner::JetLeptonCleaner(uhh2::Context & ctx, const std::vector<std::string> & filenames){
    corrector = build_corrector(filenames);
    direction = 0;
    jec_uncertainty = corrector_uncertainty(ctx, filenames, direction) ;
}

bool JetLeptonCleaner::process(uhh2::Event & event){
    assert(event.jets);
    if(event.muons){
        for(const auto & mu : *event.muons){
            if(mu_id && !(mu_id(mu, event))) continue;
            for(auto & jet : *event.jets){
                if(deltaR(jet, mu) < drmax && jet.muonMultiplicity() > 0){
                    auto jet_p4_raw = jet.v4() * jet.JEC_factor_raw();
                    // note that muon energy fraction as stored in the jet refers to the raw jet energy.
                    double muon_energy_in_jet = jet_p4_raw.E() * jet.muonEnergyFraction();
                    double new_muon_energy_in_jet = muon_energy_in_jet - mu.energy();
                    
                    // test compatibility of the hypothesis that the muon has been clustered to the jet with
                    // the jet information. The hypothesis is rejected if the muon energy in the jet is too small
                    // (but allow 10% off). Note that in general (for muon multiplicity > 1), the muon energy in
                    // the jet might be larger than from the single muon; make sure to consider that in the test
                    // by requiring one direction in the comparison only in case the muon multiplicity is 1.
                    if(new_muon_energy_in_jet < -0.1 * mu.energy() || (jet.muonMultiplicity() == 1 && new_muon_energy_in_jet > 0.1 * mu.energy())){
                        continue;
                    }
                    jet_p4_raw -= mu.v4();
                    // if that subtraction flipped the jet direction (angle between new and old > 90 degrees or pi/2), emit a warning and set its momentum to 0.
                    // Only warn if pt > 5GeV (otherwise, the jet is 0 anyway for all practical purposes).
                    if(jet_p4_raw.pt() > 5 && deltaR(jet_p4_raw, jet) > M_PI/2){
                        cout << "Warning: subtracting lepton flipped jet direction" << endl;
                        jet.set_v4(LorentzVector());
                        continue;
                    }
                    // re-correct jet. First, set p4_raw = p4_corrected such that
                    // the 'correct_jet' method does what it should do if using JEC_factor_raw ...
                    jet.set_JEC_factor_raw(1.0);
                    jet.set_v4(jet_p4_raw);
                    // set new muon multiplicity and muon energy fraction:
                    jet.set_muonMultiplicity(jet.muonMultiplicity() - 1);
                    jet.set_muonEnergyFraction(max(new_muon_energy_in_jet / jet_p4_raw.E(), 0.0));
		    //                    correct_jet(*corrector, jet, event, jec_uncertainty, direction, false);
                    correct_jet(*corrector, jet, event, jec_uncertainty, direction);
                }
            }
        }
    }
    if(event.electrons){
        for(const auto & ele : *event.electrons){
            if(ele_id && !(ele_id(ele, event))) continue;
            for(auto & jet : *event.jets){
                if(deltaR(jet, ele) < drmax && jet.electronMultiplicity() > 0){
                    auto jet_p4_raw = jet.v4() * jet.JEC_factor_raw();
                    double electron_energy_in_jet = jet_p4_raw.E() * jet.chargedEmEnergyFraction();
                    double new_electron_energy_in_jet = electron_energy_in_jet - ele.energy();
                    
                    if(new_electron_energy_in_jet < -0.1 * ele.energy() || (jet.chargedEmEnergyFraction() == 1 && new_electron_energy_in_jet > 0.1 * ele.energy())){
                        continue;
                    }
                    jet_p4_raw -= ele.v4();
                    if(jet_p4_raw.pt() > 5 && deltaR(jet_p4_raw, jet) > M_PI/2){
                        cout << "Warning: subtracting lepton flipped jet direction" << endl;
                        jet.set_v4(LorentzVector());
                        continue;
                    }
                    // re-correct jet:
                    jet.set_JEC_factor_raw(1.0);
                    jet.set_v4(jet_p4_raw);
                    jet.set_electronMultiplicity(jet.electronMultiplicity() - 1);
                    jet.set_chargedEmEnergyFraction(max(new_electron_energy_in_jet / jet_p4_raw.E(), 0.0));
                    correct_jet(*corrector, jet, event, jec_uncertainty, direction);
                }
            }
        }
    }
    return true;
}

// see ~JetCorrector
JetLeptonCleaner::~JetLeptonCleaner(){}


//// jet-lepton cleaning module using the matching of the candidates' keys

JetLeptonCleaner_by_KEYmatching::JetLeptonCleaner_by_KEYmatching(uhh2::Context& ctx, const std::vector<std::string> & filenames, const std::string& jet_label){

    h_jets_ = ctx.get_handle<std::vector<Jet>>(jet_label);
    h_topjets_ = ctx.get_handle<std::vector<TopJet>>(jet_label);
    corrector = build_corrector(filenames);
    direction = 0;
    jec_uncertainty = corrector_uncertainty(ctx, filenames, direction) ;
}

JetLeptonCleaner_by_KEYmatching::~JetLeptonCleaner_by_KEYmatching(){}

bool JetLeptonCleaner_by_KEYmatching::do_cleaning(Jet & jet, uhh2::Event& event) {
  bool correct_p4(false);
  auto jet_p4_raw = jet.v4() * jet.JEC_factor_raw();

  const auto& jet_lepton_keys = jet.lepton_keys();

    // muon-cleaning
  if(event.muons){

    for(const auto& muo : *event.muons){

      if(muo_id && !(muo_id(muo, event))) continue;

      for(const auto& muo_cand : muo.source_candidates()){

        if(std::find(jet_lepton_keys.begin(), jet_lepton_keys.end(), muo_cand.key) != jet_lepton_keys.end()){

          correct_p4 = true;

          LorentzVectorXYZE muo_cand_p4;
          muo_cand_p4.SetPx(muo_cand.px);
          muo_cand_p4.SetPy(muo_cand.py);
          muo_cand_p4.SetPz(muo_cand.pz);
          muo_cand_p4.SetE (muo_cand.E);

          jet_p4_raw -= muo_cand_p4;
        }
      }
    }
  }

    // electron-cleaning
  if(event.electrons){

    for(const auto& ele : *event.electrons){

      if(ele_id && !(ele_id(ele, event))) continue;

      for(const auto& ele_cand : ele.source_candidates()){

        if(std::find(jet_lepton_keys.begin(), jet_lepton_keys.end(), ele_cand.key) != jet_lepton_keys.end()){

          correct_p4 = true;

          LorentzVectorXYZE ele_cand_p4;
          ele_cand_p4.SetPx(ele_cand.px);
          ele_cand_p4.SetPy(ele_cand.py);
          ele_cand_p4.SetPz(ele_cand.pz);
          ele_cand_p4.SetE (ele_cand.E);

          jet_p4_raw -= ele_cand_p4;
        }
      }
    }
  }

    // jet-p4 correction
  if(correct_p4){

    jet.set_JEC_factor_raw(1.);
    jet.set_v4(jet_p4_raw);

    correct_jet(*corrector, jet, event, jec_uncertainty, direction);
    return true;
  }
  return false;
}

bool JetLeptonCleaner_by_KEYmatching::process(uhh2::Event& event){

  std::vector<Jet> * jets = NULL;
  std::vector<TopJet> * topjets = NULL;
  if (event.is_valid(h_jets_)) {
    jets = &event.get(h_jets_);
  } else {
    assert(event.is_valid(h_topjets_));
    topjets = &event.get(h_topjets_);
  }

  if (jets) {
    for(auto& jet : *jets ){
      do_cleaning(jet, event);
    }
  }
  else if (topjets) {
    for(auto& jet : *topjets ){
      do_cleaning(jet, event);
    }
  }
  else {
    std::cerr << "Neither jet nor topjet collection found for this name!\n";
    assert(false);
  }

  return true;
    
}


//// ----- modules for Jet Energy Resolution data/MC corrections -----

// https://twiki.cern.ch/twiki/bin/view/CMS/JetResolution [13TeV JER measurement]

//New JER files from Jory (28 July 2017)
const JERSmearing::SFtype1 JERSmearing::SF_13TeV_2016_03Feb2017 = {
  // 0 = upper jet-eta limit
  // 1 = JER SF
  // 2 = JER SF + 1sigma
  // 3 = JER SF - 1sigma
  {{0.5, 1.093, 1.099, 1.087}},
  {{0.8, 1.143, 1.155, 1.131}},
  {{1.1, 1.087, 1.098, 1.076}},
  {{1.3, 1.119, 1.138, 1.100}},
  {{1.7, 1.093, 1.102, 1.084}},
  {{1.9, 1.072, 1.092, 1.052}},
  {{2.1, 1.122, 1.148, 1.096}},
  {{2.3, 1.127, 1.157, 1.097}},
  {{2.5, 1.206, 1.251, 1.161}},
  {{2.8, 1.340, 1.392, 1.288}},
  {{3.0, 1.761, 1.835, 1.687}},
  {{3.2, 1.317, 1.344, 1.290}},
  {{5.0, 1.140, 1.184, 1.096}},
};

const JERSmearing::SFtype1 JERSmearing::SF_13TeV_2016 = {
  // 0 = upper jet-eta limit
  // 1 = JER SF
  // 2 = JER SF + 1sigma
  // 3 = JER SF - 1sigma
  {{0.5, 1.109, 1.117, 1.101}},
  {{0.8, 1.138, 1.151, 1.125}},
  {{1.1, 1.114, 1.127, 1.101}},
  {{1.3, 1.123, 1.147, 1.099}},
  {{1.7, 1.084, 1.092, 1.070}},
  {{1.9, 1.082, 1.117, 1.047}},
  {{2.1, 1.140, 1.187, 1.093}},
  {{2.3, 1.067, 1.120, 1.014}},
  {{2.5, 1.177, 1.218, 1.136}},
  {{2.8, 1.364, 1.403, 1.325}},
  {{3.0, 1.857, 1.928, 1.786}},
  {{3.2, 1.328, 1.350, 1.306}},
  {{5.0, 1.160, 1.189, 1.131}},
};

////

JetResolutionSmearer::JetResolutionSmearer(uhh2::Context & ctx, const JERSmearing::SFtype1& JER_sf){
    auto dir = ctx.get("jersmear_direction", "nominal");
    if(dir == "up"){
        direction = 1;
    }
    else if(dir == "down"){
        direction = -1;
    }
    else if(dir != "nominal"){
        // direction = 0 is default
        throw runtime_error("JetResolutionSmearer: invalid value jersmear_direction='" + dir + "' (valid: 'nominal', 'up', 'down')");
    }
    if(ctx.get("meta_jer_applied", "") == "true"){
        throw runtime_error("JetResolutionSmearer: tried to apply jet resolution smearing, although metadata indicates that it already has been applied!");
    }
    ctx.set_metadata("jer_applied", "true");

    JER_SFs_ = JER_sf;
}

bool JetResolutionSmearer::process(uhh2::Event & event) {

    if(!event.jets || !event.genjets){
        throw runtime_error("JetResolutionSmearer: need jets and genjets to operate, but at least one of these is missing.");
    }
    
    LorentzVector met;
    if(event.met) {
      met = event.met->v4();
    }
    for(unsigned int i=0; i<event.jets->size(); ++i) {
      auto & jet = event.jets->at(i);
      // find next genjet:
      auto closest_genjet = closestParticle(jet, *event.genjets);
      // ignore unmatched jets (=no genjets at all or large DeltaR), or jets with very low genjet pt:
      if(closest_genjet == nullptr || deltaR(*closest_genjet, jet) > 0.3) continue;
      auto genpt = closest_genjet->pt();
      if(genpt < 15.0f) {
	continue;
      }
      LorentzVector jet_v4 = jet.v4();
      float recopt = jet_v4.pt();
      float abseta = fabs(jet_v4.eta());

      int ieta(-1);

      for(unsigned int idx=0; idx<JER_SFs_.size(); ++idx){

	const float min_eta = idx ? JER_SFs_.at(idx-1).at(0) : 0.;
	const float max_eta =       JER_SFs_.at(idx)  .at(0);

	if(min_eta <= abseta && abseta < max_eta){ ieta = idx; break; }
      }
      if(ieta < 0) {
	cout << "WARNING: JetResolutionSmearer: index for JER-smearing SF not found for jet with |eta| = " << abseta << endl;
	cout << "         no JER smearing is applied." << endl;
	continue;
      }

      float c;
      if(direction == 0){
	c = JER_SFs_.at(ieta).at(1);
      }
      else if(direction == 1){
	c = JER_SFs_.at(ieta).at(2);
      }
      else{
	c = JER_SFs_.at(ieta).at(3);
      }
      float new_pt = std::max(0.0f, genpt + c * (recopt - genpt));
      jet_v4 *= new_pt / recopt;

      //update JEC_factor_raw needed for smearing MET
      float factor_raw = jet.JEC_factor_raw();
      factor_raw *= recopt/new_pt;

      jet.set_JEC_factor_raw(factor_raw);
      jet.set_v4(jet_v4);
    }

    return true;
}

JetResolutionSmearer::~JetResolutionSmearer(){}

////

GenericJetResolutionSmearer::GenericJetResolutionSmearer(uhh2::Context& ctx, const std::string& recjet_label, const std::string& genjet_label, const bool allow_met_smearing, const JERSmearing::SFtype1& JER_sf){

  if(ctx.get("meta_jer_applied__"+recjet_label, "") != "true") ctx.set_metadata("jer_applied__"+recjet_label, "true");
  else throw std::runtime_error("GenericJetResolutionSmearer::GenericJetResolutionSmearer -- JER smearing already applied to this RECO-jets collection: "+recjet_label);

  const std::string& dir = ctx.get("jersmear_direction", "nominal");
  if     (dir == "nominal") direction =  0;
  else if(dir == "up")      direction =  1;
  else if(dir == "down")    direction = -1;
  else throw std::runtime_error("GenericJetResolutionSmearer::GenericJetResolutionSmearer -- invalid value jersmear_direction='"+dir+"' (valid: 'nominal', 'up', 'down')");

  h_recjets_    = ctx.get_handle<std::vector<Jet> >      (recjet_label);
  h_rectopjets_ = ctx.get_handle<std::vector<TopJet> >   (recjet_label);

  h_genjets_    = ctx.get_handle<std::vector<Particle> > (genjet_label);
  h_gentopjets_ = ctx.get_handle<std::vector<GenTopJet> >(genjet_label);

  JER_SFs_ = JER_sf;
}

bool GenericJetResolutionSmearer::process(uhh2::Event& evt){

  if(evt.isRealData) return true;

  std::vector<Jet>*    rec_jets(0);
  std::vector<TopJet>* rec_topjets(0);

  if     (evt.is_valid(h_recjets_))    rec_jets    = &evt.get(h_recjets_);
  else if(evt.is_valid(h_rectopjets_)) rec_topjets = &evt.get(h_rectopjets_);
  else throw std::runtime_error("GenericJetResolutionSmearer::process -- invalid handle to RECO-jets");

  std::vector<Particle>*  gen_jets(0);
  std::vector<GenTopJet>* gen_topjets(0);

  if     (evt.is_valid(h_genjets_))    gen_jets    = &evt.get(h_genjets_);
  else if(evt.is_valid(h_gentopjets_)) gen_topjets = &evt.get(h_gentopjets_);
  else throw std::runtime_error("GenericJetResolutionSmearer::process -- invalid handle to GEN-jets");

  LorentzVector met;
  if(evt.met) met = evt.met->v4();

  if     (rec_jets    && gen_jets)    apply_JER_smearing(*rec_jets   , *gen_jets   , met);
  else if(rec_topjets && gen_jets)    apply_JER_smearing(*rec_topjets, *gen_jets   , met);
  else if(rec_topjets && gen_topjets) apply_JER_smearing(*rec_topjets, *gen_topjets, met);
  else throw std::runtime_error("GenericJetResolutionSmearer::process -- invalid combination of RECO-GEN jet collections");

  return true;
}

//// -----------------------------------------------------------------

SoftDropMassCalculator::SoftDropMassCalculator(uhh2::Context & ctx, bool applyCorrections, const std::string & puppiCorrFilename, const std::string & jetCollName):
applyCorrections_(applyCorrections)
{
  h_topjets_ = ctx.get_handle<std::vector<TopJet>>(jetCollName);
  if (applyCorrections_) {
    puppiCorrFile.reset(TFile::Open(locate_file(puppiCorrFilename).c_str()));
    puppisd_corrGEN.reset((TF1*) puppiCorrFile->Get("puppiJECcorr_gen"));
    puppisd_corrRECO_cen.reset((TF1*) puppiCorrFile->Get("puppiJECcorr_reco_0eta1v3"));
    puppisd_corrRECO_for.reset((TF1*) puppiCorrFile->Get("puppiJECcorr_reco_1v3eta2v5"));
  }
}

bool SoftDropMassCalculator::process(uhh2::Event & evt) {
  std::vector<TopJet>* topjets(0);

  if (evt.is_valid(h_topjets_)) topjets = &evt.get(h_topjets_);
  else throw std::runtime_error("SoftDropMassCalculator::process -- invalid handle to topjets");

  for (auto & jet : *topjets) {
    float puppi_softdrop_mass = calcSDmass(jet);
    if (applyCorrections_) { puppi_softdrop_mass *= getPUPPIweight(jet.pt(), jet.eta()); }
    jet.set_softdropmass(puppi_softdrop_mass);
  }

  return true;
}

float SoftDropMassCalculator::calcSDmass(const TopJet & jet) {
  // Calculate uncorrected SD mass from subjets
  LorentzVector puppi_softdrop;
  for (auto & subjet : jet.subjets()) {
    // Important, must use UNCORRECTED subjet
    puppi_softdrop += (subjet.JEC_factor_raw() * subjet.v4());
  }
  return inv_mass_safe(puppi_softdrop);
}

float SoftDropMassCalculator::getPUPPIweight(float pt, float eta) {
  // TODO: add check /safety if outside x range
  // Calculate correction factor for SD mass
  // The final correction is a combination of 2 factors:
  // one that corrects reco -> gen,
  // and one that corrects the generator W mass to its PDG value.
  //
  // pt and eta shold be either from CHS AK8 jets or PUPPI
  // AK8 jets with full JEC applied,
  // but not the PUPPI softdrop AK8 jet pt and eta
  float genCorr = puppisd_corrGEN->Eval(pt);
  float recoCorr = (fabs(eta) <= 1.3) ? puppisd_corrRECO_cen->Eval(pt) : puppisd_corrRECO_for->Eval(pt);
  return genCorr * recoCorr;
}
