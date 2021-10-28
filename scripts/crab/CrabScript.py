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
        
        #setting default values for UHH2 specific crab config fields, if User has not set them earlier 
        if(not hasattr(self.config.General,'storeJetConstituents')):
            self.config.General.storeJetConstituents = False
        if(not hasattr(self.config.JobType,'DefaultPsetName')):
            self.config.JobType.DefaultPsetName = True
        if(not hasattr(self.config.Data,'DefaultOutLFNDirBase')):
            self.config.Data.DefaultOutLFNDirBase = True

        if(len(listOfNames)==len(listOfDatasets)):
            for i in range(0,len(listOfDatasets)):
                print "Working on", listOfNames[i]+namePostfix
                self.config.Data.inputDataset = listOfDatasets[i]
                self.config.General.requestName = listOfNames[i]+namePostfix


                #if user wants to provied psetname/outLFNDirBase themself, don't overwrite it here
                if(self.config.JobType.DefaultPsetName):
                    self.config.JobType.psetName = os.path.join(os.environ['CMSSW_BASE'], 'src/UHH2/core/python/', get_ntuplewriter(listOfDatasets[i],self.config.General.storeJetConstituents))
                if(self.config.Data.DefaultOutLFNDirBase):
                    if(hasattr(self.config.Data,'outLFNDirBasePrefix')):
                        self.config.Data.outLFNDirBase = get_outLFNDirBase(listOfDatasets[i],prefix=self.config.Data.outLFNDirBasePrefix)
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
                
