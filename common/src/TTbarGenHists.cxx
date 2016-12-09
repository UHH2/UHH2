#include "UHH2/common/include/TTbarGenHists.h"
#include "TH1F.h"
#include "TH2F.h"

using namespace uhh2;

TTbarGenHists::TTbarGenHists(uhh2::Context & ctx, const std::string & dirname): Hists(ctx, dirname){

    Mttbar_gen =  book<TH1F>( "M_ttbar_gen", "M_{t#bar{t}} [GeV/c^{2}]", 1000, 0, 5000 ) ;
    Pt_ttbar_gen = book< TH1F>( "Pt_ttbar_gen", "P_{T,t#bar{t}} [GeV/c]", 600, 0, 600 ) ;
    shat = book< TH1F>( "shat", "#hat{s} [GeV]", 1000, 0, 5000 ) ;
    DecayChannel = book< TH1F>( "DecayChannel", "decay channel", 11, 0, 11 ) ;

    M_top = book< TH1F>("M_top", "M_{t} [GeV/c^{2}]", 1000, 150, 200) ;
    M_antitop = book< TH1F>("M_antitop", "M_{#bar{t}} [GeV/c^{2}]", 1000, 150, 200) ;  

    Pt_top = book< TH1F>( "Pt_top", "P_{T,t} [GeV/c]", 1000, 0, 2000 ) ;
    Pt_antitop = book< TH1F>( "Pt_antitop", "P_{T,#bar{t}} [GeV/c]", 1000, 0, 2000 ) ;
    
    Pt_top_over_shat = book< TH1F>( "Pt_top_over_shat", "P_{T,t}/#hat{s}", 1000, 0, 1 ) ;
    Pt_antitop_over_shat = book< TH1F>( "Pt_antitop_over_shat", "P_{T,#bar{t}}/#hat{s}", 1000, 0, 1 ) ;
    Pt_top_over_M_ttbar = book< TH1F>( "Pt_top_over_M_ttbar", "P_{T,t}/M_{t#bar{t}}", 1000, 0, 1 ) ;
    Pt_antitop_over_M_ttbar = book< TH1F>( "Pt_antitop_over_M_ttbar", "P_{T,#bar{t}}/M_{t#bar{t}}", 1000, 0, 1 ) ;
    
    M_ttbar_vs_shat = book< TH2F>( "M_ttbar_vs_shat","M_{t#bar{t}} [GeV/c^{2}] vs #hat{s} [GeV]", 500,0,5000,500,0,5000);
    
    eta_top = book< TH1F>( "eta_top", "#eta_{t}", 1000, -5, 5 ) ;
    eta_antitop = book< TH1F>( "eta_antitop", "#eta_{#bar{t}}", 1000, -5, 5 ) ;
    y_top = book< TH1F>( "y_top", "y_{t}", 1000, -5, 5 ) ;
    y_antitop = book< TH1F>( "y_antitop", "y_{#bar{t}}", 1000, -5, 5 ) ;
    
    diffabseta = book<TH1F>( "diffabseta", "|#eta_{t}|-|#eta_{#bar{t}}|",1000,-5,5);
    diffabsy = book<TH1F>( "diffabsy", "|y_{t}|-|y_{#bar{t}}|",1000,-5,5);
    
    deltaR_top_decays = book<TH1F>( "deltaR_top_decays", "#DeltaR(t decay prod.)",1000,0,5);
    deltaR_antitop_decays = book<TH1F>( "deltaR_antitop_decays", "#DeltaR(#bar{t} decay prod.)",1000,0,5);  
    M_ttbar_vs_deltaR_top = book<TH2F>( "M_ttbar_vs_deltaR_top", "M_{t#bar{t}} [GeV/c^{2}] vs #DeltaR(t decay prod.)",500,0,5000,500,0,5);
    M_ttbar_vs_deltaR_antitop = book<TH2F>( "M_ttbar_vs_deltaR_antitop", "M_{t#bar{t}} [GeV/c^{2}] vs #DeltaR(#bar{t} decay prod.)",500,0,5000,500,0,5); 
    
    shat_vs_deltaR_top = book<TH2F>( "shat_vs_deltaR_top", "#hat{s} [GeV] vs #DeltaR(t decay prod.)",500,0,5000,500,0,5);
    shat_vs_deltaR_antitop = book<TH2F>( "shat_vs_deltaR_antitop", "#hat{s} [GeV] vs #DeltaR(#bar{t} decay prod.)",500,0,5000,500,0,5); 

    Pt_top_vs_deltaR_top = book<TH2F>( "Pt_top_vs_deltaR_top", "P_{T,t} [GeV/c] vs #DeltaR(t decay prod.)",500,0,2000,500,0,5);
    Pt_antitop_vs_deltaR_antitop = book<TH2F>( "Pt_antitop_vs_deltaR_antitop", "P_{T,#bar{t}} [GeV/c] vs #DeltaR(#bar{t} decay prod.)",500,0,2000,500,0,5);

    deltaR_Wplus_decays = book<TH1F>( "deltaR_Wplus_decays", "#DeltaR(W^{+} decay prod.)",1000,0,5);
    deltaR_Wminus_decays = book<TH1F>( "deltaR_Wminus_decays", "#DeltaR(W^{-} decay prod.)",1000,0,5);  
    M_ttbar_vs_deltaR_Wplus = book<TH2F>( "M_ttbar_vs_deltaR_Wplus", "M_{t#bar{t}} [GeV/c^{2}] vs #DeltaR(W^{+} decay prod.)",500,0,5000,500,0,5);
    M_ttbar_vs_deltaR_Wminus = book<TH2F>( "M_ttbar_vs_deltaR_Wminus", "M_{t#bar{t}} [GeV/c^{2}] vs #DeltaR(W^{-} decay prod.)",500,0,5000,500,0,5); 

    M_ttbar_vs_Pt_top = book<TH2F>( "M_ttbar_vs_Pt_top", "M_{t#bar{t}} [GeV/c^{2}] vs P_{T,t} [GeV/c]",500,0,5000,500,0,2000); 
    M_ttbar_vs_Pt_antitop = book<TH2F>( "M_ttbar_vs_Pt_antitop", "M_{t#bar{t}} [GeV/c^{2}] vs P_{T,#bar{t}} [GeV/c]",500,0,5000,500,0,2000);
    shat_vs_Pt_top = book<TH2F>( "shat_vs_Pt_top", "#hat{s} [GeV] vs P_{T,t} [GeV/c]",500,0,5000,500,0,2000); 
    shat_vs_Pt_antitop = book<TH2F>( "shat_vs_Pt_antitop", "#hat{s} [GeV] vs P_{T,#bar{t}} [GeV/c]",500,0,5000,500,0,2000);
    Pt_top_vs_Pt_antitop = book<TH2F>( "Pt_top_vs_Pt_antitop", "P_{T,t} [GeV/c] vs P_{T,#bar{t}} [GeV/c]",500,0,2000,500,0,2000);
    
    Pt_b = book<TH1F>( "Pt_b","P_{T,b} [GeV/c]",1000,0,1000);
    Pt_antib = book<TH1F>( "Pt_antib","P_{T,#bar{b}} [GeV/c]",1000,0,1000);
    eta_b = book<TH1F>( "eta_b","#eta_{b}",1000,-5,5);
    eta_antib = book<TH1F>( "eta_antib","#eta_{#bar{b}}",1000,-5,5);
    y_b = book<TH1F>( "y_b","y_{b}",1000,-5,5);
    y_antib = book<TH1F>( "y_antib","y_{#bar{b}}",1000,-5,5);
    M_b = book<TH1F>( "M_b","M_{b} [GeV/c^{2}]",1000,0,10);
    M_antib = book<TH1F>( "M_antib","M_{#bar{b}} [GeV/c^{2}]",1000,0,10);

    Pt_Wplus = book<TH1F>( "Pt_Wplus","P_{T,W^{+}} [GeV/c]",1000,0,1000);
    Pt_Wminus = book<TH1F>( "Pt_Wminus","P_{T,W^{-}} [GeV/c]",1000,0,1000);
    eta_Wplus = book<TH1F>( "eta_Wplus","#eta_{W^{+}}",1000,-5,5);
    eta_Wminus = book<TH1F>( "eta_Wminus","#eta_{W^{-}}",1000,-5,5);
    y_Wplus = book<TH1F>( "y_Wplus","y_{W^{+}}",1000,-5,5);
    y_Wminus = book<TH1F>( "y_Wminus","y_{W^{-}}",1000,-5,5);
    M_Wplus = book<TH1F>( "M_Wplus","M_{W^{+}} [GeV/c^{2}]",1000,50,100);
    M_Wminus = book<TH1F>( "M_Wminus","M_{W^{-}} [GeV/c^{2}]",1000,50,100);
  
    cosThetastar_top_ttframe = book< TH1F>( "cosThetastar_top_ttframe", "cos(#Theta*)_{t}",1000,-1,1);
    cosThetastar_antitop_ttframe = book< TH1F>( "cosThetastar_antitop_ttframe", "cos(#Theta*)_{#bar{t}}",1000,-1,1);
    Pt_top_ttframe = book< TH1F>( "Pt_top_ttframe", "P_{T,t}* [GeV/c]",1000,0,2000);
    Pt_antitop_ttframe = book< TH1F>( "Pt_antitop_ttframe", "P_{T,#bar{t}}* [GeV/c]",1000,0,2000);
    M_ttbar_vs_Pt_top_ttframe = book<TH2F>( "M_ttbar_vs_Pt_top_ttframe", "M_{t#bar{t}} [GeV/c^{2}] vs P_{T,t}* [GeV/c]",500,0,5000,500,0,2000); 
    M_ttbar_vs_Pt_antitop_ttframe = book<TH2F>( "M_ttbar_vs_Pt_antitop_ttframe", "M_{t#bar{t}} [GeV/c^{2}] vs P_{T,#bar{t}}* [GeV/c]",500,0,5000,500,0,2000);

    M_ttbar_vs_eta_top = book<TH2F>( "M_ttbar_vs_eta_top", "M_{t#bar{t}} [GeV/c^{2}] vs #eta_{t}",500,0,5000,500,-5,5); 
    M_ttbar_vs_eta_antitop = book<TH2F>( "M_ttbar_vs_eta_antitop", "M_{t#bar{t}} [GeV/c^{2}] vs #eta_{#bar{t}}",500,0,5000,500,-5,5);

    h_ttbargen = ctx.get_handle<TTbarGen>("ttbargen");

    dileptop_emu_mupt = book< TH1F>( "dileptop_emu_mupt", "t#bar{t}#(e#mu), p_{T}^{#mu}",50,0,1000);
    dileptop_emu_mueta  = book< TH1F>( "dileptop_emu_mueta", "t#bar{t}#(e#mu), #eta^{#mu}",50,-5,5);
    dileptop_emu_ept= book< TH1F>( "dileptop_emu_ept", "t#bar{t}#(e#mu), p_{T}^{e}",50,0,1000);
    dileptop_emu_eeta =  book< TH1F>( "dileptop_emu_eeta", "t#bar{t}#(e#mu), #eta^{#e}",50,-5,5);
    dileptop_taumu_mupt= book< TH1F>( "dileptop_taumu_mupt", "t#bar{t}#(#tau#mu), p_{T}^{#mu}",50,0,1000);
    dileptop_taumu_mueta=  book< TH1F>( "dileptop_taumu_mueta", "t#bar{t}#(#tau#mu), #eta^{#mu}",50,-5,5);
    dileptop_taumu_taupt= book< TH1F>( "dileptop_taumu_taupt", "t#bar{t}#(#tau#mu), p_{T}^{#tau}",50,0,1000);
    dileptop_taumu_taueta=  book< TH1F>( "dileptop_taumu_taueta", "t#bar{t}#(#tau#mu), #eta^{#tau}",50,-5,5);
    dileptop_mumu_mu1pt= book< TH1F>( "dileptop_mumu_mu1pt", "t#bar{t}#(#mu#mu), p_{T}^{#mu} 1",50,0,1000);
    dileptop_mumu_mu1eta=  book< TH1F>( "dileptop_mumu_mu1eta", "t#bar{t}#(#mu#mu), #eta^{#mu} 1",50,-5,5);
    dileptop_mumu_mu2pt= book< TH1F>( "dileptop_mumu_mu2pt", "t#bar{t}#(#mu#mu), p_{T}^{#mu} 2",50,0,1000);
    dileptop_mumu_mu2eta=  book< TH1F>( "dileptop_mumu_mu2eta", "t#bar{t}#(#mu#mu), #eta^{#mu} 2",50,-5,5);
    
}


