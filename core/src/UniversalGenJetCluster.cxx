#include "UHH2/core/include/UniversalGenJetCluster.h"

using namespace std;
using namespace uhh2;
using namespace fastjet;
using namespace contrib;

UniversalGenJetCluster::UniversalGenJetCluster(vector<GenParticle> *genparticles)
{
  // get stable particles and identify primary lepton for xcone23jets
  double pt_max = 0;
  GenParticle lepton;
  for(unsigned int i = 0; i < genparticles->size(); ++i) 
    {
      //      _psj.push_back(ConvertGenToPsj(&(genparticles->at(i))));
      _psj.push_back(ConvertGenToPsj(genparticles->at(i))); //TEST
      if(abs(genparticles->at(i).pdgId()==11) || abs(genparticles->at(i).pdgId()==13)){
	if(genparticles->at(i).v4().Pt() > pt_max){
	  pt_max = genparticles->at(i).v4().Pt();
	  lepton = genparticles->at(i);
	}
      }
    }
  _lepton = lepton;
  if(pt_max == 0) _lep_found = false;
  else _lep_found = true;

  ClusterHOTVR();
  ClusterXCone23();
  ClusterXCone33();
}


// ---------------------------------------------------------------
// GenTopJets

//HOTVR
void UniversalGenJetCluster::ClusterHOTVR()
{
  double mu(30.),                 // massjump threshold
    theta(0.7),                   // massjump parameter
    max_r(1.5),                   // maximum allowed distance R
    min_r(0.1),                   // minimum allowed distance R
    rho(600.),                    // cone shrinking parameter
    hotvr_pt_min(30.);            // minimum pT of subjet
  // setup jet definition with HOTVR plugin
  HOTVR hotvr_plugin(mu, theta, min_r, max_r, rho, hotvr_pt_min, HOTVR::CALIKE); 
  JetDefinition jet_def(&hotvr_plugin);
  // setup cluster sequence
  ClusterSequence cs(_psj, jet_def);

  // write jets
  vector<PseudoJet> hotvr_jets = hotvr_plugin.get_jets();
  for (unsigned int i = 0; i < hotvr_jets.size(); ++i)
    {
      HOTVRinfo hi = hotvr_jets[i].user_info<HOTVRinfo>();
      vector<PseudoJet> subjets = hi.subjets();
      _hotvrGenTopJets.push_back(ConvertPsjToGenTopJet(hotvr_jets[i], subjets));
    }

}
std::vector<GenTopJet> UniversalGenJetCluster::GetHOTVRTopJets()
{
  return _hotvrGenTopJets;
}

//XCone
void UniversalGenJetCluster::ClusterXCone23()
{
  // Run first clustering step (N=2, R=1.2) 
  std::vector<PseudoJet> fatjets;
  XConePlugin plugin_xcone(2, 1.2, 2.0);
  JetDefinition jet_def_xcone(&plugin_xcone);
  ClusterSequence clust_seq_xcone(_psj, jet_def_xcone);
  fatjets = sorted_by_pt(clust_seq_xcone.inclusive_jets(0));
  ////

  // declare leptonic and hadronic jet (default is jet1 = had)
  bool jet1_is_had = true;
  if(_lep_found){
    double dR1=deltaR(fatjets[0], _lepton);
    double dR2=deltaR(fatjets[1], _lepton);
    if(dR1 < dR2) jet1_is_had = false;
  }

  // get and wirte list: if particle i ist clustered in jet j, the i-th entry of the list == j
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

  // check if both list contain at least 3 particles, 
  // code is likely to crash if lists contain less particles than required jets
  if(particle_in_fat1.size() < 3 || particle_in_fat2.size() <3) return;  
  ////


  // Run second clustering step (N=3, R=0.4) for each fat jet
  // (this clustering depends on which jet is the leptonic and which one is the hadronic jet)
  std::vector<fastjet::PseudoJet> subjets_1, subjets_2;

  if(jet1_is_had){
    // subjets from fat jet 1 (hadronic)
    XConePlugin plugin_xcone_sub1(3, 0.4, 2.0);
    JetDefinition jet_def_sub1(&plugin_xcone_sub1);
    ClusterSequence clust_seq_sub1(particle_in_fat1, jet_def_sub1);
    subjets_1 = sorted_by_pt(clust_seq_sub1.inclusive_jets(0));

    // subjets from fat jet 2 (leptonic)
    XConePlugin plugin_xcone_sub2(2, 0.4, 2.0);
    JetDefinition jet_def_sub2(&plugin_xcone_sub2);
    ClusterSequence clust_seq_sub2(particle_in_fat2, jet_def_sub2);
    subjets_2 = sorted_by_pt(clust_seq_sub2.inclusive_jets(0));
    ////
  }
  if(!jet1_is_had){
    // subjets from fat jet 1 (leptonic)
    XConePlugin plugin_xcone_sub1(2, 0.4, 2.0);
    JetDefinition jet_def_sub1(&plugin_xcone_sub1);
    ClusterSequence clust_seq_sub1(particle_in_fat1, jet_def_sub1);
    subjets_1 = sorted_by_pt(clust_seq_sub1.inclusive_jets(0));

    // subjets from fat jet 2 (hadronic)
    XConePlugin plugin_xcone_sub2(3, 0.4, 2.0);
    JetDefinition jet_def_sub2(&plugin_xcone_sub2);
    ClusterSequence clust_seq_sub2(particle_in_fat2, jet_def_sub2);
    subjets_2 = sorted_by_pt(clust_seq_sub2.inclusive_jets(0));
    ////
  }

  // set GenTopJets with subjets
  _xcone23TopJets.push_back(ConvertPsjToGenTopJet(fatjets[0], subjets_1));
  _xcone23TopJets.push_back(ConvertPsjToGenTopJet(fatjets[1], subjets_2));
  ////

  // delete pseudojets and lists
  subjets_1.clear();
  subjets_2.clear();
  fatjets.clear();
  particle_in_fat1.clear();
  particle_in_fat2.clear();
  ////
}
vector<GenTopJet> UniversalGenJetCluster::GetXCone23Jets()
{
  return _xcone23TopJets;
}

