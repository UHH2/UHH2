#pragma once

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/common/include/ReconstructionHypothesis.h"
#include "TMinuit.h"
#include "UHH2/core/include/Event.h"

typedef std::function< std::vector<LorentzVector>  (const LorentzVector lepton, const LorentzVector met)> NeutrinoReconstructionMethod;

using namespace uhh2;
using namespace std;


class PrimaryLepton: public uhh2::AnalysisModule {
 public:
  /**
   * AnalysisModule that searches the muon or electron with highest pT
   * Should be called after proper cleaning modules for electron and muon ID
   */
  explicit PrimaryLepton(Context & ctx);

  virtual bool process(uhh2::Event & event) override;

  virtual ~PrimaryLepton();

 private:
  Event::Handle<FlavorParticle> h_primlep;
};

class HighMassTTbarReconstruction: public uhh2::AnalysisModule {
public:
  /**
   * @short AnalysisModule that calculates ttbar reconstruction hypotheses and fills them into the event output
   *
   * Reconstructs the ttbar system by constructing a chi2 function
   * taking into account different combinations of jets with the lepton and MET. 
   * The solution which minimizes chi2 in an event is used, the hadronic side
   * is allowed to consist of 1,2 or 3 jets to account for very boosted topologies.
   *
   * neutrinofunction can be either NeutrinoReconstruction or NeutrinoFitPolar
   *
   * label = name of the hypotheses list in output tree
   */
    explicit HighMassTTbarReconstruction(Context & ctx, const NeutrinoReconstructionMethod & neutrinofunction, string label="HighMassReconstruction");
    
    virtual bool process(uhh2::Event & event) override;
    
    virtual ~HighMassTTbarReconstruction();

 private:
    NeutrinoReconstructionMethod m_neutrinofunction;
    Event::Handle<vector<ReconstructionHypothesis>> h_recohyps;
    Event::Handle<FlavorParticle> h_primlep;
};

class CAJetsReconstruction: public uhh2::AnalysisModule {
public:
  /**
   * @short AnalysisModule that calculates ttbar reconstruction hypotheses and fills them into the event output
   *
   * Take the C/A-Jet with the highest pT and use it as hadronic Top. 
   * For the leptonic side the primary lepton is taken. A list is generated 
   * with all possible combinations of one ak 0.5 Jets. Later one ak 0.5 Jet is selected  
   * via chi_square.
   * ak 0.5 Jets with  
   *
   * neutrinofunction can be either NeutrinoReconstruction or NeutrinoFitPolar
   *
   * label = name of the hypotheses list in output tree
   */
    explicit CAJetsReconstruction(Context & ctx, const NeutrinoReconstructionMethod & neutrinofunction, string label="CAJetsReconstruction");
    
    virtual bool process(uhh2::Event & event) override;
    
    virtual ~CAJetsReconstruction();

 private:
    NeutrinoReconstructionMethod m_neutrinofunction;
    Event::Handle<vector<ReconstructionHypothesis>> h_recohyps;
    Event::Handle<FlavorParticle> h_primlep;
};

class TopTagReconstruction: public uhh2::AnalysisModule {
public:
  /**
   * @short AnalysisModule that calculates ttbar reconstruction hypotheses and fills them into the event output
   *
   * Take the C/A-Jet with the TopTag and use it as hadronic Top.
   * For the leptonic side the primary lepton is taken. A list is generated 
   * with all possible combinations of single ak5 jets. Later one hypothesis is selected 
   * via chi_square. Make sure a reasonable selection is run. There is hardcoded a minimum
   * distance of lepton and TopTag of 0.8 and between ak5 jet and TopTag of 1.3 (measure in delta R)
   *
   * neutrinofunction can be either NeutrinoReconstruction or NeutrinoFitPolar
   *
   * label = name of the hypotheses list in output tree
   */
    explicit TopTagReconstruction(Context & ctx, const NeutrinoReconstructionMethod & neutrinofunction, string label="TopTagReconstruction");
    
    virtual bool process(uhh2::Event & event) override;
    
    virtual ~TopTagReconstruction();

 private:
    NeutrinoReconstructionMethod m_neutrinofunction;
    Event::Handle<vector<ReconstructionHypothesis>> h_recohyps;
    Event::Handle<FlavorParticle> h_primlep;
};



  /**
   * @short function to calculate the neutrino four-momentum from MET and charged lepton momenta
   *
   * Given the Decay:
   *
   * A -> B + Neutrino
   *
   * reconstruct the Neutrino pZ component of the Lorentz Vector given
   * it's Energy, px, and py are known.
   *
   * Calculation is carried with formula:
   *
   * P4A^2 = (P4B + P4Neutrino)^2
   *
   * assuming:
   *
   * P4Neutrino^2 = 0
   * P4B^2 = 0
   *
   * within the SM: m_neutrino = 0, mass of the second decay product
   * is neglected due to expected small mass, e.g. in case of the
   * electron: m_B = 0.5 MeV, for the muon: m_mu = 105 MeV and mass
   * of the W-boson: m_W = 80 GeV. m_B is used in formula in form:
   *
   * m_A^2 - m_B^2
   *
   * and therefore m_B can be neglected.
   *
   * The final equation is:
   *
   * (-pTlep^2) * x^2 + 2 * (mu * pZlep) * x + (mu^2 - Elep^2 * pTnu^2) = 0
   *
   * where
   * x is pz_nu
   * mu = mW^2 / 2 + pTlep * pTnu * cos(phi)
   * phi is angle between p_lepton and p_neutrino in transverse plane
   *
   */
  std::vector<LorentzVector> NeutrinoReconstruction(const LorentzVector lepton, const LorentzVector met);

  /*
   * @short Extension of EventCalc::NeutrinoReconstruction
   *
   * Extension of the neutrino reconstruction in EventCalc. This function 
   * performs a fit if the sqrt() part of the solution becomes imaginary.
   *
   * Basic idea is that for the case the Neutrino becomes an Im-Part you do a fit
   * on the px and py components. If you look into the kinematics there is a way of 
   * doing this with a W-mass constraint. As the name indicates polarcoordinates 
   * are used in the computation.
   * 
   */
  std::vector<LorentzVector> NeutrinoFitPolar(const LorentzVector lepton, const LorentzVector met);

