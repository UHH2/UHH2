#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "UHH2/common/include/NeuralNetworkBase.hpp"

using namespace std;

NeuralNetworkBase::NeuralNetworkBase(const string& path_) : path(path_) {
  LoadLayers();
  extractVariables();
  extractClasses();
  extractNormalization();
  PrintVariables();
  PrintClasses();
  PrintNormalization();
  PrintLayers();
};


void NeuralNetworkBase::extractVariables() {
  string line;
  ifstream myfile(finfo, ios::in);
  while (!myfile.eof()) {
    std::getline(myfile, line);
    if (line.find("SubSetVars") != std::string::npos) {
      line = line.substr(line.find("[")+1, line.find("]")-line.find("[")-1);
      TString tok; int from = 0;
      while (((TString)line).Tokenize(tok, from, ", ")) {
        variables.push_back(tok);
      }
    }
  }
  myfile.close();
}


void NeuralNetworkBase::extractClasses() {
  string line;
  ifstream myfile(finfo, ios::in);
  while (!myfile.eof()) {
    std::getline(myfile, line);
    if (line.find("SamplesNames") != std::string::npos) {
      line = line.substr(line.find("[")+1, line.find("]")-line.find("[")-1);
      TString tok; int from = 0;
      while (((TString)line).Tokenize(tok, from, ", ")) {
        classes.push_back(tok);
      }
    }
  }
  myfile.close();
}



void NeuralNetworkBase::extractNormalization() {
  // std::cout << fnorm << '\n';
  string line;
  ifstream myfile(fnorm, ios::in);
  std::getline(myfile, line);
  while (std::getline(myfile, line) && line!="") {
    // std::cout << line << '\n';
    vector<TString> temp;
    TString tok; int from = 0;
    while (((TString)line).Tokenize(tok, from, " ")) {
      // std::cout << tok << '\n';
      temp.push_back(tok);
    }
    Normalizer.insert(std::make_pair(temp[0], std::make_pair(temp[1],std::make_pair(temp[2].Atof(), temp[3].Atof()))));
    TString input = temp[0];
  }
  myfile.close();


}

void NeuralNetworkBase::PrintLayers(){
  std::cout << "PrintLayers: " << layers.size() << '\n';
  for (size_t i = 0; i < layers.size(); i++) {
    std::cout << layers[i].size() << "x" << layers[i][0].size() << " + " << biases[i].size() << "x" << biases[i][0].size() << " + activations: "<< activations[i] << '\n';
  }
  std::cout << std::endl;
}


void NeuralNetworkBase::PrintVariables(){
  std::cout << "PrintVariables" << '\t';
  for (size_t i = 0; i < variables.size(); i++) {
    std::cout << variables[i] << '\t';
  }
  std::cout << std::endl; std::cout << std::endl;
}


void NeuralNetworkBase::PrintClasses(){
  std::cout << "PrintClasses" << '\t';
  for (size_t i = 0; i < classes.size(); i++) {
    std::cout << classes[i] << ": " << GetClassIndex(classes[i]) << '\t';
  }
  std::cout << std::endl; std::cout << std::endl;
}

void NeuralNetworkBase::PrintNormalization() {
  std::cout << "PrintNormalization" << '\n';
  for(const auto & input : variables){
    std::cout << input << "\t" << Normalizer[input].first << "\t" << Normalizer[input].second.first << "\t" << Normalizer[input].second.second << '\n';
  }
  std::cout << std::endl;
}



void NeuralNetworkBase::extractLayers() {
  string line;
  vector<TString> temp;
  ifstream myfile(finfo, ios::in);
  while (!myfile.eof()) {
    std::getline(myfile, line);
    if (line.find("layers") != std::string::npos) {
      line = line.substr(line.find("[")+1, line.find("]")-line.find("[")-1);
      TString tok; int from = 0;
      while (((TString)line).Tokenize(tok, from, ", ")) {
        layers_dim.push_back(tok.Atoi());
      }
    }
    if (line.find("sample_names") != std::string::npos) {
      line = line.substr(line.find("[")+1, line.find("]")-line.find("[")-1);
      TString tok; int from = 0;
      while (((TString)line).Tokenize(tok, from, ", ")) {
        temp.push_back(tok);
      }
    }
  }
  layers_dim.push_back(temp.size());
  myfile.close();
}


vector< float > NeuralNetworkBase::readLineWeights(TString token) {
  vector< float > temp;
  TString tok; int from = 0;
  while (token.Tokenize(tok, from, ",\t")) {
    temp.push_back(tok.Atof());
  }
  return temp;
}







Matrix2D NeuralNetworkBase::MatrixMultiplication( Matrix2D A, Matrix2D B){
  if (A[0].size()!=B.size()) throw std::invalid_argument( "received invalid argument" );
  Matrix2D C (A.size(), vector<float> (B[0].size(),0));
  for (size_t i = 0; i < C.size(); i++) {
    for (size_t j = 0; j < C[0].size(); j++) {
      for (size_t k = 0; k < B.size(); k++) {
        C[i][j] += A[i][k]*B[k][j];
      }
    }
  }
  return C;
}

Matrix2D NeuralNetworkBase::MatrixAddition( Matrix2D A, Matrix2D B){
  if (A.size()!=B.size() || A[0].size()!=B[0].size() ) throw std::invalid_argument( "received invalid argument" );
  Matrix2D C (A.size(), vector<float> (A[0].size(),0));
  for (size_t i = 0; i < C.size(); i++) {
    for (size_t j = 0; j < C[0].size(); j++) {
      C[i][j] = A[i][j]+B[i][j];
    }
  }
  return C;
}

