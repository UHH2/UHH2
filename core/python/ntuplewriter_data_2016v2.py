import FWCore.ParameterSet.Config as cms
from UHH2.core.ntuple_generator import generate_process  # use CMSSW type path for CRAB
from UHH2.core.optionsParse import setup_opts, parse_apply_opts


"""NTuple config for 2016 v2 miniaod Data datasets.

You should try and put any centralised changes in generate_process(), not here.
"""


process = generate_process(year="2016v2", useData=True)

# Please do not commit changes to source filenames - used for consistency testing
process.source.fileNames = cms.untracked.vstring([
    '/store/data/Run2016E/JetHT/MINIAOD/03Feb2017-v1/80000/D6BA9954-07F0-E611-BEAB-0CC47A7C34D0.root'
    # '/store/data/Run2016F/SingleMuon/MINIAOD/03Feb2017-v1/110000/7A95EB9B-04EF-E611-8C78-0CC47A4D7626.root'
    # '/store/data/Run2016E/SingleElectron/MINIAOD/03Feb2017-v1/110001/7C28F3E1-7AEB-E611-BB15-0025905A6064.root'
])

# Do this after setting process.source.fileNames, since we want the ability to override it on the commandline
options = setup_opts()
parse_apply_opts(process, options)

with open('pydump_data_2016v2.py', 'w') as f:
    f.write(process.dumpPython())
