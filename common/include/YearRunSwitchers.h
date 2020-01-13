#pragma once
#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/Utils.h"
#include "UHH2/common/include/Utils.h"

#include <map>


/**
 * Classes to aid automatically running modules for different years and run periods.
 *
 * Thee encapsulate the logic of switching years/runs away, so the user does
 * not need to implement it themselves.
 */


/**
 * Class to handle different AnalysisModules for different years
 *
 * The user can setup a module for each year using the `setup*()` methods,
 * and it will automatically run the correct one for a given dataset
 * when `process()` is called.
 */
class YearSwitcher: public uhh2::AnalysisModule {
public:
  YearSwitcher(const uhh2::Context & ctx);

  // Automatically run the appropriate module.
  // If there isn't a matching one, just return true
  virtual bool process(uhh2::Event & event) override;

  // Get the relevant module, incase you need to access its other methods
  // You will need to cast it to the specific derived type,
  // e.g. std::shared_ptr<RunSwitcher> rs = std::dynamic_pointer_cast<RunSwitcher>(myYearSwitcher->module());
  std::shared_ptr<uhh2::AnalysisModule> module();

  // Methods to assign module for each year
  // Note that the setup<year>v* are more specific, and if set,
  // will take preference over the module passed to setup<year>
  void setup2016(std::shared_ptr<uhh2::AnalysisModule> module);
  void setup2016v2(std::shared_ptr<uhh2::AnalysisModule> module);
  void setup2016v3(std::shared_ptr<uhh2::AnalysisModule> module);

  void setup2017(std::shared_ptr<uhh2::AnalysisModule> module);
  void setup2017v1(std::shared_ptr<uhh2::AnalysisModule> module);
  void setup2017v2(std::shared_ptr<uhh2::AnalysisModule> module);
  void setup2017UL(std::shared_ptr<uhh2::AnalysisModule> module);

  void setup2018(std::shared_ptr<uhh2::AnalysisModule> module);
  void setup2018UL(std::shared_ptr<uhh2::AnalysisModule> module);

private:
  Year year_;
  bool doneInit_;

  // have modules for each year, plus for the specific versions of each year
  // shared_ptr, because the user might already own it, and we want to ensure
  // it is kept alive, or deleted as necessary if this is the only owner
  std::shared_ptr<uhh2::AnalysisModule> module2016_, module2016v2_, module2016v3_;
  std::shared_ptr<uhh2::AnalysisModule> module2017_, module2017v1_, module2017v2_, module2017UL_;
  std::shared_ptr<uhh2::AnalysisModule> module2018_, module2018UL_;
  std::shared_ptr<uhh2::AnalysisModule> theModule_;

};


/**
 * Class to handle different AnalysisModules for different run periods in data
 *
 * One instance of this class is designed to handle a specific year's data.
 * User should then setup a module for each run period (e.g. "A")
 * with the `setupRun()` method.
 * Calling `process()` will then automatically figure out which one to use based
 * on the event run number.
 */
class RunSwitcher: public uhh2::AnalysisModule {
public:
  RunSwitcher(const uhh2::Context & ctx, const std::string & year);

  // Run the module corresponding to this year in the relevant run period
  // If none found, return true
  virtual bool process(uhh2::Event & event) override;

  // Get the relevant module, incase you need to access its other methods
  // You will need to cast it to the specific derived type,
  // e.g. std::shared_ptr<JetCorrector> jc = std::dynamic_pointer_cast<JetCorrector>(myRunSwitcher->module());
  std::shared_ptr<uhh2::AnalysisModule> module(const uhh2::Event & event);

  // Method to assign a module to a particular run period
  void setupRun(const std::string & runPeriod, std::shared_ptr<uhh2::AnalysisModule> module);

private:
  // convert e.g. 2016v2 -> 2016
  std::string shortYear(const std::string & year);

  std::string year_;
  bool skip_;
  std::map<std::string, std::pair<int, int>> runNumberMap_;
  //shared_ptr usage: see comments in YearSwitcher
  std::map<std::string, std::shared_ptr<uhh2::AnalysisModule>> runModuleMap_;
};
