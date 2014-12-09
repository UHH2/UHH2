#pragma once

#include "TVector3.h"
#include "Math/LorentzVector.h"
#include "Math/PtEtaPhiE4D.h"

#include <cmath>

typedef ROOT::Math::LorentzVector< ROOT::Math::PtEtaPhiE4D< Double32_t > > LorentzVector;
typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > LorentzVectorXYZE;

namespace uhh2 {

/// transfroms LorentzVector to TVector3
inline TVector3 toVector(const LorentzVector & v4){
    return TVector3(v4.X(),v4.Y(),v4.Z());
}

/// transfroms LorentzVector to TVector3
inline TVector3 toVector(const LorentzVectorXYZE & v4){
    return TVector3(v4.X(),v4.Y(),v4.Z());
}

/// converts LorentzVector into LorentzVectorXYZE
inline LorentzVectorXYZE toXYZ(const LorentzVector & v4){
    return LorentzVectorXYZE(v4.Px(),v4.Py(),v4.Pz(),v4.E());
}

/// converts LorentzVectorXYZE into LorentzVector
inline LorentzVector toPtEtaPhi(const LorentzVectorXYZE & v4){
    return LorentzVector(v4.Pt(),v4.Eta(),v4.Phi(),v4.E());
}

/// distance in phi, with the convention -M_PI < deltaPhi <= M_PI
// T and U have to have a 'phi()' method, e.g. Particle, LorentzVector, etc.
template<typename T, typename U>
inline double deltaPhi(const T & p1, const U & p2){
    double deltaphi = fabs(p1.phi() - p2.phi());
    if(deltaphi > M_PI) deltaphi = 2* M_PI - deltaphi;
    return deltaphi;
}

/// distance in eta-phi space. works for any types which have 'eta' and 'phi' routines
// T and U have to have a 'phi()' and an 'eta()' method, e.g. Particle, LorentzVector, etc.
template<typename T, typename U>
inline double deltaR(const T & p1, const U & p2){
    double deltaeta = p1.eta() - p2.eta();
    double dphi = deltaPhi(p1, p2);
    return sqrt(deltaeta * deltaeta + dphi * dphi);
}

}
