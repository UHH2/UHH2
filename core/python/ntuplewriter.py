import FWCore.ParameterSet.Config as cms

isDebug = False
useData = True
# if useData:
#     #    met_sources_GL =  cms.vstring("slimmedMETs","slimmedMETsPuppi","slMETsCHS","slimmedMETsMuEGClean","slimmedMETsEGClean","slimmedMETsUncorrected")
#     met_sources_GL = cms.vstring(
#         "slimmedMETs", "slimmedMETsPuppi", "slMETsCHS")
# else:
#     met_sources_GL = cms.vstring(
#         "slimmedMETs", "slimmedMETsPuppi", "slMETsCHS")  # ,"slimmedMETsMuEGClean"
met_sources_GL = cms.vstring("slimmedMETs", "slimmedMETsPuppi", "slMETsCHS")

# minimum pt for the large-R jets (applies for all: vanilla CA8/CA15,
# cmstoptag, heptoptag). Also applied for the corresponding genjets.
fatjet_ptmin = 150.0
# fatjet_ptmin = 10.0 #TEST

bTagDiscriminators = [
    'pfJetProbabilityBJetTags',
    'pfJetBProbabilityBJetTags',
    'pfSimpleSecondaryVertexHighEffBJetTags',
    #'pfSimpleSecondaryVertexHighPurBJetTags',
    'pfCombinedInclusiveSecondaryVertexV2BJetTags',
    'pfCombinedMVAV2BJetTags',
    'pfBoostedDoubleSecondaryVertexAK8BJetTags',
    'pfBoostedDoubleSecondaryVertexCA15BJetTags'
]


bTagInfos = [
    'pfImpactParameterTagInfos', 'pfSecondaryVertexTagInfos', 'pfInclusiveSecondaryVertexFinderTagInfos', 'softPFMuonsTagInfos', 'softPFElectronsTagInfos'
]

process = cms.Process("USER")

task = cms.Task()

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(1000)
#process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(1)
process.options = cms.untracked.PSet(wantSummary=cms.untracked.bool(
    False), allowUnscheduled=cms.untracked.bool(True))
#process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) , allowUnscheduled = cms.untracked.bool(True) )

# DEBUG ----------------
if isDebug:
    process.Timing = cms.Service("Timing",
                                 summaryOnly=cms.untracked.bool(False),
                                 useJobReport=cms.untracked.bool(True)
                                 )

    process.SimpleMemoryCheck = cms.Service("SimpleMemoryCheck",
                                            ignoreTotal=cms.untracked.int32(2),
                                            moduleMemorySummary=cms.untracked.bool(
                                                True)
                                            )

# DEBUG ----------------

process.source = cms.Source("PoolSource",
                            fileNames=cms.untracked.vstring([
                                #'file:////nfs/dust/cms/user/hinzmann/6214AEE4-751A-E711-8645-0025905A6056.root'
                                #            '/store/data/Run2017B/JetHT/MINIAOD/23Jun2017-v1/00000/004DBDB2-C859-E711-8DD0-002590D0B042.root'
                                # '/store/data/Run2017B/JetHT/MINIAOD/22Jun2017-v1/00000/00063668-8858-E711-9C49-001E67792486.root'
                                # '/store/data/Run2017D/JetHT/MINIAOD/PromptReco-v1/000/302/031/00000/24C14AB9-488F-E711-A2D5-02163E019D41.root'
                                '/store/data/Run2017B/JetHT/MINIAOD/17Nov2017-v1/20000/0016BE6B-FACC-E711-88D8-B499BAAC0068.root'
                                # '/store/data/Run2017B/SingleMuon/MINIAOD/PromptReco-v1/000/297/046/00000/32AC3177-7A56-E711-BE34-02163E019D73.root'
                            ]),
                            skipEvents=cms.untracked.uint32(0)
                            )

process.maxEvents = cms.untracked.PSet(input=cms.untracked.int32(5000))

# Grid-control changes:
gc_maxevents = '__MAX_EVENTS__'
gc_skipevents = '__SKIP_EVENTS__'
gc_filenames = '__FILE_NAMES__'

import os
gc_nickname = os.getenv('DATASETNICK')

if gc_nickname is not None:
    useData = not gc_nickname.startswith('MC_')
    process.source.fileNames = map(
        lambda s: s.strip(' "'), gc_filenames.split(','))
    process.source.skipEvents = int(gc_skipevents)
    process.maxEvents.input = int(gc_maxevents)

# process.source.skipEvents = int(30000) #TEST

###############################################
# OUT
from Configuration.EventContent.EventContent_cff import *
process.out = cms.OutputModule("PoolOutputModule",
                               fileName=cms.untracked.string('miniaod.root'),
                               outputCommands=MINIAODSIMEventContent.outputCommands)

process.out.outputCommands.extend([
    'keep *_patJetsAk8CHS*_*_*',
    'keep *_patJetsAk8Puppi*_*_*',
    'keep *_patJetsCa15CHS*_*_*',
    'keep *_NjettinessAk8CHS_*_*',
    'keep *_NjettinessAk8Puppi_*_*',
    'keep *_NjettinessCa15CHS_*_*',
    'keep *_NjettinessCa15SoftDropCHS_*_*',
    #"keep *_patJetsCmsTopTagCHSPacked_*_*",
    #"keep *_patJetsCmsTopTagCHSSubjets_*_*",
    #"keep *_patJetsHepTopTagCHSPacked_*_*",
    #"keep *_patJetsHepTopTagCHSSubjets_*_*",
    "keep *_patJetsAk8CHSJetsSoftDropPacked_*_*",
    "keep *_patJetsAk8CHSJetsSoftDropSubjets_*_*",
    "keep *_patJetsAk8PuppiJetsSoftDropPacked_*_*",
    "keep *_patJetsAk8PuppiJetsSoftDropSubjets_*_*",
    "keep *_patJetsCa15CHSJetsSoftDropPacked_*_*",
    "keep *_patJetsCa15CHSJetsSoftDropSubjets_*_*",
    "keep *_patJetsAk8CHSJetsPrunedPacked_*_*",
    "keep *_patJetsAk8CHSJetsPrunedSubjets_*_*",
    "keep *_prunedPrunedGenParticles_*_*",
    "keep *_egmGsfElectronIDs_*_*"
])

