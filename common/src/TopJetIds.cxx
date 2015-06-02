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


bool HEPTopTag::operator()(const TopJet & topjet, const uhh2::Event &) const{
    
  double mjet;
  double ptjet;
  int nsubjets;
  auto subjets = topjet.subjets();
  sort_by_pt(subjets);
  
  double topmass=172.3;
  double wmass=80.4;
  
  nsubjets=subjets.size();
  
  LorentzVector allsubjets(0,0,0,0);
  
  for(int j=0; j<nsubjets; ++j) {
    allsubjets += subjets[j].v4();
  }
  if(!allsubjets.isTimelike()) {
    mjet=0;
    return false;
  }
  
  mjet = allsubjets.M();
  ptjet= topjet.pt();
    
  double m12, m13, m23;
  
  //Require exactly 3 subjets
  if(nsubjets==3) {    
    m12 = 0;
    if( (subjets[0].v4()+subjets[1].v4()).isTimelike())
      m12=(subjets[0].v4()+subjets[1].v4()).M();
    m13 = 0;
    if( (subjets[0].v4()+subjets[2].v4()).isTimelike() )
      m13=(subjets[0].v4()+subjets[2].v4()).M();
    m23 = 0;
    if( (subjets[1].v4()+subjets[2].v4()).isTimelike()  )
      m23 = (subjets[1].v4()+subjets[2].v4()).M();
    
  } 
  else return false;

  
  double rmin=m_massWindowLower*wmass/topmass;
  double rmax=m_massWindowUpper*wmass/topmass;
  
  int keep=0;
  
  //Conditions on the subjects: at least one has to be true
  //1st condition
  if(atan(m13/m12)>0.2 && atan(m13/m12)<1.3 && m23/mjet>rmin && m23/mjet<rmax) keep=1;
  
  //2nd condition
  double cond2left=pow(rmin,2)*(1+pow((m13/m12),2));
  double cond2cent=1-pow(m23/mjet,2);
  double cond2right=pow(rmax,2)*(1+pow(m13/m12,2));
  
  if(cond2left<cond2cent && cond2cent<cond2right && m23/mjet>m_cutCondition2) keep=1;
  
  //3rd condition
  double cond3left=pow(rmin,2)*(1+pow((m12/m13),2));
  double cond3cent=1-pow(m23/mjet,2);
  double cond3right=pow(rmax,2)*(1+pow(m12/m13,2));
  
  if(cond3left<cond3cent && cond3cent<cond3right && m23/mjet>m_cutCondition3) keep=1;
  
  if( mjet < 140 || mjet > 250) keep=0;
  
  //Final requirement: at least one of the three subjets conditions and total pt
  if(keep==1 && ptjet>m_ptJetMin) {
    return true;
  } else {
    return false;
  }
  
  return true; 
}

HEPTopTag::HEPTopTag(double ptJetMin, double massWindowLower, double massWindowUpper, double cutCondition2, double cutCondition3): m_ptJetMin(ptJetMin),
  m_massWindowLower(massWindowLower), m_massWindowUpper(massWindowUpper), m_cutCondition2(cutCondition2), m_cutCondition3(cutCondition3){}


    
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

