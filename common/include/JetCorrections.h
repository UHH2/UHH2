#pragma once

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/common/include/ObjectIdUtils.h"
#include "UHH2/common/include/Utils.h"
#include "UHH2/JetMETObjects/interface/JetCorrectionUncertainty.h"

class FactorizedJetCorrector;

/// namespace to define some useful filename constants to be used for jet energy corrections
namespace JERFiles {
    extern const std::vector<std::string> PHYS14_L123_MC;
    extern const std::vector<std::string> PHYS14_L123_DATA;
    extern const std::vector<std::string> PHYS14_L123_AK8PFchs_MC;
    extern const std::vector<std::string> Summer15_50ns_L123_AK4PFchs_MC;
    extern const std::vector<std::string> Summer15_50ns_L123_AK4PFchs_DATA;
    extern const std::vector<std::string> Summer15_50ns_L123_AK8PFchs_MC;
    extern const std::vector<std::string> Summer15_50ns_L123_AK8PFchs_DATA;
    extern const std::vector<std::string> Summer15_50ns_L123_AK4PFPuppi_MC;
    extern const std::vector<std::string> Summer15_50ns_L123_AK4PFPuppi_DATA;
    extern const std::vector<std::string> Summer15_50ns_L123_AK8PFPuppi_MC;
    extern const std::vector<std::string> Summer15_50ns_L123_AK8PFPuppi_DATA;
    extern const std::vector<std::string> Summer15_25nsV2_MC;
    extern const std::vector<std::string> Summer15_25ns_L123_AK4PFchs_MC;
    extern const std::vector<std::string> Summer15_25ns_L123_AK4PFchs_DATA;
    extern const std::vector<std::string> Summer15_25ns_L123_AK8PFchs_MC;
    extern const std::vector<std::string> Summer15_25ns_L123_AK8PFchs_DATA;
    extern const std::vector<std::string> Summer15_25ns_L23_AK8PFchs_MC;
    extern const std::vector<std::string> Summer15_25ns_L23_AK8PFchs_DATA;
    extern const std::vector<std::string> Summer15_25ns_L123_AK4PFPuppi_MC;
    extern const std::vector<std::string> Summer15_25ns_L123_AK4PFPuppi_DATA;
    extern const std::vector<std::string> Summer15_25ns_L123_AK8PFPuppi_MC;
    extern const std::vector<std::string> Summer15_25ns_L123_AK8PFPuppi_DATA;
}

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
    explicit JetCorrector(uhh2::Context & ctx, const std::vector<std::string> & filenames);
    
    virtual bool process(uhh2::Event & event) override;
    
    virtual ~JetCorrector();
    
private:
    std::unique_ptr<FactorizedJetCorrector> corrector;
    
    JetCorrectionUncertainty* jec_uncertainty;
    int direction = 0; // -1 = down, +1 = up, 0 = nominal
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
    explicit GenericSubJetCorrector(uhh2::Context & ctx, const std::vector<std::string> & filenames, const std::string & collectionname);
    
    virtual bool process(uhh2::Event & event) override;
    
    virtual ~GenericSubJetCorrector();
    
private:
    std::unique_ptr<FactorizedJetCorrector> corrector;
    uhh2::Event::Handle<std::vector<TopJet> > h_jets;
    JetCorrectionUncertainty* jec_uncertainty;
    int direction = 0; // -1 = down, +1 = up, 0 = nominal
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


/** \brief Smear the jet four-momenta in MC to match the resolution in data
 *
 * The corrections applied correspond to the values listed here:
 * https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetResolution
 * for 8TeV data (r36 from 2014-08-28) using the method to scale
 * the genjet pt - reco pt difference.
 *
 * Run this *after* the jet energy corrections.
 *
 * IMPORTANT: do NOT run the module twice, as then, the jets will be smeared twice, which
 * is too much.
 *
 * Options parsed from the given Context:
 *  - "jersmear_smear_met": if "true", propagate the jet resolution smearing to MET. Default is false.
 *  - "jersmear_direction": either "nominal", "up", or "down" to apply nominal, +1sigma, -1sigma smearing resp.
 * 
 * Please note that the JetResolutionSmearer does not sort the (re-)corrected jets by pt;
 * you might want to do that before running algorithms / plotting which assume that.
 */
