#pragma once

/**
   THE BUSINESS CODE IN HERE IS TAKEN FROM PhysicsTools/CandUtils/interface/EventShapeVariables.h

   \class   EventShapeVariables EventShapeVariables.h "PhysicsTools/CandUtils/interface/EventShapeVariables.h"

   \brief   Class for the calculation of several event shape variables

   Class for the calculation of several event shape variables. Isotropy, sphericity,
   aplanarity and circularity are supported. The class supports vectors of 3d vectors
   and edm::Views of reco::Candidates as input. The 3d vectors can be given in
   cartesian, cylindrical or polar coordinates. It exploits the ROOT::TMatrixDSym
   for the calculation of the sphericity and aplanarity.

   See http://cepa.fnal.gov/psm/simulation/mcgen/lund/pythia_manual/pythia6.3/pythia6301/node213.html
   for an explanation of sphericity, aplanarity and the quantities C and D.

   Original Authors: Sebastian Naumann-Emme, University of Hamburg
                     Roger Wolf, University of Hamburg
	                 Christian Veelken, UC Davis
*/

#include <string>
#include <vector>
#include <memory>

#include "TMatrixDSym.h"
#include "TVectorD.h"
#include "Math/Vector3D.h"
typedef ROOT::Math::DisplacementVector3D<ROOT::Math::Cartesian3D<double> > XYZVector;

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/Jet.h"
#include "UHH2/core/include/Electron.h"
#include "UHH2/core/include/Muon.h"


class EventShapeVariables : public uhh2::AnalysisModule {
public:
  explicit EventShapeVariables(uhh2::Context & ctx,
                               const std::string & jet_collection="jets",
                               const std::string & ele_collection="",
                               const std::string & muo_collection="",
                               const std::string & output_prefix="",
                               double r_param = 2.,
                               unsigned n_steps_for_iso_and_circ=1000);

  virtual bool process(uhh2::Event & event) override;

private:
  /// the return value is 1 for spherical events and 0 for events linear in r-phi. This function 
  /// needs the number of steps to determine how fine the granularity of the algorithm in phi 
  /// should be
  double isotropy(const unsigned int& numberOfSteps = 1000) const;
  
  /// the return value is 1 for spherical and 0 linear events in r-phi. This function needs the 
  /// number of steps to determine how fine the granularity of the algorithm in phi should be
  double circularity(const unsigned int& numberOfSteps = 1000) const;

  /// 1.5*(q1+q2) where 0<=q1<=q2<=q3 are the eigenvalues of the momemtum tensor 
  /// sum{p_j[a]*p_j[b]}/sum{p_j**2} normalized to 1. Return values are 1 for spherical, 3/4 for 
  /// plane and 0 for linear events
  double sphericity()  const;

  /// 1.5*q1 where 0<=q1<=q2<=q3 are the eigenvalues of the momemtum tensor
  /// sum{p_j[a]*p_j[b]}/sum{p_j**2} normalized to 1. Return values are 0.5 for spherical and 0 
  /// for plane and linear events
  double aplanarity()  const;

  /// 3.*(q1*q2+q1*q3+q2*q3) where 0<=q1<=q2<=q3 are the eigenvalues of the momemtum tensor 
  /// sum{p_j[a]*p_j[b]}/sum{p_j**2} normalized to 1. Return value is between 0 and 1 
  /// and measures the 3-jet structure of the event (C vanishes for a "perfect" 2-jet event)
  double C() const;

  /// 27.*(q1*q2*q3) where 0<=q1<=q2<=q3 are the eigenvalues of the momemtum tensor
  /// sum{p_j[a]*p_j[b]}/sum{p_j**2} normalized to 1. Return value is between 0 and 1 
  /// and measures the 4-jet structure of the event (D vanishes for a planar event)
  double D() const;

  TMatrixDSym compMomentumTensor(double = 2.) const;
  void compEigenValues(double = 2.);

  std::vector<XYZVector> inputVectors_;
  std::unique_ptr<TVectorD> eigen_values_;

  double r_param_;
  unsigned n_steps_for_iso_and_circ_;

  uhh2::Event::Handle<std::vector<Jet>> hndl_jets_;
  uhh2::Event::Handle<std::vector<Electron>> hndl_eles_;
  uhh2::Event::Handle<std::vector<Muon>> hndl_muos_;
  uhh2::Event::Handle<float> hndl_iso_;
  uhh2::Event::Handle<float> hndl_circ_;
  uhh2::Event::Handle<float> hndl_spher_;
  uhh2::Event::Handle<float> hndl_aplan_;
  uhh2::Event::Handle<float> hndl_C_;
  uhh2::Event::Handle<float> hndl_D_;
};


