#include "UHH2/common/include/JetCorrectionSets.h"
#include <string>

//see https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookJetEnergyCorrections#GetTxtFiles how to get the txt files with jet energy corrections from the database

// The idea of the following methods is to simplify the creation of new JEC input files.
const std::string JERFiles::JECPathStringMC(const std::string & tag,
                                            const std::string & ver,
                                            const std::string & jetCollection,
                                            const std::string & correction) {
  std::string newVer = (tag.find("Summer16_23Sep2016") != std::string::npos) ? "V" : "_V"; // because someone decided to remove the underscore in Summer16_23Sep2016
  newVer += ver;

  std::string result = "JECDatabase/textFiles/";
  result += tag;
  result += newVer;
  result += "_MC/";
  result += tag;
  result += newVer;
  result += "_MC_";
  result += correction;
  result += "_";
  result += jetCollection;
  result += ".txt";
  return result;
}

// translate individual runs into the merged ones
// e.g. B -> BCD for Summer16
const std::map<std::string, std::map<std::string, std::string> > JERFiles::jecRunMap = {
  {"Summer16", {
    {"B", "BCD"},
    {"C", "BCD"},
    {"D", "BCD"},
    {"BCD", "BCD"}, // include the combined version as well incase
    {"E", "EF"},
    {"F", "EF"},
    {"EF", "EF"},
    {"G", "GH"},
    {"H", "GH"},
    {"GH", "GH"}
  }},
  {"Fall17", {
    {"B", "B"},
    {"C", "C"},
    {"D", "DE"},
    {"E", "DE"},
    {"DE", "DE"},
    {"F", "F"}
  }},
  {"Autumn18", {
    {"A", "A"},
    {"B", "B"},
    {"C", "C"},
    {"D", "D"}
  }}
};

const std::string JERFiles::JECPathStringDATA(const std::string & tag,
                                              const std::string & ver,
                                              const std::string & jetCollection,
                                              const std::string & runName,
                                              const std::string & correction) {
  std::string newVer = (tag.find("Summer16_23Sep2016") != std::string::npos) ? "V" : "_V"; // because someone decided to remove the underscore in Summer16_23Sep2016
  newVer += ver;

  std::string campaign = tag.substr(0, tag.find("_"));
  std::string newRunName = JERFiles::jecRunMap.at(campaign).at(runName);
  // in 2018 they use "_RunA" instead of just "A"
  if (tag.find("18") != std::string::npos) {
    newRunName = "_Run" + runName;
  }

  std::string result = "JECDatabase/textFiles/";
  result += tag;
  result += newRunName;
  result += newVer;
  result += "_DATA/";
  result += tag;
  result += newRunName;
  result += newVer;
  result += "_DATA_";
  result += correction;
  result += "_";
  result += jetCollection;
  result += ".txt";
  return result;
}

const std::vector<std::string> JERFiles::L1L2L3 = {"L1FastJet", "L2Relative", "L3Absolute"};

const std::vector<std::string> JERFiles::L1L2L3Residual = {"L1FastJet", "L2Relative", "L3Absolute", "L2L3Residual"};

const std::vector<std::string> JERFiles::JECFilesMC(const std::string & tag,
                                                    const std::string & ver,
                                                    const std::string & jetCollection,
                                                    const std::vector<std::string> levels) {
  std::vector<std::string> result;
  for (const auto & level : levels){
    result.push_back(JERFiles::JECPathStringMC(tag, ver, jetCollection, level));
  }
  return result;
}

const std::vector<std::string> JERFiles::JECFilesDATA(const std::string & tag,
                                                      const std::string & ver,
                                                      const std::string & jetCollection,
                                                      const std::string & runName,
                                                      const std::vector<std::string> levels) {
  std::vector<std::string> result;
  for (const auto & level : levels){
    result.push_back(JERFiles::JECPathStringDATA(tag, ver, jetCollection, runName, level));
  }
  return result;
}


// have to use these, cannot use #tag, etc as it is inside another directive and will fail to compile
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define SET_JECFILES_MC(tag,ver,jetCollection)					                                          \
const std::vector<std::string> JERFiles::tag##_V##ver##_L123_##jetCollection##_MC =               \
  JERFiles::JECFilesMC(TOSTRING(tag), TOSTRING(ver), TOSTRING(jetCollection), JERFiles::L1L2L3);  \
const std::vector<std::string> JERFiles::tag##_V##ver##_L1RC_##jetCollection##_MC =               \
  JERFiles::JECFilesMC(TOSTRING(tag), TOSTRING(ver), TOSTRING(jetCollection), {"L1RC"});          \
