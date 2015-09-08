#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"

class MatchingBackground: public uhh2::AnalysisModule {
public:

   explicit MatchingBackground(uhh2::Context & ctx, std::string name_handletopjets, std::string collection);
   
   virtual bool process(uhh2::Event & event) override;
   
   virtual ~MatchingBackground();
   
private:
   uhh2::Event::Handle<std::vector<TopJet> > h_matchedtopjets;
   uhh2::Event::Handle<std::vector<TopJet> > h_topjets;
};
