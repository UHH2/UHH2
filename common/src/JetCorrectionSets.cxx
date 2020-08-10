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

// translate individual runs into the merged ones (as used in the txt filenames)
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
  }},
  {"Summer19UL16", {
    {"B", "B"},
    {"C", "C"},
    {"D", "D"},
    {"E", "E"},
    {"F", "F"}
  }},
  {"Summer19UL17", {
    {"B", "B"},
    {"C", "C"},
    {"D", "D"},
    {"E", "E"},
    {"F", "F"}
  }},
  {"Summer19UL18", {
    {"B", "B"},
    {"C", "C"},
    {"D", "D"},
    {"E", "E"},
    {"F", "F"}
  }},
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
  // in 2018, and for UL,  they use "_RunX" instead of just "X"
  if (tag.find("18") != std::string::npos || tag.find("UL") != std::string::npos) {
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

const std::vector<std::string> JERFiles::L1L2 = {"L1FastJet", "L2Relative"};

const std::vector<std::string> JERFiles::L1L2Residual = {"L1FastJet", "L2Relative", "L2Residual"};

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
