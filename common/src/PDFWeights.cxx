#include "UHH2/common/include/PDFWeights.h"
#include <iostream>

PDFWeights::PDFWeights(TString pdfname, TString pdfweightdir) 
{

    if( ( gSystem->Load("libLHAPDF") )==-1){
      std::cerr << "libLHAPDF not found, no pdf weights will be applied. To apply pdf re-weighting, add path to libLHAPDF.so to LD_LIBRARY_PATH" << std::endl;
      m_libvalid=false;
      return;
    }
    m_libvalid=true;
  
    LHAPDF::initPDFSet(1, (std::string)(pdfname+".LHgrid"));
    m_N_unc = LHAPDF::numberPDF();
    std::cout << "got pdfset number " << m_N_unc << std::endl;

    for(unsigned int i=0; i<=m_N_unc; i++){
      LHAPDF::PDF* pdf = LHAPDF::mkPDF( (std::string) pdfname, i);
      m_pdfs.push_back(pdf);
    }

    m_normalize_to_total_sum=false;
    if(pdfweightdir!=""){
      m_normalize_to_total_sum=true;
      
      TString filename = pdfweightdir;
      filename +="_";
      filename += pdfname;
      filename += "_weights.txt";

      std::cout << "Do pdf re-weighting with respect to weights in file: " << filename << std::endl;

      std::ifstream infile (((std::string)filename).c_str());
      
      infile>>m_N_tot;

      do{
	double number;
	infile>>number;
	m_sumofweights.push_back(number);
      }while ( !infile.eof() );
      //erase last entry which is loaded twice
      m_sumofweights.erase(m_sumofweights.end()-1);

      //std::cout << "total number of events before selection: " << m_N_tot <<  std::endl;
      //for(unsigned int i=0; i< m_sumofweights.size(); ++i){
	//std::cout << "sum of weights for pdf set " << i+1 << ": " << m_sumofweights[i] << std::endl;
      //}

      infile.close();

      if(m_sumofweights.size()!=m_N_unc){
	std::cerr << "Number of event weights in input file ("<< m_sumofweights.size()<< ") != number of parameters of chosen pdf set ("<< m_N_unc<< ")"<< std::endl;
      }
    }
}

std::vector<double> PDFWeights::GetWeightList(const uhh2::Event & event){
  std::vector<double> pdf_weights;

  if(!m_libvalid) return pdf_weights;

  //pdf weighting code taken from https://twiki.cern.ch/twiki/bin/view/CMS/TWikiTopRefSyst#PDF_uncertainties
 

  double x1=event.genInfo->pdf_x1();
  double x2=event.genInfo->pdf_x2();
 
  int id1 = event.genInfo->pdf_id1();
  int id2 = event.genInfo->pdf_id2();
 
  double q = event.genInfo->pdf_scalePDF();

  double xpdf1 = m_pdfs[0]->xfxQ(id1, x1, q);
  double xpdf2 = m_pdfs[0]->xfxQ(id2, x2, q);

  double w0 = xpdf1 * xpdf2;
  for(unsigned int i=1; i <=m_N_unc; ++i){
    double xpdf1_new = m_pdfs[i]->xfxQ(id1, x1, q);
    double xpdf2_new = m_pdfs[i]->xfxQ(id2, x2, q);
    double weight = xpdf1_new * xpdf2_new / w0;
    if(m_normalize_to_total_sum){
      pdf_weights.push_back(weight/m_sumofweights[i-1]*m_N_tot);
      
    }
    else{ 
      pdf_weights.push_back(weight);
    }
  }

  return pdf_weights;
  
}


double PDFWeights::GetWeight(unsigned int index, const uhh2::Event & event){

  if(!m_libvalid) return 1.;

  std::vector<double> pdf_weights = GetWeightList(event);

  if(index>pdf_weights.size() || index<1){
    std::cerr << "PDF index "  << index << " out of range, should be >=1 and <= " << pdf_weights.size() << std::endl;
    return 1.;
  }

  return pdf_weights.at(index-1);

}
