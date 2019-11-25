#include "UHH2/TstarTstar/include/TstarTstarRecoTstarHists.h"
#include "UHH2/core/include/Event.h"

#include "TH1F.h"
#include "TH2F.h"
#include <iostream>

using namespace std;
using namespace uhh2;
//using namespace uhh2examples;

namespace {
    
  // invariant mass of a lorentzVector, but safe for timelike / spacelike vectors
  float inv_mass(const LorentzVector& p4){ return p4.isTimelike() ? p4.mass() : -sqrt(-p4.mass2()); }

}

TstarTstarRecoTstarHists::TstarTstarRecoTstarHists(Context & ctx, const string & dirname): Hists(ctx, dirname){
  // book all histograms here
  bool is_tgtg = false; 
  bool is_tgtgamma = false;
  if(ctx.get("channel") == "tgtg") is_tgtg = true;
  if(ctx.get("channel") == "tgtgamma") is_tgtgamma = true;

  //Delta_R
  h_recohyp_ = ctx.get_handle<ReconstructionHypothesis>("TTbarReconstruction_best");
  h_is_ttbar_reconstructed_ = ctx.get_handle< bool >("is_ttbar_reconstructed_chi2");


  //For tests of ttbar reconstruction
  h_ttbar_hyps_ = ctx.get_handle< std::vector<ReconstructionHypothesis> >("TTbarReconstruction");
  // //FixME: correct match hists are not filled
  // book<TH1F>("Mtoplep_CorrectMatch", "M_{best match}^{toplep} [GeV]", 200, 0, 1000);
  // book<TH1F>("Mtophad_CorrectMatch", "M_{best macth}^{tophad} [GeV]", 200, 0, 1000);


  book<TH1F>("Mtoplep_bestchi2", "M^{best #chi^{2}}_{toplep} [GeV]", 100, 0, 500);
  book<TH1F>("Mtophad_bestchi2", "M^{best #chi^{2}}_{tophad} [GeV]", 100, 0, 500);
  book<TH1F>("Chi2_bestchi2", "#chi^{2}_{best #chi^{2}}", 50, 0, 100);
  book<TH1F>("dRCorrMatch_bestchi2", "#sum dR^{Match}_{best #chi^{2}}", 50, 0, 1e1);

  //For tests of tg+tg reconstruction (with TstarTstar_tgluon_tgluon_Reconstruction class)
  if(is_tgtg) h_recohyp_tstartstar_best_ = ctx.get_handle<ReconstructionTstarHypothesis>("TstarTstar_tgtg_best");
  if(is_tgtgamma) h_recohyp_tstartstar_best_ = ctx.get_handle<ReconstructionTstarHypothesis>("TstarTstar_tgtgamma_best");
  // h_recohyp_tstartstar_tgtg_ = ctx.get_handle< std::vector<ReconstructionHypothesis> >("TstarTstar_tgtg_hyps");
  book<TH1F>("M_Tstar_lep", "Mass T^{*}_{tgtg,lep} [GeV]", 100, 0, 3000);
  book<TH1F>("M_Tstar_had", "Mass T^{*}_{tgtg,had} [GeV]", 100, 0, 3000);

  book<TH1F>("AK8_id_topjets_ttbar", "ID of the close to rec.ttbar AK8 jets", 30, 0, 29);


  book<TH1F>("Pt_gluon_Tstar_lep", "p_{T} gluon, T^{*}_{tgtg,lep} [GeV]", 100, 0, 1000);
  book<TH1F>("Pt_gluon_Tstar_had", "p_{T} gluon, T^{*}_{tgtg,had} [GeV]", 100, 0, 1000);


  book<TH1F>("dR_top_gluon_Tstar_lep", "#DeltaR(top, gluon) T^{*}_{tgtg,lep}", 50, 0, 1e1);
  book<TH1F>("dR_top_gluon_Tstar_had", "#DeltaR(top, gluon) T^{*}_{tgtg,had}", 50, 0, 1e1);

  book<TH1F>("Pt_ratio_Tstar_lep_to_Tstar_had", "(p_{T} T^{*}_{tgtg,lep})/(p_{T} T^{*}_{tgtg,had})", 100, 0, 1e1);

}


