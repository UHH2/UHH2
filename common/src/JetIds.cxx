#include "UHH2/common/include/JetIds.h"

#include "UHH2/common/include/Utils.h"

using namespace std;
using namespace uhh2;

BTag::BTag(algo tagger, wp working_point) {
  m_working_point = (int) working_point;
  switch (tagger) {
    case DEEPCSV :
    jet_id = DeepCSVBTag((DeepCSVBTag::wp) m_working_point);
    m_algo = "DeepCSV";
    break;
    case DEEPJET :
    jet_id = DeepJetBTag((DeepJetBTag::wp) m_working_point);
    m_algo = "DeepJet";
    break;
    default:
    throw invalid_argument("invalid b-tagging algorithm passed to BTag");
  }
}

///
DeepCSVBTag::DeepCSVBTag(wp working_point) {
  m_working_point = working_point;
}

DeepCSVBTag::DeepCSVBTag(float float_point):deepcsv_threshold(float_point) {}


bool DeepCSVBTag::operator()(const Jet & jet, const Event &ev){
  if(ev.year == "2016v2"){
    //https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation80XReReco
    switch(m_working_point){
      case WP_LOOSE:
      deepcsv_threshold = 0.2219;
      break;
      case WP_MEDIUM:
      deepcsv_threshold = 0.6324;
      break;
      case WP_TIGHT:
      deepcsv_threshold = 0.8958;
      break;
      default:
      throw invalid_argument("invalid working point passed to DeepCSVBTag");
    }
  }
  else if(ev.year == "2016v3"){
    //https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation2016Legacy
    switch(m_working_point){
      case WP_LOOSE:
      deepcsv_threshold = 0.2217;
      break;
      case WP_MEDIUM:
      deepcsv_threshold = 0.6321;
      break;
      case WP_TIGHT:
      deepcsv_threshold = 0.8953;
      break;
      default:
      throw invalid_argument("invalid working point passed to DeepCSVBTag");
    }
  }
  else if(ev.year == "UL16preVFP") {
    //https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation106XUL16preVFP
    switch(m_working_point){
      case WP_LOOSE:
      deepcsv_threshold = 0.2027;
      break;
      case WP_MEDIUM:
      deepcsv_threshold = 0.6001;
      break;
      case WP_TIGHT:
      deepcsv_threshold = 0.8819;
      break;
      default:
      throw invalid_argument("invalid working point passed to DeepCSVBTag");
    }
  }
  else if(ev.year == "UL16postVFP") {
    //https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation106XUL16postVFP
    switch(m_working_point){
      case WP_LOOSE:
      deepcsv_threshold = 0.1918;
      break;
      case WP_MEDIUM:
      deepcsv_threshold = 0.5847;
      break;
      case WP_TIGHT:
      deepcsv_threshold = 0.8767;
      break;
      default:
      throw invalid_argument("invalid working point passed to DeepCSVBTag");
    }
  }
  else if(ev.year.find("17") != string::npos){
    if(ev.year.find("UL") != string::npos){
      //https://twiki.cern.ch/twiki/bin/view/CMS/BtagRecommendation106XUL17
      switch(m_working_point){
        case WP_LOOSE:
        deepcsv_threshold = 0.1355;
        break;
        case WP_MEDIUM:
        deepcsv_threshold = 0.4506;
        break;
        case WP_TIGHT:
        deepcsv_threshold = 0.7738;
        break;
        default:
        throw invalid_argument("invalid working point passed to DeepCSVBTag");
      }
    }
    else {
      //https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation94X
      switch(m_working_point){
        case WP_LOOSE:
        deepcsv_threshold = 0.1522;
        break;
        case WP_MEDIUM:
        deepcsv_threshold = 0.4941;
        break;
        case WP_TIGHT:
        deepcsv_threshold = 0.8001;
        break;
        default:
        throw invalid_argument("invalid working point passed to DeepCSVBTag");
      }
    }
  }
  else if(ev.year.find("18") != string::npos){
    if(ev.year.find("UL") != string::npos){
      //https://twiki.cern.ch/twiki/bin/view/CMS/BtagRecommendation106XUL18
      switch(m_working_point){
        case WP_LOOSE:
        deepcsv_threshold = 0.1208;
        break;
        case WP_MEDIUM:
        deepcsv_threshold = 0.4168;
        break;
        case WP_TIGHT:
        deepcsv_threshold = 0.7665;
        break;
        default:
        throw invalid_argument("invalid working point passed to DeepCSVBTag");
      }
    }
    else {
      //https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation102X
      switch(m_working_point){
        case WP_LOOSE:
        deepcsv_threshold = 0.1241;
        break;
        case WP_MEDIUM:
        deepcsv_threshold = 0.4184;
        break;
        case WP_TIGHT:
        deepcsv_threshold = 0.7527;
        break;
        default:
        throw invalid_argument("invalid working point passed to DeepCSVBTag");
      }
    }
  } else {
    cout << "Invalid year, DeepCSVBTag returning false" << endl;
    return false;
  }
  return jet.btag_DeepCSV() > deepcsv_threshold;
}

