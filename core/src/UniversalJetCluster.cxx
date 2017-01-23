//#include "FWCore/Utilities/interface/CPUTimer.h"
#include "UHH2/core/include/UniversalJetCluster.h"

using namespace std;
using namespace uhh2;
using namespace fastjet;
using namespace contrib;


UniversalJetCluster::UniversalJetCluster(vector<PFParticle> *pfparticles, bool doHOTVR=true, bool doXCone=true)
{
  for(auto pf : *pfparticles) 
    {
      if(std::isnan(pf.v4().Px()) || std::isnan(pf.v4().Py()) || std::isnan(pf.v4().Pz()) || std::isinf(pf.v4().Px()) || std::isinf(pf.v4().Py())  || std::isinf(pf.v4().Pz()))
	continue;
      _psj.push_back(ConvertPFToPsj(pf));
    }
  if(doHOTVR)ClusterHOTVR();
  //fastjet crashes with segmentation violation if not enough PFParticle are present!
  
  if(doXCone && pfparticles->size()>15)
    ClusterXCone33();
  // to calculate the area information for both hotvr and xcone makes the ntuplewrite 3-4 times slower!
  // therefore we only calculate the area with reduced max eta of 4. For xcone only subjets have an area and for 
  // hotvr the ghost area is set to 0.02 instead of 0.01

}

// ---------------------------------------------------------------
// TopJets

// HOTVR 