###############################################
# RECO AND GEN SETUP
process.load("Configuration.Geometry.GeometryRecoDB_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
# see
# https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideFrontierConditions
# for latest global tags
process.load(
    'Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
if useData:
    process.GlobalTag.globaltag = '94X_dataRun2_ReReco_EOY17_v2'
else:
    process.GlobalTag.globaltag = '94X_mc2017_realistic_v10'


from RecoJets.Configuration.RecoPFJets_cff import *
from RecoJets.JetProducers.fixedGridRhoProducerFastjet_cfi import *

process.fixedGridRhoFastjetAll = fixedGridRhoFastjetAll.clone(
    pfCandidatesTag='packedPFCandidates')


###############################################
# GEN PARTICLES
#
# The 13TeV samples mainly use Pythia8 for showering, which stores information in another way compared to Pythia6; in particular,
# many intermediate particles are stored such as top quarks or W bosons, which are not required for the analyses and makle the code more complicated.
# Therefore, the 'prunedGenParticles' collection is pruned again; see UHH2/core/python/testgenparticles.py for a test for this pruning
# and more comments.

process.prunedTmp = cms.EDProducer("GenParticlePruner",
                                   src=cms.InputTag("prunedGenParticles"),
                                   select=cms.vstring(
                                       'drop *',
                                       'keep status == 3',
                                       'keep 20 <= status <= 30',
                                       'keep 11 <= abs(pdgId)  <= 16 && numberOfMothers()==1 && abs(mother().pdgId()) >= 23 && abs(mother().pdgId()) <= 25',
                                       'keep 11 <= abs(pdgId)  <= 16 && numberOfMothers()==1 && abs(mother().pdgId()) == 6',
                                       'keep 11 <= abs(pdgId)  <= 16 && numberOfMothers()==1 && abs(mother().pdgId()) == 42'
                                   )
                                   )
task.add(process.prunedTmp)

process.prunedPrunedGenParticles = cms.EDProducer("GenParticlePruner",
                                                  src=cms.InputTag(
                                                      "prunedTmp"),
                                                  select=cms.vstring(
                                                      'keep *',
                                                      'drop 11 <= abs(pdgId) <= 16 && numberOfMothers() == 1 && abs(mother().pdgId())==6',
                                                      'keep 11 <= abs(pdgId) <= 16 && numberOfMothers() == 1 && abs(mother().pdgId())==6 && mother().numberOfDaughters() > 2 && abs(mother().daughter(0).pdgId()) != 24 && abs(mother().daughter(1).pdgId()) != 24 && abs(mother().daughter(2).pdgId()) != 24',
                                                  )
                                                  )
task.add(process.prunedPrunedGenParticles)


###############################################
# CHS JETS
#
# configure additional jet collections, based on chs.
process.chs = cms.EDFilter("CandPtrSelector", src=cms.InputTag(
    "packedPFCandidates"), cut=cms.string("fromPV(0) > 0"))
task.add(process.chs)

# process.ca8CHSJets = ca8PFJets.clone(
# src='chs', doAreaFastjet=True, jetPtMin=fatjet_ptmin)
process.ak8CHSJets = ak8PFJets.clone(
    src='chs', doAreaFastjet=True, jetPtMin=10.)
task.add(process.ak8CHSJets)
process.ak8CHSJetsFat = ak8PFJets.clone(
    src='chs', doAreaFastjet=True, jetPtMin=fatjet_ptmin)
task.add(process.ak8CHSJetsFat)
# process.ca15CHSJets = process.ca8CHSJets.clone(rParam=1.5)

# from RecoJets.JetProducers.ak4PFJetsPruned_cfi import ak4PFJetsPruned
ak4PFJetsPruned = ak4PFJets.clone(
    SubJetParameters,
    usePruning=cms.bool(True),
    useExplicitGhosts=cms.bool(True),
    writeCompound=cms.bool(True),
    jetCollInstanceName=cms.string("SubJets")
)
process.ca8CHSJetsPruned = ak4PFJetsPruned.clone(
    rParam=0.8, jetAlgorithm="CambridgeAachen", doAreaFastjet=True, src='chs', jetPtMin=fatjet_ptmin)

# from RecoJets.JetProducers.ak5PFJetsFiltered_cfi import ak5PFJetsFiltered
# process.ca15CHSJetsFiltered = ak5PFJetsFiltered.clone(
#     src='chs',
#     jetAlgorithm=cms.string("CambridgeAachen"),
#     rParam=cms.double(1.5),
#     writeCompound=cms.bool(True),
#     doAreaFastjet=cms.bool(True),
#     jetPtMin=cms.double(fatjet_ptmin)
# )

from RecoJets.JetProducers.AnomalousCellParameters_cfi import *
from RecoJets.JetProducers.PFJetParameters_cfi import *
from RecoJets.JetProducers.CATopJetParameters_cfi import CATopJetParameters
# process.cmsTopTagCHS = cms.EDProducer(
#    "CATopJetProducer",
#    PFJetParameters.clone( src = cms.InputTag('chs'),
#                           doAreaFastjet = cms.bool(True),
#                           doRhoFastjet = cms.bool(False),
#                           jetPtMin = cms.double(fatjet_ptmin)
#                           ),
#    AnomalousCellParameters,
#    CATopJetParameters.clone( jetCollInstanceName = cms.string("SubJets")),
#    jetAlgorithm = cms.string("CambridgeAachen"),
#    rParam = cms.double(0.8),
#    writeCompound = cms.bool(True)
#)

# process.hepTopTagCHS = process.cmsTopTagCHS.clone(
#    rParam = cms.double(1.5),
#    tagAlgo = cms.int32(2), #2=fastjet heptt
#    muCut = cms.double(0.8),
#    maxSubjetMass = cms.double(30.0),
#    useSubjetMass = cms.bool(False),
#)

# process.hepTopTagCHS = cms.EDProducer(
#        "HTTTopJetProducer",
#         PFJetParameters.clone( src = cms.InputTag('chs'),
#                               doAreaFastjet = cms.bool(True),
#                               doRhoFastjet = cms.bool(False),
#                               jetPtMin = cms.double(fatjet_ptmin)
#                               ),
#        AnomalousCellParameters,
#        optimalR = cms.bool(True),
#        algorithm = cms.int32(1),
#        jetCollInstanceName = cms.string("SubJets"),
#        jetAlgorithm = cms.string("CambridgeAachen"),
#        rParam = cms.double(1.5),
#        mode = cms.int32(4),
#        minFatjetPt = cms.double(fatjet_ptmin),
#        minCandPt = cms.double(0.),
#        minSubjetPt = cms.double(0.),
#        writeCompound = cms.bool(True),
#        minCandMass = cms.double(0.),
#        maxCandMass = cms.double(1000),
#        massRatioWidth = cms.double(100.),
#        minM23Cut = cms.double(0.),
#        minM13Cut = cms.double(0.),
#        maxM13Cut = cms.double(2.))

# also re-do the ak4 jet clustering, as this is much simpler for b-tagging (there does not seem to be a simple way of
# re-running b-tagging on the slimmedJets ...).
#from RecoJets.JetProducers.ak4PFJets_cfi import ak4PFJets
#process.ak4PFCHS = ak4PFJets.clone(src = 'chs')
#process.ak8PFCHS =process.ak4PFCHS.clone(rParam = 0.8)


#################################################
# Softdrop

from RecoJets.Configuration.RecoPFJets_cff import ak8PFJetsCHS
process.ak8CHSJetsSoftDrop = ak8PFJetsCHSSoftDrop.clone(
    src=cms.InputTag('chs'), jetPtMin=fatjet_ptmin)
task.add(process.ak8CHSJetsSoftDrop)
process.ca15CHSJetsSoftDrop = ak8PFJetsCHSSoftDrop.clone(src=cms.InputTag('chs'), jetPtMin=fatjet_ptmin, jetAlgorithm=cms.string(
    "CambridgeAachen"), rParam=1.5, R0=1.5, zcut=cms.double(0.2), beta=cms.double(1.0))
# process.ca15CHSJetsSoftDropforsub = process.ca8CHSJets.clone(rParam=1.5, jetPtMin=fatjet_ptmin, zcut=cms.double(
# 0.2), beta=cms.double(1.0), useSoftDrop=cms.bool(True),
# useExplicitGhosts=cms.bool(True), R0=cms.double(1.5))
process.ak8CHSJetsSoftDropforsub = process.ak8CHSJetsFat.clone(rParam=0.8, jetPtMin=fatjet_ptmin, zcut=cms.double(
    0.1), beta=cms.double(0.0), useSoftDrop=cms.bool(True), useExplicitGhosts=cms.bool(True), R0=cms.double(0.8))
task.add(process.ak8CHSJetsSoftDropforsub)

# from RecoJets.JetProducers.ak4PFJetsPruned_cfi import ak4PFJetsPruned
# Note low pt threshold as jets currently not stored but used just to
# derived pruned mass
ak4PFJetsPruned = ak4PFJets.clone(
    SubJetParameters,
    usePruning=cms.bool(True),
    useExplicitGhosts=cms.bool(True),
    writeCompound=cms.bool(True),
    jetCollInstanceName=cms.string("SubJets")
)

process.ak8CHSJetsPruned = ak4PFJetsPruned.clone(
    rParam=0.8, doAreaFastjet=True, src='chs', jetPtMin=70)
task.add(process.ak8CHSJetsPruned)
process.ca15CHSJetsPruned = ak4PFJetsPruned.clone(
    rParam=1.5, jetAlgorithm="CambridgeAachen", doAreaFastjet=True, src='chs', jetPtMin=70)

###############################################
# PUPPI JETS
process.load('CommonTools/PileupAlgos/Puppi_cff')
# e.g. to run on miniAOD
process.puppi.candName = cms.InputTag('packedPFCandidates')
process.puppi.vertexName = cms.InputTag('offlineSlimmedPrimaryVertices')
process.puppi.clonePackedCands = cms.bool(True)
task.add(process.puppi)

process.ca15PuppiJetsSoftDrop = ak8PFJetsCHSSoftDrop.clone(src=cms.InputTag('puppi'), jetPtMin=fatjet_ptmin, jetAlgorithm=cms.string(
    "CambridgeAachen"), rParam=1.5, R0=1.5, zcut=cms.double(0.2), beta=cms.double(1.0))

process.ak8PuppiJetsSoftDrop = ak8PFJetsCHSSoftDrop.clone(
    src=cms.InputTag('puppi'), jetPtMin=fatjet_ptmin)
task.add(process.ak8PuppiJetsSoftDrop)

# process.ca15PuppiJetsSoftDropforsub = process.ca8CHSJets.clone(rParam=1.5, jetPtMin=fatjet_ptmin, zcut=cms.double(0.2), beta=cms.double(
# 1.0), useSoftDrop=cms.bool(True), useExplicitGhosts=cms.bool(True),
# R0=cms.double(1.5), src=cms.InputTag('puppi'))
process.ak8PuppiJetsSoftDropforsub = process.ak8CHSJetsFat.clone(rParam=0.8, jetPtMin=fatjet_ptmin, zcut=cms.double(
    0.1), beta=cms.double(0.0), useSoftDrop=cms.bool(True), useExplicitGhosts=cms.bool(True), R0=cms.double(0.8), src=cms.InputTag('puppi'))
task.add(process.ak8PuppiJetsSoftDropforsub)

# process.ca15PuppiJets = process.ca8CHSJets.clone(rParam=1.5, src='puppi')

process.ak8PuppiJets = ak8PFJets.clone(
    src='puppi', doAreaFastjet=True, jetPtMin=10.)
task.add(process.ak8PuppiJets)

process.ak8PuppiJetsFat = process.ak8CHSJets.clone(src='puppi')
task.add(process.ak8PuppiJetsFat)

# copy all the jet collections above; just use 'puppi' instead of 'chs' as
# input:
# , 'cmsTopTagCHS', 'hepTopTagCHS']:
# for name in ['ca8CHSJets', 'ca15CHSJets', 'ca8CHSJetsPruned', 'ca15CHSJetsFiltered']:
#     setattr(process, name.replace('CHS', 'Puppi'), getattr(
#         process, name).clone(src=cms.InputTag('puppi')))
for name in ['ca8CHSJetsPruned']:
    setattr(process, name.replace('CHS', 'Puppi'), getattr(
        process, name).clone(src=cms.InputTag('puppi')))


###############################################
# PAT JETS and Gen Jets
#
# 'Patify' the jet collections defined above and also add the corresponding
# genjets.

# captitalize string; needed below to construct pat module names.


def cap(s): return s[0].upper() + s[1:]


from PhysicsTools.PatAlgos.tools.jetTools import *

# process.load('PhysicsTools.PatAlgos.slimming.unpackedTracksAndVertices_cfi')

# common parameters for the addJetCollection function, see below.
common_btag_parameters = dict(
    #trackSource = cms.InputTag('unpackedTracksAndVertices'),
    pfCandidates=cms.InputTag('packedPFCandidates'),
    pvSource=cms.InputTag('offlineSlimmedPrimaryVertices'),
    svSource=cms.InputTag('slimmedSecondaryVertices'),
    muSource=cms.InputTag('slimmedMuons'),
    elSource=cms.InputTag('slimmedElectrons'),
    btagInfos=bTagInfos,
    btagDiscriminators=bTagDiscriminators
)


process.packedGenParticlesForJetsNoNu = cms.EDFilter("CandPtrSelector", src=cms.InputTag(
    "packedGenParticles"), cut=cms.string("abs(pdgId) != 12 && abs(pdgId) != 14 && abs(pdgId) != 16"))
task.add(process.packedGenParticlesForJetsNoNu)

# Add PAT part of fat jets and subjets, and optionally gen jets. Note that the input collections for the groomed PF jets have to be defined elsewhere
# already.
# This method assumes that you follow  certain naming convention. In particular:
# * the jet algorithm ('ca' or 'ak') has to be part of the fatjets_name
# * fatjets_name is an ungroomed jet collection configured in the same job of type FastjetProducer
# * the groomed jets' label is given by groomed_jets_name
# * the subjets are available with the same name as the groomed jets with instance label 'SubJets'
#
# The method will produce several collections, also following a naming convention:
# * a petJets collection for the ungroomed fatjets with name 'patJets' + fatjets_name (unless it already exists)
# * two pat jet collections (one for fat jets and one for subjets) with names
#   - 'patJets' + groomed_jets_name for the fat jets and
#   - 'patJets' + groomed_jets_name + 'Subjets' for the subjets
# * a merged jet collection 'patJets' + groomed_jets_name + 'Packed'
# * in case genjets_name is not None, is must be a function returning the genjet collection name, given the original
#   (reco) jet collection name. In this case, the according genjet producer is added for the groomed and for
#   the ungroomed collection, using that name.
#
# Note that gen jets are produced but genjet *matching* is currently only working for the fat, ungroomed jets,
# and the subjets, but not for the groomed fat jets; this is a restriction
# of PAT.


def add_fatjets_subjets(process, fatjets_name, groomed_jets_name, jetcorr_label='AK8PFchs', jetcorr_label_subjets='AK4PFchs', genjets_name=None, verbose=True, btagging=True):
    rParam = getattr(process, fatjets_name).rParam.value()
    algo = None
    if 'ca' in fatjets_name.lower():
        algo = 'ca'
        assert getattr(process, fatjets_name).jetAlgorithm.value(
        ) == 'CambridgeAachen'
    elif 'ak' in fatjets_name.lower():
        algo = 'ak'
        assert getattr(process, fatjets_name).jetAlgorithm.value() == 'AntiKt'
    else:
        raise RuntimeError, "cannot guess jet algo (ca/ak) from fatjets name %s" % fatjets_name

    subjets_name = groomed_jets_name + 'Subjets'  # e.g. CA8CHSPruned + Subjets

    # add genjet producers, if requested:
    groomed_genjets_name = 'INVALID'
    ungroomed_genjets_name = 'INVALID'

    if genjets_name is not None:
        groomed_jetproducer = getattr(process, groomed_jets_name)
        assert groomed_jetproducer.type_() in ('FastjetJetProducer',
                                               'CATopJetProducer'), "do not know how to construct genjet collection for %s" % repr(groomed_jetproducer)
        groomed_genjets_name = genjets_name(groomed_jets_name)
        if verbose:
            print "Adding groomed genjets ", groomed_genjets_name
        if not hasattr(process, groomed_genjets_name):
            setattr(process, groomed_genjets_name, groomed_jetproducer.clone(
                src=cms.InputTag('packedGenParticlesForJetsNoNu'), jetType='GenJet'))
            task.add(getattr(process, groomed_genjets_name))
        # add for ungroomed jets if not done yet (maybe never used in case
        # ungroomed are not added, but that's ok ..)
        ungroomed_jetproducer = getattr(process, fatjets_name)
        assert ungroomed_jetproducer.type_() == 'FastjetJetProducer'
        ungroomed_genjets_name = genjets_name(fatjets_name)
        if verbose:
            print "Adding ungroomed genjets ", ungroomed_genjets_name
        if not hasattr(process, ungroomed_genjets_name):
            setattr(process, ungroomed_genjets_name, ungroomed_jetproducer.clone(
                src=cms.InputTag('packedGenParticlesForJetsNoNu'), jetType='GenJet'))
            task.add(getattr(process, ungroomed_genjets_name))

    # patify ungroomed jets, if not already done:
    add_ungroomed = not hasattr(process, 'patJets' + cap(fatjets_name))
    jetcorr_list = ['L1FastJet', 'L2Relative', 'L3Absolute']
    if useData:
        jetcorr_list = ['L1FastJet', 'L2Relative',
                        'L3Absolute', 'L2L3Residual']
    if add_ungroomed:
        if verbose:
            print "Adding ungroomed patJets" + cap(fatjets_name)
        addJetCollection(process, labelName=fatjets_name, jetSource=cms.InputTag(fatjets_name), algo=algo, rParam=rParam,
                         jetCorrections=(jetcorr_label, cms.vstring(
                             jetcorr_list), 'None'),
                         genJetCollection=cms.InputTag(ungroomed_genjets_name),
                         **common_btag_parameters
                         )
        getattr(process, "patJets" + cap(fatjets_name)).addTagInfos = True

    # patify groomed fat jets, with b-tagging:
    if verbose:
        print "adding grommed jets patJets" + cap(groomed_jets_name)
    addJetCollection(process, labelName=groomed_jets_name, jetSource=cms.InputTag(groomed_jets_name), algo=algo, rParam=rParam,
                     jetCorrections=(jetcorr_label, cms.vstring(
                         jetcorr_list), 'None'),
                     # genJetCollection = cms.InputTag(groomed_genjets_name), #
                     # nice try, but PAT looks for GenJets, whereas jets with
                     # subjets are BasicJets, so PAT cannot be used for this
                     # matching ...
                     genJetCollection=cms.InputTag("slimmedGenJets"),
                     **common_btag_parameters)
    getattr(process, "patJets" + cap(groomed_jets_name)).addTagInfos = True
    if groomed_jets_name == "hepTopTagCHS":
        getattr(process, "patJets" + cap(groomed_jets_name)).tagInfoSources = cms.VInputTag(
            cms.InputTag('hepTopTagCHS')
        )

    # patify subjets, with subjet b-tagging:
    if verbose:
        print "adding grommed jets' subjets patJets" + cap(subjets_name)
    addJetCollection(process, labelName=subjets_name, jetSource=cms.InputTag(groomed_jets_name, 'SubJets'), algo=algo, rParam=rParam,
                     jetCorrections=(jetcorr_label_subjets,
                                     cms.vstring(jetcorr_list), 'None'),
                     explicitJTA=True,
                     svClustering=True,
                     fatJets=cms.InputTag(fatjets_name), groomedFatJets=cms.InputTag(groomed_jets_name),
                     genJetCollection=cms.InputTag(
                         groomed_genjets_name, 'SubJets'),
                     **common_btag_parameters)
    # Always add taginfos to subjets, but possible not to store them,
    # configurable with ntuple writer parameter: subjet_taginfos
    getattr(process, "patJets" + cap(subjets_name)).addTagInfos = True

    # add the merged jet collection which contains the links from fat jets to
    # subjets:
    setattr(process, 'patJets' + cap(groomed_jets_name) + 'Packed', cms.EDProducer("BoostedJetMerger",
                                                                                   jetSrc=cms.InputTag(
                                                                                       "patJets" + cap(groomed_jets_name)),
                                                                                   subjetSrc=cms.InputTag("patJets" + cap(subjets_name))))
    task.add(getattr(process, 'patJets' + cap(groomed_jets_name) + 'Packed'))

    # adapt all for b-tagging, and switch off some PAT features not supported
    # in miniAOD:
    module_names = [subjets_name, groomed_jets_name]
    if add_ungroomed:
        module_names += [fatjets_name]
    for name in module_names:
        getattr(process, 'patJetPartonMatch' + cap(name)
                ).matched = 'prunedGenParticles'
        producer = getattr(process, 'patJets' + cap(name))
        producer.addJetCharge = False
        producer.addAssociatedTracks = False
        if not btagging:
            producer.addDiscriminators = False
            producer.addBTagInfo = False
            producer.getJetMCFlavour = False
        producer.addGenJetMatch = genjets_name is not None
        # for fat groomed jets, gen jet match and jet flavor is not working, so
        # switch it off:
        if name == groomed_jets_name:
            producer.addGenJetMatch = False
            producer.getJetMCFlavour = False


#add_fatjets_subjets(process, 'ca8CHSJets', 'ca8CHSJetsPruned', genjets_name = lambda s: s.replace('CHS', 'Gen'))
# add_fatjets_subjets(process, 'ca15CHSJets', 'ca15CHSJetsFiltered',
#                     genjets_name=lambda s: s.replace('CHS', 'Gen'))
#add_fatjets_subjets(process, 'ca15CHSJets', 'hepTopTagCHS')
#add_fatjets_subjets(process, 'ca8CHSJets', 'cmsTopTagCHS', genjets_name = lambda s: s.replace('CHS', 'Gen'))
#add_fatjets_subjets(process, 'ca15CHSJets', 'hepTopTagCHS', genjets_name = lambda s: s.replace('CHS', 'Gen'))
add_fatjets_subjets(process, 'ak8CHSJets', 'ak8CHSJetsSoftDrop',
                    genjets_name=lambda s: s.replace('CHS', 'Gen'))
# add_fatjets_subjets(process, 'ca15CHSJets', 'ca15CHSJetsSoftDrop',
#                     genjets_name=lambda s: s.replace('CHS', 'Gen'))
# add_fatjets_subjets(process, 'ca15PuppiJets', 'ca15PuppiJetsSoftDrop',
#                     genjets_name=lambda s: s.replace('Puppi', 'Gen'))
add_fatjets_subjets(process, 'ak8PuppiJetsFat', 'ak8PuppiJetsSoftDrop',
                    genjets_name=lambda s: s.replace('Puppi', 'Gen'))
# B-tagging not needed for pruned jets, they are just used to get the mass
add_fatjets_subjets(process, 'ak8CHSJets', 'ak8CHSJetsPruned',
                    genjets_name=lambda s: s.replace('CHS', 'Gen'), btagging=False)
# add_fatjets_subjets(process, 'ca15CHSJets', 'ca15CHSJetsPruned',
#                     genjets_name=lambda s: s.replace('CHS', 'Gen'), btagging=False)
#add_fatjets_subjets(process, 'ca8PuppiJets', 'ca8PuppiJetsPruned', genjets_name = lambda s: s.replace('Puppi', 'Gen'))
#add_fatjets_subjets(process, 'ca15PuppiJets', 'ca15PuppiJetsFiltered', genjets_name = lambda s: s.replace('Puppi', 'Gen'))
#add_fatjets_subjets(process, 'ca8PuppiJets', 'cmsTopTagPuppi', genjets_name = lambda s: s.replace('Puppi', 'Gen'))
#add_fatjets_subjets(process, 'ca15PuppiJets', 'hepTopTagPuppi')
#add_fatjets_subjets(process, 'ca8PuppiJets', 'ca8PuppiJetsSoftDrop')

# configure PAT for miniAOD:
#process.patJetPartons.particles = 'prunedGenParticles'

from PhysicsTools.PatAlgos.tools.pfTools import *
# Adapt primary vertex collection
adaptPVs(process, pvCollection=cms.InputTag('offlineSlimmedPrimaryVertices'))


# Add subjet variables (on ungroomed jets only!)
from RecoJets.JetProducers.nJettinessAdder_cfi import Njettiness
from RecoJets.JetProducers.qjetsadder_cfi import QJetsAdder

process.NjettinessAk8CHS = Njettiness.clone(
    src=cms.InputTag("ak8CHSJets"), cone=cms.double(0.8))
task.add(process.NjettinessAk8CHS)
process.NjettinessCa15CHS = Njettiness.clone(src=cms.InputTag(
    "ca15CHSJets"), cone=cms.double(1.5), R0=cms.double(1.5))
process.NjettinessCa15SoftDropCHS = Njettiness.clone(
    src=cms.InputTag("ca15CHSJetsSoftDropforsub"),
    Njets=cms.vuint32(1, 2, 3),          # compute 1-, 2-, 3- subjettiness
    # variables for measure definition :
    measureDefinition=cms.uint32(0),  # CMS default is normalized measure
    beta=cms.double(1.0),              # CMS default is 1
    R0=cms.double(1.5),                  # CMS default is jet cone size
    Rcutoff=cms.double(999.0),       # not used by default
    # variables for axes
    # definition :
    axesDefinition=cms.uint32(6),    # CMS default is 1-pass KT axes
    # not used by default
    nPass=cms.int32(999),
    # not used by default
    akAxesR0=cms.double(999.0)
)
process.NjettinessCa15SoftDropPuppi = process.NjettinessCa15SoftDropCHS.clone(
    src=cms.InputTag("ca15PuppiJetsSoftDropforsub"))
process.NjettinessAk8SoftDropCHS = Njettiness.clone(
    src=cms.InputTag("ak8CHSJetsSoftDropforsub"),
    Njets=cms.vuint32(1, 2, 3),          # compute 1-, 2-, 3- subjettiness
    # variables for measure definition :
    measureDefinition=cms.uint32(0),  # CMS default is normalized measure
    beta=cms.double(1.0),              # CMS default is 1
    R0=cms.double(0.8),                  # CMS default is jet cone size
    Rcutoff=cms.double(999.0),       # not used by default
    # variables for axes
    # definition :
    axesDefinition=cms.uint32(6),    # CMS default is 1-pass KT axes
    # not used by default
    nPass=cms.int32(999),
    # not used by default
    akAxesR0=cms.double(999.0)
)
task.add(process.NjettinessAk8SoftDropCHS)
process.NjettinessAk8SoftDropPuppi = process.NjettinessAk8SoftDropCHS.clone(
    src=cms.InputTag("ak8PuppiJetsSoftDropforsub"))
task.add(process.NjettinessAk8SoftDropPuppi)
#process.NjettinessCa15SoftDropCHS = Njettiness.clone(src = cms.InputTag("patJetsCa15CHSJetsSoftDrop"), cone = cms.double(1.5),R0 = cms.double(1.5))
#process.NjettinessCa8Puppi = Njettiness.clone(src = cms.InputTag("patJetsCa8PuppiJets"), cone = cms.double(0.8))
process.NjettinessCa15Puppi = Njettiness.clone(src=cms.InputTag(
    "ca15PuppiJets"), cone=cms.double(1.5), R0=cms.double(1.5))
process.NjettinessAk8Puppi = Njettiness.clone(
    src=cms.InputTag("ak8PuppiJetsFat"), cone=cms.double(0.8))
task.add(process.NjettinessAk8Puppi)
process.NjettinessAk8Gen = Njettiness.clone(
    src=cms.InputTag("ak8GenJets"), cone=cms.double(0.8))
process.NjettinessAk8SoftDropGen = Njettiness.clone(
    src=cms.InputTag("ak8GenJetsSoftDrop"), cone=cms.double(0.8))
"""
process.QJetsCa8CHS = QJetsAdder.clone(src = cms.InputTag("patJetsCa8CHSJets"), jetRad = cms.double(0.8))
process.QJetsCa15CHS = QJetsAdder.clone(src = cms.InputTag("patJetsCa15CHSJets"), jetRad = cms.double(1.5))

process.RandomNumberGeneratorService = cms.Service("RandomNumberGeneratorService",
   QJetsCa8CHS = cms.PSet(
      initialSeed = cms.untracked.uint32(123)
   ),
   QJetsCa15CHS = cms.PSet(
      initialSeed = cms.untracked.uint32(123)
   )
)
"""

# for JEC cluster AK8 jets with lower pt (compare to miniAOD)
addJetCollection(process, labelName='AK8PFPUPPI', jetSource=cms.InputTag('ak8PuppiJets'), algo='AK', rParam=0.8, genJetCollection=cms.InputTag('slimmedGenJetsAK8'), jetCorrections=('AK8PFchs', ['L1FastJet', 'L2Relative', 'L3Absolute'], 'None'), pfCandidates=cms.InputTag('packedPFCandidates'),
                 pvSource=cms.InputTag('offlineSlimmedPrimaryVertices'),
                 svSource=cms.InputTag('slimmedSecondaryVertices'),
                 muSource=cms.InputTag('slimmedMuons'),
                 elSource=cms.InputTag('slimmedElectrons'),
                 getJetMCFlavour=(not useData)
                 )
# manually override parton & genjet matching even though we set
# getJetMCFlavour false...
if useData:
    producer = getattr(process, 'patJetsAK8PFPUPPI')
    producer.addGenPartonMatch = cms.bool(False)
    producer.embedGenJetMatch = cms.bool(False)
    producer.embedGenPartonMatch = cms.bool(False)
    producer.genJetMatch = cms.InputTag("")
    producer.genPartonMatch = cms.InputTag("")
    producer.getJetMCFlavour = cms.bool(False)
    producer.JetFlavourInfoSource = cms.InputTag("")

addJetCollection(process, labelName='AK8PFCHS', jetSource=cms.InputTag('ak8CHSJets'), algo='AK', rParam=0.8, genJetCollection=cms.InputTag('slimmedGenJetsAK8'), jetCorrections=('AK8PFchs', ['L1FastJet', 'L2Relative', 'L3Absolute'], 'None'), pfCandidates=cms.InputTag('packedPFCandidates'),
                 pvSource=cms.InputTag('offlineSlimmedPrimaryVertices'),
                 svSource=cms.InputTag('slimmedSecondaryVertices'),
                 muSource=cms.InputTag('slimmedMuons'),
                 elSource=cms.InputTag('slimmedElectrons'),
                 getJetMCFlavour=(not useData)
                 )
if useData:
    producer = getattr(process, 'patJetsAK8PFCHS')
    producer.addGenPartonMatch = cms.bool(False)
    producer.embedGenJetMatch = cms.bool(False)
    producer.embedGenPartonMatch = cms.bool(False)
    producer.genJetMatch = cms.InputTag("")
    producer.genPartonMatch = cms.InputTag("")
    producer.getJetMCFlavour = cms.bool(False)
    producer.JetFlavourInfoSource = cms.InputTag("")


# Higgs tagging commissioning

from RecoBTag.SecondaryVertex.trackSelection_cff import *

process.pfBoostedDoubleSVTagInfos = cms.EDProducer("BoostedDoubleSVProducer",
                                                   trackSelectionBlock,
                                                   beta=cms.double(1.0),
                                                   R0=cms.double(0.8),
                                                   maxSVDeltaRToJet=cms.double(
                                                       0.7),
                                                   trackPairV0Filter=cms.PSet(
                                                       k0sMassWindow=cms.double(0.03)),
                                                   svTagInfos=cms.InputTag(
                                                       "pfInclusiveSecondaryVertexFinderTagInfosAk8CHSJets")
                                                   )
task.add(process.pfBoostedDoubleSVTagInfos)

process.pfBoostedDoubleSVTagInfos.trackSelection.jetDeltaRMax = cms.double(0.8)

# MET

# MET CHS (not available as slimmedMET collection)
# copied from
# https://github.com/cms-jet/JMEValidator/blob/CMSSW_7_6_X/python/FrameworkConfiguration.py


def clean_met_(met):
    del met.t01Variation
    del met.t1Uncertainties
    del met.t1SmearedVarsAndUncs
    del met.tXYUncForRaw
    del met.tXYUncForT1
    del met.tXYUncForT01
    del met.tXYUncForT1Smear
    del met.tXYUncForT01Smear
    del met.chsMET  # FIXME: utilise the chsMET part instead of having multiple METs?
    del met.trkMET


from PhysicsTools.PatAlgos.tools.metTools import addMETCollection

# Raw PAT METs
process.load('RecoMET.METProducers.PFMET_cfi')
process.pfMet.src = cms.InputTag('chs')
task.add(process.pfMet)
addMETCollection(process, labelName='patPFMetCHS',
                 metSource='pfMet')  # RAW MET
addMETCollection(process, labelName='patPFMet', metSource='pfMet')  # RAW MET
process.patPFMet.addGenMET = False
process.patPFMetCHS.addGenMET = False
# Slimmed METs
from PhysicsTools.PatAlgos.slimming.slimmedMETs_cfi import slimmedMETs
#### CaloMET is not available in MiniAOD
del slimmedMETs.caloMET
# ### CHS
process.slMETsCHS = slimmedMETs.clone()
process.slMETsCHS.src = cms.InputTag("patPFMetCHS")
process.slMETsCHS.rawUncertainties = cms.InputTag(
    "patPFMetCHS")  # only central value
task.add(process.slMETsCHS)
clean_met_(process.slMETsCHS)

# LEPTON cfg

# collections for lepton PF-isolation deposits
process.load('UHH2.core.pfCandidatesByType_cff')
process.load('CommonTools.ParticleFlow.deltaBetaWeights_cff')

# MUON # WILL BE IN MINIAOD OF 9_1_0 RELEASE
#from UHH2.core.muon_pfMiniIsolation_cff import *

#mu_isovals = []

# load_muonPFMiniIso(process, 'muonPFMiniIsoSequenceSTAND', algo = 'STAND',
#  src = 'slimmedMuons',
#  src_charged_hadron = 'pfAllChargedHadrons',
#  src_neutral_hadron = 'pfAllNeutralHadrons',
#  src_photon         = 'pfAllPhotons',
#  src_charged_pileup = 'pfPileUpAllChargedParticles',
#  isoval_list = mu_isovals
#)
#
# load_muonPFMiniIso(process, 'muonPFMiniIsoSequencePFWGT', algo = 'PFWGT',
#  src = 'slimmedMuons',
#  src_neutral_hadron = 'pfWeightedNeutralHadrons',
#  src_photon         = 'pfWeightedPhotons',
#  isoval_list = mu_isovals
#)
# for m in mu_isovals:
#  task.add(getattr(process,m))
#  task.add(getattr(process,m.replace("Value","Deposit")))
#
# process.slimmedMuonsUSER = cms.EDProducer('PATMuonUserData',
#  src = cms.InputTag('slimmedMuons'),
#  vmaps_double = cms.vstring(mu_isovals),
#)
# task.add(process.slimmedMuonsUSER)

# ELECTRON # WILL BE IN MINIAOD OF 9_1_0 RELEASE

# mini-isolation
#from UHH2.core.electron_pfMiniIsolation_cff import *
#
#el_isovals = []
#
# load_elecPFMiniIso(process, 'elecPFMiniIsoSequenceSTAND', algo = 'STAND',
#  src = 'slimmedElectrons',
#  src_charged_hadron = 'pfAllChargedHadrons',
#  src_neutral_hadron = 'pfAllNeutralHadrons',
#  src_photon         = 'pfAllPhotons',
#  src_charged_pileup = 'pfPileUpAllChargedParticles',
#  isoval_list = el_isovals
#)
# task.add(process.pfAllChargedHadrons)
# task.add(process.pfAllNeutralHadrons)
# task.add(process.pfAllPhotons)
# task.add(process.pfPileUpAllChargedParticles)
# task.add(process.pfPileUp)
#
# load_elecPFMiniIso(process, 'elecPFMiniIsoSequencePFWGT', algo = 'PFWGT',
#  src = 'slimmedElectrons',
#  src_neutral_hadron = 'pfWeightedNeutralHadrons',
#  src_photon         = 'pfWeightedPhotons',
#  isoval_list = el_isovals
#)
# task.add(process.pfWeightedNeutralHadrons)
# task.add(process.pfWeightedPhotons)

# electron ID from VID # WILL BE IN MINIAOD OF 9_1_0 RELEASE
# process.load('RecoEgamma.ElectronIdentification.egmGsfElectronIDs_cff')
#process.electronMVAValueMapProducer.srcMiniAOD = cms.InputTag('slimmedElectrons')
#process.egmGsfElectronIDs.physicsObjectSrc = cms.InputTag('slimmedElectrons')

# elecID_mod_ls = [
#  'RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_Summer16_80X_V1_cff',
#  'RecoEgamma.ElectronIdentification.Identification.cutBasedElectronHLTPreselecition_Summer16_V1_cff',
#  'RecoEgamma.ElectronIdentification.Identification.heepElectronID_HEEPV6_0cff',
#  'RecoEgamma.ElectronIdentification.Identification.mvaElectronID_Spring16_GeneralPurpose_V1_cff',
#  'RecoEgamma.ElectronIdentification.Identification.mvaElectronID_Spring16_HZZ_V1_cff',
#]

#from PhysicsTools.SelectorUtils.tools.vid_id_tools import *
#for mod in elecID_mod_ls: setupAllVIDIdsInModule(process, mod, setupVIDElectronSelection)

# slimmedElectronsUSER ( = slimmedElectrons + USER variables)
# process.slimmedElectronsUSER = cms.EDProducer('PATElectronUserData',
#  src = cms.InputTag('slimmedElectrons'),
#
#  vmaps_bool = cms.PSet(
#
#    cutBasedElectronID_Summer16_80X_V1_veto   = cms.InputTag('egmGsfElectronIDs:cutBasedElectronID-Summer16-80X-V1-veto'),
#    cutBasedElectronID_Summer16_80X_V1_loose  = cms.InputTag('egmGsfElectronIDs:cutBasedElectronID-Summer16-80X-V1-loose'),
#    cutBasedElectronID_Summer16_80X_V1_medium = cms.InputTag('egmGsfElectronIDs:cutBasedElectronID-Summer16-80X-V1-medium'),
#    cutBasedElectronID_Summer16_80X_V1_tight  = cms.InputTag('egmGsfElectronIDs:cutBasedElectronID-Summer16-80X-V1-tight'),
#
#    cutBasedElectronHLTPreselection_Summer16_V1 = cms.InputTag('egmGsfElectronIDs:cutBasedElectronHLTPreselection-Summer16-V1'),
#
#    heepElectronID_HEEPV60                                = cms.InputTag('egmGsfElectronIDs:heepElectronID-HEEPV60'),
#
#  ),
#
#  vmaps_float = cms.PSet(
#    ElectronMVAEstimatorRun2Spring16GeneralPurposeV1Values__user01 = cms.InputTag('electronMVAValueMapProducer:ElectronMVAEstimatorRun2Spring16GeneralPurposeV1Values'),
#    ElectronMVAEstimatorRun2Spring16HZZV1Values__user01 = cms.InputTag('electronMVAValueMapProducer:ElectronMVAEstimatorRun2Spring16HZZV1Values'),
#  ),
#
#  vmaps_double = cms.vstring(el_isovals),
#
#  effAreas_file = cms.FileInPath('RecoEgamma/ElectronIdentification/data/Summer16/effAreaElectrons_cone03_pfNeuHadronsAndPhotons_80X.txt'),
#
#  mva_GeneralPurpose = cms.string('ElectronMVAEstimatorRun2Spring16GeneralPurposeV1Values__user01'),
#  mva_HZZ = cms.string('ElectronMVAEstimatorRun2Spring16HZZV1Values__user01'),
#)


# additional MET filters not given in MiniAOD

# process.load('RecoMET.METFilters.BadPFMuonFilter_cfi')
#process.BadPFMuonFilter.muons = cms.InputTag("slimmedMuons")
#process.BadPFMuonFilter.PFCandidates = cms.InputTag("packedPFCandidates")

# process.load('RecoMET.METFilters.BadChargedCandidateFilter_cfi')
#process.BadChargedCandidateFilter.muons = cms.InputTag("slimmedMuons")
#process.BadChargedCandidateFilter.PFCandidates = cms.InputTag("packedPFCandidates")


# NtupleWriter


isreHLT = False
for x in process.source.fileNames:
    if "reHLT" in x:
        isreHLT = True

triggerpath = "HLT"
if isreHLT:
    triggerpath = "HLT2"

if useData:
    metfilterpath = "RECO"
else:
    metfilterpath = "HLT"

process.MyNtuple = cms.EDFilter('NtupleWriter',
                                # AnalysisModule = cms.PSet(
                                #    name = cms.string("NoopAnalysisModule"),
                                #    library = cms.string("libUHH2examples.so"),
                                #    # Note: all other settings of type string are passed to the module, e.g.:
                                #    TestKey = cms.string("TestValue")
                                #),
                                fileName=cms.string("Ntuple.root"),
                                doPV=cms.bool(True),
                                pv_sources=cms.vstring(
                                    "offlineSlimmedPrimaryVertices"),
                                doRho=cms.untracked.bool(True),
                                rho_source=cms.InputTag(
                                    "fixedGridRhoFastjetAll"),

                                save_lepton_keys=cms.bool(True),

                                doElectrons=cms.bool(True),
                                #doElectrons = cms.bool(False),
                                electron_source=cms.InputTag(
                                    "slimmedElectrons"),
                                electron_IDtags=cms.vstring(
                                    # keys to be stored in UHH2 Electron class via the tag mechanism:
                                    # each string should correspond to a variable saved
                                    # via the "userInt" method in the pat::Electron collection used 'electron_source'
                                    # [the configuration of the pat::Electron::userInt variables should be done in PATElectronUserData]
                                    #'cutBasedElectronID_Summer16_80X_V1_veto', # IDS NOT YET AVAILBLE IN MINIAOD, ADD BACK AFTER 9_1_0 RELEASE
                                    #'cutBasedElectronID_Summer16_80X_V1_loose',
                                    #'cutBasedElectronID_Summer16_80X_V1_medium',
                                    #'cutBasedElectronID_Summer16_80X_V1_tight',
                                    #'cutBasedElectronHLTPreselection_Summer16_V1',
                                    #'heepElectronID_HEEPV60',
                                ),
                                # #Add variables to trace possible issues with the ECAL slew rate mitigation
                                # #https://twiki.cern.ch/twiki/bin/view/CMSPublic/ReMiniAOD03Feb2017Notes#EGM
                                # doEleAddVars = cms.bool(useData),
                                # dupECALClusters_source = cms.InputTag('particleFlowEGammaGSFixed:dupECALClusters'),
                                # hitsNotReplaced_source = cms.InputTag('ecalMultiAndGSGlobalRecHitEB:hitsNotReplaced'),
                                doEleAddVars=cms.bool(False),

                                doMuons=cms.bool(True),
                                muon_sources=cms.vstring("slimmedMuons"),
                                doTaus=cms.bool(True),
                                tau_sources=cms.vstring("slimmedTaus"),
                                tau_ptmin=cms.double(0.0),
                                tau_etamax=cms.double(999.0),
                                doPhotons=cms.bool(False),
                                #photon_sources = cms.vstring("selectedPatPhotons"),

                                doJets=cms.bool(True),
                                #jet_sources = cms.vstring("patJetsAk4PFCHS", "patJetsAk8PFCHS", "patJetsCa15CHSJets", "patJetsCa8CHSJets", "patJetsCa15PuppiJets", "patJetsCa8PuppiJets"),
                                # jet_sources=cms.vstring(
                                #    "slimmedJets", "slimmedJetsPuppi"),
                                jet_sources=cms.vstring(
                                    "slimmedJets", "slimmedJetsPuppi", "patJetsAK8PFPUPPI", "patJetsAK8PFCHS"),
                                jet_ptmin=cms.double(10.0),
                                jet_etamax=cms.double(999.0),

                                doMET=cms.bool(True),
                                #met_sources =  cms.vstring("slimmedMETs","slimmedMETsPuppi","slMETsCHS","slimmedMETsMuEGClean"),
                                met_sources=met_sources_GL,

                                doGenMET=cms.bool(False),

                                doTopJets=cms.bool(False),
                                #doTopJets = cms.bool(True),
                                topjet_ptmin=cms.double(150.0),
                                topjet_etamax=cms.double(5.0),

                                TopJets=cms.VPSet(
                                    cms.PSet(
                                        # THESE ARE PUPPI JETS IN 91X
                                        topjet_source=cms.string(
                                            "slimmedJetsAK8"),
                                        # Note: use label "daughters" for  subjet_source if you want to store as subjets the linked daughters of the topjets (NOT for slimmedJetsAK8 in miniAOD!)
                                        # to store a subjet collection present in miniAOD indicate the
                                        # proper label of the subjets method in pat::Jet: SoftDrop or
                                        # CMSTopTag
                                        subjet_source=cms.string(
                                            "SoftDropPuppi"),
                                        # Specify if you want to store b-tagging taginfos for subjet collection, make sure to have included them with .addTagInfos = True
                                        # addTagInfos = True is currently true by default, however, only for collections produced and not read directly from miniAOD
                                        # Default is do_subjet_taginfo=False
                                        do_subjet_taginfo=cms.bool(False),
                                        # Note: if you want to store the MVA Higgs tagger discriminator, specify the jet collection from which to pick it up and the tagger name
                                        # currently the discriminator is trained on ungroomed jets, so
                                        # the discriminator has to be taken
                                        # from ungroomed jets
                                        higgstag_source=cms.string(
                                            "patJetsAk8PuppiJetsFat"),
                                        higgstag_name=cms.string(
                                            "pfBoostedDoubleSecondaryVertexAK8BJetTags"),
                                        # Note: if empty, njettiness is directly taken from MINIAOD UserFloat and added to jets, otherwise taken from the provided source (for Run II CMSSW_74 ntuples)
                                        #njettiness_source = cms.string(""),
                                        #substructure_variables_source = cms.string(""),
                                        #njettiness_groomed_source = cms.string(""),
                                        #substructure_groomed_variables_source = cms.string(""),
                                        # Note: for slimmedJetsAK8 on miniAOD, the pruned mass is available as user float, with label ak8PFJetsCHSPrunedMass.
                                        # Alternatively it is possible to specify another pruned jet collection (to be produced here), from which to get it by jet-matching.
                                        # Finally, it is also possible to leave the pruned mass empty
                                        # with ""
                                        prunedmass_source=cms.string(
                                            "ak8PFJetsCHSValueMap:ak8PFJetsCHSPrunedMass"),
                                        softdropmass_source=cms.string(
                                            "ak8PFJetsPuppiSoftDropMass"),
                                        # switch off qjets for now, as it takes a long time:
                                        #qjets_source = cms.string("QJetsCa8CHS")
                                    ),
                                    cms.PSet(
                                        topjet_source=cms.string(
                                            "patJetsAk8CHSJetsSoftDropPacked"),
                                        subjet_source=cms.string("daughters"),
                                        do_subjet_taginfo=cms.bool(True),
                                        higgstag_source=cms.string(
                                            "patJetsAk8CHSJets"),
                                        higgstag_name=cms.string(
                                            "pfBoostedDoubleSecondaryVertexAK8BJetTags"),
                                        higgstaginfo_source=cms.string(
                                            "pfBoostedDoubleSVTagInfos"),
                                        njettiness_source=cms.string(
                                            "NjettinessAk8CHS"),
                                        substructure_variables_source=cms.string(
                                            "ak8CHSJets"),
                                        njettiness_groomed_source=cms.string(
                                            "NjettinessAk8SoftDropCHS"),
                                        substructure_groomed_variables_source=cms.string(
                                            "ak8CHSJetsSoftDropforsub"),
                                        prunedmass_source=cms.string(
                                            "patJetsAk8CHSJetsPrunedPacked"),
                                        softdropmass_source=cms.string(
                                            "patJetsAk8CHSJetsSoftDropPacked"),
                                    ),
                                    # cms.PSet(
                                    #    topjet_source = cms.string("patJetsHepTopTagCHSPacked"),
                                    #    subjet_source = cms.string("daughters"),
                                    #    do_subjet_taginfo = cms.bool(True),
                                    #    higgstag_source = cms.string("patJetsCa15CHSJets"),
                                    #    higgstag_name = cms.string("pfBoostedDoubleSecondaryVertexAK8BJetTags"),
                                    #    njettiness_source = cms.string("NjettinessCa15CHS"),
                                    #    substructure_variables_source = cms.string("ca15CHSJets"),
                                    #    njettiness_groomed_source = cms.string("NjettinessCa15SoftDropCHS"),
                                    #    substructure_groomed_variables_source = cms.string("ca15CHSJetsSoftDropforsub"),
                                    #    prunedmass_source = cms.string("patJetsCa15CHSJetsPrunedPacked"),
                                    #    #softdropmass_source  = cms.string(""),
                                    #) ,
                                    # cms.PSet(
                                    #    topjet_source = cms.string("patJetsHepTopTagPuppiPacked"),
                                    #    subjet_source = cms.string("daughters"),
                                    #    do_subjet_taginfo = cms.bool(True),
                                    #    higgstag_source = cms.string("patJetsCa15PuppiJets"),
                                    #    higgstag_name = cms.string("pfBoostedDoubleSecondaryVertexAK8BJetTags"),
                                    #    njettiness_source = cms.string("NjettinessCa15Puppi"),
                                    #    substructure_variables_source = cms.string("ca15PuppiJets"),
                                    #    njettiness_groomed_source = cms.string("NjettinessCa15SoftDropPuppi"),
                                    #    substructure_groomed_variables_source = cms.string("ca15PuppiJetsSoftDropforsub"),
                                    #    prunedmass_source = cms.string("patJetsCa15CHSJetsPrunedPacked"),
                                    #    #softdropmass_source  = cms.string(""),
                                    #) ,
                                    cms.PSet(
                                        topjet_source=cms.string(
                                            "patJetsAk8PuppiJetsSoftDropPacked"),
                                        subjet_source=cms.string("daughters"),
                                        do_subjet_taginfo=cms.bool(True),
                                        higgstag_source=cms.string(
                                            "patJetsAk8PuppiJetsFat"),
                                        higgstag_name=cms.string(
                                            "pfBoostedDoubleSecondaryVertexAK8BJetTags"),
                                        njettiness_source=cms.string(
                                            "NjettinessAk8Puppi"),
                                        substructure_variables_source=cms.string(
                                            "ak8PuppiJetsFat"),
                                        njettiness_groomed_source=cms.string(
                                            "NjettinessAk8SoftDropPuppi"),
                                        substructure_groomed_variables_source=cms.string(
                                            "ak8PuppiJetsSoftDropforsub"),
                                        prunedmass_source=cms.string(
                                            "patJetsAk8CHSJetsPrunedPacked"),
                                        softdropmass_source=cms.string(
                                            "patJetsAk8PuppiJetsSoftDropPacked"),
                                    ),

                                ),

                                doTrigger=cms.bool(True),
                                trigger_bits=cms.InputTag(
                                    "TriggerResults", "", triggerpath),
                                # MET filters (HBHE noise, CSC, etc.) are stored as trigger Bits in
                                # MINIAOD produced in path "PAT"/"RECO" with
                                # prefix "Flag_"
                                metfilter_bits=cms.InputTag(
                                    "TriggerResults", "", metfilterpath),
                                # for now, save all the triggers:
                                trigger_prefixes=cms.vstring("HLT_", "Flag_"),

                                # Give the names of filters for that you want to store the trigger objects that have fired the respecitve trigger
                                # filter paths for a given trigger can be found in https://cmsweb.cern.ch/confdb/
                                # Example: for HLT_Mu45_eta2p1 the last trigger filter is hltL3fL1sMu16orMu25L1f0L2f10QL3Filtered45e2p1Q
                                #          for HLT_Ele35_CaloIdVT_GsfTrkIdT_PFJet150_PFJet50: relevant filters are hltEle35CaloIdVTGsfTrkIdTGsfDphiFilter (last electron filter), hltEle35CaloIdVTGsfTrkIdTDiCentralPFJet50EleCleaned and hltEle35CaloIdVTGsfTrkIdTCentralPFJet150EleCleaned (for the two jets).
                                #          The  filter hltEle35CaloIdVTGsfTrkIdTCentralPFJet150EleCleaned only included redundant objects that are already included in hltEle35CaloIdVTGsfTrkIdTCentralPFJet50EleCleaned.
                                # for HLT_Ele45_CaloIdVT_GsfTrkIdT_PFJet200_PFJet50: relevant filters
                                # are hltEle45CaloIdVTGsfTrkIdTGsfDphiFilter (last electron filter),
                                # hltEle45CaloIdVTGsfTrkIdTDiCentralPFJet50EleCleaned
                                triggerObjects_sources=(cms.vstring(
                                    # single jet trigger
                                    'hltSinglePFJet40',
                                    'hltSinglePFJet60',
                                    'hltSinglePFJet80',
                                    'hltSinglePFJet140',
                                    'hltSinglePFJet200',
                                    'hltSinglePFJet260',
                                    'hltSinglePFJet320',
                                    'hltSinglePFJet400',
                                    'hltSinglePFJet450',
                                    'hltSinglePFJet500',

                                    'hltDiPFJetAve40',
                                    'hltDiPFJetAve60',
                                    'hltDiPFJetAve80',
                                    'hltDiPFJetAve140',
                                    'hltDiPFJetAve200',
                                    'hltDiPFJetAve260',
                                    'hltDiPFJetAve320',
                                    'hltDiPFJetAve400',
                                    'hltDiPFJetAve450',
                                    'hltDiPFJetAve500',

                                    'hltDiPFJetAve15ForHFJEC',
                                    'hltDiPFJetAve25ForHFJEC',
                                    'hltDiPFJetAve35ForHFJEC',
                                    'hltDiPFJetAve60ForHFJEC',
                                    'hltDiPFJetAve80ForHFJEC',
                                    'hltDiPFJetAve100ForHFJEC',
                                    'hltDiPFJetAve160ForHFJEC',
                                    'hltDiPFJetAve220ForHFJEC',
                                    'hltDiPFJetAve300ForHFJEC',
                                ) if useData else ()),

                                #  'hltL3fL1sMu16orMu25L1f0L2f10QL3Filtered45e2p1Q',        # HLT_Mu45_eta2p1_v*
                                #  'hltEle35CaloIdVTGsfTrkIdTGsfDphiFilter',                # HLT_Ele35_CaloIdVT_GsfTrkIdT_PFJet150_PFJet50_v* (electron)
                                #  'hltEle35CaloIdVTGsfTrkIdTDiCentralPFJet50EleCleaned',   # HLT_Ele35_CaloIdVT_GsfTrkIdT_PFJet150_PFJet50_v* (jets)
                                #  'hltEle45CaloIdVTGsfTrkIdTGsfDphiFilter',                # HLT_Ele45_CaloIdVT_GsfTrkIdT_PFJet200_PFJet50_v* (electron)
                                #  'hltEle45CaloIdVTGsfTrkIdTDiCentralPFJet50EleCleaned',   # HLT_Ele45_CaloIdVT_GsfTrkIdT_PFJet200_PFJet50_v* (jets)
                                #  'hltL3crIsoL1sMu25L1f0L2f10QL3f27QL3trkIsoFiltered0p09', # HLT_IsoMu27_v*
                                #  'hltEle27WPLooseGsfTrackIsoFilter',                      # HLT_Ele27_eta2p1_WPLoose_Gsf_v*
                                #),
                                #        trigger_objects = cms.InputTag("selectedPatTrigger"),
                                trigger_objects=cms.InputTag(
                                    "slimmedPatTrigger"),
                                #        trigger_objects = cms.InputTag("patTrigger"),



                                # *** gen stuff:
                                doGenInfo=cms.bool(not useData),
                                genparticle_source=cms.InputTag(
                                    "prunedPrunedGenParticles"),
                                stablegenparticle_source=cms.InputTag(
                                    "packedGenParticles"),
                                # set to true if you want to store all gen particles, otherwise, only
                                # prunedPrunedGenParticles are stored (see
                                # above)
                                doAllGenParticles=cms.bool(False),
                                doAllGenParticlesPythia8=cms.bool(False),
                                doGenJets=cms.bool(not useData),
                                genjet_sources=cms.vstring(
                                    "slimmedGenJets", "slimmedGenJetsAK8", "ca15GenJets"),
                                genjet_ptmin=cms.double(10.0),
                                genjet_etamax=cms.double(5.0),

                                doGenTopJets=cms.bool(not useData),
                                gentopjet_sources=cms.VInputTag(
                                    cms.InputTag("ak8GenJetsSoftDrop")),
                                # gentopjet_sources =
                                # cms.VInputTag(cms.InputTag("ak8GenJets"),cms.InputTag("ak8GenJetsSoftDrop")),
                                # #this can be used to save N-subjettiness for ungroomed GenJets
                                gentopjet_ptmin=cms.double(150.0),
                                gentopjet_etamax=cms.double(5.0),
                                gentopjet_tau1=cms.VInputTag(),
                                gentopjet_tau2=cms.VInputTag(),
                                gentopjet_tau3=cms.VInputTag(),
                                # gentopjet_tau1 = cms.VInputTag(cms.InputTag("NjettinessAk8Gen","tau1"),cms.InputTag("NjettinessAk8SoftDropGen","tau1")), #this can be used to save N-subjettiness for GenJets
                                # gentopjet_tau2 = cms.VInputTag(cms.InputTag("NjettinessAk8Gen","tau2"),cms.InputTag("NjettinessAk8SoftDropGen","tau2")), #this can be used to save N-subjettiness for GenJets
                                # gentopjet_tau3 =
                                # cms.VInputTag(cms.InputTag("NjettinessAk8Gen","tau3"),cms.InputTag("NjettinessAk8SoftDropGen","tau3")),
                                # #this can be used to save N-subjettiness for GenJets

                                doGenJetsWithParts=cms.bool(False),
                                doAllPFParticles=cms.bool(False),
                                pf_collection_source=cms.InputTag(
                                    "packedPFCandidates"),

                                # # *** HOTVR & XCone stuff
                                doHOTVR=cms.bool(True),
                                doXCone=cms.bool(True),
                                doGenHOTVR=cms.bool(not useData),
                                doGenXCone=cms.bool(not useData),
                                # doHOTVR = cms.bool(False),
                                # doXCone = cms.bool(False),
                                # doGenHOTVR = cms.bool(False),
                                # doGenXCone =  cms.bool(False),

                                )

#process.content = cms.EDAnalyzer("EventContentAnalyzer")

# process.load("FWCore.MessageLogger.MessageLogger_cfi")
#process.MessageLogger = cms.Service("MessageLogger")


# Note: we run in unscheduled mode, i.e. all modules are run as required;
# just make sure that MyNtuple runs:


process.p = cms.Path(
    # process.BadPFMuonFilter *
    # process.BadChargedCandidateFilter *
    process.MyNtuple)
process.p.associate(task)
process.p.associate(process.patAlgosToolsTask)

open('pydump.py', 'w').write(process.dumpPython())