//DeepJet (=DeepFlavor)
DeepJetBTag::DeepJetBTag(wp working_point) {
  m_working_point = working_point;
}

DeepJetBTag::DeepJetBTag(float float_point):deepjet_threshold(float_point) {}

bool DeepJetBTag::operator()(const Jet & jet, const Event &ev){
  if(ev.year == "2016v2" || ev.year == "2016v3"){
    //https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation2016Legacy
    //Note: wp are provided for 2016 legacy only, i.e 2016v3
    switch(m_working_point){
      case WP_LOOSE:
      deepjet_threshold = 0.0614;
      break;
      case WP_MEDIUM:
      deepjet_threshold = 0.3093;
      break;
      case WP_TIGHT:
      deepjet_threshold = 0.7221;
      break;
      default:
      throw invalid_argument("invalid working point passed to DeepJetBTag");
    }
  }
  else if(ev.year == "UL16preVFP") {
    //https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation106XUL16preVFP
    switch(m_working_point){
      case WP_LOOSE:
      deepjet_threshold = 0.0508;
      break;
      case WP_MEDIUM:
      deepjet_threshold = 0.2598;
      break;
      case WP_TIGHT:
      deepjet_threshold = 0.6502;
      break;
      default:
      throw invalid_argument("invalid working point passed to DeepJetBTag");
    }
  }
  else if(ev.year == "UL16postVFP") {
    //https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation106XUL16postVFP
    switch(m_working_point){
      case WP_LOOSE:
      deepjet_threshold = 0.0480;
      break;
      case WP_MEDIUM:
      deepjet_threshold = 0.2489;
      break;
      case WP_TIGHT:
      deepjet_threshold = 0.6377;
      break;
      default:
      throw invalid_argument("invalid working point passed to DeepJetBTag");
    }
  }
  else if(ev.year.find("17") != string::npos){
    if(ev.year.find("UL") != string::npos){
      //https://twiki.cern.ch/twiki/bin/view/CMS/BtagRecommendation106XUL17
      switch(m_working_point){
        case WP_LOOSE:
        deepjet_threshold = 0.0532;
        break;
        case WP_MEDIUM:
        deepjet_threshold = 0.3040;
        break;
        case WP_TIGHT:
        deepjet_threshold = 0.7476;
        break;
        default:
        throw invalid_argument("invalid working point passed to DeepJetBTag");
      }
    }
    else {
      //https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation94X
      switch(m_working_point){
        case WP_LOOSE:
        deepjet_threshold = 0.0521;
        break;
        case WP_MEDIUM:
        deepjet_threshold = 0.3033;
        break;
        case WP_TIGHT:
        deepjet_threshold = 0.7489;
        break;
        default:
        throw invalid_argument("invalid working point passed to DeepJetBTag");
      }
    }
  }
  else if(ev.year.find("18") != string::npos){
    if(ev.year.find("UL") != string::npos){
      //https://twiki.cern.ch/twiki/bin/view/CMS/BtagRecommendation106XUL18
      switch(m_working_point){
        case WP_LOOSE:
        deepjet_threshold = 0.0490;
        break;
        case WP_MEDIUM:
        deepjet_threshold = 0.2783;
        break;
        case WP_TIGHT:
        deepjet_threshold = 0.7100;
        break;
        default:
        throw invalid_argument("invalid working point passed to DeepJetBTag");
      }
    }
    else {
      //https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation102X
      switch(m_working_point){
        case WP_LOOSE:
        deepjet_threshold = 0.0494;
        break;
        case WP_MEDIUM:
        deepjet_threshold = 0.2770;
        break;
        case WP_TIGHT:
        deepjet_threshold = 0.7264;
        break;
        default:
        throw invalid_argument("invalid working point passed to DeepJetBTag");
      }
    }
  } else {
    cout << "Invalid year, DeepJetBTag returning false" << endl;
    return false;
  }
  return jet.btag_DeepJet() > deepjet_threshold;
}

