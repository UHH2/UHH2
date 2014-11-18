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


#ifndef __NJETTINESS_H__
#define __NJETTINESS_H__

#include "fastjet/PseudoJet.hh"
#include "fastjet/ClusterSequence.hh"
#include <cmath>
#include <vector>
#include <list>

///////
//
// Helper classes
//
///////


// Parameters that define Njettiness
class NsubParameters {
private:
   double _beta;  // angular weighting exponent
   double _R0;    // characteristic jet radius (for normalization)
   double _Rcutoff;  // Cutoff scale for cone jet finding (default is large number such that no boundaries are used)
   
public:
   NsubParameters(const double mybeta, const double myR0, const double myRcutoff=10000.0) :
   _beta(mybeta), _R0(myR0), _Rcutoff(myRcutoff) {}
   double beta() const {return _beta;}
   double R0() const {return _R0;}
   double Rcutoff() const {return _Rcutoff;}
};

// Parameters that change minimization procedure.
// Set automatically when you choose NsubAxesMode, but can be adjusted manually as well
class KmeansParameters {
private:
   int _n_iterations;  // Number of iterations to run  (0 for no minimization, 1 for one-pass, >>1 for global minimum)
   double _precision;  // Desired precision in axes alignment
   int _halt;          // maximum number of steps per iteration
   double _noise_range;// noise range for random initialization
   
public:
   KmeansParameters() : _n_iterations(0), _precision(0.0), _halt(0), _noise_range(0.0) {}
   KmeansParameters(const int my_n_iterations, double my_precision, int my_halt, double my_noise_range) :
   _n_iterations(my_n_iterations),  _precision(my_precision), _halt(my_halt), _noise_range(my_noise_range) {}
   int n_iterations() const { return _n_iterations;}
   double precision() const {return _precision;}
   int halt() const {return _halt;}
   double noise_range() const {return _noise_range;}
};

// helper class for minimization
class LightLikeAxis {
private:
   double _rap, _phi, _weight, _mom;
   
public:
   LightLikeAxis() : _rap(0.0), _phi(0.0), _weight(0.0), _mom(0.0) {}
   LightLikeAxis(double my_rap, double my_phi, double my_weight, double my_mom) :
   _rap(my_rap), _phi(my_phi), _weight(my_weight), _mom(my_mom) {}
   double rap() const {return _rap;}
   double phi() const {return _phi;}
   double weight() const {return _weight;}
   double mom() const {return _mom;}
   void set_rap(double my_set_rap) {_rap = my_set_rap;}
   void set_phi(double my_set_phi) {_phi = my_set_phi;}
   void set_weight(double my_set_weight) {_weight = my_set_weight;}
   void set_mom(double my_set_mom) {_mom = my_set_mom;}
   void reset(double my_rap, double my_phi, double my_weight, double my_mom) {_rap=my_rap; _phi=my_phi; _weight=my_weight; _mom=my_mom;}
};


///////
//
// Main Njettiness Class
//
///////

class Njettiness {
public:
   enum AxesMode {
      kt_axes,  // exclusive kt axes
      ca_axes,  // exclusive ca axes
      antikt_0p2_axes,  // inclusive hardest axes with antikt-0.2
      min_axes, // axes that minimize N-subjettiness (100 passes by default)
      manual_axes, // set your own axes with setAxes()
      onepass_kt_axes, // one-pass minimization from kt starting point
      onepass_ca_axes, // one-pass minimization from ca starting point
      onepass_antikt_0p2_axes,  // one-pass minimization from antikt-0.2 starting point 
      onepass_manual_axes  // one-pass minimization from manual starting point
   };

private:
   AxesMode _axes;    //Axes mode choice
   NsubParameters _paraNsub;  //Parameters for Njettiness
   KmeansParameters _paraKmeans;  //Parameters for Minimization Procedure (set by NsubAxesMode automatically, but can change manually if desired)

   std::vector<fastjet::PseudoJet> _currentAxes;
   
   void establishAxes(unsigned n_jets, const std::vector <fastjet::PseudoJet> & inputs);
   
public:
   Njettiness(AxesMode axes, NsubParameters paraNsub);
   
   void setParaKmeans(KmeansParameters newPara) {_paraKmeans = newPara;}
   void setParaNsub(NsubParameters newPara) {_paraNsub = newPara;}
   
   // setAxes for Manual mode
   void setAxes(std::vector<fastjet::PseudoJet> myAxes);
   
   // The value of N-subjettiness
   double getTau(unsigned n_jets, const std::vector<fastjet::PseudoJet> & inputJets);
   
   // get axes used by getTau.
   std::vector<fastjet::PseudoJet> currentAxes();
   
   // partition inputs by Voronoi (each vector stores indices corresponding to inputJets)
   std::vector<std::list<int> > getPartition(const std::vector<fastjet::PseudoJet> & inputJets);

   // partition inputs by Voronoi
   std::vector<fastjet::PseudoJet> getJets(const std::vector<fastjet::PseudoJet> & inputJets);

   static double sq(double x);
   
   // Calculates distance between two points in rapidity-azimuth plane
   static double DistanceSq(double rap1, double phi1, double rap2, double phi2);
   
   static double Distance(double rap1, double phi1, double rap2, double phi2);
   
   template <int N> static std::vector<LightLikeAxis> UpdateAxesFast(const std::vector <LightLikeAxis> & old_axes, 
								     const std::vector <fastjet::PseudoJet> & inputJets,
								     NsubParameters paraNsub, double precision);

   // Given starting axes, update to find better axes
   // (This is just a wrapper for the templated version above.)
   static std::vector<LightLikeAxis> UpdateAxes(const std::vector <LightLikeAxis> & old_axes, 
						const std::vector <fastjet::PseudoJet> & inputJets, NsubParameters paraNsub, double precision);


   // Go from internal LightLikeAxis to PseudoJet
   static std::vector<fastjet::PseudoJet> ConvertToPseudoJet(const std::vector <LightLikeAxis>& axes);
   
   // N-subjettiness pieces
   // Returns the sub-tau values, i.e. a std::vector of the contributions to tau_N of each Voronoi region (or region within R_0)
   static std::vector<double> ConstituentTauValue(const std::vector <fastjet::PseudoJet> & particles, const std::vector<fastjet::PseudoJet>& axes, const NsubParameters& paraNsub); 

   // N-subjettiness values
   // Calculates tau_N
   static double TauValue(const std::vector <fastjet::PseudoJet>& particles, const std::vector<fastjet::PseudoJet>& axes,const NsubParameters& paraNsub);

   // Get exclusive kT subjets
   static std::vector<fastjet::PseudoJet> GetKTAxes(int n_jets, const std::vector <fastjet::PseudoJet> & inputJets);

   // Get exclusive CA subjets
   static std::vector<fastjet::PseudoJet> GetCAAxes(int n_jets, const std::vector <fastjet::PseudoJet> & inputJets);

   // Get inclusive anti kT hardest subjets
   static std::vector<fastjet::PseudoJet> GetAntiKTAxes(int n_jets, double R0, const std::vector <fastjet::PseudoJet> & inputJets);

   // Get minimization axes
   static std::vector<fastjet::PseudoJet> GetMinimumAxes(const std::vector <fastjet::PseudoJet> & seedAxes, const std::vector <fastjet::PseudoJet> & inputJets, KmeansParameters para, 
					       NsubParameters paraNsub);

};

#endif

#ifndef __CINT__
#include "Njettiness.icc"
#endif

