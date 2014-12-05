import FWCore.ParameterSet.Config as cms
process = cms.Process("TEST")
process.source = cms.Source("PoolSource", 
  # a file from: /TprimeJetToTH_M800GeV_Tune4C_13TeV-madgraph-tauola/Phys14DR-PU20bx25_PHYS14_25_V1-v1/MINIAODSIM
  #fileNames = cms.untracked.vstring('file:/nfs/dust/cms/user/ottjoc/24D444CE-216A-E411-ADC5-001E67398633.root')
  fileNames = cms.untracked.vstring('file:/nfs/dust/cms/user/ottjoc/gc-output/PHYS14v1/signals/MC_TpTp_M1000_40x25_0_miniaod.root')
)
process.load("SimGeneral.HepPDTESSource.pythiapdt_cfi")
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(10))

process.prunedTmp = cms.EDProducer("GenParticlePruner",
    src = cms.InputTag("prunedGenParticles"),
    select = cms.vstring(
        'drop *',
        'keep status == 3',              # me in MadGraph
        'keep 20 <= status <= 30',       # me in Pythia8
        'keep 11 <= abs(pdgId)  <= 16',  # leptons
    )
)

process.prunedPrunedGenParticles = cms.EDProducer("GenParticlePruner",
    src = cms.InputTag("prunedTmp"),
    select = cms.vstring(
        'drop 11 <= abs(pdgId) <= 16',  # drop leptons
        'keep+ abs(pdgId) == 24', # but keep from W ... 
        'keep+ abs(pdgId) == 23' # and from Z
    )
)

process.genpfilter = cms.EDFilter('FilterStrangeGenParticles',
  src = cms.InputTag("prunedPrunedGenParticles"),
  n_top_expected = cms.int32(-1)
)
process.genpprinter = cms.EDAnalyzer("ParticleTreeDrawer",
                                   src = cms.InputTag("prunedPrunedGenParticles"),
                                   #printP4 = cms.untracked.bool(False),
                                   #printPtEtaPhi = cms.untracked.bool(False),
                                   #printVertex = cms.untracked.bool(False),
                                   printStatus = cms.untracked.bool(True),
                                   printIndex = cms.untracked.bool(True),
                                   #status = cms.untracked.vint32( 3 )
)

process.genpprinter_orig = cms.EDAnalyzer("ParticleTreeDrawer",
                                   src = cms.InputTag("prunedGenParticles"),
                                   #printP4 = cms.untracked.bool(False),
                                   #printPtEtaPhi = cms.untracked.bool(False),
                                   #printVertex = cms.untracked.bool(False),
                                   printStatus = cms.untracked.bool(True),
                                   printIndex = cms.untracked.bool(True),
                                   #status = cms.untracked.vint32( 3 )
)

# note: do filtering on double-pruned collection, but then print both, double-pruned and pruned for debugging.
process.p = cms.Path(process.prunedTmp * process.prunedPrunedGenParticles * process.genpfilter * process.genpprinter * process.genpprinter_orig)
