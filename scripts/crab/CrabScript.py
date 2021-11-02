#!/usr/bin/env python

from CRABAPI.RawCommand import crabCommand
from CRABClient.ClientExceptions import ClientException
from httplib import HTTPException
from CRABClient.UserUtilities import config
from CrabYearUtilities import get_ntuplewriter,get_outLFNDirBase
import os

from multiprocessing import Process

class CrabConfig:
    def __init__(self, config, command='submit', options=''):
        self.config = config
        self.options = options
        self.command = command

        self.storeJetConstituents = False
        self.DefaultPsetName = True
        self.DefaultOutLFNDirBase = True
        self.outLFNDirBasePrefix = None
        #getting values for UHH2 specific from crab config fields
        if(hasattr(self.config,'UHH2')):
            if(hasattr(self.config.UHH2,'storeJetConstituents')):
                self.storeJetConstituents = self.config.UHH2.storeJetConstituents
            if(hasattr(self.config.UHH2,'DefaultPsetName')):
                self.DefaultPsetName = self.config.UHH2.DefaultPsetName
            if(hasattr(self.config.UHH2,'DefaultOutLFNDirBase')):
                self.DefaultOutLFNDirBase = self.config.UHH2.DefaultOutLFNDirBase
            if(hasattr(self.config.UHH2,'outLFNDirBasePrefix')):
                self.outLFNDirBasePrefix = self.config.UHH2.outLFNDirBasePrefix
            #delete complete UHH2 specific ConfigSection, since crab will check against internal ConfigMapping for mispelled config attributes
            del self.config.UHH2

    def _submit_(self, myconfig):
        try:
            if 'submit' not in self.command or 'resubmit' in self.command:
                crabCommand(self.command, myconfig.General.workArea+'/crab_'+myconfig.General.requestName,*self.options)
            else:
                if self.options:
                    crabCommand(self.command, *self.options, config = myconfig)
                else:
                    crabCommand(self.command,config = myconfig)
        except HTTPException as hte:
            print "Failed submitting task: %s" % (hte.headers)
        except ClientException as cle:
            print "Failed submitting task: %s" % (cle)

    def ByDatasets(self,listOfDatasets, listOfNames, namePostfix):
        #print "DataSets", listOfDatasets,"Request Name", listOfNames, "Postfix",namePostfix
        
        if(len(listOfNames)==len(listOfDatasets)):
            for i in range(0,len(listOfDatasets)):
                print "Working on", listOfNames[i]+namePostfix
                self.config.Data.inputDataset = listOfDatasets[i]
                self.config.General.requestName = listOfNames[i]+namePostfix


                #if user wants to provied psetname/outLFNDirBase themself, don't overwrite it here
                if(self.DefaultPsetName):
                    self.config.JobType.psetName = os.path.join(os.environ['CMSSW_BASE'], 'src/UHH2/core/python/', get_ntuplewriter(listOfDatasets[i],self.storeJetConstituents))
                if(self.DefaultOutLFNDirBase):
                    if(self.outLFNDirBasePrefix is not None):
                        self.config.Data.outLFNDirBase = get_outLFNDirBase(listOfDatasets[i],prefix=self.outLFNDirBasePrefix)
                    else:
                        self.config.Data.outLFNDirBase = get_outLFNDirBase(listOfDatasets[i])

                #print "Working on DataSet", listOfDatasets[i],"Request Name", listOfNames[i]+namePostfix
		#self._submit_(self.config)
		p = Process(target=self._submit_,args=(self.config,))
                try:
                    p.start()
                    p.join()
                except Exeption as e:
                    print e
        else:
            print "Number of Datasets & Names are unequal"
                
