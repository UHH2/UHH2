#include "UHH2/common/include/JetCorrections.h"
#include "UHH2/common/include/Utils.h"
#include "UHH2/core/include/Utils.h"

#include "UHH2/JetMETObjects/interface/FactorizedJetCorrector.h"
#include "UHH2/JetMETObjects/interface/JetCorrectorParameters.h"

#include <string>

using namespace std;
using namespace uhh2;

//see https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookJetEnergyCorrections#GetTxtFiles how to get the txt files with jet energy corrections from the database

//TODO make this better redeable by generating all the pathes with loops over the different possibilities


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
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V3_DATA/Summer16_03Feb2017BCD_V3_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V3_DATA/Summer16_03Feb2017BCD_V3_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V3_DATA/Summer16_03Feb2017BCD_V3_DATA_L3Absolute_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V3_EF_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017EF_V3_DATA/Summer16_03Feb2017EF_V3_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017EF_V3_DATA/Summer16_03Feb2017EF_V3_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017EF_V3_DATA/Summer16_03Feb2017EF_V3_DATA_L3Absolute_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V3_G_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017G_V3_DATA/Summer16_03Feb2017G_V3_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017G_V3_DATA/Summer16_03Feb2017G_V3_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017G_V3_DATA/Summer16_03Feb2017G_V3_DATA_L3Absolute_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V3_H_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017H_V3_DATA/Summer16_03Feb2017H_V3_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017H_V3_DATA/Summer16_03Feb2017H_V3_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017H_V3_DATA/Summer16_03Feb2017H_V3_DATA_L3Absolute_AK4PFchs.txt",
};


/***************** Summer16_03Feb2017_V3 --> Official JEC recommendation for Moriond17 **********************************/
const std::vector<std::string> JERFiles::Summer16_03Feb2017_V3_BCD_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V3_DATA/Summer16_03Feb2017BCD_V3_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V3_DATA/Summer16_03Feb2017BCD_V3_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V3_DATA/Summer16_03Feb2017BCD_V3_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V3_DATA/Summer16_03Feb2017BCD_V3_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V3_EF_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017EF_V3_DATA/Summer16_03Feb2017EF_V3_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017EF_V3_DATA/Summer16_03Feb2017EF_V3_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017EF_V3_DATA/Summer16_03Feb2017EF_V3_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017EF_V3_DATA/Summer16_03Feb2017EF_V3_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V3_G_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017G_V3_DATA/Summer16_03Feb2017GV3_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017G_V3_DATA/Summer16_03Feb2017GV3_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017G_V3_DATA/Summer16_03Feb2017GV3_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017G_V3_DATA/Summer16_03Feb2017GV3_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V3_H_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017H_V3_DATA/Summer16_03Feb2017H_V3_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017H_V3_DATA/Summer16_03Feb2017H_V3_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017H_V3_DATA/Summer16_03Feb2017H_V3_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017H_V3_DATA/Summer16_03Feb2017H_V3_DATA_L2L3Residual_AK4PFchs.txt",
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

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V3_BCD_L123_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V3_DATA/Summer16_03Feb2017BCD_V3_DATA_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V3_DATA/Summer16_03Feb2017BCD_V3_DATA_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V3_DATA/Summer16_03Feb2017BCD_V3_DATA_L3Absolute_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V3_DATA/Summer16_03Feb2017BCD_V3_DATA_L2L3Residual_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V3_EF_L123_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017EF_V3_DATA/Summer16_03Feb2017EF_V3_DATA_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017EF_V3_DATA/Summer16_03Feb2017EF_V3_DATA_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017EF_V3_DATA/Summer16_03Feb2017EF_V3_DATA_L3Absolute_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017EF_V3_DATA/Summer16_03Feb2017EF_V3_DATA_L2L3Residual_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V3_G_L123_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017G_V3_DATA/Summer16_03Feb2017G_V3_DATA_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017G_V3_DATA/Summer16_03Feb2017G_V3_DATA_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017G_V3_DATA/Summer16_03Feb2017G_V3_DATA_L3Absolute_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017G_V3_DATA/Summer16_03Feb2017G_V3_DATA_L2L3Residual_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V3_H_L123_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017H_V3_DATA/Summer16_03Feb2017H_V3_DATA_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017H_V3_DATA/Summer16_03Feb2017H_V3_DATA_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017H_V3_DATA/Summer16_03Feb2017H_V3_DATA_L3Absolute_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017H_V3_DATA/Summer16_03Feb2017H_V3_DATA_L2L3Residual_AK8PFchs.txt",
};


const std::vector<std::string> JERFiles::Summer16_03Feb2017_V3_BCD_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017BCD_V3_DATA/Summer16_03Feb2017BCD_V3_DATA_L1RC_AK4PFchs.txt"
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V3_EF_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017EF_V3_DATA/Summer16_03Feb2017EF_V3_DATA_L1RC_AK4PFchs.txt"
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V3_G_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017G_V3_DATA/Summer16_03Feb2017G_V3_DATA_L1RC_AK4PFchs.txt"
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V3_H_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017H_V3_DATA/Summer16_03Feb2017H_V3_DATA_L1RC_AK4PFchs.txt"
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
  "JECDatabase/textFiles/Summer16_03Feb2017GV4_DATA/Summer16_03Feb2017G_V4_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017GV4_DATA/Summer16_03Feb2017G_V4_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017GV4_DATA/Summer16_03Feb2017G_V4_DATA_L3Absolute_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V4_H_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Summer16_03Feb2017H_V4_DATA/Summer16_03Feb2017H_V4_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017H_V4_DATA/Summer16_03Feb2017H_V4_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017H_V4_DATA/Summer16_03Feb2017H_V4_DATA_L3Absolute_AK4PFchs.txt",
};


/***************** Summer16_03Feb2017_V4 --> Official JEC recommendation for Moriond17 **********************************/
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
  "JECDatabase/textFiles/Summer16_03Feb2017GV4_DATA/Summer16_03Feb2017G_V4_DATA_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017GV4_DATA/Summer16_03Feb2017G_V4_DATA_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017GV4_DATA/Summer16_03Feb2017G_V4_DATA_L3Absolute_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017GV4_DATA/Summer16_03Feb2017G_V4_DATA_L2L3Residual_AK8PFchs.txt",
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

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V6_L123_AK4PFchs_MC = {
  "JECDatabase/textFiles/Summer16_03Feb2017_V6_MC/Summer16_03Feb2017_V6_MC_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017_V6_MC/Summer16_03Feb2017_V6_MC_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017_V6_MC/Summer16_03Feb2017_V6_MC_L3Absolute_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V6_L123_AK8PFchs_MC = {
  "JECDatabase/textFiles/Summer16_03Feb2017V6_MC/Summer16_03Feb2017V6_MC_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017V6_MC/Summer16_03Feb2017V6_MC_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Summer16_03Feb2017V6_MC/Summer16_03Feb2017V6_MC_L3Absolute_AK8PFchs.txt",
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

const std::vector<std::string> JERFiles::Summer16_03Feb2017_V6_L1RC_AK4PFchs_MC = {
  "JECDatabase/textFiles/Summer16_03Feb2017_V6_MC/Summer16_03Feb2017_V6_MC_L1RC_AK4PFchs.txt"
};


//2017-------------------------- 17Nov2017  V4
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V4_L123_AK4PFchs_MC = {
  "JECDatabase/textFiles/Fall17_17Nov2017_V4_MC/Fall17_17Nov2017_V4_MC_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017_V4_MC/Fall17_17Nov2017_V4_MC_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017_V4_MC/Fall17_17Nov2017_V4_MC_L3Absolute_AK4PFchs.txt",

};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V4_B_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V4_DATA/Fall17_17Nov2017B_V4_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V4_DATA/Fall17_17Nov2017B_V4_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V4_DATA/Fall17_17Nov2017B_V4_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V4_DATA/Fall17_17Nov2017B_V4_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V4_C_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V4_DATA/Fall17_17Nov2017C_V4_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V4_DATA/Fall17_17Nov2017C_V4_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V4_DATA/Fall17_17Nov2017C_V4_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V4_DATA/Fall17_17Nov2017C_V4_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V4_D_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017D_V4_DATA/Fall17_17Nov2017D_V4_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V4_DATA/Fall17_17Nov2017D_V4_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V4_DATA/Fall17_17Nov2017D_V4_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V4_DATA/Fall17_17Nov2017D_V4_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V4_E_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017E_V4_DATA/Fall17_17Nov2017E_V4_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V4_DATA/Fall17_17Nov2017E_V4_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V4_DATA/Fall17_17Nov2017E_V4_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V4_DATA/Fall17_17Nov2017E_V4_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V4_F_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V4_DATA/Fall17_17Nov2017F_V4_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V4_DATA/Fall17_17Nov2017F_V4_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V4_DATA/Fall17_17Nov2017F_V4_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V4_DATA/Fall17_17Nov2017F_V4_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V4_L1RC_AK4PFchs_MC = {
  "JECDatabase/textFiles/Fall17_17Nov2017_V4_MC/Fall17_17Nov2017_V4_MC_L1RC_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V4_B_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V4_DATA/Fall17_17Nov2017B_V4_DATA_L1RC_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V4_C_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V4_DATA/Fall17_17Nov2017C_V4_DATA_L1RC_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V4_D_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017D_V4_DATA/Fall17_17Nov2017D_V4_DATA_L1RC_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V4_E_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017E_V4_DATA/Fall17_17Nov2017E_V4_DATA_L1RC_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V4_F_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V4_DATA/Fall17_17Nov2017F_V4_DATA_L1RC_AK4PFchs.txt",

};




//2017-------------------------- 17Nov2017  V5
// const std::vector<std::string> JERFiles::Fall17_17Nov2017_V5_L123_AK4PFchs_MC = {
//   "JECDatabase/textFiles/Fall17_17Nov2017_V5_MC/Fall17_17Nov2017_V5_MC_L1FastJet_AK4PFchs.txt",
//   "JECDatabase/textFiles/Fall17_17Nov2017_V5_MC/Fall17_17Nov2017_V5_MC_L2Relative_AK4PFchs.txt",
//   "JECDatabase/textFiles/Fall17_17Nov2017_V5_MC/Fall17_17Nov2017_V5_MC_L3Absolute_AK4PFchs.txt",

// };


const std::vector<std::string> JERFiles::Fall17_17Nov2017_V5_B_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V5_DATA/Fall17_17Nov2017B_V5_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V5_DATA/Fall17_17Nov2017B_V5_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V5_DATA/Fall17_17Nov2017B_V5_DATA_L3Absolute_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V5_C_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V5_DATA/Fall17_17Nov2017C_V5_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V5_DATA/Fall17_17Nov2017C_V5_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V5_DATA/Fall17_17Nov2017C_V5_DATA_L3Absolute_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V5_D_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017D_V5_DATA/Fall17_17Nov2017D_V5_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V5_DATA/Fall17_17Nov2017D_V5_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V5_DATA/Fall17_17Nov2017D_V5_DATA_L3Absolute_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V5_E_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017E_V5_DATA/Fall17_17Nov2017E_V5_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V5_DATA/Fall17_17Nov2017E_V5_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V5_DATA/Fall17_17Nov2017E_V5_DATA_L3Absolute_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V5_F_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V5_DATA/Fall17_17Nov2017F_V5_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V5_DATA/Fall17_17Nov2017F_V5_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V5_DATA/Fall17_17Nov2017F_V5_DATA_L3Absolute_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V5_B_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V5_DATA/Fall17_17Nov2017B_V5_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V5_DATA/Fall17_17Nov2017B_V5_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V5_DATA/Fall17_17Nov2017B_V5_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V5_DATA/Fall17_17Nov2017B_V5_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V5_C_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V5_DATA/Fall17_17Nov2017C_V5_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V5_DATA/Fall17_17Nov2017C_V5_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V5_DATA/Fall17_17Nov2017C_V5_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V5_DATA/Fall17_17Nov2017C_V5_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V5_D_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017D_V5_DATA/Fall17_17Nov2017D_V5_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V5_DATA/Fall17_17Nov2017D_V5_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V5_DATA/Fall17_17Nov2017D_V5_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V5_DATA/Fall17_17Nov2017D_V5_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V5_E_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017E_V5_DATA/Fall17_17Nov2017E_V5_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V5_DATA/Fall17_17Nov2017E_V5_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V5_DATA/Fall17_17Nov2017E_V5_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V5_DATA/Fall17_17Nov2017E_V5_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V5_F_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V5_DATA/Fall17_17Nov2017F_V5_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V5_DATA/Fall17_17Nov2017F_V5_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V5_DATA/Fall17_17Nov2017F_V5_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V5_DATA/Fall17_17Nov2017F_V5_DATA_L2L3Residual_AK4PFchs.txt",
};

