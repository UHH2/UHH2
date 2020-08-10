#pragma once

#include <vector>
#include <string>
#include <map>

/* namespace to define useful filename constants & functions to be used for jet energy corrections */

namespace JERFiles{

  /**
   * There are 2 ways to get access to JEC sets, needed for *JetEnergyCorrector class:
   * - use the JECFiles[DATA|MC]() functions
   * - a C++ variable, defined below with the DEFINE_* macros,
   *   and created in the cxx file with the SET_* macros
   * The 1st option is preferred in user analysis code, as it is more flexible
   * (e.g. one can use variables to change all JEC versions
   *  for different jet collections simultaneously)
   * The 2nd option just uses the output of the 1st option, and is the reason for
   * all the preprocessor commands. These are kept only for backwards-compatibility.
   */

  // JECPathString returns a single JEC filepath for given tag, ver, etc
  const std::string JECPathStringMC(const std::string & tag,
                                    const std::string & ver,
                                    const std::string & jetCollection,
                                    const std::string & correction);

  extern const std::map<std::string, std::map<std::string, std::string> > jecRunMap;


  const std::string JECPathStringDATA(const std::string & tag,
                                      const std::string & ver,
                                      const std::string & jetCollection,
                                      // runName can accept individual run (B) or combined (BCD)
                                      // - it will auto handle combined run periods
                                      const std::string & runName,
                                      const std::string & correction);

  // Some common variable to replace long vector strings
  extern const std::vector<std::string> L1L2;
  extern const std::vector<std::string> L1L2Residual;
  extern const std::vector<std::string> L1L2L3;
  extern const std::vector<std::string> L1L2L3Residual;

  // These are the methods returning the total list of JEC files for a set of levels
  // This can be passed to e.g. `JetCorrector` class
  const std::vector<std::string> JECFilesMC(const std::string & tag,
                                            const std::string & ver,
                                            const std::string & jetCollection,
                                            const std::vector<std::string> levels=JERFiles::L1L2L3);

  const std::vector<std::string> JECFilesDATA(const std::string & tag,
                                              const std::string & ver,
                                              const std::string & jetCollection,
                                              const std::string & runName,
                                              const std::vector<std::string> levels=JERFiles::L1L2L3Residual);
}
