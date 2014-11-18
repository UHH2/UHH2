#pragma once


/* This file define 'cleaning' modules, i.e. modules which throw away
 * parts of some collection in the event by applying kinematic or id criteria.
 */

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"
#include <functional>

/// Keep only jets with a minimum pt and maximum |eta|; sort jets in pt.
class JetCleaner: public uhh2::AnalysisModule {
public:
    JetCleaner(double minpt, double maxeta);
    virtual bool process(uhh2::Event & event) override;
    
private:
    double minpt, maxeta;
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
    typedef std::function<bool (const Muon &, const uhh2::Event &)> muon_id_type;
    
    explicit MuonCleaner(const  muon_id_type & muon_id);
    virtual bool process(uhh2::Event & event) override;
    
private:
    muon_id_type muon_id;
};


/** \brief Keep only electrons passing a given electron id
 * 
 * See comments for MuonCleaner.
 */
class ElectronCleaner : public uhh2::AnalysisModule {
public:
    typedef std::function<bool (const Electron &, const uhh2::Event &)> ele_id_type;
    
    explicit ElectronCleaner(const  ele_id_type & ele_id);
    virtual bool process(uhh2::Event & event) override;
    
private:
    ele_id_type ele_id;
};
