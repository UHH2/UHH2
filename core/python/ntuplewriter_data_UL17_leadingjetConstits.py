import FWCore.ParameterSet.Config as cms
from UHH2.core.ntuple_generator import generate_process  # use CMSSW type path for CRAB
from UHH2.core.optionsParse import setup_opts, parse_apply_opts


"""NTuple config for UL17 Data datasets.

You should try and put any centralised changes in generate_process(), not here.
"""


process = generate_process(year="UL17", useData=True)

# Please do not commit changes to source filenames - used for consistency testing
process.source.fileNames = cms.untracked.vstring([
    '/store/data/Run2017D/JetHT/MINIAOD/UL2017_MiniAODv2-v1/40000/C9790EDE-0D90-1947-8FC9-0855F0AA810A.root'
    # '/store/data/Run2017D/SingleElectron/MINIAOD/UL2017_MiniAODv2-v1/250000/9E13F7BD-475C-5648-A170-6DF56ADE7DFD.root'
    # '/store/data/Run2017D/SingleMuon/MINIAOD/UL2017_MiniAODv2-v1/280000/BA4F67E1-5894-4C44-92FF-E1D4374D459D.root'
])

# Turn on jet constituent storing for leading three AK8 jets
process.MyNtuple.doPFTopJetConstituentsNjets=3

# Turn on jet constituent storing for all AK8 jets with pT > 170 GeV
process.MyNtuple.doPFTopJetConstituentsMinJetPt=170

# Do this after setting process.source.fileNames, since we want the ability to override it on the commandline
options = setup_opts()
parse_apply_opts(process, options)

with open('pydump_data_UL17.py', 'w') as f:
    f.write(process.dumpPython())
