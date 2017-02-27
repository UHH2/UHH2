#!/usr/bin/env python
# -*- coding: utf-8 -*-

# usage of script: number of cores, first file, second file, ..., method to use True==Fast, False==counting weights
# python readaMCatNLO.py 4 ../../common/datasets/MC_WJet.xml True/False



import sys, multiprocessing, time
from ROOT import *

def read_xml(xmlFileDir):
    #try:
    xmlFile = open(str(xmlFileDir))
    rootFileStore = []
    comment =False
    for line in xmlFile:
        if '<!--' in line and not '-->': 
            comment = True
            continue
        if '-->' in line: 
            comment=False
            continue
        rootFileStore.append(line.split('"')[1])
    return rootFileStore
    #except:
    #    print "No able to read file Dir", xmlFileDir
    #    return 


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
            #numberOfweightedEntries+=event.m_weights[0]
            for value in event.m_weights:
                numberOfweightedEntries += value
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
        sum_list.append(xml_result)
        write_xml_entry_tag(xml,xml_result, fast)
    return sum_list
        

if __name__ == "__main__":
    xmllist = sys.argv[2:-1]
    meth = eval(sys.argv[-1])
    readEntries(sys.argv[1],xmllist,meth)
    
