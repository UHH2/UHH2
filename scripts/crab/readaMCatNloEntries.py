#!/usr/bin/env python
# -*- coding: utf-8 -*-

# usage of script: number of cores, first file, second file, ..., method to use True==Fast, False==counting weights
# python readaMCatNLO.py 4 ../../common/datasets/MC_WJet.xml True/False



import sys, multiprocessing, time, os
import subprocess
import ROOT
from ROOT import *

ROOT.PyConfig.IgnoreCommandLineOptions = True
ROOT.gROOT.SetBatch(1)


def read_xml(xmlFileDir):
    xmlFile = open(str(xmlFileDir))
    rootFileStore = []
    comment = False
    for line in xmlFile:
        if ".root" in line:
            rootFileStore.append(line.split('"')[1])
    return rootFileStore

def read_tree(rootDir):
    numberOfweightedEntries = 0
    try:
        # Use C++ script to count as significantly faster (~15x)
        cmd = "root -q -b -l 'countNumberEvents.C+(\""+rootDir+"\",false)'"
        output = subprocess.check_output(cmd, shell=True, stderr=subprocess.STDOUT)
        # now have to be careful - root will return 0 even if there's an error
        if "error:" in output.lower():
            raise RuntimeError("Error running ROOT: " + output)
    except Exception as e:
        print 'unable to count events in root file',rootDir
        print e
    numberOfweightedEntries = float(output.splitlines()[-1])
    return numberOfweightedEntries

def read_treeFast(rootDir):
    fastentries =0
    try:
        ntuple = TFile(str(rootDir))
        AnalysisTree = ntuple.Get("AnalysisTree")
        fastentries =  AnalysisTree.GetEntriesFast()
    except Exception as e:
        print 'unable to count events in root file',rootDir
        print e
    return fastentries

def readEntries(worker, xmlfiles, fast=False):
    if fast: print 'Going to use the Fast Method, no weights used'
    else: print 'Going to use the Weight Method: countNumberEvents.C'
    print "number of workers",worker
    result_list = []
    for xml in xmlfiles:
        pool = multiprocessing.Pool(processes=int(worker))
        print "open XML file:",xml
        rootFileStore = read_xml(xml)
        numberXMLFiles = len(rootFileStore)
        result = None
        if(fast):
            result = pool.map_async(read_treeFast,rootFileStore)
        else:
            result = pool.map_async(read_tree,rootFileStore)

        print result._number_left ,numberXMLFiles,result._chunksize
        while result._number_left>0:
            sys.stdout.write("\033[F")
            missing = round(float(result._number_left)*float(result._chunksize)/float(numberXMLFiles)*100)
            if(missing > 100):
                missing =100
            print "Missing [%]", missing
            time.sleep(10)
        pool.close()
        pool.join()
        entries_per_rootfile = [r for r in result.get()]
        print "number of events in",xml,sum(entries_per_rootfile)
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
    with open(xmlfile, "U") as file:
        i_ = 0
        for line in file.readlines():
            if '.root' in line:
                newText.append(line if entries_per_rootfile[i_]!=0 else '<!--EMPTY <In FileName="'+line.split('"')[1]+'" Lumi="0.0"/> -->\n')
                i_ += 1
        if len(entries_per_rootfile)!= i_: print "ERROR", len(entries_per_rootfile), i_
    with open(xmlfile, "w") as outputfile:
        for line in newText:
            outputfile.write(line)
        method = 'fast' if fast else 'weights'
        outputfile.write('<!-- < NumberEntries="'+str(sum(entries_per_rootfile))+'" Method='+method+' /> -->')


if __name__ == "__main__":
    xmllist = sys.argv[2:-1]
    meth = eval(sys.argv[-1])
    readEntries(sys.argv[1],xmllist,meth)
