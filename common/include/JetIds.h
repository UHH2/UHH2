#pragma once

#include <boost/optional.hpp>
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
    WP_TIGHT  = 2
  };
  explicit BTag(algo tagger, wp working_point);

  bool operator()(const Jet & jet, const uhh2::Event & event) const {return jet_id(jet,event);}

  std::string GetTagger() const {return m_algo;}
  int GetWorkingPoint() const {return m_working_point;}
private:
  std::string m_algo;
  int m_working_point;
  JetId jet_id;
};

class CSVBTag {
public:
  enum wp {WP_LOOSE  = 0, WP_MEDIUM = 1, WP_TIGHT  = 2};

  explicit CSVBTag(wp working_point);
  explicit CSVBTag(float float_point);

  bool operator()(const Jet & jet, const uhh2::Event & event);
private:
  float csv_threshold;
  wp m_working_point;
};

class DeepCSVBTag {
public:
  enum wp {WP_LOOSE  = 0, WP_MEDIUM = 1, WP_TIGHT  = 2 };

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
    WP_TIGHT  = 2
  };

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
  enum wp {WP_LOOSE_CHS, WP_TIGHT_CHS, WP_LOOSE_PUPPI, WP_TIGHT_PUPPI, WP_TIGHT_LEPVETO, WP_TIGHT_LEPVETO_CHS, WP_TIGHT_LEPVETO_PUPPI}; // keep WP_TIGHT_LEPVETO for some backward compatibility; use of WP_TIGHT_LEPVETO_CHS/PUPPI is encouraged
  explicit JetPFID(wp working_point);
  bool operator()(const Jet&, const uhh2::Event&) const;
private:
  wp m_working_point;

  //UL taken from https://twiki.cern.ch/twiki/bin/view/CMS/JetID13TeVUL
  // Differences between UL16 and UL17/UL18
  bool tightIDUL16_CHS(const Jet & jet) const;
  bool tightIDUL16_PUPPI(const Jet & jet) const;
  bool tightLepVetoIDUL16_CHS(const Jet & jet) const;
  bool tightLepVetoIDUL16_PUPPI(const Jet & jet) const;
  bool tightIDUL1718_CHS(const Jet & jet) const;
  bool tightIDUL1718_PUPPI(const Jet & jet) const;
  bool tightLepVetoIDUL1718_CHS(const Jet & jet) const;
  bool tightLepVetoIDUL1718_PUPPI(const Jet & jet) const;

  //2016 taken from https://twiki.cern.ch/twiki/bin/view/CMS/JetID13TeVRun2016
  bool looseID2016_CHS(const Jet & jet) const;
  bool tightID2016_CHS(const Jet & jet) const;
  bool looseID2016_PUPPI(const Jet & jet) const;
  bool tightID2016_PUPPI(const Jet & jet) const;
  bool tightLepVetoID2016(const Jet & jet) const; // same for CHS and PUPPI in 2016

  //2017 taken from https://twiki.cern.ch/twiki/bin/view/CMS/JetID13TeVRun2017
  bool tightID2017_CHS(const Jet & jet) const;
  bool tightID2017_PUPPI(const Jet & jet) const;
  bool tightLepVetoID2017(const Jet & jet) const; // same for CHS and PUPPI in 2017

  //2018 taken from https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetID13TeVRun2018
  bool tightID2018_CHS(const Jet & jet) const;
  bool tightID2018_PUPPI(const Jet & jet) const;
  bool tightLepVetoID2018_CHS(const Jet & jet) const;
  bool tightLepVetoID2018_PUPPI(const Jet & jet) const;

  //2022 taken from https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetID13p6TeV
  // bool tightID2022_CHS(const Jet & jet) const;
  bool tightID2022_PUPPI(const Jet & jet) const;
  // bool tightLepVetoID2022_CHS(const Jet & jet) const;
  // bool tightLepVetoID2022_PUPPI(const Jet & jet) const;
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
* Jet EtaPhiCut Id rejects jets, selected according to etaphi map
* http://hsiikone.web.cern.ch/hsiikone/hotcoldjets/
*/
class HotZoneVetoId {
public:
  explicit HotZoneVetoId();
  bool operator()(const Jet&, const uhh2::Event&) const;
private:
  std::map<std::string, std::map<std::string, TH2D*>> h2HotExcl;