///
JetPFID::JetPFID(wp working_point):m_working_point(working_point){}

bool JetPFID::operator()(const Jet & jet, const Event & ev) const{
  if(ev.year.find("UL") != string::npos){
    // https://twiki.cern.ch/twiki/bin/view/CMS/JetID13TeVUL
    if(ev.year.find("16") != string::npos){
      switch(m_working_point){
        case WP_TIGHT_CHS:
        return tightIDUL16_CHS(jet);
        case WP_TIGHT_PUPPI:
        return tightIDUL16_PUPPI(jet);
        case WP_TIGHT_LEPVETO_CHS:
        return tightLepVetoIDUL16_CHS(jet);
        case WP_TIGHT_LEPVETO_PUPPI:
        return tightLepVetoIDUL16_PUPPI(jet);
        case WP_TIGHT_LEPVETO:
        throw invalid_argument("In UL, the LepVeto JetPFID is not the same for CHS and PUPPI. Please specify either CHS or PUPPI working point.");
        default:
        throw invalid_argument("invalid working point passed to JetPFID");
      }
    }
    else{
      switch(m_working_point){
        case WP_TIGHT_CHS:
        return tightIDUL1718_CHS(jet);
        case WP_TIGHT_PUPPI:
        return tightIDUL1718_PUPPI(jet);
        case WP_TIGHT_LEPVETO_CHS:
        return tightLepVetoIDUL1718_CHS(jet);
        case WP_TIGHT_LEPVETO_PUPPI:
        return tightLepVetoIDUL1718_PUPPI(jet);
        case WP_TIGHT_LEPVETO:
        throw invalid_argument("In UL, the LepVeto JetPFID is not the same for CHS and PUPPI. Please specify either CHS or PUPPI working point.");
        default:
        throw invalid_argument("invalid working point passed to JetPFID");
      }
    }
  }
  else if (ev.year.find("2016") != string::npos){
    switch(m_working_point){
      case WP_LOOSE_CHS:
      return looseID2016_CHS(jet);
      case WP_TIGHT_CHS:
      return tightID2016_CHS(jet);
      case WP_LOOSE_PUPPI:
      return looseID2016_PUPPI(jet);
      case WP_TIGHT_PUPPI:
      return tightID2016_PUPPI(jet);
      case WP_TIGHT_LEPVETO:
      return tightLepVetoID2016(jet);
      case WP_TIGHT_LEPVETO_CHS:
      return tightLepVetoID2016(jet);
      case WP_TIGHT_LEPVETO_PUPPI:
      return tightLepVetoID2016(jet);
      default:
      throw invalid_argument("invalid working point passed to JetPFID");
    }
  }
  else if (ev.year.find("2017") != string::npos){
    switch(m_working_point){
      case WP_TIGHT_CHS:
      return tightID2017_CHS(jet);
      case WP_TIGHT_PUPPI:
      return tightID2017_PUPPI(jet);
      case WP_TIGHT_LEPVETO:
      return tightLepVetoID2017(jet);
      case WP_TIGHT_LEPVETO_CHS:
      return tightLepVetoID2017(jet);
      case WP_TIGHT_LEPVETO_PUPPI:
      return tightLepVetoID2017(jet);
      default:
      throw invalid_argument("invalid working point passed to JetPFID");
    }
  }
  else if (ev.year.find("2018") != string::npos){
    switch(m_working_point){
      case WP_TIGHT_CHS:
      return tightID2018_CHS(jet);
      case WP_TIGHT_PUPPI:
      return tightID2018_PUPPI(jet);
      case WP_TIGHT_LEPVETO_CHS:
      return tightLepVetoID2018_CHS(jet);
      case WP_TIGHT_LEPVETO_PUPPI:
      return tightLepVetoID2018_PUPPI(jet);
      case WP_TIGHT_LEPVETO:
      throw invalid_argument("In 2018, the LepVeto JetPFID is not the same for CHS and PUPPI. Please specify either CHS or PUPPI working point.");
      default:
      throw invalid_argument("invalid working point passed to JetPFID");
    }
  } else {
    cout<<"Invalid year, JetID set to False"<<endl;
  }
  return false;
}


