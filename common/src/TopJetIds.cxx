#include "UHH2/common/include/TopJetIds.h"
#include "UHH2/common/include/Utils.h"

using namespace std;
using namespace uhh2;

bool CMSTopTag::operator()(const TopJet & topjet, const uhh2::Event &) const {
    auto subjets = topjet.subjets();
    if(subjets.size() < 3) return false;
    
    LorentzVector allsubjets;
    for(const auto & subjet : subjets) {
        allsubjets += subjet.v4();
    }
    if(!allsubjets.isTimelike()) {
        return false;
    }
    auto mjet = allsubjets.M();
    if(mjet < m_mjetLower) return false;
    if(mjet > m_mjetUpper) return false;
    sort_by_pt(subjets);
    double m01 = 0;
    if( (subjets[0].v4()+subjets[1].v4()).isTimelike())
      m01=(subjets[0].v4()+subjets[1].v4()).M();
    double m02 = 0;
    if( (subjets[0].v4()+subjets[2].v4()).isTimelike() )
      m02=(subjets[0].v4()+subjets[2].v4()).M();
    double m12 = 0;
    if( (subjets[1].v4()+subjets[2].v4()).isTimelike() )
      m12 = (subjets[1].v4()+subjets[2].v4()).M();
    //minimum pairwise mass of all three combinations:
    auto mmin = std::min(m01,std::min(m02,m12));
    if(mmin < m_mminLower) return false;
    return true;
}


CMSTopTag::CMSTopTag(double mminLower, double mjetLower, double mjetUpper): m_mminLower(mminLower),
    m_mjetLower(mjetLower), m_mjetUpper(mjetUpper){}


    
bool Tau32::operator()(const TopJet & topjet, const uhh2::Event &) const{
    auto tau2 = topjet.tau2();
    if(!std::isfinite(tau2) || tau2 == 0.0) return false;
    auto tau3 = topjet.tau3();
    if(!std::isfinite(tau3)) return false;
    return tau3 / tau2 < threshold;
}

