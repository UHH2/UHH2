#include "UHH2/common/include/JetCorrectionSets.h"
#include <string>

//see https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookJetEnergyCorrections#GetTxtFiles how to get the txt files with jet energy corrections from the database

/* The idea of the following preprocessor directives is to simplify the creation of new JEC input files.
More details in UHH2/common/include/JetCorrectionsSets.h
*/

#define SET_NEWSTRING_DATA(tag,ver,jetCollection,sample,runName,Correction)                                                                                   \
"JECDatabase/textFiles/" #tag #runName "_V" #ver "_" #sample "/" #tag #runName "_V" #ver "_" #sample "_" #Correction "_" #jetCollection ".txt",  \

#define SET_NEWSTRING_DATA_2018(tag,ver,jetCollection,sample,runName,Correction)                                                                                            \
"JECDatabase/textFiles/" #tag "_Run" #runName "_V" #ver "_" #sample "/" #tag "_Run" #runName "_V" #ver "_" #sample "_" #Correction "_" #jetCollection ".txt",  \

// have to use these, cannot use #tag, etc as it is inside another directive and will fail to compile
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)


const std::string JERFiles::JECPathStringMC(const std::string & tag, const std::string & ver, const std::string & jetCollection, const std::string & Correction) {
  return "JECDatabase/textFiles/"+tag+"_V"+ver+"_MC/"+tag+"_V"+ver+"_MC_"+Correction+"_"+jetCollection+".txt";
}

const std::vector<std::string> JERFiles::JECFiles_MC(const std::string & tag, const std::string & ver, const std::string & jetCollection) {
  return {
    JERFiles::JECPathStringMC(tag, ver, jetCollection, "L1FastJet"),
    JERFiles::JECPathStringMC(tag, ver, jetCollection, "L2Relative"),
    JERFiles::JECPathStringMC(tag, ver, jetCollection, "L3Absolute")
  };
}

#define SET_JECFILES_MC(tag,ver,jetCollection)					                                          \
const std::vector<std::string> JERFiles::tag##_V##ver##_L123_##jetCollection##_MC =               \
  JERFiles::JECFiles_MC(TOSTRING(tag), TOSTRING(ver), TOSTRING(jetCollection));                   \
const std::vector<std::string> JERFiles::tag##_V##ver##_L1RC_##jetCollection##_MC = {             \
  JERFiles::JECPathStringMC(TOSTRING(tag), TOSTRING(ver), TOSTRING(jetCollection), "L1RC")        \
};                                                                                                \
const std::vector<std::string> JERFiles::tag##_V##ver##_L1FastJet_##jetCollection##_MC = {        \
  JERFiles::JECPathStringMC(TOSTRING(tag), TOSTRING(ver), TOSTRING(jetCollection), "L1FastJet")   \
};                                                                                                \

#define SET_CORRECTION_DATA(tag,ver,jetCollection,runName,runCorrection)                                  \
const std::vector<std::string> JERFiles::tag##_V##ver##_##runName##_L123_##jetCollection##_DATA = {       \
  SET_NEWSTRING_DATA(tag,ver,jetCollection,DATA,runCorrection,L1FastJet)                                  \
  SET_NEWSTRING_DATA(tag,ver,jetCollection,DATA,runCorrection,L2Relative)                                 \
  SET_NEWSTRING_DATA(tag,ver,jetCollection,DATA,runCorrection,L3Absolute)                                 \
  SET_NEWSTRING_DATA(tag,ver,jetCollection,DATA,runCorrection,L2L3Residual)                               \
};                                                                                                        \
const std::vector<std::string> JERFiles::tag##_V##ver##_##runName##_L123_noRes_##jetCollection##_DATA = { \
  SET_NEWSTRING_DATA(tag,ver,jetCollection,DATA,runCorrection,L1FastJet)                                  \
  SET_NEWSTRING_DATA(tag,ver,jetCollection,DATA,runCorrection,L2Relative)                                 \
  SET_NEWSTRING_DATA(tag,ver,jetCollection,DATA,runCorrection,L3Absolute)                                 \
};                                                                                                        \
const std::vector<std::string> JERFiles::tag##_V##ver##_##runName##_L1RC_##jetCollection##_DATA = {       \
  SET_NEWSTRING_DATA(tag,ver,jetCollection,DATA,runCorrection,L1RC)                                       \
};                                                                                                        \
const std::vector<std::string> JERFiles::tag##_V##ver##_##runName##_L1FastJet_##jetCollection##_DATA = {  \
  SET_NEWSTRING_DATA(tag,ver,jetCollection,DATA,runCorrection,L1FastJet)                                  \
};                                                                                                        \