// const std::vector<std::string> JERFiles::Fall17_17Nov2017_V5_L1RC_AK4PFchs_MC = {
//   "JECDatabase/textFiles/Fall17_17Nov2017_V5_MC/Fall17_17Nov2017_V5_MC_L1RC_AK4PFchs.txt",
// };

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V5_B_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V5_DATA/Fall17_17Nov2017B_V5_DATA_L1RC_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V5_C_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V5_DATA/Fall17_17Nov2017C_V5_DATA_L1RC_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V5_D_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017D_V5_DATA/Fall17_17Nov2017D_V5_DATA_L1RC_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V5_E_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017E_V5_DATA/Fall17_17Nov2017E_V5_DATA_L1RC_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V5_F_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V5_DATA/Fall17_17Nov2017F_V5_DATA_L1RC_AK4PFchs.txt",

};




//2017-------------------------- 17Nov2017  V6
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_L123_AK4PFchs_MC = {
  "JECDatabase/textFiles/Fall17_17Nov2017_V6_MC/Fall17_17Nov2017_V6_MC_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017_V6_MC/Fall17_17Nov2017_V6_MC_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017_V6_MC/Fall17_17Nov2017_V6_MC_L3Absolute_AK4PFchs.txt",

};


const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_B_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V6_DATA/Fall17_17Nov2017B_V6_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V6_DATA/Fall17_17Nov2017B_V6_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V6_DATA/Fall17_17Nov2017B_V6_DATA_L3Absolute_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_C_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V6_DATA/Fall17_17Nov2017C_V6_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V6_DATA/Fall17_17Nov2017C_V6_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V6_DATA/Fall17_17Nov2017C_V6_DATA_L3Absolute_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_D_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017D_V6_DATA/Fall17_17Nov2017D_V6_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V6_DATA/Fall17_17Nov2017D_V6_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V6_DATA/Fall17_17Nov2017D_V6_DATA_L3Absolute_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_E_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017E_V6_DATA/Fall17_17Nov2017E_V6_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V6_DATA/Fall17_17Nov2017E_V6_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V6_DATA/Fall17_17Nov2017E_V6_DATA_L3Absolute_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_F_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V6_DATA/Fall17_17Nov2017F_V6_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V6_DATA/Fall17_17Nov2017F_V6_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V6_DATA/Fall17_17Nov2017F_V6_DATA_L3Absolute_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_B_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V6_DATA/Fall17_17Nov2017B_V6_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V6_DATA/Fall17_17Nov2017B_V6_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V6_DATA/Fall17_17Nov2017B_V6_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V6_DATA/Fall17_17Nov2017B_V6_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_C_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V6_DATA/Fall17_17Nov2017C_V6_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V6_DATA/Fall17_17Nov2017C_V6_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V6_DATA/Fall17_17Nov2017C_V6_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V6_DATA/Fall17_17Nov2017C_V6_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_D_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017D_V6_DATA/Fall17_17Nov2017D_V6_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V6_DATA/Fall17_17Nov2017D_V6_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V6_DATA/Fall17_17Nov2017D_V6_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V6_DATA/Fall17_17Nov2017D_V6_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_E_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017E_V6_DATA/Fall17_17Nov2017E_V6_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V6_DATA/Fall17_17Nov2017E_V6_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V6_DATA/Fall17_17Nov2017E_V6_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V6_DATA/Fall17_17Nov2017E_V6_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_F_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V6_DATA/Fall17_17Nov2017F_V6_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V6_DATA/Fall17_17Nov2017F_V6_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V6_DATA/Fall17_17Nov2017F_V6_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V6_DATA/Fall17_17Nov2017F_V6_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_L1RC_AK4PFchs_MC = {
  "JECDatabase/textFiles/Fall17_17Nov2017_V6_MC/Fall17_17Nov2017_V6_MC_L1RC_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_B_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V6_DATA/Fall17_17Nov2017B_V6_DATA_L1RC_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_C_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V6_DATA/Fall17_17Nov2017C_V6_DATA_L1RC_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_D_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017D_V6_DATA/Fall17_17Nov2017D_V6_DATA_L1RC_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_E_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017E_V6_DATA/Fall17_17Nov2017E_V6_DATA_L1RC_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_F_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V6_DATA/Fall17_17Nov2017F_V6_DATA_L1RC_AK4PFchs.txt",

};


const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_L123_AK4PFPuppi_MC = {
  "JECDatabase/textFiles/Fall17_17Nov2017_V6_MC/Fall17_17Nov2017_V6_MC_L1FastJet_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017_V6_MC/Fall17_17Nov2017_V6_MC_L2Relative_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017_V6_MC/Fall17_17Nov2017_V6_MC_L3Absolute_AK4PFPuppi.txt",

};


const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_B_L123_noRes_AK4PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V6_DATA/Fall17_17Nov2017B_V6_DATA_L1FastJet_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V6_DATA/Fall17_17Nov2017B_V6_DATA_L2Relative_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V6_DATA/Fall17_17Nov2017B_V6_DATA_L3Absolute_AK4PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_C_L123_noRes_AK4PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V6_DATA/Fall17_17Nov2017C_V6_DATA_L1FastJet_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V6_DATA/Fall17_17Nov2017C_V6_DATA_L2Relative_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V6_DATA/Fall17_17Nov2017C_V6_DATA_L3Absolute_AK4PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_D_L123_noRes_AK4PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017D_V6_DATA/Fall17_17Nov2017D_V6_DATA_L1FastJet_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V6_DATA/Fall17_17Nov2017D_V6_DATA_L2Relative_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V6_DATA/Fall17_17Nov2017D_V6_DATA_L3Absolute_AK4PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_E_L123_noRes_AK4PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017E_V6_DATA/Fall17_17Nov2017E_V6_DATA_L1FastJet_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V6_DATA/Fall17_17Nov2017E_V6_DATA_L2Relative_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V6_DATA/Fall17_17Nov2017E_V6_DATA_L3Absolute_AK4PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_F_L123_noRes_AK4PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V6_DATA/Fall17_17Nov2017F_V6_DATA_L1FastJet_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V6_DATA/Fall17_17Nov2017F_V6_DATA_L2Relative_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V6_DATA/Fall17_17Nov2017F_V6_DATA_L3Absolute_AK4PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_B_L123_AK4PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V6_DATA/Fall17_17Nov2017B_V6_DATA_L1FastJet_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V6_DATA/Fall17_17Nov2017B_V6_DATA_L2Relative_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V6_DATA/Fall17_17Nov2017B_V6_DATA_L3Absolute_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V6_DATA/Fall17_17Nov2017B_V6_DATA_L2L3Residual_AK4PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_C_L123_AK4PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V6_DATA/Fall17_17Nov2017C_V6_DATA_L1FastJet_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V6_DATA/Fall17_17Nov2017C_V6_DATA_L2Relative_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V6_DATA/Fall17_17Nov2017C_V6_DATA_L3Absolute_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V6_DATA/Fall17_17Nov2017C_V6_DATA_L2L3Residual_AK4PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_D_L123_AK4PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017D_V6_DATA/Fall17_17Nov2017D_V6_DATA_L1FastJet_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V6_DATA/Fall17_17Nov2017D_V6_DATA_L2Relative_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V6_DATA/Fall17_17Nov2017D_V6_DATA_L3Absolute_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V6_DATA/Fall17_17Nov2017D_V6_DATA_L2L3Residual_AK4PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_E_L123_AK4PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017E_V6_DATA/Fall17_17Nov2017E_V6_DATA_L1FastJet_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V6_DATA/Fall17_17Nov2017E_V6_DATA_L2Relative_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V6_DATA/Fall17_17Nov2017E_V6_DATA_L3Absolute_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V6_DATA/Fall17_17Nov2017E_V6_DATA_L2L3Residual_AK4PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_F_L123_AK4PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V6_DATA/Fall17_17Nov2017F_V6_DATA_L1FastJet_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V6_DATA/Fall17_17Nov2017F_V6_DATA_L2Relative_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V6_DATA/Fall17_17Nov2017F_V6_DATA_L3Absolute_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V6_DATA/Fall17_17Nov2017F_V6_DATA_L2L3Residual_AK4PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_L1RC_AK4PFPuppi_MC = {
  "JECDatabase/textFiles/Fall17_17Nov2017_V6_MC/Fall17_17Nov2017_V6_MC_L1RC_AK4PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_B_L1RC_AK4PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V6_DATA/Fall17_17Nov2017B_V6_DATA_L1RC_AK4PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_C_L1RC_AK4PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V6_DATA/Fall17_17Nov2017C_V6_DATA_L1RC_AK4PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_D_L1RC_AK4PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017D_V6_DATA/Fall17_17Nov2017D_V6_DATA_L1RC_AK4PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_E_L1RC_AK4PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017E_V6_DATA/Fall17_17Nov2017E_V6_DATA_L1RC_AK4PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_F_L1RC_AK4PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V6_DATA/Fall17_17Nov2017F_V6_DATA_L1RC_AK4PFPuppi.txt",

};




const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_L123_AK8PFchs_MC = {
  "JECDatabase/textFiles/Fall17_17Nov2017_V6_MC/Fall17_17Nov2017_V6_MC_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017_V6_MC/Fall17_17Nov2017_V6_MC_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017_V6_MC/Fall17_17Nov2017_V6_MC_L3Absolute_AK8PFchs.txt",

};


