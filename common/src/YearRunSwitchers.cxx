#include "UHH2/common/include/YearRunSwitchers.h"


YearSwitcher::YearSwitcher(const uhh2::Context & ctx):
  year_(extract_year(ctx)),
  doneInit_(false),
  module2016_(nullptr),
  module2016v2_(nullptr),
  module2016v3_(nullptr),
  module2017_(nullptr),
  module2017v1_(nullptr),
  module2017v2_(nullptr),
  module2017UL_(nullptr),
  module2018_(nullptr),
  module2018UL_(nullptr),
  theModule_(nullptr)
{}

bool YearSwitcher::process(uhh2::Event & event) {
  if (!doneInit_) {
    // First time process() is called, figure out which module is needed
    // based on year from Context. This way we don't have to check each event.
    // We can only do this here, since the user might call setup*() after the ctor.
    // Also check to see if it matches the one in the event
    if (event.year != year_str_map.at(year_)) {
      throw std::runtime_error("event.year in ntuple doesn't match with dataset Version");
    }

    if ((year_ == Year::is2016v2) && module2016v2_) {
      theModule_ = module2016v2_;
    }
    else if ((year_ == Year::is2016v3) && module2016v3_) {
      theModule_ = module2016v3_;
    }
    else if ((year_ == Year::is2016v2 || year_ == Year::is2016v3) && module2016_) {
      theModule_ = module2016_;
    }

    else if ((year_ == Year::is2017v1) && module2017v1_) {
      theModule_ = module2017v1_;
    }
    else if ((year_ == Year::is2017v2) && module2017v2_) {
      theModule_ = module2017v2_;
    }
    else if ((year_ == Year::is2017UL) && module2017UL_) {
      theModule_ = module2017UL_;
    }
    else if ((year_ == Year::is2017v1 || year_ == Year::is2017v2 || year_ == Year::is2017UL) && module2017_) {
      theModule_ = module2017_;
    }

    else if ((year_ == Year::is2018UL) && module2018UL_) {
      theModule_ = module2018UL_;
    }
    else if ((year_ == Year::is2018 || year_ == Year::is2018UL) && module2018_) {
      theModule_ = module2018_;
    }
    doneInit_ = true;
  }

  if (theModule_ != nullptr) {
    return theModule_->process(event);
  }
  return true;
}

std::shared_ptr<uhh2::AnalysisModule> YearSwitcher::module() {
  return theModule_;
}

void YearSwitcher::setup2016(std::shared_ptr<uhh2::AnalysisModule> module) {
  module2016_ = module;
}

void YearSwitcher::setup2016v2(std::shared_ptr<uhh2::AnalysisModule> module) {
  module2016v2_ = module;
}

void YearSwitcher::setup2016v3(std::shared_ptr<uhh2::AnalysisModule> module) {
  module2016v3_ = module;
}

void YearSwitcher::setup2017(std::shared_ptr<uhh2::AnalysisModule> module) {
  module2017_ = module;
}

void YearSwitcher::setup2017v1(std::shared_ptr<uhh2::AnalysisModule> module) {
  module2017v1_ = module;
}

void YearSwitcher::setup2017v2(std::shared_ptr<uhh2::AnalysisModule> module) {
  module2017v2_ = module;
}

void YearSwitcher::setup2017UL(std::shared_ptr<uhh2::AnalysisModule> module) {
  module2017UL_ = module;
}

void YearSwitcher::setup2018(std::shared_ptr<uhh2::AnalysisModule> module) {
  module2018_ = module;
}

void YearSwitcher::setup2018UL(std::shared_ptr<uhh2::AnalysisModule> module) {
  module2018UL_ = module;
}



RunSwitcher::RunSwitcher(const uhh2::Context & ctx, const std::string & year):
  year_(shortYear(year)),
  skip_(false)
{
  auto foundYear = run_number_map.find(year_);
  if (foundYear == run_number_map.end()) {
    std::string valid = "";
    for (const auto & itr : run_number_map) {
      valid += itr.first;
      valid += ", ";
    }
    throw std::runtime_error("year for RunSwitcher must be one of: " + valid);
  }

  // Determine if thise module can be skipped in process(),
  // because it is not applicable to this dataset's year
  skip_ = (shortYear(year_str_map.at(extract_year(ctx))) != year_);

  // store run name <> run numbers for this year
  runNumberMap_ = foundYear->second;
}

bool RunSwitcher::process(uhh2::Event & event) {
  // this class only makes sense for data, or the applicable year
  if (!event.isRealData or skip_) return true;


  std::shared_ptr<uhh2::AnalysisModule> mod = module(event);
  if (mod != nullptr) return mod->process(event);
  return true;
}

std::shared_ptr<uhh2::AnalysisModule> RunSwitcher::module(const uhh2::Event & event) {
  // find which run period we are in using event.run
  // then use that to get the relevant module
  if (runNumberMap_.size() == 0) return nullptr;
  for (const auto & [key, val] : runNumberMap_) {
    if (event.run >= val.first && event.run <= val.second) {
      auto foundRun = runModuleMap_.find(key);
      if (foundRun != runModuleMap_.end()) {
        return foundRun->second;
      } else {
        throw std::runtime_error("RunSwitcher module mapping cannot handle run period " + key + " for year " + year_);
      }
    }
  }
  throw std::runtime_error("RunSwitcher cannot handle run number " + std::to_string(event.run) + " for year " + year_);
}

void RunSwitcher::setupRun(const std::string & runPeriod, std::shared_ptr<uhh2::AnalysisModule> module) {
  runModuleMap_[runPeriod] = module;
}

std::string RunSwitcher::shortYear(const std::string & year) {
  // sanitise year, chop off any v*
  return year.substr(0, year.find("v"));
}
