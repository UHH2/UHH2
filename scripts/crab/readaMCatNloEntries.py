#!/usr/bin/env python
# -*- coding: utf-8 -*-

# usage of script: number of cores, first file, second file, ..., method to use True==Fast, False==counting weights
# python readaMCatNLO.py 4 ../../common/datasets/MC_WJet.xml True/False



import sys, multiprocessing, time, os
import subprocess
import xml.etree.ElementTree as ET
from CommentedTreeBuilder import CommentedTreeBuilder
import ROOT
from ROOT import *

ROOT.PyConfig.IgnoreCommandLineOptions = True
ROOT.gROOT.SetBatch(1)


def get_xml_tree(xmlFileDir):
    with open(xmlFileDir) as f:
        # hack to trick XML parsing - put under a root node
        xml_str = "<data>%s</data>" % f.read()
    parser = ET.XMLParser(target=CommentedTreeBuilder())
    root = ET.fromstring(xml_str, parser)
    return root


def get_root_filenames_from_xml(xmlFileDir):
    # skips commented out lines, no matter if it has FileName="..."
    rootFileStore = []
    root = get_xml_tree(xmlFileDir)
    for child in root:
        if child.tag != "In":
            # this will also skip commented out lines
            continue
        filename = child.attrib['FileName']
        if ".root" in filename:
            rootFileStore.append(filename)
    return rootFileStore


def read_tree(rootDir):
    numberOfweightedEntries = 0
    try:
        # Use C++ script to count as significantly faster
        cmd = "root -q -b -l 'countNumberEvents.C+(\""+rootDir+"\",false)'"
        output = subprocess.check_output(cmd, shell=True, stderr=subprocess.STDOUT)
        # now have to be careful - ROOT will return 0 even if there's an error
        if "error:" in output.lower():
            raise RuntimeError("Error running ROOT: " + output)
    except Exception as e:
        print 'unable to count events in root file',rootDir
        print e
    numberOfweightedEntries = float(output.splitlines()[-1])
    # return tuple of (filename, num events), since the result gets used in
    # multiprocessing, and we can't guarantee order, so keep track of filename
    return (rootDir, numberOfweightedEntries)


def read_treeFast(rootDir):
    fastentries = 0
    try:
        ntuple = TFile(str(rootDir))
        AnalysisTree = ntuple.Get("AnalysisTree")
        fastentries =  AnalysisTree.GetEntriesFast()
    except Exception as e:
        print 'unable to count events in root file',rootDir
        print e
    # return tuple of (filename, num events), since the result gets used in
    # multiprocessing, and we can't guarantee order, so keep track of filename
    return (rootDir, fastentries)


def readEntries(worker, xmlfiles, fast=False):
    if fast: print 'Going to use the Fast Method, no weights used'
    else: print 'Going to use the Weight Method: countNumberEvents.C'
    print "number of workers",worker
    result_list = []
    for xml in xmlfiles:
        pool = multiprocessing.Pool(processes=int(worker))
        print "open XML file:", xml
        rootFileStore = get_root_filenames_from_xml(xml)
        numberXMLFiles = len(rootFileStore)
        result = None
        if(fast):
            result = pool.map_async(read_treeFast,rootFileStore)
        else:
            result = pool.map_async(read_tree,rootFileStore)

        print result._number_left, numberXMLFiles, result._chunksize
        while result._number_left>0:
            sys.stdout.write("\033[F")
            missing = round(float(result._number_left)*float(result._chunksize)/float(numberXMLFiles)*100)
            if (missing > 100):
                missing = 100
            print "Missing [%]", missing
            time.sleep(10)
        pool.close()
        pool.join()
        entries_per_rootfile = [r for r in result.get()]
        total_entries = sum(x[1] for x in entries_per_rootfile)
        # convert list of tuples to dict for easier lookup by filename
        entries_per_rootfile = dict(entries_per_rootfile)
        print "number of events in", xml, total_entries
        result_list.append(total_entries)
        updateXMLfile(xml, entries_per_rootfile, fast)
    return result_list


def updateXMLfile(xmlfile, entries_per_rootfile, fast=False):
    """Edit the XML file to add total, and comment out files with 0 events

    entries_per_rootfile is a dict of {filename : number of events}

    This is because sframe can crash if the first file has 0 events.
    (For some reason CRAB makes these empty files)

    Note that we need != 0 as some files have -ve entries due to weights
    (we assume the user knows how to handle that scenario)
    """
    newText = []
    root = get_xml_tree(xmlfile)
    with open(xmlfile, "w") as outputfile:
        for child in root:
            if child.tag == "In":
                filename = child.attrib['FileName']
                if entries_per_rootfile[filename] != 0:
                    outputfile.write(ET.tostring(child))
                else:
                    # is empty file
                    outputfile.write("<!--EMPTY %s -->\n" % ET.tostring(child).strip())
            else:
                # is a comment
                outputfile.write(ET.tostring(child))

        method = 'fast' if fast else 'weights'
        outputfile.write('<!-- < NumberEntries="'+str(sum(entries_per_rootfile.values()))+'" Method='+method+' /> -->')


if __name__ == "__main__":
    xmllist = sys.argv[2:-1]
    meth = eval(sys.argv[-1])
    readEntries(sys.argv[1],xmllist,meth)
