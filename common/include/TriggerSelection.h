#pragma once

#include "UHH2/core/include/Selection.h"
#include "UHH2/core/include/Event.h"


#include <string>

// This file is for Selections related to triggers.
// The main features are hidden in TriggerIndex, which is part of the Event.h class


class TriggerSelection: public uhh2::Selection{
 public:
  explicit TriggerSelection(std::string triggerName);
  virtual bool passes(const uhh2::Event & event);
  int prescale(const uhh2::Event & event);

 private:
  std::string triggerName;
  uhh2::Event::TriggerIndex triggerIndex;


  bool triggerInit;
};
