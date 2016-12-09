#include "UHH2/common/include/JetHists.h"
#include "UHH2/common/include/Utils.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/LorentzVector.h"

#include "TH1F.h"
#include "TH2F.h"

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

// JetHists
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
    if (!collection.empty() && !event.is_valid(h_jets)){
      cerr<<collection<<" is invalid. Going to abort from JetHists class"<<endl;
      cerr<<" GenJets are stored as Particle which is not a Jet"<<endl;
      assert(1==0);
    }
    vector<Jet> jets = collection.empty() ?  *event.jets : event.get(h_jets);
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

JetHistsBase::jetHist TopJetHists::book_topJetHist(const std::string & axisSuffix, const std::string & histSuffix, double minPt, double maxPt) {
  auto jet_hist = book_jetHist(axisSuffix, histSuffix, minPt, maxPt);
  jet_hist.mvahiggsdiscr = book<TH1F>("mvahiggsdiscr"+histSuffix,"mva-higgs-disriminator "+axisSuffix,50,0,1);
  jet_hist.prunedmass = book<TH1F>("mass_pruned"+histSuffix,"M^{ "+axisSuffix+"}_{pruned} [GeV/c^{2}]", 100, 0, 300);
  jet_hist.subjet_sum_mass = book<TH1F>("mass_subjet_sum"+histSuffix,"M^{ "+axisSuffix+"}_{subjet sum} [GeV/c^{2}]", 100, 0, 300);
  return jet_hist;
}

void TopJetHists::fill_topJetHist(const TopJet & jet, JetHistsBase::jetHist & jet_hist, double  weight) {
  fill_jetHist(jet, jet_hist, weight);
  LorentzVector subjet_sum;
  for (const auto s : jet.subjets()) {
    subjet_sum += s.v4();
  }
  jet_hist.mvahiggsdiscr->Fill(jet.mvahiggsdiscr(), weight);
  jet_hist.prunedmass->Fill(jet.prunedmass(), weight);
  jet_hist.subjet_sum_mass->Fill(subjet_sum.M(), weight);
}

TopJetHists::TopJetHists(Context & ctx,
                const std::string & dname,
                const unsigned int NumberOfPlottedJets,
                const std::string & collection_): JetHistsBase(ctx, dname), collection(collection_) {

  number = book<TH1F>("number","number of topjets",21, -.5, 20.5);
  topjetid = boost::none;
  alljets = book_topJetHist("topjet","",20,1500);
  allsubjets = book_subjetHist("subjet","_subjets",0,500);
  vector<double> maxPt {900,600,400,300};
  string axis_suffix = "topjet";
  string axis_subjetSuffix = "subjets ";
  vector<string> axis_singleSubjetSuffix {"first ","second ","third ","fourth "};
  for(unsigned int i =0; i<NumberOfPlottedJets; i++){
    if(i<4){
      single_jetHists.push_back(book_topJetHist(axis_singleSubjetSuffix[i]+axis_suffix,string("_")+to_string(i+1),0,maxPt[i]));
      subjets.push_back(book_subjetHist(axis_subjetSuffix+axis_singleSubjetSuffix[i]+axis_suffix,string("_")+to_string(i+1)+string("_subj"),0,maxPt[i]));
    }
    else{
      single_jetHists.push_back(book_topJetHist(to_string(i+1)+string("-th jet"),string("_")+to_string(i+1),20,500));
      subjets.push_back(book_subjetHist(axis_subjetSuffix+to_string(i+1)+string("-th subjet")+axis_suffix,string("_")+to_string(i+1)+string("_subj"),0,maxPt[i]));
    }
  }
  deltaRmin_1 = book<TH1F>("deltaRmin_1", "#Delta R_{min}(first jet,nearest jet)", 40, 0, 8.0);
  deltaRmin_2 = book<TH1F>("deltaRmin_2", "#Delta R_{min}(2nd jet,nearest jet)", 40, 0, 8.0);
  tau32 = book<TH1F>("tau32", "#tau_{3}/#tau_{2}", 50, 0, 1.0);
  tau21 = book<TH1F>("tau21", "#tau_{2}/#tau_{1}", 50, 0, 1.0);
  deltaR_ak4jet= book<TH1F>("deltaR_ak4jet", "#Delta R(jet,ak4 jet)", 40, 0, 8.0);
  invmass_topjetak4jet = book<TH1F>("invmass_topjetak4jet", "invariant mass(jet,ak4 jet)", 100, 0, 1000);
  deltaR_lepton= book<TH1F>("deltaR_lepton", "#Delta R(jet,lepton)", 40, 0, 8.0);
  HTT_mass = book<TH1F>("HTT_mass", "HTT mass", 100, 0, 1000);
  fRec = book<TH1F>("fRec", "fRec", 50,0,1); 

  if(!collection.empty()){
    h_topjets = ctx.get_handle<std::vector<TopJet> >(collection);
  }
}