void UniversalJetCluster::ClusterHOTVR()
{
  double mu(30.),                 // massjump threshold
    theta(0.7),                   // massjump parameter
    max_r(1.5),                   // maximum allowed distance R
    min_r(0.1),                   // minimum allowed distance R
    rho(600.),                    // cone shrinking parameter
    hotvr_pt_min(30.);            // minimum pT of subjet

  // jet definition with HOTVR plugin
  HOTVR hotvr_plugin(mu, theta, min_r, max_r, rho, hotvr_pt_min, HOTVR::CALIKE); 
  JetDefinition jet_def(&hotvr_plugin);

   // Setup cluster sequence
  ClusterSequence cs(_psj, jet_def);
  vector<PseudoJet> hotvr_jets = hotvr_plugin.get_jets();

  // NOTE: There is a problem when getting Nsubjettiness directly
  // from HOTVR jets, because the link to the cluster sequence got lost
  // somehow. Thus the approach here will be to identify the Jets from
  // the cluster sequence with the hotvr_jets by comparing their
  // four-vectors.

  // area definition
  double ghost_maxrap = 4.0;      // maxiumum y of ghost particles
  AreaDefinition area_def(active_area_explicit_ghosts, GhostedAreaSpec(ghost_maxrap, 1, 0.02));

  // setup cluster sequence with area
  HOTVR hotvr_plugin_area(mu, theta, min_r, max_r, rho, hotvr_pt_min, HOTVR::CALIKE); 
  JetDefinition jet_def_area(&hotvr_plugin_area);
  ClusterSequenceArea cs_area(_psj, jet_def_area, area_def);
  //ClusterSequence cs_area(_psj, jet_def_area);
  //vector<PseudoJet> cs_jets = cs_area.inclusive_jets(hotvr_pt_min);
  vector<PseudoJet> hotvr_jets_area = hotvr_plugin_area.get_jets();


  //in a few cases, there are jets in the original clustering without a corresponding jet in the area clustering
  //->add a dummy jet into the area collection and throw a warning because we cannot determine the area for these jets
  if (hotvr_jets_area.size() != hotvr_jets.size())
    {

    for (unsigned int i = 0; i < hotvr_jets.size(); ++i)
      {
	bool matched=false;
	for (unsigned int j = 0; j < hotvr_jets_area.size(); ++j)
	  {
	    if( fabs(hotvr_jets[i].pt() - hotvr_jets_area[j].pt())<0.0001 && fabs(hotvr_jets[i].eta() - hotvr_jets_area[j].eta())<0.0001)
	      {
		matched = true;
		break;
	      }
	  }
	if(!matched)
	  {
	    PseudoJet dummy_jet(0,0,0,0);
	    hotvr_jets_area.insert(hotvr_jets_area.begin()+i, dummy_jet);
	  }
      }

    //sometimes, there are more jets in the area clustering
    //->filter out jets from the area collection which are not matched to any jet in the original clustering

    for (unsigned int i = 0; i < hotvr_jets_area.size(); ++i)
      {
	//skip dummy jets from previous iteration
	if(hotvr_jets_area[i].pt()==0) continue;
	bool matched=false;
	for (unsigned int j = 0; j < hotvr_jets.size(); ++j)
	  {
	    if( fabs(hotvr_jets[j].pt() - hotvr_jets_area[i].pt())<0.0001 && fabs(hotvr_jets[j].eta() - hotvr_jets_area[i].eta())<0.0001)
	      {
		matched = true;
		break;
	      }
	  }
	if(!matched)
	  {
	    hotvr_jets_area.erase(hotvr_jets_area.begin()+i);
	    i--;
	  }
      }
    }
  
  //this should hopefully not happen anymore
  if(hotvr_jets_area.size() != hotvr_jets.size()){
    throw runtime_error("ERROR in UniversalJetCluster::ClusterHOTVR: number of jets found with ClusterSequence does not match number of jets with ClusterSequenceArea.");
  }
  
  for (unsigned int i = 0; i < hotvr_jets.size(); ++i)
    {
      double beta = 1.0;
      HOTVRinfo hi = hotvr_jets[i].user_info<HOTVRinfo>();
      Nsubjettiness nSub1(1,   OnePass_WTA_KT_Axes(), NormalizedMeasure(beta, hi.max_distance()));
      Nsubjettiness nSub2(2,   OnePass_WTA_KT_Axes(), NormalizedMeasure(beta, hi.max_distance()));
      Nsubjettiness nSub3(3,   OnePass_WTA_KT_Axes(), NormalizedMeasure(beta, hi.max_distance()));
      double tau1 =  nSub1(hotvr_jets[i]);
      double tau2 =  nSub2(hotvr_jets[i]);
      double tau3 =  nSub3(hotvr_jets[i]);
      //getting jet and subjet area
      double jet_area = 0;
      vector<double> subjet_area;
      vector<PseudoJet> subjets;
      if(hotvr_jets_area[i].pt()>0){
	jet_area = hotvr_jets_area[i].area();
	HOTVRinfo hi_area = hotvr_jets_area[i].user_info<HOTVRinfo>();
	subjets = hi_area.subjets();
	for (unsigned int j = 0; j < subjets.size(); ++j) {
	  subjet_area.push_back(subjets[j].area());
	  //subjet_area.push_back(0.);
	}
      }
      else{
	subjets = hi.subjets();
	for (unsigned int j = 0; j < subjets.size(); ++j) {
	  subjet_area.push_back(0);
	}
	std::cout << "WARNING in UniversalJetCluster::ClusterHOTVR: could not find area jet for a HOTVR jet; set area and subjet areas to 0." << std::endl;
      }
      _hotvrTopJets.push_back(ConvertPsjToTopJet(hotvr_jets[i], subjets, tau1, tau2, tau3, jet_area, subjet_area));
    }
  
  
}
vector<TopJet> UniversalJetCluster::GetHOTVRTopJets()
{
  return _hotvrTopJets;
}

