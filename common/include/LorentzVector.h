#pragma once

#include "TVector3.h"
#include "UHH2/core/include/Particle.h"

typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > LorentzVectorXYZE;

/// transfroms LorentzVector to TVector3
TVector3 toVector(LorentzVector v4);

/// transfroms LorentzVector to TVector3
TVector3 toVector(LorentzVectorXYZE v4);

/// converts LorentzVector into LorentzVector
LorentzVectorXYZE toXYZ(LorentzVector v4);

/// converts LorentzVector into LorentzVector
LorentzVector toPtEtaPhi(LorentzVectorXYZE v4);

/// distance in eta-phi space
double deltaR(LorentzVector v1, LorentzVector v2);
