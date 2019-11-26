import FWCore.ParameterSet.Config as cms
from UHH2.core.ntuple_generator import generate_process  # use CMSSW type path for CRAB
from UHH2.core.optionsParse import setup_opts, parse_apply_opts


"""NTuple config for 2017UL MC datasets.

You should try and put any centralised changes in generate_process(), not here.
"""


process = generate_process(year="2017UL", useData=False)

# Please do not commit changes to source filenames - used for consistency testing
process.source.fileNames = cms.untracked.vstring([
    '/store/mc/RunIISummer19UL17MiniAOD/QCD_Pt_120to170_TuneCP5_13TeV_pythia8/MINIAODSIM/106X_mc2017_realistic_v6-v2/230000/3C5C104E-B823-1843-B77A-AB75AA404055.root'
])

# Do this after setting process.source.fileNames, since we want the ability to override it on the commandline
options = setup_opts()
parse_apply_opts(process, options)

with open('pydump_mc_2017UL.py', 'w') as f:
    f.write(process.dumpPython())
