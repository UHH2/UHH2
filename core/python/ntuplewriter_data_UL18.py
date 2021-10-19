import FWCore.ParameterSet.Config as cms
from UHH2.core.ntuple_generator import generate_process  # use CMSSW type path for CRAB
from UHH2.core.optionsParse import setup_opts, parse_apply_opts


"""NTuple config for UL18 Data datasets.

You should try and put any centralised changes in generate_process(), not here.
"""


process = generate_process(year="UL18", useData=True)

# Please do not commit changes to source filenames - used for consistency testing
process.source.fileNames = cms.untracked.vstring([
    '/store/data/Run2018B/JetHT/MINIAOD/UL2018_MiniAODv2-v1/260000/DBA9BBD8-CE95-AA40-97C0-17C5AAED2AE8.root'
    # '/store/data/Run2018B/SingleMuon/MINIAOD/UL2018_MiniAODv2-v2/100000/B29BDCC8-5429-7244-96D9-A5BF6C04BB73.root'
    # '/store/data/Run2018B/EGamma/MINIAOD/UL2018_MiniAODv2-v1/2610000/16C79B67-BD8F-884F-8DC9-8025C13F6E72.root'
])

# Do this after setting process.source.fileNames, since we want the ability to override it on the commandline
options = setup_opts()
parse_apply_opts(process, options)

with open('pydump_data_UL18.py', 'w') as f:
    f.write(process.dumpPython())
