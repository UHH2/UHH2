#include "UHH2/common/include/TTbarReconstruction.h"
#include "UHH2/common/include/Utils.h" // for locate_file
#include "UHH2/common/include/LorentzVector.h"

using namespace std;


PrimaryLepton::PrimaryLepton(Context & ctx){
  
  h_primlep = ctx.get_handle<FlavorParticle>("PrimaryLepton");
}

bool PrimaryLepton::process(uhh2::Event & event){
  assert(event.muons || event.electrons);

  double ptmax = -999;
  FlavorParticle primlep;
  if(event.electrons){
    for(unsigned int i=0; i<event.electrons->size(); ++i){
      if(event.electrons->at(i).pt()>ptmax){
	ptmax = event.electrons->at(i).pt();
	primlep = (FlavorParticle) (event.electrons->at(i));
      }
    }
  }
  if(event.muons){
    for(unsigned int i=0; i<event.muons->size(); ++i){
      if(event.muons->at(i).pt()>ptmax){
	ptmax = event.muons->at(i).pt();
	primlep = event.muons->at(i);
      }
    }
  }

  event.set(h_primlep, std::move(primlep));
  return true;
}

PrimaryLepton::~PrimaryLepton(){
}

HighMassTTbarReconstruction::HighMassTTbarReconstruction(Context & ctx, const NeutrinoReconstructionMethod & neutrinofunction, string label): m_neutrinofunction(neutrinofunction){

  h_recohyps = ctx.declare_event_output<vector<ReconstructionHypothesis>>(label);
  
  h_primlep = ctx.get_handle<FlavorParticle>("PrimaryLepton");
}

CAJetsReconstruction::CAJetsReconstruction(Context & ctx, const NeutrinoReconstructionMethod & neutrinofunction, string label): m_neutrinofunction(neutrinofunction){

  h_recohyps = ctx.declare_event_output<vector<ReconstructionHypothesis>>(label);
  
  h_primlep = ctx.get_handle<FlavorParticle>("PrimaryLepton");
}

TopTagReconstruction::TopTagReconstruction(Context & ctx, const NeutrinoReconstructionMethod & neutrinofunction, string label): m_neutrinofunction(neutrinofunction){

  h_recohyps = ctx.declare_event_output<vector<ReconstructionHypothesis>>(label);
  
  h_primlep = ctx.get_handle<FlavorParticle>("PrimaryLepton");
}
    
bool HighMassTTbarReconstruction::process(uhh2::Event & event){
  assert(event.jets);
  assert(event.met);
    
  //find primary charged lepton
  Particle lepton = event.get(h_primlep);

  std::vector<ReconstructionHypothesis> recoHyps;

  //reconstruct neutrino
  std::vector<LorentzVector> neutrinos = m_neutrinofunction( lepton.v4(), event.met->v4());
 
  ReconstructionHypothesis hyp;

  hyp.set_lepton(lepton);

  //loop over neutrino solutions and jet assignments to fill hyotheses
  for(unsigned int i=0; i< neutrinos.size();++i){

    hyp.set_neutrino_v4(neutrinos[i]);
    LorentzVector wlep_v4 = lepton.v4()+neutrinos[i];

    unsigned int n_jets = event.jets->size();
    if(n_jets>10) n_jets=10; //avoid crashes in events with many jets
    unsigned int max_j = pow(3, n_jets);
    for (unsigned int j=0; j < max_j; j++) {
      LorentzVector tophad_v4(0,0,0,0);
      LorentzVector toplep_v4 = wlep_v4;
      int hadjets=0;
      int lepjets=0;
      int num = j;
      hyp.clear_jetindices();
      for (unsigned int k=0; k<n_jets; k++) {
	// num is the k-th digit of j if you
	// write j in a base-3 system. According
	// to the value of this digit (which takes
	// values from 0 to 2,
	// in all possible combinations with the other digits),
	// decide how to treat the jet.

	if(num%3==0) {
	  tophad_v4 = tophad_v4 + event.jets->at(k).v4();
	  hyp.add_tophad_jet_index(k);
	  hadjets++;
	}

	if(num%3==1) {
	  toplep_v4 = toplep_v4 + event.jets->at(k).v4();
	  hyp.add_toplep_jet_index(k);
	  lepjets++;
	}
	//if(num%3==2); //do not take this jet

	//shift the trigits of num to the right:
	num /= 3;
      }

      //search jet with highest pt assigned to leptonic top
      float maxpt=-999;
      int maxind=-1;
      for(unsigned int i=0; i<hyp.toplep_jets_indices().size(); ++i){
	float pt = event.jets->at(hyp.toplep_jets_indices().at(i)).pt();
	if(pt>maxpt){
	  maxpt=pt;
	  maxind=hyp.toplep_jets_indices().at(i);
	}
      }
      hyp.set_blep_index(maxind);


      //fill only hypotheses with at least one jet assigned to each top quark
      if(hadjets>0 && lepjets>0){
	hyp.set_tophad_v4(tophad_v4);
	hyp.set_toplep_v4(toplep_v4);
	recoHyps.push_back(hyp);
      }
    }
  }

  event.set(h_recohyps, std::move(recoHyps));
  return true;
}

