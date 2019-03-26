import FWCore.ParameterSet.Config as cms
from UHH2.core.ntuple_generator import generate_process  # use CMSSW type path for CRAB
from UHH2.core.optionsParse import setup_opts, parse_apply_opts


"""NTuple config for 2018 MC datasets.

You should try and put any centralised changes in generate_process(), not here.
"""


process = generate_process(year="2018", useData=False)

# Please do not commit changes to source filenames - used for consistency testing
process.source.fileNames = cms.untracked.vstring([
    #'/store/mc/RunIIAutumn18MiniAOD/TTToHadronic_TuneCP5_13TeV-powheg-pythia8/MINIAODSIM/102X_upgrade2018_realistic_v15-v1/100000/2A6B8F74-04C7-1B46-A56E-8C786D0C2E84.root'
    # '/store/mc/RunIIAutumn18MiniAOD/TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8/MINIAODSIM/102X_upgrade2018_realistic_v15-v1/110000/D774DA06-04F6-5E45-B02E-70ECD0DD697F.root'
#    '/store/mc/RunIIAutumn18MiniAOD/QCD_Pt-15to7000_TuneCP5_Flat2018_13TeV_pythia8/MINIAODSIM/102X_upgrade2018_realistic_v15_ext1-v1/110000/5A494E5A-1A3B-B947-9F85-AF4588ACBBBA.root'
       '/store/mc/RunIIAutumn18MiniAOD/QCD_HT100to200_TuneCP5_13TeV-madgraphMLM-pythia8/MINIAODSIM/102X_upgrade2018_realistic_v15-v1/110000/086F899A-BC70-E540-A9CD-1A61829E3FEE.root'
])

process.MyNtuple.doPFJetConstituentsNjets=3
process.MyNtuple.doPFTopJetConstituentsNjets=3
process.MyNtuple.doGenJetConstituentsNjets=3
process.MyNtuple.doGenTopJetConstituentsNjets=3
process.MyNtuple.doXCone_dijet=True
process.MyNtuple.doPFxconeDijetJetConstituentsNjets=3
process.MyNtuple.doGenXCone_dijet=True
#process.MyNtuple.doGenxconeDijetJetConstituentsNjets=0


# Do this after setting process.source.fileNames, since we want the ability to override it on the commandline
options = setup_opts()
parse_apply_opts(process, options)

with open('pydump_mc_2018.py', 'w') as f:
    f.write(process.dumpPython())

process.maxEvents = cms.untracked.PSet(input=cms.untracked.int32(10000)) 
