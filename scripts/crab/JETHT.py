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


import CrabYearUtilities
from DasQuery import autocomplete_Datasets


# set storeJetConstituents=True to store the constituents for the leading AK8 jets (using the ntuplewriter_*_leadingjetConstits)
storeJetConstituents = False

inputDatasets = [
# '/JetMET0/Run2023C-22Sep2023_v1-v1/MINIAOD',
# '/JetMET0/Run2023C-22Sep2023_v2-v1/MINIAOD',
# '/JetMET0/Run2023C-22Sep2023_v3-v1/MINIAOD',
# '/JetMET0/Run2023C-22Sep2023_v4-v1/MINIAOD',
# '/JetMET1/Run2023C-22Sep2023_v1-v1/MINIAOD',
# '/JetMET1/Run2023C-22Sep2023_v2-v1/MINIAOD',
# '/JetMET1/Run2023C-22Sep2023_v3-v1/MINIAOD',
# '/JetMET1/Run2023C-22Sep2023_v4-v1/MINIAOD',
'/JetMET0/Run2023D-22Sep2023_v1-v1/MINIAOD',
'/JetMET0/Run2023D-22Sep2023_v2-v1/MINIAOD',
'/JetMET1/Run2023D-22Sep2023_v1-v1/MINIAOD',
'/JetMET1/Run2023D-22Sep2023_v2-v1/MINIAOD',
]
inputDatasets = autocomplete_Datasets(inputDatasets)
requestNames = [CrabYearUtilities.get_request_name(x) for x in inputDatasets] # Here you can define custom request names if the get_request_names function doesn't return nice ones

if __name__=='__main__':
    print('Request names (100 characters max.) will be:')
    for x in requestNames:
        print(x.ljust(101), 'Length:', len(x))
    exit(0)

# ===============================================================================
# Classical part of crab, after resolving the * it uses in the example below just the first entry
#

from CRABClient.UserUtilities import config
from CRABClient.ClientExceptions import ProxyException
import os, sys
import re


config = config()
config.General.workArea = 'crab_Test' # Don't forget to adjust this
config.General.transferOutputs = True
config.General.transferLogs = True

config.JobType.pluginName = 'Analysis'

config.JobType.outputFiles = ["Ntuple.root"]
config.JobType.maxMemoryMB = 2500

config.Data.inputDBS = 'global'
config.Data.splitting = 'EventAwareLumiBased' # (don't use the 'Automatic' splitting as recommended in the crab TWiki for real data; it sometimes bugs out and leads to duplicate data -- an absolute No-Go!)
config.Data.unitsPerJob = 24000 # targeted approximate number of events per job


config.Data.publication = False
config.JobType.sendExternalFolder = True
#config.Data.allowNonValidInputDataset = True

config.Site.storageSite = 'T2_DE_DESY'


# The remaining lines are only used if one does not run this template with multicrab, but rather 'manually ' with crab itself.
# In this case crab will only run on the first entry in inputDatasets!

# If you know what you are doing and would like multicrab not to overwrite the psetname or outLFNDirBase
# set respective flag to False:
# DefaultPsetName = False
# DefaultOutLFNDirBase = False

# By default choose the ntuplewriter based on the DAS string
# You can also choose the ntuplewriter yourself by replacing the function get_ntuplewriter with the name of the ntuplewriter_*_*.py
# NOTE: this will not be used when submitting with multicrab, if config.JobType.DefaultPsetName is not set to False!!!
config.JobType.psetName = os.path.join(os.environ['CMSSW_BASE'], 'src/UHH2/core/python/', CrabYearUtilities.get_ntuplewriter(inputDatasets[0],storeJetConstituents))
print(config.JobType.psetName)

# Add subdirectory using year from config filename
# NOTE: this will not be used when submitting with multicrab, if config.Data.DefaultOutLFNDirBaseis not set to False!!!
config.Data.outLFNDirBase = CrabYearUtilities.get_outLFNDirBase(inputDatasets[0])

# If you want to run some private production and not put it in the group area, use this instead:
# replacing YOUR_CERN_USERNAME_HERE as appropriate
# config.Data.outLFNDirBase = CrabYearUtilities.get_outLFNDirBase(inputDatasets[0],prefix='/store/user/YOUR_CERN_USERNAME_HERE/RunII_106X_v2/')
# If you still want multicrab to choose year dependent sub-directories, but not put it in the group area, you can provide the base-dir prefix:
# outLFNDirBasePrefix = '/store/user/YOUR_CERN_USERNAME_HERE/RunII_106X_v2/'

if 'YOUR_CERN_USERNAME_HERE' in config.Data.outLFNDirBase or ('YOUR_CERN_USERNAME_HERE' in globals().get('outLFNDirBasePrefix','')):
    raise RuntimeError("You didn't insert your CERN username in config.Data.outLFNDirBase or outLFNDirBasePrefix, please fix it")

if len(inputDatasets) > 0 and len(requestNames) > 0:
    config.General.requestName = requestNames[0]
    config.Data.inputDataset = inputDatasets[0]

# If one runs in multicrab we need to set these UHH2 specific Configuration attributes,
# in order to be able to make decisions on a per dataset basis.
# If you are running multicrab in an additional wrapper, you might have to move everything out of the if-statement.
if('multicrab' in sys.argv[0]):
    config.section_("UHH2")
    config.UHH2.storeJetConstituents = storeJetConstituents
    config.UHH2.DefaultPsetName = globals().get('DefaultPsetName',True)
    config.UHH2.DefaultOutLFNDirBase = globals().get('DefaultOutLFNDirBase',True)
    config.UHH2.outLFNDirBasePrefix  = globals().get('outLFNDirBasePrefix',None)
