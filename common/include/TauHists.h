#include "UHH2/core/include/Hists.h"

/** \brief Common tau histograms such as multiplicity, pt, eta, isolation, etc.
 *
 *
 * Differences to SFrameAnalysis version:
 *   - does not sort taus in pt; do that outside this class if desired
 *   - no log-y version of the histogram; this is a plotting choice.
 */
class TauHists: public uhh2::Hists {
public:
    TauHists(uhh2::Context & ctx, const std::string & dirname);
    
    virtual void fill(const uhh2::Event & ev) override;
    
private:
    
    // declare all histograms as members. Note that one could also use get_hist
    // as in the example's ExampleHists instead of saving the histograms here. However,
    // that would entail quite a runtime overhead and it is much faster to declare the histograms
    // here directly.
    TH1F *number, *pt, *eta, *phi, *charge, *ptrel, *deltaRmin;
    TH1F *pt_1, *pt_1_binned, *pt_muon1_tau1, *pt_muon1_tau1_binned, *eta_1, *phi_1, *charge_1, *ptrel_1, *deltaRmin_1;
    TH1F *pt_2, *eta_2, *phi_2, *charge_2, *ptrel_2, *deltaRmin_2;
    
    TH2F *deltaRmin_ptrel, *deltaRmin_ptrel_1, *deltaRmin_ptrel_2;
};
