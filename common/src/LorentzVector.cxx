#include "UHH2/common/include/LorentzVector.h"

using namespace std;

TVector3 toVector(LorentzVector v4){
    TVector3 v3(0,0,0);
    v3.SetX(v4.X());
    v3.SetY(v4.Y());
    v3.SetZ(v4.Z());
    return v3;
}

TVector3 toVector(LorentzVectorXYZE v4){
    TVector3 v3(0,0,0);
    v3.SetX(v4.X());
    v3.SetY(v4.Y());
    v3.SetZ(v4.Z());
    return v3;
}

LorentzVectorXYZE toXYZ(LorentzVector v4){
    LorentzVectorXYZE v4_new(0,0,0,0);
    v4_new.SetPx(v4.Px());
    v4_new.SetPy(v4.Py());
    v4_new.SetPz(v4.Pz());
    v4_new.SetE(v4.E());
    return v4_new;
}

LorentzVector toPtEtaPhi(LorentzVectorXYZE v4){
    LorentzVector v4_new(0,0,0,0);
    v4_new.SetPt(v4.Pt());
    v4_new.SetEta(v4.Eta());
    v4_new.SetPhi(v4.Phi());
    v4_new.SetE(v4.E());
    return v4_new;
}

double deltaR(LorentzVector v1, LorentzVector v2){
    Particle p1;
    p1.set_v4(v1);
    Particle p2;
    p2.set_v4(v2);
    return p1.deltaR(p2);
}
