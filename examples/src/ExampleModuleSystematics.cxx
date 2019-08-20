#include <iostream>
#include <memory>

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/Hists.h"
#include "UHH2/core/include/Utils.h"
#include "UHH2/common/include/CommonModules.h"
#include "UHH2/common/include/MCWeight.h"
#include "UHH2/common/include/JetCorrections.h"
#include "UHH2/common/include/YearRunSwitchers.h"
#include "UHH2/common/include/JetIds.h"


using namespace std;
using namespace uhh2;

/** \brief Example module to run some common modules, and show how systematics
can be evaluated. Some more details in the XML file, examples/config/ExampleSystematics.xml,
since the "master" switch for many systematics is set there.
 */

namespace uhh2examples {

class ExampleModuleSystematics: public AnalysisModule {
public:

  explicit ExampleModuleSystematics(Context & ctx);
  virtual bool process(Event & event) override;

private:
  std::unique_ptr<CommonModules> common;
  std::unique_ptr<MCScaleVariation> scaleVar;
  std::unique_ptr<YearSwitcher> topjet_corrector_MC;
  std::unique_ptr<GenericJetResolutionSmearer> topjet_resolution_smearer;
  std::unique_ptr<MCMuonScaleFactor> muon_id_sf;
  std::unique_ptr<MCBTagScaleFactor> btag_sf;
};


ExampleModuleSystematics::ExampleModuleSystematics(Context & ctx)
{
  cout << "Hello World from ExampleModuleSystematics!" << endl;

  // Setup Common modules
  // This does the JEC, JER, for event.jets automatically, and apply it to MET,
  // as well as doing PU & lumi rewighting
  // These will automatically read from the XML file (via the ctx object),
  // and use "jecsmear_direction", "jersmear_direction", "pileup_direction"
  common.reset(new CommonModules());
  // annoyingly we have to handle PU variation ourselves -
  // we manually link up the option in XML to the init() argument
  const std::string & SysType_PU = ctx.get("pileup_direction", "nominal");
  common->init(ctx, SysType_PU);

  // Setup JEC, JER for TopJets ourselves this time
  // Setup for MC only in this example
  // Again, this read "jecsmear_direction" and "jersmear_direction" from XML
  topjet_corrector_MC.reset(new YearSwitcher(ctx));
  topjet_corrector_MC->setup2016(std::make_shared<TopJetCorrector>(ctx, JERFiles::JECFilesMC("Summer16_07Aug2017", "11", "AK8PFchs")));
  topjet_corrector_MC->setup2017(std::make_shared<TopJetCorrector>(ctx, JERFiles::JECFilesMC("Fall17_17Nov2017", "32", "AK8PFchs")));
  topjet_corrector_MC->setup2018(std::make_shared<TopJetCorrector>(ctx, JERFiles::JECFilesMC("Autumn18", "7", "AK8PFchs")));
  // This should really be set up for different years as well, but to keep life simple it's only for 2018
  topjet_resolution_smearer.reset(new GenericJetResolutionSmearer(ctx,
                                                                  "topjets",
                                                                  "gentopjets",
                                                                  JERSmearing::SF_13TeV_Autumn18_RunABCD_V4,
                                                                  "2018/Autumn18_V4_MC_PtResolution_AK8PFPuppi.txt"));


  // Setup Scale (muF, muR) reweighting
  scaleVar.reset(new MCScaleVariation(ctx));

  // Setup muon ID SF weights (similar for ISO SF, and electron SFs)
  // Only makes sense for MC
  // TODO: MuonPOG have better systematics files now - use those instead?
  float muonPtSystPercentage = 0.1; // this is an additionaly uncertainity of 0.1% of the muon pT, that gets added in quadrature to the error from the SF
  muon_id_sf.reset(new MCMuonScaleFactor(ctx,
                                         "common/data/2018/Muon_ID_SF_RunABCD.root", // make sure you pick the right file for your year - maybe setup with YearRunSwitcher?
                                         "NUM_LooseID_DEN_TrackerMuons_pt_abseta", // example - make sure you pick the name that corresponds to ID used in analysis
                                         muonPtSystPercentage,
                                         "",
                                         true,
                                         ctx.get("muon_id_sf_direction", "nominal"),  // here we link up the option in XML to the class constructor argument
                                         "muons"));

  // Setup BTag SF weights
  btag_sf.reset(new MCBTagScaleFactor(ctx,
                                      BTag::DEEPCSV, // example settings here - these should agree with whatever ID you use in your analysis
                                      BTag::WP_MEDIUM,
                                      "jets",
                                      ctx.get("btag_sf_direction", "nominal"),  // here we link up the option in XML to the class constructor argument
                                      "mujets",
                                      "incl",
                                      "MCBtagEfficiencies",
                                      "",
                                      "BTagCalibration"));
}


bool ExampleModuleSystematics::process(Event & event) {
  cout << "***** event: " << event.event << endl;

  // This will run JEC & JER for event.jets with whatever direction has been specified in the XML
  // (nominal if none explicity specified)
  // This will also be applied to MET
  // It will also run MCPileupReweight
  float oldWeight = event.weight;
  float oldPt = (event.jets->size() > 0) ? event.jets->at(0).pt() : -1;
  common->process(event);
  cout << "  --- Event weight before CommonModules: " << oldWeight << endl;
  cout << "  +++ Event weight after CommonModules: " << event.weight << endl;
  if (event.jets->size() > 0) { cout << "  --- event.jets(0).pt() before JEC, JER: " << oldPt << endl; }
  if (event.jets->size() > 0) { cout << "  +++ event.jets(0).pt() after JEC, JER: " << event.jets->at(0).pt() << endl; }

  if (!event.isRealData) {
    // only do TopJet corrections for MC
    if (event.topjets->size() > 0) { cout << "  --- event.topjets(0).pt() before JEC, JER: " << event.topjets->at(0).pt() << endl; }
    topjet_corrector_MC->process(event);
    topjet_resolution_smearer->process(event);
    if (event.topjets->size() > 0) { cout << "  +++ event.topjets(0).pt() after JEC, JER: " << event.topjets->at(0).pt() << endl; }
  }

  if (event.genInfo->systweights().size() == 0) {
    cout << "No systweights in event.genInfo - no scale variations possible! This is expected for LO Pythia8 samples." << endl;
  } else {
    cout << "  --- Event weight before MCScaleVariation: " << event.weight << endl;
    scaleVar->process(event);
    cout << "  +++ Event weight after MCScaleVariation: " << event.weight << endl;
  }

  if (event.muons->size() > 0) { cout << "  --- event.weight before muon ID SF: " << event.weight << endl; }
  muon_id_sf->process(event);
  if (event.muons->size() > 0) { cout << "  +++ event.weight after muon ID SF: " << event.weight << endl; }

  return true;
}

// as we want to run the ExampleCycleNew directly with AnalysisModuleRunner,
// make sure the ExampleModuleSystematics is found by class name. This is ensured by this macro:
UHH2_REGISTER_ANALYSIS_MODULE(ExampleModuleSystematics)

}
