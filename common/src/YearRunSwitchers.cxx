#include "UHH2/common/include/YearRunSwitchers.h"


YearSwitcher::YearSwitcher(uhh2::Context & ctx):
  year_(extract_year(ctx)),
  doneInit_(false),
  module2016_(nullptr),
  module2016v2_(nullptr),
  module2016v3_(nullptr),
  module2017_(nullptr),
  module2017v1_(nullptr),
  module2017v2_(nullptr),
  module2018_(nullptr),
  theModule_(nullptr)
{}

bool YearSwitcher::process(uhh2::Event & event) {
  if (!doneInit_) {
    // First time process() is called, figure out which module is needed
    // based on year from Context. This way we don't have to check each event.
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
    else if ((year_ == Year::is2017v1 || year_ == Year::is2017v2) && module2017_) {
      theModule_ = module2017_;
    }

    else if ((year_ == Year::is2018) && module2018_) {
      theModule_ = module2018_;
    }
    doneInit_ = true;
  }

  if (theModule_ != nullptr) {
    return theModule_->process(event);
  }
  return true;
}

uhh2::AnalysisModule * YearSwitcher::module() {
  return theModule_.get();
}

// have to accept pointer as AnalysisModule is an abstract base class
void YearSwitcher::setup2016(uhh2::AnalysisModule * module) {
  module2016_.reset(module);
}

void YearSwitcher::setup2016v2(uhh2::AnalysisModule * module) {
  module2016v2_.reset(module);
}

void YearSwitcher::setup2016v3(uhh2::AnalysisModule * module) {
  module2016v3_.reset(module);
}

void YearSwitcher::setup2017(uhh2::AnalysisModule * module) {
  module2017_.reset(module);
}

void YearSwitcher::setup2017v1(uhh2::AnalysisModule * module) {
  module2017v1_.reset(module);
}

void YearSwitcher::setup2017v2(uhh2::AnalysisModule * module) {
  module2017v2_.reset(module);
}

void YearSwitcher::setup2018(uhh2::AnalysisModule * module) {
  module2018_.reset(module);
}



RunSwitcher::RunSwitcher(const std::string & year)
{
  // sanitise year, first chop off any v*
  year_ = year.substr(0, year.find("v"));
  auto foundYear = run_number_map.find(year_);
  if (foundYear == run_number_map.end()) {
    std::string valid = "";
    for (const auto & itr : run_number_map) {
      valid += itr.first;
      valid += ", ";
    }
    throw std::runtime_error("year for RunSwitcher must be one of: " + valid);
  }
  // store run name <> run numbers for this year
  runNumberMap_ = foundYear->second;
}

bool RunSwitcher::process(uhh2::Event & event) {
  if (!event.isRealData) return true; // this class only makes sense for data

  uhh2::AnalysisModule * mod = module(event);
  if (mod != nullptr) return mod->process(event);
  return true;
}

uhh2::AnalysisModule * RunSwitcher::module(const uhh2::Event & event) {
  // find which run period we are in using event.run
  // then use that to call the relevant module
  for (const auto & [key, val] : runNumberMap_) {
    if (event.run >= val.first && event.run <= val.second) {
      auto foundRun = runModuleMap_.find(key);
      if (foundRun != runModuleMap_.end()) {
        return foundRun->second.get();
      } else {
        throw std::runtime_error("RunSwitcher cannot handle run period " + key + " for year " + year_);
      }
    }
  }
  throw std::runtime_error("RunSwitcher cannot handle run number for this year");
}

void RunSwitcher::setupRun(const std::string & runPeriod, uhh2::AnalysisModule * module) {
  runModuleMap_[runPeriod].reset(module);
}
