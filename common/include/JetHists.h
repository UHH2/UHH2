#pragma once

#include "UHH2/core/include/Hists.h"
#include "UHH2/core/include/Event.h"

#include "UHH2/core/include/LorentzVector.h"

#include <vector>
#include <string>



/** \brief Common jet histograms such as multiplicity, pt, eta, isolation, etc.
 *
 *
 * Differences to SFrameAnalysis version:
 *   - does not sort jets in pt; do that outside this class if desired
 *   - no log-y version of the histogram; this is a plotting choice.
 *   - if collection is empty, event.jets will be used
 *
 *
 * This class was extended to also host the TopJet Hists since many histograms are similar.
 * A shared parent class exist now with a common function to book and fill histograms for plain jets.
 * In the constructor the number of however ordered jets can be specified. Also multiple single jets 
 * can be addresed by their ordering (add_iJetHists/add_iTopJetHists). 
 *
 * Since subjets need some additional histograms the TopJet Hist class has additional book and fill  
 * routines for them. Subjets are connected to their respective TopJet and with it to the sorting of 
 * the topjets.
 */


/// base class for jet histograms with routines for booking and filling a 'standard set' of histograms (pt, eta, phi, mass, csv).
class JetHistsBase: public uhh2::Hists {
protected:
  struct jetHist{
      TH1F* pt, *eta, *phi, *mass, *csv; 
  };
    
  JetHistsBase(uhh2::Context & ctx, const std::string & dirname);

  jetHist book_jetHist(const std::string & axisSuffix, const std::string & histSuffix, double minPt, double maxPt);
  
  void fill_jetHist(const Jet & jet, jetHist & jet_hist, double  weight);
};




class JetHists: public JetHistsBase {
public:

  JetHists(uhh2::Context & ctx,
	   const std::string & dirname,
	   const unsigned int NumberOfPlottedJets=4,
	   const std::string & collection = "");
  
  virtual void fill(const uhh2::Event & ev) override;
  
  //UserJet defines the i-th Jet to be plotted. The other variables are needed for plotting and to have different histogram names/axis.
  void add_iJetHists(unsigned int UserJet, double minPt=20, double maxPt=800, const std::string & axisSuffix="userjet", const std::string & histSuffix="userjet");
  
 private:
  
  std::vector<unsigned int> m_userjet;
  std::vector<jetHist> userjets;
  unsigned int m_numJetsPlotted;

  TH1F *number;
  TH1F *deltaRmin_1, *deltaRmin_2;
  
  jetHist alljets;
  std::vector<jetHist> single_jetHists;
  
  std::string collection;
  uhh2::Event::Handle<std::vector<Jet> > h_jets;
};


class TopJetHists: public JetHistsBase{
 public: 
  TopJetHists(uhh2::Context & ctx,
	      const std::string & dirname,
	      const unsigned int NumberOfPlottedJets=4,
	      const std::string & collection = "");
  
  virtual void fill(const uhh2::Event & ev) override;

  //UserJet defines the i-th Jet to be plotted. The other variables are needed for plotting and to have different histogram names/axis. For each TopJet all its SubJets are also plotted. 
  void add_iTopJetHists(unsigned int UserJet, double minPt=0, double maxPt=800, double minPt_sub=0, double maxPt_sub=500, const std::string & axisSuffix="userjet", const std::string & histSuffix="userjet");


 private:
     
  struct subjetHist {
    TH1F* number, *sum4Vec, *pt, *eta, *phi, *mass, *csv;
  };
   
  subjetHist book_subjetHist(const std::string & axisSuffix, const std::string & histSuffix, double minPt, double maxPt);

  void fill_subjetHist(const TopJet & topjet, subjetHist & subjet_hist, double weight);
  
  std::string collection;

  std::vector<unsigned int> m_usertopjet;
  std::vector<jetHist> usertopjets;
  std::vector<subjetHist> usersubjets;

  TH1F *number;
  TH1F *deltaRmin_1, *deltaRmin_2;
  
  jetHist alljets;
  subjetHist allsubjets;
  std::vector<jetHist> single_jetHists;
  std::vector<subjetHist> subjets;

  uhh2::Event::Handle<std::vector<TopJet> > h_topjets;
};


