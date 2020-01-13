#pragma once

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"
#include <limits>

/** Find the particle (from particles) with smallest DeltaR to particle p
 * Prevents comparing particle to itself (if p is from the same collection) by checking pointer addresses
 * Returns NULL if (and only if) particles is empty.
 */
template<typename T>
const T * closestParticle(const Particle  & p, const std::vector<T> & particles){
    double deltarmin = std::numeric_limits<double>::infinity();
    const T* next=0;
    for(unsigned int i=0; i<particles.size(); ++i) {
        const T & pi = particles[i];
        double dr = uhh2::deltaR(pi, p);
        if(dr < deltarmin && &pi != &p) {
            deltarmin = dr;
            next = &pi;
        }
    }
    return next;
}

const Jet * nextJet(const Particle  & p, const std::vector<Jet> & jets);

/** Relative transverse momentum of the particle p with respect to reference_axis
 *
 * note: can use reference_axis = nextJet(p, *event.jets) for the 'usual' ptrel, assuming that the
 * jets have the correct filter(!).
 *
 * In case reference_axis is NULL or the 0-three-vector, 0.0 is returned.
 */
double pTrel(const Particle  & p, const Particle * reference_axis);


/** return a pair of (Delta R, pt_rel) values for Particle p w.r.t. the next jet in jets.
 *
 * Returns (infinity, infinity) if jets is empty.
 */
std::pair<double, double> drmin_pTrel(const Particle & p, const std::vector<Jet> & jets);


/** Locate a file, searching in several standard locations.
 *
 * If the file is not found, a runtime_error is thrown with a detailed error
 * message.
 *
 * If fname is an absolute name (=starting with '/'), no file resolution is done, just the check
 * whether the file exists (and exception throwing if not).
 *
 * If fname is a relative path, it first checks if the relative path exists.
 * If it doesn't, then these directories are tried and the first wins:
 *  1. $CMSSW_BASE/src/UHH2/
 *  2. $CMSSW_BASE/src/
 *  3. $SFRAME_DIR/UHH2/
 *  4. $SFRAME_DIR/
 */
std::string locate_file(const std::string & fname);



/** Sort vector of Particles descndeing in pT
 *
 */
template<typename P>
inline void sort_by_pt(std::vector<P> & particles){
    std::sort(particles.begin(), particles.end(), [](const P & p1, const P & p2){return p1.pt() > p2.pt();});
}

template<typename P>
inline void sort_by_pt(std::vector<P*> & particles){
    std::sort(particles.begin(), particles.end(), [](const P* p1, const P* p2){return p1->pt() > p2->pt();});
}

/** common code to filter out objects from a collection according to an object id
 *
 */
template<typename T>
inline void clean_collection(std::vector<T> & objects, const uhh2::Event & event, const std::function<bool (const T &, const uhh2::Event &)> obj_id){
    std::vector<T> result;
    for(const auto & obj : objects){
        if(obj_id(obj, event)){
            result.push_back(obj);
        }
    }
    std::swap(result, objects);
}

/** invariant mass of a LorentzVector, but save for timelike / spacelike vectors
 *
 */
float inv_mass_safe(const LorentzVector&);

/**
 * Year enum to handle generic switching of year-specific bits of code
 */
enum class Year {
    is2016v2,
    is2016v3,
    is2017v1,
    is2017v2,
    is2017UL,
    is2018UL,
    is2018
};

/* Map from Year to string */
const std::map<Year, std::string> year_str_map = {
    {Year::is2016v2, "2016v2"},
    {Year::is2016v3, "2016v3"},
    {Year::is2017v1, "2017v1"},
    {Year::is2017v2, "2017v2"},
    {Year::is2017UL, "2017UL"},
    {Year::is2018UL, "2018UL"},
    {Year::is2018,   "2018"},
};
// TODO: inverse map?

/* Get Year enum from dataset_version in XML config */
Year extract_year(const uhh2::Context & ctx);


/**
 * Run period names for each year
 */
const std::vector<std::string> runPeriods2016 = {"B", "C", "D", "E", "F", "G", "H"};

const std::vector<std::string> runPeriods2017 = {"B", "C", "D", "E", "F"};

const std::vector<std::string> runPeriods2018 = {"A", "B", "C", "D"};

/**
 * Map run periods to run numbers for each year
 *
 * All pairs for a run period are inclusive of both lower & upper numbers
 */
const std::unordered_map<std::string, std::map<std::string, std::pair<int, int>>>
  run_number_map = {
    // taken from https://twiki.cern.ch/twiki/bin/view/CMS/PdmV2016Analysis
    { "2016", {
        { "A", std::pair(-1, -1) }, // dummy ones for consistency
        { "B", std::pair(272007, 275376) },
        { "C", std::pair(275657, 276283) },
        { "D", std::pair(276315, 276811) },
        { "E", std::pair(276831, 277420) },
        { "F", std::pair(277772, 278808) },
        { "G", std::pair(278820, 280385) },
        { "H", std::pair(280919, 284044) },
    }},
    // taken from https://twiki.cern.ch/twiki/bin/view/CMS/PdmV2017Analysis
    { "2017", {
        { "A", std::pair(-1, -1) },
        { "B", std::pair(297020, 299329) },
        { "C", std::pair(299337, 302029) },
        { "D", std::pair(302030, 303434) },
        { "E", std::pair(303435, 304826) },
        { "F", std::pair(304911, 306462) },
        { "G", std::pair(-1, -1) },
        { "H", std::pair(-1, -1) },
    }},
    // taken from https://twiki.cern.ch/twiki/bin/view/CMS/PdmV2018Analysis
    { "2018", {
        { "A", std::pair(315252, 316995) },
        { "B", std::pair(316998, 319312) },
        { "C", std::pair(319313, 320393) },
        { "D", std::pair(320394, 325273) },
        { "E", std::pair(-1, -1) },
        { "F", std::pair(-1, -1) },
        { "G", std::pair(-1, -1) },
        { "H", std::pair(-1, -1) },
    }},
};
