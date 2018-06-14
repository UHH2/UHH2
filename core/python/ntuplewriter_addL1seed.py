import FWCore.ParameterSet.Config as cms

isDebug = False
#useData = True
useData = False

met_sources_GL = cms.vstring("slimmedMETs", "slimmedMETsPuppi")

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
    'pfDeepCSVJetTags:probb',
    'pfDeepCSVJetTags:probbb',
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
#process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(10)
process.options = cms.untracked.PSet(
    wantSummary=cms.untracked.bool(False),
    # wantSummary=cms.untracked.bool(True),
    # numberOfThreads = cms.untracked.uint32(8), # if running crab jobs, you must set this to agree with numCores
    numberOfStreams = cms.untracked.uint32(0) # 0 = use number of threads; to set use -n
)

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
#                                '/store/data/Run2017F/JetHT/MINIAOD/17Nov2017-v1/70000/F0F5EBA3-28E1-E711-85AA-44A842CFCA00.root'
#                                '/store/data/Run2017F/JetHT/MINIAOD/31Mar2018-v1/30000/54B9283C-C637-E811-A002-B496910A0554.root'
                                '/store/mc/RunIIFall17MiniAOD/QCD_Pt-15to7000_TuneCP5_Flat2017_13TeV_pythia8/MINIAODSIM/94X_mc2017_realistic_v10-v1/70000/FEA67458-19DB-E711-9D44-0242AC130002.root'
                            ]),
                            skipEvents=cms.untracked.uint32(0)
                            )
process.maxEvents = cms.untracked.PSet(input=cms.untracked.int32(1000))
#process.maxEvents = cms.untracked.PSet(input=cms.untracked.int32(50))
# process.maxEvents = cms.untracked.PSet(input=cms.untracked.int32(10))

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
                               outputCommands=MINIAODSIMEventContent.outputCommands
                               )

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
    pfCandidatesTag='packedPFCandidates'
)


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
                                       'keep 11 <= abs(pdgId) <= 16 && numberOfMothers()==1 && abs(mother().pdgId()) >= 23 && abs(mother().pdgId()) <= 25',
                                       'keep 11 <= abs(pdgId) <= 16 && numberOfMothers()==1 && abs(mother().pdgId()) == 6',
                                       'keep 11 <= abs(pdgId) <= 16 && numberOfMothers()==1 && abs(mother().pdgId()) == 42'
                                   )
                                   )
task.add(process.prunedTmp)

process.prunedPrunedGenParticles = cms.EDProducer("GenParticlePruner",
                                                  src=cms.InputTag(
                                                      "prunedTmp"),
                                                  select=cms.vstring(
                                                      'keep *',
                                                      'drop 11 <= abs(pdgId) <= 16 && numberOfMothers() == 1 && abs(mother().pdgId())==6',
                                                      'keep 11 <= abs(pdgId) <= 16 && numberOfMothers() == 1 && abs(mother().pdgId())==6 '
                                                      '&& mother().numberOfDaughters() > 2 && abs(mother().daughter(0).pdgId()) != 24 '
                                                      '&& abs(mother().daughter(1).pdgId()) != 24 && abs(mother().daughter(2).pdgId()) != 24',
                                                  )
                                                  )
task.add(process.prunedPrunedGenParticles)


###############################################
# CHS JETS
#
# configure additional jet collections, based on chs.
process.chs = cms.EDFilter("CandPtrSelector",
                           src=cms.InputTag("packedPFCandidates"),
                           cut=cms.string("fromPV(0) > 0")
                           )
task.add(process.chs)

process.ak8CHSJets = ak8PFJets.clone(
    src='chs',
    doAreaFastjet=True,
    jetPtMin=10.
)
task.add(process.ak8CHSJets)

process.ak8CHSJetsFat = ak8PFJets.clone(
    src='chs',
    doAreaFastjet=True,
    jetPtMin=fatjet_ptmin
)
task.add(process.ak8CHSJetsFat)

# Add HepTopTagger bits
from RecoJets.JetProducers.AnomalousCellParameters_cfi import *
from RecoJets.JetProducers.PFJetParameters_cfi import *
# The HTTTopJetProducer does its own clustering producing a BasicJet of
# Top candidates (i.e. sum of subjects),
# as well as producing PFJet (SubJets), and HTTTopJetTagInfo
# The CA15 jets here are the "original" fatjets going into the HTT, and thus will
# be different from the BasicJet collection that HTTTopJetProducer produces
ca15_clustering_params = dict(
    useExplicitGhosts   = cms.bool(True),
    jetAlgorithm        = cms.string("CambridgeAachen"),
    rParam              = cms.double(1.5),
    **AnomalousCellParameters.parameters_()
)
ca15_clustering_params.update(
    **(PFJetParameters.clone(
        src               = cms.InputTag("chs"),
        doAreaFastjet     = cms.bool(True),
        doRhoFastjet      = cms.bool(False),
        jetPtMin          = cms.double(200.0)
     ).parameters_())
)

process.ca15CHSJets = cms.EDProducer("FastjetJetProducer",
    **ca15_clustering_params
)
task.add(process.ca15CHSJets)

process.hepTopTagCHS = cms.EDProducer("HTTTopJetProducer",
    optimalR            = cms.bool(True),
    qJets               = cms.bool(False),
    minFatjetPt         = cms.double(200.),
    minSubjetPt         = cms.double(0.),
    minCandPt           = cms.double(0.),
    maxFatjetAbsEta     = cms.double(99.),
    subjetMass          = cms.double(30.),
    muCut               = cms.double(0.8),
    filtR               = cms.double(0.3),
    filtN               = cms.int32(5),
    mode                = cms.int32(4),
    minCandMass         = cms.double(0.),
    maxCandMass         = cms.double(999999.),
    massRatioWidth      = cms.double(999999.),
    minM23Cut           = cms.double(0.),
    minM13Cut           = cms.double(0.),
    maxM13Cut           = cms.double(999999.),
    jetCollInstanceName = cms.string("SubJets"),
    algorithm           = cms.int32(1),
    writeCompound       = cms.bool(True),
    **ca15_clustering_params
)
task.add(process.hepTopTagCHS)

#################################################
# Softdrop
#
# Note that the *SoftDrop collections produce the groomed jets (as BasicJets),
# and the subjets (as PFJets, with instance label "SubJets")
# The *SoftDropforsub produce only the groomed jets as PFJets
from RecoJets.Configuration.RecoPFJets_cff import ak8PFJetsCHS

process.ak8CHSJetsSoftDrop = ak8PFJetsCHSSoftDrop.clone(
    src=cms.InputTag('chs'),
    jetPtMin=fatjet_ptmin
)
task.add(process.ak8CHSJetsSoftDrop)

process.ak8CHSJetsSoftDropforsub = process.ak8CHSJetsFat.clone(
    rParam=0.8,
    jetPtMin=fatjet_ptmin,
    zcut=cms.double(0.1),
    beta=cms.double(0.0),
    useSoftDrop=cms.bool(True),
    useExplicitGhosts=cms.bool(True),
    R0=cms.double(0.8)
)
task.add(process.ak8CHSJetsSoftDropforsub)

