import FWCore.ParameterSet.Config as cms

#isDebug = True
isDebug = False
#useData = False
useData = True
if useData:
    met_sources_GL =  cms.vstring("slimmedMETs","slimmedMETsPuppi","slMETsCHS","slimmedMETsMuEGClean","slimmedMETsEGClean","slimmedMETsUncorrected")
else:
    met_sources_GL =  cms.vstring("slimmedMETs","slimmedMETsPuppi","slMETsCHS","slimmedMETsMuEGClean")

# minimum pt for the large-R jets (applies for all: vanilla CA8/CA15, cmstoptag, heptoptag). Also applied for the corresponding genjets.
fatjet_ptmin = 150.0
#fatjet_ptmin = 10.0 #TEST

bTagDiscriminators = [
    'pfJetProbabilityBJetTags',
    'pfJetBProbabilityBJetTags',
    'pfSimpleSecondaryVertexHighEffBJetTags',
    'pfSimpleSecondaryVertexHighPurBJetTags',
    'pfCombinedInclusiveSecondaryVertexV2BJetTags',
    'pfCombinedMVAV2BJetTags',
    'pfBoostedDoubleSecondaryVertexAK8BJetTags',
    'pfBoostedDoubleSecondaryVertexCA15BJetTags'
]


bTagInfos = [
    'pfImpactParameterTagInfos'
   ,'pfSecondaryVertexTagInfos'
   ,'pfInclusiveSecondaryVertexFinderTagInfos'
   ,'softPFMuonsTagInfos'
   ,'softPFElectronsTagInfos'
]

process = cms.Process("USER")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(1000)
#process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(1)
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) , allowUnscheduled = cms.untracked.bool(True) )
#process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) , allowUnscheduled = cms.untracked.bool(True) )

# DEBUG ----------------
if isDebug:
    process.Timing = cms.Service("Timing",
    summaryOnly = cms.untracked.bool(False),
    useJobReport = cms.untracked.bool(True)
    )

    process.SimpleMemoryCheck = cms.Service("SimpleMemoryCheck",
    ignoreTotal = cms.untracked.int32(2),                                            
    moduleMemorySummary = cms.untracked.bool(True)
    )

# DEBUG ----------------

process.source = cms.Source("PoolSource",
  fileNames  = cms.untracked.vstring([
            '/store/data/Run2016B/SingleElectron/MINIAOD/03Feb2017_ver2-v2/110000/028CD245-EFEA-E611-8A2B-90B11C2801E1.root'
  ]),
  skipEvents = cms.untracked.uint32(0)
)
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(100))
#process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1000))
#process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(10000))
#process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(50000))

# Grid-control changes:
gc_maxevents = '__MAX_EVENTS__'
gc_skipevents = '__SKIP_EVENTS__'
gc_filenames = '__FILE_NAMES__'

import os
gc_nickname = os.getenv('DATASETNICK')

if gc_nickname is not None:
    useData = not gc_nickname.startswith('MC_')
    process.source.fileNames = map(lambda s: s.strip(' "'), gc_filenames.split(','))
    process.source.skipEvents = int(gc_skipevents)
    process.maxEvents.input = int(gc_maxevents)

#process.source.skipEvents = int(30000) #TEST

###############################################
# OUT
from Configuration.EventContent.EventContent_cff import *
process.out = cms.OutputModule("PoolOutputModule",
                               fileName = cms.untracked.string('miniaod.root'),
                               outputCommands = MINIAODSIMEventContent.outputCommands )

