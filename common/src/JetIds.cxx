#include "UHH2/common/include/JetIds.h"

using namespace std;
using namespace uhh2;

CSVBTag::CSVBTag(wp working_point) {
    switch(working_point){
        case WP_LOOSE:
            csv_threshold = 0.5426f;
            break;
        case WP_MEDIUM:
            csv_threshold = 0.8484f;
            break;
        case WP_TIGHT:
            csv_threshold = 0.9535f;
            break;
        default:
            throw invalid_argument("invalid working point passed to CSVBTag");
    }
}

CSVBTag::CSVBTag(float float_point):csv_threshold(float_point) {}


bool CSVBTag::operator()(const Jet & jet, const Event &) const{
    return jet.btag_combinedSecondaryVertex() > csv_threshold;
}
///
DeepCSVBTag::DeepCSVBTag(wp working_point) {
    switch(working_point){
        case WP_LOOSE:
            deepcsv_threshold = 0.1522f;
            break;
        case WP_MEDIUM:
            deepcsv_threshold = 0.4941f;
            break;
        case WP_TIGHT:
            deepcsv_threshold = 0.8001f;
            break;
        default:
            throw invalid_argument("invalid working point passed to DeepCSVBTag");
    }
}

DeepCSVBTag::DeepCSVBTag(float float_point):deepcsv_threshold(float_point) {}


bool DeepCSVBTag::operator()(const Jet & jet, const Event &) const{
    return jet.btag_DeepCSV() > deepcsv_threshold;
}


///
JetPFID::JetPFID(wp working_point):m_working_point(working_point){}

bool JetPFID::operator()(const Jet & jet, const Event & ev) const{
  if(ev.year == "2016v2"){
    switch(m_working_point){
    case WP_LOOSE_CHS:
      return looseID2016_CHS(jet);
    case WP_TIGHT_CHS:
      return tightID2016_CHS(jet);
    case WP_LOOSE_PUPPI:
      return looseID2016_PUPPI(jet);
    case WP_TIGHT_PUPPI:
      return tightID2016_PUPPI(jet);
    case  WP_TIGHT_LEPVETO:
      return tightLepVetoID2016(jet);
    default:
      throw invalid_argument("invalid working point passed to CSVBTag");
    }
    return false;
  }
  if(ev.year == "2016v3"){
    switch(m_working_point){
    case WP_LOOSE_CHS:
      return looseID2016_CHS(jet);
    case WP_TIGHT_CHS:
      return tightID2016_CHS(jet);
    case WP_LOOSE_PUPPI:
      return looseID2016_PUPPI(jet);
    case WP_TIGHT_PUPPI:
      return tightID2016_PUPPI(jet);
    case  WP_TIGHT_LEPVETO:
      return tightLepVetoID2016(jet);
    default:
      throw invalid_argument("invalid working point passed to CSVBTag");
    }
    return false;
  }
  if(ev.year == "2017"){
   switch(m_working_point){
    case WP_TIGHT_CHS:
      return tightID2017_CHS(jet);
    case WP_TIGHT_PUPPI:
      return tightID2017_PUPPI(jet);
    case  WP_TIGHT_LEPVETO:
      return tightLepVetoID2017(jet);
    default:
      throw invalid_argument("invalid working point passed to JetPFID");
    }
  }
 if(ev.year == "2018"){
   switch(m_working_point){
   case WP_TIGHT_CHS:
     return tightID2018_CHS(jet);
   case WP_TIGHT_PUPPI:
     return tightID2018_CHS(jet);//placeholder
   case  WP_TIGHT_LEPVETO:
     return tightLepVetoID2018(jet);
   default:
     throw invalid_argument("invalid working point passed to JetPFID");
   }
 }
 if(ev.year!= "2018" && ev.year!= "2017" && ev.year!= "2016v3" && ev.year!= "2016v2")
   cout<<"Invalid year, JetID set to False"<<endl;
  return false;
}

//according to https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetID13TeVRun2017
bool JetPFID::tightID2017_CHS(const Jet & jet) const{
  if(fabs(jet.eta())<=2.7
     && jet.numberOfDaughters()>1 
     && jet.neutralHadronEnergyFraction()<0.90
     && jet.neutralEmEnergyFraction()<0.90){
    
    if(fabs(jet.eta())>=2.4)
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
    
    if(fabs(jet.eta())>=2.4)
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
  }
  else if(fabs(jet.eta())>2.6 && fabs(jet.eta())<=2.7
	  && jet.neutralHadronEnergyFraction()<0.90 
	  && jet.neutralEmEnergyFraction()<0.99
	  && jet.chargedMultiplicity()>0){
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

bool JetPFID::tightLepVetoID2018(const Jet & jet) const{
  if(fabs(jet.eta())<=2.6 && !tightID2018_CHS(jet)) return false;
  if(jet.muonEnergyFraction() <0.8
     &&jet.chargedEmEnergyFraction()<0.80)
    return true;
  return false;
}

bool JetPFID::looseID2016_CHS(const Jet & jet) const{
  if(fabs(jet.eta())<=2.7
     && jet.numberOfDaughters()>1 
     && jet.neutralHadronEnergyFraction()<0.99
     && jet.neutralEmEnergyFraction()<0.99){
    
    if(fabs(jet.eta())>=2.4)
      return true;
      
    if(jet.chargedEmEnergyFraction()<0.99
       && jet.chargedHadronEnergyFraction()>0
       && jet.chargedMultiplicity()>0)
      return true;   
  }
  else if(fabs(jet.eta())>2.7 && fabs(jet.eta())<=3
	  &&jet.neutralEmEnergyFraction()<0.90
	  &&jet.neutralMultiplicity()>2){
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
  if(!tightID2016_CHS(jet)) return false;
  if(jet.muonEnergyFraction()>=0.8) return false;
  if(fabs(jet.eta())<=2.4 && jet.chargedEmEnergyFraction()<0.90) return false;
  return true;

}
