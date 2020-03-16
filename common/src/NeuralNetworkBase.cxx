#include <fstream>
#include <sstream>
#include <string>

#include "UHH2/common/include/NeuralNetworkBase.hpp"

using namespace uhh2;
using namespace std;


NeuralNetworkBase::NeuralNetworkBase(Context& ctx, const std::string & ModelName, const std::string& ConfigName){
    tensorflow::SessionOptions sessionOptions;
    tensorflow::setThreading(sessionOptions, 1, "no_threads");
    tensorflow::setLogging("3");
    NNsession = tensorflow::createSession(tensorflow::loadGraphDef(ModelName),sessionOptions);
    std::ifstream infile(ConfigName);

    std::string line;
    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        if (line.find("feed {") != std::string::npos || line.find("fetch {") != std::string::npos) {
            std::string layerName;
            std::vector<int> shapes;
            bool isInput = line.find("feed {") != std::string::npos;
            while (line!="}") {
                std::getline(infile, line);
                if (line.find("node_name") != std::string::npos) {
                    layerName = line.substr (line.find('"')+1, line.rfind('"')-(line.find('"')+1));
                }
                if (line.find("size") != std::string::npos) {
                    shapes.push_back(std::stoi(line.substr (line.find(':')+2, line.find('}')-line.find('}')+1)));
                }
            }
            if (isInput) LayerInputs[layerName] = shapes;
            else LayerOutputs.push_back(layerName);
        }
    }
    infile.close();

}

void NeuralNetworkBase::CreateInputs(Event & event){
    NNInputs.clear();
    NNoutputs.clear();

    if (NNInputs.size()!=LayerInputs.size()) throw logic_error("NeuralNetworkBase.cxx: Create a number of inputs diffetent wrt. LayerInputs.size()="+to_string(LayerInputs.size()));
}

bool NeuralNetworkBase::process(Event & event){
    CreateInputs(event);
    std::vector<std::string> inputs;
    inputs.reserve(LayerInputs.size());
    for(auto const& imap: LayerInputs) inputs.push_back(imap.first);

    tensorflow::run(NNsession, inputs, NNInputs, LayerOutputs, &NNoutputs);
    return true;
}
