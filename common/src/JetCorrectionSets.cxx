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

#define SET_NEWSTRING_MC(tag,ver,jetCollection,sample,Correction)                                                                           \
"JECDatabase/textFiles/" #tag "_V" #ver "_" #sample "/" #tag "_V" #ver "_" #sample "_" #Correction "_" #jetCollection ".txt",  \

#define SET_JECFILES_MC(tag,ver,jetCollection)					                                    \
const std::vector<std::string> JERFiles::tag##_V##ver##_L123_##jetCollection##_MC = {       \
  SET_NEWSTRING_MC(tag,ver,jetCollection,MC,L1FastJet)                                      \
  SET_NEWSTRING_MC(tag,ver,jetCollection,MC,L2Relative)                                     \
  SET_NEWSTRING_MC(tag,ver,jetCollection,MC,L3Absolute)                                     \
};                                                                                          \
const std::vector<std::string> JERFiles::tag##_V##ver##_L1RC_##jetCollection##_MC = {       \
  SET_NEWSTRING_MC(tag,ver,jetCollection,MC,L1RC)                                           \
};                                                                                          \
const std::vector<std::string> JERFiles::tag##_V##ver##_L1FastJet_##jetCollection##_MC = {  \
  SET_NEWSTRING_MC(tag,ver,jetCollection,MC,L1FastJet)                                      \
};                                                                                          \

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




#define SET_NEWSTRING_MC2016(tag,ver,jetCollection,sample,Correction)                                                                     \
"JECDatabase/textFiles/" #tag "V" #ver "_" #sample "/" #tag "V" #ver "_" #sample "_" #Correction "_" #jetCollection ".txt",  \

#define SET_JECFILES_MC2016(tag,ver,jetCollection)					                                \
const std::vector<std::string> JERFiles::tag##_V##ver##_L123_##jetCollection##_MC = {       \
  SET_NEWSTRING_MC2016(tag,ver,jetCollection,MC,L1FastJet)                                  \
  SET_NEWSTRING_MC2016(tag,ver,jetCollection,MC,L2Relative)                                 \
  SET_NEWSTRING_MC2016(tag,ver,jetCollection,MC,L3Absolute)                                 \
};                                                                                          \
const std::vector<std::string> JERFiles::tag##_V##ver##_L1RC_##jetCollection##_MC = {       \
  SET_NEWSTRING_MC2016(tag,ver,jetCollection,MC,L1RC)                                       \
};                                                                                          \
const std::vector<std::string> JERFiles::tag##_V##ver##_L1FastJet_##jetCollection##_MC = {  \
  SET_NEWSTRING_MC2016(tag,ver,jetCollection,MC,L1FastJet)                                  \
};                                                                                          \

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

SET_JECFILES_MC(Autumn18,4,AK4PFchs)
SET_JECFILES_MC(Autumn18,4,AK4PFPuppi)
SET_JECFILES_MC(Autumn18,4,AK8PFchs)
SET_JECFILES_MC(Autumn18,4,AK8PFPuppi)

SET_JECFILES_DATA_2018(Autumn18,4,AK4PFchs)
SET_JECFILES_DATA_2018(Autumn18,4,AK4PFPuppi)
SET_JECFILES_DATA_2018(Autumn18,4,AK8PFchs)
SET_JECFILES_DATA_2018(Autumn18,4,AK8PFPuppi)
