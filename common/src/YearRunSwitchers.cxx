#include "UHH2/common/include/YearRunSwitchers.h"
#include "UHH2/common/include/Utils.h"


YearSwitcher::YearSwitcher():
  module2016_(nullptr),
  module2016v2_(nullptr),
  module2016v3_(nullptr),
  module2017_(nullptr),
  module2017v1_(nullptr),
  module2017v2_(nullptr),
  module2018_(nullptr)
{}

bool YearSwitcher::process(uhh2::Event & event) {
  if (isYear(event, "2016v2") && module2016v2_) {
    return module2016v2_->process(event);
  }
  else if (isYear(event, "2016v3") && module2016v3_) {
    return module2016v3_->process(event);
  }
  else if (isYear(event, "2016") && module2016_) {
    return module2016_->process(event);
  }

  else if (isYear(event, "2017v1") && module2017v1_) {
    return module2017v1_->process(event);
  } 
  else if (isYear(event, "2017v2") && module2017v2_) {
    return module2017v2_->process(event);
  }
  else if (isYear(event, "2017") && module2017_) {
    return module2017_->process(event);
  }
  
  else if (isYear(event, "2018") && module2018_) {
    return module2018_->process(event);
  }
  
  else {
    throw std::runtime_error("YearSwitcher cannot handle event.year = " + event.year 
                             + ", you must use the relevant setup*() method");
  }
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


bool YearSwitcher::isYear(const uhh2::Event & event, const std::string & year) {
  return event.year.find(year) != std::string::npos;
}