bool CAJetsReconstruction::process(uhh2::Event & event){
  assert(event.jets);
  assert(event.met);

  double deltaR_Lep_Tophad = .8;
  double deltaR_Jet_Tophad = 1.3;
  double mjet=0;

  //find primary charged lepton
  Particle lepton = event.get(h_primlep);

  std::vector<Jet>* antikjets = event.jets;
  std::vector<TopJet>* cajets = event.topjets;

  //reconstruct neutrino
  std::vector<LorentzVector> neutrinos = m_neutrinofunction( lepton.v4(), event.met->v4());

  std::vector<ReconstructionHypothesis> recoHyps;

  ReconstructionHypothesis hyp;

  hyp.set_lepton(lepton);
  double cajets_pt;
  if(cajets->size()>0) {
   cajets_pt   =  cajets->at(0).pt();
  int caposi_pt = 0;

  for(unsigned int m = 0; m<cajets->size(); ++m)
    {
      TopJet cajet = cajets->at(m);

      if(cajets_pt < cajet.pt())
	{
	  cajets_pt = cajet.pt();
	  caposi_pt = m;
	}

    }
   LorentzVector top_had;
   if(cajets->size()>0) top_had = cajets->at(caposi_pt).v4();
   mjet=top_had.M();
   for(unsigned int m = 0; m<cajets->size(); ++m)
    {
       TopJet cajet = cajets->at(m);
       if(deltaR_Lep_Tophad < deltaR(cajet.v4(),lepton.v4())){

	int n_jets = antikjets->size();
	if(n_jets>6) n_jets=6;
	int max_j = pow(3, n_jets);


	for(unsigned int i = 0; i < neutrinos.size();++i){

	  Particle wboson_lep;
	  wboson_lep.set_v4(lepton.v4()+neutrinos.at(i));


	  for(int j=0; j<max_j; ++j){
	    LorentzVector top_lep(0,0,0,0);
	    LorentzVector b_lep(0,0,0,0);
	    int num = j;
	    hyp.clear_jetindices();
	    for(unsigned int p=0; p<antikjets->size(); ++p){
	      if(deltaR(top_had,antikjets->at(p).v4())> deltaR_Jet_Tophad && num%3==0){
		b_lep = b_lep + antikjets->at(p).v4();
		top_lep = wboson_lep.v4() + b_lep;
		hyp.set_blep_index(p);
		hyp.set_blep_v4(b_lep);
		hyp.add_toplep_jet_index(p);
		hyp.add_tophad_jet_index(caposi_pt);

		hyp.set_neutrino_v4(neutrinos[i]);

		double egroomed = sqrt(cajet.v4().P2()+mjet*mjet);
		top_had.SetPxPyPzE(cajet.v4().Px(),cajet.v4().Py(),cajet.v4().Pz(),egroomed);
		hyp.set_tophad_v4(top_had);

		hyp.set_toplep_v4(top_lep);

		recoHyps.push_back(hyp);
	      }
	      num/=3;
	    }

	  }}}}

  }

  event.set(h_recohyps, std::move(recoHyps));
  return true;
}