const std::vector<std::string> JERFiles::tag##_V##ver##_L1FastJet_##jetCollection##_MC =          \
  JERFiles::JECFilesMC(TOSTRING(tag), TOSTRING(ver), TOSTRING(jetCollection), {"L1FastJet"});     \

#define SET_CORRECTION_DATA(tag,ver,jetCollection,runName,runCorrection)                                  \
const std::vector<std::string> JERFiles::tag##_V##ver##_##runName##_L123_##jetCollection##_DATA =         \
  JERFiles::JECFilesDATA(TOSTRING(tag), TOSTRING(ver), TOSTRING(jetCollection), TOSTRING(runCorrection), JERFiles::L1L2L3Residual); \
const std::vector<std::string> JERFiles::tag##_V##ver##_##runName##_L123_noRes_##jetCollection##_DATA =   \
  JERFiles::JECFilesDATA(TOSTRING(tag), TOSTRING(ver), TOSTRING(jetCollection), TOSTRING(runCorrection), JERFiles::L1L2L3); \
const std::vector<std::string> JERFiles::tag##_V##ver##_##runName##_L1RC_##jetCollection##_DATA =         \
  JERFiles::JECFilesDATA(TOSTRING(tag), TOSTRING(ver), TOSTRING(jetCollection), TOSTRING(runCorrection), {"L1RC"}); \
const std::vector<std::string> JERFiles::tag##_V##ver##_##runName##_L1FastJet_##jetCollection##_DATA =    \
  JERFiles::JECFilesDATA(TOSTRING(tag), TOSTRING(ver), TOSTRING(jetCollection), TOSTRING(runCorrection), {"L1FastJet"}); \


#define SET_JECFILES_DATA_2016(tag,ver,jetCollection) \
SET_CORRECTION_DATA(tag,ver,jetCollection,B,BCD)      \
SET_CORRECTION_DATA(tag,ver,jetCollection,C,BCD)      \
SET_CORRECTION_DATA(tag,ver,jetCollection,D,BCD)      \
SET_CORRECTION_DATA(tag,ver,jetCollection,E,EF)       \
SET_CORRECTION_DATA(tag,ver,jetCollection,F,EF)       \
SET_CORRECTION_DATA(tag,ver,jetCollection,G,GH)       \
SET_CORRECTION_DATA(tag,ver,jetCollection,H,GH)       \


#define SET_JECFILES_DATA_2017(tag,ver,jetCollection) \
SET_CORRECTION_DATA(tag,ver,jetCollection,B,B)        \
SET_CORRECTION_DATA(tag,ver,jetCollection,C,C)        \
SET_CORRECTION_DATA(tag,ver,jetCollection,D,DE)       \
SET_CORRECTION_DATA(tag,ver,jetCollection,E,DE)       \
SET_CORRECTION_DATA(tag,ver,jetCollection,F,F)        \


#define SET_JECFILES_DATA_2018(tag,ver,jetCollection) \
SET_CORRECTION_DATA(tag,ver,jetCollection,A,A)   \
SET_CORRECTION_DATA(tag,ver,jetCollection,B,B)   \
SET_CORRECTION_DATA(tag,ver,jetCollection,C,C)   \
SET_CORRECTION_DATA(tag,ver,jetCollection,D,D)   \


/* Here we create the new vectors. The usage is the following:
SET_JECFILES_*( a tag to identify which JEC use ,version, jet collection used)
*/
SET_JECFILES_MC(Summer16_07Aug2017,11,AK4PFchs)
SET_JECFILES_MC(Summer16_07Aug2017,11,AK4PFPuppi)
SET_JECFILES_MC(Summer16_07Aug2017,11,AK8PFchs)
SET_JECFILES_MC(Summer16_07Aug2017,11,AK8PFPuppi)
SET_JECFILES_MC(Summer16_07Aug2017,20,AK4PFchs)

SET_JECFILES_DATA_2016(Summer16_07Aug2017,11,AK4PFchs)
SET_JECFILES_DATA_2016(Summer16_07Aug2017,11,AK4PFPuppi)
SET_JECFILES_DATA_2016(Summer16_07Aug2017,11,AK8PFchs)
SET_JECFILES_DATA_2016(Summer16_07Aug2017,11,AK8PFPuppi)
SET_JECFILES_DATA_2016(Summer16_07Aug2017,20,AK4PFchs)

