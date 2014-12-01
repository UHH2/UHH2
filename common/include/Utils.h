#pragma once

#include "UHH2/core/include/Event.h"

/** Find the  next jet for particle p
 * 
 * Will return NULL if (and only if) jets is empty.
 */
const Jet * nextJet(const Particle  & p, const std::vector<Jet> & jets);

/** Relative transverse momentum of the particle p with respect to reference_axis
 * 
 * note: can use reference_axis = nextJet(p, *event.jets) for the 'usual' ptrel, assuming that the
 * jets have the correct filter(!). In case reference_axis is NULL or or the 0-three-vector, 0.0 is returned.
 */
double pTrel(const Particle  & p, const Particle * reference_axis);
