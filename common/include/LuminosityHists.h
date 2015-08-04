#pragma once

#include "UHH2/core/include/Hists.h"

/** \brief Create the "lumi plot", i.e. event yield vs. time in bins of equal integrated luminosity
 * 
 * Configuration from context:
 *  - "lumi_file": path to root file with luminosity information
 *  - "lumihists_lumi_per_bin": integrated luminosity per bin in the histogram (optional; default: 50.0)
 */

struct run_lumi {
  int run;
  int lumiblock;

};

class LuminosityHists: public uhh2::Hists {
public:
    LuminosityHists(uhh2::Context & ctx, const std::string & dirname);
    
    virtual void fill(const uhh2::Event & ev) override;
    
private:
    
    friend bool operator<(const run_lumi &, const run_lumi & rl2);
    
    // save the upper bin borders of those run/lumi numbers to
    // still include in the bin. Has size = nbins - 1, where
    // nbins is the number of bins in the lumi histogram
    std::vector<run_lumi> upper_binborders;
    TH1D * hlumi;
   double lumi_per_bin;
};

