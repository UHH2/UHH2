#include "UHH2/common/include/HypothesisHists.h"
#include "TH1F.h"
#include "TH2F.h"

using namespace uhh2;

HypothesisHists::HypothesisHists(uhh2::Context & ctx, const std::string & dirname, const std::string & hyps_name, const std::string & discriminator_name ): Hists(ctx, dirname){

  TString name = discriminator_name;
    double min=0;
    double max=500;
    if(discriminator_name=="Chi2"){
      name = "#Chi^{2}";
    }
    else{
      name += " discriminator";
    }

    if( discriminator_name=="CorrectMatch"){
      min=0;
      max=2;
    }
    if( discriminator_name=="TopDRMC"){
      min=0;
      max=6;
    }

    Discriminator = book<TH1F>("Discriminator",name,100,min,max);
    Discriminator_2 = book<TH1F>("Discriminator_2",name,50,0,10);
    Discriminator_3 = book<TH1F>("Discriminator_3",name,300,0,30); 

    M_ttbar_rec = book<TH1F>( "M_ttbar_rec", "M_{t#bar{t}}^{rec} [GeV/c^{2}]", 100, 0, 5000 ) ;
 
    M_ttbar_gen = book<TH1F>( "M_ttbar_gen", "M_{t#bar{t}}^{gen} [GeV/c^{2}]", 100, 0, 5000 ) ;
 
    M_toplep_rec = book<TH1F>( "M_toplep_rec", "M^{top,lep} [GeV/c^{2}]", 70, 0, 700 ) ;
    M_tophad_rec = book<TH1F>( "M_tophad_rec", "M^{top,had} [GeV/c^{2}]", 50, 0, 500 ) ;
  
    M_tophad_rec_1jet = book<TH1F>( "M_tophad_rec_1jet", "M^{top,had} [GeV/c^{2}]", 50, 0, 500 ) ;
    M_tophad_rec_2jet = book<TH1F>( "M_tophad_rec_2jet", "M^{top,had} [GeV/c^{2}]", 50, 0, 500 ) ;
    M_tophad_rec_3jet = book<TH1F>( "M_tophad_rec_3jet", "M^{top,had} [GeV/c^{2}]", 50, 0, 500 ) ;
    
    Pt_toplep_rec = book<TH1F>( "Pt_toplep_rec", "P_{T}^{top,lep} [GeV/c]", 60, 0, 1200 ) ;
    Pt_tophad_rec = book<TH1F>( "Pt_tophad_rec", "P_{T}^{top,had} [GeV/c]", 60, 0, 1200 ) ;
    
    Pt_ttbar_rec = book<TH1F>( "Pt_ttbar_rec", "P_{T,t#bar{t}}^{rec} [GeV/c]", 60, 0, 600 ) ;
    Pt_ttbar_gen = book<TH1F>( "Pt_ttbar_gen", "P_{T,t#bar{t}}^{gen} [GeV/c]", 60, 0, 600 ) ;
    
    Pt_ttbar_rec_vs_Pt_ttbar_gen = book<TH2F>( "Pt_ttbar_rec_vs_Pt_ttbar_gen", "P_{T,t#bar{t}}^{rec} [GeV/c] vs P_{T,t#bar{t}}^{gen} [GeV/c]", 60, 0, 600 ,60, 0, 600);

    Discriminator_vs_M_ttbar = book<TH2F>("Discriminator_vs_M_ttbar", name+" vs M_{t#bar{t}}^{rec}" , 50, min,max, 100,0,5000) ;

    M_ttbar_rec_vs_M_ttbar_gen = book<TH2F>("M_ttbar_rec_vs_M_ttbar_gen","M_{t#bar{t}}^{rec} [GeV/c^{2}] vs M_{t#bar{t}}^{gen} [GeV/c^{2}]",100,0,5000,100,0,5000);
    M_ttbar_gen_vs_M_ttbar_rec = book<TH2F>("M_ttbar_gen_vs_M_ttbar_rec","M_{t#bar{t}}^{gen} [GeV/c^{2}] vs M_{t#bar{t}}^{rec} [GeV/c^{2}]",100,0,5000,100,0,5000);
    
    M_ttbar_resolution = book<TH1F>("M_ttbar_resolution", "(M_{t#bar{t}}^{gen} - M_{t#bar{t}}^{rec})/M_{t#bar{t}}^{rec}", 100, -5,5) ;
  
    Pt_toplep_rec_vs_Pt_toplep_gen = book<TH2F>("Pt_toplep_rec_vs_Pt_toplep_gen","P_{T}^{top,lep,rec} [GeV/c] vs P_{T}^{top,lep,gen} [GeV/c]",60, 0, 1200,60, 0, 1200);
    Pt_tophad_rec_vs_Pt_tophad_gen = book<TH2F>("Pt_tophad_rec_vs_Pt_tophad_gen","P_{T}^{top,had,rec} [GeV/c] vs P_{T}^{top,had,gen} [GeV/c]",60, 0, 1200,60, 0, 1200);
    Pt_lepton_rec_vs_Pt_lepton_gen = book<TH2F>("Pt_lepton_rec_vs_Pt_lepton_gen","P_{T}^{lepton,rec} [GeV/c] vs P_{T}^{lepton,gen} [GeV/c]",60, 0, 800,60, 0, 800);
    Pt_blep_rec_vs_Pt_blep_gen = book<TH2F>("Pt_blep_rec_vs_Pt_blep_gen","P_{T}^{blep,rec} [GeV/c] vs P_{T}^{blep,gen} [GeV/c]",60, 0, 800,60, 0, 800);
    Pt_neutrino_rec_vs_Pt_neutrino_gen = book<TH2F>("Pt_neutrino_rec_vs_Pt_neutrino_gen","P_{T}^{#nu,rec} [GeV/c] vs P_{T}^{#nu,gen} [GeV/c]",60, 0, 800,60, 0, 800);
    
    eta_toplep_rec_vs_eta_toplep_gen = book<TH2F>("eta_toplep_rec_vs_eta_toplep_gen","#eta^{top,lep,rec} vs #eta^{top,lep,gen}",100,-5,5,100,-5,5);
    eta_tophad_rec_vs_eta_tophad_gen = book<TH2F>("eta_tophad_rec_vs_eta_tophad_gen","#eta^{top,had,rec} vs #eta^{top,had,gen}",100,-5,5,100,-5,5);
    eta_lepton_rec_vs_eta_lepton_gen = book<TH2F>("eta_lepton_rec_vs_eta_lepton_gen","#eta^{lepton,rec} vs #eta^{lepton,gen}",100,-5,5,100,-5,5);
    eta_blep_rec_vs_eta_blep_gen = book<TH2F>("eta_blep_rec_vs_eta_blep_gen","#eta^{blep,rec} vs #eta^{blep,gen}",100,-5,5,100,-5,5 );
    eta_neutrino_rec_vs_eta_neutrino_gen = book<TH2F>("eta_neutrino_rec_vs_eta_neutrino_gen","#eta^{#nu,rec} vs #eta^{#nu,gen}",100,-5,5,100,-5,5 );
    
    h_hyps = ctx.get_handle<std::vector<ReconstructionHypothesis>>(hyps_name);
    h_ttbargen = ctx.get_handle<TTbarGen>("ttbargen");
    m_discriminator_name = discriminator_name;
}


