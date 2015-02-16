#pragma once

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
 * If fname is a relative path, these directories are tried and the first wins:
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
