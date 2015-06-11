import FWCore.ParameterSet.Config as cms

from RecoParticleFlow.PFProducer.particleFlowTmpPtrs_cfi import *
from CommonTools.ParticleFlow.pfNoPileUp_cff import *
from CommonTools.ParticleFlow.ParticleSelectors.pfAllChargedParticles_cfi import *
from CommonTools.ParticleFlow.ParticleSelectors.pfAllChargedHadrons_cfi import *
from CommonTools.ParticleFlow.ParticleSelectors.pfAllNeutralHadrons_cfi import *
from CommonTools.ParticleFlow.ParticleSelectors.pfAllPhotons_cfi import *

convertedPackedPFCandidates = cms.EDProducer('convertPackedCandToPFCand',
  src = cms.InputTag('packedPFCandidates')
)

convertedPackedPFCandidatePtrs = particleFlowTmpPtrs.clone(src = 'convertedPackedPFCandidates')

pfPileUp.PFCandidates = 'convertedPackedPFCandidatePtrs'
pfPileUp.Vertices = 'offlineSlimmedPrimaryVertices'
pfNoPileUp.bottomCollection = pfPileUp.PFCandidates

pfAllChargedParticles.src = 'pfNoPileUp'
pfAllChargedHadrons.src = 'pfNoPileUp'
pfAllNeutralHadrons.src = 'pfNoPileUp'
pfAllPhotons.src = 'pfNoPileUp'
pfPileUpAllChargedParticles = pfAllChargedParticles.clone(src = 'pfPileUp')

pfSelectorsSequence = cms.Sequence(
    pfAllChargedParticles
  + pfAllChargedHadrons
  + pfAllNeutralHadrons
  + pfAllPhotons
  + pfPileUpAllChargedParticles
)

pfCandidatesByTypeSequence = cms.Sequence(
    convertedPackedPFCandidates
  * convertedPackedPFCandidatePtrs
  * pfNoPileUpSequence
  * pfSelectorsSequence
)