void TstarTstarRecoTstarHists::fill(const Event & event){
  // fill the histograms. Please note the comments in the header file:
  // 'hist' is used here a lot for simplicity, but it will be rather
  // slow when you have many histograms; therefore, better
  // use histogram pointers as members as in 'UHH2/common/include/ElectronHists.h'
  
  // Don't forget to always use the weight when filling.
  double weight = event.weight;
  

  ReconstructionHypothesis hyp = event.get(h_recohyp_);
  hist("Mtoplep_bestchi2")->Fill(inv_mass(hyp.toplep_v4()), weight);
  hist("Mtophad_bestchi2")->Fill(inv_mass(hyp.tophad_v4()), weight);
  hist("Chi2_bestchi2")->Fill(hyp.discriminator("chi2_total"), weight);
  hist("dRCorrMatch_bestchi2")->Fill(hyp.discriminator("CorrectMatch"), weight);


  //  cout<<"Number of ttbar candidates: "<<candidates.size()<<endl;
  //Fill hists for tgtg reconstruction check
  ReconstructionTstarHypothesis tstar_hyp_best = event.get(h_recohyp_tstartstar_best_);
  //  std::vector<ReconstructionTstarHypothesis> tstar_hyps = event.get(h_recohyp_tstartstar_tgtg_);
  hist("M_Tstar_lep")->Fill(inv_mass(tstar_hyp_best.tstarlep_v4()), weight);
  hist("M_Tstar_had")->Fill(inv_mass(tstar_hyp_best.tstarhad_v4()), weight);
  double dR_top_gluon_Tstar_had =  deltaR(tstar_hyp_best.gluon1_v4(),tstar_hyp_best.ttbar_hyp().tophad_v4());
  double dR_top_gluon_Tstar_lep =  deltaR(tstar_hyp_best.gluon2_v4(),tstar_hyp_best.ttbar_hyp().toplep_v4());
  hist("dR_top_gluon_Tstar_lep")->Fill(dR_top_gluon_Tstar_lep, weight);
  hist("dR_top_gluon_Tstar_had")->Fill(dR_top_gluon_Tstar_had, weight);
  hist("Pt_gluon_Tstar_lep")->Fill(tstar_hyp_best.gluon2_v4().pt(), weight);
  hist("Pt_gluon_Tstar_had")->Fill(tstar_hyp_best.gluon1_v4().pt(), weight);
  hist("Pt_ratio_Tstar_lep_to_Tstar_had")->Fill(tstar_hyp_best.tstarlep_v4().pt()/tstar_hyp_best.tstarhad_v4().pt(), weight);

  for(unsigned int i=0;i<tstar_hyp_best.used_ttbarjet_flags().size();i++){
    if(tstar_hyp_best.used_ttbarjet_flags()[i]) hist("AK8_id_topjets_ttbar")->Fill(i, weight);
  }
  //  book<TH1F>("dR_top_gluon_Tstar_lep", "#DeltaR(top, gluon) T^{*}_{tgtg,lep}", 50, 0, 1e1);
  //  book<TH1F>("dR_top_gluon_Tstar_had", "#DeltaR(top, gluon) T^{*}_{tgtg,had}", 50, 0, 1e1);

  //  cout<<"Following values filled into mass histograms: "<<inv_mass(tstar_hyp_best.tstarlep_v4())<<", "<<inv_mass(tstar_hyp_best.tstarhad_v4())<<endl;
}


void TstarTstarRecoTstarHists::fill_ttbarhyps(const Event & event, const ReconstructionHypothesis & hyp){
    
  // Don't forget to always use the weight when filling.
  double weight = event.weight; 
  hist("Mtoplep_all")->Fill(inv_mass(hyp.toplep_v4()), weight);
  hist("Mtophad_all")->Fill(inv_mass(hyp.tophad_v4()), weight);

}

TstarTstarRecoTstarHists::~TstarTstarRecoTstarHists(){}
