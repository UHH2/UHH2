import FWCore.ParameterSet.Config as cms
from UHH2.core.ntuple_generator import generate_process  # use CMSSW type path for CRAB
from UHH2.core.optionsParse import setup_opts, parse_apply_opts


"""NTuple config for 2017UL Data datasets.

You should try and put any centralised changes in generate_process(), not here.
"""


process = generate_process(year="2017UL", useData=True)

# Please do not commit changes to source filenames - used for consistency testing
process.source.fileNames = cms.untracked.vstring([
'/store/data/Run2017F/JetHT/MINIAOD/09Aug2019_UL2017-v1/50000/FF1B7616-113A-F14B-A97A-4939F16CB1B5.root'
])

# Do this after setting process.source.fileNames, since we want the ability to override it on the commandline
options = setup_opts()
parse_apply_opts(process, options)

with open('pydump_data_2017UL.py', 'w') as f:
    f.write(process.dumpPython())