//according to https://twiki.cern.ch/twiki/bin/view/CMS/JetID13TeVUL
bool JetPFID::tightIDUL16_CHS(const Jet & jet) const{
  if(fabs(jet.eta())<=2.4
  && jet.neutralHadronEnergyFraction()<0.90
  && jet.neutralEmEnergyFraction()<0.90
  && jet.numberOfDaughters()>1
  && jet.chargedHadronEnergyFraction()>0
  && jet.chargedMultiplicity()>0) return true;

  if(fabs(jet.eta())>2.4 && fabs(jet.eta())<=2.7
  && jet.neutralHadronEnergyFraction()<0.90
  && jet.neutralEmEnergyFraction()<0.99) return true;

  if(fabs(jet.eta())>2.7 && fabs(jet.eta())<=3.0
  && jet.neutralEmEnergyFraction()>0. && jet.neutralEmEnergyFraction()<0.99
  && jet.neutralMultiplicity()>1) return true;

  if(fabs(jet.eta())>3.0 && fabs(jet.eta())<=5.0
  && jet.neutralHadronEnergyFraction()>0.2
  && jet.neutralEmEnergyFraction()<0.9
  && jet.neutralMultiplicity()>10) return true;

  if(fabs(jet.eta())>5.0) return true; // not sure if anyone will ever use these jets but, according to the reference link above, they are not explicitly vetoed
  return false;
}

bool JetPFID::tightIDUL1718_CHS(const Jet & jet) const{
  if(fabs(jet.eta())<=2.6
  && jet.neutralHadronEnergyFraction()<0.90
  && jet.neutralEmEnergyFraction()<0.90
  && jet.numberOfDaughters()>1
  && jet.chargedHadronEnergyFraction()>0
  && jet.chargedMultiplicity()>0) return true;

  if(fabs(jet.eta())>2.6 && fabs(jet.eta())<=2.7
  && jet.neutralHadronEnergyFraction()<0.90
  && jet.neutralEmEnergyFraction()<0.99
  && jet.chargedMultiplicity()>0) return true;

  if(fabs(jet.eta())>2.7 && fabs(jet.eta())<=3.0
  && jet.neutralEmEnergyFraction()>0.01 && jet.neutralEmEnergyFraction()<0.99
  && jet.neutralMultiplicity()>1) return true;

  if(fabs(jet.eta())>3.0 && fabs(jet.eta())<=5.0
  && jet.neutralHadronEnergyFraction()>0.2
  && jet.neutralEmEnergyFraction()<0.9
  && jet.neutralMultiplicity()>10) return true;

  if(fabs(jet.eta())>5.0) return true; // not sure if anyone will ever use these jets but, according to the reference link above, they are not explicitly vetoed
  return false;
}

