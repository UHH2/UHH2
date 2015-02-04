#pragma once

#include "UHH2/core/include/Hists.h"
#include "UHH2/core/include/Event.h"

/** \brief Common jet histograms such as multiplicity, pt, eta, isolation, etc.
 *
 *
 * Differences to SFrameAnalysis version:
 *   - does not sort jets in pt; do that outside this class if desired
 *   - no log-y version of the histogram; this is a plotting choice.
 *   - if collection is empty, event.jets will be used
 */
class JetHists: public uhh2::Hists {
public:
    JetHists(uhh2::Context & ctx,
             const std::string & dirname,
             const std::string & collection = "",
             bool gen_plots=true,
             float gen_plots_eta_min=0.,
             float gen_plots_eta_max=2.4);
    
    virtual void fill(const uhh2::Event & ev) override;
    
private:
    float gen_plots_eta_min_, gen_plots_eta_max_;
    TH1F *number, *pt, *eta, *phi, *csv;
    TH1F *pt_1, *eta_1, *phi_1, *m_1, *csv_1, *deltaRmin_1;
    TH1F *pt_2, *eta_2, *phi_2, *m_2, *csv_2, *deltaRmin_2;
    TH1F *pt_3, *eta_3, *phi_3, *m_3, *csv_3;
    TH1F *pt_4, *eta_4, *phi_4, *m_4, *csv_4;
    TH1F *eff_sub, *eff_tot, *pt_response;

    uhh2::Event::Handle<std::vector<Jet> > h_jets;
};