SET_JECFILES_MC(Fall17_17Nov2017,32,AK4PFchs)
SET_JECFILES_MC(Fall17_17Nov2017,32,AK4PFPuppi)
SET_JECFILES_MC(Fall17_17Nov2017,32,AK8PFchs)
SET_JECFILES_MC(Fall17_17Nov2017,32,AK8PFPuppi)
SET_JECFILES_DATA_2017(Fall17_17Nov2017,32,AK4PFchs)
SET_JECFILES_DATA_2017(Fall17_17Nov2017,32,AK4PFPuppi)
SET_JECFILES_DATA_2017(Fall17_17Nov2017,32,AK8PFchs)
SET_JECFILES_DATA_2017(Fall17_17Nov2017,32,AK8PFPuppi)
//SET_JECFILES_DATA_2017(Fall17_09May2018,3,AK4PFchs)

//2018, update after Moriond 19
SET_JECFILES_MC(Autumn18,19,AK4PFchs)
SET_JECFILES_MC(Autumn18,19,AK4PFPuppi)
SET_JECFILES_MC(Autumn18,19,AK8PFchs)
SET_JECFILES_MC(Autumn18,19,AK8PFPuppi)

SET_JECFILES_DATA_2018(Autumn18,19,AK4PFchs)
SET_JECFILES_DATA_2018(Autumn18,19,AK4PFPuppi)
SET_JECFILES_DATA_2018(Autumn18,19,AK8PFchs)
SET_JECFILES_DATA_2018(Autumn18,19,AK8PFPuppi)

//2018, Moriond19
SET_JECFILES_MC(Autumn18,8,AK4PFchs)
SET_JECFILES_MC(Autumn18,8,AK4PFPuppi)
SET_JECFILES_MC(Autumn18,8,AK8PFchs)
SET_JECFILES_MC(Autumn18,8,AK8PFPuppi)

SET_JECFILES_DATA_2018(Autumn18,8,AK4PFchs)
SET_JECFILES_DATA_2018(Autumn18,8,AK4PFPuppi)
SET_JECFILES_DATA_2018(Autumn18,8,AK8PFchs)
SET_JECFILES_DATA_2018(Autumn18,8,AK8PFPuppi)


SET_JECFILES_MC(Autumn18,4,AK4PFchs)
SET_JECFILES_MC(Autumn18,4,AK4PFPuppi)
SET_JECFILES_MC(Autumn18,4,AK8PFchs)
SET_JECFILES_MC(Autumn18,4,AK8PFPuppi)

SET_JECFILES_DATA_2018(Autumn18,4,AK4PFchs)
SET_JECFILES_DATA_2018(Autumn18,4,AK4PFPuppi)
SET_JECFILES_DATA_2018(Autumn18,4,AK8PFchs)
SET_JECFILES_DATA_2018(Autumn18,4,AK8PFPuppi)

SET_JECFILES_MC(Autumn18,7,AK4PFchs)
SET_JECFILES_MC(Autumn18,7,AK4PFPuppi)
SET_JECFILES_MC(Autumn18,7,AK8PFchs)
SET_JECFILES_MC(Autumn18,7,AK8PFPuppi)

SET_JECFILES_DATA_2018(Autumn18,7,AK4PFchs)
SET_JECFILES_DATA_2018(Autumn18,7,AK4PFPuppi)
SET_JECFILES_DATA_2018(Autumn18,7,AK8PFchs)
SET_JECFILES_DATA_2018(Autumn18,7,AK8PFPuppi)

SET_JECFILES_MC(Autumn18,10,AK4PFchs)
SET_JECFILES_MC(Autumn18,10,AK4PFPuppi)
SET_JECFILES_MC(Autumn18,10,AK8PFchs)
SET_JECFILES_MC(Autumn18,10,AK8PFPuppi)

SET_JECFILES_DATA_2018(Autumn18,10,AK4PFchs)
SET_JECFILES_DATA_2018(Autumn18,10,AK4PFPuppi)
SET_JECFILES_DATA_2018(Autumn18,10,AK8PFchs)
SET_JECFILES_DATA_2018(Autumn18,10,AK8PFPuppi)

SET_JECFILES_MC(Autumn18,13h,AK4PFchs)
SET_JECFILES_MC(Autumn18,13h,AK4PFPuppi)
SET_JECFILES_MC(Autumn18,13h,AK8PFchs)
SET_JECFILES_MC(Autumn18,13h,AK8PFPuppi)

SET_JECFILES_DATA_2018(Autumn18,13h,AK4PFchs)
SET_JECFILES_DATA_2018(Autumn18,13h,AK4PFPuppi)
SET_JECFILES_DATA_2018(Autumn18,13h,AK8PFchs)
SET_JECFILES_DATA_2018(Autumn18,13h,AK8PFPuppi)

