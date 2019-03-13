import FWCore.ParameterSet.Config as cms
from UHH2.core.ntuple_generator import generate_process  # use CMSSW type path for CRAB
from UHH2.core.optionsParse import setup_opts, parse_apply_opts


"""NTuple config for 2018 Data datasets.

You should try and put any centralised changes in generate_process(), not here.
"""


process = generate_process(year="2018", useData=True)

# Please do not commit changes to source filenames - used for consistency testing
process.source.fileNames = cms.untracked.vstring([
    '/store/data/Run2018D/JetHT/MINIAOD/PromptReco-v2/000/325/170/00000/9494B803-292B-F343-9BCC-6CAD47CB0C8B.root'
    # '/store/data/Run2018D/SingleMuon/MINIAOD/PromptReco-v2/000/321/233/00000/6C7B6F79-24A3-E811-A7BF-FA163EC61E98.root'
    # '/store/data/Run2018D/EGamma/MINIAOD/PromptReco-v2/000/325/175/00000/9D0F9360-DD60-314A-BB24-33D62A3CD6BD.root'
])

process.MyNtuple.doPFJetConstituentsNjets=4
process.MyNtuple.doPFTopJetConstituentsNjets=4
process.MyNtuple.doXCone_dijet=True
process.MyNtuple.doPFxconeDijetJetConstituentsNjets=4
#process.MyNtuple.doGenTopJetConstituentsNjets=4
#process.MyNtuple.doGenJetConstituentsNjets=4
#process.MyNtuple.doGenXCone_dijet=True
#process.MyNtuple.doGenxconeDijetJetConstituentsNjets=4


# Do this after setting process.source.fileNames, since we want the ability to override it on the commandline
options = setup_opts()
parse_apply_opts(process, options)
with open('pydump_data_2018.py', 'w') as f:
    f.write(process.dumpPython())


#
