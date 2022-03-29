#pragma once

#include <boost/optional.hpp>

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Electron.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/Muon.h"
#include "UHH2/core/include/Selection.h"
#include "UHH2/core/include/Utils.h"

#include "UHH2/common/include/AdditionalSelections.h"
#include "UHH2/common/include/ElectronIds.h"
#include "UHH2/common/include/MuonIds.h"
#include "UHH2/common/include/ObjectIdUtils.h"
#include "UHH2/common/include/Utils.h"
#include "UHH2/common/include/YearRunSwitchers.h"


namespace uhh2 {

/** ***Easy-to-use wrapper classes for electron and muon scale factors***
 *
 * Caveat 1: The year switchers are only setup for Ultra Legacy.
 *
 * Caveat 2: If you try to do something fancy (triple-lepton analysis with different muon IDs or other wild stuff), you should carefully check what these classes do. In principle, many imaginable use cases can be
 * handled with these classes but, of course, it is not possible to have tested these classes for each of those. Follow the instructions in the comments given in this file.
 *
 * You always need to make sure that your electron/muon collection is alreay correctly cleaned since the process routine of each class will loop over all electrons/muons in the respective collection.
 * If you e.g. have a single-muon selection, then you should clean all muons except that one muon of interest and use its ID and ISO information to setup the constructors of the classes below.
 * Each class has the "do_check" argument (default: true) that initiates a check whether all electrons/muons in the given collection do actually fulfill the ID, ISO etc. you want to apply scale factors for.
 * By default, the classes consider the muons (electrons) in the "muons" ("electrons") handle which are the same as found in *event.muons (*event.electrons). For more advanced use cases (e.g. multi-muon
 * selection where you want to use different ID criteria for the selected muons) you should create handles of instance "std::vector<Muon>" per unique muon ID and setup the respective classes for each ID
 * giving the respective handle name to "handle_name". In order to avoid further problems in that case, you should also change the "weight_postfix" to a unique name per declared handle.
 *
 * You have the option to setup "dummy" classes which do not modify the event weight but create dummy branches for the scale factors in the AnalysisTree filled with dummy weights of -1.0.
 * A possible use case is the following: Imagine that you want to hadd ROOT files from a single-muon and a single-electron selection. This only works if the AnalysisTrees have the exact same structure.
 * Since you normally would only apply muon scale factors (and thus only setup muon scale factor branches) for the muon selection and, similarly, would only apply electron scale factors (and thus only setup
 * electron scale factor branches) for the electron selection, hadd would crash if you tried to add both trees. This can be fixed by setting up dummy electron scale factor branches in the muon channel and vice-versa.
 *
 * Usage example for muon isolation scale factors:
 *
 * // In the UserConfig section of your SFrame XML:
 * <Item Name="SystDirection_MuonIso" Value="nominal"/> <!-- or: up/down -->
 *
 * // In the class defintion of your AnalysisModule:
 * unique_ptr<AnalysisModule> sf_muon_iso;
 *
 * // In the constructor of your AnalysisModule:
 * if(is_muon_channel) {
 *   sf_muon_iso.reset(new uhh2::MuonIsoScaleFactors(ctx, Muon::Selector::PFIsoTight, Muon::Selector::CutBasedIdTight));
 * } else if(is_electron_channel) {
 *   sf_muon_iso.reset(new uhh2::MuonIsoScaleFactors(ctx, boost::none, boost::none, boost::none, boost::none, true)); // only if needed
 * }
 *
 * // In the process routine of your AnalysisModule:
 * sf_muon_iso->process(event);
 */


// For reference on EGamma scale factors in UL:
// https://twiki.cern.ch/twiki/bin/view/CMS/EgammaUL2016To2018

const std::string kBasePathToULEGammaSFs = "common/UHH2-data/egamma_SFs_UL/";

//____________________________________________________________________________________________________
// Note that there are different ROOT files for electron pt in interval 10-20 GeV and above 20 GeV. In case that you have electrons for both cases, you should setup this class twice and, as explained above,
// setup two handles of type std::vector<Electron> each of which contains only the low pt or high pt electrons. Then pass the handle name for low pt electrons to one constructor and set the "low_pt_electrons"
// argument to true (default: false). Proceed similarly for electrons above 20 GeV. Also do not forget to give unique "weight_postfix" arguments. In case of a single-electron selection, it is totally valid
// that only one of the two vector handles will be filled per event.
class ElectronRecoScaleFactors: public uhh2::AnalysisModule {
public:
  ElectronRecoScaleFactors(
    uhh2::Context & ctx,
    const boost::optional<bool> & low_pt_electrons = boost::none,
    const boost::optional<bool> & do_check = boost::none,
    const boost::optional<std::string> & weight_postfix = boost::none,
    const boost::optional<std::string> & handle_name = boost::none,
    const boost::optional<bool> & dummy = boost::none
  );
  virtual bool process(uhh2::Event & event) override;
private:
  const std::string fSystDirectionConfigName = "SystDirection_ElectronReco";
  const bool fLowPtElectrons;
  const bool fDoCheck;
  const std::string fWeightPostfix;
  const std::string fHandleName;
  const bool fDummy;
  const uhh2::Event::Handle<std::vector<Electron>> fHandleElectrons;
  std::vector<uhh2::Event::Handle<float>> fWeightHandles;
  std::unique_ptr<YearSwitcher> fYearSwitcher;
};

//____________________________________________________________________________________________________
// Note that for electron IDs, no scale factors were derived for 1.44 < |eta| < 1.57, cf. https://twiki.cern.ch/twiki/bin/view/CMS/EgammaUL2016To2018#General_note_about_ID_SFs
class ElectronIdScaleFactors: public uhh2::AnalysisModule {
public:
  ElectronIdScaleFactors(
    uhh2::Context & ctx,
    const boost::optional<Electron::tag> & tagID = boost::none,
    const boost::optional<bool> & do_check = boost::none,
    const boost::optional<std::string> & weight_postfix = boost::none,
    const boost::optional<std::string> & handle_name = boost::none,
    const boost::optional<bool> & dummy = boost::none
  );
  virtual bool process(uhh2::Event & event) override;
private:
  const std::string fSystDirectionConfigName = "SystDirection_ElectronId";
  ElectronId fElectronID;
  const bool fDoCheck;
  const std::string fWeightPostfix;
  const std::string fHandleName;
  const bool fDummy;
  const uhh2::Event::Handle<std::vector<Electron>> fHandleElectrons;
  std::vector<uhh2::Event::Handle<float>> fWeightHandles;
  std::unique_ptr<YearSwitcher> fYearSwitcher;
};


// Twiki: https://twiki.cern.ch/twiki/bin/view/CMS/MuonUL201{6,7,8}
// SF files: https://gitlab.cern.ch/cms-muonPOG/muonefficiencies/-/tree/master/Run2/UL

const std::string kBasePathToULMuonSFs = "common/UHH2-data/muon_SFs_UL/";

//____________________________________________________________________________________________________
class MuonIdScaleFactors: public uhh2::AnalysisModule {
public:
  MuonIdScaleFactors(
    uhh2::Context & ctx,
    const boost::optional<Muon::Selector> & selectorID = boost::none,
    const boost::optional<bool> & do_check = boost::none,
    const boost::optional<std::string> & weight_postfix = boost::none,
    const boost::optional<std::string> & handle_name = boost::none,
    const boost::optional<bool> & dummy = boost::none
  );
  virtual bool process(uhh2::Event & event) override;
private:
  const std::string fSystDirectionConfigName = "SystDirection_MuonId";
  MuonId fMuonID;
  const bool fDoCheck;
  const std::string fWeightPostfix;
  const std::string fHandleName;
  const bool fDummy;
  const uhh2::Event::Handle<std::vector<Muon>> fHandleMuons;
  std::vector<uhh2::Event::Handle<float>> fWeightHandles;
  std::unique_ptr<YearSwitcher> fYearSwitcher;
};

//____________________________________________________________________________________________________
class MuonIsoScaleFactors: public uhh2::AnalysisModule {
public:
  MuonIsoScaleFactors(
    uhh2::Context & ctx,
    const boost::optional<Muon::Selector> & selectorISO = boost::none,
    const boost::optional<Muon::Selector> & selectorID = boost::none,
    const boost::optional<bool> & do_check = boost::none,
    const boost::optional<std::string> & weight_postfix = boost::none,
    const boost::optional<std::string> & handle_name = boost::none,
    const boost::optional<bool> & dummy = boost::none
  );
  virtual bool process(uhh2::Event & event) override;
private:
  const std::string fSystDirectionConfigName = "SystDirection_MuonIso";
  MuonId fMuonID;
  const bool fDoCheck;
  const std::string fWeightPostfix;
  const std::string fHandleName;
  const bool fDummy;
  const uhh2::Event::Handle<std::vector<Muon>> fHandleMuons;
  std::vector<uhh2::Event::Handle<float>> fWeightHandles;
  std::unique_ptr<YearSwitcher> fYearSwitcher;
};

//____________________________________________________________________________________________________
// There are official scale factors for the HLT_IsoMu{27,24} et al. trigger combination and the HLT_Mu50 et al. trigger combination. Set the "use_Mu50" argument correspondingly.
// Be aware that these trigger scale factors are derived using certain ID and ISO criteria which you should adapt if you use these scale factors:
// - HLT_IsoMu{27,24} et al. scale factors derived with CutBasedIdTight + PFIsoTight
// - HLT_Mu50 et al. scale factors derived with CutBasedIdGlobalHighPt + TkIsoLoose
// Use the "ignore_id_iso" argument to toggle a sanity check for the muon collection: if set to false (default) and if a muon does not fulfill the corresponding ID and ISO criteria, then you get a crash.
//
// The official scale factors are provided binned in eta/pt (as TH2F), abseta/pt (as TH2F), charge/eta/pt (as TH3F), and charge/abseta/pt (as TH3F). The MCMuonScaleFactor class can only handle TH2F.
// If you want to use the scale factors which are also binned in charge, you have to come up with your own solution. The argument absolute_eta toggles between eta/pt (default) and abseta/pt.
class MuonTriggerScaleFactors: public uhh2::AnalysisModule {
public:
  MuonTriggerScaleFactors(
    uhh2::Context & ctx,
    const boost::optional<bool> & use_Mu50 = boost::none,
    const boost::optional<bool> & do_check = boost::none,
    const boost::optional<std::string> & weight_postfix = boost::none,
    const boost::optional<std::string> & handle_name = boost::none,
    const boost::optional<bool> & absolute_eta = boost::none,
    const boost::optional<bool> & dummy = boost::none
  );
  virtual bool process(uhh2::Event & event) override;
private:
  const std::string fSystDirectionConfigName = "SystDirection_MuonTrigger";
  const boost::optional<bool> fUseMu50;
  std::unique_ptr<OrSelection> fTriggerSelection;
  MuonId fMuonID;
  const bool fDoCheck;
  const std::string fWeightPostfix;
  const std::string fHandleName;
  const bool fAbsEta;
  const bool fDummy;
  const uhh2::Event::Handle<std::vector<Muon>> fHandleMuons;
  std::vector<uhh2::Event::Handle<float>> fWeightHandles;
  std::unique_ptr<YearSwitcher> fYearSwitcher;
};

}
