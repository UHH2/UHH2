#include "UHH2/common/include/JetCorrectionsSets.h"
#include <string>

//see https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookJetEnergyCorrections#GetTxtFiles how to get the txt files with jet energy corrections from the database

/*
█ ██████  ██████  ███████ ██████  ██████   ██████   ██████ ███████ ███████ ███████  ██████  ██████      ██████  ██ ██████  ███████  ██████ ████████ ██ ██    ██ ███████ ███████
█ ██   ██ ██   ██ ██      ██   ██ ██   ██ ██    ██ ██      ██      ██      ██      ██    ██ ██   ██     ██   ██ ██ ██   ██ ██      ██         ██    ██ ██    ██ ██      ██
█ ██████  ██████  █████   ██████  ██████  ██    ██ ██      █████   ███████ ███████ ██    ██ ██████      ██   ██ ██ ██████  █████   ██         ██    ██ ██    ██ █████   ███████
█ ██      ██   ██ ██      ██      ██   ██ ██    ██ ██      ██           ██      ██ ██    ██ ██   ██     ██   ██ ██ ██   ██ ██      ██         ██    ██  ██  ██  ██           ██
█ ██      ██   ██ ███████ ██      ██   ██  ██████   ██████ ███████ ███████ ███████  ██████  ██   ██     ██████  ██ ██   ██ ███████  ██████    ██    ██   ████   ███████ ███████
*/

#define SET_NEWSTRING(tag,ver,jetCollection,sample,run,Correction)                                                                              \
std::string("JECDatabase/textFiles/")+#tag+#run+"_V"+#ver+"_"+#sample+"/"+#tag+#run+"_V"+#ver+"_"+#sample+"_"+#Correction+"_"+#jetCollection+".txt", \

#define SET_JECFILES_MC(tag,ver,jetCollection)					                                    \
const std::vector<std::string> JERFiles::tag##_V##ver##_L123_##jetCollection##_MC = {       \
  SET_NEWSTRING(tag,ver,jetCollection,MC,"",L1FastJet)                                      \
  SET_NEWSTRING(tag,ver,jetCollection,MC,"",L2Relative)                                     \
  SET_NEWSTRING(tag,ver,jetCollection,MC,"",L3Absolute)                                     \
};                                                                                          \
const std::vector<std::string> JERFiles::tag##_V##ver##_L1RC_##jetCollection##_MC = {       \
  SET_NEWSTRING(tag,ver,jetCollection,MC,"",L1RC)                                           \
};                                                                                          \
const std::vector<std::string> JERFiles::tag##_V##ver##_L1FastJet_##jetCollection##_MC = {  \
  SET_NEWSTRING(tag,ver,jetCollection,MC,"",L1FastJet)                                      \
};                                                                                          \

#define SET_CORRECTION_DATA(tag,ver,jetCollection,run)                                                \
const std::vector<std::string> JERFiles::tag##_V##ver##_##run##_L123_##jetCollection##_DATA = {       \
  SET_NEWSTRING(tag,ver,jetCollection,DATA,run,L1FastJet)                                             \
  SET_NEWSTRING(tag,ver,jetCollection,DATA,run,L2Relative)                                            \
  SET_NEWSTRING(tag,ver,jetCollection,DATA,run,L3Absolute)                                            \
  SET_NEWSTRING(tag,ver,jetCollection,DATA,run,L2L3Residual)                                          \
};                                                                                                    \
const std::vector<std::string> JERFiles::tag##_V##ver##_##run##_L123_noRes_##jetCollection##_DATA = { \
  SET_NEWSTRING(tag,ver,jetCollection,DATA,run,L1FastJet)                                             \
  SET_NEWSTRING(tag,ver,jetCollection,DATA,run,L2Relative)                                            \
  SET_NEWSTRING(tag,ver,jetCollection,DATA,run,L3Absolute)                                            \
};                                                                                                    \
const std::vector<std::string> JERFiles::tag##_V##ver##_##run##_L1RC_##jetCollection##_DATA = {       \
  SET_NEWSTRING(tag,ver,jetCollection,DATA,run,L1RC)                                                  \
};                                                                                                    \
const std::vector<std::string> JERFiles::tag##_V##ver##_##run##_L1FastJet_##jetCollection##_DATA = {  \
  SET_NEWSTRING(tag,ver,jetCollection,DATA,run,L1FastJet)                                             \
};                                                                                                    \

#define SET_JECFILES_DATA(tag,ver,jetCollection)  \
SET_CORRECTION_DATA(tag,ver,jetCollection,B)      \
SET_CORRECTION_DATA(tag,ver,jetCollection,C)      \
SET_CORRECTION_DATA(tag,ver,jetCollection,D)      \
SET_CORRECTION_DATA(tag,ver,jetCollection,E)      \
SET_CORRECTION_DATA(tag,ver,jetCollection,F)      \
SET_CORRECTION_DATA(tag,ver,jetCollection,DE)     \

/*
█ ███████ ███████ ████████
█ ██      ██         ██
█ ███████ █████      ██
█      ██ ██         ██
█ ███████ ███████    ██
*/

SET_JECFILES_MC(Summer16_23Sep2016,4,AK4PFchs)
SET_JECFILES_MC(Fall17_17Nov2017,11,AK4PFchs)
SET_JECFILES_MC(Fall17_17Nov2017,24,AK4PFchs)
SET_JECFILES_MC(Fall17_17Nov2017,32,AK4PFchs)

SET_JECFILES_DATA(Fall17_17Nov2017,11,AK4PFchs)
SET_JECFILES_DATA(Fall17_17Nov2017,24,AK4PFchs)
SET_JECFILES_DATA(Fall17_17Nov2017,31,AK4PFchs)
SET_JECFILES_DATA(Fall17_17Nov2017,32,AK4PFchs)
SET_JECFILES_DATA(Fall17_09May2018,3,AK4PFchs)
