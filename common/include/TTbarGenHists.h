#include "UHH2/core/include/Hists.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/common/include/TTbarGen.h"

/** \brief Histograms for ttbar quantities on generator (parton) level
 * 
 * TTbarGen container has to be filled before calling this histogram class
 */
class TTbarGenHists: public uhh2::Hists {
public:
    TTbarGenHists(uhh2::Context & ctx, const std::string & dirname);

    virtual void fill(const uhh2::Event & ev) override;

protected:
    TH1F* Mttbar_gen, *Pt_ttbar_gen, *shat, *M_top, *M_antitop, *Pt_top, *Pt_antitop, *Pt_top_over_shat, *Pt_antitop_over_shat, *Pt_top_over_M_ttbar, *Pt_antitop_over_M_ttbar, *eta_top, *eta_antitop, *y_top, *y_antitop, *diffabseta, *diffabsy, *deltaR_top_decays, *deltaR_antitop_decays, *deltaR_Wplus_decays, *deltaR_Wminus_decays, *Pt_b, *Pt_antib, *eta_b, *eta_antib, *y_b, *y_antib, *M_b, *M_antib, *Pt_Wplus, *Pt_Wminus, *eta_Wplus, *eta_Wminus, *y_Wplus, *y_Wminus, *M_Wplus, *M_Wminus, *cosThetastar_top_ttframe, *cosThetastar_antitop_ttframe, *Pt_top_ttframe, *Pt_antitop_ttframe, *DecayChannel;

    TH2F* M_ttbar_vs_shat, *M_ttbar_vs_deltaR_top, *M_ttbar_vs_deltaR_antitop, *shat_vs_deltaR_top, *shat_vs_deltaR_antitop, *Pt_top_vs_deltaR_top, *Pt_antitop_vs_deltaR_antitop, *M_ttbar_vs_deltaR_Wplus, *M_ttbar_vs_deltaR_Wminus, *M_ttbar_vs_Pt_top, *M_ttbar_vs_Pt_antitop, *shat_vs_Pt_top, *shat_vs_Pt_antitop, *Pt_top_vs_Pt_antitop, *M_ttbar_vs_Pt_top_ttframe, *M_ttbar_vs_Pt_antitop_ttframe, *M_ttbar_vs_eta_top, *M_ttbar_vs_eta_antitop;
    uhh2::Event::Handle<TTbarGen> h_ttbargen;

    TH1F* dileptop_emu_mupt;
    TH1F* dileptop_emu_mueta;
    TH1F* dileptop_emu_ept;
    TH1F* dileptop_emu_eeta;
    TH1F* dileptop_taumu_mupt;
    TH1F* dileptop_taumu_mueta;
    TH1F* dileptop_taumu_taupt;
    TH1F* dileptop_taumu_taueta;
    TH1F* dileptop_mumu_mu1pt;
    TH1F* dileptop_mumu_mu1eta;
    TH1F* dileptop_mumu_mu2pt;
    TH1F* dileptop_mumu_mu2eta;
};
