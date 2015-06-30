import FWCore.ParameterSet.Config as cms


useData = False

# minimum pt for the large-R jets (applies for all: vanilla CA8/CA15, cmstoptag, heptoptag). Also applied for the corresponding genjets.
fatjet_ptmin = 100.0

bTagDiscriminators = [
    'pfJetProbabilityBJetTags',
    'pfJetBProbabilityBJetTags',
    'pfSimpleSecondaryVertexHighEffBJetTags',
    'pfSimpleSecondaryVertexHighPurBJetTags',
    'pfCombinedInclusiveSecondaryVertexV2BJetTags',
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
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(1)
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) , allowUnscheduled = cms.untracked.bool(True) )

process.source = cms.Source("PoolSource",
                            fileNames  = cms.untracked.vstring("file:/nfs/dust/cms/user/peiffer/ZprimeToTT_M-2000_W-200_50ns_MCRUN2_TEST_MINIAODSIM.root"),
                            skipEvents = cms.untracked.uint32(0)
)

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(10))

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

###############################################
# OUT
from Configuration.EventContent.EventContent_cff import *
process.out = cms.OutputModule("PoolOutputModule",
                               fileName = cms.untracked.string('miniaod.root'),
                               outputCommands = MINIAODSIMEventContent.outputCommands )

process.out.outputCommands.extend([
    'keep *_patJetsAk8CHS*_*_*',
    'keep *_patJetsCa15CHS*_*_*',
    'keep *_NjettinessCa8CHS_*_*',
    'keep *_NjettinessCa15CHS_*_*',
    "keep *_patJetsCmsTopTagCHSPacked_*_*",
    "keep *_patJetsCmsTopTagCHSSubjets_*_*",
    "keep *_patJetsHepTopTagCHSPacked_*_*",
    "keep *_patJetsHepTopTagCHSSubjets_*_*",
    "keep *_patJetsAk8CHSJetsSoftDropPacked_*_*",
    "keep *_patJetsAk8CHSJetsSoftDropSubjets_*_*",
    "keep *_patJetsAk8CHSJetsPrunedPacked_*_*",
    "keep *_patJetsAk8CHSJetsPrunedSubjets_*_*",
    "keep *_prunedPrunedGenParticles_*_*",
    "keep *_egmGsfElectronIDs_*_*"
])

###############################################
# RECO AND GEN SETUP
process.load("Configuration.Geometry.GeometryRecoDB_cff")
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = 'MCRUN2_74_V9A::All'  # NOTE: use V9A for 50ns and V9 for 25ns

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
process.ak8CHSJets  = ak8PFJets.clone (src = 'chs', doAreaFastjet = True, jetPtMin = fatjet_ptmin)
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


from RecoJets.JetProducers.PFJetParameters_cfi import PFJetParameters
from RecoJets.JetProducers.CATopJetParameters_cfi import CATopJetParameters
from RecoJets.JetProducers.AnomalousCellParameters_cfi import AnomalousCellParameters
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

process.hepTopTagCHS = process.cmsTopTagCHS.clone(
    rParam = cms.double(1.5),
    tagAlgo = cms.int32(2), #2=fastjet heptt
    muCut = cms.double(0.8),
    maxSubjetMass = cms.double(30.0),
    useSubjetMass = cms.bool(False),
)

# also re-do the ak4 jet clustering, as this is much simpler for b-tagging (there does not seem to be a simple way of
# re-running b-tagging on the slimmedJets ...).
#from RecoJets.JetProducers.ak4PFJets_cfi import ak4PFJets
#process.ak4PFCHS = ak4PFJets.clone(src = 'chs')
#process.ak8PFCHS =process.ak4PFCHS.clone(rParam = 0.8)


#################################################
### Softdrop

from RecoJets.Configuration.RecoPFJets_cff import ak8PFJetsCHS
process.ak8CHSJetsSoftDrop = ak8PFJetsCHSSoftDrop.clone(src = cms.InputTag('chs'), jetPtMin = fatjet_ptmin)

from RecoJets.JetProducers.ak4PFJetsPruned_cfi import ak4PFJetsPruned
#Note low pt threshold as jets currently not stored but used just to derived pruned mass
process.ak8CHSJetsPruned = ak4PFJetsPruned.clone(rParam = 0.8, doAreaFastjet = True, src = 'chs', jetPtMin = 70)
process.ca15CHSJetsPruned = ak4PFJetsPruned.clone(rParam = 1.5, jetAlgorithm = "CambridgeAachen", doAreaFastjet = True, src = 'chs', jetPtMin = 70)
###############################################
# PUPPI JETS
from CommonTools.PileupAlgos.Puppi_cff import puppi
process.puppi = puppi.clone(candName = cms.InputTag('packedPFCandidates'), vertexName = cms.InputTag('offlineSlimmedPrimaryVertices'))

