#include <iostream>
#include <memory>

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/common/include/CommonModules.h"
#include "UHH2/common/include/CleaningModules.h"
#include "UHH2/common/include/ElectronHists.h"
#include "UHH2/common/include/NSelections.h"
#include "UHH2/LwtnnExample/include/LwtnnExampleSelections.h"
#include "UHH2/LwtnnExample/include/LwtnnExampleHists.h"

// First include the class that does the computation
//#include "lwtnn/LightweightGraph.hh"
#include "lwtnn/LightweightNeuralNetwork.hh"
// Then include the json parsing functions
#include "lwtnn/parse_json.hh"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace std;
using namespace uhh2;

namespace uhh2examples {

/** \brief Basic analysis example of an AnalysisModule (formerly 'cycle') in UHH2
 * 
 * This is the central class which calls other AnalysisModules, Hists or Selection classes.
 * This AnalysisModule, in turn, is called (via AnalysisModuleRunner) by SFrame.
 */
class LwtnnExampleModule: public AnalysisModule {
public:
    
    explicit LwtnnExampleModule(Context & ctx);
    virtual bool process(Event & event) override;

private:
    
    std::unique_ptr<CommonModules> common;
    
    std::unique_ptr<JetCleaner> jetcleaner;
   
    // declare the Selections to use. Use unique_ptr to ensure automatic call of delete in the destructor,
    // to avoid memory leaks.
    std::unique_ptr<Selection> njet_sel, dijet_sel;
    
    // store the Hists collection as member variables. Again, use unique_ptr to avoid memory leaks.
    std::unique_ptr<Hists> h_nocuts, h_njet, h_dijet, h_ele;

