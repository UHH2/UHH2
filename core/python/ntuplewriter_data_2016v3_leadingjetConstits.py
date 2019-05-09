import FWCore.ParameterSet.Config as cms
from UHH2.core.ntuple_generator import generate_process  # use CMSSW type path for CRAB
from UHH2.core.optionsParse import setup_opts, parse_apply_opts


"""NTuple config for 2016 v3 miniaod Data datasets with jet constituents

You should try and put any centralised changes in generate_process(), not here.
"""


process = generate_process(year="2016v3", useData=True)

# Please do not commit changes to source filenames - used for consistency testing
process.source.fileNames = cms.untracked.vstring([
    '/store/data/Run2016C/JetHT/MINIAOD/17Jul2018-v1/50000/DCEA62FA-148C-E811-BB6B-A0369FD0B3A4.root'
    # '/store/data/Run2016B/SingleMuon/MINIAOD/17Jul2018_ver1-v1/20000/F846D019-069B-E811-A03B-0242AC1C0502.root'
    # '/store/data/Run2016H/SingleElectron/MINIAOD/17Jul2018-v1/00000/0CE37460-7B8F-E811-BF05-002590DBDFE2.root'
])

# Turn on jet constituent storing for leading three AK4 jets
process.MyNtuple.doPFJetConstituentsNjets=3

# Turn on jet constituent storing for leading three AK8 jets
process.MyNtuple.doPFTopJetConstituentsNjets=3

# Turn on jet constituent storing for all AK8 jets with pT > 300 GeV
process.MyNtuple.doPFTopJetConstituentsMinJetPt=300

# Turn on jet constituent storing for the two exclusive XCone jets
process.MyNtuple.doPFxconeJetConstituentsNjets=2

# Do this after setting process.source.fileNames, since we want the ability to override it on the commandline
options = setup_opts()
parse_apply_opts(process, options)

with open('pydump_data_2016v3.py', 'w') as f:
    f.write(process.dumpPython())
