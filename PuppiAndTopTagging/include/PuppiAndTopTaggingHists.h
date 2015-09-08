#pragma once

#include "UHH2/core/include/Hists.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/common/include/TopJetIds.h"


/**  \brief Example class for booking and filling histograms
 * 
 * NOTE: This class uses the 'hist' method to retrieve histograms.
 * This requires a string lookup and is therefore slow if you have
 * many histograms. Therefore, it is recommended to use histogram
 * pointers as member data instead, like in 'common/include/ElectronHists.h'.
 */
class PuppiAndTopTaggingHists: public uhh2::Hists {
public:
    // use the same constructor arguments as Hists for forwarding:
   PuppiAndTopTaggingHists(uhh2::Context & ctx, const std::string & dirname,const std::string & collection = "");
   
   virtual void fill(const uhh2::Event & ev) override;
   
   void set_TopJetId(TopJetId my_topjetid) {topjetid = my_topjetid;}
   
   bool passes_id(TopJet & TopJet, const uhh2::Event & event, TopJetId & object_id);


protected:
   TH1F *matched, *tagged;

   TopJetId topjetid;

   uhh2::Event::Handle<std::vector<TopJet> > h_matchedtopjets;
};


