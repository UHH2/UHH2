import FWCore.ParameterSet.Config as cms
process = cms.Process("TEST")
process.source = cms.Source("PoolSource",
fileNames  = cms.untracked.vstring("/store/mc/Phys14DR/TprimeTprime_M_1000_Tune4C_13TeV-madgraph/MINIAODSIM/PU40bx25_PHYS14_25_V1-v1/10000/F6F71931-6F67-E411-ABA2-20CF3027A5C9.root"))
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(10))
process.genjettest = cms.EDAnalyzer('NtupleWriterTestGenJets')
process.p = cms.Path(process.genjettest)
