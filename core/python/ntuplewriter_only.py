import FWCore.ParameterSet.Config as cms

process = cms.Process("NTUPLE")

process.source = cms.Source("PoolSource",
                            fileNames  = cms.untracked.vstring("file://./miniaod.root"),
                            skipEvents = cms.untracked.uint32(0)
)

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1000))

useData = False

process.MyNtuple = cms.EDFilter('NtupleWriter',
                                  AnalysisModule = cms.PSet(
                                      #name = cms.string("NoopAnalysisModule"),
                                      name = cms.string("ExampleModuleEventOutput"),
                                      library = cms.string("libUHH2examples.so"),
                                      # Note: all other settings of type string are passed to the module, e.g.:
                                      int_branchname = cms.string("testintbranch")
                                  ),
                                  fileName = cms.string("ntuplewriter_only_out.root"), 
                                  # rest of the configuration as in ntuplewriter.py on miniAOD ...
                                  runOnMiniAOD = cms.bool(True),
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
                                  doAllGenParticles = cms.bool(False),
                                  doTrigger = cms.bool(True),
                                  doTagInfos = cms.untracked.bool(False),
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
                                  trigger_prefixes = cms.vstring("HLT_",),
                                  
)


process.p = cms.Path(process.MyNtuple)