#define SET_CORRECTION_DATA_2018(tag,ver,jetCollection,runName,runCorrection)                             \
const std::vector<std::string> JERFiles::tag##_V##ver##_##runName##_L123_##jetCollection##_DATA = {       \
  SET_NEWSTRING_DATA_2018(tag,ver,jetCollection,DATA,runCorrection,L1FastJet)                             \
  SET_NEWSTRING_DATA_2018(tag,ver,jetCollection,DATA,runCorrection,L2Relative)                            \
  SET_NEWSTRING_DATA_2018(tag,ver,jetCollection,DATA,runCorrection,L3Absolute)                            \
  SET_NEWSTRING_DATA_2018(tag,ver,jetCollection,DATA,runCorrection,L2L3Residual)                          \
};                                                                                                        \
const std::vector<std::string> JERFiles::tag##_V##ver##_##runName##_L123_noRes_##jetCollection##_DATA = { \
  SET_NEWSTRING_DATA_2018(tag,ver,jetCollection,DATA,runCorrection,L1FastJet)                             \
  SET_NEWSTRING_DATA_2018(tag,ver,jetCollection,DATA,runCorrection,L2Relative)                            \
  SET_NEWSTRING_DATA_2018(tag,ver,jetCollection,DATA,runCorrection,L3Absolute)                            \
};                                                                                                        \
const std::vector<std::string> JERFiles::tag##_V##ver##_##runName##_L1RC_##jetCollection##_DATA = {       \
  SET_NEWSTRING_DATA_2018(tag,ver,jetCollection,DATA,runCorrection,L1RC)                                       \
};                                                                                                        \
const std::vector<std::string> JERFiles::tag##_V##ver##_##runName##_L1FastJet_##jetCollection##_DATA = {  \
  SET_NEWSTRING_DATA_2018(tag,ver,jetCollection,DATA,runCorrection,L1FastJet)                                  \
};                                                                                                        \

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
SET_CORRECTION_DATA_2018(tag,ver,jetCollection,A,A)   \
SET_CORRECTION_DATA_2018(tag,ver,jetCollection,B,B)   \
SET_CORRECTION_DATA_2018(tag,ver,jetCollection,C,C)   \
SET_CORRECTION_DATA_2018(tag,ver,jetCollection,D,D)   \


const std::string JERFiles::JECPathStringMC2016(const std::string & tag, const std::string & ver, const std::string & jetCollection, const std::string & Correction) {
  return "JECDatabase/textFiles/"+tag+"V"+ver+"_MC/"+tag+"V"+ver+"_MC_"+Correction+"_"+jetCollection+".txt";
}

const std::vector<std::string> JERFiles::JECFiles_MC2016(const std::string & tag, const std::string & ver, const std::string & jetCollection) {
  return {
    JERFiles::JECPathStringMC2016(tag, ver, jetCollection, "L1FastJet"),
    JERFiles::JECPathStringMC2016(tag, ver, jetCollection, "L2Relative"),
    JERFiles::JECPathStringMC2016(tag, ver, jetCollection, "L3Absolute")
  };
}

#define SET_JECFILES_MC2016(tag,ver,jetCollection)					                                        \
const std::vector<std::string> JERFiles::tag##_V##ver##_L123_##jetCollection##_MC =                 \
  JERFiles::JECFiles_MC2016(TOSTRING(tag), TOSTRING(ver), TOSTRING(jetCollection));                 \
const std::vector<std::string> JERFiles::tag##_V##ver##_L1RC_##jetCollection##_MC = {               \
  JERFiles::JECPathStringMC2016(TOSTRING(tag), TOSTRING(ver), TOSTRING(jetCollection), "L1RC")      \
};                                                                                                  \
const std::vector<std::string> JERFiles::tag##_V##ver##_L1FastJet_##jetCollection##_MC = {          \
  JERFiles::JECPathStringMC2016(TOSTRING(tag), TOSTRING(ver), TOSTRING(jetCollection), "L1FastJet") \
};                                                                                                  \

/* Here we create the new vectors. The usage is the following:
SET_JECFILES_*( a tag to identify which JEC use ,version, jet collection used)
*/

//SET_JECFILES_MC2016(Summer16_23Sep2016,4,AK4PFchs)

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

