#include <iostream>
#include <memory>

#include "UHH2/core/include/AnalysisModule.h"
#include "UHH2/core/include/Event.h"
#include "UHH2/common/include/PDFWeights.h"

using namespace uhh2;

class PDFWeightsModule: public AnalysisModule {

 public:
  explicit PDFWeightsModule(Context & ctx);
  ~PDFWeightsModule();
  virtual bool process(Event & event) override;

 private:
  PDFWeights* m_pdfweights;

  std::vector<double> m_sumofweights;
  int m_Ntotal;
  TString m_pdfname;
  TString m_oname;
};

PDFWeightsModule::~PDFWeightsModule(){

  TString outfilename = m_oname;
  outfilename +="_";
  outfilename += m_pdfname;
  outfilename += "_weights.txt";
  
  std::ofstream outfile;
  outfile.open (((std::string)outfilename).c_str());
  outfile << m_Ntotal <<std::endl;
  for(unsigned int i=0; i< m_pdfweights->GetNWeights(); ++i){
    outfile<< m_sumofweights[i] << " ";
  }
  outfile.close();

  delete m_pdfweights;

}

PDFWeightsModule::PDFWeightsModule(Context & ctx){

  m_pdfname = "NNPDF30_lo_as_0130";
  m_pdfweights = new PDFWeights(m_pdfname);

  m_Ntotal=0;
  
  m_sumofweights.clear();
  for(unsigned int i=0; i< m_pdfweights->GetNWeights(); ++i){
    m_sumofweights.push_back(0);
  }
  m_oname = ctx.get("dataset_version");
}

bool PDFWeightsModule::process(Event & event) {


  m_Ntotal++;

  std::vector<double> weights = m_pdfweights->GetWeightList(event);

  for(unsigned int i=0; i< m_pdfweights->GetNWeights(); ++i){
    //std::cout << i << "   " << weights[i]<< std::endl;
    m_sumofweights[i]+=weights[i];
  }

  return true;
}

UHH2_REGISTER_ANALYSIS_MODULE(PDFWeightsModule)
