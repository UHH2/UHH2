import FWCore.ParameterSet.Config as cms
from UHH2.core.ntuple_generator import generate_process  # use CMSSW type path for CRAB
from UHH2.core.optionsParse import setup_opts, parse_apply_opts


"""NTuple config for UL17 Data datasets.

You should try and put any centralised changes in generate_process(), not here.
"""


process = generate_process(year="UL17", useData=True)

# Please do not commit changes to source filenames - used for consistency testing
process.source.fileNames = cms.untracked.vstring([
    '/store/data/Run2017D/JetHT/MINIAOD/09Aug2019_UL2017-v1/50000/8320FFCD-A41A-1849-A506-52EF233246F4.root'
    # '/store/data/Run2017D/SingleElectron/MINIAOD/09Aug2019_UL2017-v1/270000/9CEAFD24-9D9D-304D-BEC0-3FE3B4903429.root'
    # '/store/data/Run2017D/SingleMuon/MINIAOD/09Aug2019_UL2017-v1/50000/ACB10C92-4B9B-5749-8C96-60942152E15C.root'
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

with open('pydump_data_UL17.py', 'w') as f:
    f.write(process.dumpPython())
