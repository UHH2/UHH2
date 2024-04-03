import ROOT
from ROOT import TFile, TCanvas, TH1F, TLegend, gStyle
import matplotlib.pyplot as plt
import numpy as np

# Open the ROOT files
year = "2023preBPix"
# path = f"/nfs/dust/cms/user/paaschal/UHH2_DiJet/CMSSW_10_6_28/src/UHH2/common/UHH2-data/{year}"
path = "."
data_file = TFile.Open('./MyDataPileupHistogram_2023preBPix.root')
mc_file = TFile.Open('./MyMCPileupHistogram_2023preBPix.root')

# Get the histograms
data_hist = data_file.Get('pileup')
data_hist.SetDirectory(0)
data_file.Close()
data_hist.Scale(1 / data_hist.Integral())
mc_hist = mc_file.Get('input_Event/N_TrueInteractions')
mc_hist.SetDirectory(0)
mc_file.Close()

print(data_hist.GetMaximum())
print(mc_hist.GetMaximum())

data_hist.SetMarkerStyle(20)  # Set marker style
data_hist.SetMarkerColor(1)  # Black color
data_hist.SetLineColor(1)  # Black color

mc_hist.SetFillColor(4)  # Blue fill color
mc_hist.SetLineColor(4)  # Blue line color
mc_hist.SetFillStyle(3000)  # Fill style

a = TCanvas('a', 'Pileup Profile', 800, 600)
a.Divide(1, 2)

# pad1 = ROOT.TPad("pad1", "pad1", 0.31, 0.05, 1, 1)
pad1 = a.cd(1)
pad1.SetPad(0.01, 0.3, 0.99, 0.99)
pad1.SetBottomMargin(0.03)  # Leave space for the ratio plot below
# ROOT.TPad("pad1", "pad1", 0.31, 0.05, 1, 1)
# pad1.SetTopMargin(0)
# pad1.SetBottomMargin(0.2)
# pad1.Draw()

data_hist.Draw('E')
mc_hist.Draw('HIST SAME')
data_hist.Draw('E SAME')

legend = TLegend(0.7, 0.7, 0.9, 0.9)
legend.AddEntry(data_hist, "Data", "lep")
legend.AddEntry(mc_hist, "MC", "f")
legend.Draw()

pad2 = a.cd(2)
pad2.SetPad(0.01, 0.01, 0.99, 0.29)
pad2.SetTopMargin(0.05)
pad2.SetBottomMargin(0.3)
# pad2 = ROOT.TPad("pad2", "pad2", 0, 0.05, 1, 0.3)
# pad2.SetTopMargin(0)
# pad2.SetBottomMargin(0.2)
# pad2.Draw()

ratio_hist = data_hist.Clone("ratio_hist")
ratio_hist.SetLineColor(ROOT.kBlack)
ratio_hist.SetMinimum(0)  # Optional: Set the minimum of the ratio plot
ratio_hist.SetMaximum(4)  # Optional: Set the maximum of the ratio plot
ratio_hist.Sumw2()
ratio_hist.GetXaxis().SetLabelSize(0.1)
ratio_hist.GetYaxis().SetLabelSize(0.1)
ratio_hist.SetStats(0)  # No statistics on ratio plot
ratio_hist.Divide(mc_hist)  # Divide data by MC for the ratio
ratio_hist.Draw("ep")  # Draw the ratio plot

a.cd()

a.SaveAs("PileupProfile_2023preBPix.png")
