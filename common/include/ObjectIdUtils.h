#pragma once

#include "UHH2/core/include/Event.h"

// this file defines some utility classes for the object ids (ElectronIds, MuonIds, etc.)
// and generic ids which can be applied to more than one object.


/** \brief Typedefs for object ids
 * 
 * Anything that is like a function return a boolean, given the object and the event can
 * be used a object id. We use the std::function here; these typedefs make it easier
 * to read what kind of argument is expected at some places.
 */
typedef std::function<bool (const Jet &, const uhh2::Event &)> JetId;
typedef std::function<bool (const Electron &, const uhh2::Event &)> ElectronId;
typedef std::function<bool (const Muon &, const uhh2::Event &)> MuonId;
typedef std::function<bool (const Tau &, const uhh2::Event &)> TauId;
typedef std::function<bool (const TopJet &, const uhh2::Event &)> TopJetId;

/** \brief Cut on minimum pt and maximum |eta| of a particle
 * 
 * This implements the kienamtic cut which is often used and imposes a minimum pt
 * requirement and a maximum eta requirement. This can be applied to many different particles
 * and is thus defined as template class where the template parameter is the type of object.
 * Usually, you can use a typedef which specifies the template parameter, e.g. JetPtEtaCut for jets,
 * ElectronPtEtaCut for electrons, etc. See below the declaration of this class for the available
 * typedefs.
 */
template<typename T>
class PtEtaCut {
public:
    PtEtaCut(float min_pt_, float max_eta_): min_pt(min_pt_), max_eta(max_eta_){}
    
    bool operator()(const T & obj, const uhh2::Event & ) const{
        return obj.pt() > min_pt && std::abs(obj.eta()) < max_eta;
    }
    
private:
    float min_pt, max_eta;
};

typedef PtEtaCut<Jet> JetPtEtaCut;
typedef PtEtaCut<Electron> ElectronPtEtaCut;
typedef PtEtaCut<Muon> MuonPtEtaCut;
typedef PtEtaCut<Tau> TauPtEtaCut;


/** \brief Construct a new object id, taking the logical and of several other ids.
 * 
 * To use an AndId for muons cutting both on kinematics and using the tight id, use like this:
 * \code
 * MuonId muid = AndId<Muon>(MuonPtEtaCut(20.0, 2.4), MuonIdTight());
 * \endcode
 * 
 * For more complicated ids consisting of more than two sub-components, use the 'add' method to add more.
 */
template<typename T>
class AndId {
public:
    typedef std::function<bool (const T&, const uhh2::Event &)> id_type;
    
    AndId(const id_type & id1, const id_type & id2){
        add(id1);
        add(id2);
    }
    
    void add(const id_type & id){
        ids.push_back(id);
    }
    
    bool operator()(const T & obj, const uhh2::Event & event) const {
        for(const auto & id : ids){
            if(!id(obj, event)) return false;
        }
        return true;
    }
    
private:
    std::vector<id_type> ids;
};


