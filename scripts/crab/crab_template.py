# This is a small example how the crab api can easily be used to create something like multi crab.
# It has some additional features like also creating the xml files for you. 
# For it to work you need inputDatasets & requestNames apart from the classical part 
#
# Make sure to have a unique directory where your joboutput is saved, otherwise the script gets confused and you too!!
#
# Usage ./CrabConfig ConfigFile [options]
#
# Take care here to make the request names *nice*
# 
# autocomplete_Datasets(ListOfDatasets) works also for several entries with *
#
from DasQuery import autocomplete_Datasets

inputDatasets = ['/DYJetsToLL_M-50_HT-*to*_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_*/MINIAODSIM']
inputDatasets = autocomplete_Datasets(inputDatasets)
requestNames = []
for x in inputDatasets:
    name = x.split('/')[1]
    modified_name =name.replace('_TuneCUETP8M1_13TeV-madgraphMLM-pythia8','')
    if 'ext1' in x:
        modified_name += '_ext1'
    elif 'ext2' in x:
        modified_name += '_ext2'
    elif 'ext' in x:
        modified_name += '_ext'
    requestNames.append(modified_name)


# ===============================================================================
# Classical part of crab, after resolving the * it uses in the example below just the first entry
#

from CRABClient.UserUtilities import config, getUsernameFromSiteDB
import os

config = config()
config.General.workArea = 'crab_Test'
config.General.transferOutputs = True
config.General.transferLogs = True
        
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = os.path.join(os.environ['CMSSW_BASE'], 'src/UHH2/core/python/ntuplewriter_mc_2016v2.py')
config.JobType.outputFiles = ["Ntuple.root"]
config.JobType.maxMemoryMB = 2500
# If you are using the L1 prefiring map, please modify the
# L1Maps location in ntuple_generator to just "L1PrefiringMaps_new.root"
# until this is properly handled
# i.e.:
# process.prefiringweight.L1Maps = cms.string("L1PrefiringMaps_new.root")
config.JobType.inputFiles = [os.path.join(os.environ['CMSSW_BASE'], 'src/L1Prefiring/EventWeightProducer/files/L1PrefiringMaps_new.root')]
        
config.Data.inputDBS = 'global'
config.Data.splitting = 'EventAwareLumiBased'
config.Data.unitsPerJob = 7500
config.Data.outLFNDirBase = '/store/user/%s/RunII_80X_v3/' % (getUsernameFromSiteDB())
config.Data.publication = False
config.JobType.sendExternalFolder = True 
#config.Data.allowNonValidInputDataset = True
#config.Data.publishDataName = 'CRAB3_tutorial_May2015_MC_analysis'

config.Site.storageSite = 'T2_DE_DESY'

config.General.requestName = requestNames[0]
config.Data.inputDataset = inputDatasets[0]