# copy all the jet collections above; just use 'puppi' instead of 'chs' as input:
#for name in ['ca8CHSJets', 'ca15CHSJets', 'ca8CHSJetsPruned', 'ca15CHSJetsFiltered', 'cmsTopTagCHS', 'hepTopTagCHS', 'ca8CHSJetsSoftDrop']:
#    setattr(process, name.replace('CHS', 'Puppi'), getattr(process, name).clone(src = cms.InputTag('puppi')))

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
    elSource = cms.InputTag('slimmedElectrons'),
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
    if add_ungroomed:
        if verbose: print "Adding ungroomed patJets" + cap(fatjets_name)
        addJetCollection(process, labelName = fatjets_name, jetSource = cms.InputTag(fatjets_name), algo = algo, rParam = rParam,
            jetCorrections = (jetcorr_label, cms.vstring(['L1FastJet', 'L2Relative', 'L3Absolute']), 'None'),
            genJetCollection = cms.InputTag(ungroomed_genjets_name),
            **common_btag_parameters
        )
    
    # patify groomed fat jets, with b-tagging:
    if verbose: print "adding grommed jets patJets" + cap(groomed_jets_name)
    addJetCollection(process, labelName = groomed_jets_name, jetSource = cms.InputTag(groomed_jets_name), algo = algo, rParam = rParam,
       jetCorrections = (jetcorr_label, cms.vstring(['L1FastJet', 'L2Relative', 'L3Absolute']), 'None'),
       #genJetCollection = cms.InputTag(groomed_genjets_name), # nice try, but PAT looks for GenJets, whereas jets with subjets are BasicJets, so PAT cannot be used for this matching ...
       **common_btag_parameters)
    # patify subjets, with subjet b-tagging:
    if verbose: print "adding grommed jets' subjets patJets" + cap(subjets_name)
    addJetCollection(process, labelName = subjets_name, jetSource = cms.InputTag(groomed_jets_name, 'SubJets'), algo = algo, rParam = rParam,
        jetCorrections = (jetcorr_label_subjets, cms.vstring(['L1FastJet', 'L2Relative', 'L3Absolute']), 'None'),
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
add_fatjets_subjets(process, 'ca15CHSJets', 'ca15CHSJetsFiltered', genjets_name = lambda s: s.replace('CHS', 'Gen'))
#add_fatjets_subjets(process, 'ca8CHSJets', 'cmsTopTagCHS', genjets_name = lambda s: s.replace('CHS', 'Gen'))
#add_fatjets_subjets(process, 'ca15CHSJets', 'hepTopTagCHS', genjets_name = lambda s: s.replace('CHS', 'Gen'))
add_fatjets_subjets(process, 'ak8CHSJets', 'ak8CHSJetsSoftDrop', genjets_name = lambda s: s.replace('CHS', 'Gen'))
#B-tagging not needed for pruned jets, they are just used to get the mass
add_fatjets_subjets(process, 'ak8CHSJets', 'ak8CHSJetsPruned', genjets_name = lambda s: s.replace('CHS', 'Gen'), btagging = False)
add_fatjets_subjets(process, 'ca15CHSJets', 'ca15CHSJetsPruned', genjets_name = lambda s: s.replace('CHS', 'Gen'), btagging = False)
#add_fatjets_subjets(process, 'ca8PuppiJets', 'ca8PuppiJetsPruned', genjets_name = lambda s: s.replace('Puppi', 'Gen'))
#add_fatjets_subjets(process, 'ca15PuppiJets', 'ca15PuppiJetsFiltered', genjets_name = lambda s: s.replace('Puppi', 'Gen'))
#add_fatjets_subjets(process, 'ca8PuppiJets', 'cmsTopTagPuppi', genjets_name = lambda s: s.replace('Puppi', 'Gen'))
#add_fatjets_subjets(process, 'ca15PuppiJets', 'hepTopTagPuppi', genjets_name = lambda s: s.replace('Puppi', 'Gen'))
#add_fatjets_subjets(process, 'ca8PuppiJets', 'ca8PuppiJetsSoftDrop')

# configure PAT for miniAOD:
#process.patJetPartons.particles = 'prunedGenParticles'

from PhysicsTools.PatAlgos.tools.pfTools import *
## Adapt primary vertex collection
adaptPVs(process, pvCollection = cms.InputTag('offlineSlimmedPrimaryVertices'))


# Add subjet variables (on ungroomed jets only!)
from RecoJets.JetProducers.nJettinessAdder_cfi import Njettiness
from RecoJets.JetProducers.qjetsadder_cfi import QJetsAdder

#process.NjettinessCa8CHS = Njettiness.clone(src = cms.InputTag("patJetsCa8CHSJets"), cone = cms.double(0.8))
#process.NjettinessCa15CHS = Njettiness.clone(src = cms.InputTag("patJetsCa15CHSJets"), cone = cms.double(1.5))
#process.NjettinessCa8Puppi = Njettiness.clone(src = cms.InputTag("patJetsCa8PuppiJets"), cone = cms.double(0.8))
#process.NjettinessCa15Puppi = Njettiness.clone(src = cms.InputTag("patJetsCa15PuppiJets"), cone = cms.double(1.5))

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
  float_vmaps = cms.vstring(mu_isovals),
)

