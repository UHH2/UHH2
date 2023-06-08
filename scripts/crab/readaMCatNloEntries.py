#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# usage of script: number of cores, first file, second file, ..., method to use True==Fast, False==counting weights
# python readaMCatNLO.py 4 ../../common/datasets/MC_WJet.xml True/False



import sys, time, os
from multiprocessing.pool import ThreadPool
import subprocess, multiprocessing
import ROOT
from tqdm import tqdm

ROOT.PyConfig.IgnoreCommandLineOptions = True
ROOT.gROOT.SetBatch(1)

def OpenTFile(fname):
    ntuple = ROOT.TFile(str(fname))
    ntuple.Close()
    return True

def VerifyFile(fname,timeout=10):
    p = multiprocessing.Process(target=OpenTFile, args=(fname,))
    p.start()
    startTime = time.time()
    while (time.time() - startTime)<timeout:
        if not p.is_alive():
            return True
    if p.is_alive():
        p.terminate()
        p.join()
        return False
    return True

def read_xml(xmlFileDir):
    xmlFile = open(str(xmlFileDir))
    rootFileStore = []
    for line in xmlFile:
        if ".root" in line:
            rootFileStore.append(line.split('"')[1])
    return rootFileStore

def read_tree(rootDir, progress_bar=None):
    numberOfweightedEntries = 0
    try:
        if not VerifyFile(fname=str(rootDir)):
            raise RuntimeError("Impossible to open "+str(rootDir))
        # Use compiled root-macro to count as significantly faster (~15x)
        cmd = "./countNumberEvents "+rootDir+" false"
        output = subprocess.check_output(cmd, shell=True, stderr=subprocess.STDOUT)
        # now have to be careful - root will return 0 even if there's an error
        if any(["error" in x.lower() for x in output.splitlines()]):
            raise RuntimeError("Error running ROOT: " + output)
    except Exception as e:
        print('unable to count events in root file',rootDir)
        print(e)
        return numberOfweightedEntries
    w = output.splitlines()[-1]
    if not w.replace(".","").replace("-","").isdigit():
        print("Problem while trying to convert countNumberEvents output to float. Affected ntuple:", rootDir)
        return numberOfweightedEntries
    numberOfweightedEntries = float(w)
    if progress_bar: progress_bar.update(1)
    return numberOfweightedEntries

def read_treeFast(rootDir, progress_bar=None):
    fastentries =0
    try:
        if not VerifyFile(fname=str(rootDir)):
            raise RuntimeError("Impossible to open "+str(rootDir))
        ntuple = ROOT.TFile(str(rootDir))
        AnalysisTree = ntuple.Get("AnalysisTree")
        fastentries =  AnalysisTree.GetEntriesFast()
    except Exception as e:
        print('unable to count events in root file',rootDir)
        print(e)
    if progress_bar: progress_bar.update(1)
    return fastentries

def readEntries(worker, xmlfiles, fast=False):
    if fast: print('Going to use the Fast Method, no weights used')
    else:
        print('Going to use the Weight Method: countNumberEvents.C')
        # let `make` decide if recompilation is necessary
        command = "make -f countNumberEventsMakefile >& countNumberEvents_C_compilation_log.txt"
        process = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE)
        process.wait()

    print("Number of workers:",worker)
    result_list = []
    for xml in xmlfiles:
        pool = ThreadPool(processes=int(worker))
        print("Open XML file:",xml)
        rootFileStore = read_xml(xml)
        entries_per_rootfile = []
        progress_bar = tqdm(total=len(rootFileStore), desc='Ntuples counted', dynamic_ncols=True, leave=False)
        for rootFile in rootFileStore:
            entries_per_rootfile.append(pool.apply_async(func=(read_treeFast if fast else read_tree), args=(rootFile,progress_bar,)))
        pool.close()
        pool.join()
        progress_bar.close()
        entries_per_rootfile = [result.get() for result in entries_per_rootfile]
        print('Done.', ('Number of events:' if fast else 'Sum of event weights:'), sum(entries_per_rootfile))
        result_list.append(sum(entries_per_rootfile))
        commentOutEmptyRootFiles(xml, entries_per_rootfile,fast)
    return result_list


def commentOutEmptyRootFiles(xmlfile, entries_per_rootfile,fast=False):
    """Edit the XML file so that ROOT files with 0 events are commented out

    This is because sframe can crash if the first file has 0 events.
    (For some reason CRAB makes these empty files)

    Note that we need != 0 as some files have -ve entries due to weights
    (we assume the user knows how to handle that scenario)
    """
    newText = []
    with open(xmlfile, "r") as file_:
        i_ = 0
        for line in file_.readlines():
            if '.root' in line:
                line = '<In FileName="'+line.split('"')[1]+'" Lumi="0.0"/>'
                if entries_per_rootfile[i_] == 0:
                    line = '<!--EMPTY {} -->'.format(line)
                newText.append(line + "\n")
                i_ += 1
            elif line.startswith('<!-- < NumberEntries'):
                newText.append(line)
        if len(entries_per_rootfile)!= i_: print("ERROR", len(entries_per_rootfile), i_)
    with open(xmlfile, "w") as outputfile:
        for line in newText:
            outputfile.write(line)
        method = 'fast' if fast else 'weights'
        outputfile.write('<!-- < NumberEntries="'+str(sum(entries_per_rootfile))+'" Method='+method+' /> -->\n')


if __name__ == "__main__":
    xmllist = sys.argv[2:-1]
    meth = eval(sys.argv[-1])
    readEntries(sys.argv[1],xmllist,meth)
