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

inputDatasets = ['/JetHT/Run2017F-17Nov2017-v1/MINIAOD']
inputDatasets = autocomplete_Datasets(inputDatasets)
requestNames = []
for x in inputDatasets:
    name = x.split('/')[1]
    modified_name =name.replace('TuneCUETP8M1_13TeV-madgraphMLM-pythia8','25ns')
    if "ext1" in x:
        modified_name += '_ext1'
    if "ext2" in x:
        modified_name += '_ext2'
    if "ext3" in x:
        modified_name += '_ext3'
    if "ext4" in x:
        modified_name += '_ext4'
    requestNames.append(modified_name)

# ===============================================================================
# Classical part of crab, after resolving the * it uses in the example below just the first entry
#

from CRABClient.UserUtilities import config, getUsernameFromSiteDB


config = config()
config.General.workArea = 'DATA_JetHT2017'
config.General.transferOutputs = True
config.General.transferLogs = True
        
config.JobType.pluginName = 'Analysis'
#config.JobType.psetName = '/nfs/dust/cms/user/karavdia/CMSSW_9_4_3/src/UHH2/core/python/ntuplewriter_addPrefireInfo_woElectrons.py'
config.JobType.psetName = '/nfs/dust/cms/user/karavdia/CMSSW_9_4_3/src/UHH2/core/python/ntuplewriter_addPrefireInfo_woElectrons_woXCONE_woHOTVR.py'
config.JobType.outputFiles = ["Ntuple.root"]
config.JobType.inputFiles = ['/nfs/dust/cms/user/karavdia/CMSSW_9_4_2/src/UHH2/JECDatabase/SQLiteFiles/Summer16_03Feb2017_V9_MC.db']
config.JobType.sendExternalFolder = True
config.Data.useParent = True
config.Data.inputDBS = 'global'
#config.Data.splitting = 'FileBased'
#config.Data.unitsPerJob = 1
config.Data.splitting = 'EventAwareLumiBased'
config.Data.unitsPerJob = 15000
#NJOBS = 1
#config.Data.totalUnits = config.Data.unitsPerJob * NJOBS #For test run 1 job only
#config.Data.unitsPerJob = 5000
config.Data.outLFNDirBase = '/store/user/%s/RunII_94X_v2/DATA_JetHT_woElectrons_woXCONE_woHOTVR/' % (getUsernameFromSiteDB())
config.Data.publication = False
config.Data.lumiMask ='/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions17/13TeV/ReReco/Cert_294927-306462_13TeV_EOY2017ReReco_Collisions17_JSON.txt'

#config.Data.allowNonValidInputDataset = True
#config.Data.publishDataName = 'CRAB3_tutorial_May2015_MC_analysis'

config.Site.storageSite = 'T2_DE_DESY'

config.General.requestName = requestNames[0]
config.Data.inputDataset = inputDatasets[0]


