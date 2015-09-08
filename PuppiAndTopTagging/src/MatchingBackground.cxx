#include "UHH2/PuppiAndTopTagging/include/MatchingBackground.h" 
#include "UHH2/common/include/Utils.h" 

using namespace uhh2;
using namespace std;

MatchingBackground::MatchingBackground(Context & ctx, std::string name_handletopjets, std::string collection) {
   
   std::string name_handle = "matched_topjets_" + collection;
   h_matchedtopjets = ctx.declare_event_output<vector<TopJet> >(name_handle); 
   h_topjets = ctx.get_handle<std::vector<TopJet> >(name_handletopjets);
}
   
MatchingBackground::~MatchingBackground() {}



bool MatchingBackground::process(uhh2::Event & event) {
   
   //matching fake rate
   double DeltaR = 100;
   double DeltaRmin = 100;
   int index_topjet = 100;
   vector<GenParticle> genp_QuarksGluon;
   vector<TopJet> matchedtopjets; 
   
   vector<TopJet> topjets =event.get(h_topjets);

   for (const auto & genp: *event.genparticles) 
      {
         if ((abs(genp.pdgId())<=5 || genp.pdgId()==21)) genp_QuarksGluon.push_back(genp);
      }
   sort_by_pt<GenParticle>(genp_QuarksGluon);
   
   GenParticle genp = genp_QuarksGluon.at(0);
  
   for(unsigned int i = 0; i <topjets.size(); i++){
      {
         TopJet tp = topjets.at(i);
         DeltaR = deltaR(genp, tp);
         if (DeltaR < DeltaRmin) 
            {
               DeltaRmin = DeltaR;
               index_topjet = i;
            }
      }
   }
   if (DeltaRmin<2.0 && genp.pt()>150) matchedtopjets.emplace_back(topjets.at(index_topjet));//TO DO:::matching und pT muss fuer jede collection angepasstr werden!!!
   
   event.set(h_matchedtopjets, move(matchedtopjets));
   return true;                                   
}
             