process.ca15CHSJetsSoftDrop = ak8PFJetsCHSSoftDrop.clone(
    src=cms.InputTag('chs'),
    jetPtMin=process.ca15CHSJets.jetPtMin,
    jetAlgorithm=cms.string("CambridgeAachen"),
    rParam=1.5,
    R0=1.5,
    zcut=cms.double(0.2),
    beta=cms.double(1.0)
)
task.add(process.ca15CHSJetsSoftDrop)

process.ca15CHSJetsSoftDropforsub = process.ca15CHSJets.clone(
    zcut=cms.double(0.1),
    beta=cms.double(0.0),
    useSoftDrop=cms.bool(True),
    useExplicitGhosts=cms.bool(True),
    R0=cms.double(1.5)
)
task.add(process.ca15CHSJetsSoftDropforsub)

#################################################
# Pruning

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
    rParam=0.8,
    doAreaFastjet=True,
    src='chs',
    jetPtMin=70
)
task.add(process.ak8CHSJetsPruned)

# process.ca8CHSJetsPruned = ak4PFJetsPruned.clone(
#     rParam=0.8,
#     jetAlgorithm="CambridgeAachen",
#     doAreaFastjet=True,
#     src='chs',
#     jetPtMin=fatjet_ptmin
# )
# task.add(process.ca8CHSJetsPruned)

# process.ca15CHSJetsPruned = ak4PFJetsPruned.clone(
#     rParam=1.5,
#     jetAlgorithm="CambridgeAachen",
#     doAreaFastjet=True,
#     src='chs',
#     jetPtMin=process.ca15CHSJets.jetPtMin
# )
# task.add(process.ca15CHSJetsPruned)

###############################################
# PUPPI JETS
process.load('CommonTools/PileupAlgos/Puppi_cff')
# e.g. to run on miniAOD
process.puppi.candName = cms.InputTag('packedPFCandidates')
process.puppi.vertexName = cms.InputTag('offlineSlimmedPrimaryVertices')
process.puppi.clonePackedCands = cms.bool(True)
process.puppi.useExistingWeights = cms.bool(True)
task.add(process.puppi)

process.ak8PuppiJets = ak8PFJets.clone(
    src='puppi',
    doAreaFastjet=True,
    jetPtMin=10.
)
task.add(process.ak8PuppiJets)

process.ak8PuppiJetsFat = process.ak8CHSJets.clone(src='puppi')
task.add(process.ak8PuppiJetsFat)

process.ak8PuppiJetsSoftDrop = ak8PFJetsCHSSoftDrop.clone(
    src=cms.InputTag('puppi'),
    jetPtMin=fatjet_ptmin
)
task.add(process.ak8PuppiJetsSoftDrop)

process.ak8PuppiJetsSoftDropforsub = process.ak8CHSJetsFat.clone(
    rParam=0.8,
    jetPtMin=fatjet_ptmin,
    zcut=cms.double(0.1),
    beta=cms.double(0.0),
    useSoftDrop=cms.bool(True),
    useExplicitGhosts=cms.bool(True),
    R0=cms.double(0.8),
    src=cms.InputTag('puppi')
)
task.add(process.ak8PuppiJetsSoftDropforsub)

# process.ca15PuppiJets = process.ca8CHSJets.clone(rParam=1.5, src='puppi')
# task.add(process.ca15PuppiJets)

# process.ca15PuppiJetsSoftDrop = ak8PFJetsCHSSoftDrop.clone(
#     src=cms.InputTag('puppi'),
#     jetPtMin=fatjet_ptmin,
#     jetAlgorithm=cms.string("CambridgeAachen"),
#     rParam=1.5,
#     R0=1.5,
#     zcut=cms.double(0.2),
#     beta=cms.double(1.0)
# )
# task.add(process.ca15PuppiJetsSoftDrop)

# process.ca15PuppiJetsSoftDropforsub = process.ca8CHSJets.clone(
#     rParam=1.5,
#     jetPtMin=fatjet_ptmin,
#     zcut=cms.double(0.2),
#     beta=cms.double(1.0),
#     useSoftDrop=cms.bool(True),
#     useExplicitGhosts=cms.bool(True),
#     R0=cms.double(1.5),
#     src=cms.InputTag('puppi')
# )
# task.add(process.ca15PuppiJetsSoftDropforsub)

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


process.packedGenParticlesForJetsNoNu = cms.EDFilter("CandPtrSelector",
                                                     src=cms.InputTag(
                                                         "packedGenParticles"),
                                                     cut=cms.string(
                                                         "abs(pdgId) != 12 && abs(pdgId) != 14 && abs(pdgId) != 16")
                                                     )
task.add(process.packedGenParticlesForJetsNoNu)


def del_attr_safely(obj, name):
    if hasattr(obj, name):
        delattr(obj, name)


def modify_patjetproducer_for_data(process, producer):
    """Modify a PATJetProducer for use with data i.e. turn off all gen-related parts

    Also delete all the extra producers it adds in

    See PhysicsTools/PatAlgos/python/producersLayer1/jetProducer_cfi.py
    for all available options
    """
    producer.addGenPartonMatch = cms.bool(False)
    producer.embedGenPartonMatch = cms.bool(False)
    del_attr_safely(process, producer.genPartonMatch.value().split(":")[0])
    producer.genPartonMatch = cms.InputTag("")

    producer.addGenJetMatch = cms.bool(False)
    producer.embedGenJetMatch = cms.bool(False)
    del_attr_safely(process, producer.genJetMatch.value().split(":")[0])
    producer.genJetMatch = cms.InputTag("")

    del_attr_safely(process, producer.JetPartonMapSource.value().split(":")[0])
    producer.JetPartonMapSource = cms.InputTag("")

    if (producer.addJetFlavourInfo):
        del_attr_safely(process, producer.JetFlavourInfoSource.value().split(":")[0])
    producer.JetFlavourInfoSource = cms.InputTag("")

    producer.addPartonJetMatch = cms.bool(False)
    producer.partonJetSource = cms.InputTag("NOT_IMPLEMENTED")

    producer.getJetMCFlavour = cms.bool(False)
    producer.addJetFlavourInfo = cms.bool(False)


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
#
# One can also enable the storing of top tagging info. In this case, it will use info from
# groomed_jets_name, and top_tagging must be set True. Additionally, btagging must be set True

