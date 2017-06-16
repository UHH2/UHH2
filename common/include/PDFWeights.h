#ifndef PDFWeights_H
#define PDFWeights_H

#include "LHAPDF/LHAPDF.h"
#include "TSystem.h"
#include "UHH2/core/include/Event.h"

/**
 *  @short pdf re-weighting tool
 *
 *
 */
class PDFWeights {
public:

  ///Default constructor
  PDFWeights(TString pdfname= "cteq66", TString pdfweightdir="");
  ///Default destructor
  ~PDFWeights() {};

  ///returns quadratically summed pdf uncertainties
  double GetWeight(unsigned int index, uhh2::Event & event);

  ///returns list of pdf uncertainties from eigenvectors
  std::vector<double> GetWeightList(uhh2::Event & event);

  unsigned int GetNWeights(){return m_N_unc;}
  
private:

  bool m_libvalid;

  unsigned int m_N_unc;

  bool m_normalize_to_total_sum;
  std::vector<double> m_sumofweights;
  int m_N_tot;
  

};

#endif