  std::map<std::string, std::map<std::string, std::vector<std::string>>> info = {
    { "UL16preVFP", {
      { "fname",  {"JECDatabase/jet_veto_maps/Summer19UL16_V0/hotjets-UL16.root", "JECDatabase/jet_veto_maps/Summer19UL16_V0/hotjets-UL16.root"}},
      { "hname",  {"h2hot_ul16_plus_hbm2_hbp12_qie11", "h2hot_mc"}},
    }},
    { "UL16postVFP", {
      { "fname",  {"JECDatabase/jet_veto_maps/Summer19UL16_V0/hotjets-UL16.root", "JECDatabase/jet_veto_maps/Summer19UL16_V0/hotjets-UL16.root"}},
      { "hname",  {"h2hot_ul16_plus_hbm2_hbp12_qie11", "h2hot_mc"}},
    }},
    { "UL17", {
      { "fname",  {"JECDatabase/jet_veto_maps/Summer19UL17_V2/hotjets-UL17_v2.root"}},
      { "hname",  {"h2hot_ul17_plus_hep17_plus_hbpw89"}},
    }},
    { "UL18", {
      { "fname",  {"JECDatabase/jet_veto_maps/Summer19UL18_V1/hotjets-UL18.root"}},
      { "hname",  {"h2hot_ul18_plus_hem1516_and_hbp2m1"}},
    }},
    { "UL18", {
      { "fname",  {"JECDatabase/jet_veto_maps/Summer19UL18_V1/hotjets-UL18.root"}},
      { "hname",  {"h2hot_ul18_plus_hem1516_and_hbp2m1"}},
    }},
    { "2022preEE", {
      { "fname",  {"JECDatabase/jet_veto_maps/Summer22_23Sep2023/Summer22_23Sep2023_RunCD_v1.root"}},
      { "hname",  {"jetvetomap"}},
    }},
    { "2022postEE", {
      { "fname",  {"JECDatabase/jet_veto_maps/Summer22EE_23Sep2023/Summer22EE_23Sep2023_RunEFG_v1.root"}},
      { "hname",  {"jetvetomap"}},
    }},
    { "2023preBPix", {
      { "fname",  {"JECDatabase/jet_veto_maps/Summer23Prompt23/Summer23Prompt23_RunC_v1.root"}},
      { "hname",  {"jetvetomap"}},
    }},
    { "2023postBPix", {
      { "fname",  {"JECDatabase/jet_veto_maps/Summer23BPixPrompt23/Summer23BPixPrompt23_RunD_v1.root"}},
      { "hname",  {"jetvetomap"}},
    }},
    { "Run3_CD", {
      { "fname",  {"JECDatabase/jet_veto_maps/Winter22Run3/Winter22Run3_RunCD_v1.root"}},
      { "hname",  {"jetvetomap"}},
    }},
  };

};


/**
* Check presence of at least one lepton inside the jet
*/
class NoLeptonInJet{
public:
  explicit NoLeptonInJet(const std::string& lepton_ = "all", const boost::optional<ElectronId> & ele_id_ = boost::none, const boost::optional<MuonId> & muo_id_ = boost::none, const boost::optional<double> & drmax_ = boost::none);
  ~NoLeptonInJet()=default;
  bool operator()(const Jet&,const uhh2::Event&) const;
private:
  std::string lepton;
  boost::optional<ElectronId> ele_id;
  boost::optional<MuonId> muo_id;
  boost::optional<double> drmax;

};
