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
typedef std::function<bool (const PrimaryVertex &, const uhh2::Event &)> PrimaryVertexId;
typedef std::function<bool (const Electron &, const uhh2::Event &)> ElectronId;
typedef std::function<bool (const Muon &, const uhh2::Event &)> MuonId;
typedef std::function<bool (const Tau &, const uhh2::Event &)> TauId;
typedef std::function<bool (const TopJet &, const uhh2::Event &)> TopJetId;

/** \brief Cut on minimum pt and maximum |eta| of a particle
 * 
 * This implements the kinematic cut which is often used and imposes a minimum pt
 * requirement and a maximum absolute-eta requirement. As such a cut can be applied to many different particles,
 * it is defined using the Particle base class; note that it can still be applied as
 * MuonId, JetId, etc. as all these inherit from Particle.
 */
class PtEtaCut {
public:
    PtEtaCut(float min_pt_, float max_eta_): min_pt(min_pt_), max_eta(max_eta_){}
    
    bool operator()(const Particle & p, const uhh2::Event & ) const{
        return p.pt() > min_pt && std::abs(p.eta()) < max_eta;
    }
    
private:
    float min_pt, max_eta;
};

/// The standard primary vertex id, i.e. ndof >= 4, |z| < 24cm, rho < 2cm
class StandardPrimaryVertexId {
public:
    bool operator()(const PrimaryVertex & pv, const uhh2::Event &) const{
        return pv.ndof() >= 4.0 && std::abs(pv.z()) < 24.0 && pv.rho() < 2.0;
    }
};


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


