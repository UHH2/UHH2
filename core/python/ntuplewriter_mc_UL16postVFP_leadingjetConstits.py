import FWCore.ParameterSet.Config as cms
from UHH2.core.ntuple_generator import generate_process  # use CMSSW type path for CRAB
from UHH2.core.optionsParse import setup_opts, parse_apply_opts


"""NTuple config for UL16postVFP MC datasets.

You should try and put any centralised changes in generate_process(), not here.
"""


process = generate_process(year="UL16postVFP", useData=False)

# Please do not commit changes to source filenames - used for consistency testing
process.source.fileNames = cms.untracked.vstring([
    '/store/mc/RunIISummer20UL16MiniAODv2/TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8/MINIAODSIM/106X_mcRun2_asymptotic_v17-v1/120000/E0C6C30C-9DBE-6149-BA0C-594E96049E68.root'
])

# Turn on jet constituent storing for leading three AK8 jets
process.MyNtuple.doPFTopJetConstituentsNjets=3
process.MyNtuple.doGenTopJetConstituentsNjets=3

# Turn on jet constituent storing for all AK8 jets with pT > 170 GeV
process.MyNtuple.doPFTopJetConstituentsMinJetPt=170
process.MyNtuple.doGenTopJetConstituentsMinJetPt=170

# Store leptons with photons in a small cone around them to add up collinear radiation, only needed for TT (switch off for other samples)
# process.MyNtuple.genjet_sources.extend(["muonGenJets", "electronGenJets"])

# Do this after setting process.source.fileNames, since we want the ability to override it on the commandline
options = setup_opts()
parse_apply_opts(process, options)

with open('pydump_mc_UL16postVFP.py', 'w') as f:
    f.write(process.dumpPython())
