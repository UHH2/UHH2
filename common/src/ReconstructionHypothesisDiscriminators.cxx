#include "UHH2/common/include/ReconstructionHypothesisDiscriminators.h"
#include "UHH2/core/include/Utils.h"

#include <set>

using namespace uhh2;
using namespace std;

namespace {
    
  // invariant mass of a lorentzVector, but safe for timelike / spacelike vectors
  float inv_mass(const LorentzVector& p4){ return p4.isTimelike() ? p4.mass() : -sqrt(-p4.mass2()); }

}

const ReconstructionHypothesis * get_best_hypothesis(const std::vector<ReconstructionHypothesis> & hyps, const std::string & label){

    const ReconstructionHypothesis * best = nullptr;
    float current_best_disc = numeric_limits<float>::infinity();
    for(const auto & hyp : hyps){
        if(!hyp.has_discriminator(label)) continue;
        auto disc = hyp.discriminator(label);
        if(disc < current_best_disc){
            best = &hyp;
            current_best_disc = disc;
        }
    }
    if(std::isfinite(current_best_disc)){
        return best;
    }
    else{
        return nullptr;
    }
}
////

Chi2Discriminator::Chi2Discriminator(Context & ctx, const std::string & rechyps_name, const cfg & config_): config(config_){

  h_hyps = ctx.get_handle<vector<ReconstructionHypothesis>>(rechyps_name);

  Mtlep_mean_  = 174.;
  Mtlep_sigma_ =  18.;
  Mthad_mean_  = 181.;
  Mthad_sigma_ =  15.;
}

bool Chi2Discriminator::process(uhh2::Event& event){

  auto& hyps = event.get(h_hyps);

  for(auto& hyp : hyps){

    const float Mtlep_reco = inv_mass(hyp.toplep_v4());
    const float Mthad_reco = inv_mass(hyp.tophad_v4());

    const double chi2_tlep = pow((Mtlep_reco - Mtlep_mean_) / Mtlep_sigma_, 2);
    const double chi2_thad = pow((Mthad_reco - Mthad_mean_) / Mthad_sigma_, 2);

    hyp.set_discriminator(config.discriminator_label+"_tlep", chi2_tlep);
    hyp.set_discriminator(config.discriminator_label+"_thad",             chi2_thad);
    hyp.set_discriminator(config.discriminator_label        , chi2_tlep + chi2_thad);
  }

  return true;
}
////

Chi2DiscriminatorTTAG::Chi2DiscriminatorTTAG(Context & ctx, const std::string & rechyps_name, const cfg & config_): config(config_){

  h_hyps = ctx.get_handle<vector<ReconstructionHypothesis>>(rechyps_name);

  Mtlep_mean_  = 174.;
  Mtlep_sigma_ =  18.;
  Mthad_mean_  = 181.;
  Mthad_sigma_ =  15.;

  use_subjet_mass_ = true;
}

bool Chi2DiscriminatorTTAG::process(uhh2::Event& event){

  auto& hyps = event.get(h_hyps);

  for(auto& hyp : hyps){

    if(!hyp.tophad_topjet_ptr()) throw std::runtime_error("Chi2DiscriminatorTTAG::process -- null pointer for TopJet associated to hadronic-top");

    float Mthad(-1.);
    if(use_subjet_mass_){

      LorentzVector tjet_subjet_sum;
      for(const auto& subj : hyp.tophad_topjet_ptr()->subjets()) tjet_subjet_sum += subj.v4();

      Mthad = inv_mass(tjet_subjet_sum);
    }
    else Mthad = hyp.tophad_topjet_ptr()->softdropmass();

    const float Mtlep_reco = inv_mass(hyp.toplep_v4());
    const float Mthad_reco = Mthad;

    const double chi2_tlep = pow((Mtlep_reco - Mtlep_mean_) / Mtlep_sigma_, 2);
    const double chi2_thad = pow((Mthad_reco - Mthad_mean_) / Mthad_sigma_, 2);

    hyp.set_discriminator(config.discriminator_label+"_tlep", chi2_tlep);
    hyp.set_discriminator(config.discriminator_label+"_thad",             chi2_thad);
    hyp.set_discriminator(config.discriminator_label        , chi2_tlep + chi2_thad);
  }

  return true;
}
////