const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_B_L123_noRes_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V6_DATA/Fall17_17Nov2017B_V6_DATA_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V6_DATA/Fall17_17Nov2017B_V6_DATA_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V6_DATA/Fall17_17Nov2017B_V6_DATA_L3Absolute_AK8PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_C_L123_noRes_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V6_DATA/Fall17_17Nov2017C_V6_DATA_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V6_DATA/Fall17_17Nov2017C_V6_DATA_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V6_DATA/Fall17_17Nov2017C_V6_DATA_L3Absolute_AK8PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_D_L123_noRes_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017D_V6_DATA/Fall17_17Nov2017D_V6_DATA_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V6_DATA/Fall17_17Nov2017D_V6_DATA_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V6_DATA/Fall17_17Nov2017D_V6_DATA_L3Absolute_AK8PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_E_L123_noRes_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017E_V6_DATA/Fall17_17Nov2017E_V6_DATA_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V6_DATA/Fall17_17Nov2017E_V6_DATA_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V6_DATA/Fall17_17Nov2017E_V6_DATA_L3Absolute_AK8PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_F_L123_noRes_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V6_DATA/Fall17_17Nov2017F_V6_DATA_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V6_DATA/Fall17_17Nov2017F_V6_DATA_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V6_DATA/Fall17_17Nov2017F_V6_DATA_L3Absolute_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_B_L123_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V6_DATA/Fall17_17Nov2017B_V6_DATA_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V6_DATA/Fall17_17Nov2017B_V6_DATA_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V6_DATA/Fall17_17Nov2017B_V6_DATA_L3Absolute_AK8PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V6_DATA/Fall17_17Nov2017B_V6_DATA_L2L3Residual_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_C_L123_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V6_DATA/Fall17_17Nov2017C_V6_DATA_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V6_DATA/Fall17_17Nov2017C_V6_DATA_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V6_DATA/Fall17_17Nov2017C_V6_DATA_L3Absolute_AK8PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V6_DATA/Fall17_17Nov2017C_V6_DATA_L2L3Residual_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_D_L123_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017D_V6_DATA/Fall17_17Nov2017D_V6_DATA_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V6_DATA/Fall17_17Nov2017D_V6_DATA_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V6_DATA/Fall17_17Nov2017D_V6_DATA_L3Absolute_AK8PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V6_DATA/Fall17_17Nov2017D_V6_DATA_L2L3Residual_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_E_L123_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017E_V6_DATA/Fall17_17Nov2017E_V6_DATA_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V6_DATA/Fall17_17Nov2017E_V6_DATA_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V6_DATA/Fall17_17Nov2017E_V6_DATA_L3Absolute_AK8PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V6_DATA/Fall17_17Nov2017E_V6_DATA_L2L3Residual_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_F_L123_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V6_DATA/Fall17_17Nov2017F_V6_DATA_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V6_DATA/Fall17_17Nov2017F_V6_DATA_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V6_DATA/Fall17_17Nov2017F_V6_DATA_L3Absolute_AK8PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V6_DATA/Fall17_17Nov2017F_V6_DATA_L2L3Residual_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_L1RC_AK8PFchs_MC = {
  "JECDatabase/textFiles/Fall17_17Nov2017_V6_MC/Fall17_17Nov2017_V6_MC_L1RC_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_B_L1RC_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V6_DATA/Fall17_17Nov2017B_V6_DATA_L1RC_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_C_L1RC_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V6_DATA/Fall17_17Nov2017C_V6_DATA_L1RC_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_D_L1RC_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017D_V6_DATA/Fall17_17Nov2017D_V6_DATA_L1RC_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_E_L1RC_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017E_V6_DATA/Fall17_17Nov2017E_V6_DATA_L1RC_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_F_L1RC_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V6_DATA/Fall17_17Nov2017F_V6_DATA_L1RC_AK8PFchs.txt",

};


const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_L123_AK8PFPuppi_MC = {
  "JECDatabase/textFiles/Fall17_17Nov2017_V6_MC/Fall17_17Nov2017_V6_MC_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017_V6_MC/Fall17_17Nov2017_V6_MC_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017_V6_MC/Fall17_17Nov2017_V6_MC_L3Absolute_AK8PFPuppi.txt",

};


const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_B_L123_noRes_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V6_DATA/Fall17_17Nov2017B_V6_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V6_DATA/Fall17_17Nov2017B_V6_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V6_DATA/Fall17_17Nov2017B_V6_DATA_L3Absolute_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_C_L123_noRes_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V6_DATA/Fall17_17Nov2017C_V6_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V6_DATA/Fall17_17Nov2017C_V6_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V6_DATA/Fall17_17Nov2017C_V6_DATA_L3Absolute_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_D_L123_noRes_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017D_V6_DATA/Fall17_17Nov2017D_V6_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V6_DATA/Fall17_17Nov2017D_V6_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V6_DATA/Fall17_17Nov2017D_V6_DATA_L3Absolute_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_E_L123_noRes_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017E_V6_DATA/Fall17_17Nov2017E_V6_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V6_DATA/Fall17_17Nov2017E_V6_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V6_DATA/Fall17_17Nov2017E_V6_DATA_L3Absolute_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_F_L123_noRes_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V6_DATA/Fall17_17Nov2017F_V6_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V6_DATA/Fall17_17Nov2017F_V6_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V6_DATA/Fall17_17Nov2017F_V6_DATA_L3Absolute_AK8PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_B_L123_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V6_DATA/Fall17_17Nov2017B_V6_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V6_DATA/Fall17_17Nov2017B_V6_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V6_DATA/Fall17_17Nov2017B_V6_DATA_L3Absolute_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V6_DATA/Fall17_17Nov2017B_V6_DATA_L2L3Residual_AK8PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_C_L123_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V6_DATA/Fall17_17Nov2017C_V6_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V6_DATA/Fall17_17Nov2017C_V6_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V6_DATA/Fall17_17Nov2017C_V6_DATA_L3Absolute_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V6_DATA/Fall17_17Nov2017C_V6_DATA_L2L3Residual_AK8PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_D_L123_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017D_V6_DATA/Fall17_17Nov2017D_V6_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V6_DATA/Fall17_17Nov2017D_V6_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V6_DATA/Fall17_17Nov2017D_V6_DATA_L3Absolute_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V6_DATA/Fall17_17Nov2017D_V6_DATA_L2L3Residual_AK8PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_E_L123_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017E_V6_DATA/Fall17_17Nov2017E_V6_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V6_DATA/Fall17_17Nov2017E_V6_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V6_DATA/Fall17_17Nov2017E_V6_DATA_L3Absolute_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V6_DATA/Fall17_17Nov2017E_V6_DATA_L2L3Residual_AK8PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_F_L123_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V6_DATA/Fall17_17Nov2017F_V6_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V6_DATA/Fall17_17Nov2017F_V6_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V6_DATA/Fall17_17Nov2017F_V6_DATA_L3Absolute_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V6_DATA/Fall17_17Nov2017F_V6_DATA_L2L3Residual_AK8PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_L1RC_AK8PFPuppi_MC = {
  "JECDatabase/textFiles/Fall17_17Nov2017_V6_MC/Fall17_17Nov2017_V6_MC_L1RC_AK8PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_B_L1RC_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V6_DATA/Fall17_17Nov2017B_V6_DATA_L1RC_AK8PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_C_L1RC_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V6_DATA/Fall17_17Nov2017C_V6_DATA_L1RC_AK8PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_D_L1RC_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017D_V6_DATA/Fall17_17Nov2017D_V6_DATA_L1RC_AK8PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_E_L1RC_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017E_V6_DATA/Fall17_17Nov2017E_V6_DATA_L1RC_AK8PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V6_F_L1RC_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V6_DATA/Fall17_17Nov2017F_V6_DATA_L1RC_AK8PFPuppi.txt",

};







//2017-------------------------- 17Nov2017  V7
// const std::vector<std::string> JERFiles::Fall17_17Nov2017_V7_L123_AK4PFchs_MC = {
//   "JECDatabase/textFiles/Fall17_17Nov2017_V7_MC/Fall17_17Nov2017_V7_MC_L1FastJet_AK4PFchs.txt",
//   "JECDatabase/textFiles/Fall17_17Nov2017_V7_MC/Fall17_17Nov2017_V7_MC_L2Relative_AK4PFchs.txt",
//   "JECDatabase/textFiles/Fall17_17Nov2017_V7_MC/Fall17_17Nov2017_V7_MC_L3Absolute_AK4PFchs.txt",

// };


const std::vector<std::string> JERFiles::Fall17_17Nov2017_V7_B_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V7_DATA/Fall17_17Nov2017B_V7_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V7_DATA/Fall17_17Nov2017B_V7_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V7_DATA/Fall17_17Nov2017B_V7_DATA_L3Absolute_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V7_C_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V7_DATA/Fall17_17Nov2017C_V7_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V7_DATA/Fall17_17Nov2017C_V7_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V7_DATA/Fall17_17Nov2017C_V7_DATA_L3Absolute_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V7_D_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017D_V7_DATA/Fall17_17Nov2017D_V7_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V7_DATA/Fall17_17Nov2017D_V7_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V7_DATA/Fall17_17Nov2017D_V7_DATA_L3Absolute_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V7_E_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017E_V7_DATA/Fall17_17Nov2017E_V7_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V7_DATA/Fall17_17Nov2017E_V7_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V7_DATA/Fall17_17Nov2017E_V7_DATA_L3Absolute_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V7_F_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V7_DATA/Fall17_17Nov2017F_V7_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V7_DATA/Fall17_17Nov2017F_V7_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V7_DATA/Fall17_17Nov2017F_V7_DATA_L3Absolute_AK4PFchs.txt",
};


const std::vector<std::string> JERFiles::Fall17_17Nov2017_V7_B_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V7_DATA/Fall17_17Nov2017B_V7_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V7_DATA/Fall17_17Nov2017B_V7_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V7_DATA/Fall17_17Nov2017B_V7_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V7_DATA/Fall17_17Nov2017B_V7_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V7_C_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V7_DATA/Fall17_17Nov2017C_V7_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V7_DATA/Fall17_17Nov2017C_V7_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V7_DATA/Fall17_17Nov2017C_V7_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V7_DATA/Fall17_17Nov2017C_V7_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V7_D_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017D_V7_DATA/Fall17_17Nov2017D_V7_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V7_DATA/Fall17_17Nov2017D_V7_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V7_DATA/Fall17_17Nov2017D_V7_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V7_DATA/Fall17_17Nov2017D_V7_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V7_E_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017E_V7_DATA/Fall17_17Nov2017E_V7_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V7_DATA/Fall17_17Nov2017E_V7_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V7_DATA/Fall17_17Nov2017E_V7_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V7_DATA/Fall17_17Nov2017E_V7_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V7_F_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V7_DATA/Fall17_17Nov2017F_V7_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V7_DATA/Fall17_17Nov2017F_V7_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V7_DATA/Fall17_17Nov2017F_V7_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V7_DATA/Fall17_17Nov2017F_V7_DATA_L2L3Residual_AK4PFchs.txt",
};

// const std::vector<std::string> JERFiles::Fall17_17Nov2017_V7_L1RC_AK4PFchs_MC = {
//   "JECDatabase/textFiles/Fall17_17Nov2017_V7_MC/Fall17_17Nov2017_V7_MC_L1RC_AK4PFchs.txt",
// };

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V7_B_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V7_DATA/Fall17_17Nov2017B_V7_DATA_L1RC_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V7_C_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V7_DATA/Fall17_17Nov2017C_V7_DATA_L1RC_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V7_D_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017D_V7_DATA/Fall17_17Nov2017D_V7_DATA_L1RC_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V7_E_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017E_V7_DATA/Fall17_17Nov2017E_V7_DATA_L1RC_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V7_F_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V7_DATA/Fall17_17Nov2017F_V7_DATA_L1RC_AK4PFchs.txt",

};




