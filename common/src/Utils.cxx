#include "UHH2/common/include/Utils.h"

#include <limits>

using namespace std;


const Jet * nextJet(const Particle  & p, const std::vector<Jet> & jets){
    double deltarmin = numeric_limits<double>::infinity();
    const Jet* nextjet=0;
    for(unsigned int i=0; i<jets.size(); ++i) {
        const Jet & ji = jets[i];
        double dr = ji.deltaR(p);
        if(dr < deltarmin) {
            deltarmin = dr;
            nextjet = &ji;
        }
    }
    return nextjet;
}


double pTrel(const Particle  & p, const Particle * reference_axis){
    double ptrel=0;
    if (!reference_axis) return ptrel;
    TVector3 p3(p.v4().Px(),p.v4().Py(),p.v4().Pz());
    TVector3 ref3(reference_axis->v4().Px(),reference_axis->v4().Py(),reference_axis->v4().Pz());
    if(ref3.Mag()!=0) {
        ptrel = p3.Cross(ref3).Mag()/ref3.Mag();
    } // otherwise: ptrel stays at 0.0
    return ptrel;
}

