import FWCore.ParameterSet.Config as cms
from UHH2.core.ntuple_generator import generate_process  # use CMSSW type path for CRAB
from UHH2.core.optionsParse import setup_opts, parse_apply_opts


"""NTuple config for 2016 GEN-only MC samples.
We turn off everything except for Gen info & GenJets.
Not that there are no genParticles in the input file either.

You should try and put any centralised changes in generate_process(), not here.
"""


process = generate_process(year="2016v3", useData=False)

# Please do not commit changes to source filenames - used for consistency testing
process.source.fileNames = cms.untracked.vstring([
    'file:/pnfs/desy.de/cms/tier2/store/user/hinzmann/dijetangular/dijet_angular/jobtmp_pythia8_ci_m1500_1900_13000_1_0_0_13TeV_Nov14-0/GEN.root'
])

process.MyNtuple.doRho = cms.untracked.bool(False)
process.MyNtuple.save_lepton_keys = cms.bool(False)
process.MyNtuple.doPV = cms.bool(False)
process.MyNtuple.doElectrons = cms.bool(False)
process.MyNtuple.doMuons = cms.bool(False)
process.MyNtuple.doTaus = cms.bool(False)
process.MyNtuple.doPhotons = cms.bool(False)
process.MyNtuple.doJets = cms.bool(False)
process.MyNtuple.doMET = cms.bool(False)
process.MyNtuple.doGenMET = cms.bool(False)
process.MyNtuple.doTopJets = cms.bool(False)
process.MyNtuple.doTrigger = cms.bool(False)
process.MyNtuple.doL1seed = cms.bool(False)
process.MyNtuple.doEcalBadCalib = cms.bool(False)
process.MyNtuple.doPrefire = cms.bool(False)

# Master switch for geninfo, genparticles, etc
process.MyNtuple.doGenInfo = cms.bool(True)
process.MyNtuple.genparticle_source = cms.InputTag("genparticles")
process.MyNtuple.doStableGenParticles = cms.bool(False)

process.MyNtuple.doGenJets = cms.bool(True)
process.MyNtuple.genjet_sources = cms.vstring("ak4GenJets")
process.MyNtuple.genjet_ptmin = cms.double(10.0)
process.MyNtuple.genjet_etamax = cms.double(5.0)
process.MyNtuple.doGenJetConstituentsNjets = cms.uint32(0) #store constituents for N leading genjets, where N is parameter
process.MyNtuple.doGenJetConstituentsMinJetPt = cms.double(-1)

process.MyNtuple.doGenTopJets = cms.bool(False)

process.MyNtuple.doAllPFParticles = cms.bool(False)

process.MyNtuple.doXCone = cms.bool(False)
process.MyNtuple.doHOTVR = cms.bool(False)
process.MyNtuple.doGenHOTVR = cms.bool(False)
process.MyNtuple.doGenXCone = cms.bool(False)
process.MyNtuple.doXCone_dijet = cms.bool(False)
process.MyNtuple.doGenXCone_dijet = cms.bool(False)

# Remove other modules
rm_modules = ['prefiringweight', 'ecalBadCalibReducedMINIAODFilter', 'BadPFMuonFilter']
for mod in rm_modules:
    if hasattr(process, mod):
        process.p.remove(getattr(process, mod))

# Do this after setting process.source.fileNames, since we want the ability to override it on the commandline
options = setup_opts()
parse_apply_opts(process, options)

with open('pydump_mc_2016v2.py', 'w') as f:
    f.write(process.dumpPython())
