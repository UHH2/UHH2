#pragma once

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/common/include/ObjectIdUtils.h"
#include "UHH2/common/include/Utils.h"
#include "UHH2/JetMETObjects/interface/JetCorrectionUncertainty.h"
#include "UHH2/JetMETObjects/interface/JetResolution.h"
#include "UHH2/common/include/JetCorrectionSets.h"
#include "TRandom.h"
#include "TFormula.h"
#include "TFile.h"
#include "TF1.h"

#include <iostream>
#include <fstream>

class FactorizedJetCorrector;
class GenericJetResolutionSmearer;

void correct_jet(FactorizedJetCorrector & corrector, Jet & jet, const uhh2::Event & event, JetCorrectionUncertainty* jec_unc = NULL, int jec_unc_direction=0);

/** \brief (Re-)Correct jets according to the corrections in the passed txt files
*
* txt files are available in JetMETObjects/data/; see README there for instructions how to produce
* updated files.
*
* For some standard jet energy corrections, you can use filenames defined in the JERFiles namespace.
*
* Options parsed from the given Context:
*  - "jecsmear_direction": either "nominal", "up", or "down" to apply nominal, +1sigma, -1sigma smearing resp.
*
* Please note that the JetCorrector does not sort the (re-)corrected jets by pt;
* you might want to do that before running algorithms / plotting which assume that.
*/
class JetCorrector: public uhh2::AnalysisModule {
public:
  explicit JetCorrector(uhh2::Context & ctx, const std::vector<std::string> & filenames, const std::vector<std::string> & filenames_L1RC = {});

  virtual bool process(uhh2::Event & event) override;
  virtual bool correct_met(uhh2::Event & event, const bool & isCHSmet = false, double pt_thresh = 15., double eta_thresh_low=0., double eta_thresh_high=5.5);

  virtual ~JetCorrector();

protected:
  std::unique_ptr<FactorizedJetCorrector> corrector;
  std::unique_ptr<FactorizedJetCorrector> corrector_L1RC;

  JetCorrectionUncertainty* jec_uncertainty;
  int direction = 0; // -1 = down, +1 = up, 0 = nominal
  bool propagate_to_met = false;
  bool used_ak4puppi = false;
  bool used_ak4chs = false;
  bool metprop_possible_ak8chs = false;
  bool metprop_possible_ak8puppi = false;
  bool used_slimmedmet = false;
  bool used_puppimet = false;
  bool used_chsmet = false;
  bool do_metL1RC = false;
};

class TopJetCorrector: public uhh2::AnalysisModule {
public:
  explicit TopJetCorrector(uhh2::Context & ctx, const std::vector<std::string> & filenames);

  virtual bool process(uhh2::Event & event) override;

  virtual ~TopJetCorrector();

private:
  std::unique_ptr<FactorizedJetCorrector> corrector;

  JetCorrectionUncertainty* jec_uncertainty;
  int direction = 0; // -1 = down, +1 = up, 0 = nominal
};

class SubJetCorrector: public uhh2::AnalysisModule {
public:
  explicit SubJetCorrector(uhh2::Context & ctx, const std::vector<std::string> & filenames);

  virtual bool process(uhh2::Event & event) override;

  virtual ~SubJetCorrector();

private:
  std::unique_ptr<FactorizedJetCorrector> corrector;
  JetCorrectionUncertainty* jec_uncertainty;
  int direction = 0; // -1 = down, +1 = up, 0 = nominal
};

class GenericJetCorrector: public uhh2::AnalysisModule {
public:
  explicit GenericJetCorrector(uhh2::Context & ctx, const std::vector<std::string> & filenames, const std::string & collectionname);

  virtual bool process(uhh2::Event & event) override;

  virtual ~GenericJetCorrector();

private:
  std::unique_ptr<FactorizedJetCorrector> corrector;
  uhh2::Event::Handle<std::vector<Jet> > h_jets;
  JetCorrectionUncertainty* jec_uncertainty;
  int direction = 0; // -1 = down, +1 = up, 0 = nominal
};

class GenericTopJetCorrector: public uhh2::AnalysisModule {
public:
  explicit GenericTopJetCorrector(uhh2::Context & ctx, const std::vector<std::string> & filenames, const std::string & collectionname);

  virtual bool process(uhh2::Event & event) override;

  virtual ~GenericTopJetCorrector();

private:
  std::unique_ptr<FactorizedJetCorrector> corrector;
  uhh2::Event::Handle<std::vector<TopJet> > h_jets;
  JetCorrectionUncertainty* jec_uncertainty;
  int direction = 0; // -1 = down, +1 = up, 0 = nominal
};

class GenericSubJetCorrector: public uhh2::AnalysisModule {
public:
  explicit GenericSubJetCorrector(uhh2::Context & ctx, const std::vector<std::string> & filenames, const std::string & _collectionname);

  virtual bool process(uhh2::Event & event) override;

  virtual ~GenericSubJetCorrector();

private:
  std::unique_ptr<FactorizedJetCorrector> corrector;
  uhh2::Event::Handle<std::vector<TopJet> > h_topjets;
  JetCorrectionUncertainty* jec_uncertainty;
  int direction = 0; // -1 = down, +1 = up, 0 = nominal
  std::string collectionname;

