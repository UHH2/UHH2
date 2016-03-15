#pragma once

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"



class TauEnergySmearing: public uhh2::AnalysisModule {
 public:
  explicit TauEnergySmearing(uhh2::Context & ctx);

  virtual bool process(uhh2::Event & event) override;

  private:
  int i_TauEnergy = 0;

};

class TauEnergyResolutionShifter: public uhh2::AnalysisModule {
 public:
  explicit TauEnergyResolutionShifter(uhh2::Context & ctx);

  virtual bool process(uhh2::Event & event) override;

  private:
  int i_TauEnergyRes = 0;

};
