import FWCore.ParameterSet.Config as cms
from UHH2.core.ntuple_generator import generate_process  # use CMSSW type path for CRAB


"""NTuple config for 2016 Data datasets.

You should try and put any centralised changes in generate_process(), not here.
"""


process = generate_process(year="2016", useData=True)

# Please do not commit changes to source filenames - used for consistency testing
process.source.fileNames = cms.untracked.vstring([
    '/store/data/Run2016D/JetHT/MINIAOD/17Jul2018-v1/80000/601A2C83-5E8D-E811-9BF9-1CB72C0A3DBD.root'
])
process.maxEvents = cms.untracked.PSet(input=cms.untracked.int32(500))
process.options.wantSummary = cms.untracked.bool(True)

with open('pydump_data_2016.py', 'w') as f:
    f.write(process.dumpPython())