//Fall17_17Nov2017_V10
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V10_L123_AK4PFchs_MC = {
  "JECDatabase/textFiles/Fall17_17Nov2017_V10_MC/Fall17_17Nov2017_V10_MC_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017_V10_MC/Fall17_17Nov2017_V10_MC_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017_V10_MC/Fall17_17Nov2017_V10_MC_L3Absolute_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V10_L1RC_AK4PFchs_MC = {
  "JECDatabase/textFiles/Fall17_17Nov2017_V10_MC/Fall17_17Nov2017_V10_MC_L1RC_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V10_L123_AK8PFPuppi_MC = {
  "JECDatabase/textFiles/Fall17_17Nov2017_V10_MC/Fall17_17Nov2017_V10_MC_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017_V10_MC/Fall17_17Nov2017_V10_MC_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017_V10_MC/Fall17_17Nov2017_V10_MC_L3Absolute_AK8PFPuppi.txt",
};


const std::vector<std::string> JERFiles::Fall17_17Nov2017_V10_B_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V10_DATA/Fall17_17Nov2017B_V10_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V10_DATA/Fall17_17Nov2017B_V10_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V10_DATA/Fall17_17Nov2017B_V10_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V10_DATA/Fall17_17Nov2017B_V10_DATA_L2L3Residual_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V10_C_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V10_DATA/Fall17_17Nov2017C_V10_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V10_DATA/Fall17_17Nov2017C_V10_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V10_DATA/Fall17_17Nov2017C_V10_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V10_DATA/Fall17_17Nov2017C_V10_DATA_L2L3Residual_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V10_D_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017D_V10_DATA/Fall17_17Nov2017D_V10_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V10_DATA/Fall17_17Nov2017D_V10_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V10_DATA/Fall17_17Nov2017D_V10_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V10_DATA/Fall17_17Nov2017D_V10_DATA_L2L3Residual_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V10_E_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017E_V10_DATA/Fall17_17Nov2017E_V10_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V10_DATA/Fall17_17Nov2017E_V10_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V10_DATA/Fall17_17Nov2017E_V10_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V10_DATA/Fall17_17Nov2017E_V10_DATA_L2L3Residual_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V10_F_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V10_DATA/Fall17_17Nov2017F_V10_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V10_DATA/Fall17_17Nov2017F_V10_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V10_DATA/Fall17_17Nov2017F_V10_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V10_DATA/Fall17_17Nov2017F_V10_DATA_L2L3Residual_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V10_B_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V10_DATA/Fall17_17Nov2017B_V10_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V10_DATA/Fall17_17Nov2017B_V10_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V10_DATA/Fall17_17Nov2017B_V10_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V10_DATA/Fall17_17Nov2017B_V10_DATA_L2L3Residual_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V10_C_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V10_DATA/Fall17_17Nov2017C_V10_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V10_DATA/Fall17_17Nov2017C_V10_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V10_DATA/Fall17_17Nov2017C_V10_DATA_L3Absolute_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V10_D_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017D_V10_DATA/Fall17_17Nov2017D_V10_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V10_DATA/Fall17_17Nov2017D_V10_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V10_DATA/Fall17_17Nov2017D_V10_DATA_L3Absolute_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V10_E_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017E_V10_DATA/Fall17_17Nov2017E_V10_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V10_DATA/Fall17_17Nov2017E_V10_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V10_DATA/Fall17_17Nov2017E_V10_DATA_L3Absolute_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V10_F_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V10_DATA/Fall17_17Nov2017F_V10_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V10_DATA/Fall17_17Nov2017F_V10_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V10_DATA/Fall17_17Nov2017F_V10_DATA_L3Absolute_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V10_B_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V10_DATA/Fall17_17Nov2017B_V10_DATA_L1RC_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V10_C_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V10_DATA/Fall17_17Nov2017C_V10_DATA_L1RC_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V10_D_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017D_V10_DATA/Fall17_17Nov2017D_V10_DATA_L1RC_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V10_E_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017E_V10_DATA/Fall17_17Nov2017E_V10_DATA_L1RC_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V10_F_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V10_DATA/Fall17_17Nov2017F_V10_DATA_L1RC_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V10_B_L123_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V10_DATA/Fall17_17Nov2017B_V10_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V10_DATA/Fall17_17Nov2017B_V10_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V10_DATA/Fall17_17Nov2017B_V10_DATA_L3Absolute_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V10_DATA/Fall17_17Nov2017B_V10_DATA_L2L3Residual_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V10_C_L123_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V10_DATA/Fall17_17Nov2017C_V10_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V10_DATA/Fall17_17Nov2017C_V10_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V10_DATA/Fall17_17Nov2017C_V10_DATA_L3Absolute_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V10_DATA/Fall17_17Nov2017C_V10_DATA_L2L3Residual_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V10_D_L123_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017D_V10_DATA/Fall17_17Nov2017D_V10_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V10_DATA/Fall17_17Nov2017D_V10_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V10_DATA/Fall17_17Nov2017D_V10_DATA_L3Absolute_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V10_DATA/Fall17_17Nov2017D_V10_DATA_L2L3Residual_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V10_E_L123_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017E_V10_DATA/Fall17_17Nov2017E_V10_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V10_DATA/Fall17_17Nov2017E_V10_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V10_DATA/Fall17_17Nov2017E_V10_DATA_L3Absolute_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V10_DATA/Fall17_17Nov2017E_V10_DATA_L2L3Residual_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V10_F_L123_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V10_DATA/Fall17_17Nov2017F_V10_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V10_DATA/Fall17_17Nov2017F_V10_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V10_DATA/Fall17_17Nov2017F_V10_DATA_L3Absolute_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V10_DATA/Fall17_17Nov2017F_V10_DATA_L2L3Residual_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V10_B_L123_noRes_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V10_DATA/Fall17_17Nov2017B_V10_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V10_DATA/Fall17_17Nov2017B_V10_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V10_DATA/Fall17_17Nov2017B_V10_DATA_L3Absolute_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V10_DATA/Fall17_17Nov2017B_V10_DATA_L2L3Residual_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V10_C_L123_noRes_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V10_DATA/Fall17_17Nov2017C_V10_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V10_DATA/Fall17_17Nov2017C_V10_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V10_DATA/Fall17_17Nov2017C_V10_DATA_L3Absolute_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V10_D_L123_noRes_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017D_V10_DATA/Fall17_17Nov2017D_V10_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V10_DATA/Fall17_17Nov2017D_V10_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V10_DATA/Fall17_17Nov2017D_V10_DATA_L3Absolute_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V10_E_L123_noRes_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017E_V10_DATA/Fall17_17Nov2017E_V10_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V10_DATA/Fall17_17Nov2017E_V10_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V10_DATA/Fall17_17Nov2017E_V10_DATA_L3Absolute_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V10_F_L123_noRes_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V10_DATA/Fall17_17Nov2017F_V10_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V10_DATA/Fall17_17Nov2017F_V10_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V10_DATA/Fall17_17Nov2017F_V10_DATA_L3Absolute_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V10_B_L1RC_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V10_DATA/Fall17_17Nov2017B_V10_DATA_L1RC_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V10_C_L1RC_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V10_DATA/Fall17_17Nov2017C_V10_DATA_L1RC_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V10_D_L1RC_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017D_V10_DATA/Fall17_17Nov2017D_V10_DATA_L1RC_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V10_E_L1RC_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017E_V10_DATA/Fall17_17Nov2017E_V10_DATA_L1RC_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V10_F_L1RC_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V10_DATA/Fall17_17Nov2017F_V10_DATA_L1RC_AK8PFPuppi.txt",
};




//Fall17_17Nov2017_V11
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V11_L123_AK4PFchs_MC = {
  "JECDatabase/textFiles/Fall17_17Nov2017_V11_MC/Fall17_17Nov2017_V11_MC_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017_V11_MC/Fall17_17Nov2017_V11_MC_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017_V11_MC/Fall17_17Nov2017_V11_MC_L3Absolute_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V11_L1RC_AK4PFchs_MC = {
  "JECDatabase/textFiles/Fall17_17Nov2017_V11_MC/Fall17_17Nov2017_V11_MC_L1RC_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V11_L123_AK8PFPuppi_MC = {
  "JECDatabase/textFiles/Fall17_17Nov2017_V11_MC/Fall17_17Nov2017_V11_MC_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017_V11_MC/Fall17_17Nov2017_V11_MC_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017_V11_MC/Fall17_17Nov2017_V11_MC_L3Absolute_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V11_B_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V11_DATA/Fall17_17Nov2017B_V11_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V11_DATA/Fall17_17Nov2017B_V11_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V11_DATA/Fall17_17Nov2017B_V11_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V11_DATA/Fall17_17Nov2017B_V11_DATA_L2L3Residual_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V11_C_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V11_DATA/Fall17_17Nov2017C_V11_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V11_DATA/Fall17_17Nov2017C_V11_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V11_DATA/Fall17_17Nov2017C_V11_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V11_DATA/Fall17_17Nov2017C_V11_DATA_L2L3Residual_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V11_D_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017D_V11_DATA/Fall17_17Nov2017D_V11_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V11_DATA/Fall17_17Nov2017D_V11_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V11_DATA/Fall17_17Nov2017D_V11_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V11_DATA/Fall17_17Nov2017D_V11_DATA_L2L3Residual_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V11_E_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017E_V11_DATA/Fall17_17Nov2017E_V11_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V11_DATA/Fall17_17Nov2017E_V11_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V11_DATA/Fall17_17Nov2017E_V11_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V11_DATA/Fall17_17Nov2017E_V11_DATA_L2L3Residual_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V11_F_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V11_DATA/Fall17_17Nov2017F_V11_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V11_DATA/Fall17_17Nov2017F_V11_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V11_DATA/Fall17_17Nov2017F_V11_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V11_DATA/Fall17_17Nov2017F_V11_DATA_L2L3Residual_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V11_B_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V11_DATA/Fall17_17Nov2017B_V11_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V11_DATA/Fall17_17Nov2017B_V11_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V11_DATA/Fall17_17Nov2017B_V11_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V11_DATA/Fall17_17Nov2017B_V11_DATA_L2L3Residual_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V11_C_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V11_DATA/Fall17_17Nov2017C_V11_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V11_DATA/Fall17_17Nov2017C_V11_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V11_DATA/Fall17_17Nov2017C_V11_DATA_L3Absolute_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V11_D_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017D_V11_DATA/Fall17_17Nov2017D_V11_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V11_DATA/Fall17_17Nov2017D_V11_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V11_DATA/Fall17_17Nov2017D_V11_DATA_L3Absolute_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V11_E_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017E_V11_DATA/Fall17_17Nov2017E_V11_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V11_DATA/Fall17_17Nov2017E_V11_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V11_DATA/Fall17_17Nov2017E_V11_DATA_L3Absolute_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V11_F_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V11_DATA/Fall17_17Nov2017F_V11_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V11_DATA/Fall17_17Nov2017F_V11_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V11_DATA/Fall17_17Nov2017F_V11_DATA_L3Absolute_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V11_B_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V11_DATA/Fall17_17Nov2017B_V11_DATA_L1RC_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V11_C_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V11_DATA/Fall17_17Nov2017C_V11_DATA_L1RC_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V11_D_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017D_V11_DATA/Fall17_17Nov2017D_V11_DATA_L1RC_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V11_E_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017E_V11_DATA/Fall17_17Nov2017E_V11_DATA_L1RC_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V11_F_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V11_DATA/Fall17_17Nov2017F_V11_DATA_L1RC_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V11_B_L123_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V11_DATA/Fall17_17Nov2017B_V11_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V11_DATA/Fall17_17Nov2017B_V11_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V11_DATA/Fall17_17Nov2017B_V11_DATA_L3Absolute_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V11_DATA/Fall17_17Nov2017B_V11_DATA_L2L3Residual_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V11_C_L123_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V11_DATA/Fall17_17Nov2017C_V11_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V11_DATA/Fall17_17Nov2017C_V11_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V11_DATA/Fall17_17Nov2017C_V11_DATA_L3Absolute_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V11_DATA/Fall17_17Nov2017C_V11_DATA_L2L3Residual_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V11_D_L123_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017D_V11_DATA/Fall17_17Nov2017D_V11_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V11_DATA/Fall17_17Nov2017D_V11_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V11_DATA/Fall17_17Nov2017D_V11_DATA_L3Absolute_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V11_DATA/Fall17_17Nov2017D_V11_DATA_L2L3Residual_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V11_E_L123_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017E_V11_DATA/Fall17_17Nov2017E_V11_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V11_DATA/Fall17_17Nov2017E_V11_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V11_DATA/Fall17_17Nov2017E_V11_DATA_L3Absolute_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V11_DATA/Fall17_17Nov2017E_V11_DATA_L2L3Residual_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V11_F_L123_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V11_DATA/Fall17_17Nov2017F_V11_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V11_DATA/Fall17_17Nov2017F_V11_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V11_DATA/Fall17_17Nov2017F_V11_DATA_L3Absolute_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V11_DATA/Fall17_17Nov2017F_V11_DATA_L2L3Residual_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V11_B_L123_noRes_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V11_DATA/Fall17_17Nov2017B_V11_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V11_DATA/Fall17_17Nov2017B_V11_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V11_DATA/Fall17_17Nov2017B_V11_DATA_L3Absolute_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V11_DATA/Fall17_17Nov2017B_V11_DATA_L2L3Residual_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V11_C_L123_noRes_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V11_DATA/Fall17_17Nov2017C_V11_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V11_DATA/Fall17_17Nov2017C_V11_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V11_DATA/Fall17_17Nov2017C_V11_DATA_L3Absolute_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V11_D_L123_noRes_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017D_V11_DATA/Fall17_17Nov2017D_V11_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V11_DATA/Fall17_17Nov2017D_V11_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V11_DATA/Fall17_17Nov2017D_V11_DATA_L3Absolute_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V11_E_L123_noRes_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017E_V11_DATA/Fall17_17Nov2017E_V11_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V11_DATA/Fall17_17Nov2017E_V11_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V11_DATA/Fall17_17Nov2017E_V11_DATA_L3Absolute_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V11_F_L123_noRes_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V11_DATA/Fall17_17Nov2017F_V11_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V11_DATA/Fall17_17Nov2017F_V11_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V11_DATA/Fall17_17Nov2017F_V11_DATA_L3Absolute_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V11_B_L1RC_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V11_DATA/Fall17_17Nov2017B_V11_DATA_L1RC_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V11_C_L1RC_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V11_DATA/Fall17_17Nov2017C_V11_DATA_L1RC_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V11_D_L1RC_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017D_V11_DATA/Fall17_17Nov2017D_V11_DATA_L1RC_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V11_E_L1RC_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017E_V11_DATA/Fall17_17Nov2017E_V11_DATA_L1RC_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V11_F_L1RC_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V11_DATA/Fall17_17Nov2017F_V11_DATA_L1RC_AK8PFPuppi.txt",
};