TopDRMCDiscriminator::TopDRMCDiscriminator(Context & ctx, const std::string & rechyps_name, const cfg & config_): config(config_){
    h_hyps = ctx.get_handle<vector<ReconstructionHypothesis>>(rechyps_name);
    h_ttbargen = ctx.get_handle<TTbarGen>(config.ttbargen_name);
}


bool TopDRMCDiscriminator::process(uhh2::Event & event){
    auto & hyps = event.get(h_hyps);
    const auto & ttbargen = event.get(h_ttbargen);
    for(auto & hyp: hyps){
        auto deltar_sum = deltaR(ttbargen.Top(), hyp.top_v4()) + deltaR(ttbargen.Antitop(), hyp.antitop_v4());
        hyp.set_discriminator(config.discriminator_label, deltar_sum);
    }
    return true;
}



CorrectMatchDiscriminator::CorrectMatchDiscriminator(Context & ctx, const std::string & rechyps_name, const cfg & config_): config(config_){
    h_hyps = ctx.get_handle<vector<ReconstructionHypothesis>>(rechyps_name);
    h_ttbargen = ctx.get_handle<TTbarGen>(config.ttbargen_name);
}

namespace {

// match particle p to one of the jets (Delta R < 0.3); return the deltaR
// of the match.
template<typename T> // T should inherit from Particle
float match_dr(const Particle & p, const std::vector<T> & jets, int& index){
  float mindr = infinity;
  index = -1;
  for(unsigned int i=0; i<jets.size(); ++i){
    float dR = deltaR(p, jets.at(i));
    if( dR <0.3 && dR<mindr) {
      mindr=dR;
      index=i;
    }
  }
  return mindr;
}

}

bool CorrectMatchDiscriminator::process(uhh2::Event & event){
    auto & hyps = event.get(h_hyps);
    const auto & ttbargen = event.get(h_ttbargen);
    auto dec = ttbargen.DecayChannel();
    if(dec != TTbarGen::e_ehad && dec != TTbarGen::e_muhad){
        for(auto & hyp: hyps){
            hyp.set_discriminator(config.discriminator_label, infinity);
        }
        return true;
    }
    
    // note that it is allowed that two partons from the hadronic ttbar decay match the same jet.
    for(auto & hyp: hyps){
        auto hadr_jets = hyp.tophad_jets();
        auto lept_jets = hyp.toplep_jets();
        
        if(lept_jets.size() != 1){
            hyp.set_discriminator(config.discriminator_label, infinity);
            continue;
        }
        if(hadr_jets.size() > 3){ // < 3 is allowed ...
            hyp.set_discriminator(config.discriminator_label, infinity);
            continue;
        }

        //index lists of jets that can be matched to partons
        std::set<int> matched_hadr_jets;

        // match b jets
        int index_l, index_h;
        float correct_dr = match_dr(ttbargen.BLep(), lept_jets, index_l) + match_dr(ttbargen.BHad(), hadr_jets, index_h);
        if(index_h >= 0) matched_hadr_jets.insert(index_h);
        //match quarks from W decays
        correct_dr += match_dr(ttbargen.Q1(), hadr_jets, index_h);
        if(index_h >= 0) matched_hadr_jets.insert(index_h);
        correct_dr += match_dr(ttbargen.Q2(), hadr_jets, index_h);
        if(index_h >= 0) matched_hadr_jets.insert(index_h);
        
        // if not all jets of the hadronic side of the reconstruction could be matched: infinite
        // value:
        if(matched_hadr_jets.size() != hadr_jets.size()){
            hyp.set_discriminator(config.discriminator_label, infinity);
            continue;
        }

        //add deltaR between reconstructed and true neutrino
        correct_dr += deltaR(ttbargen.Neutrino(), hyp.neutrino_v4());
        hyp.set_discriminator(config.discriminator_label, correct_dr);
    }
    return true;
}

