import FWCore.ParameterSet.Config as cms
from UHH2.core.ntuple_generator import generate_process  # use CMSSW type path for CRAB
from UHH2.core.optionsParse import setup_opts, parse_apply_opts


"""NTuple config for 2017v2 Data datasets.

You should try and put any centralised changes in generate_process(), not here.
"""


process = generate_process(year="2017v2", useData=True)

# Please do not commit changes to source filenames - used for consistency testing
process.source.fileNames = cms.untracked.vstring([
    '/store/data/Run2017D/JetHT/MINIAOD/31Mar2018-v1/70000/DAAA92B6-8044-E811-9E9E-0CC47A4D7638.root',
    # '/store/data/Run2017D/SingleMuon/MINIAOD/31Mar2018-v1/80000/1E703527-F436-E811-80A7-E0DB55FC1055.root',
    # '/store/data/Run2017D/SingleElectron/MINIAOD/31Mar2018-v1/80000/4899B9E7-F038-E811-8012-00000065FE80.root',
    # '/store/data/Run2017D/MET/MINIAOD/31Mar2018-v1/00000/88C7FC3D-FD38-E811-97D5-0CC47A7C340C.root',
])

# Do this after setting process.source.fileNames, since we want the ability to override it on the commandline
options = setup_opts()
parse_apply_opts(process, options)

with open('pydump_data_2017v2.py', 'w') as f:
    f.write(process.dumpPython())
