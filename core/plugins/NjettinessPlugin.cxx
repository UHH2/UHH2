//  Njettiness Package
//  Version 0.4.1 (January 22, 2012)
//  Questions/Comments?  jthaler@jthaler.net

// Copyright (c) 2011-12, Jesse Thaler, Ken Van Tilburg, and Christopher K.
// Vermilion
//
//----------------------------------------------------------------------
// This file is part of the N-jettiness package ("N-jettiness").
//
//  N-jettiness is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 3 of the License, or
//  (at your option) any later version.
//
//  SpartyJet is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with SpartyJet; if not, write to the Free Software
//  Foundation, Inc.:
//      59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//----------------------------------------------------------------------

#include "UHH2/core/plugins/NjettinessPlugin.h"

#include "fastjet/ClusterSequence.hh"
#include "fastjet/JetDefinition.hh"

#include <string>
#include <climits>

FASTJET_BEGIN_NAMESPACE      // defined in fastjet/internal/base.hh

NjettinessPlugin::NjettinessPlugin(int N, Njettiness::AxesMode mode, double beta, double R0, double Rcutoff)
  : _N(N), _njettinessFinder(mode, NsubParameters(beta, R0, Rcutoff))
{}


std::string NjettinessPlugin::description() const {return "NJettiness";}

void NjettinessPlugin::run_clustering(ClusterSequence& cs) const
{
   std::vector<fastjet::PseudoJet> particles = cs.jets();
   _njettinessFinder.getTau(_N, particles);
   std::vector<std::list<int> > partition = _njettinessFinder.getPartition(particles);

   // output clusterings for each jet
   for (size_t i = 0; i < partition.size(); ++i) {
      std::list<int>& indices = partition[i];
      if (indices.size() == 0) continue;
      //std::list<int>::const_iterator it = indices.begin();
      while (indices.size() > 1) {
         int merge_i = indices.back(); indices.pop_back();
         int merge_j = indices.back(); indices.pop_back();
         int newIndex;
         double fakeDij = -1.0;
         cs.plugin_record_ij_recombination(merge_i, merge_j, fakeDij, newIndex);
         indices.push_back(newIndex);
      }
      double fakeDib = -1.0;
      cs.plugin_record_iB_recombination(indices.back(), fakeDib);
   }
}

FASTJET_END_NAMESPACE      // defined in fastjet/internal/base.hh
