#include "UHH2/common/include/Utils.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;
using uhh2::deltaR;

namespace {
bool file_exists(const std::string & path){
    struct stat s;
    errno = 0;
    int res = stat(path.c_str(), &s);
    if(res < 0){
        return false;
    }
    else{
        return S_ISREG(s.st_mode);
    }
}
    
}

const Jet * nextJet(const Particle  & p, const std::vector<Jet> & jets){
    return closestParticle(p, jets);
}

float inv_mass_safe(const LorentzVector& p4){

  return p4.isTimelike() ? p4.mass() : -sqrt(-p4.mass2());
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

std::pair<double, double> drmin_pTrel(const Particle & p, const std::vector<Jet> & jets){
    auto nj = nextJet(p, jets);
    const constexpr auto inf = numeric_limits<double>::infinity();
    if(nj == nullptr){
        return make_pair(inf, inf);
    }
    return make_pair(deltaR(p, *nj), pTrel(p, nj));
}

std::string locate_file(const std::string & fname){
    if(fname.empty()){
        throw invalid_argument("locate_file with empty fname called");
    }
    if(fname[0] == '/'){
        if(!file_exists(fname)){
            throw runtime_error("locate_file: file '" + fname + "' does not exist!");
        }
        return fname;
    }
    // relative: try the various locations ...
    auto cmssw_base = getenv("CMSSW_BASE");
    if(cmssw_base != NULL){
        auto fname_test = string(cmssw_base) + "/src/UHH2/" + fname;
        if(file_exists(fname_test)){
            return fname_test;
        }
        fname_test = string(cmssw_base) + "/src/" + fname;
        if(file_exists(fname_test)){
            return fname_test;
        }
    }
    auto sframe_dir = getenv("SFRAME_DIR");
    if(sframe_dir != NULL){
        auto fname_test = string(sframe_dir) + "/UHH2/" + fname;
        if(file_exists(fname_test)){
            return fname_test;
        }
        fname_test = string(sframe_dir) + "/" + fname;
        if(file_exists(fname_test)){
            return fname_test;
        }
    }
    throw runtime_error("Could not locate file '" + fname + "' in $CMSSW_BASE/src/{UHH2,} or $SFRAME_DIR/{UHH2,}.");
}
