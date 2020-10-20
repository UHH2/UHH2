import FWCore.ParameterSet.Config as cms
from UHH2.core.ntuple_generator import generate_process  # use CMSSW type path for CRAB
from UHH2.core.optionsParse import setup_opts, parse_apply_opts


"""NTuple config for UL16postVFP Data datasets.

You should try and put any centralised changes in generate_process(), not here.
"""


process = generate_process(year="UL16postVFP", useData=True)

# Please do not commit changes to source filenames - used for consistency testing
process.source.fileNames = cms.untracked.vstring([
    '/store/data/Run2016F/JetHT/MINIAOD/21Feb2020_UL2016-v1/20000/0A917D93-1CCE-8D47-ABFB-24E36E58575F.root'
])

# Do this after setting process.source.fileNames, since we want the ability to override it on the commandline
options = setup_opts()
parse_apply_opts(process, options)

with open('pydump_data_UL16postVFP.py', 'w') as f:
    f.write(process.dumpPython())