class JetResolutionSmearer: public uhh2::AnalysisModule {
public:
    explicit JetResolutionSmearer(uhh2::Context & ctx);

    virtual bool process(uhh2::Event & event) override;

    virtual ~JetResolutionSmearer();
private:
    
    bool smear_met;
    int direction = 0; // -1 = down, +1 = up, 0 = nominal
};

////

/** \brief generalization of JetResolutionSmearer (see the latter for additional info)
 *         to apply jet-energy-resolution smearing on non-default jet collections
 *
 *  options parsed from Context:
 *   - "jersmear_smear_met": if "true", propagate the jet resolution smearing to MET. Default is false.
 *   - "jersmear_direction": either "nominal", "up", or "down" to apply nominal, +1sigma, -1sigma smearing correction
 *
 */
class GenericJetResolutionSmearer : public uhh2::AnalysisModule {

 public:
  explicit GenericJetResolutionSmearer(uhh2::Context&, const std::string& recj="jets", const std::string& genj="genjets", const bool allow_met_smear=true);
  virtual ~GenericJetResolutionSmearer() {}

  virtual bool process(uhh2::Event&) override;

  template<typename RJ, typename GJ> void apply_JER_smearing(std::vector<RJ>&, const std::vector<GJ>&, LorentzVector&);

 private:
  uhh2::Event::Handle<std::vector<Jet> >       h_recjets_;
  uhh2::Event::Handle<std::vector<Particle> >  h_genjets_;
  uhh2::Event::Handle<std::vector<TopJet> >    h_rectopjets_;
  uhh2::Event::Handle<std::vector<GenTopJet> > h_gentopjets_;
  bool smear_met;
  int direction = 0; // -1 = down, +1 = up, 0 = nominal
};

template<typename RJ, typename GJ>
void GenericJetResolutionSmearer::apply_JER_smearing(std::vector<RJ>& rec_jets, const std::vector<GJ>& gen_jets, LorentzVector& met){

  // https://twiki.cern.ch/twiki/bin/view/CMS/JetResolution [13TeV JER measurement]
  constexpr const size_t n = 7;

  static float eta_hi[n] = {0.8  , 1.3  , 1.9  , 2.5  , 3.0  , 3.2  , 5.0  };
  static float c_ct  [n] = {1.061, 1.088, 1.106, 1.126, 1.343, 1.303, 1.320};
  static float c_up  [n] = {1.084, 1.117, 1.136, 1.220, 1.466, 1.414, 1.606};
  static float c_dn  [n] = {1.038, 1.059, 1.076, 1.032, 1.220, 1.192, 1.034};

  for(unsigned int i=0; i<rec_jets.size(); ++i){

    auto& jet = rec_jets.at(i);

    // find next genjet:
    auto closest_genjet = closestParticle(jet, gen_jets);
    // ignore unmatched jets (=no genjets at all or large DeltaR), or jets with very low genjet pt:
    if(closest_genjet == nullptr || uhh2::deltaR(*closest_genjet, jet) > 0.3) continue;
    const float genpt = closest_genjet->pt();
    if(genpt < 15.) continue;

    LorentzVector jet_v4 = jet.v4();
    LorentzVector jet_v4_raw = jet_v4 * jet.JEC_factor_raw();

    const float recopt = jet_v4.pt();
    const float abseta = fabs(jet_v4.eta());

    size_t ieta = 0;
    while(ieta < n && eta_hi[ieta] < abseta) ++ieta;
    if(ieta == n) ieta = n-1;

    float c;
    if     (direction ==  0) c = c_ct[ieta];
    else if(direction ==  1) c = c_up[ieta];
    else if(direction == -1) c = c_dn[ieta];
    else throw std::runtime_error("GenericJetResolutionSmearer::process -- invalid value for JER 'direction' (must be 0, +1 or -1): "+std::to_string(direction));

    const float new_pt = std::max(0.0f, genpt + c * (recopt - genpt));

    jet_v4 *= new_pt / recopt;
    jet.set_v4(jet_v4);

    // propagate JER shifts to MET by using same factor, but for raw jet p4:
    if(smear_met){

      met += jet_v4_raw;
      jet_v4_raw *= new_pt / recopt;
      met -= jet_v4_raw;
    }
  }

  return;
}

////
