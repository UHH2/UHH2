#!/usr/bin/env python

from CRABAPI.RawCommand import crabCommand
from CRABClient.ClientExceptions import ClientException
from httplib import HTTPException
from CRABClient.UserUtilities import config

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
        if(len(listOfNames)==len(listOfDatasets)):
            for i in range(0,len(listOfDatasets)):
                print "Working on", listOfNames[i]+namePostfix
                self.config.Data.inputDataset = listOfDatasets[i]
                self.config.General.requestName = listOfNames[i]+namePostfix
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
                
