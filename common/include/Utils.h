#pragma once

#include "UHH2/core/include/Event.h"

/** Find the jet with smallest DeltaR to particle p
 * 
 * Returns NULL if (and only if) jets is empty.
 */
const Jet * nextJet(const Particle  & p, const std::vector<Jet> & jets);

/** Relative transverse momentum of the particle p with respect to reference_axis
 * 
 * note: can use reference_axis = nextJet(p, *event.jets) for the 'usual' ptrel, assuming that the
 * jets have the correct filter(!).
 * 
 * In case reference_axis is NULL or or the 0-three-vector, 0.0 is returned.
 */
double pTrel(const Particle  & p, const Particle * reference_axis);


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
