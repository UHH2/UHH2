#pragma once

#include <unordered_map>
#include <vector>

#include <UHH2/core/include/Event.h>

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

// Electron IDs ------------------------------------------------------


// --- Cut-Based ID
bool Electron_CutBasedID(const Electron&, const uhh2::Event&, const std::string&, const std::string&, const bool);
bool Electron_CutBasedID(const Electron&, const uhh2::Event&, const std::string&, const std::string&, const int, const bool);

// --- Cut-Based ID: PHYS14 25ns
bool ElectronID_PHYS14_25ns_veto        (const Electron&, const uhh2::Event&);
bool ElectronID_PHYS14_25ns_veto_noIso  (const Electron&, const uhh2::Event&);
bool ElectronID_PHYS14_25ns_loose       (const Electron&, const uhh2::Event&);
bool ElectronID_PHYS14_25ns_loose_noIso (const Electron&, const uhh2::Event&);
bool ElectronID_PHYS14_25ns_medium      (const Electron&, const uhh2::Event&);
bool ElectronID_PHYS14_25ns_medium_noIso(const Electron&, const uhh2::Event&);
bool ElectronID_PHYS14_25ns_tight       (const Electron&, const uhh2::Event&);
bool ElectronID_PHYS14_25ns_tight_noIso (const Electron&, const uhh2::Event&);

// --- Cut-Based ID: Spring15 50ns
bool ElectronID_Spring15_50ns_veto        (const Electron&, const uhh2::Event&);
bool ElectronID_Spring15_50ns_veto_noIso  (const Electron&, const uhh2::Event&);
bool ElectronID_Spring15_50ns_loose       (const Electron&, const uhh2::Event&);
bool ElectronID_Spring15_50ns_loose_noIso (const Electron&, const uhh2::Event&);
bool ElectronID_Spring15_50ns_medium      (const Electron&, const uhh2::Event&);
bool ElectronID_Spring15_50ns_medium_noIso(const Electron&, const uhh2::Event&);
bool ElectronID_Spring15_50ns_tight       (const Electron&, const uhh2::Event&);
bool ElectronID_Spring15_50ns_tight_noIso (const Electron&, const uhh2::Event&);

// --- Cut-Based ID: Spring15 25ns
bool ElectronID_Spring15_25ns_veto        (const Electron&, const uhh2::Event&);
bool ElectronID_Spring15_25ns_veto_noIso  (const Electron&, const uhh2::Event&);
bool ElectronID_Spring15_25ns_loose       (const Electron&, const uhh2::Event&);
bool ElectronID_Spring15_25ns_loose_noIso (const Electron&, const uhh2::Event&);
bool ElectronID_Spring15_25ns_medium      (const Electron&, const uhh2::Event&);
bool ElectronID_Spring15_25ns_medium_noIso(const Electron&, const uhh2::Event&);
bool ElectronID_Spring15_25ns_tight       (const Electron&, const uhh2::Event&);
bool ElectronID_Spring15_25ns_tight_noIso (const Electron&, const uhh2::Event&);

// --- Cut-Based ID: look-up table for ID working points
// REF https://twiki.cern.ch/twiki/bin/view/CMS/CutBasedElectronIdentificationRun2
namespace ElectronID {

