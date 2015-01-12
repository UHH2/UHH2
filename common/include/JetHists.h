#include "UHH2/core/include/Hists.h"

/** \brief Common jet histograms such as multiplicity, pt, eta, isolation, etc.
 *
 *
 * Differences to SFrameAnalysis version:
 *   - does not sort jets in pt; do that outside this class if desired
 *   - no log-y version of the histogram; this is a plotting choice.
 */
class JetHists: public uhh2::Hists {
public:
    JetHists(uhh2::Context & ctx, const std::string & dirname);
    
    virtual void fill(const uhh2::Event & ev) override;
    
private:
    
    // declare all histograms as members. Note that one could also use get_hist
    // as in the example's ExampleHists instead of saving the histograms here. However,
    // that would entail quite a runtime overhead and it is much faster to declare the histograms
    // here directly.
    TH1F *number, *pt, *eta, *phi, *isolation, *charge, *ptrel, *deltaRmin;
    TH1F *pt_1, *eta_1, *phi_1, *m_1, *isolation_1, *charge_1, *ptrel_1, *deltaRmin_1;
    TH1F *pt_2, *eta_2, *phi_2, *m_2, *isolation_2, *charge_2, *ptrel_2, *deltaRmin_2;
    TH1F *pt_3, *eta_3, *phi_3, *m_3; 
    TH1F *pt_4, *eta_4, *phi_4, *m_4; 
 
    TH2F *deltaRmin_ptrel, *deltaRmin_ptrel_1, *deltaRmin_ptrel_2;
};
