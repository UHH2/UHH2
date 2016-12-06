#pragma once

#include "UHH2/core/include/Event.h"
#include "UHH2/core/include/Tags.h"
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
  
  enum class MassType {groomed, ungroomed};

  explicit CMSTopTag(double mminLower=50., double mjetLower=140., double mjetUpper=250., MassType typeOfMass = MassType::ungroomed);
  explicit CMSTopTag(MassType typeOfMass);
  
  bool operator()(const TopJet & topjet, const uhh2::Event & event) const;

 private:
  double m_mminLower;
  double m_mjetLower;
  double m_mjetUpper;
  MassType m_typeOfMass;
};

/** \type-2 top tag 
 * Cuts on the ungrommed 'fat jet mass' or on the groomed 'fat jet mass'.  
 *  
 */

class Type2TopTag {
public:
  
  enum class MassType {groomed, ungroomed};
  explicit Type2TopTag(double mjetLower=60., double mjetUpper=100., MassType typeOfMass = MassType::ungroomed, boost::optional<JetId> SubjetId=boost::none);
  explicit Type2TopTag(MassType typeOfMass);
  
  bool operator()(const TopJet & topjet, const uhh2::Event & event) const;

 private:
   double m_mjetLower;
   double m_mjetUpper;
   MassType m_typeOfMass;
   boost::optional<JetId> m_SubjetId;
};


class HEPTopTag{
public:
  
  explicit HEPTopTag(double ptJetMin=200., double massWindowLower=0.85, double massWindowUpper=1.15, double cutCondition2=0.35, double cutCondition3=0.35);
  
  bool operator()(const TopJet & topjet, const uhh2::Event & event) const;

 private:
  double m_ptJetMin;
  double m_massWindowLower;
  double m_massWindowUpper;
  double m_cutCondition2;
  double m_cutCondition3;
};

/** \Hep top tag V2
 * 
 * Cuts on the HTT mass and fRec
 * 
 * The default values of the constructor correspond to the 40% efficiency WP presented in the top tagging talk on https://indico.cern.ch/event/393337/#preview:1078843.
 */

class HEPTopTagV2{
public:

   explicit HEPTopTagV2(double minHTTmass=128., double maxHTTmass=171., double maxfRec=0.18, boost::optional<JetId> SubjetId=boost::none);

  bool operator()(const TopJet & topjet, const uhh2::Event & event) const;

 private:
   double m_minHTTmass;
   double m_maxHTTmass;
   double m_maxfRec;
   boost::optional<JetId> m_SubjetId;
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

/** \brief Cut on tau2/tau1 < threshold
 * 
 */
class Tau21 {
public:
    explicit Tau21(double threshold_ = 0.6): threshold(threshold_){}
    
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

//groomed TopJet mass, obtained using the sum of the four-vectors of all the subjets.
//as used by CMSTopTagger and HEPTopTagger
//returns -1 for space-like vectors
double m_groomed(const TopJet & topjet);
double m_groomed(const GenTopJet & topjet);