void HypothesisHists::fill(const uhh2::Event & e){

  std::vector<ReconstructionHypothesis> hyps = e.get(h_hyps);
  const ReconstructionHypothesis* hyp = get_best_hypothesis( hyps, m_discriminator_name );
  double weight = e.weight;

  double mttbar_rec = 0;
  if( (hyp->top_v4()+hyp->antitop_v4()).isTimelike() )
    mttbar_rec = (hyp->top_v4()+hyp->antitop_v4()).M();
  else{
    mttbar_rec = sqrt( -(hyp->top_v4()+hyp->antitop_v4()).mass2());
  }
  double ptttbar_rec = (hyp->top_v4()+hyp->antitop_v4()).Pt();
  double mttbar_gen = 0;
  double ptttbar_gen = 0;

  M_ttbar_rec->Fill(mttbar_rec, weight);
  Pt_ttbar_rec->Fill ( ptttbar_rec, weight);
  
  if(e.is_valid(h_ttbargen)){
    const auto & ttbargen = e.get(h_ttbargen);
    mttbar_gen = ( ttbargen.Top().v4() + ttbargen.Antitop().v4()).M();
    ptttbar_gen = ( ttbargen.Top().v4() + ttbargen.Antitop().v4()).Pt();
  
    M_ttbar_gen->Fill(mttbar_gen, weight);
    
    M_ttbar_rec_vs_M_ttbar_gen->Fill(mttbar_rec, mttbar_gen,weight);
    M_ttbar_gen_vs_M_ttbar_rec->Fill(mttbar_gen, mttbar_rec,weight);

    M_ttbar_resolution->Fill( (mttbar_gen-mttbar_rec)/mttbar_gen, weight);

    Pt_ttbar_gen->Fill ( ptttbar_gen, weight);
    Pt_ttbar_rec_vs_Pt_ttbar_gen->Fill(ptttbar_rec, ptttbar_gen,weight);

    if(ttbargen.DecayChannel()==TTbarGen::e_ehad || ttbargen.DecayChannel()==TTbarGen::e_muhad){
      double lep_pt_gen = ttbargen.ChargedLepton().pt();
      double lep_eta_gen = ttbargen.ChargedLepton().eta();
      double nu_pt_gen = ttbargen.Neutrino().pt();
      double nu_eta_gen = ttbargen.Neutrino().eta();
      double blep_pt_gen = ttbargen.BLep().pt();
      double blep_eta_gen = ttbargen.BLep().eta();
      double toplep_pt_gen = ttbargen.TopLep().pt();
      double toplep_eta_gen = ttbargen.TopLep().eta();
      double tophad_pt_gen = ttbargen.TopHad().pt();
      double tophad_eta_gen = ttbargen.TopHad().eta();

      Pt_tophad_rec_vs_Pt_tophad_gen->Fill(hyp->tophad_v4().Pt(), tophad_pt_gen, weight);
      Pt_toplep_rec_vs_Pt_toplep_gen->Fill(hyp->toplep_v4().Pt(), toplep_pt_gen, weight);
      Pt_lepton_rec_vs_Pt_lepton_gen->Fill(hyp->lepton().pt(), lep_pt_gen, weight);
      Pt_neutrino_rec_vs_Pt_neutrino_gen->Fill(hyp->neutrino_v4().Pt(), nu_pt_gen, weight);
      Pt_blep_rec_vs_Pt_blep_gen->Fill(hyp->blep_v4().Pt(), blep_pt_gen, weight);
      eta_tophad_rec_vs_eta_tophad_gen->Fill(hyp->tophad_v4().eta(), tophad_eta_gen, weight);
      eta_toplep_rec_vs_eta_toplep_gen->Fill(hyp->toplep_v4().eta(), toplep_eta_gen, weight);
      eta_lepton_rec_vs_eta_lepton_gen->Fill(hyp->lepton().eta(), lep_eta_gen, weight);
      eta_neutrino_rec_vs_eta_neutrino_gen->Fill(hyp->neutrino_v4().eta(), nu_eta_gen, weight);
      eta_blep_rec_vs_eta_blep_gen->Fill(hyp->blep_v4().eta(), blep_eta_gen, weight);
    }
  }
  
  Discriminator->Fill(hyp->discriminator(m_discriminator_name) ,weight);
  Discriminator_2->Fill(hyp->discriminator(m_discriminator_name) ,weight);
  Discriminator_3->Fill(hyp->discriminator(m_discriminator_name) ,weight);

  Discriminator_vs_M_ttbar->Fill(hyp->discriminator(m_discriminator_name), mttbar_rec, weight);

  double mtoplep=0;
  double mtophad=0;
  if(hyp->toplep_v4().isTimelike()) mtoplep = hyp->toplep_v4().M();
  if(hyp->tophad_v4().isTimelike()) mtophad = hyp->tophad_v4().M();
  M_toplep_rec->Fill(mtoplep,weight);
  M_tophad_rec->Fill(mtophad,weight);

  if(hyp->tophad_jets().size()==1) M_tophad_rec_1jet->Fill(mtophad,weight);
  if(hyp->tophad_jets().size()==2) M_tophad_rec_2jet->Fill(mtophad,weight);
  if(hyp->tophad_jets().size()>=3) M_tophad_rec_3jet->Fill(mtophad,weight);

  Pt_toplep_rec->Fill( hyp->toplep_v4().Pt(),weight );
  Pt_tophad_rec->Fill( hyp->tophad_v4().Pt(),weight );


}
