#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <exception>

#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TTreeReaderArray.h"

#include "UHH2/core/include/Event.h"

#include <boost/algorithm/string/predicate.hpp>

using namespace std;


/**
 * Standalone utility to produce LumiList of run : lumisections from Ntuple(s)
 *
 * The output can then be used with the standard LumiList tools:
 * https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideGoodLumiSectionsJSONFile
 *
 * fileName: Ntuple filename (can use wildcard for globbing),
 *           or text file with list of Ntuple filenames
 * outputFile: output JSON filename
 *
 * Done in C++ for speed (neccessary when iterating over all events in tree)
 *
 * Run:
 * root -q -b 'dump_lumilist.C+("/a/b/c.root","output.json")'
 *
 * DO NOT PUT SPACES INBETWEEN ARGS
 *
 * root:
 * > .x dump_lumilist("/a/b/c.root","output.json")
 *
 * In a bash script with args $1, $2:
 *
 * root -q -b -l 'dump_lumilist.C+("'${1}'","'${2}'")'
 */


/**
 * Get map of {run number : [lumisection, lumisection...]} for tree in fileName
 * (i.e. "runs-and-lumis" format)
 *
 * @param fileName Ntuple filename (can use wildcards for globbing)
 * @returns map with data
 */
map<int, set<int> > getRunLumiData(const string & fileName) {
  TChain * chain = new TChain("AnalysisTree");
  chain->Add(fileName.c_str());
  TTreeReader reader(chain);
  TTreeReaderValue<int> run(reader, "run");
  TTreeReaderValue<int> luminosityBlock(reader, "luminosityBlock");

  // Store lumisections in a set, since many events have the same LS,
  // but we only want one copy of each. It also automatically sorts it.
  // I guess we could speed things up by storing the last LS and checking
  // against it, but no guarantee events are ordered such that all events
  // of a given LS are together.
  map<int, set<int> > lumiData;
  int counter = 0;
  while (reader.Next()) {
    if (lumiData.count(*run) == 0) {
      lumiData[*run] = {};
    }
    lumiData[*run].insert(*luminosityBlock);
    counter++;
  }

  delete chain;

  cout << "Ran over " << counter << " events" << endl;
  return lumiData;
}


/**
 * Convert from run-and-lumis format to compact format,
 * since the latter is the only one LumiList can parse from file
 *
 * Logic taken from __init__ of LumiList.py
 */
map<int, vector<pair<int, int>> > convertRunAndLumisToCompact(const map<int, set<int>> & lumiData) {
  map<int, vector<pair<int, int>> > compactList;
  for (auto & runLumi : lumiData) {
    int run = runLumi.first;
    auto lumiList = runLumi.second;
    int lastLumi = -1000;
    compactList[run] = {};
    for (auto & lumi : lumiList) {
      if (lumi == lastLumi) {
        // do nothing, originally designed to store duplicates
      } else if (lumi != lastLumi + 1) { // Break in lumi sequence
        compactList[run].push_back(make_pair(lumi, lumi));
      } else {
        compactList[run].back().second = lumi;
      }
      lastLumi = lumi;
    }
  }
  return compactList;
}


/**
 * Save Run-lumisection map to JSON file
 *
 * @param lumiData   Compact format of runs & lumisections, from convertRunAndLumisToCompact()
 * @param outputFile JSON filename
 */
void dumpCompactRunLumiDataToJSON(map<int, vector<pair<int, int> > > & lumiData, const string & outputFile) {
  // Do it manually, since boost propertytree makes everything a string
  // Can't use ROOT - TBuffer::ToJSON only in 6.13+
  // So hand-roll our own...
  // Format is the compact format:
  // {
  // "run" : [[LS, LS], [LS, LS]],
  // "run" : [[LS, LS], [LS, LS]
  // }
  //
  // Just hard to handle those dangling commas with map/sets, hence all the
  // conversions to vectors

  // map keys to vector
  vector<int> runKeys;
  for (auto & runLumi : lumiData) {
    runKeys.push_back(runLumi.first);
  }

  ofstream jsonFile;
  jsonFile.open(outputFile);

  jsonFile << "{" << endl;
  for (uint runInd=0; runInd<runKeys.size(); runInd++) {
    int runNum = runKeys.at(runInd);
    jsonFile << "\"" << runNum << "\": [";

    const auto & lumis = lumiData[runNum];

    for (uint lumiInd=0; lumiInd<lumis.size(); lumiInd++) {
      auto & lumiPair = lumis.at(lumiInd);
      jsonFile << "[" << lumiPair.first << ", " << lumiPair.second << "]";
      if (lumiInd < lumis.size()-1) {
        jsonFile << ", ";
      }
    }
    jsonFile << "]";
    if (runInd < runKeys.size()-1) {
      jsonFile << ",";
    }
    jsonFile << endl;
  }
  jsonFile << "}" << endl;
  jsonFile.close();
}


void dump_lumilist(const string & fileName, const string & outputFile) {

  if (!(boost::ends_with(outputFile, ".json") || boost::ends_with(outputFile, ".JSON"))) {
    throw runtime_error("outputFile must be *.json");
  }

  // Create temporary filename for run-and-lumis format JSON,
  // user's choice will have the compact format

  if (boost::ends_with(fileName, ".root")) {
    // handle ROOT filename(s)
    cout << "Assuming ROOT file(s)..." << endl;
    auto lumiData = getRunLumiData(fileName);
    auto compactList = convertRunAndLumisToCompact(lumiData);
    dumpCompactRunLumiDataToJSON(compactList, outputFile);

  } else {
    // handle list of ROOT files
    cout << "Assuming list of ROOT file(s)..." << endl;
    ifstream infile(fileName);
    if (infile.is_open()) {
      map<int, set<int> > allLumiData;
      string line;
      while (getline(infile, line)) {
        if (!boost::ends_with(line, ".root")) continue;
        auto lumiData = getRunLumiData(line);

        // merge new map in with old - can't use insert() as it doesn't combine sets,
        // so have to do manually
        for (auto & runLS : lumiData) {
          auto run = runLS.first;
          auto ls = runLS.second;
          if (allLumiData.count(run) == 0) {
            allLumiData[run] = {};
          }
          allLumiData[run].insert(ls.begin(), ls.end());
        }
      }
      auto compactList = convertRunAndLumisToCompact(allLumiData);
      dumpCompactRunLumiDataToJSON(compactList, outputFile);

    } else {
      cout << "Couldn't open " << fileName << endl;
    }

  }

  cout << "Written JSON to " << outputFile << endl;
}

int main(int argc, char * argv[]){
  cout << "Calling main" << endl;
  dump_lumilist(argv[0], argv[1]);
}
