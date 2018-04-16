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

bool JetPFID::operator()(const Jet & jet, const Event &) const{
  switch(m_working_point){
  case WP_TIGHT:
    return tightID(jet);
  case  WP_TIGHT_LEPVETO:
    return tightLepVetoID(jet);
  default:
    throw invalid_argument("invalid working point passed to JetPFID");
  }
  return false;
}

//according to https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetID13TeVRun2017
bool JetPFID::tightID(const Jet & jet) const{
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
	  &&jet.neutralEmEnergyFraction()<0.90
	  &&jet.neutralEmEnergyFraction()>0.02
	  &&jet.neutralMultiplicity()>2){
    return true;
  }
  else if(fabs(jet.eta())>3
	  && jet.neutralMultiplicity()>10
	  && jet.neutralEmEnergyFraction()<0.99
	  && jet.neutralHadronEnergyFraction()>0.02){
    return true;
  }
  else  
    if(fabs(jet.eta())>2.7)
      return true;
  return false;
}

bool JetPFID::tightLepVetoID(const Jet & jet) const{
  if(!tightID(jet))return false;
  if(jet.muonEnergyFraction() <0.8
     &&jet.chargedEmEnergyFraction()<0.80)
    return true;
  return false;
}