def add_fatjets_subjets(process, fatjets_name, groomed_jets_name, jetcorr_label='AK8PFchs', jetcorr_label_subjets='AK4PFchs', genjets_name=None, verbose=True, btagging=True, top_tagging=False):
    rParam = getattr(process, fatjets_name).rParam.value()
    algo_dict = {"CambridgeAachen": "ca", "AntiKt": "ak"}
    algo = algo_dict.get(getattr(process, fatjets_name).jetAlgorithm.value(), None)
    if algo is None:
        raise RuntimeError, "cannot guess jet algo (ca/ak) from jet producer %s", fatjets_name

    if verbose:
        print '* Adding fatjets_subjets for', fatjets_name

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
            print "Adding groomed genjets", groomed_genjets_name
        if not hasattr(process, groomed_genjets_name):
            setattr(process,
                    groomed_genjets_name,
                    groomed_jetproducer.clone(
                        src=cms.InputTag('packedGenParticlesForJetsNoNu'),
                        jetType='GenJet'
                    )
                    )
            task.add(getattr(process, groomed_genjets_name))
        # add for ungroomed jets if not done yet (maybe never used in case
        # ungroomed are not added, but that's ok ..)
        ungroomed_jetproducer = getattr(process, fatjets_name)
        assert ungroomed_jetproducer.type_() == 'FastjetJetProducer'
        ungroomed_genjets_name = genjets_name(fatjets_name)
        if verbose:
            print "Adding ungroomed genjets", ungroomed_genjets_name
        if not hasattr(process, ungroomed_genjets_name):
            setattr(process,
                    ungroomed_genjets_name,
                    ungroomed_jetproducer.clone(
                        src=cms.InputTag('packedGenParticlesForJetsNoNu'),
                        jetType='GenJet'
                    )
                    )
            task.add(getattr(process, ungroomed_genjets_name))

    jetcorr_list = ['L1FastJet', 'L2Relative', 'L3Absolute']
    if useData:
        jetcorr_list.append('L2L3Residual')
    if "puppi" in fatjets_name.lower():
        # Since used for both fat & subjets,
        # assumes both require same level of corrections
        jetcorr_list.remove("L1FastJet")
    if jetcorr_label:
        jetcorr_arg = (jetcorr_label, cms.vstring(jetcorr_list), 'None')
    else:
        jetcorr_arg = None

    # patify ungroomed jets, if not already done:
    ungroomed_patname = 'patJets' + cap(fatjets_name)
    add_ungroomed = not hasattr(process, ungroomed_patname)
    if add_ungroomed:
        if verbose:
            print "Adding ungroomed jets", ungroomed_patname
        addJetCollection(process,
                         labelName=fatjets_name,
                         jetSource=cms.InputTag(fatjets_name),
                         algo=algo,
                         rParam=rParam,
                         jetCorrections=jetcorr_arg,
                         genJetCollection=cms.InputTag(ungroomed_genjets_name),
                         getJetMCFlavour=not useData,
                         **common_btag_parameters
                         )
        getattr(process, ungroomed_patname).addTagInfos = True
        delattr(process, "selectedPatJets"+cap(fatjets_name))

    # patify groomed fat jets, with b-tagging:
    groomed_patname = "patJets" + cap(groomed_jets_name)
    if verbose:
        print "adding groomed jets", groomed_patname
    addJetCollection(process,
                     labelName=groomed_jets_name,
                     jetSource=cms.InputTag(groomed_jets_name),
                     algo=algo,
                     rParam=rParam,
                     jetCorrections=jetcorr_arg,
                     # genJetCollection = cms.InputTag(groomed_genjets_name), #
                     # nice try, but PAT looks for GenJets, whereas jets with
                     # subjets are BasicJets, so PAT cannot be used for this
                     # matching ...
                     genJetCollection=cms.InputTag("slimmedGenJets"),
                     getJetMCFlavour=not useData,
                     **common_btag_parameters
                     )
    getattr(process, groomed_patname).addTagInfos = True
    if top_tagging:
        getattr(process, groomed_patname).tagInfoSources = cms.VInputTag(groomed_jets_name)
    delattr(process, "selectedPatJets"+cap(groomed_jets_name))

    # patify subjets, with subjet b-tagging:
    subjets_patname = "patJets" + cap(subjets_name)
    if verbose:
        print "adding groomed jets' subjets", subjets_patname
    if jetcorr_label_subjets:
        jetcorr_arg = (jetcorr_label_subjets, cms.vstring(jetcorr_list), 'None')
    else:
        jetcorr_arg = None
    addJetCollection(process,
                     labelName=subjets_name,
                     jetSource=cms.InputTag(groomed_jets_name, 'SubJets'),
                     algo=algo,
                     rParam=rParam,
                     jetCorrections=jetcorr_arg,
                     explicitJTA=True,
                     svClustering=True,
                     fatJets=cms.InputTag(fatjets_name),
                     groomedFatJets=cms.InputTag(groomed_jets_name),
                     genJetCollection=cms.InputTag(
                         groomed_genjets_name, 'SubJets'),
                     getJetMCFlavour=not useData,
                     **common_btag_parameters
                     )
    # Always add taginfos to subjets, but possible not to store them,
    # configurable with ntuple writer parameter: subjet_taginfos
    getattr(process, subjets_patname).addTagInfos = True
    delattr(process, "selectedPatJets"+cap(subjets_name))

    # add the merged jet collection which contains the links from fat jets to
    # subjets:
    groomed_packed_name = groomed_patname + 'Packed'
    setattr(process,
            groomed_packed_name,
            cms.EDProducer("BoostedJetMerger",
                           jetSrc=cms.InputTag(groomed_patname),
                           subjetSrc=cms.InputTag(subjets_patname)
                           )
            )
    task.add(getattr(process, groomed_packed_name))

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
        # For data, turn off every gen-related part - can't do this via
        # addJetCollection annoyingly
        if useData:
            modify_patjetproducer_for_data(process, producer)


#add_fatjets_subjets(process, 'ca8CHSJets', 'ca8CHSJetsPruned', genjets_name = lambda s: s.replace('CHS', 'Gen'))
# add_fatjets_subjets(process, 'ca15CHSJets', 'ca15CHSJetsFiltered', genjets_name=lambda s: s.replace('CHS', 'Gen'))
#add_fatjets_subjets(process, 'ca15CHSJets', 'hepTopTagCHS')
#add_fatjets_subjets(process, 'ca8CHSJets', 'cmsTopTagCHS', genjets_name = lambda s: s.replace('CHS', 'Gen'))
add_fatjets_subjets(process, 'ca15CHSJets', 'hepTopTagCHS', top_tagging=True, jetcorr_label=None, jetcorr_label_subjets=None)  # CA JEC don't exist so use nothing
add_fatjets_subjets(process, 'ak8CHSJets', 'ak8CHSJetsSoftDrop',
                    genjets_name=lambda s: s.replace('CHS', 'Gen'))
# add_fatjets_subjets(process, 'ca15CHSJets', 'ca15CHSJetsSoftDrop', genjets_name=lambda s: s.replace('CHS', 'Gen'))
# add_fatjets_subjets(process, 'ca15PuppiJets', 'ca15PuppiJetsSoftDrop', genjets_name=lambda s: s.replace('Puppi', 'Gen'))

add_fatjets_subjets(process, 'ak8PuppiJetsFat', 'ak8PuppiJetsSoftDrop', genjets_name=lambda s: s.replace('Puppi', 'Gen'), jetcorr_label="AK8PFPuppi", jetcorr_label_subjets="AK4PFPuppi")

# B-tagging not needed for pruned jets, they are just used to get the mass
add_fatjets_subjets(process, 'ak8CHSJets', 'ak8CHSJetsPruned',
                    genjets_name=lambda s: s.replace('CHS', 'Gen'), btagging=False)
