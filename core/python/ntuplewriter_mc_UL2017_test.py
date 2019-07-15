import FWCore.ParameterSet.Config as cms
from UHH2.core.ntuple_generator import generate_process  # use CMSSW type path for CRAB
from UHH2.core.optionsParse import setup_opts, parse_apply_opts


"""NTuple config for UL17 Datasets.

You should try and put any centralised changes in generate_process(), not here.
"""


process = generate_process(year="2018UL17", useData=False)

# Please do not commit changes to source filenames - used for consistency testing
process.source.fileNames = cms.untracked.vstring([
    # '/store/relval/CMSSW_10_6_1/JetHT/MINIAOD/106X_dataRun2_v15_rsb_RelVal_2017C-v1/10000/DF91AEDE-0FC0-DB42-9037-8CABDB66F4CE.root',
    '/store/relval/CMSSW_10_6_1/JetHT/MINIAOD/106X_dataRun2_v15_rsb_RelVal_2017F-v1/10000/A6A473E4-D626-3C4E-89A1-5BCE1D44B359.root'
])

# Do this after setting process.source.fileNames, since we want the ability to override it on the commandline
options = setup_opts()
parse_apply_opts(process, options)
with open('pydump_mc_2018UL17.py', 'w') as f:
    f.write(process.dumpPython())


#