bool TopTagReconstruction::process(uhh2::Event & event){

  std::cerr<< "TopTagger is not yet implemented in UHH2! As soon as it is implemented you can import this function from TopFitCalc::CalculateTopTag in UHHAnalysis." << std::endl;

  return true;
}

HighMassTTbarReconstruction::~HighMassTTbarReconstruction(){}

CAJetsReconstruction::~CAJetsReconstruction(){}

TopTagReconstruction::~TopTagReconstruction(){}

std::vector<LorentzVector> NeutrinoReconstruction(const LorentzVector lepton, const LorentzVector met)
{

  TVector3 lepton_pT = toVector(lepton);
  lepton_pT.SetZ(0);
  
  TVector3 neutrino_pT = toVector(met);
  neutrino_pT.SetZ(0);
  
  const float mass_w = 80.399;
  float mu = mass_w * mass_w / 2 + lepton_pT * neutrino_pT;
  
  float A = - (lepton_pT * lepton_pT);
  float B = mu * lepton.pz();
  float C = mu * mu - lepton.e() * lepton.e() * (neutrino_pT * neutrino_pT);
  
  float discriminant = B * B - A * C;
  
  std::vector<LorentzVector> solutions;
  
  if (0 >= discriminant)
    {
      // Take only real part of the solution
      //
      LorentzVectorXYZE solution (0,0,0,0);
      solution.SetPx(met.Px());
      solution.SetPy(met.Py());
      solution.SetPz(-B / A);
      solution.SetE(toVector(solution).Mag());
      
      solutions.push_back(toPtEtaPhi(solution));
      
      //_solutions = 0 > discriminant ? 0 : 1;
    }
  else
    {
      discriminant = sqrt(discriminant);
      
      LorentzVectorXYZE solution (0,0,0,0);
      solution.SetPx(met.Px());
      solution.SetPy(met.Py());
      solution.SetPz((-B - discriminant) / A);
      solution.SetE(toVector(solution).Mag());
      
      solutions.push_back(toPtEtaPhi(solution));
      
      LorentzVectorXYZE solution2 (0,0,0,0);
      solution2.SetPx(met.Px());
      solution2.SetPy(met.Py());
      solution2.SetPz((-B + discriminant) / A);
      solution2.SetE(toVector(solution2).Mag());
      
      solutions.push_back(toPtEtaPhi(solution2));
      
      //_solutions = 2;
    }
  
  return solutions;
}

double DeltaPolarNeutrino(double PhiN, double metPx, double metPy, double PhiLep, double PtLep)
{
  double PyN;
  double PxN;
  const double mass_w = 80.399;

  double num = 10.e-7;

  if(1-cos(fabs(PhiLep-PhiN)> M_PI ? 2*M_PI-fabs(PhiLep-PhiN) : fabs(PhiLep-PhiN )) < num){
    PyN = 0.5*mass_w*mass_w* sin(PhiN)/(PtLep*num);
    PxN = 0.5*mass_w*mass_w* cos(PhiN)/(PtLep*num);
  }
  else{
    PyN = 0.5*mass_w*mass_w* sin(PhiN)/(PtLep*(1-cos(fabs(PhiLep-PhiN)> M_PI ? 2*M_PI-fabs(PhiLep-PhiN) : fabs(PhiLep-PhiN ))));
    PxN = 0.5*mass_w*mass_w* cos(PhiN)/(PtLep*(1-cos(fabs(PhiLep-PhiN)> M_PI ? 2*M_PI-fabs(PhiLep-PhiN) : fabs(PhiLep-PhiN ))));
  }

  return pow(PxN-metPx,2)+pow(PyN-metPy,2);

}

static void polarminuitfunc(int& nDim, double* gout, double& result, double par[], int flg){
  result = DeltaPolarNeutrino(par[0],par[1],par[2],par[3],par[4]);

}