# add_fatjets_subjets(process, 'ca15CHSJets', 'ca15CHSJetsPruned', jetcorr_label=None, jetcorr_label_subjets=None)  # we only use this to make packed collection for pruned mass
#add_fatjets_subjets(process, 'ca8PuppiJets', 'ca8PuppiJetsPruned', genjets_name = lambda s: s.replace('Puppi', 'Gen'))
#add_fatjets_subjets(process, 'ca15PuppiJets', 'ca15PuppiJetsFiltered', genjets_name = lambda s: s.replace('Puppi', 'Gen'))
#add_fatjets_subjets(process, 'ca8PuppiJets', 'cmsTopTagPuppi', genjets_name = lambda s: s.replace('Puppi', 'Gen'))
#add_fatjets_subjets(process, 'ca15PuppiJets', 'hepTopTagPuppi')
#add_fatjets_subjets(process, 'ca8PuppiJets', 'ca8PuppiJetsSoftDrop')


from PhysicsTools.PatAlgos.tools.pfTools import *
# Adapt primary vertex collection
adaptPVs(process, pvCollection=cms.InputTag('offlineSlimmedPrimaryVertices'))


# Add subjet variables
# For each type of jet, we add ungroomed and groomed quantities
from RecoJets.JetProducers.nJettinessAdder_cfi import Njettiness
from RecoJets.JetProducers.qjetsadder_cfi import QJetsAdder

# AK8 CHS
process.NjettinessAk8CHS = Njettiness.clone(
    src=cms.InputTag("ak8CHSJets"),
    cone=cms.double(0.8)
)
task.add(process.NjettinessAk8CHS)

