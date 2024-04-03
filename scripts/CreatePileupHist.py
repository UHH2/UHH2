#!/usr/bin/env python

import os

"""
Script to create pileup profiles.
Mind that it requires the csv file from brilcalc!
"""

def makePilupMC(year, pileup, outpath):
    os.system("python makeMCPileupHist.py "+pileup+" --outputFilename "+outpath+"MyMCPileupHistogram_"+year+".root")

def makePilupData(year, json, pileup, outpath):
    # Taken from https://hypernews.cern.ch/HyperNews/CMS/get/luminosity/613/2/1/1/1.html
    # - Minimum Bias X-section 69.2 +- 4.6% microbarn
    os.system("pileupCalc.py -i "+json+" --inputLumiJSON "+pileup+" --calcMode true --minBiasXsec 69200 --maxPileupBin 100 --numPileupBins 100  "+outpath+"/MyDataPileupHistogram_"+year+".root")
    os.system("pileupCalc.py -i "+json+" --inputLumiJSON "+pileup+" --calcMode true --minBiasXsec 66017 --maxPileupBin 100 --numPileupBins 100  "+outpath+"/MyDataPileupHistogram_"+year+"_66017.root")
    os.system("pileupCalc.py -i "+json+" --inputLumiJSON "+pileup+" --calcMode true --minBiasXsec 72383 --maxPileupBin 100 --numPileupBins 100  "+outpath+"/MyDataPileupHistogram_"+year+"_72383.root")

def makeLumiHist(infile, outfile):
    os.system("python myLumiCalc.py outfilename="+outfile.replace(".root","_normtag.root")+" infilename="+infile)

if __name__ == "__main__":

    jsonPath = "/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/"
    jsonPathRun3 = "/nfs/dust/cms/user/paaschal/WorkingArea/JERCProtoLab/macros/lumi_PU/InputFiles/"
    normTagPath = "/cvmfs/cms-bril.cern.ch/cms-lumi-pog/Normtags/"
    pileupFile = "/13TeV/PileUp/pileup_latest.txt"

    outputPath = os.environ["CMSSW_BASE"]+"/src/UHH2/common/UHH2-data/"

    UL16runSplit = 278802
    runSplit2023 = 369803

    info = {
        "2022":         {"JSONpath":    jsonPathRun3+"Run3/",
                         "JSON":        "Cert_Collisions2022_355794_362180_Golden_allruns.json",
                         "normtag":     normTagPath+"normtag_BRIL.json", # correct ?
                         "Pileup":      jsonPathRun3+"pileup_latest_2022.txt",
                         "PileupMC":    "SimGeneral.MixingModule.Run3_2022_LHC_Simulation_10h_2h_cfi",
                         "HN":          "https://hypernews.cern.ch/HyperNews/CMS/get/physics-validation/3526.html ", # wrong!
                        },
        "2023preBPix":  {"JSONpath":    "/nfs/dust/cms/user/paaschal/UHH2_DiJet/CMSSW_10_6_28/src/UHH2/scripts/",
                         "JSON":        "Cert_Collisions2023_366442_370790_Golden.json",
                         "normtag":     normTagPath+"normtag_BRIL.json", # correct ?
                         "Pileup":      jsonPathRun3+"pileup_latest_2023.txt",
                         "PileupMC":    "SimGeneral.MixingModule.mix_2023_25ns_EraC_PoissonOOTPU_cfi",
                         "HN":          "https://hypernews.cern.ch/HyperNews/CMS/get/physics-validation/3526.html ", # wrong!
                        },
        "2023postBPix": {"JSONpath":    "/nfs/dust/cms/user/paaschal/UHH2_DiJet/CMSSW_10_6_28/src/UHH2/common/UHH2-data/2023/",
                         "JSON":        "Cert_Collisions2023_eraD_369803_370790_Golden.json",
                         "normtag":     normTagPath+"normtag_BRIL.json", # correct ?
                         "Pileup":      jsonPathRun3+"pileup_latest_2023.txt",
                         "PileupMC":    "SimGeneral.MixingModule.mix_2023_25ns_EraD_PoissonOOTPU_cfi",
                         "HN":          "https://hypernews.cern.ch/HyperNews/CMS/get/physics-validation/3526.html ", # wrong!
                        },
    }

    for year in info:
        outdir = outputPath+year+"/"
        json = info[year]["JSONpath"]+info[year]["JSON"]
        os.system("cp "+json+" "+outdir)
        makeLumiHist(outdir+"TempOut"+year+".csv", outdir+info[year]["JSON"].replace("txt","root").replace(".json",".root"))
        makePilupMC(year, info[year]["PileupMC"], outdir)
        # makePilupData(year, json, info[year]["Pileup"], outdir)
