#!/usr/bin/env python3

import os

"""
Script to create pileup profiles.
Mind that it requires the csv file from brilcalc!
"""

def makePilupMC(year, pileup, outpath):
    print("./makeMCPileupHist.py "+pileup+" --outputFilename "+outpath+"MyMCPileupHistogram_"+year+".root")
    # os.system("python3 makeMCPileupHist.py "+pileup+" --outputFilename "+outpath+"MyMCPileupHistogram_"+year+".root")

def makePilupData(year, json, pileup, outpath):
    # Taken from https://hypernews.cern.ch/HyperNews/CMS/get/luminosity/613/2/1/1/1.html
    # - Minimum Bias X-section 69.2 +- 4.6% microbarn
    print("pileupCalc.py -i "+json+" --inputLumiJSON "+pileup+" --calcMode true --minBiasXsec 69200 --maxPileupBin 100 --numPileupBins 100  "+outpath+"/MyDataPileupHistogram_"+year+".root")
    # os.system("pileupCalc.py -i "+json+" --inputLumiJSON "+pileup+" --calcMode true --minBiasXsec 69200 --maxPileupBin 100 --numPileupBins 100  "+outpath+"/MyDataPileupHistogram_"+year+".root")
    # os.system("pileupCalc.py -i "+json+" --inputLumiJSON "+pileup+" --calcMode true --minBiasXsec 66017 --maxPileupBin 100 --numPileupBins 100  "+outpath+"/MyDataPileupHistogram_"+year+"_66017.root")
    # os.system("pileupCalc.py -i "+json+" --inputLumiJSON "+pileup+" --calcMode true --minBiasXsec 72383 --maxPileupBin 100 --numPileupBins 100  "+outpath+"/MyDataPileupHistogram_"+year+"_72383.root")

def makeLumiHist(infile, outfile):
    print("python3 myLumiCalc.py outfilename="+outfile.replace(".root","_normtag.root")+" infilename="+infile)
    # os.system("python3 myLumiCalc.py outfilename="+outfile.replace(".root","_normtag.root")+" infilename="+infile)

