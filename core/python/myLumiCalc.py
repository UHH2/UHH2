#!/usr/bin/env python

#usage:
#1. run lcr2.py:
#  cd /afs/cern.ch/user/m/marlow/public/lcr2
#  python lcr2.py -i /afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/13TeV/Cert_246908-251252_13TeV_PromptReco_Collisions15_JSON.txt --lumibyls -o /afs/desy.de/user/p/peiffer/xxl-af-cms/CMSSW_7_4_7/src/UHH2/core/python/TempOut.csv
#
# 2. run myLumiCalc.py:
#  cd /afs/desy.de/user/p/peiffer/xxl-af-cms/CMSSW_7_4_7/src/UHH2/core/python
#  ./myLumiCalc.py outfilename=Out.root infilename=TempOut.csv


import os,sys,time
import coral

from ROOT import gROOT, TFile, TTree, AddressOf, TString, std
from ctypes import *

gROOT.Reset()


gROOT.ProcessLine(\
       "struct MyStruct{\
       UInt_t runnr;\
       UInt_t luminosityBlock;\
       Double_t intgRecLumi;\
       Double_t HLTpresc;\
       Double_t L1presc;\
       };")
from ROOT import MyStruct



from FWCore.ParameterSet.VarParsing import VarParsing
options = VarParsing ('python')

options.register ('outfilename',
                  'OutFile.root',
                  VarParsing.multiplicity.singleton,
                  VarParsing.varType.string,
                  'name of output root file')

options.register ('infilename',
                  'TempOut.csv',
                  VarParsing.multiplicity.singleton,
                  VarParsing.varType.string,
                  'name of input file from lcr2.py')

options.parseArguments()

   
ofile = TFile(options.outfilename,"RECREATE")
tr = TTree("AnalysisTree","AnalysisTree")
    
s = MyStruct()

hltpath = TString("")
    

tr.Branch('run',AddressOf(s,'runnr'),'runnr/i')
tr.Branch('luminosityBlock',AddressOf(s,'luminosityBlock'),'luminosityBlock/i')
#tr.Branch("HLTpath","TString",hltpath)
#tr.Branch('L1bit','TString',l1bit)
tr.Branch('intgRecLumi',AddressOf(s,'intgRecLumi'),'intgRecLumi/D')   
#tr.Branch('HLTpresc',AddressOf(s,'HLTpresc'),'HLTpresc/D')
#tr.Branch('L1presc',AddressOf(s,'L1presc'),'L1presc/D')

infile = open("TempOut.csv", "r")

infile.readline()
for line in infile:
    #print line
    run = line[0:line.find(":")]
    s.runnr = int(run)
    line = line[len(run)+1:]
    fill = line[0:line.find(",")]
    line = line[len(fill)+1:]
    lb = line[0:line.find(":")]
    s.luminosityBlock = int(lb)
    line = line[2*len(lb)+2:]
    utctime = line[0:line.find(",")]
    line = line[len(utctime)+1:]
    status =  line[0:line.find(",")]
    line = line[len(status)+1:]
    energy = line[0:line.find(",")]
    line = line[len(energy)+1:]
    intLumiDeliv = line[0:line.find(",")]
    line = line[len(intLumiDeliv)+1:]
    intLumi = line[0:line.find(",")]
    s.intgRecLumi = float(intLumi)
    line = line[len(intLumi)+1:]
    
    tr.Fill()

    
ofile.Write()
ofile.Close()  


    


