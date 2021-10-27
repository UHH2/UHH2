import FWCore.ParameterSet.Config as cms
from UHH2.core.ntuple_generator import generate_process  # use CMSSW type path for CRAB
from UHH2.core.optionsParse import setup_opts, parse_apply_opts


"""NTuple config for UL16preVFP Data datasets.

You should try and put any centralised changes in generate_process(), not here.
"""


process = generate_process(year="UL16preVFP", useData=True)

# Please do not commit changes to source filenames - used for consistency testing
process.source.fileNames = cms.untracked.vstring([
    '/store/data/Run2016B/JetHT/MINIAOD/ver1_HIPM_UL2016_MiniAODv2-v2/140000/284A6DBF-F437-D54F-A4D6-575270EA5B7D.root'
])

# Turn on jet constituent storing for leading three AK8 jets
process.MyNtuple.doPFTopJetConstituentsNjets=3

# Turn on jet constituent storing for all AK8 jets with pT > 170 GeV
process.MyNtuple.doPFTopJetConstituentsMinJetPt=170

# Do this after setting process.source.fileNames, since we want the ability to override it on the commandline
options = setup_opts()
parse_apply_opts(process, options)

with open('pydump_data_UL16preVFP.py', 'w') as f:
    f.write(process.dumpPython())
