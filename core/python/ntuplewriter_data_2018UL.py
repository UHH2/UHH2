import FWCore.ParameterSet.Config as cms
from UHH2.core.ntuple_generator import generate_process  # use CMSSW type path for CRAB
from UHH2.core.optionsParse import setup_opts, parse_apply_opts


"""NTuple config for 2018UL Data datasets.

You should try and put any centralised changes in generate_process(), not here.
"""


process = generate_process(year="2018UL", useData=True)

# Please do not commit changes to source filenames - used for consistency testing
process.source.fileNames = cms.untracked.vstring([
    '/store/data/Run2018D/JetHT/MINIAOD/12Nov2019_UL2018-v2/270000/FEA68F16-AA26-2D44-8D1C-C979BE62A35C.root'
])

# Do this after setting process.source.fileNames, since we want the ability to override it on the commandline
options = setup_opts()
parse_apply_opts(process, options)

with open('pydump_data_2018UL.py', 'w') as f:
    f.write(process.dumpPython())