  std::unique_ptr<GenericJetResolutionSmearer> m_gjrs;
  uhh2::Event::Handle<std::vector<GenTopJet> > h_gentopjets;
};

/** \brief Cross-clean lepton and jets by subtracting lepton four momenta from nearby jets
*
* Leptons are subtracted from the jets' raw four-momentum if:
*  - DR(jet, lepton) < drmax (default: 0.4) and
*  - electron/muon multiplicity is greater than 0 and
*  - electron energy / muon energy of jet is compatible with lepton to subtract
*
* Only implemented for muons and electrons, not for taus. As default, all muons
* and electrons are used. To not consider all electrons/muons either:
*   - run an appropriate cleaning module before this one or
*   - set an explicit id via the set_electron_id / set_muon_id.
*
* Note that the cleaning works well if using a muon or electron id which is stricly a subset of the
* particle-flow id, because only particle-flow muons/electrons are considered in the muon/electron
* energy fraction stored in the jet which is used to decide whether or not to subtract it.
* So if you use non-PF muons or non-PF electrons, you might need to re-write the
* JetLeptonCleaner for that case.
*
* Please note that the JetLeptonCleaner does not sort the (re-)corrected jets by pt;
* you might want to do that before running algorithms / plotting which assume that.
*
*/
class JetLeptonCleaner: public uhh2::AnalysisModule {
public:
    // jec_filenames is teh same as for the JetCorrector.
    explicit JetLeptonCleaner(uhh2::Context & ctx, const std::vector<std::string> & jec_filenames);

    void set_muon_id(const MuonId & mu_id_){
        mu_id = mu_id_;
    }

    void set_electron_id(const ElectronId & ele_id_){
        ele_id = ele_id_;
    }

    void set_drmax(double drmax_){
        drmax = drmax_;
    }

  virtual bool process(uhh2::Event & event) override;

  virtual ~JetLeptonCleaner();

private:
  std::unique_ptr<FactorizedJetCorrector> corrector;
  MuonId mu_id;
  ElectronId ele_id;
  double drmax = 0.4;
  JetCorrectionUncertainty* jec_uncertainty;
  int direction = 0; // -1 = down, +1 = up, 0 = nominal
};



/** \brief JetLeptonCleaner using the matching of candidates' keys
*
* Can now run on TopJet Collection as well; the class will first check whether a Jet collection with
* the given label name exists, if not, it will look for a TopJet collection with this name.
*
* Default is 'jets' so it will run over the standard Ak4 jet collection.
*
* DISCLAIMER: In case of Ak8 jets, this JetLeptonCleaner only runs over the fat jet itself, not
* the subjets of the Ak8 jets! Whether this is necessary and has any effects on subjet-related
* quantities might need to be tested in the future.
*/
class JetLeptonCleaner_by_KEYmatching: public uhh2::AnalysisModule {

public:
  explicit JetLeptonCleaner_by_KEYmatching(uhh2::Context&, const std::vector<std::string>&, const std::string& jet_label="jets");
  virtual ~JetLeptonCleaner_by_KEYmatching();

  virtual bool process(uhh2::Event & event) override;
  bool do_cleaning(Jet & jet, uhh2::Event & event);

  void set_muon_id(const MuonId& muo_id_){ muo_id = muo_id_; }

  void set_electron_id(const ElectronId& ele_id_){ ele_id = ele_id_; }

private:
  uhh2::Event::Handle<std::vector<Jet> > h_jets_;
  uhh2::Event::Handle<std::vector<TopJet> > h_topjets_;
  std::unique_ptr<FactorizedJetCorrector> corrector;
  MuonId     muo_id;
  ElectronId ele_id;
  JetCorrectionUncertainty* jec_uncertainty;
  int direction = 0; // -1 = down, +1 = up, 0 = nominal
};


//// ----- modules for Jet Energy Resolution data/MC corrections -----

namespace JERSmearing {

  typedef std::vector<std::array<float, 4> > SFtype1;

  extern const SFtype1 SF_13TeV_2016;
  extern const SFtype1 SF_13TeV_2016_03Feb2017;
  extern const SFtype1 SF_13TeV_Summer16_25nsV1;

  extern const SFtype1 SF_13TeV_Fall17;
  extern const SFtype1 SF_13TeV_Fall17_V3;
  extern const SFtype1 SF_13TeV_Fall17_V3_RunBCDEF_Madgraph;// for JERC people only

  // For 2018 V7 and onwards, use text files instead for scale factors

  //Moriond19
  extern const SFtype1 SF_13TeV_Autumn18_V1;
  extern const SFtype1 SF_13TeV_Autumn18_RunABC_V1;
  extern const SFtype1 SF_13TeV_Autumn18_RunD_V1;

  //LHCP19
  extern const SFtype1 SF_13TeV_Autumn18_RunABCD_V4;
  extern const SFtype1 SF_13TeV_Autumn18_RunABC_V4;
  extern const SFtype1 SF_13TeV_Autumn18_RunD_V4;


}



////

