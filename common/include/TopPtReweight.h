#pragma once

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Hists.h"
#include "UHH2/core/include/Event.h"

#include <UHH2/common/include/TTbarGen.h>

#include "TH1F.h"

 /*
 ** Based on the class TopPtWeight from VLQ semilep Repo (thanks to Dominik & Heiner)
 ** 
 ** The hist class can be used to calculate the change in the total normalisation.  
 ** Which then can be adjusted with the norm_factor.
 **
 */


class TopPtReweight : public uhh2::AnalysisModule {
 public:
  explicit TopPtReweight(uhh2::Context& ctx,
			 float a, float b,
			 const std::string& ttgen_name ="",
			 const std::string& weight_name="weight_ttbar",
			 bool apply_weight=false,
			 double norm_factor=1.0);


  virtual bool process(uhh2::Event& event) override;
 private:
  uhh2::Event::Handle<TTbarGen> h_ttbargen_;
  uhh2::Event::Handle<float> h_weight_;
  float a_, b_;
  bool apply_weight_;
  double norm_factor_;
  std::string version_;
  std::string ttgen_name_;
  std::string weight_name_;
};



class TopPtReweightHist: public uhh2::Hists {
 public:
  explicit TopPtReweightHist(uhh2::Context& ctx,
			     const std::string & dirname,
			     const std::string & weight_name);
  
  virtual void fill(const uhh2::Event& event) override;
 private:
  uhh2::Event::Handle<float> h_weight_;
  TH1F * hist;
};
