#pragma once

#include <vector>
#include <string>

/* /// namespace to define some useful filename constants to be used for jet energy corrections */
//2017
namespace JERFiles{

  /*
  █ ██████  ██████  ███████ ██████  ██████   ██████   ██████ ███████ ███████ ███████  ██████  ██████      ██████  ██ ██████  ███████  ██████ ████████ ██ ██    ██ ███████ ███████
  █ ██   ██ ██   ██ ██      ██   ██ ██   ██ ██    ██ ██      ██      ██      ██      ██    ██ ██   ██     ██   ██ ██ ██   ██ ██      ██         ██    ██ ██    ██ ██      ██
  █ ██████  ██████  █████   ██████  ██████  ██    ██ ██      █████   ███████ ███████ ██    ██ ██████      ██   ██ ██ ██████  █████   ██         ██    ██ ██    ██ █████   ███████
  █ ██      ██   ██ ██      ██      ██   ██ ██    ██ ██      ██           ██      ██ ██    ██ ██   ██     ██   ██ ██ ██   ██ ██      ██         ██    ██  ██  ██  ██           ██
  █ ██      ██   ██ ███████ ██      ██   ██  ██████   ██████ ███████ ███████ ███████  ██████  ██   ██     ██████  ██ ██   ██ ███████  ██████    ██    ██   ████   ███████ ███████
  */

  #define DEFINE_CORRECTION_MC(tag,ver,jetCollection,Correction)                            \
  extern const std::vector<std::string> tag##_V##ver##_##Correction##_##jetCollection##_MC; \

  #define DEFINE_CORRECTION_DATA(tag,ver,jetCollection,Correction)                                \
  extern const std::vector<std::string> tag##_V##ver##_B_##Correction##_##jetCollection##_DATA;   \
  extern const std::vector<std::string> tag##_V##ver##_C_##Correction##_##jetCollection##_DATA;   \
  extern const std::vector<std::string> tag##_V##ver##_D_##Correction##_##jetCollection##_DATA;   \
  extern const std::vector<std::string> tag##_V##ver##_E_##Correction##_##jetCollection##_DATA;   \
  extern const std::vector<std::string> tag##_V##ver##_F_##Correction##_##jetCollection##_DATA;   \
  extern const std::vector<std::string> tag##_V##ver##_DE_##Correction##_##jetCollection##_DATA;  \

  #define DEFINE_JECFILES_MC(tag,ver,jetCollection)     \
  DEFINE_CORRECTION_MC(tag,ver,jetCollection,L123)      \
  DEFINE_CORRECTION_MC(tag,ver,jetCollection,L1RC)      \
  DEFINE_CORRECTION_MC(tag,ver,jetCollection,L1FastJet) \

  #define DEFINE_JECFILES_DATA(tag,ver,jetCollection)       \
  DEFINE_CORRECTION_DATA(tag,ver,jetCollection,L123)        \
  DEFINE_CORRECTION_DATA(tag,ver,jetCollection,L1RC)        \
  DEFINE_CORRECTION_DATA(tag,ver,jetCollection,L1FastJet)   \
  DEFINE_CORRECTION_DATA(tag,ver,jetCollection,L123_noRes)  \

  /*
  █ ██████  ███████ ███████ ██ ███    ██ ███████
  █ ██   ██ ██      ██      ██ ████   ██ ██
  █ ██   ██ █████   █████   ██ ██ ██  ██ █████
  █ ██   ██ ██      ██      ██ ██  ██ ██ ██
  █ ██████  ███████ ██      ██ ██   ████ ███████
  */

  DEFINE_JECFILES_MC(Summer16_23Sep2016,4,AK4PFchs)
  DEFINE_JECFILES_MC(Fall17_17Nov2017,11,AK4PFchs)
  DEFINE_JECFILES_MC(Fall17_17Nov2017,24,AK4PFchs)
  DEFINE_JECFILES_MC(Fall17_17Nov2017,32,AK4PFchs)

  DEFINE_JECFILES_DATA(Fall17_17Nov2017,11,AK4PFchs)
  DEFINE_JECFILES_DATA(Fall17_17Nov2017,24,AK4PFchs)
  DEFINE_JECFILES_DATA(Fall17_17Nov2017,31,AK4PFchs)
  DEFINE_JECFILES_DATA(Fall17_17Nov2017,32,AK4PFchs)
  DEFINE_JECFILES_DATA(Fall17_09May2018,3,AK4PFchs)

}