  const std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<std::string, std::vector<float> > > > CutBased_LUT = {

  /** PHYS14_25ns **/
    {"PHYS14_25ns", {

      {"barrel", {
                          /* Veto     , Loose    , Medium   , Tight    */
        {"sigmaIetaIeta" , { 0.011586 , 0.010331 , 0.009996 , 0.009947 }},
        {"|dEtaIn|"      , { 0.013625 , 0.009277 , 0.008925 , 0.006046 }},
        {"|dPhiIn|"      , { 0.230374 , 0.094739 , 0.035973 , 0.028092 }},
        {"HoverE"        , { 0.181130 , 0.093068 , 0.050537 , 0.045772 }},
        {"relIsoEA"      , { 0.158721 , 0.130136 , 0.107587 , 0.069537 }},
        {"|ooEmooP|"     , { 0.295751 , 0.189968 , 0.091942 , 0.020118 }},
        {"|d0|"          , { 0.094095 , 0.035904 , 0.012235 , 0.008790 }},
        {"|dz|"          , { 0.713070 , 0.075496 , 0.042020 , 0.021226 }},
        {"missingHits"   , { 2        , 1        , 1        , 1        }},
        {"passConvVeto"  , { 1        , 1        , 1        , 1        }},
       },
      },

      {"endcap", {
                          /* Veto     , Loose    , Medium   , Tight    */
        {"sigmaIetaIeta" , { 0.031849 , 0.031838 , 0.030135 , 0.028237 }},
        {"|dEtaIn|"      , { 0.011932 , 0.009833 , 0.007429 , 0.007057 }},
        {"|dPhiIn|"      , { 0.255450 , 0.149934 , 0.067879 , 0.030159 }},
        {"HoverE"        , { 0.223870 , 0.115754 , 0.086782 , 0.067778 }},
        {"relIsoEA"      , { 0.177032 , 0.163368 , 0.113254 , 0.078265 }},
        {"|ooEmooP|"     , { 0.155501 , 0.140662 , 0.100683 , 0.098919 }},
        {"|d0|"          , { 0.342293 , 0.099266 , 0.036719 , 0.027984 }},
        {"|dz|"          , { 0.953461 , 0.197897 , 0.138142 , 0.133431 }},
        {"missingHits"   , { 3        , 1        , 1        , 1        }},
        {"passConvVeto"  , { 1        , 1        , 1        , 1        }},
       },

      },
     },
    },
  /*****************/

  /** Spring15_50ns **/
    {"Spring15_50ns", {

      {"barrel", {
                          /* Veto   , Loose   , Medium  , Tight   */
        {"sigmaIetaIeta" , { 0.012  , 0.0105  , 0.0101  , 0.0101  }},
        {"|dEtaIn|"      , { 0.0126 , 0.00976 , 0.0094  , 0.00864 }},
        {"|dPhiIn|"      , { 0.107  , 0.0929  , 0.0296  , 0.0286  }},
        {"HoverE"        , { 0.186  , 0.0765  , 0.0372  , 0.0342  }},
        {"relIsoEA"      , { 0.161  , 0.118   , 0.0987  , 0.0591  }},
        {"|ooEmooP|"     , { 0.239  , 0.184   , 0.118   , 0.0116  }},
        {"|d0|"          , { 0.0621 , 0.0227  , 0.0151  , 0.0103  }},
        {"|dz|"          , { 0.613  , 0.379   , 0.238   , 0.170   }},
        {"missingHits"   , { 2      , 2       , 2       , 2       }},
        {"passConvVeto"  , { 1      , 1       , 1       , 1       }},
       },
      },

      {"endcap", {
                          /* Veto   , Loose   , Medium  , Tight   */
        {"sigmaIetaIeta" , { 0.0339 , 0.0318  , 0.0287  , 0.0287  }},
        {"|dEtaIn|"      , { 0.0109 , 0.00952 , 0.00773 , 0.00762 }},
        {"|dPhiIn|"      , { 0.219  , 0.181   , 0.148   , 0.0439  }},
        {"HoverE"        , { 0.0962 , 0.0824  , 0.0546  , 0.0544  }},
        {"relIsoEA"      , { 0.193  , 0.118   , 0.0902  , 0.0759  }},
        {"|ooEmooP|"     , { 0.141  , 0.125   , 0.104   , 0.01    }},
        {"|d0|"          , { 0.279  , 0.242   , 0.0535  , 0.0377  }},
        {"|dz|"          , { 0.947  , 0.921   , 0.572   , 0.571   }},
        {"missingHits"   , { 3      , 1       , 1       , 1       }},
        {"passConvVeto"  , { 1      , 1       , 1       , 1       }},
       },

      },
     },
    },
  /*******************/

  /** Spring15_25ns **/
    {"Spring15_25ns", {

      {"barrel", {
                          /* Veto   , Loose   , Medium  , Tight   */
        {"sigmaIetaIeta" , { 0.0114 , 0.0103  , 0.0101  , 0.0101  }},
        {"|dEtaIn|"      , { 0.0152 , 0.0105  , 0.0103  , 0.00926 }},
        {"|dPhiIn|"      , { 0.216  , 0.115   , 0.0336  , 0.0336  }},
        {"HoverE"        , { 0.181  , 0.104   , 0.0876  , 0.0597  }},
        {"relIsoEA"      , { 0.126  , 0.0893  , 0.0766  , 0.0354  }},
        {"|ooEmooP|"     , { 0.207  , 0.102   , 0.0174  , 0.012   }},
        {"|d0|"          , { 0.0564 , 0.0261  , 0.0118  , 0.0111  }},
        {"|dz|"          , { 0.472  , 0.41    , 0.373   , 0.0466  }},
        {"missingHits"   , { 2      , 2       , 2       , 2       }},
        {"passConvVeto"  , { 1      , 1       , 1       , 1       }},
       },
      },

      {"endcap", {
                          /* Veto   , Loose   , Medium  , Tight   */
        {"sigmaIetaIeta" , { 0.0352 , 0.0301  , 0.0283  , 0.0279  }},
        {"|dEtaIn|"      , { 0.0113 , 0.00814 , 0.00733 , 0.00724 }},
        {"|dPhiIn|"      , { 0.237  , 0.182   , 0.114   , 0.0918  }},
        {"HoverE"        , { 0.116  , 0.0897  , 0.0678  , 0.0615  }},
        {"relIsoEA"      , { 0.144  , 0.121   , 0.0678  , 0.0646  }},
        {"|ooEmooP|"     , { 0.174  , 0.126   , 0.0898  , 0.00999 }},
        {"|d0|"          , { 0.222  , 0.118   , 0.0739  , 0.0351  }},
        {"|dz|"          , { 0.921  , 0.822   , 0.602   , 0.417   }},
        {"missingHits"   , { 3      , 1       , 1       , 1       }},
        {"passConvVeto"  , { 1      , 1       , 1       , 1       }},
       },

      },
     },
    },
  /*******************/

  };

}

