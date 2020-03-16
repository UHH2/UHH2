#include <iostream>
#include <memory>

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Selection.h"
#include "UHH2/core/include/Utils.h"
#include "UHH2/core/include/Event.h"

#include "UHH2/common/include/EventHists.h"
#include "UHH2/common/include/ElectronHists.h"
#include "UHH2/common/include/MuonHists.h"
#include "UHH2/common/include/MuonIds.h"
#include "UHH2/common/include/ElectronIds.h"
#include "UHH2/common/include/ObjectIdUtils.h"
#include "UHH2/common/include/JetIds.h"
#include "UHH2/common/include/JetCorrections.h"
#include "UHH2/common/include/CleaningModules.h"
#include "UHH2/common/include/CommonModules.h"
#include "UHH2/common/include/JetCorrections.h"
#include "UHH2/common/include/MCWeight.h"
#include "UHH2/common/include/EventVariables.h"
#include "UHH2/common/include/CleaningModules.h"
#include "UHH2/common/include/EventVariables.h"
#include "UHH2/common/include/NSelections.h"
#include "UHH2/common/include/AdditionalSelections.h"
#include "UHH2/common/include/LumiSelection.h"
#include "UHH2/common/include/TriggerSelection.h"
#include "UHH2/common/include/Utils.h"

#include "UHH2/common/include/NeuralNetworkBase.hpp"

using namespace std;

class NeuralNetworkModule: public NeuralNetworkBase {
public:
  explicit NeuralNetworkModule(uhh2::Context&, const std::string & ModelName, const std::string& ConfigName);
  virtual void CreateInputs(uhh2::Event & event) override;
protected:
  uhh2::Event::Handle<std::vector<TopJet> > h_topjets;
};


NeuralNetworkModule::NeuralNetworkModule(Context& ctx, const std::string & ModelName, const std::string& ConfigName): NeuralNetworkBase(ctx, ModelName, ConfigName){
    h_topjets = ctx.get_handle<std::vector<TopJet>>("toppuppijets");
}


void NeuralNetworkModule::CreateInputs(Event & event){
  NNInputs.clear();
  NNoutputs.clear();

  NNInputs.push_back( tensorflow::Tensor(tensorflow::DT_FLOAT, {1, 8}));
  NNInputs.at(0).tensor<float, 2>()(0,0) = event.get(h_topjets).at(0).pt();
  NNInputs.at(0).tensor<float, 2>()(0,1) = event.get(h_topjets).at(0).eta();
  NNInputs.at(0).tensor<float, 2>()(0,2) = event.get(h_topjets).at(0).phi();
  NNInputs.at(0).tensor<float, 2>()(0,3) = event.get(h_topjets).at(0).energy();
  NNInputs.at(0).tensor<float, 2>()(0,4) = event.get(h_topjets).at(0).tau1_groomed();
  NNInputs.at(0).tensor<float, 2>()(0,5) = event.get(h_topjets).at(0).tau2_groomed();
  NNInputs.at(0).tensor<float, 2>()(0,6) = event.get(h_topjets).at(0).tau3_groomed();
  NNInputs.at(0).tensor<float, 2>()(0,7) = event.get(h_topjets).at(0).tau4_groomed();

  if (NNInputs.size()!=LayerInputs.size()) throw logic_error("NeuralNetworkModule.cxx: Create a number of inputs diffetent wrt. LayerInputs.size()="+to_string(LayerInputs.size()));
}

class ExampleModuleNNApplication: public AnalysisModule {

public:

  explicit ExampleModuleNNApplication(uhh2::Context&);
  virtual bool process(uhh2::Event&) override;

protected:

  Event::Handle<std::vector<TopJet> > h_topjets;
  Event::Handle<std::vector<tensorflow::Tensor> > h_NNoutput;
  Event::Handle<double> h_NNoutput1;

  std::unique_ptr<NeuralNetworkModule> NNModule;

};



ExampleModuleNNApplication::ExampleModuleNNApplication(uhh2::Context& ctx){

  h_topjets = ctx.get_handle<std::vector<TopJet>>("toppuppijets");
  h_NNoutput = ctx.get_handle<std::vector<tensorflow::Tensor>>("NNoutput");
  h_NNoutput1 = ctx.declare_event_output<double>("NNoutput1");
  NNModule.reset( new NeuralNetworkModule(ctx, "./mymodel.pb", "./mymodel.config.pbtxt"));

}


bool ExampleModuleNNApplication::process(uhh2::Event& event) {

  NNModule->process(event);
  std::vector<tensorflow::Tensor> NNoutputs = NNModule->GetOutputs();
  event.set(h_NNoutput1, (double)(NNoutputs[0].tensor<float, 2>()(0,0)));
  event.set(h_NNoutput, NNoutputs);

  return true;
}

// as we want to run the ExampleCycleNew directly with AnalysisModuleRunner,
// make sure the ExampleModuleNNApplication is found by class name. This is ensured by this macro:
UHH2_REGISTER_ANALYSIS_MODULE(ExampleModuleNNApplication)
