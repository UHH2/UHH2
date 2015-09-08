#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/common/include/TTbarGen.h"


class MatchingSignal: public uhh2::AnalysisModule {
public:

   explicit MatchingSignal(uhh2::Context & ctx, std::string name_handletopjets, std::string collection);
   
   virtual bool process(uhh2::Event & event) override;
   
   virtual ~MatchingSignal();
   
private:
   uhh2::Event::Handle<std::vector<TopJet> > h_matchedtopjets;
   uhh2::Event::Handle<TTbarGen> h_ttbargen;
   uhh2::Event::Handle<std::vector<TopJet> > h_topjets;
};