std::vector<LorentzVector> NeutrinoFitPolar(const LorentzVector lepton, const LorentzVector met)
{

  TVector3 lepton_pT = toVector(lepton);
  lepton_pT.SetZ(0);

  TVector3 neutrino_pT = toVector(met);
  neutrino_pT.SetZ(0);

  const double mass_w = 80.399;


  double min = -2*M_PI;
  double max = 2*M_PI;
  double start = met.phi();
  double step = 10.e-5;

  double mu = mass_w * mass_w / 2 + lepton_pT * neutrino_pT;
  double A = - (lepton_pT * lepton_pT);
  double B = mu * lepton.pz();
  double C = mu * mu - lepton.e() * lepton.e() * (neutrino_pT * neutrino_pT);

  double discriminant = B * B - A * C;

  std::vector<LorentzVector> solutions;


  if (0 >= discriminant)
    {

    double resultPhi = 0;
    double error = 0;
    int ierflg;
    double* arg = new double[1];

    TMinuit* positiv = new TMinuit(5);
    positiv->SetPrintLevel(-1); // -1 quiet, 0 normal, 1 verbose; Preset 0

    positiv->SetFCN(polarminuitfunc);

    positiv->DefineParameter(0,"PhiN",start, step,  min, max);
    positiv->DefineParameter(1,"metPx",met.px(),0,0,0);
    positiv->DefineParameter(2,"metPy",met.py(),0,0,0);
    positiv->DefineParameter(3,"PhiLep",lepton.phi(),0,0,0);
    positiv->DefineParameter(4,"PtLep",lepton.pt(),0,0,0);

    positiv->FixParameter(1);
    positiv->FixParameter(2);
    positiv->FixParameter(3);
    positiv->FixParameter(4);

    positiv->SetMaxIterations(500);


    arg[0]= 2;
    positiv->mnexcm("SET STR",arg,1,ierflg);

    positiv->Migrad();

    positiv->GetParameter(0,resultPhi,error);

    delete[] arg;

    if(resultPhi != resultPhi)
      {
	std::cerr << "neutrino phi is NAN " << std::endl;
      }

    if(resultPhi > M_PI) resultPhi = resultPhi-2*M_PI;
    if(resultPhi < M_PI) resultPhi = resultPhi+2*M_PI;


    double PyN;
    double PxN;

    double num = 10.e-7;

    if(1-cos(deltaPhiAbs(lepton.phi(), resultPhi)) < num){
      PyN = 0.5*mass_w*mass_w* sin(resultPhi)/(lepton.pt()*num);
      PxN = 0.5*mass_w*mass_w* cos(resultPhi)/(lepton.pt()*num);
    }
    else{
      PyN = 0.5*mass_w*mass_w* sin(resultPhi)/(lepton.pt()*(1-cos(deltaPhiAbs(lepton.phi(), resultPhi))));
      PxN = 0.5*mass_w*mass_w* cos(resultPhi)/(lepton.pt()*(1-cos(deltaPhiAbs(lepton.phi(), resultPhi))));
    }

    LorentzVectorXYZE neutrino_result(0,0,0,0);
    neutrino_result.SetPx(PxN);
    neutrino_result.SetPy(PyN);

    double pzfit =  lepton.pz()*neutrino_result.pt()/lepton.pt();

    LorentzVectorXYZE solution (0,0,0,0);
    solution.SetPx(PxN);
    solution.SetPy(PyN);
    solution.SetPz(pzfit);
    solution.SetE(toVector(solution).Mag());


    solutions.push_back(toPtEtaPhi(solution));

    delete positiv;
    }
  else
    {
      discriminant = sqrt(discriminant);

      LorentzVectorXYZE solution (0,0,0,0);
      solution.SetPx(met.Px());
      solution.SetPy(met.Py());
      solution.SetPz((-B - discriminant) / A);
      solution.SetE(toVector(solution).Mag());

      solutions.push_back(toPtEtaPhi(solution));

      LorentzVectorXYZE solution2 (0,0,0,0);
      solution2.SetPx(met.Px());
      solution2.SetPy(met.Py());
      solution2.SetPz((-B + discriminant) / A);
      solution2.SetE(toVector(solution2).Mag());

      solutions.push_back(toPtEtaPhi(solution2));

   }

  return solutions;
}



