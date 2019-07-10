#pragma once

#include "UHH2/core/include/Event.h"
#include "UHH2/common/include/ObjectIdUtils.h"
#include "TH2.h"
#include "TFile.h"

// see comments in ElectronIds.h for general comments on IDs.

/** \brief The CSV V2 b-tag as jet id
 * 
 * Cuts on uhh2::Jet::btag_combinedSecondaryVertex, which (in CMSSW) is called "combinedInclusiveSecondaryVertexV2BJetTags"
 * (see core/plugins/NtupleWriterJets.cxx), also known as "CombinedSecondaryVertex v2 with IVF".
 * 
 * Uses the preliminary thresholds for that tagger recommended here:
 *  https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation53XReReco
 * 
 * Note that this will certainly need updates once 13TeV recommendations are available.
 */
class BTag {
  // wrapper class to hold btag id and algorithm/working point information
 public:
  enum algo {CSVV2, DEEPCSV, DEEPJET};
  enum wp {WP_LOOSE  = 0,
	   WP_MEDIUM = 1,
	   WP_TIGHT  = 2 };
  explicit BTag(algo tagger, wp working_point);

  bool operator()(const Jet & jet, const uhh2::Event & event) {return jet_id(jet,event);}

  std::string GetTagger() {return m_algo;}
  int GetWorkingPoint() {return m_working_point;}
 private:
  std::string m_algo;
  int m_working_point;
  JetId jet_id;
};

class CSVBTag {
public:
  enum wp {WP_LOOSE  = 0,
	   WP_MEDIUM = 1,
	   WP_TIGHT  = 2 };
  
    explicit CSVBTag(wp working_point);
    explicit CSVBTag(float float_point);

    bool operator()(const Jet & jet, const uhh2::Event & event);
private:
    float csv_threshold;
    wp m_working_point;
};

class DeepCSVBTag {
public:
  enum wp {WP_LOOSE  = 0,
	   WP_MEDIUM = 1,
	   WP_TIGHT  = 2 };
  
    explicit DeepCSVBTag(wp working_point);
    explicit DeepCSVBTag(float float_point);

    bool operator()(const Jet & jet, const uhh2::Event & event);
private:
    float deepcsv_threshold;
    wp m_working_point;
};

class DeepJetBTag {
public:
  enum wp {WP_LOOSE  = 0,
	   WP_MEDIUM = 1,
	   WP_TIGHT  = 2 };
  
    explicit DeepJetBTag(wp working_point);
    explicit DeepJetBTag(float float_point);

    bool operator()(const Jet & jet, const uhh2::Event & event);
private:
    float deepjet_threshold;
    wp m_working_point;
};

/**
 * Jet Id following recomendations from JetMET for RunII
 */
class JetPFID {
 public:
  enum wp {WP_LOOSE_CHS, WP_TIGHT_CHS, 
	   WP_LOOSE_PUPPI, WP_TIGHT_PUPPI,
	   WP_TIGHT_LEPVETO};
  explicit JetPFID(wp working_point);
  bool operator()(const Jet&, const uhh2::Event&) const;
 private:
  wp m_working_point;
  //2016 taken from https://twiki.cern.ch/twiki/bin/view/CMS/JetID13TeVRun2016
  bool looseID2016_CHS(const Jet & jet) const;
  bool tightID2016_CHS(const Jet & jet) const;
  bool looseID2016_PUPPI(const Jet & jet) const;
  bool tightID2016_PUPPI(const Jet & jet) const;
  bool tightLepVetoID2016(const Jet & jet) const;

  //2017 taken from https://twiki.cern.ch/twiki/bin/view/CMS/JetID13TeVRun2017
  bool tightID2017_CHS(const Jet & jet) const;
  bool tightID2017_PUPPI(const Jet & jet) const;
  bool tightLepVetoID2017(const Jet & jet) const;

  //2018 taken from https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetID13TeVRun2018
  bool tightID2018_CHS(const Jet & jet) const;
  bool tightLepVetoID2018(const Jet & jet) const;

};

/**
 * Jet PU Id following recomendations from JetMET for RunII
 */

class JetPUid {
 public:
  enum wp {WP_LOOSE, WP_TIGHT, WP_MEDIUM};
  explicit JetPUid(wp working_point);
  bool operator()(const Jet&, const uhh2::Event&) const;
 private:
  wp m_working_point;
};

/**
 * Jet EtaPhiaCut Id rejects jets, selected according to etaphi map
 */
class JetEtaPhiCleaningId{
 public:
	explicit JetEtaPhiCleaningId(const std::string & mapFilename, const std::string & mapHistname="h2hotfilter");
  ~JetEtaPhiCleaningId()=default;
	bool operator()(const Jet&,const uhh2::Event&) const;
 private:
	TH2D h_map;
};
