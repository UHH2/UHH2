#!/usr/bin/env python
# -*- coding: utf-8 -*-

# usage of script: number of cores, first file, second file, ..., method to use True==Fast, False==counting weights
# python readaMCatNLO.py 4 ../../common/datasets/MC_WJet.xml True/False



import sys, multiprocessing, time, os
from ROOT import *

def read_xml(xmlFileDir):
    xmlFile = open(str(xmlFileDir))
    rootFileStore = []
    comment = False                      # Think this code through via this example:
    for line in xmlFile:                 #
        if '<!--' in line:               # asdfasdfasdfasdfasdf
            if '-->' in line:            # asdfasdfasdfasdfasdf
                continue                 # <!-- asfdasdfasdfasdfasdf -->
            else:                        # asdfasdfasdfasdfasdf
                comment = True           # <!-- asdfasdfasfdasdfasdf
                continue                 # asdfasdfasdfasdfasdf
        if comment == True:              # asdfasdfasdfasdfasdf -->
            if not '-->' in line:        # asdfasdfasdfasdfasdf
                continue                 # 
            else:                        # Not waterproof if comment starts mid-line (or violates this scheme in another
                comment = False          # way) but mid-line comments should not exist in these files in the first place
                continue
        if comment == False:
            rootFileStore.append(line.split('"')[1])
    return rootFileStore

def write_xml_entry_tag(xmlFile,result,fast):
    xmlFile = open(str(xmlFile),'a')
    method = 'weights'
    if(fast):method = 'fast'
    xmlFile.write('<!-- < NumberEntries="'+str(result)+'" Method='+method+' /> -->')

def read_tree(rootDir):
    numberOfweightedEntries = 0 
    try:
        ntuple = TFile(str(rootDir))
        AnalysisTree = ntuple.Get("AnalysisTree")
        for event in AnalysisTree:
            numberOfweightedEntries+=event.m_weights[0]
            #for value in event.m_weights:
            #    numberOfweightedEntries += value
    except Exception as e:
        print 'unable to count events in root file',rootDir
        print e
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
    print "number of workers",worker
    sum_list =[]
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
            #print result._number_left ,numberXMLFiles,result._chunksize
            missing = round(float(result._number_left)*float(result._chunksize)/float(numberXMLFiles)*100)
            if(missing > 100):
                missing =100
            print "Missing [%]", missing
            time.sleep(10)
        pool.close()
        pool.join()
        xml_result = sum(result.get())
        print "number of events in",xml,xml_result
        sum_list.append([r for r in result.get()])
        write_xml_entry_tag(xml,xml_result, fast)
    return sum_list

def commentOutEmptyRootFiles(xmlfile, entries_per_rootfile):
    root_files = read_xml(xmlfile)
    if len(root_files) != len(entries_per_rootfile): print "Something's really wrong."
    empty_root_files = []
    for i in range(len(root_files)):
        if entries_per_rootfile[i] == 0:
            empty_root_files.append(root_files[i])
    xml = open(str(xmlfile))
    temp_filename = str(xmlfile)+".tmp"
    xml_temporary = open(temp_filename,"w+")
    for line in xml:
        newline = line
        for rf in empty_root_files:
            if str(rf) in line:
                newline = "<!--EMPTY "+line.strip("\n")+"-->\n"
        xml_temporary.write(newline)
    xml.close()
    xml_temporary.close()
    os.rename(temp_filename,xmlfile)
    return empty_root_files

if __name__ == "__main__":
    xmllist = sys.argv[2:-1]
    meth = eval(sys.argv[-1])
    sum_list = readEntries(sys.argv[1],xmllist,meth)

    print "Searching for NTuples which are empty as a consequence of a relatively common crab error which has been around for a long time now..."
    for i in range(len(xmllist)):
        empty_files = commentOutEmptyRootFiles(xmllist[i], sum_list[i])
        if len(empty_files) > 0:
            print str(xmllist[i])+":    Found and commented out empty root files:"
            for ef in empty_files: print str(ef)
        else: print "\033[0;40;32m"+"No empty root files detected. Yay!\033[0m" # green color