void TopJetHists::add_iTopJetHists(unsigned int UserJet, double minPt, double maxPt, double minPt_sub, double maxPt_sub, const std::string & axisSuffix, const std::string & histSuffix ){
  m_usertopjet.push_back(UserJet); 
  usertopjets.push_back(book_topJetHist(axisSuffix,histSuffix, minPt, maxPt));
  usersubjets.push_back(book_subjetHist(axisSuffix+"_sub", histSuffix+"_sub", minPt_sub, maxPt_sub));
}


void TopJetHists::fill(const Event & event){
    auto w = event.weight;
    vector<TopJet> jets = collection.empty() ? *event.topjets : event.get(h_topjets);
    vector<Jet> ak4jets = *event.jets;

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
      fill_topJetHist(jet,alljets,w);
      fill_subjetHist(jet,allsubjets,w);
      tau32->Fill(jet.tau3()/jet.tau2(),w);
      tau21->Fill(jet.tau2()/jet.tau1(),w);
      for (unsigned int m =0; m<m_usertopjet.size(); ++m){ 
        if(m_usertopjet[m] == i){  
          fill_topJetHist(jet,usertopjets[m],w);
          fill_subjetHist(jet,usersubjets[m],w);  
        }
      }
      if(i < single_jetHists.size()){
          fill_topJetHist(jet,single_jetHists[i],w);  
          fill_subjetHist(jet,subjets[i],w);  
      }
      if(i<2){
        auto next_jet = closestParticle(jet, jets);
        auto drmin = next_jet ? deltaR(jet, *next_jet) : numeric_limits<float>::infinity();
        if(i==0)deltaRmin_1->Fill(drmin, w);
        else deltaRmin_2->Fill(drmin, w);
      }
    
      for (unsigned int j = 0; j <ak4jets.size(); j++)
         {
            const auto & ak4jet = ak4jets[i];
            double deltaRtopjetak4jet = deltaR(jet, ak4jet);
            deltaR_ak4jet->Fill(deltaRtopjetak4jet,w);
            invmass_topjetak4jet->Fill((jet.v4()+ak4jet.v4()).M(),w);
         }
      if (jet.has_tag(jet.tagname2tag("mass"))) HTT_mass ->Fill(jet.get_tag(jet.tagname2tag("mass")),w);
      if (jet.has_tag(jet.tagname2tag("fRec"))) fRec ->Fill(jet.get_tag(jet.tagname2tag("fRec")),w);
  
      vector<Muon> muons = *event.muons;
      for (unsigned int j = 0; j <muons.size(); j++){
         double deltaRtjlepton = deltaR(jet, muons[j]);
         deltaR_lepton->Fill(deltaRtjlepton, w);
      }
      vector<Electron> electrons = *event.electrons;
      for (unsigned int j = 0; j <electrons.size(); j++){
         double deltaRtjlepton = deltaR(jet, electrons[j]);
         deltaR_lepton->Fill(deltaRtjlepton, w);
      }
      
    }
  
}