    std::unique_ptr<lwt::LightweightNeuralNetwork> NeuralNetwork;
    std::map<std::string, double> in_vals;//input for NN
};


LwtnnExampleModule::LwtnnExampleModule(Context & ctx){
    // In the constructor, the typical tasks are to initialize the
    // member variables, in particular the AnalysisModules such as
    // CommonModules or some cleaner module, Selections and Hists.
    // But you can do more and e.g. access the configuration, as shown below.
    
    cout << "Hello World from LwtnnExampleModule!" << endl;



    std::ifstream in_file("/nfs/dust/cms/user/karavdia/lwtnn_TEST/CMSSW_10_2_11/src/UHH2/LwtnnExample/KerasDNNModel/neural_net.json");

    // build the network
    auto config = lwt::parse_json(in_file);

    // 
    // vector<char> outputs_names;
    // outputs_names.push_back("QCD_Mu");//TEST
    NeuralNetwork.reset(new lwt::LightweightNeuralNetwork(config.inputs, config.layers, config.outputs));
    //    NeuralNetwork.reset(new lwt::LightweightNeuralNetwork(config.inputs, config.layers, outputs_names));

    //  std::map<std::string, double> in_vals;
    const size_t total_inputs = config.inputs.size();
    for (size_t nnn = 0; nnn < total_inputs; nnn++) {
      const auto& input = config.inputs.at(nnn);
      //    double ramp_val = ramp(input, nnn, total_inputs);
      cout<<"input.name: "<<input.name<<endl;
      in_vals[input.name] = 0.9;
    }
    const size_t total_outputs = config.outputs.size();
    cout<<"total_outputs = "<<total_outputs<<endl;
    for (size_t nnn = 0; nnn < total_outputs; nnn++) {
      const auto& output = config.outputs.at(nnn);
      cout<<"output.name: "<<output<<endl;
    }


    // // get your saved JSON file as an std::istream object
    // std::ifstream in_file1("/nfs/dust/cms/user/karavdia/lwtnn_TEST/CMSSW_10_2_11/src/UHH2/LwtnnExample/KerasDNNModel/neural_net.json");
    // boost::property_tree::ptree pt;
    // boost::property_tree::read_json(in_file1, pt);
    // for (const auto& v: pt.get_child("outputs"))
    // {
    //   //      cout<<"v.size = "<<v.size()<<endl;
    //   cout<<"v.first: "<<v.first.data()<<" v.second:"<<v.second.get()<<endl;
    //   //      lwt::OutputNodeConfig confout = config.get_output_node(v);
    // }


    // If needed, access the configuration of the module here, e.g.:
    string testvalue = ctx.get("TestKey", "<not set>");
    cout << "TestKey in the configuration was: " << testvalue << endl;
    
    // If running in SFrame, the keys "dataset_version", "dataset_type", "dataset_lumi",
    // and "target_lumi" are set to the according values in the xml file. For CMSSW, these are
    // not set automatically, but can be set in the python config file.
    for(auto & kv : ctx.get_all()){
        cout << " " << kv.first << " = " << kv.second << endl;
    }
    
    // 1. setup other modules. CommonModules and the JetCleaner:
    common.reset(new CommonModules());
    // TODO: configure common here, e.g. by 
    // calling common->set_*_id or common->disable_*
    common->init(ctx);
    jetcleaner.reset(new JetCleaner(ctx, 30.0, 2.4)); 
    
    // note that the JetCleaner is only kept for the sake of example;
    // instead of constructing a jetcleaner explicitly,
    // the cleaning can also be achieved with less code via CommonModules with:
    // common->set_jet_id(PtEtaCut(30.0, 2.4));
    // before the 'common->init(ctx)' line.
    
    // 2. set up selections
    njet_sel.reset(new NJetSelection(2)); // see common/include/NSelections.h
    dijet_sel.reset(new DijetSelection()); // see LwtnnExampleSelections

    // 3. Set up Hists classes:
    h_nocuts.reset(new LwtnnExampleHists(ctx, "NoCuts"));
    h_njet.reset(new LwtnnExampleHists(ctx, "Njet"));
    h_dijet.reset(new LwtnnExampleHists(ctx, "Dijet"));
    h_ele.reset(new ElectronHists(ctx, "ele_nocuts"));

   

}


bool LwtnnExampleModule::process(Event & event) {
    // This is the main procedure, called for each event. Typically,
    // do some pre-processing by calling the modules' process method
    // of the modules constructed in the constructor (1).
    // Then, test whether the event passes some selection and -- if yes --
    // use it to fill the histograms (2).
    // Finally, decide whether or not to keep the event in the output (3);
    // this is controlled by the return value of this method: If it
    // returns true, the event is kept; if it returns false, the event
    // is thrown away.
    
  //    cout << "LwtnnExampleModule: Starting to process event (runid, eventid) = (" << event.run << ", " << event.event << "); weight = " << event.weight << endl;
    
    // 1. run all modules other modules.
    common->process(event);
    jetcleaner->process(event);
    
    // 2. test selections and fill histograms
    h_ele->fill(event);
    h_nocuts->fill(event);
    
    bool njet_selection = njet_sel->passes(event);
    if(njet_selection){
        h_njet->fill(event);
    }
    bool dijet_selection = dijet_sel->passes(event);
    if(dijet_selection){
        h_dijet->fill(event);
    }

    // 3. Calculate NN output
    // fill a map of input nodes
    // //    std::map<std::string, double> inputs;
    // double value = 0; double value_2 = 0; double another_value = 0;
    // std::map<std::string, std::map<std::string, double> > inputs;
    // inputs["inputs"] = {{"variable_0", value}, {"variable_1", value_2}};

  

    auto out_vals = NeuralNetwork->compute(in_vals);
    cout<<"out_vals: "<<endl;
    for (const auto& out: out_vals) {
      std::cout << out.first << " " << out.second << std::endl;
    }
    //    cout<<"out_vals[QCD_Mu] = "<<out_vals["QCD_Mu"]<<endl;

// for(int i=0;i<59;i++){
    //   std::string name = "variable_"+std::to_string(i);
    //   cout<<"Name: "<<name<<endl;
    //   inputs[name] = 0.0;
    // }
    //inputs["variable_0"] = 0;

    // inputs["another_input_node"] = {{"another_value", another_value}};
    // compute the output values
    //    std::map<std::string, double> outputs = graph->compute(inputs);
    //    float NNresponse = 2*(outputs.begin()->second-1.0);
    //    cout<<" NNresponse = "<<NNresponse<<endl;

    //  cout << "LwtnnExampleModule: Starting to process event (runid, eventid) = (" << event.run << ", " << event.event << "); weight = " << event.weight << endl;
    // 4. decide whether or not to keep the current event in the output:
    return njet_selection && dijet_selection;
}

// as we want to run the ExampleCycleNew directly with AnalysisModuleRunner,
// make sure the LwtnnExampleModule is found by class name. This is ensured by this macro:
UHH2_REGISTER_ANALYSIS_MODULE(LwtnnExampleModule)

}