bool JetPFID::tightIDUL16_PUPPI(const Jet & jet) const{
  if(fabs(jet.eta())<=2.4
  && jet.neutralHadronEnergyFraction()<0.90
  && jet.neutralEmEnergyFraction()<0.90
  && jet.numberOfDaughters()>1
  && jet.chargedHadronEnergyFraction()>0
  && jet.chargedMultiplicity()>0) return true;

  if(fabs(jet.eta())>2.4 && fabs(jet.eta())<=2.7
  && jet.neutralHadronEnergyFraction()<0.98
  && jet.neutralEmEnergyFraction()<0.99) return true;

  if(fabs(jet.eta())>2.7 && fabs(jet.eta())<=3.0
  && jet.neutralMultiplicity()>=1) return true;

  if(fabs(jet.eta())>3.0 && fabs(jet.eta())<=5.0
  && jet.neutralEmEnergyFraction()<0.9
  && jet.neutralPuppiMultiplicity()>2) return true;

  if(fabs(jet.eta())>5.0) return true; // not sure if anyone will ever use these jets but, according to the reference link above, they are not explicitly vetoed

  return false;
}

bool JetPFID::tightIDUL1718_PUPPI(const Jet & jet) const{
  if(fabs(jet.eta())<=2.6
  && jet.neutralHadronEnergyFraction()<0.90
  && jet.neutralEmEnergyFraction()<0.90
  && jet.numberOfDaughters()>1
  && jet.chargedHadronEnergyFraction()>0
  && jet.chargedMultiplicity()>0) return true;

  if(fabs(jet.eta())>2.6 && fabs(jet.eta())<=2.7
  && jet.neutralHadronEnergyFraction()<0.90
  && jet.neutralEmEnergyFraction()<0.99) return true;

  if(fabs(jet.eta())>2.7 && fabs(jet.eta())<=3.0
  && jet.neutralHadronEnergyFraction()<0.9999) return true;

  if(fabs(jet.eta())>3.0 && fabs(jet.eta())<=5.0
  && jet.neutralEmEnergyFraction()<0.9
  && jet.neutralPuppiMultiplicity()>2) return true;

  if(fabs(jet.eta())>5.0) return true; // not sure if anyone will ever use these jets but, according to the reference link above, they are not explicitly vetoed

  return false;
}

bool JetPFID::tightLepVetoIDUL16_CHS(const Jet & jet) const{
  if(!tightIDUL16_CHS(jet)) return false;
  if(fabs(jet.eta())>2.4) return true;
  if(jet.muonEnergyFraction()<0.80
  && jet.chargedEmEnergyFraction()<0.80) return true;
  return false;
}

bool JetPFID::tightLepVetoIDUL1718_CHS(const Jet & jet) const{
  if(!tightIDUL1718_CHS(jet)) return false;
  if(fabs(jet.eta())>2.7) return true;
  if(jet.muonEnergyFraction()<0.80
  && jet.chargedEmEnergyFraction()<0.80) return true;
  return false;
}

bool JetPFID::tightLepVetoIDUL16_PUPPI(const Jet & jet) const{
  if(!tightIDUL16_PUPPI(jet)) return false;
  if(fabs(jet.eta())>2.4) return true;
  if(jet.muonEnergyFraction()<0.80
  && jet.chargedEmEnergyFraction()<0.80) return true;
  return false;
}

bool JetPFID::tightLepVetoIDUL1718_PUPPI(const Jet & jet) const{
  if(!tightIDUL1718_PUPPI(jet)) return false;
  if(fabs(jet.eta())>2.7) return true;
  if(jet.muonEnergyFraction()<0.80
  && jet.chargedEmEnergyFraction()<0.80) return true;
  return false;
}