//Fall17_17Nov2017_V12
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V12_B_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V12_DATA/Fall17_17Nov2017B_V12_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V12_DATA/Fall17_17Nov2017B_V12_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V12_DATA/Fall17_17Nov2017B_V12_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V12_DATA/Fall17_17Nov2017B_V12_DATA_L2L3Residual_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V12_C_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V12_DATA/Fall17_17Nov2017C_V12_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V12_DATA/Fall17_17Nov2017C_V12_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V12_DATA/Fall17_17Nov2017C_V12_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V12_DATA/Fall17_17Nov2017C_V12_DATA_L2L3Residual_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V12_D_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017D_V12_DATA/Fall17_17Nov2017D_V12_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V12_DATA/Fall17_17Nov2017D_V12_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V12_DATA/Fall17_17Nov2017D_V12_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V12_DATA/Fall17_17Nov2017D_V12_DATA_L2L3Residual_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V12_E_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017E_V12_DATA/Fall17_17Nov2017E_V12_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V12_DATA/Fall17_17Nov2017E_V12_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V12_DATA/Fall17_17Nov2017E_V12_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V12_DATA/Fall17_17Nov2017E_V12_DATA_L2L3Residual_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V12_F_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V12_DATA/Fall17_17Nov2017F_V12_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V12_DATA/Fall17_17Nov2017F_V12_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V12_DATA/Fall17_17Nov2017F_V12_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V12_DATA/Fall17_17Nov2017F_V12_DATA_L2L3Residual_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V12_B_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V12_DATA/Fall17_17Nov2017B_V12_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V12_DATA/Fall17_17Nov2017B_V12_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V12_DATA/Fall17_17Nov2017B_V12_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V12_DATA/Fall17_17Nov2017B_V12_DATA_L2L3Residual_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V12_C_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V12_DATA/Fall17_17Nov2017C_V12_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V12_DATA/Fall17_17Nov2017C_V12_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V12_DATA/Fall17_17Nov2017C_V12_DATA_L3Absolute_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V12_D_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017D_V12_DATA/Fall17_17Nov2017D_V12_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V12_DATA/Fall17_17Nov2017D_V12_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V12_DATA/Fall17_17Nov2017D_V12_DATA_L3Absolute_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V12_E_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017E_V12_DATA/Fall17_17Nov2017E_V12_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V12_DATA/Fall17_17Nov2017E_V12_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V12_DATA/Fall17_17Nov2017E_V12_DATA_L3Absolute_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V12_F_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V12_DATA/Fall17_17Nov2017F_V12_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V12_DATA/Fall17_17Nov2017F_V12_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V12_DATA/Fall17_17Nov2017F_V12_DATA_L3Absolute_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V12_B_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V12_DATA/Fall17_17Nov2017B_V12_DATA_L1RC_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V12_C_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V12_DATA/Fall17_17Nov2017C_V12_DATA_L1RC_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V12_D_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017D_V12_DATA/Fall17_17Nov2017D_V12_DATA_L1RC_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V12_E_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017E_V12_DATA/Fall17_17Nov2017E_V12_DATA_L1RC_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V12_F_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V12_DATA/Fall17_17Nov2017F_V12_DATA_L1RC_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V12_B_L123_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V12_DATA/Fall17_17Nov2017B_V12_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V12_DATA/Fall17_17Nov2017B_V12_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V12_DATA/Fall17_17Nov2017B_V12_DATA_L3Absolute_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V12_DATA/Fall17_17Nov2017B_V12_DATA_L2L3Residual_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V12_C_L123_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V12_DATA/Fall17_17Nov2017C_V12_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V12_DATA/Fall17_17Nov2017C_V12_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V12_DATA/Fall17_17Nov2017C_V12_DATA_L3Absolute_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V12_DATA/Fall17_17Nov2017C_V12_DATA_L2L3Residual_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V12_D_L123_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017D_V12_DATA/Fall17_17Nov2017D_V12_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V12_DATA/Fall17_17Nov2017D_V12_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V12_DATA/Fall17_17Nov2017D_V12_DATA_L3Absolute_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V12_DATA/Fall17_17Nov2017D_V12_DATA_L2L3Residual_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V12_E_L123_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017E_V12_DATA/Fall17_17Nov2017E_V12_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V12_DATA/Fall17_17Nov2017E_V12_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V12_DATA/Fall17_17Nov2017E_V12_DATA_L3Absolute_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V12_DATA/Fall17_17Nov2017E_V12_DATA_L2L3Residual_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V12_F_L123_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V12_DATA/Fall17_17Nov2017F_V12_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V12_DATA/Fall17_17Nov2017F_V12_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V12_DATA/Fall17_17Nov2017F_V12_DATA_L3Absolute_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V12_DATA/Fall17_17Nov2017F_V12_DATA_L2L3Residual_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V12_B_L123_noRes_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V12_DATA/Fall17_17Nov2017B_V12_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V12_DATA/Fall17_17Nov2017B_V12_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V12_DATA/Fall17_17Nov2017B_V12_DATA_L3Absolute_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V12_DATA/Fall17_17Nov2017B_V12_DATA_L2L3Residual_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V12_C_L123_noRes_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V12_DATA/Fall17_17Nov2017C_V12_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V12_DATA/Fall17_17Nov2017C_V12_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V12_DATA/Fall17_17Nov2017C_V12_DATA_L3Absolute_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V12_D_L123_noRes_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017D_V12_DATA/Fall17_17Nov2017D_V12_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V12_DATA/Fall17_17Nov2017D_V12_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V12_DATA/Fall17_17Nov2017D_V12_DATA_L3Absolute_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V12_E_L123_noRes_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017E_V12_DATA/Fall17_17Nov2017E_V12_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V12_DATA/Fall17_17Nov2017E_V12_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V12_DATA/Fall17_17Nov2017E_V12_DATA_L3Absolute_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V12_F_L123_noRes_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V12_DATA/Fall17_17Nov2017F_V12_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V12_DATA/Fall17_17Nov2017F_V12_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V12_DATA/Fall17_17Nov2017F_V12_DATA_L3Absolute_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V12_B_L1RC_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V12_DATA/Fall17_17Nov2017B_V12_DATA_L1RC_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V12_C_L1RC_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V12_DATA/Fall17_17Nov2017C_V12_DATA_L1RC_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V12_D_L1RC_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017D_V12_DATA/Fall17_17Nov2017D_V12_DATA_L1RC_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V12_E_L1RC_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017E_V12_DATA/Fall17_17Nov2017E_V12_DATA_L1RC_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V12_F_L1RC_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V12_DATA/Fall17_17Nov2017F_V12_DATA_L1RC_AK8PFPuppi.txt",
};


