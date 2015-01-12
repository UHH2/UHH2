#include "UHH2/core/include/Hists.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/common/include/ReconstructionHypothesisDiscriminators.h"
#include "UHH2/common/include/ReconstructionHypothesis.h"
#include "UHH2/common/include/TTbarGen.h"

/** \brief Common histograms for reconstruction hypotheses
 *
 * hyps_name is the name of the reconstruction hypothesis collection, for instance "HighMassReconstruction"
 * discriminator_name is the name of the discriminator used to choose the best reconstruction hypothesis, for instance "Chi2"
 */
class HypothesisHists: public uhh2::Hists {
public:
    HypothesisHists(uhh2::Context & ctx, const std::string & dirname, const std::string & hyps_name, const std::string & discriminator_name);

    virtual void fill(const uhh2::Event & ev) override;

private:
    TH1F *Discriminator, *Discriminator_2, *Discriminator_3;
    TH1F *M_ttbar_rec, *M_ttbar_gen, *M_toplep_rec, *M_tophad_rec, *M_tophad_rec_1jet, *M_tophad_rec_2jet, *M_tophad_rec_3jet;
    TH1F *Pt_toplep_rec, *Pt_tophad_rec, *Pt_ttbar_rec, *Pt_ttbar_gen;
    TH2F *Pt_ttbar_rec_vs_Pt_ttbar_gen, *Discriminator_vs_M_ttbar, *M_ttbar_rec_vs_M_ttbar_gen, *M_ttbar_gen_vs_M_ttbar_rec;
    TH1F *M_ttbar_resolution;
    TH2F *Pt_toplep_rec_vs_Pt_toplep_gen, *Pt_tophad_rec_vs_Pt_tophad_gen, *Pt_lepton_rec_vs_Pt_lepton_gen, *Pt_blep_rec_vs_Pt_blep_gen, *Pt_neutrino_rec_vs_Pt_neutrino_gen;
    TH2F *eta_toplep_rec_vs_eta_toplep_gen, *eta_tophad_rec_vs_eta_tophad_gen, *eta_lepton_rec_vs_eta_lepton_gen, *eta_blep_rec_vs_eta_blep_gen, *eta_neutrino_rec_vs_eta_neutrino_gen;

    uhh2::Event::Handle<std::vector<ReconstructionHypothesis>> h_hyps;
    uhh2::Event::Handle<TTbarGen> h_ttbargen;
    std::string m_discriminator_name;
    
};