void TTbarGenHists::fill(const uhh2::Event & e){
    //do not fill histograms if ttbargen information has not been filled
    if(!e.is_valid(h_ttbargen)){
      return;
    }
    const auto & ttbargen = e.get(h_ttbargen);
    
    LorentzVector top = ttbargen.Top().v4();
    LorentzVector antitop = ttbargen.Antitop().v4();

 
    double mttbar_gen = (top+antitop).M();
    double  ptttbar_gen = ( top + antitop ).Pt();
    double sh = (e.genparticles->at(0).v4()+ e.genparticles->at(1).v4()).M();

    DecayChannel->Fill(ttbargen.DecayChannel(), e.weight);
    Mttbar_gen->Fill( mttbar_gen,e.weight);
    Pt_ttbar_gen->Fill ( ptttbar_gen, e.weight);
    shat->Fill(sh, e.weight);
    
    M_top->Fill( top.M(), e.weight);
    M_antitop->Fill(antitop.M(), e.weight);
    Pt_top->Fill( top.Pt(), e.weight);
    Pt_antitop->Fill(antitop.Pt(), e.weight);

    Pt_top_over_shat->Fill( top.Pt()/sh, e.weight);
    Pt_antitop_over_shat->Fill( antitop.Pt()/sh, e.weight);
    Pt_top_over_M_ttbar->Fill( top.Pt()/mttbar_gen, e.weight);
    Pt_antitop_over_M_ttbar->Fill( antitop.Pt()/mttbar_gen, e.weight);

    eta_top->Fill( top.eta(), e.weight);
    eta_antitop->Fill(antitop.eta(), e.weight);
    y_top->Fill( top.Rapidity(), e.weight);
    y_antitop->Fill(antitop.Rapidity(), e.weight);

    double difabseta = fabs( top.eta()) - fabs( antitop.eta());
    double difabsy = fabs( top.Rapidity()) - fabs( antitop.Rapidity());

    diffabseta->Fill(difabseta, e.weight);
    diffabsy->Fill(difabsy, e.weight);

    double deltaR_top = std::max (std::max( uhh2::deltaR(ttbargen.bTop(), ttbargen.Wdecay1() ), 
				  uhh2::deltaR(ttbargen.bTop(), ttbargen.Wdecay2() ) )
			     , uhh2::deltaR(ttbargen.Wdecay1(), ttbargen.Wdecay2() ) );

    double deltaR_antitop = std::max (std::max( uhh2::deltaR(ttbargen.bAntitop(), ttbargen.WMinusdecay1() ), 
				      uhh2::deltaR(ttbargen.bAntitop(), ttbargen.WMinusdecay2() ) )
				 , uhh2::deltaR(ttbargen.WMinusdecay1(), ttbargen.WMinusdecay2() ) );

    deltaR_top_decays->Fill(deltaR_top,e.weight);
    deltaR_antitop_decays->Fill(deltaR_antitop,e.weight);
    
    M_ttbar_vs_deltaR_top->Fill(mttbar_gen, deltaR_top, e.weight);
    M_ttbar_vs_deltaR_antitop->Fill(mttbar_gen, deltaR_antitop, e.weight);

    double deltaR_Wplus = uhh2::deltaR(ttbargen.Wdecay1(), ttbargen.Wdecay2());
    double deltaR_Wminus = uhh2::deltaR(ttbargen.WMinusdecay1(), ttbargen.WMinusdecay2());
    deltaR_Wplus_decays->Fill(deltaR_Wplus,e.weight);
    deltaR_Wminus_decays->Fill(deltaR_Wminus,e.weight);
    M_ttbar_vs_deltaR_Wplus->Fill(mttbar_gen, deltaR_Wplus, e.weight);
    M_ttbar_vs_deltaR_Wminus->Fill(mttbar_gen, deltaR_Wminus, e.weight);

    Pt_top_vs_deltaR_top->Fill(top.Pt(), deltaR_top, e.weight);
    Pt_antitop_vs_deltaR_antitop->Fill(antitop.Pt(), deltaR_antitop, e.weight);
    M_ttbar_vs_Pt_top->Fill(mttbar_gen, top.Pt(), e.weight);
    M_ttbar_vs_Pt_antitop->Fill(mttbar_gen, antitop.Pt(), e.weight);
    Pt_top_vs_Pt_antitop->Fill(top.Pt(), antitop.Pt(), e.weight);
    M_ttbar_vs_shat->Fill(mttbar_gen,sh,e.weight);

    shat_vs_deltaR_top->Fill(sh, deltaR_top, e.weight);
    shat_vs_deltaR_antitop->Fill(sh, deltaR_antitop, e.weight);
    shat_vs_Pt_top->Fill(sh, top.Pt(),  e.weight);
    shat_vs_Pt_antitop->Fill(sh, antitop.Pt(),  e.weight);

    Pt_b->Fill( ttbargen.bTop().pt(), e.weight);
    Pt_antib->Fill( ttbargen.bAntitop().pt(), e.weight);  
    eta_b->Fill( ttbargen.bTop().eta(), e.weight);
    eta_antib->Fill( ttbargen.bAntitop().eta(), e.weight); 
    y_b->Fill( ttbargen.bTop().v4().Rapidity(), e.weight);
    y_antib->Fill( ttbargen.bAntitop().v4().Rapidity(), e.weight); 
    M_b->Fill( ttbargen.bTop().v4().M(), e.weight);
    M_antib->Fill( ttbargen.bAntitop().v4().M(), e.weight); 

    Pt_Wplus->Fill( ttbargen.WTop().pt(), e.weight);
    Pt_Wminus->Fill( ttbargen.WAntitop().pt(), e.weight); 
    eta_Wplus->Fill( ttbargen.WTop().eta(), e.weight);
    eta_Wminus->Fill( ttbargen.WAntitop().eta(), e.weight);
    y_Wplus->Fill( ttbargen.WTop().v4().Rapidity(), e.weight);
    y_Wminus->Fill( ttbargen.WAntitop().v4().Rapidity(), e.weight); 
    if(ttbargen.WTop().v4().isTimelike())
      M_Wplus->Fill( ttbargen.WTop().v4().M(), e.weight);
    if(ttbargen.WAntitop().v4().isTimelike())
      M_Wminus->Fill( ttbargen.WAntitop().v4().M(), e.weight); 

    TLorentzVector top_ttframe(0,0,0,0);
    top_ttframe.SetPtEtaPhiE(top.pt(), top.eta(), top.phi(), top.E());
    TLorentzVector antitop_ttframe(0,0,0,0);
    antitop_ttframe.SetPtEtaPhiE(antitop.pt(), antitop.eta(), antitop.phi(), antitop.E());
    TLorentzVector ttbar(0,0,0,0);
    ttbar.SetPtEtaPhiE((top+antitop).pt(), (top+antitop).eta(), (top+antitop).phi(), (top+antitop).E()); 
    
    top_ttframe.Boost(-ttbar.BoostVector());
    antitop_ttframe.Boost(-ttbar.BoostVector());

    cosThetastar_top_ttframe->Fill(cos(top_ttframe.Theta()) ,e.weight);
    cosThetastar_antitop_ttframe->Fill(cos(antitop_ttframe.Theta()) ,e.weight);
    Pt_top_ttframe->Fill(top_ttframe.Pt(),e.weight);
    Pt_antitop_ttframe->Fill(antitop_ttframe.Pt(),e.weight);  

    M_ttbar_vs_Pt_top_ttframe->Fill(mttbar_gen, top_ttframe.Pt(),e.weight );
    M_ttbar_vs_Pt_antitop_ttframe->Fill(mttbar_gen, antitop_ttframe.Pt(),e.weight ); 
    M_ttbar_vs_eta_top->Fill(mttbar_gen, top.eta(),  e.weight);
    M_ttbar_vs_eta_antitop->Fill(mttbar_gen, antitop.eta(),  e.weight);


    if (ttbargen.DecayChannel() == 7 || ttbargen.DecayChannel() == 5 || ttbargen.DecayChannel() == 9)
       {
          GenParticle lep1;
          GenParticle lep2;
          if (abs(ttbargen.Wdecay1().pdgId())== 11 ||  abs(ttbargen.Wdecay1().pdgId())== 13 || abs(ttbargen.Wdecay1().pdgId())== 15) lep1 = ttbargen.Wdecay1();
          else lep1 = ttbargen.Wdecay2();
          
          if (abs(ttbargen.WMinusdecay1().pdgId())== 11 ||  abs(ttbargen.WMinusdecay1().pdgId())== 13 || abs(ttbargen.WMinusdecay1().pdgId())== 15) lep2 = ttbargen.WMinusdecay1();
          else lep2 = ttbargen.WMinusdecay2();
          
          if (ttbargen.DecayChannel() == 7){
             if (abs(lep1.pdgId()) == 13){
                dileptop_emu_mupt->Fill(lep1.pt(), e.weight);
                dileptop_emu_mueta ->Fill(lep1.eta(), e.weight);
                dileptop_emu_ept->Fill(lep2.pt(), e.weight);
                dileptop_emu_eeta->Fill(lep2.eta(), e.weight);
             }
             else {
                dileptop_emu_mupt->Fill(lep2.pt(), e.weight);
                dileptop_emu_mueta ->Fill(lep2.eta(), e.weight);
                dileptop_emu_ept->Fill(lep1.pt(), e.weight);
                dileptop_emu_eeta->Fill(lep1.eta(), e.weight);
             }
          }
          if (ttbargen.DecayChannel() == 9){
             if (abs(lep1.pdgId()) == 13){
                dileptop_taumu_mupt->Fill(lep1.pt(), e.weight);
                dileptop_taumu_mueta->Fill(lep1.eta(), e.weight);
                dileptop_taumu_taupt->Fill(lep2.pt(), e.weight);
                dileptop_taumu_taueta->Fill(lep2.eta(), e.weight);
             }
             else {
                dileptop_taumu_mupt->Fill(lep2.pt(), e.weight);
                dileptop_taumu_mueta ->Fill(lep2.eta(), e.weight);
                dileptop_taumu_taupt->Fill(lep1.pt(), e.weight);
                dileptop_taumu_taueta->Fill(lep1.eta(), e.weight);
             }
          }
          if (ttbargen.DecayChannel() == 5){
             if (abs(lep1.pdgId()) == 13){
                dileptop_mumu_mu1pt->Fill(lep1.pt(), e.weight);
                dileptop_mumu_mu1eta->Fill(lep1.eta(), e.weight);
                dileptop_mumu_mu2pt->Fill(lep2.pt(), e.weight);
                dileptop_mumu_mu2eta->Fill(lep2.eta(), e.weight);
             }
             else{
                dileptop_mumu_mu1pt->Fill(lep2.pt(), e.weight);
                dileptop_mumu_mu1eta ->Fill(lep2.eta(), e.weight);
                dileptop_mumu_mu2pt->Fill(lep1.pt(), e.weight);
                dileptop_mumu_mu2eta->Fill(lep1.eta(), e.weight);
             }
          }
       }
}
