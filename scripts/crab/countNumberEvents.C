#include <iostream>
#include <vector>
#include <iomanip>      // std::setprecision

#include "TFile.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TTreeReaderArray.h"

#include "UHH2/core/include/GenInfo.h"

using namespace std;

/**
 * Standalone utility to count number of events in a Ntuple tree.
 *
 * ignoreWeights: true to ignore event weights, false to sum over them
 *
 * Should ONLY print number of events...although ROOT also prints gubbins
 * Done in C++ for speed (neccessary when iterating over all events in tree)
 *
 * Run:
 * root -q -b 'countNumberEvents.C+("/a/b/c.root",false)'
 *
 * DO NOT PUT SPACES INBETWEEN ARGS
 *
 * root:
 * > .x countNumberEvents("/a/b/c.root", false)
 *
 * In a bash script with args $1, $2:
 *
 * root -q -b -l 'countNumberEvents.C+("'${1}'",'${2}')'
 */

void countNumberEvents(string filename, bool ignoreWeights) {
  if (filename=="") return;
  TFile * f = TFile::Open(filename.c_str());
  if (f == nullptr || f->IsZombie()) {
    throw std::runtime_error("Cannot open " + filename);
  }
  TTree * tree = (TTree*) f->Get("AnalysisTree");
  if (tree == nullptr || tree->IsZombie()) {
    throw std::runtime_error("Cannot get TTree");
  }
  if (ignoreWeights) {
    cout << tree->GetEntriesFast() << endl;
  } else {
    TTreeReader reader(tree);
    TTreeReaderValue<GenInfo> genInfo(reader, "genInfo");
    double sum = 0.; // MUST be double otherwise loses precision
    while (reader.Next()) {
      // yes I should do a check here, but the ROOT example doesn't work so bah
      sum += genInfo->weights()[0];
    }
    cout << setprecision(9) << sum << endl;
  }
  f->Close();
}

int main(int argc, char * argv[]){
  countNumberEvents(argv[0], bool(argv[1]));
}
