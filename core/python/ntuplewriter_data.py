import FWCore.ParameterSet.Config as cms
from UHH2.core.ntuple_generator import generate_process  # use CMSSW type path for CRAB


"""NTuple config for MC datasets.

You should try and put any centralised changes in generate_process(), not here.
"""


process = generate_process(useData=True)

process.source.fileNames = cms.untracked.vstring([
    '/store/data/Run2017D/JetHT/MINIAOD/17Nov2017-v1/20000/0249B143-8CCC-E711-BA7C-0025905C2CD0.root'    
])
process.maxEvents = cms.untracked.PSet(input=cms.untracked.int32(100))
process.options.wantSummary = cms.untracked.bool(True)

with open('pydump_data.py', 'w') as f:
    f.write(process.dumpPython())
