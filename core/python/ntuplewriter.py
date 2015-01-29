import FWCore.ParameterSet.Config as cms


useData = False

# minimum pt for the large-R jets (applies for all: vanilla CA8/CA15, cmstoptag, heptoptag). Also applied for the corresponding genjets.
fatjet_ptmin = 100.0

bTagDiscriminators = [
    'pfJetProbabilityBJetTags',
    'pfJetBProbabilityBJetTags',
    'pfSimpleSecondaryVertexHighEffBJetTags',
    'pfSimpleSecondaryVertexHighPurBJetTags',
    'pfCombinedInclusiveSecondaryVertexV2BJetTags'
]




process = cms.Process("USER")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(1)
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) , allowUnscheduled = cms.untracked.bool(True) )

process.source = cms.Source("PoolSource",
                            fileNames  = cms.untracked.vstring("/store/mc/Phys14DR/TprimeTprime_M_1000_Tune4C_13TeV-madgraph/MINIAODSIM/PU40bx25_PHYS14_25_V1-v1/10000/F6F71931-6F67-E411-ABA2-20CF3027A5C9.root"),
                            skipEvents = cms.untracked.uint32(0)
)

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(100))

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
    'keep *_patJetsCa8CHS*_*_*',
    'keep *_patJetsCa15CHS*_*_*',
    'keep *_NjettinessCa8CHS_*_*',
    'keep *_NjettinessCa15CHS_*_*',
    "keep *_patJetsCmsTopTagCHSPacked_*_*",
    "keep *_patJetsCmsTopTagCHSSubjets_*_*",
    "keep *_patJetsHepTopTagCHSPacked_*_*",
    "keep *_patJetsHepTopTagCHSSubjets_*_*",
    "keep *_prunedPrunedGenParticles_*_*",
    "keep *_electronIDValueMapProducer_eleFull5x5SigmaIEtaIEta_*",
    "keep *_egmGsfElectronIDs_*_*"
])

