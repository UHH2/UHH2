#include "UHH2/PuppiAndTopTagging/include/PuppiAndTopTaggingHists.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/common/include/TopJetIds.h"

#include "TH1F.h"
#include <iostream>

using namespace std;
using namespace uhh2;

PuppiAndTopTaggingHists::PuppiAndTopTaggingHists(Context & ctx, const string & dirname, const std::string & collection): Hists(ctx, dirname){
   
   matched = book<TH1F>( "MatchedTopJets", "p_{T}", 100, 0,2000);
   tagged = book<TH1F>( "TaggedTopJets", "p_{T} (tagged)", 100, 0,2000);
   
   std::string name_handle = "matched_topjets_" +collection;
   h_matchedtopjets = ctx.get_handle<vector<TopJet> >(name_handle);
}


void PuppiAndTopTaggingHists::fill(const Event & event){
   
   double weight = event.weight;
   vector<TopJet> matchedtopjets = event.get(h_matchedtopjets);
   
   for (unsigned int i=0; i<matchedtopjets.size(); i++)
      {
         TopJet tp = matchedtopjets.at(i);
         matched->Fill(tp.pt(),weight);
         if (passes_id(tp, event, topjetid)) tagged->Fill(tp.pt(),weight); 
      }
}


bool PuppiAndTopTaggingHists::passes_id(TopJet & TopJet, const uhh2::Event & event, TopJetId & object_id){
  if((object_id)(TopJet, event)) return true;
  return false;
}


