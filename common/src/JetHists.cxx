#include "UHH2/common/include/JetHists.h"
#include "UHH2/common/include/Utils.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/LorentzVector.h"

#include "TH1F.h"
#include "TH2F.h"

#include <vector>

using namespace uhh2;
using namespace std;


JetHistsBase::JetHistsBase(uhh2::Context & ctx, const std::string & dirname): Hists(ctx, dirname){}

JetHistsBase::jetHist JetHistsBase::book_jetHist(const string & axisSuffix, const string & histSuffix, double minPt, double maxPt){
  jetHist jet_hist;
  jet_hist.pt = book<TH1F>("pt"+histSuffix,"p_{T} "+axisSuffix,50,minPt,maxPt);
  jet_hist.eta = book<TH1F>("eta"+histSuffix,"#eta "+axisSuffix,100,-5,5);
  jet_hist.phi = book<TH1F>("phi"+histSuffix,"#phi "+axisSuffix,50,-M_PI,M_PI);
  jet_hist.mass = book<TH1F>("mass"+histSuffix,"M^{ "+axisSuffix+"} [GeV/c^{2}]", 100, 0, 300);
  jet_hist.csv = book<TH1F>("csv"+histSuffix,"csv-disriminator "+axisSuffix,50,0,1);
  return jet_hist;
}

void JetHistsBase::fill_jetHist(const Jet & jet, JetHistsBase::jetHist & jet_hist, double weight){
  jet_hist.pt->Fill(jet.pt(), weight);
  jet_hist.eta->Fill(jet.eta(), weight);
  jet_hist.phi->Fill(jet.phi(), weight);
  jet_hist.mass->Fill(jet.v4().M(), weight);
  jet_hist.csv->Fill(jet.btag_combinedSecondaryVertex(), weight);
}

JetHists::JetHists(Context & ctx,
                   const std::string & dname,
                   const unsigned int NumberOfPlottedJets,
                   const std::string & collection_): JetHistsBase(ctx, dname), collection(collection_){
    number = book<TH1F>("number","number of jets",21, -.5, 20.5);
    jetid = boost::none;
    alljets = book_jetHist("jet","_jet",20,1500);
    vector<double> minPt {20,20,20,20};
    vector<double> maxPt {1500,1000,500,350};
    vector<string> axis_suffix {"first jet","second jet","third jet","fourth jet"};
    for(unsigned int i =0; i<NumberOfPlottedJets; i++){
      if(i<4){
        single_jetHists.push_back(book_jetHist(axis_suffix[i],"_"+to_string(i+1),minPt[i],maxPt[i]));
      }
      else {
        single_jetHists.push_back(book_jetHist(to_string(i+1)+"-th jet","_"+to_string(i+1),20,500));
      }
    }
    deltaRmin_1 = book<TH1F>("deltaRmin_1", "#Delta R_{min}(first jet,nearest jet)", 40, 0, 8.0);
    deltaRmin_2 = book<TH1F>("deltaRmin_2", "#Delta R_{min}(2nd jet,nearest jet)", 40, 0, 8.0);
    if(!collection.empty()){
        h_jets = ctx.get_handle<std::vector<Jet> >(collection);
    }
}

void JetHists::add_iJetHists(unsigned int UserJet, double minPt, double maxPt, const std::string & axisSuffix, const std::string & histSuffix ){
  m_userjet.push_back(UserJet); 
  userjets.push_back(book_jetHist(axisSuffix, histSuffix, minPt, maxPt));
}

void JetHists::fill(const Event & event){
    auto w = event.weight;
    vector<Jet> jets = collection.empty() ? *event.jets : event.get(h_jets);
    if(jetid){
      vector<Jet> help_jets;
      for(const auto jet : jets)
	if(passes_id(jet,event,jetid)) help_jets.push_back(jet);
      jets.swap(help_jets);
    }
    number->Fill(jets.size(), w);
    for(unsigned int i = 0; i <jets.size(); i++){
      const auto & jet = jets[i];
      fill_jetHist(jet,alljets,w);
      for (unsigned int m =0; m<m_userjet.size(); ++m) {
          if(m_userjet[m] == i)  fill_jetHist(jet, userjets[m], w);
      }
      if(i < single_jetHists.size()){
        fill_jetHist(jet, single_jetHists[i], w);
      }
      if(i < 2){
        auto next_jet = closestParticle(jet, jets);
        auto drmin = next_jet ? deltaR(jet, *next_jet) : numeric_limits<float>::infinity();
        if(i==0) deltaRmin_1->Fill(drmin, w);
        else deltaRmin_2->Fill(drmin, w);
      }
    }
}

//----------------------------------------------
/** TopJet Histograms
 */
TopJetHists::subjetHist TopJetHists::book_subjetHist(const std::string & axisSuffix, const std::string & histSuffix, double minPt, double maxPt){
  subjetHist subjet_hist;

  subjet_hist.number = book<TH1F>("number"+histSuffix,"number "+axisSuffix,7, -.5, 6.5);
    
  subjet_hist.pt = book<TH1F>("pt"+histSuffix,"p_{T} "+axisSuffix,50,minPt,maxPt);
  subjet_hist.eta = book<TH1F>("eta"+histSuffix,"#eta "+axisSuffix,100,-5,5);
  subjet_hist.phi = book<TH1F>("phi"+histSuffix,"#phi "+axisSuffix,50,-M_PI,M_PI);
  subjet_hist.mass = book<TH1F>("mass"+histSuffix,"M^{ "+axisSuffix+"} [GeV/c^{2}]", 100, 0, 200);
  subjet_hist.csv = book<TH1F>("csv"+histSuffix,"csv-disriminator "+axisSuffix,50,0,1);
  subjet_hist.sum4Vec = book<TH1F>("sum_mass"+histSuffix,"Mass sum  "+axisSuffix,100,0,350);

  return subjet_hist;
}

