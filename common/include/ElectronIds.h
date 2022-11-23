#pragma once

#include <unordered_map>
#include <vector>

#include <UHH2/core/include/Event.h>
#include <UHH2/core/include/Utils.h>

// Note that the electronIds are implemented not by inheriting from a common base class, but
// rather as 'something' that can be used as function with the signature
//   bool electron_id(const Electron &, const Event &);
// This allows to write selections or other modules taking these ids as
//  std::function<bool (const Electron &, const Event &)>
// and allows to pass either function pointers or more complicated objects.
//
// Note that the id function takes the Event as argument; this might look unnecessary at
// first, but actually quite some ids require accessing other event information, e.g. lepton
// ids require pileup density event.rho to calculate corrected PF isolation, or the jet-lepton
// DeltaR and ptrel, etc.

// Electron selector for "pT + supercluster_eta" cuts ----------------
class PtEtaSCCut {
 public:
  PtEtaSCCut(float min_pt_, float max_etaSC_): min_pt(min_pt_), max_etaSC(max_etaSC_) {}

  bool operator()(const Electron& ele, const uhh2::Event&) const {
    /* std::cout<<"ele.pt() = "<<ele.pt()<<std::endl; */
    /* std::cout<<"ele.supercluster_eta() = "<<ele.supercluster_eta()<<std::endl; */
    return (ele.pt() > min_pt) && (fabs(ele.supercluster_eta()) < max_etaSC);
  }

 private:
  float min_pt, max_etaSC;
};

// Generic selector for the stored Electron tags

class ElectronTagID{
 public:
  ElectronTagID(Electron::tag sel_): sel(sel_) {}

  bool operator()(const Electron & el, const uhh2::Event &) const {
    return el.get_tag(sel); // no need to check, done in Tag class
  }

 private:
  Electron::tag sel;

};

// Electron ID to clean out "crack" electrons ------------------------
class ElectronEtaWindowId {
  // Electron selector for non-crack electrons
  public:
    ElectronEtaWindowId() {}

    bool operator()(const Electron & ele, const uhh2::Event &) const {
      float eta = abs(ele.eta());
      return (eta < eta_window_lower) || (eta > eta_window_upper);
    }

  private:
    float eta_window_upper = 1.5560;
    float eta_window_lower = 1.4442;
};

// Electron selector for PF MINI-Isolation ---------------------------
class Electron_MINIIso {

 public:
  explicit Electron_MINIIso(const float iso_cut, const std::string& iso_key): iso_cut_(iso_cut), iso_key_(iso_key) {}
  virtual ~Electron_MINIIso() {}

  bool operator()(const Electron&, const uhh2::Event&) const;

 protected:
  float iso_cut_;
  std::string iso_key_;
};

// ####################
// #### DEPRECATED ####
// ####################
// All below IDs are deprecated and should no longer be used, instead us ID via tags from the Electron.h class!

// Electron IDs ------------------------------------------------------


// --- Cut-Based ID
bool Electron_CutBasedID(const Electron&, const uhh2::Event&, const std::string&, const std::string&, const bool);
bool Electron_CutBasedID(const Electron&, const uhh2::Event&, const std::string&, const std::string&, const int, const bool);



// --- Cut-Based ID: Summer16
bool ElectronID_Summer16_veto        (const Electron&, const uhh2::Event&);
bool ElectronID_Summer16_veto_noIso  (const Electron&, const uhh2::Event&);
bool ElectronID_Summer16_loose       (const Electron&, const uhh2::Event&);
bool ElectronID_Summer16_loose_noIso (const Electron&, const uhh2::Event&);
bool ElectronID_Summer16_medium      (const Electron&, const uhh2::Event&);
bool ElectronID_Summer16_medium_noIso(const Electron&, const uhh2::Event&);
bool ElectronID_Summer16_tight       (const Electron&, const uhh2::Event&);
bool ElectronID_Summer16_tight_noIso (const Electron&, const uhh2::Event&);

// --- Cut-Based ID: Fall17
bool ElectronID_Fall17_veto        (const Electron&, const uhh2::Event&);
bool ElectronID_Fall17_veto_noIso  (const Electron&, const uhh2::Event&);
bool ElectronID_Fall17_loose       (const Electron&, const uhh2::Event&);
bool ElectronID_Fall17_loose_noIso (const Electron&, const uhh2::Event&);
bool ElectronID_Fall17_medium      (const Electron&, const uhh2::Event&);
bool ElectronID_Fall17_medium_noIso(const Electron&, const uhh2::Event&);
bool ElectronID_Fall17_tight       (const Electron&, const uhh2::Event&);
bool ElectronID_Fall17_tight_noIso (const Electron&, const uhh2::Event&);

// --- Cut-Based ID: look-up table for ID working points
// REF https://twiki.cern.ch/twiki/bin/view/CMS/CutBasedElectronIdentificationRun2
namespace ElectronID {

