#include "UHH2/common/include/EventShapeVariables.h"
#include "TMath.h"


/// constructor from r theta phi coordinates
EventShapeVariables::EventShapeVariables(
    uhh2::Context & ctx,
    const std::string & jet_collection,
    const std::string & ele_collection,
    const std::string & muo_collection,
    const std::string & output_prefix,
    double r_param,
    unsigned n_steps_for_iso_and_circ
):
    r_param_(r_param),
    n_steps_for_iso_and_circ_(n_steps_for_iso_and_circ),
    hndl_jets_(ctx.get_handle<std::vector<Jet>>(jet_collection)),
    hndl_eles_(ctx.get_handle<std::vector<Electron>>(ele_collection)),
    hndl_muos_(ctx.get_handle<std::vector<Muon>>(muo_collection)),
    hndl_iso_(ctx.get_handle<float>(output_prefix + "isotropy")),
    hndl_circ_(ctx.get_handle<float>(output_prefix + "circularity")),
    hndl_spher_(ctx.get_handle<float>(output_prefix + "sphericity")),
    hndl_aplan_(ctx.get_handle<float>(output_prefix + "aplanarity")),
    hndl_C_(ctx.get_handle<float>(output_prefix + "C")),
    hndl_D_(ctx.get_handle<float>(output_prefix + "D"))
{}

/// the return value is 1 for spherical events and 0 for events linear in r-phi. This function
/// needs the number of steps to determine how fine the granularity of the algorithm in phi
/// should be
double
EventShapeVariables::isotropy(const unsigned int& numberOfSteps) const
{
  const double deltaPhi=2*TMath::Pi()/numberOfSteps;
  double phi = 0, eIn =-1., eOut=-1.;
  for(unsigned int i=0; i<numberOfSteps; ++i){
    phi+=deltaPhi;
    double sum=0;
    for(unsigned int j=0; j<inputVectors_.size(); ++j){
      // sum over inner product of unit vectors and momenta
      sum+=TMath::Abs(TMath::Cos(phi)*inputVectors_[j].x()+TMath::Sin(phi)*inputVectors_[j].y());
    }
    if( eOut<0. || sum<eOut ) eOut=sum;
    if( eIn <0. || sum>eIn  ) eIn =sum;
  }
  return (eIn-eOut)/eIn;
}

/// the return value is 1 for spherical and 0 linear events in r-phi. This function needs the
/// number of steps to determine how fine the granularity of the algorithm in phi should be
double
EventShapeVariables::circularity(const unsigned int& numberOfSteps) const
{
  const double deltaPhi=2*TMath::Pi()/numberOfSteps;
  double circularity=-1, phi=0, area = 0;
  for(unsigned int i=0;i<inputVectors_.size();i++) {
    area+=TMath::Sqrt(inputVectors_[i].x()*inputVectors_[i].x()+inputVectors_[i].y()*inputVectors_[i].y());
  }
  for(unsigned int i=0; i<numberOfSteps; ++i){
    phi+=deltaPhi;
    double sum=0, tmp=0.;
    for(unsigned int j=0; j<inputVectors_.size(); ++j){
      sum+=TMath::Abs(TMath::Cos(phi)*inputVectors_[j].x()+TMath::Sin(phi)*inputVectors_[j].y());
    }
    tmp=TMath::Pi()/2*sum/area;
    if( circularity<0 || tmp<circularity ){
      circularity=tmp;
    }
  }
  return circularity;
}

/// helper function to fill the 3 dimensional momentum tensor from the inputVecotrs where needed
TMatrixDSym
EventShapeVariables::compMomentumTensor(double r) const
{
  TMatrixDSym momentumTensor(3);
  momentumTensor.Zero();

  if ( inputVectors_.size() < 2 ){
    return momentumTensor;
  }

  // fill momentumTensor from inputVectors
  double norm = 0.;
  for ( int i = 0; i < (int)inputVectors_.size(); ++i ){
    double p2 = inputVectors_[i].Dot(inputVectors_[i]);
    double pR = ( r == 2. ) ? p2 : TMath::Power(p2, 0.5*r);
    norm += pR;
    double pRminus2 = ( r == 2. ) ? 1. : TMath::Power(p2, 0.5*r - 1.);
    momentumTensor(0,0) += pRminus2*inputVectors_[i].x()*inputVectors_[i].x();
    momentumTensor(0,1) += pRminus2*inputVectors_[i].x()*inputVectors_[i].y();
    momentumTensor(0,2) += pRminus2*inputVectors_[i].x()*inputVectors_[i].z();
    momentumTensor(1,0) += pRminus2*inputVectors_[i].y()*inputVectors_[i].x();
    momentumTensor(1,1) += pRminus2*inputVectors_[i].y()*inputVectors_[i].y();
    momentumTensor(1,2) += pRminus2*inputVectors_[i].y()*inputVectors_[i].z();
    momentumTensor(2,0) += pRminus2*inputVectors_[i].z()*inputVectors_[i].x();
    momentumTensor(2,1) += pRminus2*inputVectors_[i].z()*inputVectors_[i].y();
    momentumTensor(2,2) += pRminus2*inputVectors_[i].z()*inputVectors_[i].z();
  }

  //std::cout << "momentumTensor:" << std::endl;
  //std::cout << momentumTensor(0,0) << " " << momentumTensor(0,1) << " " << momentumTensor(0,2)
  //          << momentumTensor(1,0) << " " << momentumTensor(1,1) << " " << momentumTensor(1,2)
  //	      << momentumTensor(2,0) << " " << momentumTensor(2,1) << " " << momentumTensor(2,2) << std::endl;

  // return momentumTensor normalized to determinant 1
  return (1./norm)*momentumTensor;
}