process.NjettinessAk8SoftDropCHS = Njettiness.clone(
    src=cms.InputTag("ak8CHSJetsSoftDropforsub"),
    Njets=cms.vuint32(1, 2, 3, 4),          # compute 1-, 2-, 3-, 4- subjettiness
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

# CA15 CHS
process.NjettinessCa15CHS = Njettiness.clone(
    src=cms.InputTag("ca15CHSJets"),
    cone=cms.double(1.5),
    R0=cms.double(1.5)
)
task.add(process.NjettinessCa15CHS)

process.NjettinessCa15SoftDropCHS = Njettiness.clone(
    src=cms.InputTag("ca15CHSJetsSoftDropforsub"),
    Njets=cms.vuint32(1, 2, 3, 4),          # compute 1-, 2-, 3-, 4- subjettiness
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
task.add(process.NjettinessCa15SoftDropCHS)

# CA15 PUPPI
# process.NjettinessCa15Puppi = Njettiness.clone(
#     src=cms.InputTag("ca15PuppiJets"),
#     cone=cms.double(1.5),
#     R0=cms.double(1.5)
# )
# task.add(process.NjettinessCa15Puppi)

# process.NjettinessCa15SoftDropPuppi = process.NjettinessCa15SoftDropCHS.clone(
#     src=cms.InputTag("ca15PuppiJetsSoftDropforsub")
# )
# task.add(process.NjettinessCa15SoftDropPuppi)

# AK8 PUPPI
# process.NjettinessAk8Puppi = Njettiness.clone(
#     src=cms.InputTag("ak8PuppiJetsFat"),
#     cone=cms.double(0.8)
# )
# task.add(process.NjettinessAk8Puppi)

process.NjettinessAk8SoftDropPuppi = process.NjettinessAk8SoftDropCHS.clone(
    src=cms.InputTag("ak8PuppiJetsSoftDropforsub")
)
task.add(process.NjettinessAk8SoftDropPuppi)

# AK8 GenJets
# process.NjettinessAk8Gen = Njettiness.clone(
#     src=cms.InputTag("ak8GenJets"),
#     cone=cms.double(0.8)
# )
# task.add(process.NjettinessAk8Gen)

# process.NjettinessAk8SoftDropGen = Njettiness.clone(
#     src=cms.InputTag("ak8GenJetsSoftDrop"),
#     cone=cms.double(0.8)
# )
# task.add(process.NjettinessAk8SoftDropGen)

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

# Add in Energy Correlation Functions for groomed jets only
# The cut is taken from PhysicsTools/PatAlgos/python/slimming/applySubstructure_cff.py
from RecoJets.JetProducers.ECF_cff import ecfNbeta1, ecfNbeta2
process.ECFNbeta1Ak8SoftDropCHS = ecfNbeta1.clone(
    src=cms.InputTag("ak8CHSJetsSoftDropforsub"),
    cuts=cms.vstring('', '', 'pt > 250')
)
task.add(process.ECFNbeta1Ak8SoftDropCHS)

process.ECFNbeta2Ak8SoftDropCHS = ecfNbeta2.clone(
    src=cms.InputTag("ak8CHSJetsSoftDropforsub"),
    cuts=cms.vstring('', '', 'pt > 250')
)
task.add(process.ECFNbeta2Ak8SoftDropCHS)


# process.ECFNbeta1Ak8SoftDropPuppi = ecfNbeta1.clone(
#     src=cms.InputTag("ak8PuppiJetsSoftDropforsub"),
#     cuts=cms.vstring('', '', 'pt > 250')
# )
# task.add(process.ECFNbeta1Ak8SoftDropPuppi)

# process.ECFNbeta2Ak8SoftDropPuppi = ecfNbeta2.clone(
#     src=cms.InputTag("ak8PuppiJetsSoftDropforsub"),
#     cuts=cms.vstring('', '', 'pt > 250')
# )
# task.add(process.ECFNbeta2Ak8SoftDropPuppi)

# Warning, can be very slow
# process.ECFNbeta1CA15SoftDropCHS = ecfNbeta1.clone(
#     src=cms.InputTag("ca15CHSJetsSoftDropforsub"),
#     cuts=cms.vstring('', '', 'pt > 250')
# )
# task.add(process.ECFNbeta1CA15SoftDropCHS)

# process.ECFNbeta2CA15SoftDropCHS = ecfNbeta2.clone(
#     src=cms.InputTag("ca15CHSJetsSoftDropforsub"),
#     cuts=cms.vstring('', '', 'pt > 250')
# )
# task.add(process.ECFNbeta2CA15SoftDropCHS)


# for JEC purposes, cluster AK8 jets but with lower pt (compared to higher
# threshold in miniAOD)
ak8_label = "AK8PFPUPPI"
ak8puppi_patname = 'patJets' + ak8_label
print 'Adding', ak8puppi_patname
addJetCollection(process,
                 labelName=ak8_label,
                 jetSource=cms.InputTag('ak8PuppiJets'),
                 algo='AK',
                 rParam=0.8,
                 genJetCollection=cms.InputTag('slimmedGenJetsAK8'),

                 jetCorrections=('AK8PFPuppi', ['L2Relative', 'L3Absolute'], 'None'),

                 pfCandidates=cms.InputTag('packedPFCandidates'),
                 pvSource=cms.InputTag('offlineSlimmedPrimaryVertices'),
                 svSource=cms.InputTag('slimmedSecondaryVertices'),
                 muSource=cms.InputTag('slimmedMuons'),
                 elSource=cms.InputTag('slimmedElectrons'),
                 getJetMCFlavour=not useData
                 )
delattr(process, "selectedPatJets"+cap(ak8_label))
# For data, turn off every gen-related part - can't do this via
# addJetCollection annoyingly
if useData:
    producer = getattr(process, ak8puppi_patname)
    modify_patjetproducer_for_data(process, producer)

ak8_label = "AK8PFCHS"
ak8chs_patname = 'patJets' + ak8_label
print 'Adding', ak8chs_patname
addJetCollection(process,
                 labelName=ak8_label,
                 jetSource=cms.InputTag('ak8CHSJets'),
                 algo='AK',
                 rParam=0.8,
                 genJetCollection=cms.InputTag('slimmedGenJetsAK8'),
                 jetCorrections=(
                     'AK8PFchs', ['L1FastJet', 'L2Relative', 'L3Absolute'], 'None'),
                 pfCandidates=cms.InputTag('packedPFCandidates'),
                 pvSource=cms.InputTag('offlineSlimmedPrimaryVertices'),
                 svSource=cms.InputTag('slimmedSecondaryVertices'),
                 muSource=cms.InputTag('slimmedMuons'),
                 elSource=cms.InputTag('slimmedElectrons'),
                 getJetMCFlavour=not useData
                 )
delattr(process, "selectedPatJets"+cap(ak8_label))
# For data, turn off every gen-related part - can't do this via
# addJetCollection annoyingly
if useData:
    producer = getattr(process, ak8chs_patname)
    modify_patjetproducer_for_data(process, producer)

# Add puppi multiplicity producers
# For each, we have to add a PATPuppiJetSpecificProducer,
# then update the relevant pat::Jet collection using updateJetCollection
for name in ["slimmedJetsPuppi", "patJetsAK8PFPUPPI", "slimmedJetsAK8"]:
    suffix = cap(name)
    update_name = "updatedPatJets"+suffix
    # This is hard coded into NtupleWriterJets - don't change it!
    # (should get user to properly specify it)
    puppi_mult_name = "patPuppiJetSpecificProducer" + update_name
    setattr(process,
            puppi_mult_name,
            cms.EDProducer("PATPuppiJetSpecificProducer",
                src = cms.InputTag(name)
                )
            )
    task.add(getattr(process, puppi_mult_name))

    # produces module called "updatedPatJets"+labelName
    updateJetCollection(
        process,
        labelName = suffix,
        jetSource = cms.InputTag(name),
    )
    getattr(process, update_name).userData.userFloats.src = [
        '%s:puppiMultiplicity' % puppi_mult_name,
        '%s:neutralPuppiMultiplicity' % puppi_mult_name,
        '%s:neutralHadronPuppiMultiplicity' % puppi_mult_name,
        '%s:photonPuppiMultiplicity' % puppi_mult_name,
        '%s:HFHadronPuppiMultiplicity' % puppi_mult_name,
        '%s:HFEMPuppiMultiplicity' % puppi_mult_name
    ]


# Higgs tagging commissioning

from RecoBTag.SecondaryVertex.trackSelection_cff import *

process.pfBoostedDoubleSVTagInfos = cms.EDProducer("BoostedDoubleSVProducer",
                                                   trackSelectionBlock,
                                                   beta=cms.double(1.0),
                                                   R0=cms.double(0.8),
                                                   maxSVDeltaRToJet=cms.double(
                                                       0.7),
                                                   trackPairV0Filter=cms.PSet(
                                                       k0sMassWindow=cms.double(
                                                           0.03)
                                                   ),
                                                   svTagInfos=cms.InputTag(
                                                       "pfInclusiveSecondaryVertexFinderTagInfosAk8CHSJets")
                                                   )
task.add(process.pfBoostedDoubleSVTagInfos)

process.pfBoostedDoubleSVTagInfos.trackSelection.jetDeltaRMax = cms.double(0.8)

# Add subjets from groomed fat jet to its corresponding ungroomed fatjet
process.packedPatJetsAk8CHSJets = cms.EDProducer("JetSubstructurePacker",
    jetSrc = cms.InputTag("patJetsAk8CHSJets"),
    distMax = cms.double(0.8),
    algoTags = cms.VInputTag(
        cms.InputTag("patJetsAk8CHSJetsSoftDropPacked")
    ),
    algoLabels = cms.vstring(
        'SoftDropCHS'
    ),
    fixDaughters = cms.bool(False)
)
task.add(process.packedPatJetsAk8CHSJets)

# HOTVR & XCONE
process.hotvrPuppi = cms.EDProducer("HOTVRProducer",
    src=cms.InputTag("puppi")
)
task.add(process.hotvrPuppi)

process.hotvrCHS = cms.EDProducer("HOTVRProducer",
    src=cms.InputTag("chs")
)
task.add(process.hotvrCHS)

usePseudoXCone = cms.bool(True)
process.xconePuppi = cms.EDProducer("XConeProducer",
    src=cms.InputTag("puppi"),
    usePseudoXCone=usePseudoXCone,  # use PseudoXCone (faster) or XCone
    NJets = cms.uint32(2),          # number of fatjets
    RJets = cms.double(1.2),        # cone radius of fatjets
    BetaJets = cms.double(2.0),     # conical mesure (beta = 2.0 is XCone default)
    NSubJets = cms.uint32(3),       # number of subjets in each fatjet
    RSubJets = cms.double(0.4),     # cone radius of subjetSrc
    BetaSubJets = cms.double(2.0)   # conical mesure for subjets
)
task.add(process.xconePuppi)

process.xconeCHS = cms.EDProducer("XConeProducer",
    src=cms.InputTag("chs"),
    usePseudoXCone=usePseudoXCone,  # use PseudoXCone (faster) or XCone
    NJets = cms.uint32(2),          # number of fatjets
    RJets = cms.double(1.2),        # cone radius of fatjets
    BetaJets = cms.double(2.0),     # conical mesure (beta = 2.0 is XCone default)
    NSubJets = cms.uint32(3),       # number of subjets in each fatjet
    RSubJets = cms.double(0.4),     # cone radius of subjetSrc
    BetaSubJets = cms.double(2.0)   # conical mesure for subjets
)
task.add(process.xconeCHS)


# LEPTON cfg

# collections for lepton PF-isolation deposits
process.load('UHH2.core.pfCandidatesByType_cff')
process.load('CommonTools.ParticleFlow.deltaBetaWeights_cff')

# MUON # WILL BE IN MINIAOD OF 9_1_0 RELEASE
from UHH2.core.muon_pfMiniIsolation_cff import *

mu_isovals = []

load_muonPFMiniIso(process, 'muonPFMiniIsoSequenceSTAND', algo = 'STAND',
                   src = 'slimmedMuons',
                   src_charged_hadron = 'pfAllChargedHadrons',
                   src_neutral_hadron = 'pfAllNeutralHadrons',
                   src_photon         = 'pfAllPhotons',
                   src_charged_pileup = 'pfPileUpAllChargedParticles',
                   isoval_list = mu_isovals
                   )

load_muonPFMiniIso(process, 'muonPFMiniIsoSequencePFWGT', algo = 'PFWGT',
                   src = 'slimmedMuons',
                   src_neutral_hadron = 'pfWeightedNeutralHadrons',
                   src_photon         = 'pfWeightedPhotons',
                   isoval_list = mu_isovals
                   )
for m in mu_isovals:
    task.add(getattr(process,m))
    task.add(getattr(process,m.replace("Value","Deposit")))

process.slimmedMuonsUSER = cms.EDProducer('PATMuonUserData',
                                          src = cms.InputTag('slimmedMuons'),
                                          vmaps_double = cms.vstring(mu_isovals),
                                          )
task.add(process.slimmedMuonsUSER)

# ELECTRON

# mini-isolation
# FIXME: should we use the already existing miniIsolatioin in pat::Lepton?
from UHH2.core.electron_pfMiniIsolation_cff import *

el_isovals = []

load_elecPFMiniIso(process,
                   'elecPFMiniIsoSequenceSTAND',
                   algo='STAND',
                   src='slimmedElectrons',
                   src_charged_hadron='pfAllChargedHadrons',
                   src_neutral_hadron='pfAllNeutralHadrons',
                   src_photon='pfAllPhotons',
                   src_charged_pileup='pfPileUpAllChargedParticles',
                   isoval_list=el_isovals
                   )
needed_collections = [
    'convertedPackedPFCandidates',
    'convertedPackedPFCandidatePtrs',
    'pfPileUp',
    'pfNoPileUp',
    'pfAllChargedHadrons',
    'pfAllNeutralHadrons',
    'pfAllPhotons',
    'pfPileUpAllChargedParticles',
    'pfWeightedNeutralHadrons',
    'pfWeightedPhotons',
    'pfAllChargedParticles',
]
for m in needed_collections:
    task.add(getattr(process, m))

load_elecPFMiniIso(process,
                   'elecPFMiniIsoSequencePFWGT',
                   algo='PFWGT',
                   src='slimmedElectrons',
                   src_neutral_hadron='pfWeightedNeutralHadrons',
                   src_photon='pfWeightedPhotons',
                   isoval_list=el_isovals
                   )
for m in el_isovals:
    task.add(getattr(process, m))
    task.add(getattr(process, m.replace('Value', 'Deposit')))


# electron ID from VID
from PhysicsTools.SelectorUtils.tools.vid_id_tools import switchOnVIDElectronIdProducer, setupAllVIDIdsInModule, DataFormat, setupVIDElectronSelection

switchOnVIDElectronIdProducer(process, DataFormat.MiniAOD)

elecID_mod_ls = [
    'RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_Fall17_94X_V1_cff',
    'RecoEgamma.ElectronIdentification.Identification.heepElectronID_HEEPV70_cff',
    'RecoEgamma.ElectronIdentification.Identification.mvaElectronID_Fall17_noIso_V1_cff',
    'RecoEgamma.ElectronIdentification.Identification.mvaElectronID_Fall17_iso_V1_cff',
]

for mod in elecID_mod_ls:
    setupAllVIDIdsInModule(process, mod, setupVIDElectronSelection)

# slimmedElectronsUSER ( = slimmedElectrons + USER variables)
process.slimmedElectronsUSER = cms.EDProducer('PATElectronUserData',
                                              src=cms.InputTag(
                                                  'slimmedElectrons'),

                                              vmaps_bool=cms.PSet(

                                                  cutBasedElectronID_Fall17_94X_V1_veto=cms.InputTag(
                                                      'egmGsfElectronIDs:cutBasedElectronID-Fall17-94X-V1-veto'),
                                                  cutBasedElectronID_Fall17_94X_V1_loose=cms.InputTag(
                                                      'egmGsfElectronIDs:cutBasedElectronID-Fall17-94X-V1-loose'),
                                                  cutBasedElectronID_Fall17_94X_V1_medium=cms.InputTag(
                                                      'egmGsfElectronIDs:cutBasedElectronID-Fall17-94X-V1-medium'),
                                                  cutBasedElectronID_Fall17_94X_V1_tight=cms.InputTag(
                                                      'egmGsfElectronIDs:cutBasedElectronID-Fall17-94X-V1-tight'),
                                                  heepElectronID_HEEPV70=cms.InputTag(
                                                      'egmGsfElectronIDs:heepElectronID-HEEPV70'),
                                                  mvaEleID_Fall17_noIso_V1_wp90=cms.InputTag(
                                                    'egmGsfElectronIDs:mvaEleID-Fall17-noIso-V1-wp90'),
                                                  mvaEleID_Fall17_noIso_V1_wp80=cms.InputTag(
                                                    'egmGsfElectronIDs:mvaEleID-Fall17-noIso-V1-wp80'),
                                                  mvaEleID_Fall17_noIso_V1_wpLoose=cms.InputTag(
                                                    'egmGsfElectronIDs:mvaEleID-Fall17-noIso-V1-wpLoose'),
                                                  mvaEleID_Fall17_iso_V1_wp90=cms.InputTag(
                                                    'egmGsfElectronIDs:mvaEleID-Fall17-iso-V1-wp90'),
                                                  mvaEleID_Fall17_iso_V1_wp80=cms.InputTag(
                                                    'egmGsfElectronIDs:mvaEleID-Fall17-iso-V1-wp80'),
                                                  mvaEleID_Fall17_iso_V1_wpLoose=cms.InputTag(
                                                    'egmGsfElectronIDs:mvaEleID-Fall17-iso-V1-wpLoose'),
                                              ),

                                               vmaps_float = cms.PSet(
                                                   ElectronMVAEstimatorIso = cms.InputTag('electronMVAValueMapProducer:ElectronMVAEstimatorRun2Fall17IsoV1Values'),
                                                   ElectronMVAEstimatorNoIso = cms.InputTag('electronMVAValueMapProducer:ElectronMVAEstimatorRun2Fall17NoIsoV1Values')
                                               ),

                                              vmaps_double=cms.vstring(
                                                  el_isovals),

                                              effAreas_file=cms.FileInPath(
                                                  'RecoEgamma/ElectronIdentification/data/Fall17/effAreaElectrons_cone03_pfNeuHadronsAndPhotons_92X.txt'),
                                              )

process.electronMVAValueMapProducer.src = cms.InputTag("")
process.heepIDVarValueMaps.candVetosAOD = cms.vstring([])
process.heepIDVarValueMaps.candsAOD = cms.VInputTag()
process.heepIDVarValueMaps.ebRecHitsAOD = cms.InputTag("")
process.heepIDVarValueMaps.eeRecHitsAOD = cms.InputTag("")
process.heepIDVarValueMaps.elesAOD = cms.InputTag("") 
task.add(process.egmGsfElectronIDs)
task.add(process.slimmedElectronsUSER)

# NtupleWriter

if useData:
    metfilterpath = "RECO"
else:
    metfilterpath = "PAT"

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
#                                doElectrons=cms.bool(False),
                                electron_source=cms.InputTag(
                                    "slimmedElectronsUSER"),
                                electron_IDtags=cms.vstring(
                                    # keys to be stored in UHH2 Electron class via the tag mechanism:
                                    # each string should correspond to a variable saved
                                    # via the "userInt" method in the pat::Electron collection used 'electron_source'
                                    # [the configuration of the pat::Electron::userInt variables should be done in PATElectronUserData]
                                    'cutBasedElectronID_Fall17_94X_V1_veto',
                                    'cutBasedElectronID_Fall17_94X_V1_loose',
                                    'cutBasedElectronID_Fall17_94X_V1_medium',
                                    'cutBasedElectronID_Fall17_94X_V1_tight',
                                    'heepElectronID_HEEPV70',
                                    'mvaEleID_Fall17_noIso_V1_wp90',
                                    'mvaEleID_Fall17_noIso_V1_wp80',
                                    'mvaEleID_Fall17_noIso_V1_wpLoose',
                                    'mvaEleID_Fall17_iso_V1_wp90',
                                    'mvaEleID_Fall17_iso_V1_wp80',
                                    'mvaEleID_Fall17_iso_V1_wpLoose',
                                ),
                                # #Add variables to trace possible issues with the ECAL slew rate mitigation
                                # #https://twiki.cern.ch/twiki/bin/view/CMSPublic/ReMiniAOD03Feb2017Notes#EGM
                                # doEleAddVars = cms.bool(useData),
                                # dupECALClusters_source = cms.InputTag('particleFlowEGammaGSFixed:dupECALClusters'),
                                # hitsNotReplaced_source = cms.InputTag('ecalMultiAndGSGlobalRecHitEB:hitsNotReplaced'),
                                doEleAddVars=cms.bool(False),

                                doMuons=cms.bool(True),
                                muon_sources=cms.vstring("slimmedMuonsUSER"),

                                doTaus=cms.bool(True),
                                tau_sources=cms.vstring("slimmedTaus"),
                                tau_ptmin=cms.double(0.0),
                                tau_etamax=cms.double(999.0),

                                doPhotons=cms.bool(False),
                                #photon_sources = cms.vstring("selectedPatPhotons"),

                                doJets=cms.bool(True),
                                #jet_sources = cms.vstring("patJetsAk4PFCHS", "patJetsAk8PFCHS", "patJetsCa15CHSJets", "patJetsCa8CHSJets", "patJetsCa15PuppiJets", "patJetsCa8PuppiJets"),
                                jet_sources=cms.vstring(
                                    # "slimmedJets", "slimmedJetsPuppi", ak8puppi_patname, ak8chs_patname),
                                    "slimmedJets", "updatedPatJetsSlimmedJetsPuppi", "updatedPatJetsPatJetsAK8PFPUPPI", ak8chs_patname),
                                jet_ptmin=cms.double(10.0),
                                jet_etamax=cms.double(999.0),

                                doMET=cms.bool(True),
                                met_sources=met_sources_GL,

                                doGenMET=cms.bool(False),

                                # doTopJets=cms.bool(False),
                                doTopJets=cms.bool(True),
                                topjet_ptmin=cms.double(150.0),
                                topjet_etamax=cms.double(5.0),

                                TopJets=cms.VPSet(
                                    cms.PSet(
                                        # Each PSet outputs a TopJet collection, with name {topjet_source}_{subjet_source}
                                        # For each, we store the jets in topjet_source as TopJet objects,
                                        # with its subjets stored depending on subjet_source.
                                        # Tagging info can also be stored, as well as various substructure variables.
                                        # The topjet_source can either be groomed or ungroomed,
                                        # but it determines the pt/eta/phi etc of the TopJet object
                                        topjet_source=cms.string(
                                            # "slimmedJetsAK8"),  # puppi jets in 2017 MiniAOD
                                            "updatedPatJetsSlimmedJetsAK8"),  # puppi jets in 2017 MiniAOD
                                        # For subjet_source, use label "daughters" if you want to store as
                                        # subjets the linked daughters of the topjets (NOT for slimmedJetsAK8 in miniAOD!).
                                        # Otherwise, to store a subjet collection present in miniAOD indicate the
                                        # proper label to be passed to pat::Jet::subjet(...)
                                        # e.g. SoftDropPuppi or CMSTopTag
                                        # If you include "CHS" in this string,
                                        # it will use the matched CHS 4-vector for the *main* jet 4-vector.
                                        subjet_source=cms.string(
                                            "SoftDropPuppi"),
                                        # Specify if you want to store b-tagging taginfos for subjet collection,
                                        # make sure to have included them with .addTagInfos = True
                                        # addTagInfos = True is currently true by default, however,
                                        # only for collections produced and not read directly from miniAOD
                                        # Default is do_subjet_taginfo=False
                                        do_subjet_taginfo=cms.bool(False),
                                        # Note: if you want to store the MVA Higgs tagger discriminator,
                                        # specify the jet collection from which to pick it up and the tagger name
                                        # currently the discriminator is trained on ungroomed jets, so
                                        # the discriminator has to be taken
                                        # from ungroomed jets
                                        higgstag_source=cms.string(
                                            "patJetsAk8PuppiJetsFat"),
                                        higgstag_name=cms.string(
                                            "pfBoostedDoubleSecondaryVertexAK8BJetTags"),
                                        # If empty, njettiness is directly taken from MINIAOD UserFloat
                                        # and added to jets, otherwise taken from the provided source
                                        #njettiness_source = cms.string(""),
                                        #substructure_variables_source = cms.string(""),
                                        njettiness_groomed_source = cms.string(
                                            "NjettinessAk8SoftDropPuppi"),
                                        substructure_groomed_variables_source = cms.string(
                                            "ak8PuppiJetsSoftDropforsub"),
                                        # Note: for slimmedJetsAK8 on miniAOD, the pruned mass is
                                        # available as user float, with label ak8PFJetsCHSPrunedMass.
                                        # Alternatively it is possible to specify another pruned jet collection
                                        # (to be produced here), from which to get it by jet-matching.
                                        # Finally, it is also possible to leave the pruned mass empty with ""
                                        prunedmass_source=cms.string(
                                            "ak8PFJetsCHSValueMap:ak8PFJetsCHSPrunedMass"),
                                        softdropmass_source=cms.string(
                                            "ak8PFJetsPuppiSoftDropMass"),
                                        # switch off qjets for now, as it takes a long time:
                                        #qjets_source = cms.string("QJetsCa8CHS")
                                        # Energy correlation functions, for beta=1 and beta=2
                                        # If blank, will use the ones in the jet userFloat.
                                        # These are assumed to be calculated from the
                                        # substructure_groomed_variables_source
                                        # ecf_beta1_source=cms.string(""),
                                        # ecf_beta2_source=cms.string("")
                                    ),
                                    cms.PSet(
                                        topjet_source=cms.string(
                                            "packedPatJetsAk8CHSJets"),  # store ungroomed vars
                                        subjet_source=cms.string("SoftDropCHS"),
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
                                        ecf_beta1_source=cms.string(
                                            "ECFNbeta1Ak8SoftDropCHS"),
                                        ecf_beta2_source=cms.string(
                                            "ECFNbeta2Ak8SoftDropCHS")
                                    ),
                                    cms.PSet(
                                        # The fat jets that HepTopTag produces are the Top jet candidates,
                                        # i.e. the sum of its subjets. Therefore they will NOT have
                                        # the same pt/eta/phi as normal ca15 jets.
                                        # Unlike the other TopJet collections,
                                        # the pt/eta/phi here is the groomed one, since
                                        # we are primarily interested in the HTTTopJetTagInfo,
                                        # which is only stored for each groomed jet.
                                        topjet_source = cms.string(
                                            "patJetsHepTopTagCHSPacked"),
                                        subjet_source = cms.string("daughters"),
                                        do_subjet_taginfo = cms.bool(True),
                                        higgstag_source = cms.string(
                                            "patJetsCa15CHSJets"),
                                        higgstag_name = cms.string(
                                            "pfBoostedDoubleSecondaryVertexAK8BJetTags"),
                                        njettiness_source = cms.string(
                                            "NjettinessCa15CHS"),
                                        substructure_variables_source = cms.string(
                                            "ca15CHSJets"),
                                        njettiness_groomed_source = cms.string(
                                            "NjettinessCa15SoftDropCHS"),
                                        substructure_groomed_variables_source = cms.string(
                                            "ca15CHSJetsSoftDropforsub"),
                                        # Specify the module that makes reco::HTTTopJetTagInfo
                                        toptagging_source = cms.string(
                                            "hepTopTagCHS"),
                                        # prunedmass_source = cms.string(
                                        #     "patJetsCa15CHSJetsPrunedPacked"),
                                        # softdropmass_source  = cms.string(""),
                                        # ecf_beta1_source=cms.string(
                                        #     "ECFNbeta1CA15SoftDropCHS"),
                                        # ecf_beta2_source=cms.string(
                                        #     "ECFNbeta2CA15SoftDropCHS")
                                    ) ,
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
                                    # cms.PSet(
                                    #     topjet_source=cms.string(
                                    #         "patJetsAk8PuppiJetsSoftDropPacked"),
                                    #     subjet_source=cms.string("daughters"),
                                    #     do_subjet_taginfo=cms.bool(True),
                                    #     higgstag_source=cms.string(
                                    #         "patJetsAk8PuppiJetsFat"),
                                    #     higgstag_name=cms.string(
                                    #         "pfBoostedDoubleSecondaryVertexAK8BJetTags"),
                                    #     njettiness_source=cms.string(
                                    #         "NjettinessAk8Puppi"),
                                    #     substructure_variables_source=cms.string(
                                    #         "ak8PuppiJetsFat"),
                                    #     njettiness_groomed_source=cms.string(
                                    #         "NjettinessAk8SoftDropPuppi"),
                                    #     substructure_groomed_variables_source=cms.string(
                                    #         "ak8PuppiJetsSoftDropforsub"),
                                    #     prunedmass_source=cms.string(
                                    #         "patJetsAk8CHSJetsPrunedPacked"),
                                    #     softdropmass_source=cms.string(
                                    #         "patJetsAk8PuppiJetsSoftDropPacked"),
                                    #     ecf_beta1_source=cms.string(
                                    #         "ECFNbeta1Ak8SoftDropPuppi"),
                                    #     ecf_beta2_source=cms.string(
                                    #         "ECFNbeta2Ak8SoftDropPuppi")

                                    # ),

                                ),

                                doTrigger=cms.bool(True),
                                trigger_bits=cms.InputTag(
                                    "TriggerResults", "", "HLT"),
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
                                    #single jet FWD triggers
                                    'hltSinglePFFwdJet40',
                                    'hltSinglePFFwdJet60',
                                    'hltSinglePFFwdJet80',
                                    'hltSinglePFFwdJet140',
                                    'hltSinglePFFwdJet200',
                                    'hltSinglePFFwdJet260',
                                    'hltSinglePFFwdJet320',
                                    'hltSinglePFFwdJet400',
                                    'hltSinglePFFwdJet450',
                                    'hltSinglePFFwdJet500',

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
                                ) if useData else cms.vstring()),

                                #  'hltL3fL1sMu16orMu25L1f0L2f10QL3Filtered45e2p1Q',        # HLT_Mu45_eta2p1_v*
                                #  'hltEle35CaloIdVTGsfTrkIdTGsfDphiFilter',                # HLT_Ele35_CaloIdVT_GsfTrkIdT_PFJet150_PFJet50_v* (electron)
                                #  'hltEle35CaloIdVTGsfTrkIdTDiCentralPFJet50EleCleaned',   # HLT_Ele35_CaloIdVT_GsfTrkIdT_PFJet150_PFJet50_v* (jets)
                                #  'hltEle45CaloIdVTGsfTrkIdTGsfDphiFilter',                # HLT_Ele45_CaloIdVT_GsfTrkIdT_PFJet200_PFJet50_v* (electron)
                                #  'hltEle45CaloIdVTGsfTrkIdTDiCentralPFJet50EleCleaned',   # HLT_Ele45_CaloIdVT_GsfTrkIdT_PFJet200_PFJet50_v* (jets)
                                #  'hltL3crIsoL1sMu25L1f0L2f10QL3f27QL3trkIsoFiltered0p09', # HLT_IsoMu27_v*
                                #  'hltEle27WPLooseGsfTrackIsoFilter',                      # HLT_Ele27_eta2p1_WPLoose_Gsf_v*
                                #),
                                trigger_objects=cms.InputTag(
                                    "slimmedPatTrigger"),

                                #For 2017 data, store prefiring Info
                                doL1seed=cms.bool(True),