//Fall17_17Nov2017_V13
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V13_B_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V13_DATA/Fall17_17Nov2017B_V13_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V13_DATA/Fall17_17Nov2017B_V13_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V13_DATA/Fall17_17Nov2017B_V13_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V13_DATA/Fall17_17Nov2017B_V13_DATA_L2L3Residual_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V13_C_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V13_DATA/Fall17_17Nov2017C_V13_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V13_DATA/Fall17_17Nov2017C_V13_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V13_DATA/Fall17_17Nov2017C_V13_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V13_DATA/Fall17_17Nov2017C_V13_DATA_L2L3Residual_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V13_D_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017D_V13_DATA/Fall17_17Nov2017D_V13_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V13_DATA/Fall17_17Nov2017D_V13_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V13_DATA/Fall17_17Nov2017D_V13_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V13_DATA/Fall17_17Nov2017D_V13_DATA_L2L3Residual_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V13_E_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017E_V13_DATA/Fall17_17Nov2017E_V13_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V13_DATA/Fall17_17Nov2017E_V13_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V13_DATA/Fall17_17Nov2017E_V13_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V13_DATA/Fall17_17Nov2017E_V13_DATA_L2L3Residual_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V13_F_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V13_DATA/Fall17_17Nov2017F_V13_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V13_DATA/Fall17_17Nov2017F_V13_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V13_DATA/Fall17_17Nov2017F_V13_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V13_DATA/Fall17_17Nov2017F_V13_DATA_L2L3Residual_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V13_B_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V13_DATA/Fall17_17Nov2017B_V13_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V13_DATA/Fall17_17Nov2017B_V13_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V13_DATA/Fall17_17Nov2017B_V13_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V13_DATA/Fall17_17Nov2017B_V13_DATA_L2L3Residual_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V13_C_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V13_DATA/Fall17_17Nov2017C_V13_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V13_DATA/Fall17_17Nov2017C_V13_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V13_DATA/Fall17_17Nov2017C_V13_DATA_L3Absolute_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V13_D_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017D_V13_DATA/Fall17_17Nov2017D_V13_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V13_DATA/Fall17_17Nov2017D_V13_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V13_DATA/Fall17_17Nov2017D_V13_DATA_L3Absolute_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V13_E_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017E_V13_DATA/Fall17_17Nov2017E_V13_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V13_DATA/Fall17_17Nov2017E_V13_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V13_DATA/Fall17_17Nov2017E_V13_DATA_L3Absolute_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V13_F_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V13_DATA/Fall17_17Nov2017F_V13_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V13_DATA/Fall17_17Nov2017F_V13_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V13_DATA/Fall17_17Nov2017F_V13_DATA_L3Absolute_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V13_B_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V13_DATA/Fall17_17Nov2017B_V13_DATA_L1RC_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V13_C_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V13_DATA/Fall17_17Nov2017C_V13_DATA_L1RC_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V13_D_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017D_V13_DATA/Fall17_17Nov2017D_V13_DATA_L1RC_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V13_E_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017E_V13_DATA/Fall17_17Nov2017E_V13_DATA_L1RC_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V13_F_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V13_DATA/Fall17_17Nov2017F_V13_DATA_L1RC_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V13_B_L123_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V13_DATA/Fall17_17Nov2017B_V13_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V13_DATA/Fall17_17Nov2017B_V13_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V13_DATA/Fall17_17Nov2017B_V13_DATA_L3Absolute_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V13_DATA/Fall17_17Nov2017B_V13_DATA_L2L3Residual_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V13_C_L123_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V13_DATA/Fall17_17Nov2017C_V13_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V13_DATA/Fall17_17Nov2017C_V13_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V13_DATA/Fall17_17Nov2017C_V13_DATA_L3Absolute_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V13_DATA/Fall17_17Nov2017C_V13_DATA_L2L3Residual_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V13_D_L123_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017D_V13_DATA/Fall17_17Nov2017D_V13_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V13_DATA/Fall17_17Nov2017D_V13_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V13_DATA/Fall17_17Nov2017D_V13_DATA_L3Absolute_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V13_DATA/Fall17_17Nov2017D_V13_DATA_L2L3Residual_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V13_E_L123_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017E_V13_DATA/Fall17_17Nov2017E_V13_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V13_DATA/Fall17_17Nov2017E_V13_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V13_DATA/Fall17_17Nov2017E_V13_DATA_L3Absolute_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V13_DATA/Fall17_17Nov2017E_V13_DATA_L2L3Residual_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V13_F_L123_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V13_DATA/Fall17_17Nov2017F_V13_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V13_DATA/Fall17_17Nov2017F_V13_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V13_DATA/Fall17_17Nov2017F_V13_DATA_L3Absolute_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V13_DATA/Fall17_17Nov2017F_V13_DATA_L2L3Residual_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V13_B_L123_noRes_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V13_DATA/Fall17_17Nov2017B_V13_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V13_DATA/Fall17_17Nov2017B_V13_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V13_DATA/Fall17_17Nov2017B_V13_DATA_L3Absolute_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V13_DATA/Fall17_17Nov2017B_V13_DATA_L2L3Residual_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V13_C_L123_noRes_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V13_DATA/Fall17_17Nov2017C_V13_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V13_DATA/Fall17_17Nov2017C_V13_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V13_DATA/Fall17_17Nov2017C_V13_DATA_L3Absolute_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V13_D_L123_noRes_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017D_V13_DATA/Fall17_17Nov2017D_V13_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V13_DATA/Fall17_17Nov2017D_V13_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017D_V13_DATA/Fall17_17Nov2017D_V13_DATA_L3Absolute_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V13_E_L123_noRes_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017E_V13_DATA/Fall17_17Nov2017E_V13_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V13_DATA/Fall17_17Nov2017E_V13_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017E_V13_DATA/Fall17_17Nov2017E_V13_DATA_L3Absolute_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V13_F_L123_noRes_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V13_DATA/Fall17_17Nov2017F_V13_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V13_DATA/Fall17_17Nov2017F_V13_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V13_DATA/Fall17_17Nov2017F_V13_DATA_L3Absolute_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V13_B_L1RC_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V13_DATA/Fall17_17Nov2017B_V13_DATA_L1RC_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V13_C_L1RC_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V13_DATA/Fall17_17Nov2017C_V13_DATA_L1RC_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V13_D_L1RC_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017D_V13_DATA/Fall17_17Nov2017D_V13_DATA_L1RC_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V13_E_L1RC_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017E_V13_DATA/Fall17_17Nov2017E_V13_DATA_L1RC_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V13_F_L1RC_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V13_DATA/Fall17_17Nov2017F_V13_DATA_L1RC_AK8PFPuppi.txt",
};


//2017-------------------------- 17Nov2017  V32 --> To be used with 2017 data and 94X MC
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_L123_AK4PFchs_MC = {
  "JECDatabase/textFiles/Fall17_17Nov2017_V32_MC/Fall17_17Nov2017_V32_MC_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017_V32_MC/Fall17_17Nov2017_V32_MC_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017_V32_MC/Fall17_17Nov2017_V32_MC_L3Absolute_AK4PFchs.txt",

};


const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_B_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V32_DATA/Fall17_17Nov2017B_V32_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V32_DATA/Fall17_17Nov2017B_V32_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V32_DATA/Fall17_17Nov2017B_V32_DATA_L3Absolute_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_C_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V32_DATA/Fall17_17Nov2017C_V32_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V32_DATA/Fall17_17Nov2017C_V32_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V32_DATA/Fall17_17Nov2017C_V32_DATA_L3Absolute_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_DE_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L3Absolute_AK4PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_F_L123_noRes_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V32_DATA/Fall17_17Nov2017F_V32_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V32_DATA/Fall17_17Nov2017F_V32_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V32_DATA/Fall17_17Nov2017F_V32_DATA_L3Absolute_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_B_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V32_DATA/Fall17_17Nov2017B_V32_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V32_DATA/Fall17_17Nov2017B_V32_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V32_DATA/Fall17_17Nov2017B_V32_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V32_DATA/Fall17_17Nov2017B_V32_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_C_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V32_DATA/Fall17_17Nov2017C_V32_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V32_DATA/Fall17_17Nov2017C_V32_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V32_DATA/Fall17_17Nov2017C_V32_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V32_DATA/Fall17_17Nov2017C_V32_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_DE_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_F_L123_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V32_DATA/Fall17_17Nov2017F_V32_DATA_L1FastJet_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V32_DATA/Fall17_17Nov2017F_V32_DATA_L2Relative_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V32_DATA/Fall17_17Nov2017F_V32_DATA_L3Absolute_AK4PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V32_DATA/Fall17_17Nov2017F_V32_DATA_L2L3Residual_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_L1RC_AK4PFchs_MC = {
  "JECDatabase/textFiles/Fall17_17Nov2017_V32_MC/Fall17_17Nov2017_V32_MC_L1RC_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_B_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V32_DATA/Fall17_17Nov2017B_V32_DATA_L1RC_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_C_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V32_DATA/Fall17_17Nov2017C_V32_DATA_L1RC_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_DE_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L1RC_AK4PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_F_L1RC_AK4PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V32_DATA/Fall17_17Nov2017F_V32_DATA_L1RC_AK4PFchs.txt",

};


const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_L123_AK4PFPuppi_MC = {
  "JECDatabase/textFiles/Fall17_17Nov2017_V32_MC/Fall17_17Nov2017_V32_MC_L1FastJet_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017_V32_MC/Fall17_17Nov2017_V32_MC_L2Relative_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017_V32_MC/Fall17_17Nov2017_V32_MC_L3Absolute_AK4PFPuppi.txt",

};


const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_B_L123_noRes_AK4PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V32_DATA/Fall17_17Nov2017B_V32_DATA_L1FastJet_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V32_DATA/Fall17_17Nov2017B_V32_DATA_L2Relative_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V32_DATA/Fall17_17Nov2017B_V32_DATA_L3Absolute_AK4PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_C_L123_noRes_AK4PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V32_DATA/Fall17_17Nov2017C_V32_DATA_L1FastJet_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V32_DATA/Fall17_17Nov2017C_V32_DATA_L2Relative_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V32_DATA/Fall17_17Nov2017C_V32_DATA_L3Absolute_AK4PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_DE_L123_noRes_AK4PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L1FastJet_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L2Relative_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L3Absolute_AK4PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_F_L123_noRes_AK4PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V32_DATA/Fall17_17Nov2017F_V32_DATA_L1FastJet_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V32_DATA/Fall17_17Nov2017F_V32_DATA_L2Relative_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V32_DATA/Fall17_17Nov2017F_V32_DATA_L3Absolute_AK4PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_B_L123_AK4PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V32_DATA/Fall17_17Nov2017B_V32_DATA_L1FastJet_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V32_DATA/Fall17_17Nov2017B_V32_DATA_L2Relative_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V32_DATA/Fall17_17Nov2017B_V32_DATA_L3Absolute_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V32_DATA/Fall17_17Nov2017B_V32_DATA_L2L3Residual_AK4PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_C_L123_AK4PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V32_DATA/Fall17_17Nov2017C_V32_DATA_L1FastJet_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V32_DATA/Fall17_17Nov2017C_V32_DATA_L2Relative_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V32_DATA/Fall17_17Nov2017C_V32_DATA_L3Absolute_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V32_DATA/Fall17_17Nov2017C_V32_DATA_L2L3Residual_AK4PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_DE_L123_AK4PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L1FastJet_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L2Relative_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L3Absolute_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L2L3Residual_AK4PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_F_L123_AK4PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V32_DATA/Fall17_17Nov2017F_V32_DATA_L1FastJet_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V32_DATA/Fall17_17Nov2017F_V32_DATA_L2Relative_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V32_DATA/Fall17_17Nov2017F_V32_DATA_L3Absolute_AK4PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V32_DATA/Fall17_17Nov2017F_V32_DATA_L2L3Residual_AK4PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_L1RC_AK4PFPuppi_MC = {
  "JECDatabase/textFiles/Fall17_17Nov2017_V32_MC/Fall17_17Nov2017_V32_MC_L1RC_AK4PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_B_L1RC_AK4PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V32_DATA/Fall17_17Nov2017B_V32_DATA_L1RC_AK4PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_C_L1RC_AK4PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V32_DATA/Fall17_17Nov2017C_V32_DATA_L1RC_AK4PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_DE_L1RC_AK4PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L1RC_AK4PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_F_L1RC_AK4PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V32_DATA/Fall17_17Nov2017F_V32_DATA_L1RC_AK4PFPuppi.txt",

};




const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_L123_AK8PFchs_MC = {
  "JECDatabase/textFiles/Fall17_17Nov2017_V32_MC/Fall17_17Nov2017_V32_MC_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017_V32_MC/Fall17_17Nov2017_V32_MC_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017_V32_MC/Fall17_17Nov2017_V32_MC_L3Absolute_AK8PFchs.txt",

};