###############################################
# RECO AND GEN SETUP
process.load('Configuration.StandardSequences.Geometry_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = 'PHYS14_25_V3::All'

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
        'keep 11 <= abs(pdgId)  <= 16 && numberOfMothers()==1 && abs(mother().pdgId()) == 6'
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
# JET COLLECTIONS
#
# configure additional jet collections, based on
# chs.
process.chs = cms.EDFilter("CandPtrSelector", src = cms.InputTag("packedPFCandidates"), cut = cms.string("fromPV"))

process.ca8CHSJets  = ca8PFJets.clone (src = 'chs', doAreaFastjet = True, jetPtMin = fatjet_ptmin)
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



###############################################
# PAT JETS and Gen Jets
#
# 'Patify' the jet collections defined above and also add the corresponding
# genjets.
from PhysicsTools.PatAlgos.tools.jetTools import *
process.load('PhysicsTools.PatAlgos.slimming.unpackedTracksAndVertices_cfi')


# common parameters for the addJetCollection function, see below.
common_btag_parameters = dict(trackSource = cms.InputTag('unpackedTracksAndVertices'),
    pfCandidates = cms.InputTag('packedPFCandidates'),
    pvSource = cms.InputTag('offlineSlimmedPrimaryVertices'),
    svSource = cms.InputTag('unpackedTracksAndVertices', 'secondary'),
    btagDiscriminators = bTagDiscriminators
)

# captitalize string
def cap(s): return s[0].upper() + s[1:]

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
# * in case add_genjets is true, a genjet collection with name groomed_jets_name + 'Gen'
#   - if also the ungroomed jet collection has been added, the corresponding GenJet collection is also added
#
# Note that gen jets are currently only working for the fat, ungroomed jets, and the subjets, but not for the
# groomed fat jets; this is a restriction of PAT.
def add_fatjets_subjets(process, fatjets_name, groomed_jets_name, jetcorr_label = 'AK7PFchs', jetcorr_label_subjets = 'AK4PFchs', add_genjets = False):
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
    
    subjets_name = groomed_jets_name + 'Subjets' # e.g. CA8CHS + pruned + Subjets
    
    if add_genjets:
        #raise RuntimeError, "not yet implemented"
        groomed_jetproducer = getattr(process, groomed_jets_name)
        if groomed_jetproducer.type_() in ('FastjetJetProducer', 'CATopJetProducer'):
            setattr(process, groomed_jets_name + 'Gen', groomed_jetproducer.clone(src = cms.InputTag('packedGenParticles'), jetType = 'GenJet'))
        else:
            raise RuntimeError, "do not know how to construct genjet collection for %s" % repr(groomed_jetproducer)
        # add for ungroomed jets if not done yet (maybe never used in case ungroomed are not added,
        # but that's ok ..)
        if not hasattr(process, fatjets_name + 'Gen'):
            ungroomed_jetproducer = getattr(process, fatjets_name)
            assert ungroomed_jetproducer.type_() == 'FastjetJetProducer'
            setattr(process, fatjets_name + 'Gen', ungroomed_jetproducer.clone(src = cms.InputTag('packedGenParticles'), jetType = 'GenJet'))
        

    # patify ungroomed jets, if not already done:
    add_ungroomed = not hasattr(process, 'patJets' + fatjets_name)
    if add_ungroomed:
        addJetCollection(process, labelName = fatjets_name, jetSource = cms.InputTag(fatjets_name), algo = algo, rParam = rParam,
            jetCorrections = (jetcorr_label, cms.vstring(['L1FastJet', 'L2Relative', 'L3Absolute']), 'None'),
            genJetCollection = cms.InputTag(fatjets_name + 'Gen'),
            **common_btag_parameters
        )
    
    # patify groomed fat jets, with b-tagging:
    addJetCollection(process, labelName = groomed_jets_name, jetSource = cms.InputTag(groomed_jets_name), algo = algo, rParam = rParam,
       jetCorrections = (jetcorr_label, cms.vstring(['L1FastJet', 'L2Relative', 'L3Absolute']), 'None'),
       #genJetCollection = cms.InputTag(groomed_jets_name + 'Gen'), # nice try, but PAT looks for GenJets, whereas jets with subjets are BasicJets, so PAT cannot be used for this matching ...
       **common_btag_parameters)
    # patify subjets, with subjet b-tagging:
    addJetCollection(process, labelName = subjets_name, jetSource = cms.InputTag(groomed_jets_name, 'SubJets'), algo = algo, rParam = rParam,
        jetCorrections = (jetcorr_label_subjets, cms.vstring(['L1FastJet', 'L2Relative', 'L3Absolute']), 'None'),
        explicitJTA = True,
        svClustering = True,
        fatJets = cms.InputTag(fatjets_name), groomedFatJets = cms.InputTag(groomed_jets_name),
        genJetCollection = cms.InputTag(groomed_jets_name + 'Gen', 'SubJets'),
        **common_btag_parameters)
        
    # add the merged jet collection which contains the links from fat jets to subjets:
    setattr(process, 'patJets' + cap(groomed_jets_name) + 'Packed',cms.EDProducer("BoostedJetMerger",
        jetSrc=cms.InputTag("patJets" + cap(groomed_jets_name)),
        subjetSrc=cms.InputTag("patJets" + cap(subjets_name))))
        
    # adapt all for b-tagging, and switch off some PAT features not supported in miniAOD:
    module_names = [subjets_name, groomed_jets_name]
    if add_ungroomed: module_names += [fatjets_name]
    for name in module_names:
        if hasattr(process,'pfInclusiveSecondaryVertexFinderTagInfos' + cap(name)):
            producer = getattr(process,'pfInclusiveSecondaryVertexFinderTagInfos' + cap(name))
            producer.extSVCollection = cms.InputTag('slimmedSecondaryVertices')
        getattr(process, 'patJetCorrFactors' + cap(name)).primaryVertices = 'offlineSlimmedPrimaryVertices'
        getattr(process, 'patJetPartonMatch' + cap(name)).matched = 'prunedGenParticles'
        producer = getattr(process, 'patJets' + cap(name))
        producer.addJetCharge = False
        producer.addAssociatedTracks = False
        producer.addGenJetMatch = add_genjets
        # for fat groomed jets, gen jet match and jet flavor is not working, so switch it off:
        if name == groomed_jets_name:
            producer.addGenJetMatch = False
            producer.getJetMCFlavour = False


add_fatjets_subjets(process, 'ca8CHSJets', 'ca8CHSJetsPruned', add_genjets = True)
add_fatjets_subjets(process, 'ca15CHSJets', 'ca15CHSJetsFiltered', add_genjets = True)

add_fatjets_subjets(process, 'ca8CHSJets', 'cmsTopTagCHS', add_genjets = True)
add_fatjets_subjets(process, 'ca15CHSJets', 'hepTopTagCHS', add_genjets = True)

# configure PAT for miniAOD:
process.patJetPartons.particles = 'prunedGenParticles'

from PhysicsTools.PatAlgos.tools.pfTools import *
## Adapt primary vertex collection
adaptPVs(process, pvCollection = cms.InputTag('offlineSlimmedPrimaryVertices'))


# Add subjet variables (on ungroomed jets only!)
from RecoJets.JetProducers.nJettinessAdder_cfi import Njettiness
from RecoJets.JetProducers.qjetsadder_cfi import QJetsAdder

process.NjettinessCa8CHS = Njettiness.clone(src = cms.InputTag("patJetsCa8CHSJets"), cone = cms.double(0.8))
process.NjettinessCa15CHS = Njettiness.clone(src = cms.InputTag("patJetsCa15CHSJets"), cone = cms.double(1.5))
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


# electron ids for PHYS14:
from PhysicsTools.SelectorUtils.tools.vid_id_tools import *
process.load("RecoEgamma.ElectronIdentification.egmGsfElectronIDs_cfi")
process.egmGsfElectronIDs.physicsObjectSrc = cms.InputTag('slimmedElectrons')
setupAllVIDIdsInModule(process, 'RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_PHYS14_PU20bx25_V1_miniAOD_cff', setupVIDElectronSelection)

# for the electron id, we need the full 5x5 ietaieta, which has to be produced:
process.load('RecoEgamma.ElectronIdentification.ElectronIDValueMapProducer_cfi')
process.electronIDValueMapProducer.ebReducedRecHitCollection = cms.InputTag('reducedEgamma:reducedEBRecHits')
process.electronIDValueMapProducer.eeReducedRecHitCollection = cms.InputTag('reducedEgamma:reducedEERecHits')
process.electronIDValueMapProducer.esReducedRecHitCollection = cms.InputTag('reducedEgamma:reducedESRecHits')
process.electronIDValueMapProducer.src = cms.InputTag('slimmedElectrons')
process.electronIDValueMapProducer.dataFormat = 'PAT'

#NtupleWriter
process.MyNtuple = cms.EDFilter('NtupleWriter',
                                  #AnalysisModule = cms.PSet(
                                  #    name = cms.string("NoopAnalysisModule"),
                                  #    library = cms.string("libUHH2examples.so"),
                                  #    # Note: all other settings of type string are passed to the module, e.g.:
                                  #    TestKey = cms.string("TestValue")
                                  #),
                                  fileName = cms.string("Ntuple.root"), 
                                  runOnMiniAOD = cms.bool(True),
                                  doElectrons = cms.bool(True),
                                  doMuons = cms.bool(True),
                                  doTaus = cms.bool(True),
                                  doJets = cms.bool(True),
                                  doTopJets = cms.bool(True),
                                  doGenJets = cms.bool(not useData),
                                  doPhotons = cms.bool(False),
                                  doMET = cms.bool(True),
                                  doPV = cms.bool(True),
                                  doRho = cms.untracked.bool(True),
                                  doGenInfo = cms.bool(not useData),
                                  doAllGenParticles = cms.bool(False), #set to true if you want to store all gen particles, otherwise, only prunedPrunedGenParticles are stored (see above)
                                  doTrigger = cms.bool(True),
                                  rho_source = cms.InputTag("fixedGridRhoFastjetAll"),
                                  genparticle_source = cms.InputTag("prunedPrunedGenParticles" ),
                                  stablegenparticle_source = cms.InputTag("packedGenParticles" ),
                                  electron_source = cms.InputTag("slimmedElectrons"),
                                  electron_id_sources = cms.PSet (
                                     # use the Electron::tag enumeration as parameter name; value should be the InputTag
                                     # to use to read the ValueMap<float> from.
                                     eid_PHYS14_20x25_veto = cms.InputTag('egmGsfElectronIDs:cutBasedElectronID-PHYS14-PU20bx25-V1-miniAOD-standalone-veto'),
                                     eid_PHYS14_20x25_loose = cms.InputTag('egmGsfElectronIDs:cutBasedElectronID-PHYS14-PU20bx25-V1-miniAOD-standalone-loose'),
                                     eid_PHYS14_20x25_medium = cms.InputTag('egmGsfElectronIDs:cutBasedElectronID-PHYS14-PU20bx25-V1-miniAOD-standalone-medium'),
                                     eid_PHYS14_20x25_tight = cms.InputTag('egmGsfElectronIDs:cutBasedElectronID-PHYS14-PU20bx25-V1-miniAOD-standalone-tight')
                                  ),
                                  muon_sources = cms.vstring("slimmedMuons"),
                                  tau_sources = cms.vstring("slimmedTaus" ),
                                  tau_ptmin = cms.double(0.0),
                                  tau_etamax = cms.double(999.0),
                                  jet_sources = cms.vstring("slimmedJets","slimmedJetsAK8"),
                                  jet_ptmin = cms.double(10.0),
                                  jet_etamax = cms.double(5.0),
                                  genjet_sources = cms.vstring("slimmedGenJets", "ca8CHSJetsGen", "ca15CHSJetsGen"),
                                  genjet_ptmin = cms.double(10.0),
                                  genjet_etamax = cms.double(5.0),
                                  #photon_sources = cms.vstring("selectedPatPhotons"),
                                  topjet_sources = cms.vstring("patJetsHepTopTagCHSPacked", "patJetsCmsTopTagCHSPacked", "patJetsCa8CHSJetsPrunedPacked", "patJetsCa15CHSJetsFilteredPacked"),
                                  # jets to match to the topjets in order to get njettiness, in the same order as topjet_sources:
                                  topjet_substructure_variables_sources = cms.vstring("patJetsCa15CHSJets", "patJetsCa8CHSJets", "patJetsCa8CHSJets", "patJetsCa15CHSJets"),
                                  topjet_njettiness_sources = cms.vstring("NjettinessCa15CHS", "NjettinessCa8CHS", "NjettinessCa8CHS", "NjettinessCa15CHS"),
                                  topjet_qjets_sources = cms.vstring("QJetsCa15CHS", "QJetsCa8CHS", "QJetsCa8CHS", "QJetsCa15CHS"),
                                  
                                  topjet_ptmin = cms.double(100.0), 
                                  topjet_etamax = cms.double(5.0),
                                  #missing in miniaod
                                  doGenTopJets = cms.bool(True),
                                  gentopjet_sources = cms.vstring("ca8CHSJetsPrunedGen", "ca15CHSJetsFilteredGen", "cmsTopTagCHSGen", "hepTopTagCHSGen"),
                                  gentopjet_ptmin = cms.double(50.0), 
                                  gentopjet_etamax = cms.double(5.0),
                                  #missing in miniaod
                                  doGenJetsWithParts = cms.bool(False),
                                  #genjetwithparts_sources = cms.vstring("slimmedGenJets"),
                                  #genjetwithparts_ptmin = cms.double(10.0), 
                                  #genjetwithparts_etamax = cms.double(5.0),
                                  met_sources =  cms.vstring("slimmedMETs"),
                                  pv_sources = cms.vstring("offlineSlimmedPrimaryVertices"),
                                  trigger_bits = cms.InputTag("TriggerResults","","HLT"),
                                  trigger_prefixes = cms.vstring(#"HLT_IsoMu", "HLT_Mu",
                                                                 #"HLT_L1SingleMu", "HLT_L2Mu",
                                                                 #"HLT_Ele",
                                                                 "HLT_",
                                                                 #"HLT_DoubleMu", "HLT_DoubleEle"
	                                                         ),
                                  
)

#process.content = cms.EDAnalyzer("EventContentAnalyzer")

# Note: we run in unscheduled mode, i.e. all modules are run as required; just make sure that MyNtuple runs:
process.p = cms.Path(process.MyNtuple)
#process.end = cms.EndPath(process.out)

