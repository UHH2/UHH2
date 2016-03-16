#pragma once

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"


/** \brief Vary Tau energy
 *
 * https://twiki.cern.ch/twiki/bin/view/CMS/TauIDRecommendation13TeV#Tau_energy_scale
 *
 * Interprets "TauEnergyVariation" from Context, the values may be "up" or "down".
 * Any other value will result in no scale variation. 
 */
class TauEnergySmearing: public uhh2::AnalysisModule {
 public:
  explicit TauEnergySmearing(uhh2::Context & ctx);

  virtual bool process(uhh2::Event & event) override;

  private:
  int i_TauEnergy = 0;

};

/** \brief Vary Tau energy resolution
 *
 * Migrated from the 8 TeV LQ analysis. Not needed yet. At the moment, there is no recommendation to apply this uncertainty for any 13TeV analyses.
 *
 * Interprets "TauEnergyResolutionVariation" from Context, the values may be "up" or "down".
 * Any other value will result in no scale variation. 
 */
class TauEnergyResolutionShifter: public uhh2::AnalysisModule {
 public:
  explicit TauEnergyResolutionShifter(uhh2::Context & ctx);

  virtual bool process(uhh2::Event & event) override;

  private:
  int i_TauEnergyRes = 0;

};