#                                doL1seed=cms.bool(False),
                                l1GtSrc = cms.InputTag("gtStage2Digis"),
                                l1EGSrc = cms.InputTag("caloStage2Digis:EGamma"),
                                l1JetSrc = cms.InputTag("caloStage2Digis:Jet"),
                                # *** gen stuff:
                                doGenInfo=cms.bool(not useData),
                                genparticle_source=cms.InputTag(
                                    "prunedPrunedGenParticles"),
                                stablegenparticle_source=cms.InputTag(
                                    "packedGenParticlesForJetsNoNu"),
                                # set to true if you want to store all gen particles, otherwise, only
                                # prunedPrunedGenParticles are stored (see
                                # above)
                                doAllGenParticles=cms.bool(False),
                                doAllGenParticlesPythia8=cms.bool(False),
                                doGenJets=cms.bool(not useData),
                                genjet_sources=cms.vstring(
                                    #"slimmedGenJets", "slimmedGenJetsAK8", "ca15GenJets"),
                                    "slimmedGenJets", "slimmedGenJetsAK8"),
                                genjet_ptmin=cms.double(10.0),
                                genjet_etamax=cms.double(5.0),

                                doGenTopJets=cms.bool(not useData),
                                gentopjet_sources=cms.VInputTag(
                                    cms.InputTag("ak8GenJetsSoftDrop")
                                ),
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

                                # *** HOTVR & XCone stuff
                                doHOTVR=cms.bool(True),
#                                doHOTVR=cms.bool(False),
                                doXCone=cms.bool(True),
#                                doXCone=cms.bool(False),
                                HOTVR_sources=cms.VInputTag(
                                    cms.InputTag("hotvrCHS"),
                                    cms.InputTag("hotvrPuppi")
                                ),
                                XCone_sources=cms.VInputTag(
                                    cms.InputTag("xconeCHS"),
                                    cms.InputTag("xconePuppi")
                                ),

                                doGenHOTVR=cms.bool(not useData),
                                doGenXCone=cms.bool(not useData),
                                )

#process.content = cms.EDAnalyzer("EventContentAnalyzer")



# Note: we run in unscheduled mode, i.e. all modules are run as required;
# just make sure that the electron IDs run before MyNtuple
process.p = cms.Path(
    process.egmGsfElectronIDSequence *
    process.MyNtuple
)

process.p.associate(task)
process.p.associate(process.patAlgosToolsTask)

open('pydump.py', 'w').write(process.dumpPython())
