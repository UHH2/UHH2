import FWCore.ParameterSet.Config as cms
from UHH2.core.ntuple_generator import generate_process  # use CMSSW type path for CRAB
from UHH2.core.optionsParse import setup_opts, parse_apply_opts


"""NTuple config for UL16postVFP Data datasets.

You should try and put any centralised changes in generate_process(), not here.
"""


process = generate_process(year="UL16postVFP", useData=True)

# Please do not commit changes to source filenames - used for consistency testing
process.source.fileNames = cms.untracked.vstring([
    '/store/data/Run2016F/JetHT/MINIAOD/UL2016_MiniAODv2-v2/70000/7265A535-3E52-6943-89F6-9D1E0800C538.root'
])

# Do this after setting process.source.fileNames, since we want the ability to override it on the commandline
options = setup_opts()
parse_apply_opts(process, options)

with open('pydump_data_UL16postVFP.py', 'w') as f:
    f.write(process.dumpPython())
