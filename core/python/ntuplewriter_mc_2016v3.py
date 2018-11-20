import FWCore.ParameterSet.Config as cms
from UHH2.core.ntuple_generator import generate_process  # use CMSSW type path for CRAB


"""NTuple config for 2016 MC datasets.

You should try and put any centralised changes in generate_process(), not here.
"""


process = generate_process(year="2016v3", useData=False)

# Please do not commit changes to source filenames - used for consistency testing
process.source.fileNames = cms.untracked.vstring([
    '/store/mc/RunIISummer16MiniAODv3/TT_TuneCUETP8M2T4_13TeV-powheg-pythia8/MINIAODSIM/PUMoriond17_94X_mcRun2_asymptotic_v3-v1/00000/02AF6CD5-22C3-E811-B5AA-0CC47AFC3C64.root'
])
process.maxEvents = cms.untracked.PSet(input=cms.untracked.int32(500))
process.options.wantSummary = cms.untracked.bool(True)

with open('pydump_mc_2016v3.py', 'w') as f:
    f.write(process.dumpPython())
