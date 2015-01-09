# Before using this file, at least:
# * change name, config.Data.inputDataset accordingly; consider also increasing unitsPerJob to process more files per job
# * create a symlink 'pset.py' to point to the actual CMSSW configuration to use; note that the python config is evaluated locally



# set name to some value to be used in the workdir (see below) and as
# requestName for crab (which controls the location of the output file).
name = 'TEST'

from WMCore.Configuration import Configuration
import os

config = Configuration()

config.section_("General")
config.General.requestName = name
config.General.workArea = os.getenv('HOME') + '/xxl-af-cms/crab-workdir/' + name

config.section_("JobType")
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'pset.py'

config.section_("Data")
config.Data.inputDataset = '/ZH_ZToLL_HToBB_M-125_8TeV-powheg-herwigpp/Summer12_DR53X-PU_S10_START53_V7A-v1/AODSIM'
config.Data.inputDBS = 'global'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 1
config.Data.publication = False

config.section_("Site")
config.Site.storageSite = 'T2_DE_DESY'
#config.Site.blacklist = ['T2_BE_UCL']

config.section_("User")
config.User.voGroup = 'dcms'
