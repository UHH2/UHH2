//investigation of difference between two hists from AnalysisTree
// author: A.Karavdina
// date: 22.02.2019
// Run it with following command:
// root -l -b -q compare_hists.C

void compare_hists(){
  gStyle->SetOptStat(0);
  gStyle->SetTitleSize(0.045,"x");  
  gStyle->SetTitleSize(0.045,"y");
  gStyle->SetTitleYOffset(0.9);

  Double_t w = 600;
  Double_t h = 600;

  // bool isWeights = true;
  bool isWeights = false;

  //Files with Analysis tree, e.g after UHH2 ntuple-writer
  //file1, hist 1
  TString path1 = "/nfs/dust/cms/user/karavdia/CMSSW_10_2_11/src/UHH2/core/python/";
  TString name1 = "Ntuple_tthad2018_NotredoBslimmed_10GeV.root";
  TString gl_label1 = "DeepCSV, jet pt>100GeV (original)";

  TFile *input1 = TFile::Open(path1+name1);
  TTree *TTreeAna1 = (TTree*)input1->Get("AnalysisTree");
  TTreeAna1->SetAlias("DeepCSV", "jetsAk4Puppi.m_btag_DeepCSV_probb+jetsAk4Puppi.m_btag_DeepCSV_probbb");
  TTreeAna1->SetAlias("jetPt", "jetsAk4Puppi.m_pt");

  TH1F *hist1 = new TH1F("hist1",";DeepCSV",88,-2,1.2);
  TTreeAna1->Project("hist1","DeepCSV","jetPt>100");
  //  hist1 ->SetMarkerColor(kBlack);
  hist1 ->SetLineColor(kBlack);
  // hist1 ->SetMarkerSize(1.1);
  // hist1 ->SetMarkerStyle(20);

  //file2, hist 2
  TString path2 = "/nfs/dust/cms/user/karavdia/CMSSW_10_2_11/src/UHH2/core/python/";
  TString name2 = "Ntuple_tthad2018_redoBslimmed_10GeV.root";
  TString gl_label2 = "DeepCSV, jet pt>100GeV (redone)";

  TFile *input2 = TFile::Open(path2+name2);
  TTree *TTreeAna2 = (TTree*)input2->Get("AnalysisTree");
  TTreeAna2->SetAlias("DeepCSV", "jetsAk4Puppi.m_btag_DeepCSV_probb+jetsAk4Puppi.m_btag_DeepCSV_probbb");
  TTreeAna2->SetAlias("jetPt", "jetsAk4Puppi.m_pt");

  TH1F *hist2 = new TH1F("hist2",";DeepCSV",88,-2,1.2);
  TTreeAna2->Project("hist2","DeepCSV","jetPt>100");
  //  hist2 ->SetMarkerColor(kRed);
  hist2 ->SetLineColor(kRed);
  hist2 ->SetLineWidth(3);
  // hist2 ->SetMarkerSize(1.1);
  // hist2 ->SetMarkerStyle(21);

  //  hist2 ->GetYaxis()->SetRangeUser(0, 4500);
  hist2 ->GetYaxis()->SetRangeUser(0, 1000);

  
  TCanvas * c1 = new TCanvas("cplot", "c", w, h);
  TLegend *leg = new TLegend(0.17,0.78,0.88,0.88);
  leg->SetHeader();
  leg->AddEntry(hist1,gl_label1,"lep");
  leg->AddEntry(hist2,gl_label2,"lep");
  leg->SetTextSize(0.04);
  hist2 ->Draw("");
  hist1 ->Draw("same");
  leg->Draw();
  c1->SaveAs("BtaggersTest_100GeV.pdf");
  c1->SaveAs("BtaggersTest_100GeV.png");

}