// Electron MVA IDs
// REF https://twiki.cern.ch/twiki/bin/view/CMS/MultivariateElectronIdentificationRun2

// --- Non-Triggering MVA ID
bool Electron_NonTrigMVAID(const Electron&, const uhh2::Event&, const std::string&, const std::string&);

// --- Non-Triggering MVA ID: PHYS14
bool ElectronID_MVAnotrig_PHYS14_loose(const Electron&, const uhh2::Event&);
bool ElectronID_MVAnotrig_PHYS14_tight(const Electron&, const uhh2::Event&);

// --- Non-Triggering MVA ID: Spring15
bool ElectronID_MVAnotrig_Spring15_25ns_loose(const Electron&, const uhh2::Event&);
bool ElectronID_MVAnotrig_Spring15_25ns_tight(const Electron&, const uhh2::Event&);

namespace ElectronID {

  const std::unordered_map<std::string, std::unordered_map<std::string, std::vector<float> > > NonTrigMVA_LUT = {

  /** PHYS14 **/
    {"PHYS14", {
                         /* 80% SE , 90% SE */
      {"low-pt_barrel1" , { -0.253 , -0.483 }},
      {"low-pt_barrel2" , {  0.081 , -0.267 }},
      {"low-pt_endcap"  , { -0.081 , -0.323 }},
      {"high-pt_barrel1", {  0.965 ,  0.933 }},
      {"high-pt_barrel2", {  0.917 ,  0.825 }},
      {"high-pt_endcap" , {  0.683 ,  0.337 }},

     },
    },
  /************/

  /** Spring15 **/
    {"Spring15", {
                         /* 80% SE    , 90% SE    */
      {"low-pt_barrel1" , {  0.287435 , -0.083313 }},
      {"low-pt_barrel2" , {  0.221846 , -0.235222 }},
      {"low-pt_endcap"  , { -0.303263 , -0.67099  }},
      {"high-pt_barrel1", {  0.967083 ,  0.913286 }},
      {"high-pt_barrel2", {  0.929117 ,  0.805013 }},
      {"high-pt_endcap" , {  0.726311 ,  0.358969 }},

     },
    },
  /**************/

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
