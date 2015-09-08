#include "UHH2/PuppiAndTopTagging/include/MatchingSignal.h" 

using namespace uhh2;
using namespace std;

MatchingSignal::MatchingSignal(Context & ctx, std::string name_handletopjets, std::string collection) {
   
   std::string name_handle = "matched_topjets_" + collection;
   h_matchedtopjets = ctx.declare_event_output<vector<TopJet> >(name_handle); 
   h_ttbargen = ctx.get_handle<TTbarGen>("ttbargen");
   h_topjets = ctx.get_handle<std::vector<TopJet> >(name_handletopjets);
  
}
   
MatchingSignal::~MatchingSignal() {}



bool MatchingSignal::process(uhh2::Event & event) {
 
   if(!event.is_valid(h_ttbargen)){
      return false;
   }
   const auto & ttbargen = event.get(h_ttbargen);
   
   vector<TopJet> topjets =event.get(h_topjets);
   
   int index_Top=100;
   int index_Antitop=100;
   double DeltaR_TopMin = 100;
   double DeltaR_AntitopMin = 100;
   double DeltaR_Top = 100;
   double DeltaR_Antitop = 100;
   vector<TopJet> matchedtopjets; 
 
   for (unsigned int i=0; i<topjets.size();i++)
      {
         TopJet tp = topjets.at(i);
         DeltaR_Top = deltaR(ttbargen.Top(),tp);
            if (DeltaR_Top<DeltaR_TopMin) 
               {
                  DeltaR_TopMin = DeltaR_Top;
                  index_Top = i;
               }
            DeltaR_Antitop = deltaR(ttbargen.Antitop(),tp);
            if (DeltaR_Antitop<DeltaR_AntitopMin) 
               {
                  DeltaR_AntitopMin= DeltaR_Antitop;
                  index_Antitop = i;
               }
      }
 
   if (DeltaR_AntitopMin<2.0 && ttbargen.Antitop().pt()>150 && ttbargen.IsAntiTopHadronicDecay() ) matchedtopjets.emplace_back(topjets.at(index_Antitop)); //TO DO:::matching und pT muss fuer jede collection angepasstr werden!!!!!!
   if (DeltaR_TopMin<2.0 && ttbargen.Top().pt()>150 && ttbargen.IsTopHadronicDecay()) matchedtopjets.emplace_back(topjets.at(index_Top));
   

   event.set(h_matchedtopjets, move(matchedtopjets));
   return true;                                   
}
             
