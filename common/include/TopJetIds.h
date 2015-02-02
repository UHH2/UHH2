#pragma once

#include "UHH2/core/include/Event.h"

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

