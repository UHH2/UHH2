import FWCore.ParameterSet.Config as cms
from UHH2.core.ntuple_generator import generate_process  # use CMSSW type path for CRAB
from UHH2.core.optionsParse import setup_opts, parse_apply_opts


"""NTuple config for Run3 MC datasets.

You should try and put any centralised changes in generate_process(), not here.
"""


process = generate_process(year="2022", useData=False)

# Please do not commit changes to source filenames - used for consistency testing
process.source.fileNames = cms.untracked.vstring([
    "/store/mc/Run3Winter22MiniAOD/QCD_Pt-15to7000_TuneCP5_Flat_13p6TeV_pythia8/MINIAODSIM/122X_mcRun3_2021_realistic_v9-v2/2430000/01796a5d-9b6a-46fc-a36f-150cb43af911.root"
])

# Do this after setting process.source.fileNames, since we want the ability to override it on the commandline
options = setup_opts()
parse_apply_opts(process, options)

with open('pydump_mc_Run3.py', 'w') as f:
    f.write(process.dumpPython())