Matrix2D NeuralNetworkBase::ApplyActivation( Matrix2D A, TString activation){
  if (activation=="ID") {
    return A;
  }
  Matrix2D C (A.size(), vector<float> (A[0].size(),0));
  if (activation=="relu") {
    for (size_t i = 0; i < C.size(); i++) {
      for (size_t j = 0; j < C[0].size(); j++) {
        C[i][j] = (A[i][j]>0)?A[i][j]:0;
      }
    }
  }
  if (activation=="softmax") {
    for (size_t i = 0; i < C.size(); i++) {
      double sum = 0.;
      for (size_t j = 0; j < C[0].size(); j++) sum += exp(A[i][j]);
      for (size_t j = 0; j < C[0].size(); j++) {
        C[i][j] = exp(A[i][j])/sum;
      }
    }
  }
  return C;
}



void NeuralNetworkBase::LoadLayers() {
  string line;
  vector< string > lines;
  ifstream myfile(fname, ios::in);
  while (std::getline(myfile, line)) lines.push_back(line);
  myfile.close();

  for (size_t i = 0; i < lines.size(); i++) {
    line = lines[i];

    if (line.find("New Layer") != std::string::npos) {
      line = lines[i+=1];
      layers_name.push_back((TString)line);
    }

    if (line.find("weights") != std::string::npos) {
      vector< vector< float > > weights;
      while (true) {
        line = lines[i+=1];
        if (line.find("bias") != std::string::npos) {
          break;
        }
        line = line.substr(0,line.size()-1);
        weights.push_back(readLineWeights((TString)line));
      }
      layers.push_back(weights);
    }

    if (line.find("bias") != std::string::npos) {
      line = lines[i+=1];
      line = line.substr(0,line.size()-1);
      vector< vector< float > > bias;
      bias.push_back(readLineWeights((TString)line));
      biases.push_back(bias);
    }

    if (line.find("activation") != std::string::npos) {
      line = line.substr(line.find("\t"),line.size());
      line.erase(std::remove(line.begin(), line.end(), '\t'), line.end());
      activations.push_back((TString)line);
    }

    if (line.find("epsilon") != std::string::npos) {
      vector< float > epsilon;
      line = lines[i+=1];
      epsilon.push_back(((TString)line).Atof());
      line = lines[i+=2];
      vector< float > gamma = readLineWeights((TString)line);
      line = lines[i+=2];
      vector< float > beta = readLineWeights((TString)line);
      line = lines[i+=2];
      vector< float > moving_mean = readLineWeights((TString)line);
      line = lines[i+=2];
      vector< float > moving_variance = readLineWeights((TString)line);
      vector< vector< float > > matrix(moving_variance.size(), vector< float > (moving_variance.size(), 0.) );
      vector< vector< float > > bias(1, vector< float > (moving_variance.size(), 0.) );
      for (size_t i = 0; i < moving_variance.size(); i++) {
        matrix[i][i] = gamma[i]/sqrt(moving_variance[i]+epsilon[0]);
        bias[0][i] = - moving_mean[i]*gamma[i]/sqrt(moving_variance[i]+epsilon[0])+beta[i];
      }
      layers.push_back(matrix);
      biases.push_back(bias);
      activations.push_back((TString)("ID"));
    }
  }
}

void NeuralNetworkBase::CreateInputs(const TopJet& jet) {
  std::vector<float> v;
  for(const auto & var : variables){
    double val;
    if (var == "pt") val = jet.pt();
    else if (var == "eta") val = jet.eta();
    else if (var == "phi") val = jet.phi();
    else if (var == "energy") val = jet.energy();
    else if (var == "tau1") val = jet.tau1();
    else if (var == "tau2") val = jet.tau2();
    else if (var == "tau3") val = jet.tau3();
    else if (var == "tau4") val = jet.tau4();
    else if (var == "btag_combinedSecondaryVertex") val = jet.btag_combinedSecondaryVertex();
    else if (var == "btag_combinedSecondaryVertexMVA") val = jet.btag_combinedSecondaryVertexMVA();
    else if (var == "btag_DeepCSV_probb") val = jet.btag_DeepCSV_probb();
    else if (var == "btag_DeepCSV_probbb") val = jet.btag_DeepCSV_probbb();
    else if (var == "softdropmass") val = jet.softdropmass();
    else throw std::invalid_argument( "received invalid argument" );
    v.push_back(val);
  }
  inputs.clear();
  inputs.push_back(v);
}

void NeuralNetworkBase::NormalizeInput() {
  for (size_t i = 0; i < variables.size(); i++) {
    inputs[0][i] = (inputs[0][i] - Normalizer[variables[i]].second.first) / Normalizer[variables[i]].second.second;
  }
}

void NeuralNetworkBase::Apply(const TopJet& jet) {
  CreateInputs(jet);
  // std::cout << "Input" << '\n';
  // for (size_t i = 0; i < inputs[0].size(); i++) {
  //   std::cout << inputs[0][i] << '\t';
  // }
  // std::cout << '\n';
  NormalizeInput();
  outputs.clear();
  outputs = inputs;
  // std::cout << "NORM" << '\n';
  // for (size_t i = 0; i < outputs[0].size(); i++) {
  //   std::cout << outputs[0][i] << '\t';
  // }
  // std::cout << '\n';
  for (size_t i = 0; i < layers_name.size(); i++) {
    outputs = MatrixMultiplication(outputs,layers[i]);
    outputs = MatrixAddition(outputs,biases[i]);
    outputs = ApplyActivation(outputs,activations[i]);
  }
  // std::cout << "Output " << outputs.size() << " " << outputs[0].size() << '\n';
  // for (size_t i = 0; i < outputs[0].size(); i++) {
  //   std::cout << outputs[0][i] << '\t';
  // }
  // std::cout << '\n';
}