//according to https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetID13TeVRun2017
bool JetPFID::tightID2017_CHS(const Jet & jet) const{
  if(fabs(jet.eta())<=2.7
  && jet.numberOfDaughters()>1
  && jet.neutralHadronEnergyFraction()<0.90
  && jet.neutralEmEnergyFraction()<0.90){

    if(fabs(jet.eta())>2.4)
    return true;

    if(jet.chargedHadronEnergyFraction()>0
    && jet.chargedMultiplicity()>0)
    return true;
  }
  else if(fabs(jet.eta())>2.7 && fabs(jet.eta())<=3
  &&jet.neutralEmEnergyFraction()<0.99
  &&jet.neutralEmEnergyFraction()>0.02
  &&jet.neutralMultiplicity()>2){
    return true;
  }
  else if(fabs(jet.eta())>3
  && jet.neutralMultiplicity()>10
  && jet.neutralEmEnergyFraction()<0.90
  && jet.neutralHadronEnergyFraction()>0.02){
    return true;
  }
  return false;
}

//PUPPI with puppi multiplicities, according to https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetID13TeVRun2017
bool JetPFID::tightID2017_PUPPI(const Jet & jet) const{
  if(fabs(jet.eta())<=2.7
  && jet.numberOfDaughters()>1
  && jet.neutralHadronEnergyFraction()<0.90
  && jet.neutralEmEnergyFraction()<0.90){

    if(fabs(jet.eta())>2.4)
    return true;

    if(jet.chargedHadronEnergyFraction()>0
    && jet.chargedMultiplicity()>0)
    return true;
  }
  else if(fabs(jet.eta())>2.7 && fabs(jet.eta())<=3
  &&jet.neutralHadronEnergyFraction()<0.99){
    return true;
  }
  else if(fabs(jet.eta())>3
  && jet.neutralPuppiMultiplicity()>2 && jet.neutralPuppiMultiplicity()<15
  && jet.neutralEmEnergyFraction()<0.90
  && jet.neutralHadronEnergyFraction()>0.02){
    return true;
  }
  return false;
}

bool JetPFID::tightLepVetoID2017(const Jet & jet) const{
  if(fabs(jet.eta())>2.7) return true;
  if(fabs(jet.eta())<=2.7 && !tightID2017_CHS(jet)) return false;
  if(jet.muonEnergyFraction() <0.8
  &&jet.chargedEmEnergyFraction()<0.80)
  return true;
  return false;
}

//according to https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetID13TeVRun2018
bool JetPFID::tightID2018_CHS(const Jet & jet) const{
  if(fabs(jet.eta())<=2.6
  && jet.numberOfDaughters()>1
  && jet.neutralHadronEnergyFraction()<0.90
  && jet.neutralEmEnergyFraction()<0.90
  && jet.chargedHadronEnergyFraction()>0
  && jet.chargedMultiplicity()>0){
    return true;
  }
  else if(fabs(jet.eta())>2.6 && fabs(jet.eta())<=2.7
  && jet.neutralHadronEnergyFraction()<0.90
  && jet.neutralEmEnergyFraction()<0.99
  && jet.chargedMultiplicity()>0){
    return true;
  }
  else if(fabs(jet.eta())>2.7 && fabs(jet.eta())<=3
  && jet.neutralEmEnergyFraction()<0.99
  && jet.neutralEmEnergyFraction()>0.02
  && jet.neutralMultiplicity()>2){
    return true;
  }
  else if(fabs(jet.eta())>3 && fabs(jet.eta()) <= 5.0
  && jet.neutralMultiplicity()>10
  && jet.neutralEmEnergyFraction()<0.90
  && jet.neutralHadronEnergyFraction()>0.02){
    return true;
  }
  else if(fabs(jet.eta())>5.0){ // not sure if anyone will ever use these jets but, according to the reference link above, they are not explicitly vetoed
    return true;
  }
  return false;
}