  const std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<std::string, std::vector<float> > > > CutBased_LUT = {


    /** Summer16 **/
    {"Summer16", {

	{"barrel", {
                          /* Veto   , Loose   , Medium  , Tight   */
	{"sigmaIetaIeta" , { 0.0115, 0.011, 	0.00998,   0.00998  }},
        {"|dEtaIn|"      , { 0.00749, 0.00477, 	0.00311,  0.00308 }},
        {"|dPhiIn|"      , { 0.228, 	0.222, 	0.103, 	0.0816  }},
        {"HoverE_C0"     , { 0.356, 	0.298, 	0.253, 	0.0414  }},
        {"HoverE_C1"     , { 0.0, 	0.0,    0.0,    0.0     }},
        {"HoverE_C2"     , { 0.0, 	0.0,    0.0,    0.0     }},
        {"relIsoEA_C0"   , { 0.175, 	0.0994, 0.0695, 0.0588  }},
        {"relIsoEA_C1"   , { 0.0, 	0.0,    0.0,    0.0     }},
        {"|ooEmooP|"     , { 0.299, 	0.241, 	0.134, 	0.0129  }},
        {"|d0|"          , { float(uhh2::infinity), float(uhh2::infinity)  , float(uhh2::infinity)  , float(uhh2::infinity)  }},
        {"|dz|"          , { float(uhh2::infinity) , float(uhh2::infinity)    , float(uhh2::infinity)   , float(uhh2::infinity)  }},
        {"missingHits"   , { 2,       1, 	1, 	1   }},
        {"passConvVeto"  , { 1      , 1       , 1       , 1       }},
       },
      },

	{"endcap", {
                          /* Veto   , Loose   , Medium  , Tight   */
	{"sigmaIetaIeta" , { 0.037, 	0.0314,   0.0298,     0.0292 }},
        {"|dEtaIn|"      , { 0.00895, 	0.00868,  0.00609,    0.00605 }},
        {"|dPhiIn|"      , { 0.213, 	0.213, 	  0.045,      0.0394  }},
        {"HoverE_C0"     , { 0.211, 	0.101, 	  0.0878,     0.0641 }},
        {"HoverE_C1"     , { 0.0, 	0.0,    0.0,    0.0     }},
        {"HoverE_C2"     , { 0.0, 	0.0,    0.0,    0.0     }},
        {"relIsoEA_C0"   , { 0.159, 	0.107, 	  0.0821,     0.0571  }},
        {"relIsoEA_C1"   , { 0.0, 	0.0,    0.0,    0.0     }},
        {"|ooEmooP|"     , { 0.15, 	0.14, 	  0.13,       0.0129}},
        {"|d0|"          , { float(uhh2::infinity)  , float(uhh2::infinity)   , float(uhh2::infinity)  , float(uhh2::infinity)  }},
        {"|dz|"          , { float(uhh2::infinity)  , float(uhh2::infinity)   , float(uhh2::infinity)   , float(uhh2::infinity)   }},
        {"missingHits"   , { 3      , 1       , 1       , 1       }},
        {"passConvVeto"  , { 1      , 1       , 1       , 1       }},
       },

      },
     },
    },
  /*******************/


    /** Fall17 according to https://twiki.cern.ch/twiki/bin/view/CMS/CutBasedElectronIdentificationRun2#Working_points_for_94X_and_later **/
    {"Fall17", {

	{"barrel", {
                          /* Veto   , Loose   , Medium  , Tight   */
	{"sigmaIetaIeta" , { 0.0126 , 0.0112  , 0.0106  , 0.0104  }},
        {"|dEtaIn|"      , { 0.00463, 0.00377 , 0.0032 , 0.00255 }},
        {"|dPhiIn|"      , { 0.148  , 0.0884  , 0.0547  , 0.022  }},
        {"HoverE_C0"     , { 0.05   , 0.05    , 0.046   , 0.026   }},
        {"HoverE_C1"     , { 1.16    , 1.16 , 1.16   , 1.15   }},
        {"HoverE_C2"     , { 0.0324  , 0.0324 , 0.0324   , 0.0324   }},
        {"relIsoEA_C0"   , { 0.198  , 0.112   , 0.0478  , 0.0287  }},
        {"relIsoEA_C1"   , { 0.506  , 0.506   , 0.506  , 0.506  }},
        {"|ooEmooP|"     , { 0.209  , 0.193   , 0.184  , 0.159  }},
        {"|d0|"          , { float(uhh2::infinity), float(uhh2::infinity)  , float(uhh2::infinity)  , float(uhh2::infinity)     }},
        {"|dz|"          , { float(uhh2::infinity) , float(uhh2::infinity)    , float(uhh2::infinity)   , float(uhh2::infinity) }},
        {"missingHits"   , { 2      , 1       , 1       , 1       }},
        {"passConvVeto"  , { 1      , 1       , 1       , 1       }},
       },
      },

	{"endcap", {
                          /* Veto   , Loose   , Medium  , Tight   */
	{"sigmaIetaIeta" , { 0.0457  , 0.0425 , 0.0387  , 0.0353  }},
	{"|dEtaIn|"      , { 0.00814 , 0.00674 , 0.00632 , 0.00501 }},
	{"|dPhiIn|"      , { 0.19   , 0.169  , 0.0394  , 0.0236  }},
        {"HoverE_C0"     , { 0.05    , 0.0441 , 0.0275   , 0.0188   }},
        {"HoverE_C1"     , { 2.54    , 2.54 , 2.52   , 2.06   }},
        {"HoverE_C2"     , { 0.183  , 0.183 , 0.183   , 0.183   }},
	{"relIsoEA_C0"   , { 0.203   , 0.108  , 0.0658   , 0.0445   }},
        {"relIsoEA_C1"   , { 0.963  , 0.963   , 0.963  , 0.963  }},
	{"|ooEmooP|"     , { 0.132  , 0.111 , 0.0721  , 0.0197  }},
        {"|d0|"          , { float(uhh2::infinity)  , float(uhh2::infinity)   , float(uhh2::infinity)  , float(uhh2::infinity)  }},
        {"|dz|"          , { float(uhh2::infinity)  , float(uhh2::infinity)   , float(uhh2::infinity)   , float(uhh2::infinity) }},
        {"missingHits"   , { 3       , 1      , 1       , 1       }},
        {"passConvVeto"  , { 1       , 1      , 1       , 1       }},
       },

      },
     },
    },
  /*******************/

  };

}

