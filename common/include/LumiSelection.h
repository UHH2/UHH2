#pragma once

#include "UHH2/core/include/Selection.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/common/include/LuminosityHists.h"

#include <string>

//This module removes all data events that are not given in the run/lumisection list given in the lumi_file.
//The lumi file is defined with "lumi_file" in the xml configuration

class LumiSelection: public uhh2::Selection{
 public:
  explicit LumiSelection(uhh2::Context & ctx);
  virtual bool passes(const uhh2::Event & event);

 private:
  //list of good run-lumiblock entries from lumi file
  std::vector<run_lumi> rls;

};
