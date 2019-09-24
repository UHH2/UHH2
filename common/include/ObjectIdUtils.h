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
typedef std::function<bool (const Photon &, const uhh2::Event &)> PhotonId;
typedef std::function<bool (const Muon &, const uhh2::Event &)> MuonId;
typedef std::function<bool (const Tau &, const uhh2::Event &)> TauId;
typedef std::function<bool (const TopJet &, const uhh2::Event &)> TopJetId;
typedef std::function<bool (const GenParticle &, const uhh2::Event &)> GenParticleId;
typedef std::function<bool (const GenJet &, const uhh2::Event &)> GenJetId;


/** \brief Cut on minimum pt and maximum |eta| of a particle
 * 
 * This implements the kinematic cut which is often used and imposes a minimum pt
 * requirement and a maximum absolute-eta requirement. As such a cut can be applied to many different particles,
 * it is defined using the Particle base class; note that it can still be applied as
 * MuonId, JetId, etc. as all these inherit from Particle.
 */
class PtEtaCut {
public:
 PtEtaCut(float min_pt_, float max_eta_, float max_pt_ =-1, float min_eta_=-1): min_pt(min_pt_), max_eta(max_eta_), max_pt(max_pt_),min_eta(min_eta_){}

    bool operator()(const Particle & p, const uhh2::Event & ) const{
      return p.pt() > min_pt && (p.pt() < max_pt || max_pt == -1) && std::abs(p.eta()) < max_eta && std::abs(p.eta())> min_eta ;
    }
    
private:
    float min_pt, max_eta, max_pt,min_eta ;
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
 * MuonId muid = AndId<Muon>(PtEtaCut(20.0, 2.4), MuonIdTight());
 * \endcode
 * 
 * For more complicated ids consisting of more than four sub-components, use the 'add' method directly.
 * Note that 'add' can be called several times like this:
 * \code
 *  AndId<Muon> muid;
 *  muid.add(PtEtaCut(20.0, 2.4)).add(MuonIdTight()).add(MuonIso(0.12));
 * \endcode
 */
template<typename T>
class AndId {
public:
    typedef std::function<bool (const T&, const uhh2::Event &)> id_type;
    
    AndId(const id_type & id1, const id_type & id2){
        add(id1);
        add(id2);
    }
    
    AndId(const id_type & id1, const id_type & id2, const id_type & id3){
        add(id1);
        add(id2);
        add(id3);
    }
    
    AndId(const id_type & id1, const id_type & id2, const id_type & id3, const id_type & id4){
        add(id1);
        add(id2);
        add(id3);
        add(id4);
    }
    
    AndId & add(const id_type & id){
        ids.push_back(id);
        return *this;
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

/** \brief Construct a new object id, taking the logical or of several other ids.
 * 
 * Similar to above, except that this Id requires only one of the implemented ids to be passed in
 * order to pass 'true'.
 * Can e.g. be used for a GenParticleId.
 */
template<typename T>
class OrId {
public:
    typedef std::function<bool (const T&, const uhh2::Event &)> id_type;
    
    OrId(const id_type & id1, const id_type & id2){
        add(id1);
        add(id2);
    }
    
    OrId(const id_type & id1, const id_type & id2, const id_type & id3){
        add(id1);
        add(id2);
        add(id3);
    }
    
    OrId(const id_type & id1, const id_type & id2, const id_type & id3, const id_type & id4){
        add(id1);
        add(id2);
        add(id3);
        add(id4);
    }
    
    OrId & add(const id_type & id){
        ids.push_back(id);
        return *this;
    }
    
    bool operator()(const T & obj, const uhh2::Event & event) const {
        for(const auto & id : ids){
            if(id(obj, event)) return true;
        }
        return false;
    }
    
private:
    std::vector<id_type> ids;
};

/** \brief Construct a new object id that takes another Id as input and returns 'false' if the 
 * object passes that Id.
 */
template<typename T>
class VetoId {
public:
    typedef std::function<bool (const T&, const uhh2::Event &)> id_type;
    
    VetoId(const id_type & id): id_(id) {}
    
    bool operator()(const T & obj, const uhh2::Event & event) const {
        if(id_(obj, event)) return false;
        return true;
    }
    
private:
    id_type id_;
};

