#include "UHH2/common/include/NSelections.h"
#include "UHH2/core/include/Event.h"

using namespace uhh2;
using namespace std;

namespace {
    
template<typename T>
bool passes_minmax(const vector<T> & objects, int nmin, int nmax, const Event & event, const boost::optional<std::function<bool (const T &, const Event & )>> & object_id){
    int n_objects = objects.size();
    if(object_id){
        n_objects = 0;
        for(const auto & obj : objects){
            if((*object_id)(obj, event)) ++n_objects;
        }
    }
    return n_objects >= nmin && (nmax < 0 || n_objects <= nmax);
}
    
}


NTauSelection::NTauSelection(int nmin_, int nmax_, const boost::optional<TauId> & tauid_): nmin(nmin_), nmax(nmax_), tauid(tauid_){}

bool NTauSelection::passes(const Event & event){
    return passes_minmax(*event.taus, nmin, nmax, event, tauid);
}
    
NMuonSelection::NMuonSelection(int nmin_, int nmax_, const boost::optional<MuonId> & muid_): nmin(nmin_), nmax(nmax_), muid(muid_){}

bool NMuonSelection::passes(const Event & event){
    return passes_minmax(*event.muons, nmin, nmax, event, muid);
}


NElectronSelection::NElectronSelection(int nmin_, int nmax_, const boost::optional<ElectronId> & eleid_): nmin(nmin_), nmax(nmax_), eleid(eleid_){
}

bool NElectronSelection::passes(const Event & event){
    return passes_minmax(*event.electrons, nmin, nmax, event, eleid);
}

NJetSelection::NJetSelection(int nmin_, int nmax_,
    const boost::optional<JetId> & jetid_,
    const boost::optional<Event::Handle<std::vector<Jet> > > & jetcollection_) :
    nmin(nmin_), nmax(nmax_), jetid(jetid_), jetcollection(jetcollection_){}

bool NJetSelection::passes(const Event & event){
    const auto & jets = jetcollection ? event.get(*jetcollection) : *event.jets;
    return passes_minmax(jets, nmin, nmax, event, jetid);
}

NTopJetSelection::NTopJetSelection(int nmin_, int nmax_,
    const boost::optional<TopJetId> & topjetid_,
    const boost::optional<Event::Handle<std::vector<TopJet> > > & topjetcollection_) :
    nmin(nmin_), nmax(nmax_), topjetid(topjetid_), topjetcollection(topjetcollection_){}

bool NTopJetSelection::passes(const Event & event){
    const auto & jets = topjetcollection ? event.get(*topjetcollection) : *event.topjets;
    return passes_minmax(jets, nmin, nmax, event, topjetid);
}

NPVSelection::NPVSelection(int nmin_, int nmax_,const boost::optional<PrimaryVertexId> & pvid_): nmin(nmin_), nmax(nmax_), pvid(pvid_){
}

bool NPVSelection::passes(const Event & event){
  assert(event.pvs);
  return passes_minmax(*event.pvs, nmin, nmax, event, pvid);
}
