import FWCore.ParameterSet.Config as cms
from UHH2.core.ntuple_generator import generate_process  # use CMSSW type path for CRAB
from UHH2.core.optionsParse import setup_opts, parse_apply_opts


"""NTuple config for 2018 miniaod Data datasets with jet constituents

You should try and put any centralised changes in generate_process(), not here.
"""


process = generate_process(year="2018", useData=True)

# Please do not commit changes to source filenames - used for consistency testing
process.source.fileNames = cms.untracked.vstring([
     '/store/data/Run2018D/JetHT/MINIAOD/PromptReco-v2/000/325/170/00000/9494B803-292B-F343-9BCC-6CAD47CB0C8B.root'
     # '/store/data/Run2018D/SingleMuon/MINIAOD/PromptReco-v2/000/321/233/00000/6C7B6F79-24A3-E811-A7BF-FA163EC61E98.root'
     # '/store/data/Run2018D/EGamma/MINIAOD/PromptReco-v2/000/325/175/00000/9D0F9360-DD60-314A-BB24-33D62A3CD6BD.root'
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

with open('pydump_data_2018.py', 'w') as f:
    f.write(process.dumpPython())
