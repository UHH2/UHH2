#pragma once

#include "UHH2/core/include/Event.h"
#include "UHH2/common/include/ObjectIdUtils.h"
#include "UHH2/common/include/JetIds.h"


// see also ElectronIds.h for general comments


/** \brief CMS top tagger
 * 
 * Cuts on the minimum pairwise subjet mass (default: > 50 GeV), and on
 * the 'fat jet mass', which is calculated from the four-vector sum of all (i.e. 3-4) subjets.
 * A cut on n_subjets >= 3 is always applied.
 * 
 * The default values of the constructor correspond to the default values used in CMS.
 */
class CMSTopTag {
public:
  
  explicit CMSTopTag(double mminLower=50., double mjetLower=140., double mjetUpper=250.);
  
  bool operator()(const TopJet & topjet, const uhh2::Event & event) const;

 private:
  double m_mminLower;
  double m_mjetLower;
  double m_mjetUpper;
};



/** \brief Cut on tau3/tau2 < threshold
 * 
 * Usually used together with other tagger (e.g. in AndId<TopJet>).
 */
class Tau32 {
public:
    explicit Tau32(double threshold_ = 0.7): threshold(threshold_){}
    
    bool operator()(const TopJet & topjet, const uhh2::Event & event) const;
    
private:
    double threshold;
};

/** \brief Higgs tagger as e.g. used in Rebekka's analysis
 * 
 * Look for the number of b-tagged subjets (so far, the default is a medium working point but
 * probably have to change that!). If this number is greater or equal two, calculate the mass
 * of the two leading b-tagged subjets and return true if it is greater than minmass (default 60 GeV).
 */

class HiggsTag {
public:
    explicit HiggsTag(float minmass = 60.f, float maxmass = std::numeric_limits<float>::infinity(), JetId const & id = CSVBTag(CSVBTag::WP_MEDIUM)) :
        minmass_(minmass), maxmass_(maxmass), btagid_(id) {}

    bool operator()(TopJet const & topjet, uhh2::Event const & event) const;

private:
    float minmass_, maxmass_;
    JetId btagid_;

};

   
// minimum di-subjet-mass using the three pt-hardest subjets, as used by the CMS top tagger.
// It is provided as function here to allow calculating this variable for plotting, etc.
double m_disubjet_min(const TopJet & topjet);
double m_disubjet_min(const GenTopJet & topjet);
