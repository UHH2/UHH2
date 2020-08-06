import FWCore.ParameterSet.Config as cms
from UHH2.core.ntuple_generator import generate_process  # use CMSSW type path for CRAB
from UHH2.core.optionsParse import setup_opts, parse_apply_opts


"""NTuple config for UL18 Data datasets.

You should try and put any centralised changes in generate_process(), not here.
"""


process = generate_process(year="UL18", useData=True)

# Please do not commit changes to source filenames - used for consistency testing
process.source.fileNames = cms.untracked.vstring([
    '/store/data/Run2018B/JetHT/MINIAOD/12Nov2019_UL2018-v2/110000/C65A083B-9762-AF49-ACA1-DA764EA05A9E.root'
    # '/store/data/Run2018B/SingleMuon/MINIAOD/12Nov2019_UL2018-v2/70000/5EEF8EA9-86A1-A147-91BB-194B64FC4972.root'
    # '/store/data/Run2018B/EGamma/MINIAOD/12Nov2019_UL2018-v2/100000/F37232DA-6F44-D640-A3BB-5CFEC6D6D04A.root'
])

# Turn on jet constituent storing for leading three AK4 jets
process.MyNtuple.doPFJetConstituentsNjets=3

# Turn on jet constituent storing for leading three AK8 jets
process.MyNtuple.doPFTopJetConstituentsNjets=3

# Turn on jet constituent storing for all AK8 jets with pT > 300 GeV
process.MyNtuple.doPFTopJetConstituentsMinJetPt=300

# Turn on jet constituent storing for the two exclusive XCone jets
process.MyNtuple.doPFxconeJetConstituentsNjets=2

# Do this after setting process.source.fileNames, since we want the ability to override it on the commandline
options = setup_opts()
parse_apply_opts(process, options)

with open('pydump_data_UL18.py', 'w') as f:
    f.write(process.dumpPython())