// XCone
void UniversalJetCluster::ClusterXCone33()
{
  /*
  cout<<"=============================="<<endl;
  cout<<"Entering into XCone clustering"<<endl;
  cout<<"total number "<<_psj.size()<<endl;
  ofstream out;
  out.open("minimal.txt");
  out<<"Format: px py pz E"<<endl;
  for(auto jet : _psj)
    out<<jet.px()<<" "<<jet.py()<<" "<<jet.pz()<<" "<<jet.E()<<endl;
    //cout<<"phi "<<jet.phi_std()<<" eta "<< jet.eta()<<" pt "<<jet.pt()<<endl;

  out.close();
  */


  if(_psj.size()<2){
    cerr<<"Less then 2 particles for first XCone step"<<endl;
    return;
  }
  


  // Run first clustering step (N=2, R=1.2) 
  vector<PseudoJet> fatjets;
  XConePlugin plugin_xcone(2, 1.2, 2.0);
  double ghost_maxrap = 4.0;      // maxiumum y of ghost particles
  AreaDefinition area_def(active_area, GhostedAreaSpec(ghost_maxrap));
  JetDefinition jet_def_xcone(&plugin_xcone);
  //ClusterSequenceArea clust_seq_xcone(_psj, jet_def_xcone, area_def);
  ClusterSequence clust_seq_xcone(_psj, jet_def_xcone);
  fatjets = sorted_by_pt(clust_seq_xcone.inclusive_jets(0));
  ////

  //cout<<"getting softdrop mass"<<endl;
  // get SoftDrop Mass
  SoftDrop sd(0.0, 0.1, 1.2);
  PseudoJet sdjet1 = sd(fatjets[0]);
  PseudoJet sdjet2 = sd(fatjets[1]);
  float sd_mass1 = sdjet1.m();
  float sd_mass2 = sdjet2.m();
  ////

  //cout<<"setting list of particles"<<endl;

  // get and wirte list: if particle i is clustered in jet j, the i-th entry of the list == j
  vector<int> list_fat;
  list_fat.clear();
  list_fat = clust_seq_xcone.particle_jet_indices(fatjets);
  vector<PseudoJet> particle_in_fat1, particle_in_fat2;

 
  // get one set of particles for each jet
  for (unsigned int ipart=0; ipart < _psj.size(); ++ipart){
    if (list_fat[ipart]==0){
      particle_in_fat1.push_back(_psj.at(ipart)); // get vector as PseudoJet for fatjet 1
    }
    if (list_fat[ipart]==1){
      particle_in_fat2.push_back(_psj.at(ipart)); // get vector as PseudoJet for fatjet 2
   }
  }
  ////
  /*
  //====================
  cout<<"Number of particles per jet"<<endl;
  cout<<"jet 1: "<<particle_in_fat1.size()<<" jet 2: "<< particle_in_fat2.size()<<endl;
  //====================
  */  
  if(particle_in_fat1.size() < 3 || particle_in_fat2.size()<3){
    cerr<<"not enough particles to run second XCone step"<<endl;
    cerr<<"particles 1: "<< particle_in_fat1.size()<<" particles 2: "<<particle_in_fat2.size()<<endl;
    return;
  }
  // Run second clustering step (N=3, R=0.4) for each fat jet
  vector<PseudoJet> subjets_1, subjets_2;

  // subjets from fat jet 1 
  XConePlugin plugin_xcone_sub1(3, 0.4, 2.0);
  JetDefinition jet_def_sub1(&plugin_xcone_sub1);
  ClusterSequenceArea clust_seq_sub1(particle_in_fat1, jet_def_sub1, area_def);
  //ClusterSequence clust_seq_sub1(particle_in_fat1, jet_def_sub1);
  subjets_1 = sorted_by_pt(clust_seq_sub1.inclusive_jets(0));
  
  //cout<<"cluster jet 1 into 04AKjets done"<<endl;


  //subjets from fat jet 2 
  XConePlugin plugin_xcone_sub2(3, 0.4, 2.0);
  JetDefinition jet_def_sub2(&plugin_xcone_sub2);
  ClusterSequenceArea clust_seq_sub2(particle_in_fat2, jet_def_sub2, area_def); 
  subjets_2 = sorted_by_pt(clust_seq_sub2.inclusive_jets(0));

  //cout<<"cluster jet 2 into 04AKjets done"<<endl;

  ////
  
  // set TopJets with subjets and JetArea
  
  //double jet1_area = fatjets[0].area();
  //double jet2_area = fatjets[1].area();
  vector<double> subjet1_area;
  vector<double> subjet2_area;
  for (unsigned int j = 0; j < subjets_1.size(); ++j) subjet1_area.push_back(subjets_1[j].area());
  for (unsigned int k = 0; k < subjets_2.size(); ++k) subjet2_area.push_back(subjets_2[k].area());
  
  double jet1_area = 0;
  double jet2_area = 0;
  /*
  vector<double> subjet1_area;
  vector<double> subjet2_area;
  for (unsigned int j = 0; j < subjets_1.size(); ++j) subjet1_area.push_back(0.);
  for (unsigned int k = 0; k < subjets_2.size(); ++k) subjet2_area.push_back(0.);
  */
  _xcone33TopJets.push_back(ConvertPsjToTopJet(fatjets[0], subjets_1, jet1_area, subjet1_area, sd_mass1));
  _xcone33TopJets.push_back(ConvertPsjToTopJet(fatjets[1], subjets_2, jet2_area, subjet2_area, sd_mass2));
  ////
 
 
  // delete pseudojets and lists
  subjets_1.clear();
  subjets_2.clear();
  fatjets.clear();
  particle_in_fat1.clear();
  particle_in_fat2.clear();
  ////
 
}
vector<TopJet> UniversalJetCluster::GetXCone33Jets()
{
  return _xcone33TopJets;
}