if __name__ == "__main__":

    jsonPath = "/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/"
    jsonPathRun3 = "/nfs/dust/cms/user/paaschal/WorkingArea/JERCProtoLab/macros/lumi_PU/InputFiles/"
    jsonPath2023 = "/nfs/dust/cms/user/paaschal/UHH3_2023/CMSSW_13_0_13/src/UHH2/common/UHH2-data/"
    normTagPath = "/cvmfs/cms-bril.cern.ch/cms-lumi-pog/Normtags/"
    pileupFile = "/13TeV/PileUp/pileup_latest.txt"

    outputPath = os.environ["CMSSW_BASE"]+"/src/UHH2/common/UHH2-data/"

    UL16runSplit = 278802
    runSplit2023 = 369803

    info = {
        # "UL16":         {"JSONpath":    jsonPath+"Collisions16/13TeV/Legacy_2016/",
        #                  "JSON":        "Cert_271036-284044_13TeV_Legacy2016_Collisions16_JSON.txt",
        #                  "normtag":     normTagPath+"normtag_PUBLICPLOTS.json",
        #                  "Pileup":      jsonPath+"Collisions16"+pileupFile,
        #                  "PileupMC":    "SimGeneral.MixingModule.mix_2016_25ns_UltraLegacy_PoissonOOTPU_cfi",
        #                  "HN":          "https://hypernews.cern.ch/HyperNews/CMS/get/physics-validation/3577/1.html"
        #                 },
        # "UL17":         {"JSONpath":    jsonPath+"Collisions17/13TeV/Legacy_2017/",
        #                  "JSON":        "Cert_294927-306462_13TeV_UL2017_Collisions17_GoldenJSON.txt",
        #                  "normtag":     normTagPath+"normtag_PHYSICS.json",
        #                  "Pileup":      jsonPath+"Collisions17"+pileupFile,
        #                  "PileupMC":    "SimGeneral.MixingModule.mix_2017_25ns_UltraLegacy_PoissonOOTPU_cfi",
        #                  "HN":          "https://hypernews.cern.ch/HyperNews/CMS/get/physics-validation/3499.html ",
        #                 },
        # "UL18":         {"JSONpath":    jsonPath+"Collisions18/13TeV/Legacy_2018/",
        #                  "JSON":        "Cert_314472-325175_13TeV_Legacy2018_Collisions18_JSON.txt",
        #                  "normtag":     normTagPath+"normtag_BRIL.json",
        #                  "Pileup":      jsonPath+"Collisions18"+pileupFile,
        #                  "PileupMC":    "SimGeneral.MixingModule.mix_2018_25ns_UltraLegacy_PoissonOOTPU_cfi",
        #                  "HN":          "https://hypernews.cern.ch/HyperNews/CMS/get/physics-validation/3526.html ",
        #                 },
        # "2022":         {"JSONpath":    jsonPathRun3+"Run3/",
        #                  "JSON":        "Cert_Collisions2022_355794_362180_Golden_allruns.json",
        #                  "normtag":     normTagPath+"normtag_BRIL.json", # correct ? 
        #                  "Pileup":      jsonPathRun3+"pileup_latest_2022.txt",
        #                  "PileupMC":    "SimGeneral.MixingModule.Run3_2022_LHC_Simulation_10h_2h_cfi",
        #                  "HN":          "https://hypernews.cern.ch/HyperNews/CMS/get/physics-validation/3526.html ", # wrong!
        #                 },
        "2023preBPix":  {"JSONpath":    "/nfs/dust/cms/user/paaschal/UHH3_2023/CMSSW_13_0_13/src/UHH2/common/UHH2-data/2023preBPix/",
                         "JSON":        "Cert_Collisions2023_eraC_367095_368823_Golden.json",
                         "normtag":     normTagPath+"normtag_BRIL.json", # correct ? 
                         "Pileup":      jsonPath2023+"2023preBPix/pileup_JSON.txt",
                         "PileupMC":    "SimGeneral.MixingModule.mix_2023_25ns_EraC_PoissonOOTPU_cfi",
                         "HN":          "https://hypernews.cern.ch/HyperNews/CMS/get/physics-validation/3526.html ", # wrong!
                        },
        "2023postBPix": {"JSONpath":    "/nfs/dust/cms/user/paaschal/UHH3_2023/CMSSW_13_0_13/src/UHH2/common/UHH2-data/2023postBPix/",
                         "JSON":        "Cert_Collisions2023_eraD_369803_370790_Golden.json",
                         "normtag":     normTagPath+"normtag_BRIL.json", # correct ? 
                         "Pileup":      jsonPath2023+"2023postBPix/pileup_JSON.txt",
                         "PileupMC":    "SimGeneral.MixingModule.mix_2023_25ns_EraD_PoissonOOTPU_cfi",
                         "HN":          "https://hypernews.cern.ch/HyperNews/CMS/get/physics-validation/3526.html ", # wrong!
                        },
        
    }

    for year in info:
        print(year)
        outdir = outputPath+year+"/"
        json = info[year]["JSONpath"]+info[year]["JSON"]
        # Copy Json and split for UL16
        if not "UL16" in year:
            os.system("cp "+json+" "+outdir)
            makeLumiHist(outdir+"TempOut"+year+".csv", outdir+info[year]["JSON"].replace("txt","root").replace(".json",".root"))
        # else:
        #     os.system("cp "+json+" "+outdir.replace(year,year+"preVFP"))
        #     os.system("cp "+json+" "+outdir.replace(year,year+"postVFP"))
        #     os.system("filterJSON.py --max "+str(UL16runSplit-1)+" "+json+" --output "+outdir.replace(year,year+"preVFP")+info[year]["JSON"].replace(".txt","_"+year+"preVFP.txt"))
        #     os.system("filterJSON.py --min "+str(UL16runSplit)+" "+json+" --output "+outdir.replace(year,year+"postVFP")+info[year]["JSON"].replace(".txt","_"+year+"postVFP.txt"))
        #     makeLumiHist(outdir.replace(year,year+"preVFP")+"TempOut"+year+".csv", outdir.replace(year,year+"preVFP")+info[year]["JSON"].replace("txt","root"))
        #     makeLumiHist(outdir.replace(year,year+"postVFP")+"TempOut"+year+".csv", outdir.replace(year,year+"postVFP")+info[year]["JSON"].replace("txt","root"))
        #     makeLumiHist(outdir.replace(year,year+"preVFP")+"TempOut"+year+"preVFP.csv", outdir.replace(year,year+"preVFP")+info[year]["JSON"].replace(".txt","_"+year+"preVFP.root"))
        #     makeLumiHist(outdir.replace(year,year+"postVFP")+"TempOut"+year+"postVFP.csv", outdir.replace(year,year+"postVFP")+info[year]["JSON"].replace(".txt","_"+year+"postVFP.root"))

        # Create pileup profiles
        if not "UL16" in year:
            print("pass")
            makePilupMC(year, info[year]["PileupMC"], outdir)
            makePilupData(year, json, info[year]["Pileup"], outdir)
        # else:
        #     makePilupMC(year+"preVFP",  info[year]["PileupMC"], outdir.replace(year,year+"preVFP"))
        #     makePilupMC(year+"postVFP", info[year]["PileupMC"], outdir.replace(year,year+"postVFP"))
        #     makePilupData(year, json, info[year]["Pileup"], outdir.replace(year,year+"preVFP"))
        #     makePilupData(year, json, info[year]["Pileup"], outdir.replace(year,year+"postVFP"))
        #     makePilupData(year+"preVFP",  outdir.replace(year,year+"preVFP")+info[year]["JSON"].replace(".txt","_"+year+"preVFP.txt"),   info[year]["Pileup"], outdir.replace(year,year+"preVFP"))
        #     makePilupData(year+"postVFP", outdir.replace(year,year+"postVFP")+info[year]["JSON"].replace(".txt","_"+year+"postVFP.txt"), info[year]["Pileup"], outdir.replace(year,year+"postVFP"))