process.out.outputCommands.extend([
    'keep *_patJetsAk8CHS*_*_*',
    'keep *_patJetsAk8Puppi*_*_*',
    'keep *_patJetsCa15CHS*_*_*',
    'keep *_NjettinessAk8CHS_*_*',
    'keep *_NjettinessAk8Puppi_*_*',
    'keep *_NjettinessCa15CHS_*_*',
    'keep *_NjettinessCa15SoftDropCHS_*_*',
    "keep *_patJetsCmsTopTagCHSPacked_*_*",
    "keep *_patJetsCmsTopTagCHSSubjets_*_*",
    "keep *_patJetsHepTopTagCHSPacked_*_*",
    "keep *_patJetsHepTopTagCHSSubjets_*_*",
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
#see https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideFrontierConditions for latest global tags
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
if useData:
    process.GlobalTag.globaltag = '80X_dataRun2_2016SeptRepro_v7' 
else:
    process.GlobalTag.globaltag = '80X_mcRun2_asymptotic_2016_TrancheIV_v6' 


from RecoJets.Configuration.RecoPFJets_cff import *
from RecoJets.JetProducers.fixedGridRhoProducerFastjet_cfi import *

process.fixedGridRhoFastjetAll = fixedGridRhoFastjetAll.clone(pfCandidatesTag = 'packedPFCandidates')



###############################################
# GEN PARTICLES
#
# The 13TeV samples mainly use Pythia8 for showering, which stores information in another way compared to Pythia6; in particular,
# many intermediate particles are stored such as top quarks or W bosons, which are not required for the analyses and makle the code more complicated.
# Therefore, the 'prunedGenParticles' collection is pruned again; see UHH2/core/python/testgenparticles.py for a test for this pruning
# and more comments.

process.prunedTmp = cms.EDProducer("GenParticlePruner",
    src = cms.InputTag("prunedGenParticles"),
    select = cms.vstring(
        'drop *',
        'keep status == 3',
        'keep 20 <= status <= 30',
        'keep 11 <= abs(pdgId)  <= 16 && numberOfMothers()==1 && abs(mother().pdgId()) >= 23 && abs(mother().pdgId()) <= 25',
        'keep 11 <= abs(pdgId)  <= 16 && numberOfMothers()==1 && abs(mother().pdgId()) == 6',
        'keep 11 <= abs(pdgId)  <= 16 && numberOfMothers()==1 && abs(mother().pdgId()) == 42'
    )
)

process.prunedPrunedGenParticles = cms.EDProducer("GenParticlePruner",
    src = cms.InputTag("prunedTmp"),
    select = cms.vstring(
        'keep *',
        'drop 11 <= abs(pdgId) <= 16 && numberOfMothers() == 1 && abs(mother().pdgId())==6',  
        'keep 11 <= abs(pdgId) <= 16 && numberOfMothers() == 1 && abs(mother().pdgId())==6 && mother().numberOfDaughters() > 2 && abs(mother().daughter(0).pdgId()) != 24 && abs(mother().daughter(1).pdgId()) != 24 && abs(mother().daughter(2).pdgId()) != 24',
    )
)


###############################################
# CHS JETS
#
# configure additional jet collections, based on chs.
process.chs = cms.EDFilter("CandPtrSelector", src = cms.InputTag("packedPFCandidates"), cut = cms.string("fromPV"))

process.ca8CHSJets  = ca8PFJets.clone (src = 'chs', doAreaFastjet = True, jetPtMin = fatjet_ptmin)
process.ak8CHSJets  = ak8PFJets.clone (src = 'chs', doAreaFastjet = True, jetPtMin = 10.)
process.ak8CHSJetsFat  = ak8PFJets.clone (src = 'chs', doAreaFastjet = True, jetPtMin = fatjet_ptmin)
process.ca15CHSJets = process.ca8CHSJets.clone (rParam = 1.5)

from RecoJets.JetProducers.ak4PFJetsPruned_cfi import ak4PFJetsPruned
process.ca8CHSJetsPruned = ak4PFJetsPruned.clone(rParam = 0.8, jetAlgorithm = "CambridgeAachen", doAreaFastjet = True, src = 'chs', jetPtMin = fatjet_ptmin)

from RecoJets.JetProducers.ak5PFJetsFiltered_cfi import ak5PFJetsFiltered
process.ca15CHSJetsFiltered = ak5PFJetsFiltered.clone(
        src = 'chs',
        jetAlgorithm = cms.string("CambridgeAachen"),
        rParam       = cms.double(1.5),
        writeCompound = cms.bool(True),
        doAreaFastjet = cms.bool(True),
        jetPtMin = cms.double(fatjet_ptmin)
)

from RecoJets.JetProducers.AnomalousCellParameters_cfi import *  
from RecoJets.JetProducers.PFJetParameters_cfi import *          
from RecoJets.JetProducers.CATopJetParameters_cfi import CATopJetParameters
process.cmsTopTagCHS = cms.EDProducer(
    "CATopJetProducer",
    PFJetParameters.clone( src = cms.InputTag('chs'),
                           doAreaFastjet = cms.bool(True),
                           doRhoFastjet = cms.bool(False),
                           jetPtMin = cms.double(fatjet_ptmin)
                           ),
    AnomalousCellParameters,
    CATopJetParameters.clone( jetCollInstanceName = cms.string("SubJets")),
    jetAlgorithm = cms.string("CambridgeAachen"),
    rParam = cms.double(0.8),
    writeCompound = cms.bool(True)
)

#process.hepTopTagCHS = process.cmsTopTagCHS.clone(  
#    rParam = cms.double(1.5),
#    tagAlgo = cms.int32(2), #2=fastjet heptt
#    muCut = cms.double(0.8),
#    maxSubjetMass = cms.double(30.0),
#    useSubjetMass = cms.bool(False),
#)

process.hepTopTagCHS = cms.EDProducer(     
        "HTTTopJetProducer",
         PFJetParameters.clone( src = cms.InputTag('chs'),
                               doAreaFastjet = cms.bool(True),
                               doRhoFastjet = cms.bool(False),
                               jetPtMin = cms.double(fatjet_ptmin)
                               ),   
        AnomalousCellParameters,
        optimalR = cms.bool(True),
        algorithm = cms.int32(1),
        jetCollInstanceName = cms.string("SubJets"),
        jetAlgorithm = cms.string("CambridgeAachen"),
        rParam = cms.double(1.5),
        mode = cms.int32(4),
        minFatjetPt = cms.double(fatjet_ptmin),
        minCandPt = cms.double(0.),
        minSubjetPt = cms.double(0.),
        writeCompound = cms.bool(True),
        minCandMass = cms.double(0.),
        maxCandMass = cms.double(1000),
        massRatioWidth = cms.double(100.),
        minM23Cut = cms.double(0.),
        minM13Cut = cms.double(0.),
        maxM13Cut = cms.double(2.))

# also re-do the ak4 jet clustering, as this is much simpler for b-tagging (there does not seem to be a simple way of
# re-running b-tagging on the slimmedJets ...).
#from RecoJets.JetProducers.ak4PFJets_cfi import ak4PFJets
#process.ak4PFCHS = ak4PFJets.clone(src = 'chs')
#process.ak8PFCHS =process.ak4PFCHS.clone(rParam = 0.8)




#################################################
### Softdrop

from RecoJets.Configuration.RecoPFJets_cff import ak8PFJetsCHS
process.ak8CHSJetsSoftDrop = ak8PFJetsCHSSoftDrop.clone(src = cms.InputTag('chs'), jetPtMin = fatjet_ptmin)
process.ca15CHSJetsSoftDrop = ak8PFJetsCHSSoftDrop.clone(src = cms.InputTag('chs'), jetPtMin = fatjet_ptmin, jetAlgorithm = cms.string("CambridgeAachen"), rParam = 1.5, R0 = 1.5, zcut = cms.double(0.2), beta = cms.double(1.0))
process.ca15CHSJetsSoftDropforsub = process.ca8CHSJets.clone (rParam = 1.5, jetPtMin = fatjet_ptmin, zcut = cms.double(0.2), beta = cms.double(1.0), useSoftDrop = cms.bool(True), useExplicitGhosts = cms.bool(True), R0 = cms.double(1.5))
process.ak8CHSJetsSoftDropforsub = process.ak8CHSJetsFat.clone (rParam = 0.8, jetPtMin = fatjet_ptmin, zcut = cms.double(0.1), beta = cms.double(0.0), useSoftDrop = cms.bool(True), useExplicitGhosts = cms.bool(True), R0 = cms.double(0.8))

from RecoJets.JetProducers.ak4PFJetsPruned_cfi import ak4PFJetsPruned
#Note low pt threshold as jets currently not stored but used just to derived pruned mass
process.ak8CHSJetsPruned = ak4PFJetsPruned.clone(rParam = 0.8, doAreaFastjet = True, src = 'chs', jetPtMin = 70)
process.ca15CHSJetsPruned = ak4PFJetsPruned.clone(rParam = 1.5, jetAlgorithm = "CambridgeAachen", doAreaFastjet = True, src = 'chs', jetPtMin = 70)

###############################################
# PUPPI JETS
process.load('CommonTools/PileupAlgos/Puppi_cff')
## e.g. to run on miniAOD
process.puppi.candName = cms.InputTag('packedPFCandidates')
process.puppi.vertexName = cms.InputTag('offlineSlimmedPrimaryVertices')
process.puppi.clonePackedCands   = cms.bool(True)

process.ca15PuppiJetsSoftDrop = ak8PFJetsCHSSoftDrop.clone(src = cms.InputTag('puppi'), jetPtMin = fatjet_ptmin, jetAlgorithm = cms.string("CambridgeAachen"), rParam = 1.5, R0 = 1.5, zcut = cms.double(0.2), beta = cms.double(1.0))

process.ak8PuppiJetsSoftDrop = ak8PFJetsCHSSoftDrop.clone(src = cms.InputTag('puppi'), jetPtMin = fatjet_ptmin)

process.ca15PuppiJetsSoftDropforsub = process.ca8CHSJets.clone (rParam = 1.5, jetPtMin = fatjet_ptmin, zcut = cms.double(0.2), beta = cms.double(1.0), useSoftDrop = cms.bool(True), useExplicitGhosts = cms.bool(True), R0 = cms.double(1.5), src = cms.InputTag('puppi'))
process.ak8PuppiJetsSoftDropforsub = process.ak8CHSJetsFat.clone (rParam = 0.8, jetPtMin = fatjet_ptmin, zcut = cms.double(0.1), beta = cms.double(0.0), useSoftDrop = cms.bool(True), useExplicitGhosts = cms.bool(True), R0 = cms.double(0.8), src = cms.InputTag('puppi'))

process.ca15PuppiJets = process.ca8CHSJets.clone (rParam = 1.5, src='puppi')

process.ak8PuppiJets  = ak8PFJets.clone (src = 'puppi', doAreaFastjet = True, jetPtMin = 10.)

process.ak8PuppiJetsFat = process.ak8CHSJets.clone (src='puppi')

# copy all the jet collections above; just use 'puppi' instead of 'chs' as input:
for name in ['ca8CHSJets', 'ca15CHSJets', 'ca8CHSJetsPruned', 'ca15CHSJetsFiltered', 'cmsTopTagCHS', 'hepTopTagCHS']:
    setattr(process, name.replace('CHS', 'Puppi'), getattr(process, name).clone(src = cms.InputTag('puppi')))

###############################################
# PAT JETS and Gen Jets
#
# 'Patify' the jet collections defined above and also add the corresponding
# genjets.

# captitalize string; needed below to construct pat module names.
def cap(s): return s[0].upper() + s[1:]

from PhysicsTools.PatAlgos.tools.jetTools import *

#process.load('PhysicsTools.PatAlgos.slimming.unpackedTracksAndVertices_cfi')

# common parameters for the addJetCollection function, see below.
common_btag_parameters = dict(
    #trackSource = cms.InputTag('unpackedTracksAndVertices'),
    pfCandidates = cms.InputTag('packedPFCandidates'),
    pvSource = cms.InputTag('offlineSlimmedPrimaryVertices'),
    svSource = cms.InputTag('slimmedSecondaryVertices'),
    muSource =cms.InputTag( 'slimmedMuons'),
    elSource = cms.InputTag('selectedElectrons'),
    btagInfos = bTagInfos,
    btagDiscriminators = bTagDiscriminators
)


process.packedGenParticlesForJetsNoNu = cms.EDFilter("CandPtrSelector", src = cms.InputTag("packedGenParticles"), cut = cms.string("abs(pdgId) != 12 && abs(pdgId) != 14 && abs(pdgId) != 16"))

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
#  and the subjets, but not for the groomed fat jets; this is a restriction of PAT.
def add_fatjets_subjets(process, fatjets_name, groomed_jets_name, jetcorr_label = 'AK8PFchs', jetcorr_label_subjets = 'AK4PFchs', genjets_name = None, verbose = True, btagging = True):
    rParam = getattr(process, fatjets_name).rParam.value()
    algo = None
    if 'ca' in fatjets_name.lower():
        algo = 'ca'
        assert getattr(process, fatjets_name).jetAlgorithm.value() == 'CambridgeAachen'
    elif 'ak' in fatjets_name.lower():
        algo = 'ak'
        assert getattr(process, fatjets_name).jetAlgorithm.value() == 'AntiKt'
    else:
        raise RuntimeError, "cannot guess jet algo (ca/ak) from fatjets name %s" % fatjets_name
    
    subjets_name = groomed_jets_name + 'Subjets' # e.g. CA8CHSPruned + Subjets
    
    # add genjet producers, if requested:
    groomed_genjets_name = 'INVALID'
    ungroomed_genjets_name = 'INVALID'
    
    if genjets_name is not None:
        groomed_jetproducer = getattr(process, groomed_jets_name)
        assert groomed_jetproducer.type_() in ('FastjetJetProducer', 'CATopJetProducer'), "do not know how to construct genjet collection for %s" % repr(groomed_jetproducer)
        groomed_genjets_name = genjets_name(groomed_jets_name)
        if verbose: print "Adding groomed genjets ", groomed_genjets_name
        setattr(process, groomed_genjets_name, groomed_jetproducer.clone(src = cms.InputTag('packedGenParticlesForJetsNoNu'), jetType = 'GenJet'))
        # add for ungroomed jets if not done yet (maybe never used in case ungroomed are not added, but that's ok ..)
        ungroomed_jetproducer = getattr(process, fatjets_name)
        assert ungroomed_jetproducer.type_() == 'FastjetJetProducer'
        ungroomed_genjets_name = genjets_name(fatjets_name)
        if verbose: print "Adding ungroomed genjets ", ungroomed_genjets_name
        setattr(process, ungroomed_genjets_name, ungroomed_jetproducer.clone(src = cms.InputTag('packedGenParticlesForJetsNoNu'), jetType = 'GenJet'))
        

    # patify ungroomed jets, if not already done:
    add_ungroomed = not hasattr(process, 'patJets' + cap(fatjets_name))
    jetcorr_list = ['L1FastJet', 'L2Relative', 'L3Absolute']
    if useData:
        jetcorr_list = ['L1FastJet', 'L2Relative', 'L3Absolute','L2L3Residual']
    if add_ungroomed:
        if verbose: print "Adding ungroomed patJets" + cap(fatjets_name)
        addJetCollection(process, labelName = fatjets_name, jetSource = cms.InputTag(fatjets_name), algo = algo, rParam = rParam,
            jetCorrections = (jetcorr_label, cms.vstring(jetcorr_list), 'None'),
            genJetCollection = cms.InputTag(ungroomed_genjets_name),
            **common_btag_parameters
        )
        getattr(process,"patJets" + cap(fatjets_name)).addTagInfos = True
    
    # patify groomed fat jets, with b-tagging:
    if verbose: print "adding grommed jets patJets" + cap(groomed_jets_name)
    addJetCollection(process, labelName = groomed_jets_name, jetSource = cms.InputTag(groomed_jets_name), algo = algo, rParam = rParam,
       jetCorrections = (jetcorr_label, cms.vstring(jetcorr_list), 'None'),
       #genJetCollection = cms.InputTag(groomed_genjets_name), # nice try, but PAT looks for GenJets, whereas jets with subjets are BasicJets, so PAT cannot be used for this matching ...
       **common_btag_parameters)
    getattr(process,"patJets" + cap(groomed_jets_name)).addTagInfos = True
    if groomed_jets_name == "hepTopTagCHS":
       getattr(process, "patJets" + cap(groomed_jets_name)).tagInfoSources = cms.VInputTag(
                        cms.InputTag('hepTopTagCHS')
                        )

    # patify subjets, with subjet b-tagging:
    if verbose: print "adding grommed jets' subjets patJets" + cap(subjets_name)
    addJetCollection(process, labelName = subjets_name, jetSource = cms.InputTag(groomed_jets_name, 'SubJets'), algo = algo, rParam = rParam,
        jetCorrections = (jetcorr_label_subjets, cms.vstring(jetcorr_list), 'None'),
        explicitJTA = True,
        svClustering = True,
        fatJets = cms.InputTag(fatjets_name), groomedFatJets = cms.InputTag(groomed_jets_name),
        genJetCollection = cms.InputTag(groomed_genjets_name, 'SubJets'),
        **common_btag_parameters)
        #Always add taginfos to subjets, but possible not to store them, configurable with ntuple writer parameter: subjet_taginfos
    getattr(process,"patJets" + cap(subjets_name)).addTagInfos = True

    # add the merged jet collection which contains the links from fat jets to subjets:
    setattr(process, 'patJets' + cap(groomed_jets_name) + 'Packed',cms.EDProducer("BoostedJetMerger",
        jetSrc=cms.InputTag("patJets" + cap(groomed_jets_name)),
        subjetSrc=cms.InputTag("patJets" + cap(subjets_name))))
        
    # adapt all for b-tagging, and switch off some PAT features not supported in miniAOD:
    module_names = [subjets_name, groomed_jets_name]
    if add_ungroomed: module_names += [fatjets_name]
    for name in module_names:
        getattr(process, 'patJetPartonMatch' + cap(name)).matched = 'prunedGenParticles'
        producer = getattr(process, 'patJets' + cap(name))
        producer.addJetCharge = False
        producer.addAssociatedTracks = False
        if not btagging:
            producer.addDiscriminators = False
            producer.addBTagInfo = False
            producer.getJetMCFlavour = False
        producer.addGenJetMatch = genjets_name is not None
        # for fat groomed jets, gen jet match and jet flavor is not working, so switch it off:
        if name == groomed_jets_name:
            producer.addGenJetMatch = False
            producer.getJetMCFlavour = False

#add_fatjets_subjets(process, 'ca8CHSJets', 'ca8CHSJetsPruned', genjets_name = lambda s: s.replace('CHS', 'Gen'))
add_fatjets_subjets(process, 'ca15CHSJets', 'ca15CHSJetsFiltered',genjets_name = lambda s: s.replace('CHS', 'Gen'))
add_fatjets_subjets(process, 'ca15CHSJets', 'hepTopTagCHS')
#add_fatjets_subjets(process, 'ca8CHSJets', 'cmsTopTagCHS', genjets_name = lambda s: s.replace('CHS', 'Gen'))
#add_fatjets_subjets(process, 'ca15CHSJets', 'hepTopTagCHS', genjets_name = lambda s: s.replace('CHS', 'Gen'))
add_fatjets_subjets(process, 'ak8CHSJets', 'ak8CHSJetsSoftDrop', genjets_name = lambda s: s.replace('CHS', 'Gen'))
add_fatjets_subjets(process, 'ca15CHSJets', 'ca15CHSJetsSoftDrop', genjets_name = lambda s: s.replace('CHS', 'Gen'))
add_fatjets_subjets(process, 'ca15PuppiJets', 'ca15PuppiJetsSoftDrop', genjets_name = lambda s: s.replace('Puppi', 'Gen'))
add_fatjets_subjets(process, 'ak8PuppiJetsFat', 'ak8PuppiJetsSoftDrop', genjets_name = lambda s: s.replace('Puppi', 'Gen'))
#B-tagging not needed for pruned jets, they are just used to get the mass
add_fatjets_subjets(process, 'ak8CHSJets', 'ak8CHSJetsPruned', genjets_name = lambda s: s.replace('CHS', 'Gen'), btagging = False)
add_fatjets_subjets(process, 'ca15CHSJets', 'ca15CHSJetsPruned', genjets_name = lambda s: s.replace('CHS', 'Gen'), btagging = False)
#add_fatjets_subjets(process, 'ca8PuppiJets', 'ca8PuppiJetsPruned', genjets_name = lambda s: s.replace('Puppi', 'Gen'))
#add_fatjets_subjets(process, 'ca15PuppiJets', 'ca15PuppiJetsFiltered', genjets_name = lambda s: s.replace('Puppi', 'Gen'))
#add_fatjets_subjets(process, 'ca8PuppiJets', 'cmsTopTagPuppi', genjets_name = lambda s: s.replace('Puppi', 'Gen'))
add_fatjets_subjets(process, 'ca15PuppiJets', 'hepTopTagPuppi')
#add_fatjets_subjets(process, 'ca8PuppiJets', 'ca8PuppiJetsSoftDrop')

# configure PAT for miniAOD:
#process.patJetPartons.particles = 'prunedGenParticles'

from PhysicsTools.PatAlgos.tools.pfTools import *
## Adapt primary vertex collection
adaptPVs(process, pvCollection = cms.InputTag('offlineSlimmedPrimaryVertices'))


# Add subjet variables (on ungroomed jets only!)
from RecoJets.JetProducers.nJettinessAdder_cfi import Njettiness
from RecoJets.JetProducers.qjetsadder_cfi import QJetsAdder

process.NjettinessAk8CHS = Njettiness.clone(src = cms.InputTag("ak8CHSJets"), cone = cms.double(0.8))
process.NjettinessCa15CHS = Njettiness.clone(src = cms.InputTag("ca15CHSJets"), cone = cms.double(1.5),R0 = cms.double(1.5))
process.NjettinessCa15SoftDropCHS = Njettiness.clone(
                                                 src = cms.InputTag("ca15CHSJetsSoftDropforsub"),
                                                 Njets=cms.vuint32(1,2,3),          # compute 1-, 2-, 3- subjettiness
                                                 # variables for measure definition : 
                                                 measureDefinition = cms.uint32( 0 ), # CMS default is normalized measure
                                                 beta = cms.double(1.0),              # CMS default is 1
                                                 R0 = cms.double(1.5),                  # CMS default is jet cone size
                                                 Rcutoff = cms.double( 999.0),       # not used by default
                                                 # variables for axes definition :
                                                 axesDefinition = cms.uint32( 6 ),    # CMS default is 1-pass KT axes
                                                 nPass = cms.int32(999),             # not used by default
                                                 akAxesR0 = cms.double(999.0)        # not used by default
                                                 )
process.NjettinessCa15SoftDropPuppi = process.NjettinessCa15SoftDropCHS.clone(src = cms.InputTag("ca15PuppiJetsSoftDropforsub"))
process.NjettinessAk8SoftDropCHS = Njettiness.clone(
                                                 src = cms.InputTag("ak8CHSJetsSoftDropforsub"),
                                                 Njets=cms.vuint32(1,2,3),          # compute 1-, 2-, 3- subjettiness
                                                 # variables for measure definition : 
                                                 measureDefinition = cms.uint32( 0 ), # CMS default is normalized measure
                                                 beta = cms.double(1.0),              # CMS default is 1
                                                 R0 = cms.double(0.8),                  # CMS default is jet cone size
                                                 Rcutoff = cms.double( 999.0),       # not used by default
                                                 # variables for axes definition :
                                                 axesDefinition = cms.uint32( 6 ),    # CMS default is 1-pass KT axes
                                                 nPass = cms.int32(999),             # not used by default
                                                 akAxesR0 = cms.double(999.0)        # not used by default
                                                 )
process.NjettinessAk8SoftDropPuppi = process.NjettinessAk8SoftDropCHS.clone(src = cms.InputTag("ak8PuppiJetsSoftDropforsub"))
#process.NjettinessCa15SoftDropCHS = Njettiness.clone(src = cms.InputTag("patJetsCa15CHSJetsSoftDrop"), cone = cms.double(1.5),R0 = cms.double(1.5))
#process.NjettinessCa8Puppi = Njettiness.clone(src = cms.InputTag("patJetsCa8PuppiJets"), cone = cms.double(0.8))
process.NjettinessCa15Puppi = Njettiness.clone(src = cms.InputTag("ca15PuppiJets"), cone = cms.double(1.5),R0 = cms.double(1.5))
process.NjettinessAk8Puppi = Njettiness.clone(src = cms.InputTag("ak8PuppiJetsFat"), cone = cms.double(0.8))
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
addJetCollection(process,labelName = 'AK8PFPUPPI', jetSource = cms.InputTag('ak8PuppiJets'), algo = 'AK', rParam=0.8, genJetCollection=cms.InputTag('slimmedGenJetsAK8'), jetCorrections = ('AK8PFchs', ['L1FastJet', 'L2Relative', 'L3Absolute'], 'None'),pfCandidates = cms.InputTag('packedPFCandidates'),
    pvSource = cms.InputTag('offlineSlimmedPrimaryVertices'),
    svSource = cms.InputTag('slimmedSecondaryVertices'),
    muSource =cms.InputTag( 'slimmedMuons'),
    elSource = cms.InputTag('selectedElectrons')
)
addJetCollection(process,labelName = 'AK8PFCHS', jetSource = cms.InputTag('ak8CHSJets'), algo = 'AK', rParam=0.8, genJetCollection=cms.InputTag('slimmedGenJetsAK8'), jetCorrections = ('AK8PFchs', ['L1FastJet', 'L2Relative', 'L3Absolute'], 'None'),pfCandidates = cms.InputTag('packedPFCandidates'),
    pvSource = cms.InputTag('offlineSlimmedPrimaryVertices'),
    svSource = cms.InputTag('slimmedSecondaryVertices'),
    muSource =cms.InputTag( 'slimmedMuons'),
    elSource = cms.InputTag('selectedElectrons')
)


### MET
# # Re-correct MET on-fly 
# # How to re-correct MET based on e/gamma gain switch correction on the fly for Re-Miniaod Data 
# # https://twiki.cern.ch/twiki/bin/view/CMSPublic/ReMiniAOD03Feb2017Notes#MET_Recipes

## Following lines are for default MET for Type1 corrections.
from PhysicsTools.PatUtils.tools.runMETCorrectionsAndUncertainties import runMetCorAndUncFromMiniAOD
# If you only want to re-correct for JEC and get the proper uncertainties for the default MET
runMetCorAndUncFromMiniAOD(process,
                           isData=useData,
                           )
# Now you are creating the e/g corrected MET on top of the bad muon corrected MET (on re-miniaod)
from PhysicsTools.PatUtils.tools.corMETFromMuonAndEG import corMETFromMuonAndEG
corMETFromMuonAndEG(process,
                    pfCandCollection="", #not needed
                    electronCollection="slimmedElectronsBeforeGSFix",
                    photonCollection="slimmedPhotonsBeforeGSFix",
                    corElectronCollection="selectedElectrons",
                    corPhotonCollection="slimmedPhotons",
                    allMETEGCorrected=True,
                    muCorrection=False,
                    eGCorrection=True,
                    runOnMiniAOD=True,
                    postfix="MuEGClean"
                    )
process.slimmedMETsMuEGClean = process.slimmedMETs.clone()
process.slimmedMETsMuEGClean.src = cms.InputTag("patPFMetT1MuEGClean")
process.slimmedMETsMuEGClean.rawVariation =  cms.InputTag("patPFMetRawMuEGClean")
process.slimmedMETsMuEGClean.t1Uncertainties = cms.InputTag("patPFMetT1%sMuEGClean")
del process.slimmedMETsMuEGClean.caloMET

process.egcorrMET = cms.Sequence(
    process.cleanedPhotonsMuEGClean+process.cleanedCorPhotonsMuEGClean+
    process.matchedPhotonsMuEGClean + process.matchedElectronsMuEGClean +
    process.corMETPhotonMuEGClean+process.corMETElectronMuEGClean+
    process.patPFMetT1MuEGClean+process.patPFMetRawMuEGClean+
    process.patPFMetT1SmearMuEGClean+process.patPFMetT1TxyMuEGClean+
    process.patPFMetTxyMuEGClean+process.patPFMetT1JetEnUpMuEGClean+
    process.patPFMetT1JetResUpMuEGClean+process.patPFMetT1SmearJetResUpMuEGClean+
    process.patPFMetT1ElectronEnUpMuEGClean+process.patPFMetT1PhotonEnUpMuEGClean+
    process.patPFMetT1MuonEnUpMuEGClean+process.patPFMetT1TauEnUpMuEGClean+
    process.patPFMetT1UnclusteredEnUpMuEGClean+process.patPFMetT1JetEnDownMuEGClean+
    process.patPFMetT1JetResDownMuEGClean+process.patPFMetT1SmearJetResDownMuEGClean+
    process.patPFMetT1ElectronEnDownMuEGClean+process.patPFMetT1PhotonEnDownMuEGClean+
    process.patPFMetT1MuonEnDownMuEGClean+process.patPFMetT1TauEnDownMuEGClean+
    process.patPFMetT1UnclusteredEnDownMuEGClean+process.slimmedMETsMuEGClean)


## MET CHS (not available as slimmedMET collection)
## copied from https://github.com/cms-jet/JMEValidator/blob/CMSSW_7_6_X/python/FrameworkConfiguration.py
def clean_met_(met):
    del met.t01Variation
    del met.t1Uncertainties
    del met.t1SmearedVarsAndUncs
    del met.tXYUncForRaw
    del met.tXYUncForT1
    del met.tXYUncForT01
    del met.tXYUncForT1Smear
    del met.tXYUncForT01Smear

from PhysicsTools.PatAlgos.tools.metTools import addMETCollection

## Raw PF METs
#process.load('RecoMET.METProducers.PFMET_cfi')
process.pfMet.src = cms.InputTag('packedPFCandidates')
addMETCollection(process, labelName='patPFMet', metSource='pfMet') # RAW MET
process.patPFMet.addGenMET = False

process.pfMetCHS = process.pfMet.clone()
process.pfMetCHS.src = cms.InputTag("chs")
process.pfMetCHS.alias = cms.string('pfMetCHS')
addMETCollection(process, labelName='patPFMetCHS', metSource='pfMetCHS') # RAW CHS MET
process.patPFMetCHS.addGenMET = False


## Slimmed METs
from PhysicsTools.PatAlgos.slimming.slimmedMETs_cfi import slimmedMETs
#### CaloMET is not available in MiniAOD
del slimmedMETs.caloMET

### CHS
process.slimmedMETsCHS = slimmedMETs.clone()
if hasattr(process, "patPFMetCHS"):
    # Create MET from Type 1 PF collection
    process.patPFMetCHS.addGenMET = False
    process.slimmedMETsCHS.src = cms.InputTag("patPFMetCHS")
    process.slimmedMETsCHS.rawUncertainties = cms.InputTag("patPFMetCHS") # only central value
else:
    # Create MET from RAW PF collection
    process.patPFMetCHS.addGenMET = False
    process.slimmedMETsCHS.src = cms.InputTag("patPFMetCHS")
    del process.slimmedMETsCHS.rawUncertainties # not available
    
clean_met_(process.slimmedMETsCHS)
addMETCollection(process, labelName="slMETsCHS", metSource="slimmedMETsCHS")
process.slMETsCHS.addGenMET = False

### LEPTON cfg

# collections for lepton PF-isolation deposits
process.load('UHH2.core.pfCandidatesByType_cff')
process.load('CommonTools.ParticleFlow.deltaBetaWeights_cff')

## MUON
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

process.slimmedMuonsUSER = cms.EDProducer('PATMuonUserData',
  src = cms.InputTag('slimmedMuons'),
  vmaps_double = cms.vstring(mu_isovals),
)

## ELECTRON

# regression
# read from DB
from EgammaAnalysis.ElectronTools.regressionWeights_cfi import regressionWeights
process = regressionWeights(process)
process.load('EgammaAnalysis.ElectronTools.regressionApplication_cff')
#process.p =  cms.Path(process.regressionApplication * process.analysis)

# the regression and the smearer on miniAOD
# https://twiki.cern.ch/twiki/bin/view/CMS/EGMRegression#Consistent_EGMSmearer
from EgammaAnalysis.ElectronTools.regressionWeights_cfi import regressionWeights
process = regressionWeights(process)

process.RandomNumberGeneratorService = cms.Service("RandomNumberGeneratorService",
                  calibratedPatElectrons  = cms.PSet( initialSeed = cms.untracked.uint32(8675389),
                                                      engineName = cms.untracked.string('TRandom3'),
                                                      ),
                  calibratedPatPhotons    = cms.PSet( initialSeed = cms.untracked.uint32(8675389),
                                                      engineName = cms.untracked.string('TRandom3'),
                                                      ),
                                                   )
process.load('EgammaAnalysis.ElectronTools.regressionApplication_cff')
process.load('EgammaAnalysis.ElectronTools.calibratedPatElectronsRun2_cfi')
process.load('EgammaAnalysis.ElectronTools.calibratedPatPhotonsRun2_cfi')

# Path and EndPath definitions
process.calibratedPatElectrons.isMC = cms.bool(not useData)
process.calibratedPatPhotons.isMC = cms.bool(not useData)
process.EGMRegression = cms.Path(process.regressionApplication)
process.EGMSmearerElectrons = cms.Path(process.calibratedPatElectrons)
process.EGMSmearerPhotons   = cms.Path(process.calibratedPatPhotons)

# Set the lines below to True or False depending if you are correcting the scale (data) or smearing the resolution (MC) 
#TODO swithc to useDATA flag
#process.EGMSmearerElectrons.isMC = cms.bool(not useData)
#process.EGMSmearerPhotons.isMC = cms.bool(not useData)


# mini-isolation
from UHH2.core.electron_pfMiniIsolation_cff import *

el_isovals = []

load_elecPFMiniIso(process, 'elecPFMiniIsoSequenceSTAND', algo = 'STAND',
  src = 'selectedElectrons',
  src_charged_hadron = 'pfAllChargedHadrons',
  src_neutral_hadron = 'pfAllNeutralHadrons',
  src_photon         = 'pfAllPhotons',
  src_charged_pileup = 'pfPileUpAllChargedParticles',
  isoval_list = el_isovals
)

load_elecPFMiniIso(process, 'elecPFMiniIsoSequencePFWGT', algo = 'PFWGT',
  src = 'selectedElectrons',
  src_neutral_hadron = 'pfWeightedNeutralHadrons',
  src_photon         = 'pfWeightedPhotons',
  isoval_list = el_isovals
)



# electron ID from VID
process.load('RecoEgamma.ElectronIdentification.egmGsfElectronIDs_cff')
#process.electronMVAValueMapProducer.srcMiniAOD = cms.InputTag('slimmedElectrons')
#process.egmGsfElectronIDs.physicsObjectSrc = cms.InputTag('slimmedElectrons')

# https://twiki.cern.ch/twiki/bin/view/CMS/EGMRegression#Compatibility_with_VID
process.selectedElectrons = cms.EDFilter("PATElectronSelector",
    src = cms.InputTag("calibratedPatElectrons"),
    cut = cms.string("pt>5 && abs(eta)")
)

process.egmGsfElectronIDs.physicsObjectSrc = cms.InputTag('selectedElectrons')
#process.electronIDValueMapProducer.srcMiniAOD = cms.InputTag('selectedElectrons')
#process.electronRegressionValueMapProducer.srcMiniAOD = cms.InputTag('selectedElectrons')
process.electronMVAValueMapProducer.srcMiniAOD = cms.InputTag('selectedElectrons')

elecID_mod_ls = [
  'RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_Summer16_80X_V1_cff',
  'RecoEgamma.ElectronIdentification.Identification.cutBasedElectronHLTPreselecition_Summer16_V1_cff',
  'RecoEgamma.ElectronIdentification.Identification.heepElectronID_HEEPV60_cff',
  'RecoEgamma.ElectronIdentification.Identification.mvaElectronID_Spring16_GeneralPurpose_V1_cff',
  'RecoEgamma.ElectronIdentification.Identification.mvaElectronID_Spring16_HZZ_V1_cff',
]

from PhysicsTools.SelectorUtils.tools.vid_id_tools import *
for mod in elecID_mod_ls: setupAllVIDIdsInModule(process, mod, setupVIDElectronSelection)

# slimmedElectronsUSER ( = slimmedElectrons + USER variables)
process.slimmedElectronsUSER = cms.EDProducer('PATElectronUserData',
  #src = cms.InputTag('slimmedElectrons'),
  src = cms.InputTag('selectedElectrons'),

  vmaps_bool = cms.PSet(

    cutBasedElectronID_Summer16_80X_V1_veto   = cms.InputTag('egmGsfElectronIDs:cutBasedElectronID-Summer16-80X-V1-veto'),
    cutBasedElectronID_Summer16_80X_V1_loose  = cms.InputTag('egmGsfElectronIDs:cutBasedElectronID-Summer16-80X-V1-loose'),
    cutBasedElectronID_Summer16_80X_V1_medium = cms.InputTag('egmGsfElectronIDs:cutBasedElectronID-Summer16-80X-V1-medium'),
    cutBasedElectronID_Summer16_80X_V1_tight  = cms.InputTag('egmGsfElectronIDs:cutBasedElectronID-Summer16-80X-V1-tight'),

    cutBasedElectronHLTPreselection_Summer16_V1 = cms.InputTag('egmGsfElectronIDs:cutBasedElectronHLTPreselection-Summer16-V1'),

    heepElectronID_HEEPV60                                = cms.InputTag('egmGsfElectronIDs:heepElectronID-HEEPV60'),

  ),

  vmaps_float = cms.PSet(
    ElectronMVAEstimatorRun2Spring16GeneralPurposeV1Values__user01 = cms.InputTag('electronMVAValueMapProducer:ElectronMVAEstimatorRun2Spring16GeneralPurposeV1Values'),
    ElectronMVAEstimatorRun2Spring16HZZV1Values__user01 = cms.InputTag('electronMVAValueMapProducer:ElectronMVAEstimatorRun2Spring16HZZV1Values'),
  ),

  vmaps_double = cms.vstring(el_isovals),

  effAreas_file = cms.FileInPath('RecoEgamma/ElectronIdentification/data/Summer16/effAreaElectrons_cone03_pfNeuHadronsAndPhotons_80X.txt'),

  mva_GeneralPurpose = cms.string('ElectronMVAEstimatorRun2Spring16GeneralPurposeV1Values__user01'),
  mva_HZZ = cms.string('ElectronMVAEstimatorRun2Spring16HZZV1Values__user01'),
)


### additional MET filters not given in MiniAOD

process.load('RecoMET.METFilters.BadPFMuonFilter_cfi')
process.BadPFMuonFilter.muons = cms.InputTag("slimmedMuons")
process.BadPFMuonFilter.PFCandidates = cms.InputTag("packedPFCandidates")

process.load('RecoMET.METFilters.BadChargedCandidateFilter_cfi')
process.BadChargedCandidateFilter.muons = cms.InputTag("slimmedMuons")
process.BadChargedCandidateFilter.PFCandidates = cms.InputTag("packedPFCandidates")


### NtupleWriter


isreHLT = False
for x in process.source.fileNames:
    if "reHLT" in x:
        isreHLT = True

triggerpath="HLT"
if isreHLT:
    triggerpath="HLT2"

if useData:
    metfilterpath="RECO"
else:
    metfilterpath="PAT"

process.MyNtuple = cms.EDFilter('NtupleWriter',
        #AnalysisModule = cms.PSet(
        #    name = cms.string("NoopAnalysisModule"),
        #    library = cms.string("libUHH2examples.so"),
        #    # Note: all other settings of type string are passed to the module, e.g.:
        #    TestKey = cms.string("TestValue")
        #),
        fileName = cms.string("Ntuple.root"), 
        doPV = cms.bool(True),
        pv_sources = cms.vstring("offlineSlimmedPrimaryVertices"),
        doRho = cms.untracked.bool(True),
        rho_source = cms.InputTag("fixedGridRhoFastjetAll"),

        save_lepton_keys = cms.bool(True),

        doElectrons = cms.bool(True),
        #doElectrons = cms.bool(False),
        electron_source = cms.InputTag("slimmedElectronsUSER"),
        electron_IDtags = cms.vstring(
          # keys to be stored in UHH2 Electron class via the tag mechanism:
          # each string should correspond to a variable saved
          # via the "userInt" method in the pat::Electron collection used 'electron_source'
          # [the configuration of the pat::Electron::userInt variables should be done in PATElectronUserData]
          'cutBasedElectronID_Summer16_80X_V1_veto',
          'cutBasedElectronID_Summer16_80X_V1_loose',
          'cutBasedElectronID_Summer16_80X_V1_medium',
          'cutBasedElectronID_Summer16_80X_V1_tight',
          'cutBasedElectronHLTPreselection_Summer16_V1',
          'heepElectronID_HEEPV60',
        ),
        #Add variables to trace possible issues with the ECAL slew rate mitigation 
        #https://twiki.cern.ch/twiki/bin/view/CMSPublic/ReMiniAOD03Feb2017Notes#EGM
        doEleAddVars = cms.bool(useData),
        dupECALClusters_source = cms.InputTag('particleFlowEGammaGSFixed:dupECALClusters'),
        hitsNotReplaced_source = cms.InputTag('ecalMultiAndGSGlobalRecHitEB:hitsNotReplaced'),

        doMuons = cms.bool(True),
        muon_sources = cms.vstring("slimmedMuonsUSER"),
        doTaus = cms.bool(True),
        tau_sources = cms.vstring("slimmedTaus" ),
        tau_ptmin = cms.double(0.0),
        tau_etamax = cms.double(999.0),
        doPhotons = cms.bool(False),
        #photon_sources = cms.vstring("selectedPatPhotons"),
        
        doJets = cms.bool(True),
        #jet_sources = cms.vstring("patJetsAk4PFCHS", "patJetsAk8PFCHS", "patJetsCa15CHSJets", "patJetsCa8CHSJets", "patJetsCa15PuppiJets", "patJetsCa8PuppiJets"),
   #     jet_sources = cms.vstring("slimmedJets","slimmedJetsPuppi"),
        jet_sources = cms.vstring("slimmedJets","slimmedJetsPuppi","patJetsAK8PFPUPPI","patJetsAK8PFCHS"),
        jet_ptmin = cms.double(10.0),
        jet_etamax = cms.double(999.0),
        
        doMET = cms.bool(True),
        #met_sources =  cms.vstring("slimmedMETs","slimmedMETsPuppi","slMETsCHS","slimmedMETsMuEGClean"),
        met_sources =  met_sources_GL,
        #doTopJets = cms.bool(False),
        doTopJets = cms.bool(True),
        topjet_ptmin = cms.double(150.0),
        topjet_etamax = cms.double(5.0),                                                                               
        topjet_sources = cms.vstring("slimmedJetsAK8","patJetsAk8CHSJetsSoftDropPacked","patJetsHepTopTagCHSPacked","patJetsHepTopTagPuppiPacked","patJetsAk8PuppiJetsSoftDropPacked"),
      #  topjet_sources = cms.vstring("slimmedJetsAK8","patJetsAk8CHSJetsSoftDropPacked","patJetsAk8PuppiJetsSoftDropPacked"),
        #Note: use label "daughters" for  subjet_sources if you want to store as subjets the linked daughters of the topjets (NOT for slimmedJetsAK8 in miniAOD!)
        #to store a subjet collection present in miniAOD indicate the proper label of the subjets method in pat::Jet: SoftDrop or CMSTopTag
        subjet_sources = cms.vstring("SoftDrop","daughters","daughters","daughters","daughters"),
        #Specify "store" if you want to store b-tagging taginfos for subjet collection, make sure to have included them with .addTagInfos = True
        #addTagInfos = True is currently true by default, however, only for collections produced and not read directly from miniAOD
        #If you don't want to store stubjet taginfos leave string empy ""
        subjet_taginfos = cms.vstring("","store","store","store","store"),
        #Note: if you want to store the MVA Higgs tagger discriminator, specify the jet collection from which to pick it up and the tagger name
        #currently the discriminator is trained on ungroomed jets, so the discriminaotr has to be taken from ungroomed jets
        higgstag_sources = cms.vstring("patJetsAk8CHSJets","patJetsAk8CHSJets","patJetsCa15CHSJets","patJetsCa15PuppiJets","patJetsAk8PuppiJetsFat"),
#        higgstag_sources = cms.vstring("patJetsAK8PFCHS","patJetsAK8PFCHS","patJetsCa15CHSJets","patJetsCa15PuppiJets","patJetsAk8PuppiJetsFat"), #TEST
        higgstag_names = cms.vstring("pfBoostedDoubleSecondaryVertexAK8BJetTags","pfBoostedDoubleSecondaryVertexAK8BJetTags","pfBoostedDoubleSecondaryVertexCA15BJetTags","pfBoostedDoubleSecondaryVertexCA15BJetTags","pfBoostedDoubleSecondaryVertexAK8BJetTags"),
        #Note: if empty, njettiness is directly taken from MINIAOD UserFloat and added to jets, otherwise taken from the provided source (for Run II CMSSW_74 ntuples)
        topjet_njettiness_sources = cms.vstring("","NjettinessAk8CHS","NjettinessCa15CHS","NjettinessCa15Puppi","NjettinessAk8Puppi"),
        topjet_substructure_variables_sources = cms.vstring("","ak8CHSJets","ca15CHSJets", "ca15PuppiJets", "ak8PuppiJetsFat"),
        topjet_njettiness_groomed_sources = cms.vstring("","NjettinessAk8SoftDropCHS","NjettinessCa15SoftDropCHS","NjettinessCa15SoftDropPuppi","NjettinessAk8SoftDropPuppi"),
        topjet_substructure_groomed_variables_sources = cms.vstring("","ak8CHSJetsSoftDropforsub","ca15CHSJetsSoftDropforsub", "ca15PuppiJetsSoftDropforsub", "ak8PuppiJetsSoftDropforsub"),
        #Note: for slimmedJetsAK8 on miniAOD, the pruned mass is available as user flot, with label ak8PFJetsCHSPrunedMass.
        #Alternatively it is possible to specify another pruned jet collection (to be produced here), from which to get it by jet-matching.
        #Finally, it is also possible to leave the pruned mass empty with ""
        topjet_prunedmass_sources = cms.vstring("ak8PFJetsCHSPrunedMass","patJetsAk8CHSJetsPrunedPacked","patJetsCa15CHSJetsPrunedPacked","patJetsCa15CHSJetsPrunedPacked","patJetsAk8CHSJetsPrunedPacked"),
        topjet_softdropmass_sources = cms.vstring("ak8PFJetsCHSSoftDropMass", "", "", "", ""),
        #topjet_sources = cms.vstring("patJetsHepTopTagCHSPacked", "patJetsCmsTopTagCHSPacked", "patJetsCa8CHSJetsPrunedPacked", "patJetsCa15CHSJetsFilteredPacked",
        #        "patJetsHepTopTagPuppiPacked", "patJetsCmsTopTagPuppiPacked", "patJetsCa8PuppiJetsPrunedPacked", "patJetsCa15PuppiJetsFilteredPacked",
        #        'patJetsCa8CHSJetsSoftDropPacked', 'patJetsCa8PuppiJetsSoftDropPacked'
        #        ),
        # jets to match to the topjets in order to get njettiness, in the same order as topjet_sources.
        # Note that no substructure variables are added for the softdrop jets.
        #topjet_substructure_variables_sources = cms.vstring("patJetsCa15CHSJets", "patJetsCa8CHSJets", "patJetsCa8CHSJets", "patJetsCa15CHSJets",
        #        "patJetsCa15PuppiJets", "patJetsCa8PuppiJets", "patJetsCa8PuppiJets", "patJetsCa15PuppiJets",
        #        "patJetsCa8CHSJets", "patJetsCa8PuppiJets"),
        #topjet_njettiness_sources = cms.vstring("NjettinessCa15CHS", "NjettinessCa8CHS", "NjettinessCa8CHS", "NjettinessCa15CHS",
        #        "NjettinessCa15Puppi", "NjettinessCa8Puppi", "NjettinessCa8Puppi", "NjettinessCa15Puppi",
        #        "NjettinessCa8CHS", "NjettinessCa8Puppi"),

        # switch off qjets for now, as it takes a long time:
        #topjet_qjets_sources = cms.vstring("QJetsCa15CHS", "QJetsCa8CHS", "QJetsCa8CHS", "QJetsCa15CHS"),
        
        doTrigger = cms.bool(True), 
        trigger_bits = cms.InputTag("TriggerResults","",triggerpath),
        # MET filters (HBHE noise, CSC, etc.) are stored as trigger Bits in MINIAOD produced in path "PAT"/"RECO" with prefix "Flag_"
        metfilter_bits = cms.InputTag("TriggerResults","",metfilterpath),
        # for now, save all the triggers:
        trigger_prefixes = cms.vstring("HLT_","Flag_"),

        # Give the names of filters for that you want to store the trigger objects that have fired the respecitve trigger
        # filter paths for a given trigger can be found in https://cmsweb.cern.ch/confdb/
        # Example: for HLT_Mu45_eta2p1 the last trigger filter is hltL3fL1sMu16orMu25L1f0L2f10QL3Filtered45e2p1Q
        #          for HLT_Ele35_CaloIdVT_GsfTrkIdT_PFJet150_PFJet50: relevant filters are hltEle35CaloIdVTGsfTrkIdTGsfDphiFilter (last electron filter), hltEle35CaloIdVTGsfTrkIdTDiCentralPFJet50EleCleaned and hltEle35CaloIdVTGsfTrkIdTCentralPFJet150EleCleaned (for the two jets). 
        #          The  filter hltEle35CaloIdVTGsfTrkIdTCentralPFJet150EleCleaned only included redundant objects that are already included in hltEle35CaloIdVTGsfTrkIdTCentralPFJet50EleCleaned.
        #          for HLT_Ele45_CaloIdVT_GsfTrkIdT_PFJet200_PFJet50: relevant filters are hltEle45CaloIdVTGsfTrkIdTGsfDphiFilter (last electron filter), hltEle45CaloIdVTGsfTrkIdTDiCentralPFJet50EleCleaned
        triggerObjects_sources = cms.vstring(""),
        #  'hltL3fL1sMu16orMu25L1f0L2f10QL3Filtered45e2p1Q',        # HLT_Mu45_eta2p1_v*
        #  'hltEle35CaloIdVTGsfTrkIdTGsfDphiFilter',                # HLT_Ele35_CaloIdVT_GsfTrkIdT_PFJet150_PFJet50_v* (electron)
        #  'hltEle35CaloIdVTGsfTrkIdTDiCentralPFJet50EleCleaned',   # HLT_Ele35_CaloIdVT_GsfTrkIdT_PFJet150_PFJet50_v* (jets)
        #  'hltEle45CaloIdVTGsfTrkIdTGsfDphiFilter',                # HLT_Ele45_CaloIdVT_GsfTrkIdT_PFJet200_PFJet50_v* (electron)
        #  'hltEle45CaloIdVTGsfTrkIdTDiCentralPFJet50EleCleaned',   # HLT_Ele45_CaloIdVT_GsfTrkIdT_PFJet200_PFJet50_v* (jets)
        #  'hltL3crIsoL1sMu25L1f0L2f10QL3f27QL3trkIsoFiltered0p09', # HLT_IsoMu27_v*
        #  'hltEle27WPLooseGsfTrackIsoFilter',                      # HLT_Ele27_eta2p1_WPLoose_Gsf_v*
        #),
        trigger_objects = cms.InputTag("selectedPatTrigger"),


        
        # *** gen stuff:
        doGenInfo = cms.bool(not useData),
        genparticle_source = cms.InputTag("prunedPrunedGenParticles"),
        stablegenparticle_source = cms.InputTag("packedGenParticles"),
        doAllGenParticles = cms.bool(False), #set to true if you want to store all gen particles, otherwise, only prunedPrunedGenParticles are stored (see above)
        doGenJets = cms.bool(not useData),
        genjet_sources = cms.vstring("slimmedGenJets","slimmedGenJetsAK8","ca15GenJets"),
        genjet_ptmin = cms.double(10.0),
        genjet_etamax = cms.double(5.0),
                            
        doGenTopJets = cms.bool(not useData),
        gentopjet_sources = cms.vstring("ak8GenJetsSoftDrop"),
        gentopjet_ptmin = cms.double(150.0), 
        gentopjet_etamax = cms.double(5.0),
        
        doGenJetsWithParts = cms.bool(False),
        doAllPFParticles = cms.bool(False),
        pf_collection_source = cms.InputTag("packedPFCandidates"),

        # # *** HOTVR & XCone stuff
        doHOTVR = cms.bool(True),
        doXCone = cms.bool(True),
        doGenHOTVR = cms.bool(not useData),
        doGenXCone = cms.bool(not useData),    
         # doHOTVR = cms.bool(False),
         # doXCone = cms.bool(False),
         # doGenHOTVR = cms.bool(False),
         # doGenXCone =  cms.bool(False),

)

#process.content = cms.EDAnalyzer("EventContentAnalyzer")

#process.load("FWCore.MessageLogger.MessageLogger_cfi")
#process.MessageLogger = cms.Service("MessageLogger")



# Note: we run in unscheduled mode, i.e. all modules are run as required; just make sure that MyNtuple runs:
# process.regressionApplication * 
#     process.EGMRegression * 
#     process.EGMSmearerElectrons * 
#     process.EGMSmearerPhotons *
 # process.EGMRegression * 
 #    process.EGMSmearerElectrons * 
 #    process.EGMSmearerPhotons * 

process.p = cms.Path(
    process.BadPFMuonFilter * 
    process.BadChargedCandidateFilter * 
    process.fullPatMetSequence * # If you are re-correctign the default MET
    process.egcorrMET  *
    process.MyNtuple)
process.schedule = cms.Schedule(process.EGMRegression,process.EGMSmearerElectrons,process.EGMSmearerPhotons,process.p)
#open('pydump.py','w').write(process.dumpPython())