template<typename T>
  bool JetHistsBase::passes_id(const T & object, const Event & event, const boost::optional<std::function<bool (const T &, const Event & )>> & object_id){
  if((*object_id)(object, event))
    return true;
  return false;
}

//Btag stuff

BTagMCEfficiencyHists::BTagMCEfficiencyHists(
  uhh2::Context & ctx,
  const std::string & dirname,
  const CSVBTag::wp & working_point,
  const std::string & jets_handle_name
):
  Hists(ctx, dirname),
  btag_(CSVBTag(working_point)),
  hist_b_passing_(   book<TH2F>("BTagMCEffFlavBPassing",    ";jet pt;jet eta", BTagMCEffBinsPt.size()-1, BTagMCEffBinsPt.data(), BTagMCEffBinsEta.size()-1, BTagMCEffBinsEta.data())),
  hist_b_total_(     book<TH2F>("BTagMCEffFlavBTotal",      ";jet pt;jet eta", BTagMCEffBinsPt.size()-1, BTagMCEffBinsPt.data(), BTagMCEffBinsEta.size()-1, BTagMCEffBinsEta.data())),
  hist_c_passing_(   book<TH2F>("BTagMCEffFlavCPassing",    ";jet pt;jet eta", BTagMCEffBinsPt.size()-1, BTagMCEffBinsPt.data(), BTagMCEffBinsEta.size()-1, BTagMCEffBinsEta.data())),
  hist_c_total_(     book<TH2F>("BTagMCEffFlavCTotal",      ";jet pt;jet eta", BTagMCEffBinsPt.size()-1, BTagMCEffBinsPt.data(), BTagMCEffBinsEta.size()-1, BTagMCEffBinsEta.data())),
  hist_udsg_passing_(book<TH2F>("BTagMCEffFlavUDSGPassing", ";jet pt;jet eta", BTagMCEffBinsPt.size()-1, BTagMCEffBinsPt.data(), BTagMCEffBinsEta.size()-1, BTagMCEffBinsEta.data())),
  hist_udsg_total_(  book<TH2F>("BTagMCEffFlavUDSGTotal",   ";jet pt;jet eta", BTagMCEffBinsPt.size()-1, BTagMCEffBinsPt.data(), BTagMCEffBinsEta.size()-1, BTagMCEffBinsEta.data())),
  h_topjets_(ctx.get_handle<vector<TopJet>>(jets_handle_name)),
  h_jets_(   ctx.get_handle<vector<Jet>>(   jets_handle_name))
{}

void BTagMCEfficiencyHists::fill(const Event & event)
{
  if (event.is_valid(h_topjets_)) {
    do_fill(event.get(h_topjets_), event);
  } else {
    assert(event.is_valid(h_jets_));
    TopJet tj;
    tj.set_subjets(event.get(h_jets_));
    do_fill(vector<TopJet>({tj}), event);
  }
}

void BTagMCEfficiencyHists::do_fill(const std::vector<TopJet> & jets, const Event & event)
{
  for (const auto & topjet : jets) { for (const auto & jet : topjet.subjets()) {

    auto flav = jet.hadronFlavor();
    bool is_tagged = btag_(jet, event);
    float pt = jet.pt(), eta = jet.eta(), w = event.weight;

    if (flav == 5) {
      hist_b_total_->Fill(pt, eta, w);
      if (is_tagged) {
        hist_b_passing_->Fill(pt, eta, w);
      }
    } else if (flav == 4) {
      hist_c_total_->Fill(pt, eta, w);
      if (is_tagged) {
        hist_c_passing_->Fill(pt, eta, w);
      }
    } else {
      hist_udsg_total_->Fill(pt, eta, w);
      if (is_tagged) {
        hist_udsg_passing_->Fill(pt, eta, w);
      }
    }

  }}
}
