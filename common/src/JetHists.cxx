#include "UHH2/common/include/JetHists.h"
#include "UHH2/common/include/Utils.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/LorentzVector.h"

#include "TH1F.h"
#include "TH2F.h"

#include <vector>

using namespace uhh2;
using namespace std;


JetHistsBase::JetHistsBase(uhh2::Context & ctx,
			   const std::string & dirname): Hists(ctx, dirname){}

jetHist JetHistsBase::jetHistsBooker(string suffix, double minPt, double maxPt){
  jetHist jet_hist;
  
  string histname;
  if(suffix.compare("")==0)
    histname = suffix;
  else
    histname = string("_")+suffix;

  jet_hist.pt = book<TH1F>((string("pt")+histname).c_str(),(string("p_{T} jet ")+suffix).c_str(),50,minPt,maxPt);
  jet_hist.eta = book<TH1F>((string("eta")+histname).c_str(),(string("#eta jet ")+suffix).c_str(),100,-5,5);
  jet_hist.phi = book<TH1F>((string("phi")+histname).c_str(),(string("#phi jet ")+suffix).c_str(),50,-M_PI,M_PI);
  jet_hist.mass = book<TH1F>((string("mass")+histname).c_str(),(string("M^{jet ")+suffix+string("} [GeV/c^{2}]")).c_str(), 100, 0, 300);
  jet_hist.csv = book<TH1F>((string("csv")+histname).c_str(),(string("csv-disriminator jet ")+suffix).c_str(),50,0,1);

  return jet_hist;
}

template <typename T>
void JetHistsBase::jetHistFiller(T jet, jetHist jet_hist, double  weight){
  jet_hist.pt->Fill(jet.pt(), weight);
  jet_hist.eta->Fill(jet.eta(), weight);
  jet_hist.phi->Fill(jet.phi(), weight);
  jet_hist.mass->Fill(jet.v4().M(), weight);
  jet_hist.csv->Fill(jet.btag_combinedSecondaryVertex(), weight);
}

JetHists::JetHists(Context & ctx,
                   const std::string & dname,
                   const std::string & collection): JetHistsBase(ctx, dname){


    number = book<TH1F>("number","number of jets",21, -.5, 20.5);
    
    alljets = jetHistsBooker("",20,1500);

    vector<double> minPt {20,20,20,20};
    vector<double> maxPt {1500,1000,500,350};
    vector<string> h_suffix {"1","2","3","4"};

    for(unsigned int i =0; i<4; i++){
      single_jetHists.push_back(jetHistsBooker(h_suffix[i],minPt[i],maxPt[i]));
    }
  
    deltaRmin_1 = book<TH1F>("deltaRmin_1", "#Delta R_{min}(first jet,nearest jet)", 40, 0, 2.0);
    deltaRmin_2 = book<TH1F>("deltaRmin_2", "#Delta R_{min}(2nd jet,nearest jet)", 40, 0, 2.0);

    h_jets = ctx.get_handle<std::vector<Jet> >(collection);
}

void JetHists::fill(const Event & event){
    auto w = event.weight;

    const auto jets = (event.get_state(h_jets) == GenericEvent::state::valid)
                      ? &event.get(h_jets) : event.jets;

    assert(jets);
    number->Fill(jets->size(), w);
    

    for(unsigned int i = 0; i <jets->size(); i++){
      const auto & jet = (*jets)[i];

      jetHistFiller(jet,alljets,w);
      
      if(i<4){
	jetHistFiller(jet,single_jetHists.at(i),w);
	
	if(i<2){
	  
	  auto next_jet = closestParticle(jet, *jets);
	  auto drmin = next_jet ? deltaR(jet, *next_jet) : numeric_limits<float>::infinity();
	  
	  //cout<<drmin<<endl;

	  if(i==0)deltaRmin_1->Fill(drmin, w);
	  else deltaRmin_2->Fill(drmin, w);
	}
      }
    }
}

//----------------------------------------------
//----------------------------------------------
//----------------------------------------------
//----------------------------------------------
/** TopJet Histograms
 *
 *
 *
 *
 */

TopJetHists::TopJetHists(Context & ctx,
			 const std::string & dname,
			 const std::string & collection): JetHistsBase(ctx, dname){

  number = book<TH1F>("number","number of jets",21, -.5, 20.5);
  
  alljets = jetHistsBooker("",20,1500);
 
  vector<double> minPt {20,20,20,20};
  vector<double> maxPt {1500,1000,500,350};
  vector<string> h_suffix {"1","2","3","4"};
  
  for(unsigned int i =0; i<4; i++)
    single_jetHists.push_back(jetHistsBooker(h_suffix[i],minPt[i],maxPt[i]));
    
  deltaRmin_1 = book<TH1F>("deltaRmin_1", "#Delta R_{min}(first jet,nearest jet)", 40, 0, 2.0);
  deltaRmin_2 = book<TH1F>("deltaRmin_2", "#Delta R_{min}(2nd jet,nearest jet)", 40, 0, 2.0);

  h_topjets = ctx.get_handle<std::vector<TopJet> >(collection);
  
}

void TopJetHists::fill(const Event & event){
    auto w = event.weight;

    const auto jets = (event.get_state(h_topjets) == GenericEvent::state::valid)
                      ? &event.get(h_topjets) : event.topjets;

    //const auto jets =  event.topjets;

    assert(jets);
    number->Fill(jets->size(), w);
    

    for(unsigned int i = 0; i <jets->size(); i++){
      const auto & jet = (*jets)[i];

      jetHistFiller(jet,alljets,w);
      
      if(i<4){
	jetHistFiller(jet,single_jetHists.at(i),w);  

	if(i<2){
	  
	  auto next_jet = closestParticle(jet, *jets);
	  auto drmin = next_jet ? deltaR(jet, *next_jet) : numeric_limits<float>::infinity();
	  
	  //cout<<drmin<<endl;

	  if(i==0)deltaRmin_1->Fill(drmin, w);
	  else deltaRmin_2->Fill(drmin, w);
	}

      }
    }
}

