import FWCore.ParameterSet.Config as cms
from UHH2.core.ntuple_generator import generate_process  # use CMSSW type path for CRAB
from UHH2.core.optionsParse import setup_opts, parse_apply_opts


"""NTuple config for Run3 Data datasets.

You should try and put any centralised changes in generate_process(), not here.
"""


process = generate_process(year="2022", useData=True)

# Please do not commit changes to source filenames - used for consistency testing
process.source.fileNames = cms.untracked.vstring([
    'root://cms-xrd-global.cern.ch//store/data/Run2022F/JetMET/MINIAOD/PromptReco-v1/000/360/381/00000/f2444ee4-ccae-42cc-a961-3cd3266bfd0f.root' 
])

# Do this after setting process.source.fileNames, since we want the ability to override it on the commandline
options = setup_opts()
parse_apply_opts(process, options)

with open('pydump_data_Run3.py', 'w') as f:
    f.write(process.dumpPython())