/** \brief generalization of JetResolutionSmearer (see the latter for additional info)
*         to apply jet-energy-resolution smearing on non-default jet collections
*
*  Note that the reco and genjets can refer to either Jet or TopJet collections,
*  and GenJet or GenTopJet collections, for flexibility
*
*  Options parsed from Context (e.g. from XML file):
*   - "jersmear_direction": either "nominal", "up", or "down" to apply nominal, +1sigma, -1sigma smearing correction
*
*  There are 2 constructors, depending on the format of the scale factors.
*  For "old-style" scale factors, where they are independent of pt, one can use
*  the constructor that takes a JERSmearing::SFtype1 argument.
*  For "new-style" scale factors that come as a text file, one should use
*  the constructor that takes a TString instead.
*
*  Note that the ResolutionFileName argument will have "common/data" prepended
*  to it.
*
*/
class GenericJetResolutionSmearer : public uhh2::AnalysisModule {

public:
  explicit GenericJetResolutionSmearer(uhh2::Context&, const std::string& recj="jets", const std::string& genj="genjets",
                                       const JERSmearing::SFtype1& JER_sf=JERSmearing::SF_13TeV_Fall17_V3, const TString ResolutionFileName="Fall17_V3_MC_PtResolution_AK4PFchs.txt");
  explicit GenericJetResolutionSmearer(uhh2::Context&, const std::string& recj="jets", const std::string& genj="genjets",
                                       const TString ScaleFactorFileName="", const TString ResolutionFileName="Fall17_V3_MC_PtResolution_AK4PFchs.txt");
  virtual ~GenericJetResolutionSmearer() = default;

  virtual bool process(uhh2::Event&) override;

  template<typename RJ, typename GJ> void apply_JER_smearing(std::vector<RJ>&, const std::vector<GJ>&, float radius, float rho);

 private:
  virtual float getScaleFactor(float pt, float eta);

  uhh2::Event::Handle<std::vector<Jet> >       h_recjets_;
  uhh2::Event::Handle<std::vector<GenJet> >    h_genjets_;
  uhh2::Event::Handle<std::vector<TopJet> >    h_rectopjets_;
  uhh2::Event::Handle<std::vector<GenTopJet> > h_gentopjets_;

  int direction = 0; // -1 = down, +1 = up, 0 = nominal
  JERSmearing::SFtype1 JER_SFs_;

  JME::JetResolution resolution_;
  JME::JetResolutionScaleFactor res_sf_;

};



/** \brief Smear the Jets' (specified in event.jets / JetCollection) four-momenta in MC to match the resolution in data.
* It will have no effect on data events.
*
* The constructor with only the Context argument automatically chooses
* the resolution txt file & scale factors based on year & jet algorithm.
* The other constructors allow the user to specify the scale factors
* (either as vector of floats or from text file), and the resolution txt file
*
* See https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetResolution for details
* We use the hybrid method, depending on if a matching genjet was found.
*
* Run this *after* the jet energy corrections.
*
* IMPORTANT: do NOT run the module twice, as then, the jets will be smeared twice, which
* is too much.
*
* Options parsed from the given Context in the XML file:
*  - "jersmear_direction": either "nominal", "up", or "down" to apply
*    nominal, +1sigma, -1sigma smearing respectively
*
* Please note that the JetResolutionSmearer does not sort the (re-)corrected jets by pt;
* you might want to do that before running algorithms / plotting which assume that.
*/
class JetResolutionSmearer: public uhh2::AnalysisModule{
public:
  explicit JetResolutionSmearer(uhh2::Context & ctx);
  explicit JetResolutionSmearer(uhh2::Context & ctx, const JERSmearing::SFtype1& JER_sf, const std::string& resFilename);
  explicit JetResolutionSmearer(uhh2::Context & ctx, const std::string& scaleFactorFilename, const std::string& resFilename);

  virtual bool process(uhh2::Event & event) override;

  virtual ~JetResolutionSmearer();
private:
  GenericJetResolutionSmearer* m_gjrs;
};

//// -----------------------------------------------------------------
/**
* Calculate the softdrop mass using the subjets, and can optionally apply corrections
* See https://twiki.cern.ch/twiki/bin/view/CMS/JetWtagging
*/
class SoftDropMassCalculator: public uhh2::AnalysisModule {
public:
    explicit SoftDropMassCalculator(uhh2::Context & ctx,
                                    bool applyCorrections=true,
                                    const std::string & puppiCorrFilename="",
                                    const std::string & jetCollName="topjets");
    virtual ~SoftDropMassCalculator() {};
    virtual bool process(uhh2::Event&) override;
    float calcSDmass(const TopJet & jet);
    float getPUPPIweight(float pt, float eta);
private:
    uhh2::Event::Handle<std::vector<TopJet>> h_topjets_;
    uhh2::Event::Handle<std::vector<GenTopJet>> h_gentopjets_;
    bool applyCorrections_;
    std::unique_ptr<TFile> puppiCorrFile;
    std::unique_ptr<TF1> puppisd_corrGEN, puppisd_corrRECO_cen, puppisd_corrRECO_for;
};
//// -----------------------------------------------------------------
