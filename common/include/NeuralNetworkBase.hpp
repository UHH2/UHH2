#pragma once

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"

// save diagnostic state
#pragma GCC diagnostic push
// turn off the specific warning. Can also use "-Wall"
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include "PhysicsTools/TensorFlow/interface/TensorFlow.h"
#pragma GCC diagnostic pop


class NeuralNetworkBase: public uhh2::AnalysisModule {
public:
    explicit NeuralNetworkBase(uhh2::Context&, const std::string & ModelName, const std::string& );
    bool process(uhh2::Event & event) override;
    virtual void CreateInputs(uhh2::Event & event);
    std::vector<tensorflow::Tensor> GetInputs() const{return NNInputs;}
    std::vector<tensorflow::Tensor> GetOutputs() const{return NNoutputs;}
protected:
    tensorflow::Session* NNsession;
    std::map<std::string, std::vector<int>> LayerInputs;
    std::vector<std::string> LayerOutputs;
    std::vector<tensorflow::Tensor> NNInputs;
    std::vector<tensorflow::Tensor> NNoutputs;
};
