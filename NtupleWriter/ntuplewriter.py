import FWCore.ParameterSet.Config as cms


# This configuration runs a number of (substructure) jet algortihms
# on top of either AOD or MiniAOD and then runs the NtupleWriter
# to write out the SFrame ntuple and/or run a uhh2::AnalysisModule.
#
# In the current configuration, the cmsRun job will produce both, a
# miniAOD file (including the additional top jet collections) AND a SFrame
# ntuple.

useMiniAOD = True
useData = False

# AOD
pfcandidates          = 'particleFlow'
chsstring             = 'pfNoPileUpJME'
genjetparticles       = 'genParticles'
importantgenparticles = 'genParticles'
tracks                = 'generalTracks'
vertices              = 'offlinePrimaryVertices'
mergedvertices        = 'inclusiveMergedVertices' 
mergedvertices2       = '' 
primaryvertices       = 'offlinePrimaryVertices'

#miniAOD
if useMiniAOD:
  pfcandidates          = 'packedPFCandidates'
  genjetparticles       = 'packedGenParticles'
  importantgenparticles = 'prunedGenParticles'
  tracks                = 'unpackedTracksAndVertices'
  vertices              = 'unpackedTracksAndVertices'
  mergedvertices        = 'unpackedTracksAndVertices'
  mergedvertices2       = 'secondary'
  primaryvertices       = 'offlineSlimmedPrimaryVertices'


print 'useMiniAOD = '+str(useMiniAOD)
print ' pfcandidates          = '+pfcandidates         
print ' genjetparticles       = '+genjetparticles      
print ' importantgenparticles = '+importantgenparticles
print ' tracks                = '+tracks               
print ' vertices              = '+vertices             
print ' mergedvertices        = '+mergedvertices       
print ' mergedvertices2       = '+mergedvertices2
print ' primaryvertices       = '+primaryvertices 

process = cms.Process("USER")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(100)
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) , allowUnscheduled = cms.untracked.bool(True) )

process.source = cms.Source("PoolSource",
                            fileNames  = cms.untracked.vstring("/store/mc/Phys14DR/TprimeTprime_M_1000_Tune4C_13TeV-madgraph/MINIAODSIM/PU40bx25_PHYS14_25_V1-v1/10000/F6F71931-6F67-E411-ABA2-20CF3027A5C9.root"),
                            skipEvents = cms.untracked.uint32(0)
)

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1000))


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
    'keep *_patJetsCA8CHS_*_*',
    'keep *_patJetsCA15CHS_*_*',
    "keep *_patJetsCMSTopTagCHSPacked_*_*",
    "keep *_patJetsCMSTopTagCHSSubjets_*_*",
    "keep *_patJetsHEPTopTagCHSPacked_*_*",
    "keep *_patJetsHEPTopTagCHSSubjets_*_*",
])

