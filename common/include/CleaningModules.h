#pragma once


/* This file define 'cleaning' modules, i.e. modules which throw away
 * parts of some collection in the event by applying kinematic or id criteria.
 */

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/common/include/ObjectIdUtils.h"

/// Keep only jets fulfilling a certain JetId / with a minimum pt and maximum |eta|
class JetCleaner: public uhh2::AnalysisModule {
public:
    
    explicit JetCleaner(uhh2::Context & ctx, const JetId & jet_id_, std::string const & label_ = "jets");
    JetCleaner(uhh2::Context & ctx, float minpt, float maxeta, std::string const & label_ = "jets");
    virtual bool process(uhh2::Event & event) override;
    
private:
    JetId jet_id;
    uhh2::Event::Handle<std::vector<Jet>> hndl;
};


/** \brief Keep only primary vertices fulfilling an id.
 * 
 * The default id is the StandardPrimaryVertexId, which is the id used in 8TeV
 * (for details, see definiction of StandardPrimaryVertexId in ObjectUtils.h).
 */
class PrimaryVertexCleaner: public uhh2::AnalysisModule {
public:
    explicit PrimaryVertexCleaner(const PrimaryVertexId & vtx_id = StandardPrimaryVertexId());
    virtual bool process(uhh2::Event & event) override;
    
private:
    PrimaryVertexId vtx_id;
};

/** \brief Keep only muon passing a given muon id
 * 
 * The muon id is passed as function with the signature
 * \code
 * bool muonid(const Muon & muon, const Event & event);
 * \endcode
 * Those muons for which this function returns true are kept, the others are discarded.
 * See MuonIds.h for some ready muon ids.
 */
class MuonCleaner : public uhh2::AnalysisModule {
public:
    
    explicit MuonCleaner(const MuonId & muon_id);
    virtual bool process(uhh2::Event & event) override;
    
private:
    MuonId muon_id;
};


/** \brief Keep only electrons passing a given electron id
 * 
 * See comments for MuonCleaner.
 */
class ElectronCleaner : public uhh2::AnalysisModule {
public:
    
    explicit ElectronCleaner(const ElectronId & ele_id);
    virtual bool process(uhh2::Event & event) override;
    
private:
    ElectronId ele_id;
};

/** \brief Keep only taus passing a given tau id
 *
 * See comments for MuonCleaner.
 */
class TauCleaner : public uhh2::AnalysisModule {
public:

    explicit TauCleaner(const TauId & tau_id);
    virtual bool process(uhh2::Event & event) override;

private:
    TauId tau_id;
};



/** \brief Keep only taus passing a given tau id
 *
 * See comments for MuonCleaner.
 */
class TopJetCleaner : public uhh2::AnalysisModule {
public:

    explicit TopJetCleaner(uhh2::Context & ctx, const TopJetId & jet_id_, std::string const & label_ = "topjets");
    virtual bool process(uhh2::Event & event) override;

private:
    TopJetId topjet_id;
    uhh2::Event::Handle<std::vector<TopJet>> hndl;
};

class JetMuonOverlapRemoval: public uhh2::AnalysisModule {
   public:
      explicit JetMuonOverlapRemoval(double deltaRmax);
      virtual bool process(uhh2::Event & event) override;
      
   private:
      double deltaRmin_;
 };
