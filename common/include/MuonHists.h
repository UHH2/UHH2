#pragma once
#include "UHH2/core/include/Hists.h"
#include "UHH2/core/include/Event.h"

#include <vector>
#include <string>


/** \brief Common muon histograms such as multiplicity, pt, eta, isolation, etc.
*
*
* Differences to SFrameAnalysis version:
*   - does not sort muons in pt; do that outside this class if desired
*   - no log-y version of the histogram; this is a plotting choice.
*/
class MuonHists: public uhh2::Hists {
public:
  MuonHists(uhh2::Context & ctx, const std::string & dirname, const std::string & collection_="jets", bool gen_plots=true);

  virtual void fill(const uhh2::Event & ev) override;

protected:

  // declare all histograms as members. Note that one could also use get_hist
  // as in the example's ExampleHists instead of saving the histograms here. However,
  // that would entail quite a runtime overhead and it is much faster to declare the histograms
  // here directly.
  TH1F *number, *pt, *eta, *phi, *isolation, *charge, *ptrel, *deltaRmin;
  TH1F *pt_1, *eta_1, *phi_1, *isolation_1, *charge_1, *ptrel_1, *deltaRmin_1;
  TH1F *pt_2, *eta_2, *phi_2, *isolation_2, *charge_2, *ptrel_2, *deltaRmin_2;
  TH1F *eff_sub, *eff_tot, *pt_response;

  TH2F *deltaRmin_ptrel, *deltaRmin_ptrel_1, *deltaRmin_ptrel_2;

  std::string collection;
  uhh2::Event::Handle<std::vector<Jet> > h_jets;
  uhh2::Event::Handle<std::vector<TopJet> > h_topjets;
};
