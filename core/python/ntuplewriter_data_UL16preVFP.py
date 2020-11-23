import FWCore.ParameterSet.Config as cms
from UHH2.core.ntuple_generator import generate_process  # use CMSSW type path for CRAB
from UHH2.core.optionsParse import setup_opts, parse_apply_opts


"""NTuple config for UL16preVFP Data datasets.

You should try and put any centralised changes in generate_process(), not here.
"""


process = generate_process(year="UL16preVFP", useData=True)

# Please do not commit changes to source filenames - used for consistency testing
process.source.fileNames = cms.untracked.vstring([
    '/store/data/Run2016B/JetHT/MINIAOD/21Feb2020_ver1_UL2016_HIPM-v1/240000/07EDE82B-253D-EA4E-B120-2F82BB60DA5F.root'
])

# Do this after setting process.source.fileNames, since we want the ability to override it on the commandline
options = setup_opts()
parse_apply_opts(process, options)

with open('pydump_data_UL16preVFP.py', 'w') as f:
    f.write(process.dumpPython())