## ELECTRON

from UHH2.core.electron_pfMiniIsolation_cff import *

el_isovals = []

load_elecPFMiniIso(process, 'elecPFMiniIsoSequenceSTAND', algo = 'STAND',
  src = 'slimmedElectrons',
  src_charged_hadron = 'pfAllChargedHadrons',
  src_neutral_hadron = 'pfAllNeutralHadrons',
  src_photon         = 'pfAllPhotons',
  src_charged_pileup = 'pfPileUpAllChargedParticles',
  isoval_list = el_isovals
)

load_elecPFMiniIso(process, 'elecPFMiniIsoSequencePFWGT', algo = 'PFWGT',
  src = 'slimmedElectrons',
  src_neutral_hadron = 'pfWeightedNeutralHadrons',
  src_photon         = 'pfWeightedPhotons',
  isoval_list = el_isovals
)

process.slimmedElectronsUSER = cms.EDProducer('PATElectronUserData',
  src = cms.InputTag('slimmedElectrons'),
  float_vmaps = cms.vstring(el_isovals),

  weights_mvaNoTrig = cms.vstring(
    'EgammaAnalysis/ElectronTools/data/CSA14/EIDmva_EB_5_50ns_BDT.weights.xml',
    'EgammaAnalysis/ElectronTools/data/CSA14/EIDmva_EE_5_50ns_BDT.weights.xml',
    'EgammaAnalysis/ElectronTools/data/CSA14/EIDmva_EB_10_50ns_BDT.weights.xml',
    'EgammaAnalysis/ElectronTools/data/CSA14/EIDmva_EE_10_50ns_BDT.weights.xml',
#    'EgammaAnalysis/ElectronTools/data/CSA14/EIDmva_EB_5_25ns_BDT.weights.xml',
#    'EgammaAnalysis/ElectronTools/data/CSA14/EIDmva_EE_5_25ns_BDT.weights.xml',
#    'EgammaAnalysis/ElectronTools/data/CSA14/EIDmva_EB_10_25ns_BDT.weights.xml',
#    'EgammaAnalysis/ElectronTools/data/CSA14/EIDmva_EE_10_25ns_BDT.weights.xml',
  ),
  weights_mvaTrig = cms.vstring(
    'EgammaAnalysis/ElectronTools/data/CSA14/TrigIDMVA_50ns_EB_BDT.weights.xml',
    'EgammaAnalysis/ElectronTools/data/CSA14/TrigIDMVA_50ns_EE_BDT.weights.xml',
#    'EgammaAnalysis/ElectronTools/data/CSA14/TrigIDMVA_25ns_EB_BDT.weights.xml',
#    'EgammaAnalysis/ElectronTools/data/CSA14/TrigIDMVA_25ns_EE_BDT.weights.xml',
  ),
)

# electron id:
from PhysicsTools.SelectorUtils.tools.vid_id_tools import *
process.load("RecoEgamma.ElectronIdentification.egmGsfElectronIDs_cfi")
process.egmGsfElectronIDs.physicsObjectSrc = cms.InputTag('slimmedElectronsUSER')
setupAllVIDIdsInModule(process, 'RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_PHYS14_PU20bx25_V2_cff', setupVIDElectronSelection)

