import FWCore.ParameterSet.Config as cms
from ntuple_generator import generate_process


"""NTuple config for MC datasets.

You should try and put any centralised changes in generate_process(), not here.
"""


process = generate_process(useData=False)

process.source.fileNames = cms.untracked.vstring([
    '/store/mc/RunIIFall17MiniAOD/QCD_Pt-15to7000_TuneCP5_Flat_13TeV_pythia8/MINIAODSIM/94X_mc2017_realistic_v10-v1/50000/00197229-2FDD-E711-9070-0025904AC2C4.root'
])
process.maxEvents = cms.untracked.PSet(input=cms.untracked.int32(100))
process.options.wantSummary = cms.untracked.bool(True)

with open('pydump_mc.py', 'w') as f:
    f.write(process.dumpPython())