###############################################
# RECO AND GEN SETUP
process.load('Configuration.StandardSequences.Geometry_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = 'START70_V6::All'

from RecoJets.Configuration.RecoPFJets_cff import *
from RecoJets.JetProducers.fixedGridRhoProducerFastjet_cfi import *

process.fixedGridRhoFastjetAll = fixedGridRhoFastjetAll.clone(pfCandidatesTag = pfcandidates)

from RecoJets.JetProducers.SubJetParameters_cfi import SubJetParameters
from RecoJets.JetProducers.PFJetParameters_cfi import *
from RecoJets.JetProducers.CaloJetParameters_cfi import *
from RecoJets.JetProducers.AnomalousCellParameters_cfi import *
from RecoJets.JetProducers.CATopJetParameters_cfi import *
from RecoJets.JetProducers.GenJetParameters_cfi import *
from RecoJets.JetProducers.caTopTaggers_cff import *

###############################################
# GEN JETS


from RecoJets.JetProducers.ca4GenJets_cfi import ca4GenJets
from RecoJets.JetProducers.ak5GenJets_cfi import ak5GenJets

process.ca8GenJets = ca4GenJets.clone( rParam = cms.double(0.8),
                                           src = cms.InputTag(genjetparticles))

process.ak3GenJets = ak5GenJets.clone( rParam = cms.double(0.3),
                                           src = cms.InputTag(genjetparticles))
                                           
process.ca8GenJetsPruned = ca4GenJets.clone(
  rParam = cms.double(0.8),
  src = cms.InputTag(genjetparticles),
  usePruning = cms.bool(True),
  zcut = cms.double(0.1),
  rcut_factor = cms.double(0.5),
  nFilt = cms.int32(2),
  writeCompound = cms.bool(True),
  jetCollInstanceName=cms.string("SubJets")
  )

if useMiniAOD:
  process.chs = cms.EDFilter("CandPtrSelector", src = cms.InputTag("packedPFCandidates"), cut = cms.string("fromPV"))
  chsstring = 'chs'
  
process.ca8CHSJets  = ca8PFJets.clone (src = chsstring)

from RecoJets.JetProducers.ak4PFJetsPruned_cfi import *
process.ca8CHSJetsPruned = ak4PFJetsPruned.clone(rParam=0.8, jetAlgorithm = cms.string("CambridgeAachen"), doAreaFastjet = True, src = chsstring)

# CATopJet PF Jets with adjacency 
#process.cmsTopTagCHS = cmsTopTagPFJetsCHS.clone()
process.cmsTopTagCHS = cms.EDProducer(
    "CATopJetProducer",
    PFJetParameters.clone( src = cms.InputTag(chsstring),
                           doAreaFastjet = cms.bool(True),
                           doRhoFastjet = cms.bool(False),
                           jetPtMin = cms.double(100.0)
                           ),
    AnomalousCellParameters,
    CATopJetParameters.clone( jetCollInstanceName = cms.string("SubJets"), 
                              verbose = cms.bool(False),
                              algorithm = cms.int32(1),                 # 0 = KT, 1 = CA, 2 = anti-KT
                              tagAlgo = cms.int32(0),                   #0=legacy top
                              useAdjacency = cms.int32(2),              # modified adjacency
                              centralEtaCut = cms.double(2.5),          # eta for defining "central" jets
                              sumEtBins = cms.vdouble(0,1600,2600),     # sumEt bins over which cuts vary. vector={bin 0 lower bound, bin 1 lower bound, ...}
                              rBins = cms.vdouble(0.8,0.8,0.8),         # Jet distance paramter R. R values depend on sumEt bins.
                              ptFracBins = cms.vdouble(0.05,0.05,0.05), # minimum fraction of central jet pt for subjets (deltap)
                              deltarBins = cms.vdouble(0.19,0.19,0.19), # Applicable only if useAdjacency=1. deltar adjacency values for each sumEtBin
                              nCellBins = cms.vdouble(1.9,1.9,1.9), 
                            ),
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

process.ca15CHSJets = process.hepTopTagCHS.clone ( writeCompound = cms.bool(False), jetCollInstanceName = cms.string('') )


process.CATopTagInfos = cms.EDProducer("CATopJetTagger",
                                    src = cms.InputTag("cmsTopTagCHS"),
                                    TopMass = cms.double(171),
                                    TopMassMin = cms.double(0.),
                                    TopMassMax = cms.double(250.),
                                    WMass = cms.double(80.4),
                                    WMassMin = cms.double(0.0),
                                    WMassMax = cms.double(200.0),
                                    MinMassMin = cms.double(0.0),
                                    MinMassMax = cms.double(200.0),
                                    verbose = cms.bool(False)
                                    )

process.hepTopTagCHSTagInfos = process.CATopTagInfos.clone(
  src = cms.InputTag("hepTopTagCHS")
)

###############################################
# PAT JETS
from PhysicsTools.PatAlgos.tools.jetTools import *

process.load('RecoBTag.Configuration.RecoBTag_cff')
process.load('RecoJets.Configuration.RecoJetAssociations_cff')
process.load('PhysicsTools.PatAlgos.slimming.unpackedTracksAndVertices_cfi')

# fix JTA (see https://github.com/cms-sw/cmssw/tree/CMSSW_7_0_X/RecoJets/JetAssociationProducers/python)
process.load('RecoJets.JetAssociationProducers.ak5JTA_cff')
process.ak5JetTracksAssociatorAtVertexPF.tracks = cms.InputTag(tracks)
process.impactParameterTagInfos.primaryVertex = cms.InputTag(vertices)
process.inclusiveSecondaryVertexFinderTagInfos.extSVCollection = cms.InputTag(mergedvertices,mergedvertices2,"")
process.combinedSecondaryVertex.trackMultiplicityMin = 1

#patJetsCA8CHS
addJetCollection(
    process,
    labelName = 'CA8CHS',
    jetSource = cms.InputTag('ca8CHSJets'),
    algo = 'ca8',
    rParam = 0.8,
    jetCorrections = ('AK7PFchs', cms.vstring(['L1FastJet', 'L2Relative', 'L3Absolute']), 'None'),
    trackSource = cms.InputTag(tracks),
    pvSource = cms.InputTag(vertices),
    btagDiscriminators = ['combinedSecondaryVertexBJetTags'],
    )
process.patJetPartonMatchCA8CHS.matched=importantgenparticles
process.patJetCorrFactorsCA8CHS.primaryVertices = primaryvertices
process.patJetGenJetMatchCA8CHS.matched = 'ca8GenJets'#'slimmedGenJets'
process.jetTracksAssociatorAtVertexCA8CHS=process.ak5JetTracksAssociatorAtVertexPF.clone(jets = cms.InputTag('ca8CHSJets'), coneSize = 0.8)
process.secondaryVertexTagInfosCA8CHS.trackSelection.jetDeltaRMax = cms.double(0.8) # default is 0.3
process.secondaryVertexTagInfosCA8CHS.vertexCuts.maxDeltaRToJetAxis = cms.double(0.8) # default is 0.5
process.combinedSecondaryVertexCA8CHS= process.combinedSecondaryVertex.clone()
process.combinedSecondaryVertexCA8CHS.trackSelection.jetDeltaRMax = cms.double(0.8)
process.combinedSecondaryVertexCA8CHS.trackPseudoSelection.jetDeltaRMax = cms.double(0.8)
process.combinedSecondaryVertexBJetTagsCA8CHS.jetTagComputer = cms.string('combinedSecondaryVertexCA8CHS')



# patJetsCA8CHSpruned
addJetCollection(
    process,
    labelName = 'CA8CHSpruned',
    jetSource = cms.InputTag('ca8CHSJetsPruned'),
    jetCorrections = ('AK7PFchs', cms.vstring(['L1FastJet', 'L2Relative', 'L3Absolute']), 'None'),
    trackSource = cms.InputTag(tracks),
    pvSource = cms.InputTag(vertices),
    btagDiscriminators = ['combinedSecondaryVertexBJetTags'],
    )
process.patJetPartonMatchCA8CHSpruned.matched=importantgenparticles
process.patJetCorrFactorsCA8CHSpruned.primaryVertices = primaryvertices
process.patJetGenJetMatchCA8CHSpruned.matched = 'ca8GenJetsPruned'#'slimmedGenJets'
process.patJetPartonMatchCA8CHSpruned.matched = importantgenparticles
process.jetTracksAssociatorAtVertexCA8CHSpruned=process.ak5JetTracksAssociatorAtVertexPF.clone(jets = cms.InputTag('ca8CHSJetsPruned'), coneSize = 0.8)
process.secondaryVertexTagInfosCA8CHSpruned.trackSelection.jetDeltaRMax = cms.double(0.8) # default is 0.3
process.secondaryVertexTagInfosCA8CHSpruned.vertexCuts.maxDeltaRToJetAxis = cms.double(0.8) # default is 0.5
process.combinedSecondaryVertexCA8CHSpruned= process.combinedSecondaryVertex.clone()
process.combinedSecondaryVertexCA8CHSpruned.trackSelection.jetDeltaRMax = cms.double(0.8)
process.combinedSecondaryVertexCA8CHSpruned.trackPseudoSelection.jetDeltaRMax = cms.double(0.8)
process.combinedSecondaryVertexBJetTagsCA8CHSpruned.jetTagComputer = cms.string('combinedSecondaryVertexCA8CHSpruned')

addJetCollection(
    process,
    labelName = 'CA8CHSprunedSubjets',
    jetSource = cms.InputTag('ca8CHSJetsPruned','SubJets'),
    jetCorrections = ('AK7PFchs', cms.vstring(['L1FastJet', 'L2Relative', 'L3Absolute']), 'None'),
    trackSource = cms.InputTag(tracks),
    pvSource = cms.InputTag(vertices),
    btagDiscriminators = ['combinedSecondaryVertexBJetTags'],
    getJetMCFlavour = False,
    )

process.patJetPartonMatchCA8CHSprunedSubjets.matched=importantgenparticles
process.patJetCorrFactorsCA8CHSprunedSubjets.primaryVertices = primaryvertices
process.patJetGenJetMatchCA8CHSprunedSubjets.matched = 'ak3GenJets'#slimmedGenJets'
process.patJetPartonMatchCA8CHSprunedSubjets.matched = importantgenparticles

process.patJetsCA8CHSprunedPacked = cms.EDProducer("BoostedJetMerger",
    jetSrc=cms.InputTag("patJetsCA8CHSpruned" ),
    subjetSrc=cms.InputTag("patJetsCA8CHSprunedSubjets")
      )

# patJetsCMSTopTagCHS
addJetCollection(
    process,
    labelName = 'CMSTopTagCHS',
    jetSource = cms.InputTag('cmsTopTagCHS'),
    jetCorrections = ('AK7PFchs', cms.vstring(['L1FastJet', 'L2Relative', 'L3Absolute']), 'None'),
    trackSource = cms.InputTag(tracks),
    pvSource = cms.InputTag(vertices),
    btagDiscriminators = ['combinedSecondaryVertexBJetTags'],
        # btagInfos = [
        # 'CATopTagInfos'
        #  ]   
    )
process.patJetPartonMatchCMSTopTagCHS.matched=importantgenparticles
process.patJetCorrFactorsCMSTopTagCHS.primaryVertices = primaryvertices
process.patJetGenJetMatchCMSTopTagCHS.matched = 'ca8GenJetsPruned'#'slimmedGenJets'
process.patJetPartonMatchCMSTopTagCHS.matched = importantgenparticles
process.jetTracksAssociatorAtVertexCMSTopTagCHS=process.ak5JetTracksAssociatorAtVertexPF.clone(jets = cms.InputTag('cmsTopTagCHS'), coneSize = 0.8)
process.secondaryVertexTagInfosCMSTopTagCHS.trackSelection.jetDeltaRMax = cms.double(0.8) # default is 0.3
process.secondaryVertexTagInfosCMSTopTagCHS.vertexCuts.maxDeltaRToJetAxis = cms.double(0.8) # default is 0.5
process.combinedSecondaryVertexCMSTopTagCHS= process.combinedSecondaryVertex.clone()
process.combinedSecondaryVertexCMSTopTagCHS.trackSelection.jetDeltaRMax = cms.double(0.8)
process.combinedSecondaryVertexCMSTopTagCHS.trackPseudoSelection.jetDeltaRMax = cms.double(0.8)
process.combinedSecondaryVertexBJetTagsCMSTopTagCHS.jetTagComputer = cms.string('combinedSecondaryVertexCMSTopTagCHS')
process.patJetsCMSTopTagCHS.addTagInfos = True
process.patJetsCMSTopTagCHS.tagInfoSources = cms.VInputTag(
    cms.InputTag('CATopTagInfos')
    )

addJetCollection(
    process,
    labelName = 'CMSTopTagCHSSubjets',
    jetSource = cms.InputTag('cmsTopTagCHS','SubJets'),
    jetCorrections = ('AK7PFchs', cms.vstring(['L1FastJet', 'L2Relative', 'L3Absolute']), 'None'),
    trackSource = cms.InputTag(tracks),
    pvSource = cms.InputTag(vertices),
    btagDiscriminators = ['combinedSecondaryVertexBJetTags'],
    getJetMCFlavour = False,
    )
process.patJetPartonMatchCMSTopTagCHSSubjets.matched=importantgenparticles
process.patJetCorrFactorsCMSTopTagCHSSubjets.primaryVertices = primaryvertices
process.patJetGenJetMatchCMSTopTagCHSSubjets.matched = 'cmsTopTagGENSubJets'#slimmedGenJets'
process.patJetPartonMatchCMSTopTagCHSSubjets.matched = importantgenparticles

process.patJetsCMSTopTagCHSPacked = cms.EDProducer("BoostedJetMerger",
    jetSrc=cms.InputTag("patJetsCMSTopTagCHS" ),
    subjetSrc=cms.InputTag("patJetsCMSTopTagCHSSubjets")
      )



#patJetsCA15CHS
addJetCollection(
    process,
    labelName = 'CA15CHS',
    jetSource = cms.InputTag('ca15CHSJets'),
#    algo = 'ca15',
#    rParam = 1.5,
    jetCorrections = ('AK7PFchs', cms.vstring(['L1FastJet', 'L2Relative', 'L3Absolute']), 'None'),
    trackSource = cms.InputTag(tracks),
    pvSource = cms.InputTag(vertices),
    btagDiscriminators = ['combinedSecondaryVertexBJetTags'],
    )
process.patJetPartonMatchCA15CHS.matched=importantgenparticles
process.patJetCorrFactorsCA15CHS.primaryVertices = primaryvertices
process.patJetGenJetMatchCA15CHS.matched = 'ca15GenJets'#'slimmedGenJets'
process.jetTracksAssociatorAtVertexCA15CHS=process.ak5JetTracksAssociatorAtVertexPF.clone(jets = cms.InputTag('ca15CHSJets'), coneSize = 1.5)
process.secondaryVertexTagInfosCA15CHS.trackSelection.jetDeltaRMax = cms.double(1.5) # default is 0.3
process.secondaryVertexTagInfosCA15CHS.vertexCuts.maxDeltaRToJetAxis = cms.double(1.5) # default is 0.5
process.combinedSecondaryVertexCA15CHS= process.combinedSecondaryVertex.clone()
process.combinedSecondaryVertexCA15CHS.trackSelection.jetDeltaRMax = cms.double(1.5)
process.combinedSecondaryVertexCA15CHS.trackPseudoSelection.jetDeltaRMax = cms.double(1.5)
process.combinedSecondaryVertexBJetTagsCA15CHS.jetTagComputer = cms.string('combinedSecondaryVertexCA15CHS')


# patJetsHEPTopTagCHS
addJetCollection(
    process,
    labelName = 'HEPTopTagCHS',
    jetSource = cms.InputTag('hepTopTagCHS'),
    jetCorrections = ('AK7PFchs', cms.vstring(['L1FastJet', 'L2Relative', 'L3Absolute']), 'None'),
    trackSource = cms.InputTag(tracks),
    pvSource = cms.InputTag(vertices),
    btagDiscriminators = ['combinedSecondaryVertexBJetTags'],
    )
process.patJetPartonMatchHEPTopTagCHS.matched=importantgenparticles
process.patJetCorrFactorsHEPTopTagCHS.primaryVertices = primaryvertices
process.patJetGenJetMatchHEPTopTagCHS.matched = 'ca8GenJetsPruned'#'slimmedGenJets'
process.patJetPartonMatchHEPTopTagCHS.matched = importantgenparticles
process.jetTracksAssociatorAtVertexHEPTopTagCHS=process.ak5JetTracksAssociatorAtVertexPF.clone(jets = cms.InputTag('hepTopTagCHS'), coneSize = 1.5)
process.secondaryVertexTagInfosHEPTopTagCHS.trackSelection.jetDeltaRMax = cms.double(1.5) # default is 0.3
process.secondaryVertexTagInfosHEPTopTagCHS.vertexCuts.maxDeltaRToJetAxis = cms.double(1.5) # default is 0.5
process.combinedSecondaryVertexHEPTopTagCHS= process.combinedSecondaryVertex.clone()
process.combinedSecondaryVertexHEPTopTagCHS.trackSelection.jetDeltaRMax = cms.double(1.5)
process.combinedSecondaryVertexHEPTopTagCHS.trackPseudoSelection.jetDeltaRMax = cms.double(1.5)
process.combinedSecondaryVertexBJetTagsHEPTopTagCHS.jetTagComputer = cms.string('combinedSecondaryVertexHEPTopTagCHS')
process.patJetsHEPTopTagCHS.addTagInfos = True
process.patJetsHEPTopTagCHS.tagInfoSources = cms.VInputTag(
    cms.InputTag('hepTopTagCHSTagInfos')
    )

addJetCollection(
    process,
    labelName = 'HEPTopTagCHSSubjets',
    jetSource = cms.InputTag('hepTopTagCHS','SubJets'),
    jetCorrections = ('AK7PFchs', cms.vstring(['L1FastJet', 'L2Relative', 'L3Absolute']), 'None'),
    trackSource = cms.InputTag(tracks),
    pvSource = cms.InputTag(vertices),
    btagDiscriminators = ['combinedSecondaryVertexBJetTags'],
    getJetMCFlavour = False,
    )
process.patJetPartonMatchHEPTopTagCHSSubjets.matched=importantgenparticles
process.patJetCorrFactorsHEPTopTagCHSSubjets.primaryVertices = primaryvertices
process.patJetGenJetMatchHEPTopTagCHSSubjets.matched = 'ak3GenJets'#slimmedGenJets'
process.patJetPartonMatchHEPTopTagCHSSubjets.matched = importantgenparticles

process.patJetsHEPTopTagCHSPacked = cms.EDProducer("BoostedJetMerger",
    jetSrc=cms.InputTag("patJetsHEPTopTagCHS" ),
    subjetSrc=cms.InputTag("patJetsHEPTopTagCHSSubjets")
      )

###############################################
# Modifify all

for jetcoll in (process.patJetsCA8CHS,
                process.patJetsCA15CHS,
                process.patJetsCA8CHSpruned,
                process.patJetsCA8CHSprunedSubjets,
                process.patJetsCMSTopTagCHS,
                process.patJetsCMSTopTagCHSSubjets,
                process.patJetsHEPTopTagCHS,
                process.patJetsHEPTopTagCHSSubjets,
                ) :
        jetcoll.embedGenJetMatch = False
        jetcoll.getJetMCFlavour = False
        jetcoll.addGenPartonMatch = False

#NtupleWriter
process.MyNtuple = cms.EDFilter('NtupleWriter',
                                  #AnalysisModule = cms.PSet(
                                  #    name = cms.string("NoopAnalysisModule"),
                                  #    library = cms.string("libUHH2examples.so"),
                                  #    # Note: all other settings of type string are passed to the module, e.g.:
                                  #    TestKey = cms.string("TestValue")
                                  #),
                                  fileName = cms.string("Ntuple.root"), 
                                  runOnMiniAOD = cms.bool(useMiniAOD),
                                  doElectrons = cms.bool(True),
                                  doMuons = cms.bool(True),
                                  doTaus = cms.bool(True),
                                  doJets = cms.bool(True),
                                  doJetsConstituents = cms.bool(False),
                                  doTopJets = cms.bool(True),
                                  doTopJetsConstituents = cms.bool(False),
                                  doGenJets = cms.bool(not useData),
                                  doPhotons = cms.bool(False),
                                  doMET = cms.bool(True),
                                  doPV = cms.bool(True),
                                  doRho = cms.untracked.bool(True),
                                  doAllPFConstituents = cms.bool(False),
                                  pf_constituents_sources = cms.vstring("packedPFCandidates"),
                                  storePFsAroundLeptons = cms.untracked.bool(False),
                                  doGenInfo = cms.bool(not useData),
                                  doAllGenParticles = cms.bool(False), #set to true if you want to store all gen particles, otherwise, only tops and status 3 particles are stored
                                  doTrigger = cms.bool(True),
                                  doTagInfos = cms.untracked.bool(False), # when set to true crashes for the 'packed' jet collections
                                  svComputer = cms.untracked.InputTag("combinedSecondaryVertex"),
                                  rho_source = cms.InputTag("fixedGridRhoFastjetAll"),
                                  genparticle_source = cms.InputTag("prunedGenParticles" ),
                                  stablegenparticle_source = cms.InputTag("packedGenParticles" ),
                                  electron_sources = cms.vstring("slimmedElectrons"),
                                  muon_sources = cms.vstring("slimmedMuons"),
                                  tau_sources = cms.vstring("slimmedTaus" ),
                                  tau_ptmin = cms.double(0.0),
                                  tau_etamax = cms.double(999.0),
                                  jet_sources = cms.vstring("slimmedJets","slimmedJetsAK8"),
                                  jet_ptmin = cms.double(10.0),
                                  jet_etamax = cms.double(5.0),
                                  genjet_sources = cms.vstring("slimmedGenJets"),
                                  genjet_ptmin = cms.double(10.0),
                                  genjet_etamax = cms.double(5.0),
                                  #photon_sources = cms.vstring("selectedPatPhotons"),
                                  #topjet_sources = cms.vstring("patJetsHEPTopTagCHSPacked","patJetsCMSTopTagCHSPacked","patJetsCA8CHSprunedPacked"),
                                  #topjet_constituents_sources = cms.vstring("patJetsHEPTopTagCHS","patJetsCMSTopTagCHS", "patJetsCA8CHSpruned"),
                                  topjet_sources = cms.vstring("patJetsCMSTopTagCHSPacked","patJetsHEPTopTagCHSPacked"),
                                  topjet_constituents_sources = cms.vstring("patJetsCA8CHS","patJetsCA15CHS"),
                                  topjet_ptmin = cms.double(100.0), 
                                  topjet_etamax = cms.double(5.0),
                                  pf_around_leptons_sources = cms.vstring("packedPFCandidates"),
                                  #missing in miniaod
                                  doGenTopJets = cms.bool(False),			      
                                  #gentopjet_sources = cms.vstring("caTopTagGen","ca12TopTagGen", "caFilteredGenJetsNoNu", "caHEPTopTagGen"),
                                  #gentopjet_ptmin = cms.double(150.0), 
                                  #gentopjet_etamax = cms.double(5.0),
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
process.end = cms.EndPath(process.out)
