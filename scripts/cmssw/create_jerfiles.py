# see https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookJetEnergyCorrections#GetTxtFiles

# note: the global tags PHYS14_50_V1 and PHYS14_25_V1 are the same in terms of JER.
gt = 'PHYS14_50_V1'

import FWCore.ParameterSet.Config as cms
process = cms.Process("jectxt")
process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = gt + '::All'
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(1))
process.source = cms.Source("EmptySource")
process.readAK4PFchs    = cms.EDAnalyzer('JetCorrectorDBReader',
      payloadName    = cms.untracked.string('AK4PFchs'),
      globalTag      = cms.untracked.string(gt),
      printScreen    = cms.untracked.bool(False),
      createTextFile = cms.untracked.bool(True)
)
process.p = cms.Path(process.readAK4PFchs)