bool JetPFID::tightID2018_PUPPI(const Jet & jet) const{
  if(fabs(jet.eta())<=2.6
  && jet.numberOfDaughters()>1
  && jet.neutralHadronEnergyFraction()<0.90
  && jet.neutralEmEnergyFraction()<0.90
  && jet.chargedHadronEnergyFraction()>0
  && jet.chargedMultiplicity()>0){
    return true;
  }
  else if(fabs(jet.eta())>2.6 && fabs(jet.eta())<=2.7
  && jet.neutralHadronEnergyFraction()<0.90
  && jet.neutralEmEnergyFraction()<0.99){
    return true;
  }
  else if(fabs(jet.eta())>2.7 && fabs(jet.eta())<=3.0
  && jet.neutralHadronEnergyFraction()<0.99){
    return true;
  }
  else if(fabs(jet.eta())>3.0 && fabs(jet.eta())<=5.0
  && jet.neutralHadronEnergyFraction()>0.02
  && jet.neutralEmEnergyFraction()<0.9
  && jet.neutralPuppiMultiplicity()>2 && jet.neutralPuppiMultiplicity()<15){
    return true;
  }
  else if(fabs(jet.eta())>5.0){ // not sure if anyone will ever use these jets but, according to the reference link above, they are not explicitly vetoed
    return true;
  }
  return false;
}

bool JetPFID::tightLepVetoID2018_CHS(const Jet & jet) const{
  if(fabs(jet.eta())>2.7) return true;
  if(fabs(jet.eta())<=2.6 && !tightID2018_CHS(jet)) return false;
  if(jet.muonEnergyFraction()<0.80
  &&jet.chargedEmEnergyFraction()<0.80){
    return true;
  }
  return false;
}

bool JetPFID::tightLepVetoID2018_PUPPI(const Jet & jet) const{
  if(fabs(jet.eta())>2.7) return true;
  if(fabs(jet.eta())<=2.6 && !tightID2018_PUPPI(jet)) return false;
  if(jet.muonEnergyFraction()<0.80
  &&jet.chargedEmEnergyFraction()<0.80){
    return true;
  }
  return false;
}

// according to https://twiki.cern.ch/twiki/bin/view/CMS/JetID13TeVRun2016
bool JetPFID::looseID2016_CHS(const Jet & jet) const{
  if(fabs(jet.eta())<=2.7
  && jet.numberOfDaughters()>1
  && jet.neutralHadronEnergyFraction()<0.99
  && jet.neutralEmEnergyFraction()<0.99){

    if(fabs(jet.eta())>2.4)
    return true;

    if(jet.chargedEmEnergyFraction()<0.99
    && jet.chargedHadronEnergyFraction()>0
    && jet.chargedMultiplicity()>0)
    return true;
  }
  else if(fabs(jet.eta())>2.7 && fabs(jet.eta())<=3
  && jet.neutralEmEnergyFraction()>0.01
  && jet.neutralHadronEnergyFraction()<0.98
  && jet.neutralMultiplicity()>2){
    return true;
  }
  else if(fabs(jet.eta())>3
  && jet.neutralMultiplicity()>10
  && jet.neutralEmEnergyFraction()<0.90){
    return true;
  }
  return false;
}

bool JetPFID::tightID2016_CHS(const Jet & jet) const{
  if(!looseID2016_CHS(jet)) return false;
  if(fabs(jet.eta())<=2.7
  && jet.neutralEmEnergyFraction()<0.90
  && jet.neutralHadronEnergyFraction()<0.90){
    return true;
  }
  else if(fabs(jet.eta())>2.7){//the same as in looseID2016_CHS, thus checked
    return true;
  }
  return false;
}

bool JetPFID::looseID2016_PUPPI(const Jet & jet) const{
  if(fabs(jet.eta())>2.7) return true;
  return looseID2016_CHS(jet);
}

bool JetPFID::tightID2016_PUPPI(const Jet & jet) const{
  if(fabs(jet.eta())>2.7) return true;
  return tightID2016_CHS(jet);
}