const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_B_L123_noRes_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V32_DATA/Fall17_17Nov2017B_V32_DATA_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V32_DATA/Fall17_17Nov2017B_V32_DATA_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V32_DATA/Fall17_17Nov2017B_V32_DATA_L3Absolute_AK8PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_C_L123_noRes_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V32_DATA/Fall17_17Nov2017C_V32_DATA_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V32_DATA/Fall17_17Nov2017C_V32_DATA_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V32_DATA/Fall17_17Nov2017C_V32_DATA_L3Absolute_AK8PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_DE_L123_noRes_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L3Absolute_AK8PFchs.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_F_L123_noRes_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V32_DATA/Fall17_17Nov2017F_V32_DATA_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V32_DATA/Fall17_17Nov2017F_V32_DATA_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V32_DATA/Fall17_17Nov2017F_V32_DATA_L3Absolute_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_B_L123_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V32_DATA/Fall17_17Nov2017B_V32_DATA_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V32_DATA/Fall17_17Nov2017B_V32_DATA_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V32_DATA/Fall17_17Nov2017B_V32_DATA_L3Absolute_AK8PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V32_DATA/Fall17_17Nov2017B_V32_DATA_L2L3Residual_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_C_L123_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V32_DATA/Fall17_17Nov2017C_V32_DATA_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V32_DATA/Fall17_17Nov2017C_V32_DATA_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V32_DATA/Fall17_17Nov2017C_V32_DATA_L3Absolute_AK8PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V32_DATA/Fall17_17Nov2017C_V32_DATA_L2L3Residual_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_DE_L123_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L3Absolute_AK8PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L2L3Residual_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_F_L123_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V32_DATA/Fall17_17Nov2017F_V32_DATA_L1FastJet_AK8PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V32_DATA/Fall17_17Nov2017F_V32_DATA_L2Relative_AK8PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V32_DATA/Fall17_17Nov2017F_V32_DATA_L3Absolute_AK8PFchs.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V32_DATA/Fall17_17Nov2017F_V32_DATA_L2L3Residual_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_L1RC_AK8PFchs_MC = {
  "JECDatabase/textFiles/Fall17_17Nov2017_V32_MC/Fall17_17Nov2017_V32_MC_L1RC_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_B_L1RC_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V32_DATA/Fall17_17Nov2017B_V32_DATA_L1RC_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_C_L1RC_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V32_DATA/Fall17_17Nov2017C_V32_DATA_L1RC_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_DE_L1RC_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L1RC_AK8PFchs.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_F_L1RC_AK8PFchs_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V32_DATA/Fall17_17Nov2017F_V32_DATA_L1RC_AK8PFchs.txt",

};


const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_L123_AK8PFPuppi_MC = {
  "JECDatabase/textFiles/Fall17_17Nov2017_V32_MC/Fall17_17Nov2017_V32_MC_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017_V32_MC/Fall17_17Nov2017_V32_MC_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017_V32_MC/Fall17_17Nov2017_V32_MC_L3Absolute_AK8PFPuppi.txt",

};


const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_B_L123_noRes_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V32_DATA/Fall17_17Nov2017B_V32_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V32_DATA/Fall17_17Nov2017B_V32_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V32_DATA/Fall17_17Nov2017B_V32_DATA_L3Absolute_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_C_L123_noRes_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V32_DATA/Fall17_17Nov2017C_V32_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V32_DATA/Fall17_17Nov2017C_V32_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V32_DATA/Fall17_17Nov2017C_V32_DATA_L3Absolute_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_DE_L123_noRes_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L3Absolute_AK8PFPuppi.txt",
};
const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_F_L123_noRes_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V32_DATA/Fall17_17Nov2017F_V32_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V32_DATA/Fall17_17Nov2017F_V32_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V32_DATA/Fall17_17Nov2017F_V32_DATA_L3Absolute_AK8PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_B_L123_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V32_DATA/Fall17_17Nov2017B_V32_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V32_DATA/Fall17_17Nov2017B_V32_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V32_DATA/Fall17_17Nov2017B_V32_DATA_L3Absolute_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017B_V32_DATA/Fall17_17Nov2017B_V32_DATA_L2L3Residual_AK8PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_C_L123_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V32_DATA/Fall17_17Nov2017C_V32_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V32_DATA/Fall17_17Nov2017C_V32_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V32_DATA/Fall17_17Nov2017C_V32_DATA_L3Absolute_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017C_V32_DATA/Fall17_17Nov2017C_V32_DATA_L2L3Residual_AK8PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_DE_L123_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L3Absolute_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L2L3Residual_AK8PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_F_L123_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V32_DATA/Fall17_17Nov2017F_V32_DATA_L1FastJet_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V32_DATA/Fall17_17Nov2017F_V32_DATA_L2Relative_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V32_DATA/Fall17_17Nov2017F_V32_DATA_L3Absolute_AK8PFPuppi.txt",
  "JECDatabase/textFiles/Fall17_17Nov2017F_V32_DATA/Fall17_17Nov2017F_V32_DATA_L2L3Residual_AK8PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_L1RC_AK8PFPuppi_MC = {
  "JECDatabase/textFiles/Fall17_17Nov2017_V32_MC/Fall17_17Nov2017_V32_MC_L1RC_AK8PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_B_L1RC_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017B_V32_DATA/Fall17_17Nov2017B_V32_DATA_L1RC_AK8PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_C_L1RC_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017C_V32_DATA/Fall17_17Nov2017C_V32_DATA_L1RC_AK8PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_DE_L1RC_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017DE_V32_DATA/Fall17_17Nov2017DE_V32_DATA_L1RC_AK8PFPuppi.txt",
};

const std::vector<std::string> JERFiles::Fall17_17Nov2017_V32_F_L1RC_AK8PFPuppi_DATA = {
  "JECDatabase/textFiles/Fall17_17Nov2017F_V32_DATA/Fall17_17Nov2017F_V32_DATA_L1RC_AK8PFPuppi.txt",

};