// Electron MVA IDs
// REF https://twiki.cern.ch/twiki/bin/view/CMS/MultivariateElectronIdentificationRun2

// --- MVA ID
bool Electron_MVAID(const Electron&, const uhh2::Event&, const std::string&, const std::string&, const bool);

// --- Specific MVA IDs: Fall17
bool ElectronID_MVA_Fall17_loose_iso(const Electron&, const uhh2::Event&);
bool ElectronID_MVA_Fall17_loose_noIso(const Electron&, const uhh2::Event&);


namespace ElectronID {

  const std::unordered_map<std::string, std::unordered_map<std::string, std::vector<float> > > MVA_LUT_Iso = {
    {"Fall17",
     {
      // currently only for wp loose
      {"low-pt_barrel1",  {  -0.13  }},
      {"low-pt_barrel2",  {  -0.32  }},
      {"low-pt_endcap" ,  {  -0.08  }},
      {"high-pt_barrel1", {  -0.86  }},
      {"high-pt_barrel2", {  -0.81  }},
      {"high-pt_endcap" , {  -0.72  }},
     }
    }
  };

  const std::unordered_map<std::string, std::unordered_map<std::string, std::vector<float> > > MVA_LUT_NoIso = {
    {"Fall17",
     {
      // currently only for wp loose
      {"low-pt_barrel1",  {  -0.10  }},
      {"low-pt_barrel2",  {  -0.28  }},
      {"low-pt_endcap" ,  {  -0.05  }},
      {"high-pt_barrel1", {  -0.83  }},
      {"high-pt_barrel2", {  -0.77  }},
      {"high-pt_endcap" , {  -0.69  }},
     }
    }
  };

}


// --- Electron HEEP ID
// REF https://twiki.cern.ch/twiki/bin/view/CMS/HEEPElectronIdentificationRun2
bool Electron_HEEP(const Electron&, const std::string&, const std::string&);
bool Electron_HEEP(const Electron&, const std::string&, const std::string&, const int);

bool ElectronID_HEEP_RunII_25ns(const Electron&, const uhh2::Event&);

namespace ElectronID {

  const std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<std::string, std::vector<float> > > > HEEP_LUT = {

  /** Spring 15 **/
    {"RunII_25ns", {
      {"barrel", {

        {"Et"            , { 35.   }},
        {"|dEtaInSeed|"  , { 0.004 }},
        {"|dPhiIn|"      , { 0.06  }},
        {"HoverE coef"   , { 1.    }},
        {"HoverE const"  , { 0.05  }},
        {"sigmaIEtaIEta" , { 1e6   }},
        {"E2x5 over E5x5", { 0.94  }},
        {"E1x5 over E5x5", { 0.83  }},
        {"missingHits"   , { 1     }},
        {"|dxy|"         , { 0.02  }},
	},
      },

      {"endcap", {

        {"Et"            , { 35.   }},
        {"|dEtaInSeed|"  , { 0.006 }},
        {"|dPhiIn|"      , { 0.06  }},
        {"HoverE coef"   , { 5.    }},
        {"HoverE const"  , { 0.05  }},
        {"sigmaIEtaIEta" , { 0.03  }},
        {"E2x5 over E5x5", { -1.   }},
        {"E1x5 over E5x5", { -1.   }},
        {"missingHits"   , { 1     }},
        {"|dxy|"         , { 0.05  }},
       },
      },

     },
    },
  /***************/

  };

}