#NtupleWriter
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
        doElectrons = cms.bool(True),
        electron_source = cms.InputTag("slimmedElectronsUSER"),
        electron_id_sources = cms.PSet (
            # use the Electron::tag enumeration as parameter name; value should be the InputTag
            # to use to read the ValueMap<float> from.
            eid_PHYS14_20x25_veto = cms.InputTag('egmGsfElectronIDs:cutBasedElectronID-PHYS14-PU20bx25-V2-standalone-veto'),
            eid_PHYS14_20x25_loose = cms.InputTag('egmGsfElectronIDs:cutBasedElectronID-PHYS14-PU20bx25-V2-standalone-loose'),
            eid_PHYS14_20x25_medium = cms.InputTag('egmGsfElectronIDs:cutBasedElectronID-PHYS14-PU20bx25-V2-standalone-medium'),
            eid_PHYS14_20x25_tight = cms.InputTag('egmGsfElectronIDs:cutBasedElectronID-PHYS14-PU20bx25-V2-standalone-tight')
        ),
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
        jet_sources = cms.vstring("slimmedJets","slimmedJetsPuppi"),
        jet_ptmin = cms.double(10.0),
        jet_etamax = cms.double(999.0),
        
        doMET = cms.bool(True),
        met_sources =  cms.vstring("slimmedMETs","slimmedMETsPuppi"),
        
        
        doTopJets = cms.bool(True),
        topjet_ptmin = cms.double(100.0),
        topjet_etamax = cms.double(5.0),
        topjet_sources = cms.vstring("slimmedJetsAK8","patJetsAk8CHSJetsSoftDropPacked","patJetsCa15CHSJetsFilteredPacked"),                        
        #Note: use label "daughters" for  subjet_sources if you want to store as subjets the linked daughters of the topjets (NOT for slimmedJetsAK8 in miniAOD!)
        #to store a subjet collection present in miniAOD indicate the proper label of the subjets method in pat::Jet: SoftDrop or CMSTopTag
        subjet_sources = cms.vstring("CMSTopTag","daughters","daughters"),
        #Specify "store" if you want to store b-tagging taginfos for subjet collection, make sure to have included them with .addTagInfos = True
        #addTagInfos = True is currently true by default, however, only for collections produced and not read directly from miniAOD
        #If you don't want to store stubjet taginfos leave string empy ""
        subjet_taginfos = cms.vstring("","store","store"),
        #Note: if you want to store the MVA Higgs tagger discriminator, specify the jet collection from which to pick it up and the tagger name
        #currently the discriminator is trained on ungroomed jets, so the discriminaotr has to be taken from ungroomed jets
        higgstag_sources = cms.vstring("patJetsAk8CHSJets","patJetsAk8CHSJets","patJetsCa15CHSJets"),
        higgstag_names = cms.vstring("pfBoostedDoubleSecondaryVertexAK8BJetTags","pfBoostedDoubleSecondaryVertexAK8BJetTags","pfBoostedDoubleSecondaryVertexCA15BJetTags"),                    
        #Note: topjet_substructure_variables_sources only needed for qjets; njettiness is now directly taken from UserFloat added to jets in MINIAOD (for Run II CMSSW_74 ntuples)
        topjet_njettiness_sources = cms.vstring("NjettinessAK8","",""),
        #Note: for slimmedJetsAK8 on miniAOD, the pruned mass is available as user flot, with label ak8PFJetsCHSPrunedMass.
        #Alternatively it is possible to specify another pruned jet collection (to be produced here), from which to get it by jet-matching.
        #Finally, it is also possible to leave the pruned mass empty with ""
        topjet_prunedmass_sources = cms.vstring("ak8PFJetsCHSPrunedMass","patJetsAk8CHSJetsPrunedPacked","patJetsCa15CHSJetsPrunedPacked"),
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
        trigger_bits = cms.InputTag("TriggerResults","","HLT"),
        # for now, save all the triggers:
        trigger_prefixes = cms.vstring("HLT_",),
        doTrigHTEmu = cms.bool(True),
        trigger_objects = cms.InputTag("selectedPatTrigger"),

        
        # *** gen stuff:
        doGenInfo = cms.bool(not useData),
        genparticle_source = cms.InputTag("prunedPrunedGenParticles"),
        stablegenparticle_source = cms.InputTag("packedGenParticles"),
        doAllGenParticles = cms.bool(False), #set to true if you want to store all gen particles, otherwise, only prunedPrunedGenParticles are stored (see above)
        doGenJets = cms.bool(not useData),
        genjet_sources = cms.vstring("slimmedGenJets","slimmedGenJetsAK8","ak8GenJets"),
        genjet_ptmin = cms.double(10.0),
        genjet_etamax = cms.double(5.0),
                            
        doGenTopJets = cms.bool(True),
        gentopjet_sources = cms.vstring("ak8GenJetsSoftDrop"),
        gentopjet_ptmin = cms.double(100.0), 
        gentopjet_etamax = cms.double(5.0),
        
        doGenJetsWithParts = cms.bool(False),
)

#process.content = cms.EDAnalyzer("EventContentAnalyzer")

# Note: we run in unscheduled mode, i.e. all modules are run as required; just make sure that MyNtuple runs:
process.p = cms.Path(process.MyNtuple)
#process.end = cms.EndPath(process.out)

open('pydump.py','w').write(process.dumpPython())
