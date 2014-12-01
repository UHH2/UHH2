#include "UHH2/core/include/Hists.h"

/** \brief Common electron histograms such as multiplicity, pt, eta, isolation, etc.
 *
 *
 * Differences to SFrameAnalysis version:
 *   - does not sort electrons in pt; do that outside this class if desired
 *   - no log-y version of the histogram; this is a plotting choice.
 */
class ElectronHists: public uhh2::Hists {
public:
    ElectronHists(uhh2::Context & ctx, const std::string & dirname);
    
    virtual void fill(uhh2::Event & ev) override;
    
private:
    TH1F *number, *pt, *eta, *phi, *isolation;
};


