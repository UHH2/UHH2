#pragma once

#include <string>
#include <unordered_map>

#include <TH1F.h>
#include <TH2F.h>

#include <UHH2/core/include/Hists.h>
#include "UHH2/core/include/Event.h"

typedef std::vector < std::vector < float > > Matrix2D;

class NeuralNetworkBase {

public:
  explicit NeuralNetworkBase(const std::string& path_);
  void extractLayers();
  void extractVariables();
  void PrintVariables();
  void extractClasses();
  std::vector<TString> GetClasses() {return classes;};
  int GetClassIndex(TString key) {return std::distance(classes.begin(),std::find(classes.begin(), classes.end(), key) );};
  void PrintClasses();
  void extractNormalization();
  void PrintNormalization();
  std::vector< float > readLineWeights(TString token);
  void LoadLayers();
  void PrintLayers();
  void NormalizeInput();
  void Apply(const TopJet& jet);

  void CreateInputs(const TopJet& jet);

  Matrix2D MatrixMultiplication( Matrix2D A, Matrix2D B);
  Matrix2D MatrixAddition( Matrix2D A, Matrix2D B);
  Matrix2D ApplyActivation( Matrix2D A, TString activation);
  Matrix2D getOutputs() {return outputs;};

protected:

  TString path;
  TString fname = path+"mymodel.txt";
  TString finfo = path+"mymodelinfo.txt";
  TString fnorm = path+"NormInfo.txt";
  std::vector<int> layers_dim;
  std::vector<TString> variables;
  std::vector<TString> classes;

  std::vector< TString > layers_name;
  std::vector< Matrix2D > layers;
  std::vector< Matrix2D > biases;
  std::vector< TString > activations;

  std::map< TString, std::pair< TString, std::pair<float, float> > > Normalizer;
  Matrix2D inputs;
  Matrix2D outputs;


};
