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

inputDatasets = ['/TTbarDMJets_scalar_Mchi-1_Mphi-10_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISpring16MiniAODv2-PUSpring16RAWAODSIM_reHLT_80X_mcRun2_asymptotic_v14_ext1-v1/MINIAODSIM'
              '/TTbarDMJets_scalar_Mchi-1_Mphi-20_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISpring16MiniAODv2-PUSpring16RAWAODSIM_reHLT_80X_mcRun2_asymptotic_v14_ext1-v1/MINIAODSIM',
              '/TTbarDMJets_scalar_Mchi-1_Mphi-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISpring16MiniAODv2-PUSpring16RAWAODSIM_reHLT_80X_mcRun2_asymptotic_v14_ext1-v1/MINIAODSIM',
              '/TTbarDMJets_scalar_Mchi-1_Mphi-100_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISpring16MiniAODv2-PUSpring16RAWAODSIM_reHLT_80X_mcRun2_asymptotic_v14_ext1-v1/MINIAODSIM',
              '/TTbarDMJets_scalar_Mchi-1_Mphi-200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISpring16MiniAODv2-PUSpring16RAWAODSIM_reHLT_80X_mcRun2_asymptotic_v14_ext1-v1/MINIAODSIM',
              '/TTbarDMJets_scalar_Mchi-1_Mphi-300_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISpring16MiniAODv2-PUSpring16RAWAODSIM_reHLT_80X_mcRun2_asymptotic_v14_ext1-v1/MINIAODSIM',
              '/TTbarDMJets_scalar_Mchi-1_Mphi-500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISpring16MiniAODv2-PUSpring16RAWAODSIM_reHLT_80X_mcRun2_asymptotic_v14_ext1-v1/MINIAODSIM',
              '/TTbarDMJets_scalar_Mchi-1_Mphi-10000_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISpring16MiniAODv2-PUSpring16RAWAODSIM_reHLT_80X_mcRun2_asymptotic_v14_ext1-v2/MINIAODSIM',
              '/TTbarDMJets_scalar_Mchi-10_Mphi-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISpring16MiniAODv2-PUSpring16RAWAODSIM_reHLT_80X_mcRun2_asymptotic_v14_ext1-v1/MINIAODSIM',
              '/TTbarDMJets_scalar_Mchi-10_Mphi-10_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISpring16MiniAODv2-PUSpring16RAWAODSIM_reHLT_80X_mcRun2_asymptotic_v14_ext1-v1/MINIAODSIM',
              '/TTbarDMJets_scalar_Mchi-10_Mphi-15_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISpring16MiniAODv2-PUSpring16RAWAODSIM_reHLT_80X_mcRun2_asymptotic_v14_ext1-v1/MINIAODSIM',
              '/TTbarDMJets_scalar_Mchi-10_Mphi-100_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISpring16MiniAODv2-PUSpring16RAWAODSIM_reHLT_80X_mcRun2_asymptotic_v14_ext1-v1/MINIAODSIM',
              '/TTbarDMJets_scalar_Mchi-50_Mphi-10_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISpring16MiniAODv2-PUSpring16RAWAODSIM_reHLT_80X_mcRun2_asymptotic_v14_ext1-v1/MINIAODSIM',
              '/TTbarDMJets_scalar_Mchi-50_Mphi-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISpring16MiniAODv2-PUSpring16RAWAODSIM_reHLT_80X_mcRun2_asymptotic_v14_ext1-v1/MINIAODSIM',
              '/TTbarDMJets_scalar_Mchi-50_Mphi-95_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISpring16MiniAODv2-PUSpring16RAWAODSIM_reHLT_80X_mcRun2_asymptotic_v14_ext1-v1/MINIAODSIM',
              '/TTbarDMJets_scalar_Mchi-50_Mphi-200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISpring16MiniAODv2-PUSpring16RAWAODSIM_reHLT_80X_mcRun2_asymptotic_v14_ext1-v1/MINIAODSIM',
              '/TTbarDMJets_scalar_Mchi-50_Mphi-300_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISpring16MiniAODv2-PUSpring16RAWAODSIM_reHLT_80X_mcRun2_asymptotic_v14_ext1-v1/MINIAODSIM'
]
inputDatasets = autocomplete_Datasets(inputDatasets)
requestNames = []
for x in inputDatasets:
    name = x.split('/')[1]
    modified_name =name.replace('TuneCUETP8M1_13TeV-madgraphMLM-pythia8','25ns')
    requestNames.append(modified_name)

# ===============================================================================
# Classical part of crab, after resolving the * it uses in the example below just the first entry
#

from CRABClient.UserUtilities import config, getUsernameFromSiteDB


config = config()
config.General.workArea = 'crab_Test'
config.General.transferOutputs = True
config.General.transferLogs = True
        
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = '/afs/desy.de/user/m/mameyer/xxl-af-cms/CMSSW_8_0_20/src/UHH2/core/python/ntuplewriter.py'
config.JobType.outputFiles = ["Ntuple.root"]
config.JobType.inputFiles = ['/afs/desy.de/user/m/mameyer/xxl-af-cms/CMSSW_8_0_20/src/UHH2/core/python/Summer15_25nsV2_MC.db']
        
config.Data.inputDBS = 'global'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 1
config.Data.outLFNDirBase = '/store/user/%s/RunII_80X_v2' % (getUsernameFromSiteDB())
config.Data.publication = False
#config.Data.allowNonValidInputDataset = True
#config.Data.publishDataName = 'CRAB3_tutorial_May2015_MC_analysis'

config.Site.storageSite = 'T2_DE_DESY'

config.General.requestName = requestNames[0]
config.Data.inputDataset = inputDatasets[0]