bool JetPFID::tightLepVetoID2016(const Jet & jet) const{
  if(fabs(jet.eta())>2.7) return true;//no requirement for eta>2.7
  if(!tightID2016_CHS(jet)) return false; // due to previous line, there effectively is no difference between CHS and PUPPI LepVetoID in 2016
  if(jet.muonEnergyFraction()>=0.8) return false;
  if(fabs(jet.eta())<=2.4 && jet.chargedEmEnergyFraction()>=0.90) return false;
  return true;
}

//////// Jet PU id
JetPUid::JetPUid(wp working_point):m_working_point(working_point){}

// JetPUid::JetPUid(wp working_point) {
//   m_working_point = working_point;
// }


bool JetPUid::operator()(const Jet & jet, const Event &ev) const{
  (void) ev;
  TString wp_str;
  //  int wp_id;
  switch(m_working_point){
    case WP_LOOSE:
    wp_str = "pileup_loose";
    // wp_id = 0;
    break;
    case WP_MEDIUM:
    //wp_id = 1;
    wp_str = "pileup_medium";
    break;
    case WP_TIGHT:
    //wp_id = 2;
    wp_str = "pileup_tight";
    break;
    default:
    throw invalid_argument("invalid working point passed to JetPUid");
  }
  return jet.get_tag(jet.tagname2tag(wp_str.Data()))>0;
  //return jet.get_tag(wp_id)>0;
}


//////// HotZoneVetoId

HotZoneVetoId::HotZoneVetoId() {

  for (const auto& x : info) {
    std::string year = x.first;
    for (unsigned int i=0; i<x.second.at("hname").size(); i++) {
      TFile f_(locate_file(x.second.at("fname")[i]).c_str());
      h2HotExcl[year][x.second.at("hname")[i]] = (TH2D*)f_.Get(x.second.at("hname")[i].c_str());
      h2HotExcl.at(year).at(x.second.at("hname")[i])->SetDirectory(0);
      f_.Close();
    }
  }
}


bool HotZoneVetoId::operator()(const Jet &jet, const Event &ev) const{
  if (h2HotExcl.find(ev.year) == h2HotExcl.end()) throw std::runtime_error("In HotZoneVetoId: "+ev.year+" not found.");
  for (const auto& h : h2HotExcl.at(ev.year)) {
    if (h.second->GetBinContent(h.second->FindBin(jet.eta(),jet.phi())) > 0) return false;
  }
  return true;
}

// NoLeptonInJet

NoLeptonInJet::NoLeptonInJet(const string& lepton_, const boost::optional<ElectronId> & ele_id_, const boost::optional<MuonId> & muo_id_, const boost::optional<double>& drmax_): lepton(lepton_), ele_id(ele_id_), muo_id(muo_id_), drmax(drmax_) {};

bool NoLeptonInJet::operator()(const Jet& jet, const uhh2::Event& ev) const {

  vector<long int> jlk = jet.lepton_keys();

  bool doMuons = ev.muons && (lepton=="muon" || lepton=="all");
  bool doElectrons = ev.electrons && (lepton=="ele" || lepton=="all");

  if(doMuons){
    for(const auto& muo : *ev.muons){
      if(muo_id && !(*muo_id)(muo, ev)) continue;

      if (drmax && (*drmax)>0) {
        if(deltaR(jet, muo) < (*drmax) && jet.muonMultiplicity() > 0){
          return false;
        }
      } else {
        for(const auto& muo_cand : muo.source_candidates()){
          if(find(jlk.begin(), jlk.end(), muo_cand.key) != jlk.end()) {
            return false;
          }
        }
      }
    }
  }

  if(doElectrons){
    for(const auto& ele : *ev.electrons){
      if(ele_id && !(*ele_id)(ele, ev)) continue;
      if (drmax && (*drmax)>0) {
        if(deltaR(jet, ele) < (*drmax) && jet.electronMultiplicity() > 0){
          return false;
        }
      } else {
        for(const auto& ele_cand : ele.source_candidates()){
          if(find(jlk.begin(), jlk.end(), ele_cand.key) != jlk.end()) {
            return false;
          }
        }
      }
    }
  }

  return true;

}
