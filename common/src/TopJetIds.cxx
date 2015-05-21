#include "UHH2/common/include/TopJetIds.h"
#include "UHH2/common/include/Utils.h"

using namespace std;
using namespace uhh2;

bool CMSTopTag::operator()(const TopJet & topjet, const uhh2::Event &) const {
    auto subjets = topjet.subjets();
    if(subjets.size() < 3) return false;
    
    auto mjet = topjet.v4().M();
    if(mjet < m_mjetLower) return false;
    if(mjet > m_mjetUpper) return false;

    auto mmin = m_disubjet_min(topjet);
    if(mmin < m_mminLower) return false;

    return true;
}


CMSTopTag::CMSTopTag(double mminLower, double mjetLower, double mjetUpper): m_mminLower(mminLower),
    m_mjetLower(mjetLower), m_mjetUpper(mjetUpper){}


    
bool Tau32::operator()(const TopJet & topjet, const uhh2::Event &) const {
    auto tau2 = topjet.tau2();
    if(!std::isfinite(tau2) || tau2 == 0.0) return false;
    auto tau3 = topjet.tau3();
    if(!std::isfinite(tau3)) return false;
    return tau3 / tau2 < threshold;
}


bool HiggsTag::operator()(TopJet const & topjet, uhh2::Event const & event) const {
    auto subjets = topjet.subjets();
    if(subjets.size() < 2) return false;
    clean_collection(subjets, event, btagid_);
    if (subjets.size() < 2) return false;
    sort_by_pt(subjets);

    LorentzVector firsttwosubjets = subjets[0].v4() + subjets[1].v4();
    if(!firsttwosubjets.isTimelike()) {
        return false;
    }
    auto mjet = firsttwosubjets.M();
    if(mjet < minmass_) return false;
    if(mjet > maxmass_) return false;
    return true;
}


namespace {
    
template<typename T>
double m_disubjet_minT(const T & topjet){
    auto subjets = topjet.subjets();
    if(subjets.size() < 2) return 0.0;
    
    // only need to sort if subjets there are more than 3 subjets, as
    // otherwise, we use all 3 anyway.
    if(subjets.size() > 3) sort_by_pt(subjets);
    
    double m01 = 0;
    auto sum01 = subjets[0].v4()+subjets[1].v4();
    if(sum01.isTimelike())  m01 = sum01.M();
    
    if(subjets.size() < 3) return m01;
    
    double m02 = 0;
    auto sum02 = subjets[0].v4()+subjets[2].v4();
    if( sum02.isTimelike() ) m02 = sum02.M();
    
    double m12 = 0;
    auto sum12 = subjets[1].v4()+subjets[2].v4();
    if( sum12.isTimelike() )  m12 = sum12.M();
    
    return std::min(m01,std::min(m02, m12));
}

}

double m_disubjet_min(const TopJet & topjet){
    return m_disubjet_minT<TopJet>(topjet);
}


double m_disubjet_min(const GenTopJet & topjet){
    return m_disubjet_minT<GenTopJet>(topjet);
}

