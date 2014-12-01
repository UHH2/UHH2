import FWCore.ParameterSet.Config as cms
process = cms.Process("TEST")
process.source = cms.Source("PoolSource",
  #fileNames  = cms.untracked.vstring("/store/mc/Phys14DR/TprimeTprime_M_1000_Tune4C_13TeV-madgraph/MINIAODSIM/PU40bx25_PHYS14_25_V1-v1/10000/F6F71931-6F67-E411-ABA2-20CF3027A5C9.root")
  #fileNames = cms.untracked.vstring('file:///pnfs/desy.de/cms/tier2/store/mc/Phys14DR/QCD_Pt-15to3000_Tune4C_Flat_13TeV_pythia8/AODSIM/PU20bx25_trkalmb_PHYS14_25_V1-v1/00000/125A6B71-C56A-E411-9D2B-0025907609BE.root'),
  
  fileNames = cms.untracked.vstring('file:///pnfs/desy.de/cms/tier2/store/mc/Phys14DR/QCD_Pt-15to3000_Tune4C_Flat_13TeV_pythia8/MINIAODSIM/PU20bx25_trkalmb_PHYS14_25_V1-v1/00000/1020E374-B26B-E411-8F91-E0CB4E29C513.root'),
 
  )
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1))
process.dump = cms.EDAnalyzer('DumpMiniAOD')
process.p = cms.Path(process.dump)
