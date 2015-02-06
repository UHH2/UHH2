#pragma once

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/common/include/ObjectIdUtils.h"

class FactorizedJetCorrector;

/// namespace to define some useful filename constants to be used for jet energy corrections
namespace JERFiles {
    extern const std::vector<std::string> PHYS14_L123_MC;
    extern const std::vector<std::string> PHYS14_L123_DATA;
    extern const std::vector<std::string> PHYS14_L123_AK8PFchs_MC;
}

/** \brief (Re-)Correct jets according to the corrections in the passed txt files
 * 
 * txt files are available in JetMETObjects/data/; see README there for instructions how to produce
 * updated files.
 * 
 * For some standard jet energy corrections, you can use filenames defined in the JERFiles namespace.
 * 
 * Please note that the JetCorrector does not sort the (re-)corrected jets by pt;
 * you might want to do that before running algorithms / plotting which assume that.
 */
class JetCorrector: public uhh2::AnalysisModule {
public:
    explicit JetCorrector(const std::vector<std::string> & filenames);
    
    virtual bool process(uhh2::Event & event) override;
    
    virtual ~JetCorrector();
    
private:
    std::unique_ptr<FactorizedJetCorrector> corrector;
};

class TopJetCorrector: public uhh2::AnalysisModule {
public:
    explicit TopJetCorrector(const std::vector<std::string> & filenames);
    
    virtual bool process(uhh2::Event & event) override;
    
    virtual ~TopJetCorrector();
    
private:
    std::unique_ptr<FactorizedJetCorrector> corrector;
};

class SubJetCorrector: public uhh2::AnalysisModule {
public:
    explicit SubJetCorrector(const std::vector<std::string> & filenames);
    
    virtual bool process(uhh2::Event & event) override;
    
    virtual ~SubJetCorrector();
    
private:
    std::unique_ptr<FactorizedJetCorrector> corrector;
};

class GenericJetCorrector: public uhh2::AnalysisModule {
public:
    explicit GenericJetCorrector(uhh2::Context & ctx, const std::vector<std::string> & filenames, const std::string & collectionname);
    
    virtual bool process(uhh2::Event & event) override;
    
    virtual ~GenericJetCorrector();
    
private:
    std::unique_ptr<FactorizedJetCorrector> corrector;
    uhh2::Event::Handle<std::vector<Jet> > h_jets;
};

class GenericTopJetCorrector: public uhh2::AnalysisModule {
public:
    explicit GenericTopJetCorrector(uhh2::Context & ctx, const std::vector<std::string> & filenames, const std::string & collectionname);
    
    virtual bool process(uhh2::Event & event) override;
    
    virtual ~GenericTopJetCorrector();
    
private:
    std::unique_ptr<FactorizedJetCorrector> corrector;
    uhh2::Event::Handle<std::vector<TopJet> > h_jets;
};

class GenericSubJetCorrector: public uhh2::AnalysisModule {
public:
    explicit GenericSubJetCorrector(uhh2::Context & ctx, const std::vector<std::string> & filenames, const std::string & collectionname);
    
    virtual bool process(uhh2::Event & event) override;
    
    virtual ~GenericSubJetCorrector();
    
private:
    std::unique_ptr<FactorizedJetCorrector> corrector;
    uhh2::Event::Handle<std::vector<TopJet> > h_jets;
};

/** \brief Cross-clean lepton and jets by subtracting lepton four momenta from nearby jets
 * 
 * Leptons are subtracted from the jets' raw four-momentum if:
 *  - DR(jet, lepton) < drmax (default: 0.5) and
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
    explicit JetLeptonCleaner(const std::vector<std::string> & jec_filenames);
    
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
    double drmax = 0.5;
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
    void jer_already_applied();
    
    bool smear_met;
    int direction = 0; // -1 = down, +1 = up, 0 = nominal
    bool first_event = true;
    uhh2::Context::MetadataObject<bool> jer_applied;
};
