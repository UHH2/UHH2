#include "UHH2/common/include/YearRunSwitchers.h"


YearSwitcher::YearSwitcher(const uhh2::Context & ctx):
  year_(extract_year(ctx)),
  doneInit_(false),
  module2016_(nullptr),
  module2016v2_(nullptr),
  module2016v3_(nullptr),
  moduleUL16preVFP_(nullptr),
  moduleUL16postVFP_(nullptr),
  module2017_(nullptr),
  module2017v1_(nullptr),
  module2017v2_(nullptr),
  moduleUL17_(nullptr),
  module2018_(nullptr),
  moduleUL18_(nullptr),
  module2022_(nullptr),
  module2023_(nullptr),
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
    else if ((year_ == Year::isUL16preVFP) && moduleUL16preVFP_) {
      theModule_ = moduleUL16preVFP_;
    }
    else if ((year_ == Year::isUL16postVFP) && moduleUL16postVFP_) {
      theModule_ = moduleUL16postVFP_;
    }

    else if ((year_ == Year::is2017v1) && module2017v1_) {
      theModule_ = module2017v1_;
    }
    else if ((year_ == Year::is2017v2) && module2017v2_) {
      theModule_ = module2017v2_;
    }
    else if ((year_ == Year::isUL17) && moduleUL17_) {
      theModule_ = moduleUL17_;
    }
    else if ((year_ == Year::is2017v1 || year_ == Year::is2017v2) && module2017_) {
      theModule_ = module2017_;
    }

    else if ((year_ == Year::isUL18) && moduleUL18_) {
      theModule_ = moduleUL18_;
    }
    else if ((year_ == Year::is2018) && module2018_) {
      theModule_ = module2018_;
    }
    else if ((year_ == Year::is2022) && module2022_) {
      theModule_ = module2022_;
    }
    else if ((year_ == Year::is2023) && module2023_) {
      theModule_ = module2023_;
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

void YearSwitcher::setupUL16preVFP(std::shared_ptr<uhh2::AnalysisModule> module) {
  moduleUL16preVFP_ = module;
}

void YearSwitcher::setupUL16postVFP(std::shared_ptr<uhh2::AnalysisModule> module) {
  moduleUL16postVFP_ = module;
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

void YearSwitcher::setupUL17(std::shared_ptr<uhh2::AnalysisModule> module) {
  moduleUL17_ = module;
}

void YearSwitcher::setup2018(std::shared_ptr<uhh2::AnalysisModule> module) {
  module2018_ = module;
}

void YearSwitcher::setupUL18(std::shared_ptr<uhh2::AnalysisModule> module) {
  moduleUL18_ = module;
}

void YearSwitcher::setup2022(std::shared_ptr<uhh2::AnalysisModule> module) {
  module2022_ = module;
}

void YearSwitcher::setup2023(std::shared_ptr<uhh2::AnalysisModule> module) {
  module2023_ = module;
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
  // sanitise year: chop off any v*, UL
  // ensure it has 20 at the start
  if (year.find("16") != std::string::npos) return "2016";
  if (year.find("17") != std::string::npos) return "2017";
  if (year.find("18") != std::string::npos) return "2018";
  if (year.find("22") != std::string::npos) return "2022";
  if (year.find("23") != std::string::npos) return "2023";
  throw std::runtime_error("Cannot identify shortYear from " + year);
}