void TopJetHists::fill_subjetHist(const TopJet & topjet, subjetHist & subjet_hist, double weight){
  auto subjets = topjet.subjets();
  subjet_hist.number->Fill(subjets.size(),weight);
  LorentzVector sumLorenzv4;
  for (auto & subjet : subjets) {
    subjet_hist.pt->Fill(subjet.pt(), weight);
    subjet_hist.eta->Fill(subjet.eta(), weight);
    subjet_hist.phi->Fill(subjet.phi(), weight);
    subjet_hist.mass->Fill(subjet.v4().M(), weight);
    subjet_hist.csv->Fill(subjet.btag_combinedSecondaryVertex(), weight);
    sumLorenzv4 += subjet.v4();
  }
  subjet_hist.sum4Vec->Fill(sumLorenzv4.M(), weight);
}

TopJetHists::TopJetHists(Context & ctx,
                const std::string & dname,
                const unsigned int NumberOfPlottedJets,
                const std::string & collection_): JetHistsBase(ctx, dname), collection(collection_) {

  number = book<TH1F>("number","number of topjets",21, -.5, 20.5);
  topjetid = boost::none;
  alljets = book_jetHist("topjet","",20,1500);
  allsubjets = book_subjetHist("subjet","_subjets",0,500);
  vector<double> maxPt {900,600,400,300};
  string axis_suffix = "topjet";
  string axis_subjetSuffix = "subjets ";
  vector<string> axis_singleSubjetSuffix {"first ","second ","third ","fourth "};
  for(unsigned int i =0; i<NumberOfPlottedJets; i++){
    if(i<4){
      single_jetHists.push_back(book_jetHist(axis_singleSubjetSuffix[i]+axis_suffix,string("_")+to_string(i+1),0,maxPt[i]));
      subjets.push_back(book_subjetHist(axis_subjetSuffix+axis_singleSubjetSuffix[i]+axis_suffix,string("_")+to_string(i+1)+string("_subj"),0,maxPt[i]));
    }
    else{
      single_jetHists.push_back(book_jetHist(to_string(i+1)+string("-th jet"),string("_")+to_string(i+1),20,500));
      subjets.push_back(book_subjetHist(axis_subjetSuffix+to_string(i+1)+string("-th subjet")+axis_suffix,string("_")+to_string(i+1)+string("_subj"),0,maxPt[i]));
    }
  }
  deltaRmin_1 = book<TH1F>("deltaRmin_1", "#Delta R_{min}(first jet,nearest jet)", 40, 0, 8.0);
  deltaRmin_2 = book<TH1F>("deltaRmin_2", "#Delta R_{min}(2nd jet,nearest jet)", 40, 0, 8.0);
  if(!collection.empty()){
    h_topjets = ctx.get_handle<std::vector<TopJet> >(collection);
  }
}

void TopJetHists::add_iTopJetHists(unsigned int UserJet, double minPt, double maxPt, double minPt_sub, double maxPt_sub, const std::string & axisSuffix, const std::string & histSuffix ){
  m_usertopjet.push_back(UserJet); 
  usertopjets.push_back(book_jetHist(axisSuffix,histSuffix, minPt, maxPt));
  usersubjets.push_back(book_subjetHist(axisSuffix+"_sub", histSuffix+"_sub", minPt_sub, maxPt_sub));
}


void TopJetHists::fill(const Event & event){
    auto w = event.weight;
    vector<TopJet> jets = collection.empty() ? *event.topjets : event.get(h_topjets);
    //assert(jets);
    if(topjetid){
      vector<TopJet> help_jets;
      for(const auto jet : jets)
	if(passes_id(jet,event,topjetid)) help_jets.push_back(jet);
      jets.swap(help_jets);
    }
    number->Fill(jets.size(), w);
    for(unsigned int i = 0; i <jets.size(); i++){
      const auto & jet = jets[i];
      fill_jetHist(jet,alljets,w);
      fill_subjetHist(jet,allsubjets,w);
      for (unsigned int m =0; m<m_usertopjet.size(); ++m){ 
        if(m_usertopjet[m] == i){  
          fill_jetHist(jet,usertopjets[m],w);
          fill_subjetHist(jet,usersubjets[m],w);  
        }
      }
      if(i < single_jetHists.size()){
          fill_jetHist(jet,single_jetHists[i],w);  
          fill_subjetHist(jet,subjets[i],w);  
      }
      if(i<2){
        auto next_jet = closestParticle(jet, jets);
        auto drmin = next_jet ? deltaR(jet, *next_jet) : numeric_limits<float>::infinity();
        if(i==0)deltaRmin_1->Fill(drmin, w);
        else deltaRmin_2->Fill(drmin, w);
      }
    }
}

template<typename T>
  bool JetHistsBase::passes_id(const T & object, const Event & event, const boost::optional<std::function<bool (const T &, const Event & )>> & object_id){
  if((*object_id)(object, event))
    return true;
  return false;
}
