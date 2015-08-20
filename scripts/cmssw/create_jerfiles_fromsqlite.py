# see https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookJetEnergyCorrections#GetTxtFiles

import FWCore.ParameterSet.Config as cms
process = cms.Process("jectxt")

process.load('Configuration.StandardSequences.Services_cff')

process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(1))
process.source = cms.Source("EmptySource")

process.load("CondCore.DBCommon.CondDBCommon_cfi")
from CondCore.DBCommon.CondDBSetup_cfi import *
process.jec = cms.ESSource("PoolDBESSource",
      DBParameters = cms.PSet(
        messageLevel = cms.untracked.int32(0)
        ),
      timetype = cms.string('runnumber'),
      toGet = cms.VPSet(
      cms.PSet(
            record = cms.string('JetCorrectionsRecord'),
            tag    = cms.string('JetCorrectorParametersCollection_Summer15_50nsV4_MC_AK4PFchs'),
            label  = cms.untracked.string('AK4PFchs')
            ),

      ## here you add as many jet types as you need
      ## note that the tag name is specific for the particular sqlite file 
      ), 
      ##download the latest sqlite file from https://twiki.cern.ch/twiki/bin/viewauth/CMS/JECDataMC
      connect = cms.string('sqlite:../../../PhysicsTools/PatAlgos/test/Summer15_50nsV4_MC.db')
)
## add an es_prefer statement to resolve a possible conflict from simultaneous connection to a global tag
process.es_prefer_jec = cms.ESPrefer('PoolDBESSource','jec')

#global tag just used for the name of the outpt files
gt = 'Summer15_50nsV2'

process.readAK4PFchs    = cms.EDAnalyzer('JetCorrectorDBReader',
      payloadName    = cms.untracked.string('AK4PFchs'),
      globalTag      = cms.untracked.string(gt),
      printScreen    = cms.untracked.bool(False),
      createTextFile = cms.untracked.bool(True)
)
process.p = cms.Path(process.readAK4PFchs)
