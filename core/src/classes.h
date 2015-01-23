#include "UHH2/core/include/Particle.h"
#include "UHH2/core/include/FlavorParticle.h"
#include "UHH2/core/include/Jet.h"
#include "UHH2/core/include/Electron.h"
#include "UHH2/core/include/Muon.h"
#include "UHH2/core/include/Tau.h"
#include "UHH2/core/include/Tags.h"
#include "UHH2/core/include/Photon.h"
#include "UHH2/core/include/MET.h"
#include "UHH2/core/include/PrimaryVertex.h"
#include "UHH2/core/include/TopJet.h"
#include "UHH2/core/include/GenJetWithParts.h"
#include "UHH2/core/include/GenTopJet.h"
#include "UHH2/core/include/GenInfo.h"
#include "UHH2/core/include/GenParticle.h"

#include <vector>
#include <map>

namespace {
  namespace {
    Tags t;
    std::map<int, float> mif;
    Particle p;
    std::vector<Particle> ps;
    FlavorParticle pfl;
    std::vector<FlavorParticle> pfls;
    Jet jet;
    std::vector<Jet> jets;
    TopJet topjet;
    std::vector<TopJet> topjets;
    GenJetWithParts genjetwithparts;
    std::vector<GenJetWithParts> genjetswithparts;
    GenTopJet gentopjet;
    std::vector<GenTopJet> gentopjets;
    Electron ele; 
    std::vector<Electron> eles; 
    Muon mu; 
    std::vector<Muon> mus; 
    Tau tau;
    std::vector<Tau> taus; 
    Photon ph; 
    std::vector<Photon> phs; 
    MET met;
    PrimaryVertex pv;
    std::vector<PrimaryVertex> pvs; 
    GenInfo genInfo;
    GenParticle genp;
    std::vector<GenParticle> genps;
  }
}
