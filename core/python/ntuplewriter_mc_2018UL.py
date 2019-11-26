import FWCore.ParameterSet.Config as cms
from UHH2.core.ntuple_generator import generate_process  # use CMSSW type path for CRAB
from UHH2.core.optionsParse import setup_opts, parse_apply_opts


"""NTuple config for 2018UL MC datasets.

You should try and put any centralised changes in generate_process(), not here.
"""


process = generate_process(year="2018UL", useData=False)

# Please do not commit changes to source filenames - used for consistency testing
process.source.fileNames = cms.untracked.vstring([
    '/store/mc/RunIISummer19UL18MiniAOD/QCD_Pt-15to7000_TuneCP5_Flat_13TeV_pythia8/MINIAODSIM/pilot_106X_upgrade2018_realistic_v11_L1v1-v2/40000/CA158127-73ED-2847-A487-ACD682817E15.root'
])

# Do this after setting process.source.fileNames, since we want the ability to override it on the commandline
options = setup_opts()
parse_apply_opts(process, options)

with open('pydump_mc_2018UL.py', 'w') as f:
    f.write(process.dumpPython())
