import FWCore.ParameterSet.Config as cms
from UHH2.core.ntuple_generator import generate_process  # use CMSSW type path for CRAB
from UHH2.core.optionsParse import setup_opts, parse_apply_opts


"""NTuple config for 2016 v3 miniaod MC datasets.

You should try and put any centralised changes in generate_process(), not here.
"""


process = generate_process(year="2016v3", useData=False)

# Please do not commit changes to source filenames - used for consistency testing
process.source.fileNames = cms.untracked.vstring([
    '/store/mc/RunIISummer16MiniAODv3/ST_tW_top_5f_inclusiveDecays_13TeV-powheg-pythia8_TuneCUETP8M2T4/MINIAODSIM/PUMoriond17_94X_mcRun2_asymptotic_v3-v1/100000/12971E7E-A9BC-E811-A71A-0CC47A1DF800.root'
])

# Do this after setting process.source.fileNames, since we want the ability to override it on the commandline
options = setup_opts()
parse_apply_opts(process, options)

with open('pydump_mc_2016v3.py', 'w') as f:
    f.write(process.dumpPython())
