#!/usr/bin/env python

"""
Produce comparison plots for all variable in all branches on two ROOT files
with same tree name. Each branch will be in its own subdirectory.
Histograms with differing # entries or means will have "DIFF_" prepended to their filename.
"""


import os
import ROOT
import sys
import argparse
from collections import OrderedDict


ROOT.PyConfig.IgnoreCommandLineOptions = True
ROOT.gROOT.SetBatch(1)
ROOT.TH1.SetDefaultSumw2()
# ROOT.gErrorIgnoreLevel = ROOT.kError
ROOT.gErrorIgnoreLevel = ROOT.kBreak  # to turn off all Error in <TCanvas::Range> etc


def do_comparison_plot(T1, T2, name, output_name, print_warning=True):
    c = ROOT.TCanvas("c"+name, "", 800, 600)

    h1name = "h1_%s" % name
    T1.Draw(name + ">>" + h1name)
    h1 = ROOT.TH1F(ROOT.gROOT.FindObject(h1name))
    h1_colour = ROOT.kBlack
    h1.SetLineColor(h1_colour)
    h1.SetLineWidth(2)
    h1.Draw("HISTE")

    h2name = "h2_%s" % name
    T2.Draw(name + ">>" + h2name)
    h2 = ROOT.TH1F(ROOT.gROOT.FindObject(h2name))
    h2_colour = ROOT.kRed
    h2.SetLineColor(h2_colour)
    h2.SetLineStyle(2)
    h2.SetLineWidth(2)
    h2.Draw("HISTE")
    c.Update()
    # To get the 2nd stats box we MUST NOT draw with SAME
    # Instead draw by itself, then plot them together afterwards
    stats2 = h2.GetListOfFunctions().FindObject("stats").Clone("stats1")
    stats2.SetY1NDC(0.52);
    stats2.SetY2NDC(0.7);
    stats2.SetTextColor(h2_colour);

    if h1.GetEntries() == 0 and h2.GetEntries() == 0:
        return

    # Do a simple check to see if hists differ
    is_diff = False
    if print_warning and (h1.GetEntries() > 0 or h2.GetEntries() > 0):
        if h1.GetEntries() != h2.GetEntries():
            is_diff = True
            print name, " has differing entries", h1.GetEntries(), "vs", h2.GetEntries()
        if h1.GetMean() != h2.GetMean():
            is_diff = True
            print name, " has differing means", h1.GetMean(), "vs", h2.GetMean()

    if is_diff:
        basename = os.path.basename(output_name)
        output_name = output_name.replace(basename, "DIFF_"+basename)

    h1.Draw("HISTE")
    h2.Draw("HISTE SAME")
    stats2.Draw()
    c.Modified()
    c.SaveAs(output_name)

    return is_diff


if __name__ == "__main__":

    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("filename1", help="Reference ROOT filename")
    parser.add_argument("filename2", help="Comparison ROOT filename")
    default_dir = "ComparisonPlots"
    parser.add_argument("--outputDir", help="Output directory for plots, defaults to %s" % default_dir, default=default_dir)
    default_tree = "AnalysisTree"
    parser.add_argument("--treeName", help="Name of TTree, defaults to %s" % default_tree, default=default_tree)
    args = parser.parse_args()

    f1 = ROOT.TFile.Open(args.filename1)
    f2 = ROOT.TFile.Open(args.filename2)

    T1 = f1.Get(args.treeName)
    T2 = f2.Get(args.treeName)

    br_list1 = T1.GetListOfBranches()
    # br_list2 = T2.GetListOfBranches()

    results = OrderedDict()

    print "Plots produced in", args.outputDir
    ROOT.gSystem.mkdir(args.outputDir)

    for i in range(br_list1.GetEntries()):
    # for i in range(0, 20):
        branch1 = br_list1.At(i)
        branch_name = branch1.GetName()
        print "BRANCH", i, ":", branch_name
        branch2 = T2.GetBranch(branch_name)
        if branch2 is None:
            print "WARNING Tree2 doesn't have branch ", branch_name

        ROOT.gSystem.mkdir(os.path.join(args.outputDir, branch_name))

        leaves1 = branch1.GetListOfBranches()
        leaves2 = branch2.GetListOfBranches()

        if leaves1.GetEntries() == 0:
            # means its a leaf not a branch
            output_name = os.path.join(args.outputDir, branch_name, branch_name+"_compare.pdf")
            is_diff = do_comparison_plot(T1, T2, branch_name, output_name)
            results[branch_name] = is_diff
        else:
            for j in range(leaves1.GetEntries()):
                leaf1 = leaves1.At(j)
                leaf2 = leaves2.At(j)

                leaf_type = leaf1.GetTypeName()
                if leaf_type.startswith("map"):
                    # don't know how to print these yet
                    continue

                var_name = leaf1.GetName()
                if var_name != leaf2.GetName():
                    print "WARNING diff leaf names", var_name, "vs", leaf2.GetName()

                print var_name
                parts = var_name.replace(branch_name+".", "").replace(".", "_")
                output_name = os.path.join(args.outputDir, branch_name, parts + "_compare.pdf")
                is_diff = do_comparison_plot(T1, T2, var_name, output_name)
                results[var_name] = is_diff

    f1.Close()
    f2.Close()

    # Print results
    if any(results.values()):
        max_len = max([len(x) for x in results.keys()])
        print "*" * max_len
        print "Differing vars:"
        print "*" * max_len
        for name, result in results.iteritems():
            if result:
                print name
        print "*" * max_len
        sys.exit(1)
    else:
        print "All distributions same"
        sys.exit(0)
