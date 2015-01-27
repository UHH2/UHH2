#pragma once

#include "UHH2/core/include/Hists.h"
#include "UHH2/core/include/Event.h"

#include <vector>
#include <string>

using namespace uhh2;
using namespace std;

/** \brief Common jet histograms such as multiplicity, pt, eta, isolation, etc.
 *
 *
 * Differences to SFrameAnalysis version:
 *   - does not sort jets in pt; do that outside this class if desired
 *   - no log-y version of the histogram; this is a plotting choice.
 *   - if collection is empty, event.jets will be used
 */

struct jetHist{
  TH1F* pt, *eta, *phi, *mass, *csv; 
}; 

class JetHistsBase: public uhh2::Hists {
 public:
  JetHistsBase(uhh2::Context & ctx,
	      const std::string & dirname);


  jetHist jetHistsBooker(string suffix, double minPt, double maxPt);
  
  template <typename T>
    void jetHistFiller(T jet,jetHist jet_hist, double  weight);

  virtual void fill(const uhh2::Event & ev) =0;
};




class JetHists: public JetHistsBase {
public:

  JetHists(uhh2::Context & ctx,
	   const std::string & dirname,
	   const std::string & collection = "");
  
  virtual void fill(const uhh2::Event & ev) override;
  
  
  
 private:
  
  TH1F *number;
  TH1F *deltaRmin_1, *deltaRmin_2;
  
  jetHist alljets;
  vector<jetHist> single_jetHists;
  
  uhh2::Event::Handle<std::vector<Jet> > h_jets;
};



class TopJetHists: public JetHistsBase{
 public: 
  TopJetHists(uhh2::Context & ctx,
	      const std::string & dirname,
	      const std::string & collection = "");
  
 
  
  virtual void fill(const uhh2::Event & ev) override;
   

 private:
  
  TH1F *number;
  TH1F *deltaRmin_1, *deltaRmin_2;
  
  jetHist alljets;
  vector<jetHist> single_jetHists;
  
  uhh2::Event::Handle<std::vector<TopJet> > h_topjets;

};