void UniversalGenJetCluster::ClusterXCone33()
{
  // Run first clustering step (N=2, R=1.2) 
  vector<PseudoJet> fatjets;
  XConePlugin plugin_xcone(2, 1.2, 2.0);
  JetDefinition jet_def_xcone(&plugin_xcone);
  ClusterSequence clust_seq_xcone(_psj, jet_def_xcone);
  fatjets = sorted_by_pt(clust_seq_xcone.inclusive_jets(0));
  ////

  // get and wirte list: if particle i ist clustered in jet j, the i-th entry of the list == j
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

  // check if both list contain at least 3 particles, 
  // code is likely to crash if lists contain less particles than required jets
  if(particle_in_fat1.size() < 3 || particle_in_fat2.size() <3) return;  
  ////

  // Run second clustering step (N=3, R=0.4) for each fat jet
  vector<PseudoJet> subjets_1, subjets_2;

  // subjets from fat jet 1 
  XConePlugin plugin_xcone_sub1(3, 0.4, 2.0);
  JetDefinition jet_def_sub1(&plugin_xcone_sub1);
  ClusterSequence clust_seq_sub1(particle_in_fat1, jet_def_sub1);
  subjets_1 = sorted_by_pt(clust_seq_sub1.inclusive_jets(0));

  // subjets from fat jet 2 
  XConePlugin plugin_xcone_sub2(3, 0.4, 2.0);
  JetDefinition jet_def_sub2(&plugin_xcone_sub2);
  ClusterSequence clust_seq_sub2(particle_in_fat2, jet_def_sub2);
  subjets_2 = sorted_by_pt(clust_seq_sub2.inclusive_jets(0));
  
  ////

  // set GenTopJets with subjets
  _xcone33TopJets.push_back(ConvertPsjToGenTopJet(fatjets[0], subjets_1));
  _xcone33TopJets.push_back(ConvertPsjToGenTopJet(fatjets[1], subjets_2));
  ////

  // Also write Jets with SoftDrop 
  SoftDrop sd(0.0, 0.1, 1.2);
  PseudoJet sdjet1 = sd(fatjets[0]);
  PseudoJet sdjet2 = sd(fatjets[1]);
  _xcone33TopJets_softdrop.push_back(ConvertPsjToGenTopJet(sdjet1, subjets_1));
  _xcone33TopJets_softdrop.push_back(ConvertPsjToGenTopJet(sdjet2, subjets_2));
  ////

  // delete pseudojets and lists
  subjets_1.clear();
  subjets_2.clear();
  fatjets.clear();
  particle_in_fat1.clear();
  particle_in_fat2.clear();
  ////
}
vector<GenTopJet> UniversalGenJetCluster::GetXCone33Jets()
{
  return _xcone33TopJets;
}
vector<GenTopJet> UniversalGenJetCluster::GetXCone33Jets_softdrop()
{
  return _xcone33TopJets_softdrop;
}


// ---------------------------------------------------------------
// Converters

//PseudoJet UniversalGenJetCluster::ConvertGenToPsj(GenParticle * genp)
 PseudoJet UniversalGenJetCluster::ConvertGenToPsj(const GenParticle & genp) //TEST
{
  //  PseudoJet psj(genp->v4().X(), genp->v4().Y(), genp->v4().Z(), genp->v4().T());
  PseudoJet psj(genp.v4().X(), genp.v4().Y(), genp.v4().Z(), genp.v4().T());//TEST
  return psj;
}

Particle UniversalGenJetCluster::ConvertPsjToParticle(const PseudoJet & psj)
{
  Particle part;
  part.set_pt(psj.pt());
  part.set_eta(psj.eta());
  part.set_phi(psj.phi());
  part.set_energy(psj.E());
  return part;
}

GenTopJet UniversalGenJetCluster::ConvertPsjToGenTopJet(const PseudoJet & psj, const vector<PseudoJet> &subpsj)
{
  GenTopJet topjet;
  topjet.set_pt(psj.pt());
  topjet.set_eta(psj.eta());
  topjet.set_phi(psj.phi());
  topjet.set_energy(psj.E());
  for (unsigned int i = 0; i < subpsj.size(); ++i) 
    {
      topjet.add_subjet(ConvertPsjToParticle(subpsj[i]));
    }
  return topjet;
}
