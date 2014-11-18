//  Njettiness Package
//  Version 0.4.1 (January 22, 2012)
//  Questions/Comments?  jthaler@jthaler.net

// Copyright (c) 2011-12, Jesse Thaler, Ken Van Tilburg, and Christopher K.
// Vermilion
//
//----------------------------------------------------------------------
// This file is part of the N-jettiness package ("N-jettiness").
//
// This file has been modified to run with the SFrame analysis. 
// It's original version comes from SpartyJet.
//----------------------------------------------------------------------


#ifndef __NSUBJETTINESS_HH__
#define __NSUBJETTINESS_HH__

#include "Njettiness.h"

#include "fastjet/FunctionOfPseudoJet.hh"

#include <string>
#include <climits>

#ifndef G__DICTIONARY
typedef double Double32_t; // ROOT will store as 32-bit, but in code is double
#endif


FASTJET_BEGIN_NAMESPACE      // defined in fastjet/internal/base.hh

/// Nsubjettiness extends the concept of Njettiness to a jet shape, but other
/// than the set of particles considered, they are identical.  This class
/// wraps the core Njettiness code to provide the fastjet::FunctionOfPseudoJet
/// interface for convenience in larger analyses.  See NjettinessPlugin.hh for
/// definitions of tau_N and the constructor options.

class Nsubjettiness : public FunctionOfPseudoJet<Double32_t> {
public:

   Nsubjettiness(int N, Njettiness::AxesMode mode, double beta, double R0, double Rcutoff=std::numeric_limits<double>::max());
   
   /// returns tau_N, measured on the constituents of this jet
   Double32_t result(const PseudoJet& jet) const;

   Double32_t result(const std::vector<fastjet::PseudoJet> particles) const;

private:

   int _N;
   mutable Njettiness _njettinessFinder; // should muck with this so result can be const without this mutable

};

FASTJET_END_NAMESPACE      // defined in fastjet/internal/base.hh

#endif  // __NSUBJETTINESS_HH__
