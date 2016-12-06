#pragma once

#include "UHH2/core/include/Hists.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/LorentzVector.h"

#include "UHH2/common/include/JetIds.h"
#include "UHH2/common/include/TopJetIds.h"

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
      TH1F* pt, *eta, *phi, *mass, *csv, *mvahiggsdiscr, *prunedmass, *subjet_sum_mass; 
  };
    
  JetHistsBase(uhh2::Context & ctx, const std::string & dirname);

  jetHist book_jetHist(const std::string & axisSuffix, const std::string & histSuffix, double minPt, double maxPt);
  void fill_jetHist(const Jet & jet, jetHist & jet_hist, double  weight);
  template<typename T>
    bool passes_id(const T & object, const uhh2::Event & event, const boost::optional<std::function<bool (const T &, const uhh2::Event & )>> & object_id);
};




class JetHists: public JetHistsBase {
public:
  JetHists(uhh2::Context & ctx,
	   const std::string & dirname,
	   const unsigned int NumberOfPlottedJets=4,
	   const std::string & collection = "");
  virtual void fill(const uhh2::Event & ev) override;
  //set id which jets should be looked at
  void set_JetId(JetId my_jetId){jetid = my_jetId;}
 //UserJet defines the i-th Jet to be plotted. The other variables are needed for plotting and to have different histogram names/axis.
  void add_iJetHists(unsigned int UserJet, double minPt=20, double maxPt=800, const std::string & axisSuffix="userjet", const std::string & histSuffix="userjet");
 protected:
  std::vector<unsigned int> m_userjet;
  std::vector<jetHist> userjets;
  unsigned int m_numJetsPlotted;
  TH1F *number;
  TH1F *deltaRmin_1, *deltaRmin_2;
  jetHist alljets;
  std::vector<jetHist> single_jetHists;
  std::string collection;
  uhh2::Event::Handle<std::vector<Jet> > h_jets;
  boost::optional<JetId> jetid;
};


class TopJetHists: public JetHistsBase{
 public: 
  TopJetHists(uhh2::Context & ctx,
	      const std::string & dirname,
	      const unsigned int NumberOfPlottedJets=4,
	      const std::string & collection = "");
  
  virtual void fill(const uhh2::Event & ev) override;
  //set id which topjets should be looked at
  void set_TopJetId(TopJetId my_jetId){topjetid = my_jetId;}
  //UserJet defines the i-th Jet to be plotted. The other variables are needed for plotting and to have different histogram names/axis. For each TopJet all its SubJets are also plotted. 
  void add_iTopJetHists(unsigned int UserJet, double minPt=0, double maxPt=800, double minPt_sub=0, double maxPt_sub=500, const std::string & axisSuffix="userjet", const std::string & histSuffix="userjet");
 protected:
  JetHistsBase::jetHist book_topJetHist(const std::string & axisSuffix, const std::string & histSuffix, double minPt, double maxPt);
  void fill_topJetHist(const TopJet & jet, JetHistsBase::jetHist & jet_hist, double  weight);

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
  TH1F *deltaRmin_1, *deltaRmin_2, *tau32,*tau21,*deltaR_ak4jet,*invmass_topjetak4jet,*HTT_mass,*fRec,*deltaR_lepton;

  jetHist alljets;
  subjetHist allsubjets;
  std::vector<jetHist> single_jetHists;
  std::vector<subjetHist> subjets;
  uhh2::Event::Handle<std::vector<TopJet> > h_topjets;
  boost::optional<TopJetId> topjetid;
};



static const std::vector<float> BTagMCEffBinsEta = {-2.4, 2.4};
static const std::vector<float> BTagMCEffBinsPt = {30., 50., 70., 100., 140., 200., 300., 670.};
/** \brief measure btag efficiency in MC
 *
 * jets_handle_name should point to a handle of type vector<Jet> _or_
 * vector<TopJet>, were in the latter case all of the subjets are used.
 */
class BTagMCEfficiencyHists: public uhh2::Hists {
public:
  BTagMCEfficiencyHists(uhh2::Context & ctx,
                        const std::string & dirname,
                        const CSVBTag::wp & working_point,
                        const std::string & jets_handle_name="jets");

  virtual void fill(const uhh2::Event & ev) override;

protected:
  void do_fill(const std::vector<TopJet> & jets, const uhh2::Event & event);

  const CSVBTag btag_;
  TH2F * hist_b_passing_;
  TH2F * hist_b_total_;
  TH2F * hist_c_passing_;
  TH2F * hist_c_total_;
  TH2F * hist_udsg_passing_;
  TH2F * hist_udsg_total_;
  uhh2::Event::Handle<std::vector<TopJet>> h_topjets_;
  uhh2::Event::Handle<std::vector<Jet>>    h_jets_;
};
