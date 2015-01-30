import FWCore.ParameterSet.Config as cms

process = cms.Process("NTUPLE")

process.source = cms.Source("PoolSource",
                            fileNames  = cms.untracked.vstring("/store/mc/Phys14DR/BprimeJetToTW_M800GeV_Tune4C_13TeV-madgraph-tauola/MINIAODSIM/PU20bx25_PHYS14_25_V1-v1/00000/86D970BD-5E69-E411-BA40-002590AC4CC8.root"),
                            skipEvents = cms.untracked.uint32(0)
)

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(10))

process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) , allowUnscheduled = cms.untracked.bool(True) )


process.prunedTmp = cms.EDProducer("GenParticlePruner",
    src = cms.InputTag("prunedGenParticles"),
    select = cms.vstring(
        'drop *',
        'keep status == 3',              # me in MadGraph
        'keep 20 <= status <= 30',       # me in Pythia8
        'keep 11 <= abs(pdgId)  <= 16',  # keep leptons (but: see next step in prunedPrunedGenParticles)
    )
)

process.prunedPrunedGenParticles = cms.EDProducer("GenParticlePruner",
    src = cms.InputTag("prunedTmp"),
    select = cms.vstring(
        'keep *',
        'drop 11 <= abs(pdgId) <= 16',  # drop leptons
        'keep+ abs(pdgId) == 24', # but keep from W ... 
        'keep+ abs(pdgId) == 23', # and from Z
        'keep+ abs(pdgId) == 25', # and from h0
    )
)

process.MyNtuple = cms.EDFilter('NtupleWriter',
                                  fileName = cms.string("ntuplewriter_genparticles.root"), 
                                  # rest of the configuration as in ntuplewriter.py on miniAOD ...
                                  runOnMiniAOD = cms.bool(True),
                                  doElectrons = cms.bool(False),
                                  doMuons = cms.bool(False),
                                  doTaus = cms.bool(False),
                                  doJets = cms.bool(False),
                                  doJetsConstituents = cms.bool(False),
                                  doPuppi = cms.bool(False),
                                  doTopJets = cms.bool(False),
                                  doTopJetsConstituents = cms.bool(False),
                                  doGenJets = cms.bool(False),
                                  doGenJetsWithParts = cms.bool(False),
                                  doGenTopJets = cms.bool(False),
                                  doPhotons = cms.bool(False),
                                  doMET = cms.bool(False),
                                  doPV = cms.bool(True),
                                  doRho = cms.untracked.bool(True),
                                  doAllPFConstituents = cms.bool(False),
                                  storePFsAroundLeptons = cms.untracked.bool(False),
                                  doGenInfo = cms.bool(True),
                                  doAllGenParticles = cms.bool(False),
                                  doTrigger = cms.bool(False),
                                  doTagInfos = cms.untracked.bool(False),
                                  rho_source = cms.InputTag("fixedGridRhoFastjetAll"),
                                  genparticle_source = cms.InputTag("prunedPrunedGenParticles"),
                                  pv_sources = cms.vstring("offlineSlimmedPrimaryVertices"),
)


process.p = cms.Path(process.MyNtuple)
