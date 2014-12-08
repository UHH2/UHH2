#pragma once

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/common/include/ReconstructionHypothesis.h"
#include "TMinuit.h"
#include "UHH2/core/include/Event.h"

typedef std::function< std::vector<LorentzVector>  (const LorentzVector & lepton, const LorentzVector & met)> NeutrinoReconstructionMethod;


/** \brief Search the highest-pt lepton (electron or muon)
 * 
 * The found lepton is written to the event as FlavourParticle
 * with name "PrimaryLepton". Should be called after proper cleaning
 * modules for electron and muon ID.
 *
 */
class PrimaryLepton: public uhh2::AnalysisModule {
public:
  explicit PrimaryLepton(uhh2::Context & ctx);

  virtual bool process(uhh2::Event & event) override;

  virtual ~PrimaryLepton();

private:
  uhh2::Event::Handle<FlavorParticle> h_primlep;
};


/**
 * @short Make a list of ttbar reconstruction hypotheses as used in high-mass semileptonic ttbar 8 TeV analysis
 *
 * Make a list of ttbar reconstruction hypothesis using all (~ 3^Njet) combinations
 * of assigning jets to either the leptonic top, the hadronic top, or none of them; 
 * hypotheses not assigning any jet to either the hadronic or leptonic top
 * are discarded.
 * For the leptonic side, the primary lepton and the neutrino reconstruction
 * according to the neutrinofunction parameter is done, which typically doubles
 * the number of hypotheses.
 * 
 * Make sure to run an appropriate cleaner to keep only jets which should be used
 * in the hypothesis. Only works for events with Njets >= 2.
 *
 * neutrinofunction can be e.g. NeutrinoReconstruction or NeutrinoFitPolar
 *
 * label = name of the hypotheses list in the event / output tree
 */
class HighMassTTbarReconstruction: public uhh2::AnalysisModule {
public:
  
    explicit HighMassTTbarReconstruction(uhh2::Context & ctx, const NeutrinoReconstructionMethod & neutrinofunction, const std::string & label="HighMassReconstruction");
    
    virtual bool process(uhh2::Event & event) override;
    
    virtual ~HighMassTTbarReconstruction();

 private:
    NeutrinoReconstructionMethod m_neutrinofunction;
    uhh2::Event::Handle<std::vector<ReconstructionHypothesis>> h_recohyps;
    uhh2::Event::Handle<FlavorParticle> h_primlep;
};


/**
 * @short Make a list of ttbar reconstruction hypotheses using top tagging
 *
 * Take the C/A-Jet with the TopTag and use it as hadronic Top.
 * For the leptonic side the primary lepton is taken. A list is generated 
 * with all possible combinations of single ak5 jets.
 * Make sure a reasonable selection is run before to get hypotheses.
 * There is hardcoded a minimum
 * distance of lepton and TopTag of 0.8 and between ak5 jet and TopTag of 1.3 (measure in delta R)
 *
 * neutrinofunction can be either NeutrinoReconstruction or NeutrinoFitPolar
 *
 * label = name of the hypotheses list in the event / output tree
 * 
 * TODO: implement, document better.
 */
class TopTagReconstruction: public uhh2::AnalysisModule {
public: 
    explicit TopTagReconstruction(uhh2::Context & ctx, const NeutrinoReconstructionMethod & neutrinofunction, const std::string & label="TopTagReconstruction");
    
    virtual bool process(uhh2::Event & event) override;
    
    virtual ~TopTagReconstruction();

 private:
    NeutrinoReconstructionMethod m_neutrinofunction;
    uhh2::Event::Handle<std::vector<ReconstructionHypothesis>> h_recohyps;
    uhh2::Event::Handle<FlavorParticle> h_primlep;
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
std::vector<LorentzVector> NeutrinoReconstruction(const LorentzVector & lepton, const LorentzVector & met);


/**
 * @short Extension of NeutrinoReconstruction
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
std::vector<LorentzVector> NeutrinoFitPolar(const LorentzVector & lepton, const LorentzVector & met);