void correct_jet(FactorizedJetCorrector & corrector, Jet & jet, const Event & event, JetCorrectionUncertainty* jec_unc, int jec_unc_direction){
  auto factor_raw = jet.JEC_factor_raw();
  corrector.setJetPt(jet.pt() * factor_raw);
  corrector.setJetEta(jet.eta());
  corrector.setJetE(jet.energy() * factor_raw);
  corrector.setJetA(jet.jetArea());
  corrector.setRho(event.rho);
  auto correctionfactors = corrector.getSubCorrections();
  auto correctionfactor_L1  = correctionfactors.front();
  auto correctionfactor = correctionfactors.back();

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




  //propagate to MET
  //apply type1 MET correction to RAW MET
  //NB: jet with substracted muon Pt should be used
  void correct_MET(const Event & event, double pt_thresh, double eta_thresh_low, double eta_thresh_high){
    //we start from raw MET
    LorentzVector metv4= event.met->uncorr_v4();
    for(auto & jet : *event.jets){
      //thresholds on the corrected jets: pt > 15, EM fraction < 0.9
      bool to_be_corrected = jet.v4().Pt() > 15.;
      to_be_corrected = to_be_corrected && ( fabs(jet.v4().Eta())<eta_thresh_low || fabs(jet.v4().Eta())>eta_thresh_high || jet.v4().Pt() > pt_thresh );
      to_be_corrected = to_be_corrected && (jet.neutralEmEnergyFraction()+jet.chargedEmEnergyFraction())<0.9;
      if(to_be_corrected){
        auto factor_raw = jet.JEC_factor_raw();
        auto L1factor_raw = jet.JEC_L1factor_raw();

        LorentzVector L1corr =   (L1factor_raw*factor_raw)*jet.v4();            //L1 corrected jets
        LorentzVector L123corr = jet.v4();                                      //L123 corrected jets (L23 in case of puppi)
        metv4 -=  L123corr;

        //slimmed MET is corrected by L1FastJet, for Puppi: L1factor_raw = 1 --> L1corr = raw-jet pT.
        metv4 += L1corr;

      }
    }
    event.met->set_pt(metv4.Pt());
    event.met->set_phi(metv4.Phi());
  }

  //propagate JEC to chsMET
  //Attention: the corrected values stored as standart MET values
  void correct_MET(const Event & event, FactorizedJetCorrector & corrector_L1RC, double pt_thresh, double eta_thresh_low, double eta_thresh_high){

    //we start from raw CHS MET
    LorentzVector metv4= LorentzVector(0,0,0,0);
    metv4.SetPt(hypot(event.met->rawCHS_px(),event.met->rawCHS_py()));
    metv4.SetPhi(TMath::ATan2(event.met->rawCHS_py(),event.met->rawCHS_px()));
    for(auto & jet : *event.jets){
      //thresholds on the corrected jets: pt > 15, EM fraction < 0.9
      bool to_be_corrected = jet.v4().Pt() > 15.;
      to_be_corrected = to_be_corrected && ( fabs(jet.v4().Eta())<eta_thresh_low || fabs(jet.v4().Eta())>eta_thresh_high || jet.v4().Pt() > pt_thresh );
      to_be_corrected = to_be_corrected && (jet.neutralEmEnergyFraction()+jet.chargedEmEnergyFraction())<0.9;
      if(to_be_corrected){
        auto factor_raw = jet.JEC_factor_raw();

        corrector_L1RC.setJetPt(jet.pt() * factor_raw);
        corrector_L1RC.setJetEta(jet.eta());
        corrector_L1RC.setJetE(jet.energy() * factor_raw);
        corrector_L1RC.setJetA(jet.jetArea());
        corrector_L1RC.setRho(event.rho);
        auto correctionfactors_L1RC = corrector_L1RC.getSubCorrections();
        auto correctionfactor_L1RC  = correctionfactors_L1RC.back();

        LorentzVector L1RCcorr = (correctionfactor_L1RC*factor_raw)*jet.v4();   //L1RC corrected jets
        LorentzVector L123corr = jet.v4();                                      //L123 corrected jets (L23 in case of puppi)
        metv4 -=  L123corr;
        metv4 += L1RCcorr;
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
  used_ak4puppi = ctx.get("JetCollection")=="slimmedJetsPuppi" || ctx.get("JetCollection")=="updatedPatJetsSlimmedJetsPuppi";
  metprop_possible_ak8chs = ctx.get("JetCollection")=="patJetsAK8PFCHS";
  metprop_possible_ak8puppi = ctx.get("JetCollection")=="patJetsAK8PFPUPPI" || ctx.get("JetCollection")=="updatedPatJetsPatJetsAK8PFPUPPI";

  //MET is always corrected using the jet collection stated in the "JetCollection" Item in the context and only in case one of the stated jet collections is used.
  //Particularly, only one of these two AK8 collections should be used.
  propagate_to_met = used_ak4chs || used_ak4puppi || metprop_possible_ak8chs || metprop_possible_ak8puppi;
  if(!propagate_to_met) cout << "WARNING in JetCorrections.cxx: You specified a jet collection in the 'JetCollection' item in the config file that is not suited to correct MET. You should change that if you are using MET." << endl;

  //The first two collections are standard MET collections. The third one is only used for derivation of JECs
  used_slimmedmet = ctx.get("METName")=="slimmedMETs";
  used_puppimet = ctx.get("METName")=="slimmedMETsPuppi";


  //if CHS MET is used, the correction is based on the (L123 - L1RC) scheme, else it is based on the standard (L123-L1).
  //See also: https://twiki.cern.ch/twiki/bin/viewauth/CMS/METType1Type2Formulae
  //If using CHS MET and therefore going for (L123 - L1RC),
  //the L1RC corrections have to be provided in a separate const std::vector<std::string>. This must only contain the L1RC correction.
  if(filenames_L1RC.size() == 1)
  corrector_L1RC = build_corrector(filenames_L1RC);
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

  return true;
}

bool JetCorrector::correct_met(uhh2::Event & event, const bool & isCHSmet, double pt_thresh, double eta_thresh_low, double eta_thresh_high){
  assert(event.jets);
  if(!isCHSmet){ //for standart MET collection (most of the case) proceed with standart correction
    //propagate jet corrections to MET
    correct_MET(event, pt_thresh, eta_thresh_low,  eta_thresh_high);
  }
  else{
    correct_MET(event, *corrector_L1RC, pt_thresh, eta_thresh_low,  eta_thresh_high);
  }

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


// https://twiki.cern.ch/twiki/bin/view/CMS/JetResolution [13TeV JER measurement]
// New JER files from Marek (8 Aug 2017)
const JERSmearing::SFtype1 JERSmearing::SF_13TeV_2016_03Feb2017 = {
  // 0 = upper jet-eta limit
  // 1 = JER SF
  // 2 = JER SF + 1sigma
  // 3 = JER SF - 1sigma
  {{0.5, 1.093, 1.201, 0.983}},
  {{0.8, 1.144, 1.259, 1.029}},
  {{1.1, 1.087, 1.202, 0.972}},
  {{1.3, 1.119, 1.234, 1.006}},
  {{1.7, 1.093, 1.218, 0.996}},
  {{1.9, 1.072, 1.199, 0.977}},
  {{2.1, 1.122, 1.266, 1.030}},
  {{2.3, 1.127, 1.305, 0.997}},
  {{2.5, 1.206, 1.370, 1.102}},
  {{2.8, 1.340, 1.485, 1.197}},
  {{3.0, 1.761, 1.957, 1.551}},
  {{3.2, 1.317, 1.452, 1.182}},
  {{5.0, 1.140, 1.270, 1.018}},
};



// https://twiki.cern.ch/twiki/bin/view/CMS/JetResolution [13TeV JER measurement]
// Summer16_25nsV1 to be used with 2017 MC as long as results on 2017 data are not official
const JERSmearing::SFtype1 JERSmearing::SF_13TeV_Summer16_25nsV1 = {
  // 0 = upper jet-eta limit
  // 1 = JER SF
  // 2 = JER SF + 1sigma
  // 3 = JER SF - 1sigma
  {{0.522, 1.1595, 1.2240, 1.0950}},
  {{0.783, 1.1948, 1.2600, 1.1296}},
  {{1.131, 1.1464, 1.2096, 1.0832}},
  {{1.305, 1.1609, 1.2634, 1.0584}},
  {{1.740, 1.1278, 1.2264, 1.0292}},
  {{1.930, 1.1000, 1.2079, 0.9921}},
  {{2.043, 1.1426, 1.2640, 1.0212}},
  {{2.322, 1.1512, 1.2652, 1.0372}},
  {{2.500, 1.2963, 1.5334, 1.0592}},
  {{2.853, 1.3418, 1.5509, 1.1327}},
  {{2.964, 1.7788, 1.9796, 1.5780}},
  {{3.139, 1.1869, 1.3112, 1.0626}},
  {{5.191, 1.1922, 1.3410, 1.0434}},
};


// 2017
const JERSmearing::SFtype1 JERSmearing::SF_13TeV_Fall17 = {
  // 0 = upper jet-eta limit
  // 1 = JER SF
  // 2 = JER SF + 1sigma
  // 3 = JER SF - 1sigma
  {{0.522, 1.05325, 1.0563, 1.0502}},
  {{0.783, 1.12443, 1.13123, 1.11763}},
  {{1.131, 1.07745, 1.08264, 1.07226}},
  {{1.305, 1.07706, 1.08918, 1.06494}},
  {{1.74, 1.07232, 1.07863, 1.06601}},
  {{1.93, 1.07241, 1.09621, 1.04861}},
  {{2.043, 1.08814, 1.14028, 1.036}},
  {{2.322, 1.07464, 1.10508, 1.0442}},
  {{2.5, 1.07105, 1.16178, 0.980317}},
  {{2.853, 1.46856, 1.55712, 1.38}},
  {{2.964, 2.65947, 2.72613, 2.59281}},
  {{3.139, 1.43135, 1.44844, 1.41426}},
  {{5.191, 1.31982, 1.34467, 1.29497}},

};


// 2017 --> To be used with 2017 data and 94X MC
const JERSmearing::SFtype1 JERSmearing::SF_13TeV_Fall17_V3 = {
  // 0 = upper jet-eta limit
  // 1 = JER SF
  // 2 = JER SF + 1sigma
  // 3 = JER SF - 1sigma
  {{0.522, 1.14320, 1.16543, 1.12097}},
  {{0.783, 1.18152, 1.22987, 1.13317}},
  {{1.131, 1.09887, 1.14444, 1.05330}},
  {{1.305, 1.11365, 1.25332, 0.973979}},
  {{1.740, 1.13072, 1.27776, 0.98369}},
  {{1.930, 1.15996, 1.25759, 1.06232}},
  {{2.043, 1.23926, 1.43013, 1.04839}},
  {{2.322, 1.26039, 1.41049, 1.11029}},
  {{2.500, 1.40853, 1.61049, 1.20657}},
  {{2.853, 1.99090, 2.55927, 1.42253}},
  {{2.964, 2.29227, 2.66654, 1.91800}},
  {{3.139, 1.26957, 1.37847, 1.16067}},
  {{5.191, 1.15425, 1.30663, 1.00187}},

};
////

JetResolutionSmearer::JetResolutionSmearer(uhh2::Context & ctx, const JERSmearing::SFtype1& JER_sf){
  m_gjrs = new GenericJetResolutionSmearer(ctx, "jets", "genjets", JER_sf);
}

bool JetResolutionSmearer::process(uhh2::Event & event) {

  m_gjrs->process(event);
  return true;
}

JetResolutionSmearer::~JetResolutionSmearer(){}

////

GenericJetResolutionSmearer::GenericJetResolutionSmearer(uhh2::Context& ctx, const std::string& recjet_label, const std::string& genjet_label, const JERSmearing::SFtype1& JER_sf, const TString ResolutionFileName){

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

  //read in file for jet resolution (taken from https://github.com/cms-jet/JRDatabase/blob/master/textFiles/)
  m_ResolutionFileName = ResolutionFileName;

  TString filename = std::getenv("CMSSW_BASE");
  filename += "/src/UHH2/common/data/";
  filename += ResolutionFileName;

  m_resfile.open(filename);

  //get the formula from the header
  TString dummy;
  TString formula;
  m_resfile >>dummy;
  m_resfile >>dummy;
  m_resfile >>dummy;
  m_resfile >>dummy;
  m_resfile >>dummy;
  m_resfile >>formula;

  res_formula =  new TFormula("res_formula",formula);

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

  if     (rec_jets    && gen_jets){
    apply_JER_smearing(*rec_jets   , *gen_jets   , 0.4, evt.rho);
  }
  else if(rec_topjets && gen_jets){
    apply_JER_smearing(*rec_topjets, *gen_jets   , 0.8, evt.rho);
  }
  else if(rec_topjets && gen_topjets){
    apply_JER_smearing(*rec_topjets, *gen_topjets, 0.8, evt.rho);
  }
  else throw std::runtime_error("GenericJetResolutionSmearer::process -- invalid combination of RECO-GEN jet collections");

  return true;
}

template<typename RJ, typename GJ>
void GenericJetResolutionSmearer::apply_JER_smearing(std::vector<RJ>& rec_jets, const std::vector<GJ>& gen_jets, float radius, float rho){

  for(unsigned int i=0; i<rec_jets.size(); ++i){


    auto& jet = rec_jets.at(i);

    LorentzVector jet_v4 = jet.v4();
    float recopt = jet_v4.pt();
    float abseta = fabs(jet_v4.eta());

    // find next genjet:
    auto closest_genjet = closestParticle(jet, gen_jets);
    float genpt = -1.;

    float resolution = getResolution(jet_v4.eta(), rho, jet_v4.pt())  ;

    // ignore unmatched jets (=no genjets at all or large DeltaR), or jets with very low genjet pt. These jets will be treated with the stochastic method.
    if(!(closest_genjet == nullptr) && uhh2::deltaR(*closest_genjet, jet) < 0.5*radius){
      genpt = closest_genjet->pt();
    }
    if( fabs(genpt-recopt) > 3*resolution*recopt){
      genpt=-1;
    }
    if(genpt < 15.0f) {
      genpt=-1.;
    }


    int ieta(-1);

    for(unsigned int idx=0; idx<JER_SFs_.size(); ++idx){


      const float min_eta = idx ? JER_SFs_.at(idx-1).at(0) : 0.;
      const float max_eta =       JER_SFs_.at(idx)  .at(0);

      if(min_eta <= abseta && abseta < max_eta){ ieta = idx; break; }
    }
    if(ieta < 0) {
      std::cout << "WARNING: JetResolutionSmearer: index for JER-smearing SF not found for jet with |eta| = " << abseta << std::endl;
      std::cout << "         no JER smearing is applied." << std::endl;
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
    float new_pt = -1.;
    //use smearing method in case a matching generator jet was found
    if(genpt>0){
      new_pt = std::max(0.0f, genpt + c * (recopt - genpt));
    }
    //use stochastic method if no generator jet could be matched to the reco jet
    else{
      //initialize random generator with eta-dependend random seed to be reproducible
      TRandom rand((int)(1000*abseta));
      float random_gauss = rand.Gaus(0,resolution);
      new_pt = recopt * (1 + random_gauss*sqrt(std::max( c*c-1,0.0f)));
    }
    jet_v4 *= new_pt / recopt;

    //update JEC_factor_raw needed for smearing MET
    float factor_raw = jet.JEC_factor_raw();
    factor_raw *= recopt/new_pt;

    jet.set_JEC_factor_raw(factor_raw);
    jet.set_v4(jet_v4);

  }

  return;
}

float GenericJetResolutionSmearer::getResolution(float eta, float rho, float pt){
  float resolution = 0.;

  //go to beginning of the file
  m_resfile.clear();
  m_resfile.seekg(0, ios::beg);

  //drop the header from the file
  char header[1000];
  m_resfile.getline( header, 1000);

  float eta_min;
  float eta_max;
  float rho_min;
  float rho_max;
  int N;
  float pt_min;
  float pt_max;
  float par0;
  float par1;
  float par2;
  float par3;

  bool valid=false;

  while(!m_resfile.eof() && !valid){

    m_resfile >> eta_min;
    m_resfile >> eta_max;
    m_resfile >> rho_min;
    m_resfile >> rho_max;
    m_resfile >> N;
    m_resfile >> pt_min;
    m_resfile >> pt_max;
    m_resfile >> par0;
    m_resfile >> par1;
    m_resfile >> par2;
    m_resfile >> par3;

    //find correct bin
    if(eta_min <= eta && eta_max > eta && rho_min <= rho && rho_max > rho && pt_min <= pt && pt_max > pt){
      valid=true;
    }

  }
  if(valid){
    res_formula->SetParameters(par0,par1,par2,par3);
    resolution = res_formula->Eval(pt);
  }

  return resolution;
}


//// -----------------------------------------------------------------
