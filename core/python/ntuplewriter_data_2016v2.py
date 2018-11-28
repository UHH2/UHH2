import FWCore.ParameterSet.Config as cms
from UHH2.core.ntuple_generator import generate_process  # use CMSSW type path for CRAB
from UHH2.core.optionsParse import setup_opts, parse_apply_opts


"""NTuple config for 2016 v2 miniaod Data datasets.

You should try and put any centralised changes in generate_process(), not here.
"""


process = generate_process(year="2016v2", useData=True)

# Please do not commit changes to source filenames - used for consistency testing
process.source.fileNames = cms.untracked.vstring([
    '/store/data/Run2016H/JetHT/MINIAOD/PromptReco-v2/000/283/353/00000/6C6DA9F1-0197-E611-8BB7-02163E011C40.root'
    # '/store/data/Run2016F/JetHT/MINIAOD/03Feb2017-v1/80000/9E632D4B-33EC-E611-92E0-0026B9278603.root'
])

# Do this after setting process.source.fileNames, since we want the ability to override it on the commandline
options = setup_opts()
parse_apply_opts(process, options)

with open('pydump_data_2016v2.py', 'w') as f:
    f.write(process.dumpPython())