// ---------------------------------------------------------------
// Converters

// Convert PFParticle to PseudoJet
PseudoJet UniversalJetCluster::ConvertPFToPsj(const PFParticle & pfp)
{
  PseudoJet psj(pfp.v4().X(), pfp.v4().Y(), pfp.v4().Z(), pfp.v4().T());
  return psj;
}

// Convert to Jets with Area
Jet UniversalJetCluster::ConvertPsjToJet(const PseudoJet & psj, double jet_area)
{
  Jet jet;
  jet.set_pt(psj.pt());
  jet.set_eta(psj.eta());
  jet.set_phi(psj.phi());
  jet.set_energy(psj.E());
  jet.set_jetArea(jet_area);
  return jet;
}

// Convert to Jets
Jet UniversalJetCluster::ConvertPsjToJet(const PseudoJet & psj)
{
  Jet jet;
  jet.set_pt(psj.pt());
  jet.set_eta(psj.eta());
  jet.set_phi(psj.phi());
  jet.set_energy(psj.E());
  return jet;
}

// Convert to TopJets with groomed Nsubjettiness
TopJet UniversalJetCluster::ConvertPsjToTopJet(const PseudoJet & psj, const vector<PseudoJet> &subpsj, double tau1, double tau2, double tau3, double jet_area, vector<double> subjet_area)
{
  TopJet topjet;
  topjet.set_pt(psj.pt());
  topjet.set_eta(psj.eta());
  topjet.set_phi(psj.phi());
  topjet.set_energy(psj.E());
  topjet.set_tau1_groomed(tau1);
  topjet.set_tau2_groomed(tau2);
  topjet.set_tau3_groomed(tau3);
  topjet.set_jetArea(jet_area);
  for (unsigned int i = 0; i < subpsj.size(); ++i) 
    {
      topjet.add_subjet(ConvertPsjToJet(subpsj[i], subjet_area[i]));
    }
  return topjet;
}

// Convert to Topjets with Area and softdrop mass
TopJet UniversalJetCluster::ConvertPsjToTopJet(const PseudoJet & psj, const vector<PseudoJet> &subpsj, double jet_area, vector<double> subjet_area, float sd_mass)
{
  TopJet topjet;
  topjet.set_pt(psj.pt());
  topjet.set_eta(psj.eta());
  topjet.set_phi(psj.phi());
  topjet.set_energy(psj.E());
  topjet.set_jetArea(jet_area);
  topjet.set_softdropmass(sd_mass);

  for (unsigned int i = 0; i < subpsj.size(); ++i) 
    {
      topjet.add_subjet(ConvertPsjToJet(subpsj[i], subjet_area[i]));
    }
  return topjet;
}