/// helper function to fill the 3 dimensional vector of eigen-values;
/// the largest (smallest) eigen-value is stored at index position 0 (2)
void EventShapeVariables::compEigenValues(double r)
{
  eigen_values_.reset(new TVectorD(3));
  TMatrixDSym myTensor = compMomentumTensor(r);
  if( myTensor.IsSymmetric() ){
    if( myTensor.NonZeros() != 0 ) myTensor.EigenVectors(*eigen_values_);
  }

  // CV: TMatrixDSym::EigenVectors returns eigen-values and eigen-vectors
  //     ordered by descending eigen-values, so no need to do any sorting here...
  // std::cout << "eigenValues(0) = " << (*eigen_values_)(0) << ","
  // 	      << " eigenValues(1) = " << (*eigen_values_)(1) << ","
  // 	      << " eigenValues(2) = " << (*eigen_values_)(2) << std::endl;
}

/// 1.5*(q1+q2) where 0<=q1<=q2<=q3 are the eigenvalues of the momentum tensor sum{p_j[a]*p_j[b]}/sum{p_j**2}
/// normalized to 1. Return values are 1 for spherical, 3/4 for plane and 0 for linear events
double
EventShapeVariables::sphericity() const
{
  const TVectorD & eigenValues = *eigen_values_;
  return 1.5*(eigenValues(1) + eigenValues(2));
}

/// 1.5*q1 where 0<=q1<=q2<=q3 are the eigenvalues of the momentum tensor sum{p_j[a]*p_j[b]}/sum{p_j**2}
/// normalized to 1. Return values are 0.5 for spherical and 0 for plane and linear events
double
EventShapeVariables::aplanarity() const
{
  const TVectorD & eigenValues = *eigen_values_;
  return 1.5*eigenValues(2);
}

/// 3.*(q1*q2+q1*q3+q2*q3) where 0<=q1<=q2<=q3 are the eigenvalues of the momentum tensor sum{p_j[a]*p_j[b]}/sum{p_j**2}
/// normalized to 1. Return value is between 0 and 1
/// and measures the 3-jet structure of the event (C vanishes for a "perfect" 2-jet event)
double
EventShapeVariables::C() const
{
  const TVectorD & eigenValues = *eigen_values_;
  return 3.*(eigenValues(0)*eigenValues(1) + eigenValues(0)*eigenValues(2) + eigenValues(1)*eigenValues(2));
}

/// 27.*(q1*q2*q3) where 0<=q1<=q2<=q3 are the eigenvalues of the momemtum tensor sum{p_j[a]*p_j[b]}/sum{p_j**2}
/// normalized to 1. Return value is between 0 and 1
/// and measures the 4-jet structure of the event (D vanishes for a planar event)
double
EventShapeVariables::D() const
{
  const TVectorD & eigenValues = *eigen_values_;
  return 27.*eigenValues(0)*eigenValues(1)*eigenValues(2);
}

bool EventShapeVariables::process(uhh2::Event & event) {

    // fill input vectors
    const auto & jets = event.get(hndl_jets_);
    const auto & eles = event.is_valid(hndl_eles_) ? event.get(hndl_eles_) : std::vector<Electron>();
    const auto & muos = event.is_valid(hndl_muos_) ? event.get(hndl_muos_) : std::vector<Muon>();
    inputVectors_.reserve( jets.size() + eles.size() + muos.size());
    for(const auto & obj : jets) {
        const auto & vec = obj.v4();
        inputVectors_.emplace_back(vec.x(), vec.y(), vec.z());
    }
    for(const auto & obj : eles) {
        const auto & vec = obj.v4();
        inputVectors_.emplace_back(vec.x(), vec.y(), vec.z());
    }
    for(const auto & obj : muos) {
        const auto & vec = obj.v4();
        inputVectors_.emplace_back(vec.x(), vec.y(), vec.z());
    }


    // calculate eigen vectors
    compEigenValues(r_param_);

    // now everything is setup for calculation
    event.set(hndl_iso_, isotropy(n_steps_for_iso_and_circ_));
    event.set(hndl_circ_, circularity(n_steps_for_iso_and_circ_));
    event.set(hndl_spher_, sphericity());
    event.set(hndl_aplan_, aplanarity());
    event.set(hndl_C_, C());
    event.set(hndl_D_, D());

    // clean up
    inputVectors_.clear();
    delete eigen_values_.release();
    return true;
}

