import FWCore.ParameterSet.Config as cms
from UHH2.core.ntuple_generator import generate_process  # use CMSSW type path for CRAB
from UHH2.core.optionsParse import setup_opts, parse_apply_opts


"""NTuple config for 2016 v2 miniaod MC datasets.

You should try and put any centralised changes in generate_process(), not here.
"""


process = generate_process(year="2016v2", useData=False)

# Please do not commit changes to source filenames - used for consistency testing
process.source.fileNames = cms.untracked.vstring([
    '/store/mc/RunIISummer16MiniAODv2/TT_TuneCUETP8M2T4_13TeV-powheg-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/80000/E022CF80-0ABF-E611-B5CD-00259048A87C.root'
])

# Turn on jet constituent storing for leading three AK4 jets
process.MyNtuple.doPFJetConstituentsNjets=3
process.MyNtuple.doGenJetConstituentsNjets=3

# Turn on jet constituent storing for leading three AK8 jets
process.MyNtuple.doPFTopJetConstituentsNjets=3
process.MyNtuple.doGenTopJetConstituentsNjets=3

# Turn on jet constituent storing for all AK8 jets with pT > 300 GeV
process.MyNtuple.doPFTopJetConstituentsMinJetPt=300
process.MyNtuple.doGenTopJetConstituentsMinJetPt=300

# Turn on jet constituent storing for the two exclusive XCone jets
process.MyNtuple.doPFxconeJetConstituentsNjets=2
process.MyNtuple.doGenxconeJetConstituentsNjets=2

# Store leptons with photons in a small cone around them to add up collinear radiation, only needed for TT (switch off for other samples)
process.MyNtuple.genjet_sources.extend(["muonGenJets", "electronGenJets"])

# Do this after setting process.source.fileNames, since we want the ability to override it on the commandline
options = setup_opts()
parse_apply_opts(process, options)

with open('pydump_mc_2016v2.py', 'w') as f:
    f.write(process.dumpPython())
