#include "UHH2/common/include/JetIds.h"

#include "UHH2/common/include/Utils.h"

using namespace std;
using namespace uhh2;

BTag::BTag(algo tagger, wp working_point) {
  m_working_point = (int) working_point;
  switch (tagger) {
    case CSVV2 :
    jet_id = CSVBTag((CSVBTag::wp) m_working_point);
    m_algo = "CSVv2";
    break;
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

CSVBTag::CSVBTag(wp working_point) {
  m_working_point = working_point;
}

CSVBTag::CSVBTag(float float_point):csv_threshold(float_point) {}


bool CSVBTag::operator()(const Jet & jet, const Event & ev){
  if(ev.year == "2016v2" || ev.year == "2016v3"){
    //  https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation80XReReco
    switch(m_working_point){
      case WP_LOOSE:
      csv_threshold = 0.5426;
      break;
      case WP_MEDIUM:
      csv_threshold = 0.8484;
      break;
      case WP_TIGHT:
      csv_threshold = 0.9535;
      break;
      default:
      throw invalid_argument("invalid working point passed to CSVBTag");
    }
  }
  if(ev.year.find("2017") != string::npos || ev.year.find("2018") != string::npos){
    //https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation94X
    //Note: CSV is not supported for 2018 analyses
    switch(m_working_point){
      case WP_LOOSE:
      csv_threshold = 0.5803;
      break;
      case WP_MEDIUM:
      csv_threshold = 0.8838;
      break;
      case WP_TIGHT:
      csv_threshold = 0.9693;
      break;
      default:
      throw invalid_argument("invalid working point passed to CSVBTag");
    }
  }
  return jet.btag_combinedSecondaryVertex() > csv_threshold;
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
  else if(ev.year.find("2017") != string::npos){
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
  else if(ev.year == "2018"){
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
  else if(ev.year.find("2017") != string::npos){
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
  else if(ev.year == "2018"){
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
  } else {
    cout << "Invalid year, DeepJetBTag returning false" << endl;
    return false;
  }
  return jet.btag_DeepJet() > deepjet_threshold;
}

///
JetPFID::JetPFID(wp working_point):m_working_point(working_point){}

bool JetPFID::operator()(const Jet & jet, const Event & ev) const{
  if (ev.year.find("2016") != string::npos){
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
      throw invalid_argument("invalid working point passed to JetPFID");
    }
    return false;
  }
  else if (ev.year.find("2017") != string::npos){
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
  else if (ev.year.find("2018") != string::npos){
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
  } else {
    cout<<"Invalid year, JetID set to False"<<endl;
  }
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
  else if(fabs(jet.eta())>3
  && jet.neutralMultiplicity()>10
  && jet.neutralEmEnergyFraction()<0.90
  && jet.neutralHadronEnergyFraction()>0.02){
    return true;
  }
  return false;
}

bool JetPFID::tightLepVetoID2018(const Jet & jet) const{
  if(fabs(jet.eta())>2.6) return true;
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
  if(!tightID2016_CHS(jet)) return false;
  if(jet.muonEnergyFraction()>=0.8) return false;
  if(fabs(jet.eta())<=2.4 && jet.chargedEmEnergyFraction()>0.90) return false;
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

HotZoneVetoId::HotZoneVetoId(const bool& isHotZoneOnly) {
  std::string mapHistname;

  for (const auto& el : year_str_map) {
    std::string year = el.second;
    if (year.find("UL")!=std::string::npos) {
      mapHistname = isHotZoneOnly? "h2hot_ul17": "h2hot_ul17_plus_hep17";
      try {
        TFile file_map(locate_file("JECDatabase/hotzone_maps/Summer19UL17_V1/hotjets-UL17.root").c_str());
        h2HotExcl[year] = (TH2D*) file_map.Get(mapHistname.c_str());
        h2HotExcl.at(year)->SetDirectory(0);
        file_map.Close();
      } catch (...){}

      continue;
    }

    mapHistname="h2hotfilter";
    for (auto ver: {"v1", "v2", "v3"}) if (year.find(ver) != std::string::npos) year.erase(year.find(ver), 2);

    try {
      TFile file_mc(locate_file("common/data/"+year+"/hotjets-"+year+"_MC.root").c_str());
      h2HotExcl[year+"_MC"] = (TH2D*) file_mc.Get(mapHistname.c_str());
      h2HotExcl.at(year+"_MC")->SetDirectory(0);
      file_mc.Close();
    } catch (...){}

    for (const auto & runItr : year2runPeriods(year)) {
      try {
        TFile file_data(locate_file("common/data/"+year+"/hotjets-"+year+"_Run"+runItr+".root").c_str());
        h2HotExcl[year+"_"+runItr] = (TH2D*) file_data.Get(mapHistname.c_str());
        h2HotExcl.at(year+"_"+runItr)->SetDirectory(0);
        file_data.Close();
      } catch (...){}
    }
  }
}


bool HotZoneVetoId::operator()(const Jet &jet, const Event &ev) const{
  string mapName;
  string runItr = "";
  std::string year = ev.year;
  for (auto ver: {"v1", "v2", "v3"}) if (year.find(ver) != std::string::npos) year.erase(year.find(ver), 2);

  if(!ev.isRealData) runItr = "MC";
  else {
    for (const auto & [key, val] : run_number_map.at(year)) {
      if (ev.run >= val.first && ev.run <= val.second) runItr = key;
    }
  }
  mapName = year+"_"+runItr;
  if (year.find("UL")!=std::string::npos) mapName = year;

  if (h2HotExcl.find(mapName) == h2HotExcl.end()) throw std::runtime_error("In HotZoneVetoId: "+mapName+" not found.");

  return (h2HotExcl.at(mapName)->GetBinContent(h2HotExcl.at(mapName)->FindBin(jet.eta(),jet.phi())) <= 0);
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

      if (drmax && drmax>0) {
        if(deltaR(jet, muo) < drmax && jet.muonMultiplicity() > 0){
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
      if (drmax && drmax>0) {
        if(deltaR(jet, ele) < drmax && jet.electronMultiplicity() > 0){
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
