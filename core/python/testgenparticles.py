import FWCore.ParameterSet.Config as cms
process = cms.Process("TEST")
process.source = cms.Source("PoolSource", 
  # a file from: /TprimeJetToTH_M800GeV_Tune4C_13TeV-madgraph-tauola/Phys14DR-PU20bx25_PHYS14_25_V1-v1/MINIAODSIM
  #fileNames = cms.untracked.vstring('file:/nfs/dust/cms/user/ottjoc/24D444CE-216A-E411-ADC5-001E67398633.root')
  #fileNames = cms.untracked.vstring('file:/nfs/dust/cms/user/ottjoc/gc-output/PHYS14v1/signals/MC_TpTp_M1000_40x25_0_miniaod.root')
  #fileNames = cms.untracked.vstring('/store/mc/Phys14DR/TTJets_MSDecaysCKM_central_Tune4C_13TeV-madgraph-tauola/MINIAODSIM/PU20bx25_PHYS14_25_V1-v1/00000/00C90EFC-3074-E411-A845-002590DB9262.root')
  #fileNames = cms.untracked.vstring('/store/mc/Phys14DR/TprimeJetToTH_M800GeV_Tune4C_13TeV-madgraph-tauola/MINIAODSIM/PU20bx25_PHYS14_25_V1-v1/00000/24D444CE-216A-E411-ADC5-001E67398633.root'),
  fileNames = cms.untracked.vstring('/store/mc/Phys14DR/BprimeJetToTW_M800GeV_Tune4C_13TeV-madgraph-tauola/MINIAODSIM/PU20bx25_PHYS14_25_V1-v1/00000/86D970BD-5E69-E411-BA40-002590AC4CC8.root'),
  skipEvents = cms.untracked.uint32(0)
)
process.load("SimGeneral.HepPDTESSource.pythiapdt_cfi")
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1000))

process.prunedTmp = cms.EDProducer("GenParticlePruner",
    src = cms.InputTag("prunedGenParticles"),
    select = cms.vstring(
        'drop *',
        'keep status == 3',              # me in MadGraph
        'keep 20 <= status <= 30',       # me in Pythia8
        'keep 11 <= abs(pdgId)  <= 16 && numberOfMothers()==1 && abs(mother().pdgId()) >= 23 && abs(mother().pdgId()) <= 25',  # leptons from W,Z,H
        # in some cases, the intermediate W from the top quark decay is not stored; in this case,
        # the leptons can appear directly as daughters from the top. Keep those:
        'keep 11 <= abs(pdgId)  <= 16 && numberOfMothers()==1 && abs(mother().pdgId()) == 6'
        # note that keeping the lepton-daughters from the top quark also keeps cases where t-> t + gamma* with gamma* -> l+l-.
        # We do not want these, and they are removed in the second step (see below). They cannot be reliably removed
        # here, as testing whether the top has a W daughter is only possible after removing the t->t->t->... chains.
    )
)

process.prunedPrunedGenParticles = cms.EDProducer("GenParticlePruner",
    src = cms.InputTag("prunedTmp"),
    select = cms.vstring(
        'keep *',
        # drop leptons from top ...
        'drop 11 <= abs(pdgId) <= 16 && numberOfMothers() == 1 && abs(mother().pdgId())==6',  
        # ... but keep those where the W from the top decay is missing:
        'keep 11 <= abs(pdgId) <= 16 && numberOfMothers() == 1 && abs(mother().pdgId())==6 && mother().numberOfDaughters() > 2 && abs(mother().daughter(0).pdgId()) != 24 && abs(mother().daughter(1).pdgId()) != 24 && abs(mother().daughter(2).pdgId()) != 24',
    )
)

process.genpfilter = cms.EDFilter('FilterStrangeGenParticles',
  src = cms.InputTag("prunedPrunedGenParticles"),
  #n_top_expected = cms.untracked.int32(2)
)
process.genpprinter = cms.EDAnalyzer("ParticleTreeDrawer",
                                   src = cms.InputTag("prunedPrunedGenParticles"),
                                   printP4 = cms.untracked.bool(True),
                                   #printPtEtaPhi = cms.untracked.bool(False),
                                   #printVertex = cms.untracked.bool(False),
                                   printStatus = cms.untracked.bool(True),
                                   printIndex = cms.untracked.bool(True),
                                   #status = cms.untracked.vint32( 3 )
)

process.genpprinter_intermediate = cms.EDAnalyzer("ParticleTreeDrawer",
                                   src = cms.InputTag("prunedTmp"),
                                   printP4 = cms.untracked.bool(True),
                                   #printPtEtaPhi = cms.untracked.bool(False),
                                   #printVertex = cms.untracked.bool(False),
                                   printStatus = cms.untracked.bool(True),
                                   printIndex = cms.untracked.bool(True),
                                   #status = cms.untracked.vint32( 3 )
)

process.genpprinter_orig = cms.EDAnalyzer("ParticleTreeDrawer",
                                   src = cms.InputTag("prunedGenParticles"),
                                   printP4 = cms.untracked.bool(True),
                                   #printPtEtaPhi = cms.untracked.bool(False),
                                   #printVertex = cms.untracked.bool(False),
                                   printStatus = cms.untracked.bool(True),
                                   printIndex = cms.untracked.bool(True),
                                   #status = cms.untracked.vint32( 3 )
)

# note: do filtering on double-pruned collection, but then print both, double-pruned and pruned for debugging.
process.p = cms.Path(process.prunedTmp * process.prunedPrunedGenParticles * process.genpfilter * process.genpprinter * process.genpprinter_intermediate * process.genpprinter_orig)
