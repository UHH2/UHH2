"""
Module to generate our whole ntuple-making chain.

Main function imported & used in other scripts - this file should not be used directly with cmsRun

e.g.:

>> import FWCore.ParameterSet.Config as cms
>> from UHH2.core.ntuple_generator import generate_process
>> process = generate_process(year="2016", useData=False)

"""


import os

import FWCore.ParameterSet.Config as cms
from Configuration.EventContent.EventContent_cff import *
from RecoJets.Configuration.RecoPFJets_cff import *
from RecoJets.JetProducers.fixedGridRhoProducerFastjet_cfi import *
from RecoJets.JetProducers.AnomalousCellParameters_cfi import *
from RecoJets.JetProducers.PFJetParameters_cfi import *
from PhysicsTools.PatAlgos.tools.jetTools import *
from PhysicsTools.PatAlgos.tools.pfTools import *
from RecoBTag.SecondaryVertex.trackSelection_cff import *
from UHH2.core.muon_pfMiniIsolation_cff import *
from UHH2.core.electron_pfMiniIsolation_cff import *
from PhysicsTools.SelectorUtils.tools.vid_id_tools import *

# NOTE: all from xxx import * must go here, not inside the function


def generate_process(year, useData=True, isDebug=False, fatjet_ptmin=150.):
    """Main function to make a cms.Process object to create ntuples.

    Parameters
    ----------
    year : str
        Year to setup ntuplewriter for.
    useData : bool, optional
        True if running over data, False for MC
    isDebug : bool, optional
        True for extra debug printout, don't use in production as slower
    fatjet_ptmin : float, optional
        Minimum pT for large-R reco jets & their corresponding genjets

    Returns
    -------
    cms.Process
        Required process object for cmsRun configs

    Raises
    ------
    ValueError
        If the year argument is not one of the allowable options
    """
    year = str(year)  # sanitise string
    acceptable_years = ["2016v2", "2016v3", "2017", "2018"]
    if year not in acceptable_years:
        raise ValueError("year argument in generate_process() should be one of: %s. You provided: %s" % (acceptable_years, year))

    met_sources_GL = cms.vstring("slimmedMETs", "slimmedMETsPuppi")


    if year == "2016v2" and useData:
        # https://twiki.cern.ch/twiki/bin/view/CMSPublic/ReMiniAOD03Feb2017Notes#MET_Recipes
        met_sources_GL.extend(['slimmedMETsEGClean', 'slimmedMETsMuEGClean', 'slimmedMETsUncorrected'])
    if (year=="2016v2" or year=="2016v3"):
        met_sources_GL.extend(['slMETsCHS'])
    


    bTagDiscriminators = [
        'pfJetProbabilityBJetTags',
        'pfJetBProbabilityBJetTags',
        'pfSimpleSecondaryVertexHighEffBJetTags',
        'pfCombinedInclusiveSecondaryVertexV2BJetTags',
        'pfCombinedMVAV2BJetTags',
        'pfDeepCSVJetTags:probb',
        'pfDeepCSVJetTags:probbb',
        'pfBoostedDoubleSecondaryVertexAK8BJetTags',
        'pfBoostedDoubleSecondaryVertexCA15BJetTags',
    ]
 
    ak4btagDiscriminators = [
        'pfDeepFlavourJetTags:probb',
        'pfDeepFlavourJetTags:probbb',
        'pfDeepFlavourJetTags:problepb',
        'pfDeepFlavourJetTags:probc',
        'pfDeepFlavourJetTags:probuds',
        'pfDeepFlavourJetTags:probg'
    ]

    ak8btagDiscriminators = [
        'pfMassDecorrelatedDeepBoostedDiscriminatorsJetTags:bbvsLight',
        'pfMassDecorrelatedDeepBoostedDiscriminatorsJetTags:ccvsLight',
        'pfMassDecorrelatedDeepBoostedDiscriminatorsJetTags:TvsQCD',
        'pfMassDecorrelatedDeepBoostedDiscriminatorsJetTags:ZHccvsQCD',
        'pfMassDecorrelatedDeepBoostedDiscriminatorsJetTags:WvsQCD',
        'pfMassDecorrelatedDeepBoostedDiscriminatorsJetTags:ZHbbvsQCD',
        'pfMassDecorrelatedDeepBoostedDiscriminatorsJetTags:H4qvsQCD',
        'pfMassDecorrelatedDeepBoostedDiscriminatorsJetTags:HbbvsQCD',
        'pfMassDecorrelatedDeepBoostedDiscriminatorsJetTags:ZbbvsQCD',
        'pfMassDecorrelatedDeepBoostedDiscriminatorsJetTags:ZvsQCD',
        'pfDeepBoostedDiscriminatorsJetTags:TvsQCD',
        'pfDeepBoostedDiscriminatorsJetTags:WvsQCD',
        'pfDeepBoostedDiscriminatorsJetTags:H4qvsQCD',
        'pfDeepBoostedDiscriminatorsJetTags:HbbvsQCD',
        'pfDeepBoostedDiscriminatorsJetTags:ZbbvsQCD',
        'pfDeepBoostedDiscriminatorsJetTags:ZvsQCD',
        'pfMassDecorrelatedDeepBoostedJetTags:probHbb',
        'pfMassDecorrelatedDeepBoostedJetTags:probQCDc',
        'pfMassDecorrelatedDeepBoostedJetTags:probQCDbb',
        'pfMassDecorrelatedDeepBoostedJetTags:probTbqq',
        'pfMassDecorrelatedDeepBoostedJetTags:probTbcq',
        'pfMassDecorrelatedDeepBoostedJetTags:probTbq',
        'pfMassDecorrelatedDeepBoostedJetTags:probQCDothers',
        'pfMassDecorrelatedDeepBoostedJetTags:probQCDb',
        'pfMassDecorrelatedDeepBoostedJetTags:probTbc',
        'pfMassDecorrelatedDeepBoostedJetTags:probWqq',
        'pfMassDecorrelatedDeepBoostedJetTags:probQCDcc',
        'pfMassDecorrelatedDeepBoostedJetTags:probHcc',
        'pfMassDecorrelatedDeepBoostedJetTags:probWcq',
        'pfMassDecorrelatedDeepBoostedJetTags:probZcc',
        'pfMassDecorrelatedDeepBoostedJetTags:probZqq',
        'pfMassDecorrelatedDeepBoostedJetTags:probHqqqq',
        'pfMassDecorrelatedDeepBoostedJetTags:probZbb',
        'pfDeepDoubleBvLJetTags:probHbb',
        'pfDeepDoubleBvLJetTags:probQCD',
        'pfDeepDoubleCvBJetTags:probHbb',
        'pfDeepDoubleCvBJetTags:probHcc',
        'pfDeepDoubleCvLJetTags:probHcc',
        'pfDeepDoubleCvLJetTags:probQCD',
        'pfMassIndependentDeepDoubleBvLJetTags:probHbb',
        'pfMassIndependentDeepDoubleBvLJetTags:probQCD',
        'pfMassIndependentDeepDoubleCvBJetTags:probHbb',
        'pfMassIndependentDeepDoubleCvBJetTags:probHcc',
        'pfMassIndependentDeepDoubleCvLJetTags:probHcc',
        'pfMassIndependentDeepDoubleCvLJetTags:probQCD',
        'pfDeepBoostedJetTags:probHbb',
        'pfDeepBoostedJetTags:probQCDc',
        'pfDeepBoostedJetTags:probQCDbb',
        'pfDeepBoostedJetTags:probTbqq',
        'pfDeepBoostedJetTags:probTbcq',
        'pfDeepBoostedJetTags:probTbq',
        'pfDeepBoostedJetTags:probQCDothers',
        'pfDeepBoostedJetTags:probQCDb',
        'pfDeepBoostedJetTags:probTbc',
        'pfDeepBoostedJetTags:probWqq',
        'pfDeepBoostedJetTags:probQCDcc',
        'pfDeepBoostedJetTags:probHcc',
        'pfDeepBoostedJetTags:probWcq',
        'pfDeepBoostedJetTags:probZcc',
        'pfDeepBoostedJetTags:probZqq',
        'pfDeepBoostedJetTags:probHqqqq',
        'pfDeepBoostedJetTags:probZbb'
    ]


    bTagInfos = [
        'pfImpactParameterTagInfos', 'pfSecondaryVertexTagInfos', 'pfInclusiveSecondaryVertexFinderTagInfos', 'softPFMuonsTagInfos', 'softPFElectronsTagInfos'
    ]

    process = cms.Process("USER")

    task = cms.Task()

    process.load("FWCore.MessageService.MessageLogger_cfi")
    process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(250)
    #process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(1)
    process.options = cms.untracked.PSet(
        wantSummary=cms.untracked.bool(False),
        # wantSummary=cms.untracked.bool(True),
        # numberOfThreads = cms.untracked.uint32(8), # if running crab jobs, you must set this to agree with numCores
        numberOfStreams = cms.untracked.uint32(0) # 0 = use number of threads; to set use -n
    )

    # DEBUG ----------------
    if isDebug:
        process.Timing = cms.Service("Timing",
                                     summaryOnly=cms.untracked.bool(False),
                                     useJobReport=cms.untracked.bool(True)
                                     )

        process.SimpleMemoryCheck = cms.Service("SimpleMemoryCheck",
                                                ignoreTotal=cms.untracked.int32(2),
                                                moduleMemorySummary=cms.untracked.bool(True)
                                                )

    # DEBUG ----------------

    process.source = cms.Source("PoolSource",
                                fileNames=cms.untracked.vstring([
                                    '/store/data/Run2017D/JetHT/MINIAOD/17Nov2017-v1/20000/0249B143-8CCC-E711-BA7C-0025905C2CD0.root'
                                ]),
                                skipEvents=cms.untracked.uint32(0)
                                )

    process.maxEvents = cms.untracked.PSet(input=cms.untracked.int32(100))

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
    process.out = cms.OutputModule("PoolOutputModule",
                                   fileName=cms.untracked.string('miniaod.root'),
                                   outputCommands=MINIAODSIMEventContent.outputCommands
    )

    process.out.outputCommands.extend([
        'keep *_patJetsAk8CHS*_*_*',
        'keep *_patJetsAk8Puppi*_*_*',
        'keep *_patJetsCa15CHS*_*_*',
        'keep *_NjettinessAk8CHS_*_*',
        'keep *_NjettinessAk8Puppi_*_*',
        'keep *_NjettinessCa15CHS_*_*',
        'keep *_NjettinessCa15SoftDropCHS_*_*',
        #"keep *_patJetsCmsTopTagCHSPacked_*_*",
        #"keep *_patJetsCmsTopTagCHSSubjets_*_*",
        #"keep *_patJetsHepTopTagCHSPacked_*_*",
        #"keep *_patJetsHepTopTagCHSSubjets_*_*",
        "keep *_patJetsAk8CHSJetsSoftDropPacked_*_*",
        "keep *_patJetsAk8CHSJetsSoftDropSubjets_*_*",
        "keep *_patJetsAk8PuppiJetsSoftDropPacked_*_*",
        "keep *_patJetsAk8PuppiJetsSoftDropSubjets_*_*",
        "keep *_patJetsCa15CHSJetsSoftDropPacked_*_*",
        "keep *_patJetsCa15CHSJetsSoftDropSubjets_*_*",
        "keep *_patJetsAk8CHSJetsPrunedPacked_*_*",
        "keep *_patJetsAk8CHSJetsPrunedSubjets_*_*",
        "keep *_prunedPrunedGenParticles_*_*",
        "keep *_egmGsfElectronIDs_*_*"
    ])

    ###############################################
    # RECO AND GEN SETUP
    process.load("Configuration.Geometry.GeometryRecoDB_cff")
    process.load("Configuration.StandardSequences.MagneticField_cff")

    # see https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideFrontierConditions
    # for latest global tags
    # But check with https://twiki.cern.ch/twiki/bin/viewauth/CMS/PdmVAnalysisSummaryTable
    process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')

    # There should be 1 key:value entry per entry in acceptable_years, and each
    # should have a dictionary of "data" and "mc" with their respsective global tags
    global_tags = {
        "2016v2": {
            "data": "80X_dataRun2_2016SeptRepro_v7",
            "mc": "80X_mcRun2_asymptotic_2016_TrancheIV_v8",
        },
        "2016v3": {
            # TODO: does this need runH specific GTs?
            "data": "94X_dataRun2_v10",
            "mc": "94X_mcRun2_asymptotic_v3",
        },
        "2017": {
            "data": "94X_dataRun2_v6",
            "mc": "94X_mc2017_realistic_v14"
        },
        "2018": {
            "data": "102X_dataRun2_Prompt_v6",
            "mc": "102X_upgrade2018_realistic_v15",
        },
    }
    if set(global_tags.keys()) != set(acceptable_years):
        raise KeyError("Mismatch between acceptable_years and global_tags")

    process.GlobalTag.globaltag = global_tags[year]['data' if useData else 'mc']

    process.fixedGridRhoFastjetAll = fixedGridRhoFastjetAll.clone(
        pfCandidatesTag='packedPFCandidates'
    )


    ###############################################
    # GEN PARTICLES
    #
    # The 13TeV samples mainly use Pythia8 for showering, which stores information in another way compared to Pythia6; in particular,
    # many intermediate particles are stored such as top quarks or W bosons, which are not required for the analyses and makle the code more complicated.
    # Therefore, the 'prunedGenParticles' collection is pruned again; see UHH2/core/python/testgenparticles.py for a test for this pruning
    # and more comments.

    process.prunedTmp = cms.EDProducer("GenParticlePruner",
                                       src=cms.InputTag("prunedGenParticles"),
                                       select=cms.vstring(
                                           'drop *',
                                           'keep status == 3',
                                           'keep 20 <= status <= 30',
                                           'keep 11 <= abs(pdgId) <= 16 && numberOfMothers()==1 && abs(mother().pdgId()) >= 23 && abs(mother().pdgId()) <= 25',
                                           'keep 11 <= abs(pdgId) <= 16 && numberOfMothers()==1 && abs(mother().pdgId()) == 6',
                                           'keep 11 <= abs(pdgId) <= 16 && numberOfMothers()==1 && abs(mother().pdgId()) == 42'
                                       )
    )
    task.add(process.prunedTmp)

    process.prunedPrunedGenParticles = cms.EDProducer("GenParticlePruner",
                                                      src=cms.InputTag("prunedTmp"),
                                                      select=cms.vstring(
                                                          'keep *',
                                                          'drop 11 <= abs(pdgId) <= 16 && numberOfMothers() == 1 && abs(mother().pdgId())==6',
                                                          'keep 11 <= abs(pdgId) <= 16 && numberOfMothers() == 1 && abs(mother().pdgId())==6 ' \
                                                          '&& mother().numberOfDaughters() > 2 && abs(mother().daughter(0).pdgId()) != 24 ' \
                                                          '&& abs(mother().daughter(1).pdgId()) != 24 && abs(mother().daughter(2).pdgId()) != 24',
                                                      )
    )
    task.add(process.prunedPrunedGenParticles)


    ###############################################
    # Modified TypeI MET
    #
    # Only applicable during 2017, this corrects the MET due to excess EE noise
    # https://twiki.cern.ch/twiki/bin/viewauth/CMS/MissingETUncertaintyPrescription?rev=89#Instructions_for_9_4_X_X_9_or_10
    # To be used with 17Nov2017 and 31Mar2018 rereco of 2017 data, and MC events.
    # The more accurate and long-term solution will come incorporated in planed "Ultra-Legacy" recreco
    if year in ['2017']:
        from PhysicsTools.PatUtils.tools.runMETCorrectionsAndUncertainties import runMetCorAndUncFromMiniAOD

        runMetCorAndUncFromMiniAOD(
            process,
            isData=useData,
            fixEE2017=True,
            fixEE2017Params={'userawPt': True, 'ptThreshold': 50.0, 'minEtaThreshold': 2.65, 'maxEtaThreshold': 3.139},
            postfix="ModifiedMET"
        )

        met_sources_GL.append("slimmedMETsModifiedMET")

    ###############################################
    # CHS JETS
    #
    # configure additional jet collections, based on CHS.
    # also add in softdrop for CHS jets
    process.chs = cms.EDFilter("CandPtrSelector",
        src=cms.InputTag("packedPFCandidates"),
        cut=cms.string("fromPV(0) > 0")
    )
    task.add(process.chs)

    # This is primarily used for JEC studies.
    # It has a low pT threshold, and will not get any substructure info.
    process.ak8CHSJets = ak8PFJets.clone(
        src='chs',
        doAreaFastjet=True,
        jetPtMin=10.
    )
    task.add(process.ak8CHSJets)

    # This is used for TopJets with substructure info.
    # It has a higher pT threshold, and there will be corresponding groomed versions
    # While I'd love to avoid reclustering by using a PtMinCandSelector, it
    # doesn't work well when tring to inherit the same clustering settings in
    # add_fatjets_subjets()
    process.ak8CHSJetsFat = ak8PFJets.clone(
        src='chs',
        doAreaFastjet=True,
        jetPtMin=fatjet_ptmin
    )
    task.add(process.ak8CHSJetsFat)

    # The ak8CHSJetsSoftDrop collection produce the groomed jets as reco::BasicJets,
    # and the subjets as reco::PFJets, with instance label "SubJets"
    # It is used for its subjets
    process.ak8CHSJetsSoftDrop = ak8PFJetsCHSSoftDrop.clone(
        src=cms.InputTag('chs'),
        jetPtMin=fatjet_ptmin
    )
    task.add(process.ak8CHSJetsSoftDrop)

    # The ak8CHSJetsSoftDropforsub produces only the groomed jets as PFJets
    # It is used for calculating quantities on the groomed fat jet that require
    # reco::PFJets rather than reco::BasicJets as produced by ak8CHSJetsSoftDrop
    process.ak8CHSJetsSoftDropforsub = process.ak8CHSJetsSoftDrop.clone()
    delattr(process.ak8CHSJetsSoftDropforsub, "writeCompound")
    delattr(process.ak8CHSJetsSoftDropforsub, "jetCollInstanceName")
    task.add(process.ak8CHSJetsSoftDropforsub)

    ###############################################
    # Add HepTopTagger bits
    #
    # The HTTTopJetProducer does its own clustering producing a BasicJet of
    # Top candidates (i.e. sum of subjects),
    # as well as producing PFJet (SubJets), and HTTTopJetTagInfo
    # The CA15 jets here are the "original" fatjets going into the HTT, and thus will
    # be different from the BasicJet collection that HTTTopJetProducer produces
    # ca15_clustering_params = dict(
    #     useExplicitGhosts=cms.bool(True),
    #     jetAlgorithm=cms.string("CambridgeAachen"),
    #     rParam=cms.double(1.5),
    #     **AnomalousCellParameters.parameters_()
    # )
    # ca15_clustering_params.update(
    #     **(PFJetParameters.clone(
    #         src=cms.InputTag("chs"),
    #         doAreaFastjet=cms.bool(True),
    #         doRhoFastjet=cms.bool(False),
    #         jetPtMin=cms.double(200.0)
    #     ).parameters_())
    # )

    # process.ca15CHSJets = cms.EDProducer("FastjetJetProducer",
    #                                      **ca15_clustering_params
    #                                      )
    # task.add(process.ca15CHSJets)

    # process.hepTopTagCHS = cms.EDProducer("HTTTopJetProducer",
    #                                       optimalR=cms.bool(True),
    #                                       qJets=cms.bool(False),
    #                                       minFatjetPt=cms.double(200.),
    #                                       minSubjetPt=cms.double(0.),
    #                                       minCandPt=cms.double(0.),
    #                                       maxFatjetAbsEta=cms.double(99.),
    #                                       subjetMass=cms.double(30.),
    #                                       muCut=cms.double(0.8),
    #                                       filtR=cms.double(0.3),
    #                                       filtN=cms.int32(5),
    #                                       mode=cms.int32(4),
    #                                       minCandMass=cms.double(0.),
    #                                       maxCandMass=cms.double(999999.),
    #                                       massRatioWidth=cms.double(999999.),
    #                                       minM23Cut=cms.double(0.),
    #                                       minM13Cut=cms.double(0.),
    #                                       maxM13Cut=cms.double(999999.),
    #                                       jetCollInstanceName=cms.string(
    #                                           "SubJets"),
    #                                       algorithm=cms.int32(1),
    #                                       writeCompound=cms.bool(True),
    #                                       **ca15_clustering_params
    #                                       )
    # task.add(process.hepTopTagCHS)

    # process.ca15CHSJetsSoftDrop = ak8PFJetsCHSSoftDrop.clone(
    #     src=cms.InputTag('chs'),
    #     jetPtMin=process.ca15CHSJets.jetPtMin,
    #     jetAlgorithm=cms.string("CambridgeAachen"),
    #     rParam=1.5,
    #     R0=1.5,
    #     zcut=cms.double(0.2),
    #     beta=cms.double(1.0)
    # )
    # task.add(process.ca15CHSJetsSoftDrop)

    # process.ca15CHSJetsSoftDropforsub = process.ca15CHSJets.clone(
    #     zcut=cms.double(0.1),
    #     beta=cms.double(0.0),
    #     useSoftDrop=cms.bool(True),
    #     useExplicitGhosts=cms.bool(True),
    #     R0=cms.double(1.5)
    # )
    # task.add(process.ca15CHSJetsSoftDropforsub)

    ###############################################
    # PUPPI JETS
    #
    process.load('CommonTools/PileupAlgos/Puppi_cff')
    # recreate PUPPI candidates for MiniAOD
    # unpack stored weights to save time recalculating it
    process.puppi.candName = cms.InputTag('packedPFCandidates')
    process.puppi.vertexName = cms.InputTag('offlineSlimmedPrimaryVertices')
    process.puppi.clonePackedCands = cms.bool(True)
    process.puppi.useExistingWeights = cms.bool(True)
    task.add(process.puppi)

    # This is primarily used for JEC studies.
    # It has a low pT threshold, and will not get any substructure info.
    process.ak8PuppiJets = process.ak8CHSJets.clone(
        src=cms.InputTag('puppi')
    )
    task.add(process.ak8PuppiJets)

    # This is used for TopJets with substructure info.
    # It has a higher pT threshold, and there will be corresponding groomed versions
    process.ak8PuppiJetsFat = process.ak8CHSJetsFat.clone(
        src=cms.InputTag('puppi')
    )
    task.add(process.ak8PuppiJetsFat)

    # Like for CHS, this makes groomed fatjets as reco::BasicJets,
    # and the subjets as reco::PFJets
    process.ak8PuppiJetsSoftDrop = process.ak8CHSJetsSoftDrop.clone(
        src=cms.InputTag('puppi')
    )
    task.add(process.ak8PuppiJetsSoftDrop)

    # Like for CHS, this makes only the groomed fatjets as reco::PFJets
    process.ak8PuppiJetsSoftDropforsub = process.ak8CHSJetsSoftDropforsub.clone(
        src=cms.InputTag('puppi')
    )
    task.add(process.ak8PuppiJetsSoftDropforsub)

    # process.ca15PuppiJets = process.ca8CHSJets.clone(rParam=1.5, src='puppi')
    # task.add(process.ca15PuppiJets)

    # process.ca15PuppiJetsSoftDrop = ak8PFJetsCHSSoftDrop.clone(
    #     src=cms.InputTag('puppi'),
    #     jetPtMin=fatjet_ptmin,
    #     jetAlgorithm=cms.string("CambridgeAachen"),
    #     rParam=1.5,
    #     R0=1.5,
    #     zcut=cms.double(0.2),
    #     beta=cms.double(1.0)
    # )
    # task.add(process.ca15PuppiJetsSoftDrop)

    # process.ca15PuppiJetsSoftDropforsub = process.ca8CHSJets.clone(
    #     rParam=1.5,
    #     jetPtMin=fatjet_ptmin,
    #     zcut=cms.double(0.2),
    #     beta=cms.double(1.0),
    #     useSoftDrop=cms.bool(True),
    #     useExplicitGhosts=cms.bool(True),
    #     R0=cms.double(1.5),
    #     src=cms.InputTag('puppi')
    # )
    # task.add(process.ca15PuppiJetsSoftDropforsub)

    ###############################################
    # PAT JETS and Gen Jets
    #
    # 'Patify' the jet collections defined above and also add the corresponding
    # genjets.

    # captitalize string; needed below to construct pat module names.
    def cap(s): return s[0].upper() + s[1:]

    # process.load('PhysicsTools.PatAlgos.slimming.unpackedTracksAndVertices_cfi')

    # common parameters for the addJetCollection function, see below.
    common_btag_parameters = dict(
        #trackSource = cms.InputTag('unpackedTracksAndVertices'),
        pfCandidates=cms.InputTag('packedPFCandidates'),
        pvSource=cms.InputTag('offlineSlimmedPrimaryVertices'),
        svSource=cms.InputTag('slimmedSecondaryVertices'),
        muSource=cms.InputTag('slimmedMuons'),
        elSource=cms.InputTag('slimmedElectrons'),
        btagInfos=bTagInfos,
        btagDiscriminators=bTagDiscriminators
    )

    process.packedGenParticlesForJetsNoNu = cms.EDFilter("CandPtrSelector",
        src=cms.InputTag("packedGenParticles"),
        cut=cms.string("abs(pdgId) != 12 && abs(pdgId) != 14 && abs(pdgId) != 16")
    )
    task.add(process.packedGenParticlesForJetsNoNu)


    def del_attr_safely(obj, name):
        if hasattr(obj, name):
            delattr(obj, name)


    def modify_patjetproducer_for_data(process, producer):
        """Modify a PATJetProducer for use with data i.e. turn off all gen-related parts

        Also delete all the extra producers it adds in

        See PhysicsTools/PatAlgos/python/producersLayer1/jetProducer_cfi.py
        for all available options
        """
        producer.addGenPartonMatch = cms.bool(False)
        producer.embedGenPartonMatch = cms.bool(False)
        del_attr_safely(process, producer.genPartonMatch.value().split(":")[0])
        producer.genPartonMatch = cms.InputTag("")

        producer.addGenJetMatch = cms.bool(False)
        producer.embedGenJetMatch = cms.bool(False)
        del_attr_safely(process, producer.genJetMatch.value().split(":")[0])
        producer.genJetMatch = cms.InputTag("")

        del_attr_safely(process, producer.JetPartonMapSource.value().split(":")[0])
        producer.JetPartonMapSource = cms.InputTag("")

        if (producer.addJetFlavourInfo):
            del_attr_safely(process, producer.JetFlavourInfoSource.value().split(":")[0])
        producer.JetFlavourInfoSource = cms.InputTag("")

        producer.addPartonJetMatch = cms.bool(False)
        producer.partonJetSource = cms.InputTag("NOT_IMPLEMENTED")

        producer.getJetMCFlavour = cms.bool(False)
        producer.addJetFlavourInfo  = cms.bool(False)


    # Add PAT part of fat jets and subjets, and optionally gen jets. Note that the input collections for the groomed PF jets have to be defined elsewhere
    # already.
    # This method assumes that you follow  certain naming convention. In particular:
    # * the jet algorithm ('ca' or 'ak') has to be part of the fatjets_name
    # * fatjets_name is an ungroomed jet collection configured in the same job of type FastjetProducer
    # * the groomed jets' label is given by groomed_jets_name
    # * the subjets are available with the same name as the groomed jets with instance label 'SubJets'
    #
    # The method will produce several collections, also following a naming convention:
    # * a patJets collection for the ungroomed fatjets with name 'patJets' + fatjets_name (unless it already exists)
    # * two pat jet collections (one for fat jets and one for subjets) with names
    #   - 'patJets' + groomed_jets_name for the fat jets and
    #   - 'patJets' + groomed_jets_name + 'Subjets' for the subjets
    # * a merged jet collection 'patJets' + groomed_jets_name + 'Packed'
    # * in case genjets_name is not None, is must be a function returning the genjet collection name, given the original
    #   (reco) jet collection name. In this case, the according genjet producer is added for the groomed and for
    #   the ungroomed collection, using that name.
    #
    # Note that gen jets are produced but genjet *matching* is currently only working for the fat, ungroomed jets,
    # and the subjets, but not for the groomed fat jets; this is a restriction
    # of PAT.
    #
    # One can also enable the storing of top tagging info. In this case, it will use info from
    # groomed_jets_name, and top_tagging must be set True. Additionally,
    # btagging must be set True

    def add_fatjets_subjets(process, fatjets_name, groomed_jets_name, jetcorr_label='AK8PFchs', jetcorr_label_subjets='AK4PFchs', genjets_name=None, verbose=True, btagging=True, top_tagging=False):
        rParam = getattr(process, fatjets_name).rParam.value()
        algo_dict = {"CambridgeAachen": "ca", "AntiKt": "ak"}
        algo = algo_dict.get(
            getattr(process, fatjets_name).jetAlgorithm.value(), None)
        if algo is None:
            raise RuntimeError, "cannot guess jet algo (ca/ak) from jet producer %s", fatjets_name

        if verbose:
            print '***  Adding fatjets_subjets for', fatjets_name

        subjets_name = groomed_jets_name + 'Subjets'  # e.g. CA8CHSPruned + Subjets

        # add genjet producers, if requested:
        groomed_genjets_name = 'INVALID'
        ungroomed_genjets_name = 'INVALID'

        if genjets_name is not None:
            groomed_jetproducer = getattr(process, groomed_jets_name)
            assert groomed_jetproducer.type_() in ('FastjetJetProducer',
                                                   'CATopJetProducer'), "do not know how to construct genjet collection for %s" % repr(groomed_jetproducer)
            groomed_genjets_name = genjets_name(groomed_jets_name)
            if verbose:
                print "  Adding groomed genjets:", groomed_genjets_name
            if not hasattr(process, groomed_genjets_name):
                setattr(process,
                        groomed_genjets_name,
                        groomed_jetproducer.clone(
                            src=cms.InputTag('packedGenParticlesForJetsNoNu'),
                            jetType='GenJet'
                            )
                        )
                task.add(getattr(process, groomed_genjets_name))
            # add for ungroomed jets if not done yet (maybe never used in case
            # ungroomed are not added, but that's ok ..)
            ungroomed_jetproducer = getattr(process, fatjets_name)
            assert ungroomed_jetproducer.type_() == 'FastjetJetProducer', "ungroomed_jetproducer is not a FastjetJetProducer"
            ungroomed_genjets_name = genjets_name(fatjets_name)
            if verbose:
                print "  Adding ungroomed genjets:", ungroomed_genjets_name
            if not hasattr(process, ungroomed_genjets_name):
                setattr(process,
                        ungroomed_genjets_name,
                        ungroomed_jetproducer.clone(
                            src=cms.InputTag('packedGenParticlesForJetsNoNu'),
                            jetType='GenJet'
                            )
                        )
                task.add(getattr(process, ungroomed_genjets_name))

        jetcorr_list = ['L1FastJet', 'L2Relative', 'L3Absolute']
        if useData:
            jetcorr_list.append('L2L3Residual')
        if "puppi" in fatjets_name.lower():
            jetcorr_list = jetcorr_list[1:]

        if jetcorr_label:
            jetcorr_arg = (jetcorr_label, cms.vstring(jetcorr_list), 'None')
        else:
            jetcorr_arg = None

        # patify ungroomed jets, if not already done:
        ungroomed_patname = 'patJets' + cap(fatjets_name)
        add_ungroomed = not hasattr(process, ungroomed_patname)
        if add_ungroomed:
            if verbose:
                print "  Adding ungroomed jets:", ungroomed_patname
            addJetCollection(process,
                             labelName=fatjets_name,
                             jetSource=cms.InputTag(fatjets_name),
                             algo=algo,
                             rParam=rParam,
                             jetCorrections=jetcorr_arg,
                             genJetCollection=cms.InputTag(ungroomed_genjets_name),
                             getJetMCFlavour=not useData,
                             **common_btag_parameters
                             )
            getattr(process, ungroomed_patname).addTagInfos = True
            delattr(process, "selectedPatJets"+cap(fatjets_name))

        # patify groomed fat jets, with b-tagging:
        groomed_patname = "patJets" + cap(groomed_jets_name)
        if verbose:
            print "  Adding groomed jets:", groomed_patname
        addJetCollection(process,
                         labelName=groomed_jets_name,
                         jetSource=cms.InputTag(groomed_jets_name),
                         algo=algo,
                         rParam=rParam,
                         jetCorrections=jetcorr_arg,
                         # genJetCollection = cms.InputTag(groomed_genjets_name), #
                         # nice try, but PAT looks for GenJets, whereas jets with
                         # subjets are BasicJets, so PAT cannot be used for this
                         # matching ...
                         genJetCollection=cms.InputTag("slimmedGenJets"),
                         getJetMCFlavour=not useData,
                         **common_btag_parameters
                         )
        getattr(process, groomed_patname).addTagInfos = True
        if top_tagging:
            getattr(process, groomed_patname).tagInfoSources = cms.VInputTag(groomed_jets_name)
        delattr(process, "selectedPatJets"+cap(groomed_jets_name))

        # patify subjets, with subjet b-tagging:
        subjets_patname = "patJets" + cap(subjets_name)
        if verbose:
            print "  Adding groomed jets' subjets:", subjets_patname
        if jetcorr_label_subjets:
            jetcorr_arg = (jetcorr_label_subjets,
                           cms.vstring(jetcorr_list), 'None')
        else:
            jetcorr_arg = None
        addJetCollection(process,
                         labelName=subjets_name,
                         jetSource=cms.InputTag(groomed_jets_name, 'SubJets'),
                         algo=algo,
                         rParam=rParam,
                         jetCorrections=jetcorr_arg,
                         explicitJTA=True,
                         svClustering=True,
                         fatJets=cms.InputTag(fatjets_name),
                         groomedFatJets=cms.InputTag(groomed_jets_name),
                         genJetCollection=cms.InputTag(
                             groomed_genjets_name, 'SubJets'),
                         getJetMCFlavour=not useData,
                         **common_btag_parameters
#                         **common_btag_parameters_subjet
                         )
        # Always add taginfos to subjets, but possible not to store them,
        # configurable with ntuple writer parameter: subjet_taginfos
        # Attention: Only CVS b-tag info is stored for sub-jets
        getattr(process, subjets_patname).addTagInfos = True
        delattr(process, "selectedPatJets"+cap(subjets_name))


        # Add DeepFlavor b-tagging to sub-jets
        labelName = cap(subjets_patname)
        is_puppi = "puppi" in labelName.lower()

        # # This call to updateJetCollection adds one PATJetUpdater to only remove the JECs,
        # # then uses that as the input to another PATJetUpdater, which re-applies the JECs,
        # # adds in all b tag stuff, etc.
        # # The 2nd PATJetsUpdater has the extra "TransientCorrected" bit in its name.
        # # It also produces a final similar "selectedUpdatedPatJets"+labelName+postfix collection
        # # which is a PATJetSelector
        postfix = ''
        updater_src = "updatedPatJets" + labelName + postfix  # 1st PATJetUpdater, that removes JECs, is src to updater_name
        updater_name = "updatedPatJetsTransientCorrected" + labelName + postfix  # 2nd PATJetUpdater
        selector_name = "selectedUpdatedPatJets" + labelName + postfix
        if is_puppi:
            correction_tag = "AK4PFPuppi"
        else:
            correction_tag = "AK4PFchs"

        jetcorr_list = ['L1FastJet', 'L2Relative', 'L3Absolute']
        if is_puppi:
            jetcorr_list = jetcorr_list[1:]
        if useData:
            jetcorr_list.append("L2L3Residual")
        discriminators = ak4btagDiscriminators[:]

        updateJetCollection(
            process,
            labelName=labelName,
            jetSource=cms.InputTag(subjets_patname),
            pvSource=cms.InputTag('offlineSlimmedPrimaryVertices'),
            svSource=cms.InputTag('slimmedSecondaryVertices'),
            jetCorrections=jetcorr_arg,  
            btagDiscriminators=discriminators,
            postfix=postfix
        )

        subjets_patname = "updatedPatJetsTransientCorrected" + cap(subjets_patname)

        # print "btagDiscriminators ", discriminators

        # add the merged jet collection which contains the links from groomed
        # fat jets to the subjets:
        groomed_packed_name = groomed_patname + 'Packed'
        if verbose:
            print "  Adding groomed jets + subjets packer:", groomed_packed_name
        setattr(process,
                groomed_packed_name,
                cms.EDProducer("BoostedJetMerger",
                                jetSrc=cms.InputTag(groomed_patname),
                                subjetSrc=cms.InputTag(subjets_patname)
                              )
                )
        task.add(getattr(process, groomed_packed_name))

        # adapt all for b-tagging, and switch off some PAT features not supported
        # in miniAOD:
        module_names = [subjets_name, groomed_jets_name]
        if add_ungroomed:
            module_names += [fatjets_name]
        for name in module_names:
            getattr(process, 'patJetPartonMatch' + cap(name)).matched = 'prunedGenParticles'
            producer = getattr(process, 'patJets' + cap(name))
            producer.addJetCharge = False
            producer.addAssociatedTracks = False
            if not btagging:
                producer.addDiscriminators = False
                producer.addBTagInfo = False
                producer.getJetMCFlavour = False
            producer.addGenJetMatch = genjets_name is not None
            # for fat groomed jets, gen jet match and jet flavor is not working, so
            # switch it off:
            if name == groomed_jets_name:
                producer.addGenJetMatch = False
                producer.getJetMCFlavour = False
            # For data, turn off every gen-related part - can't do this via addJetCollection annoyingly
            if useData:
                modify_patjetproducer_for_data(process, producer)


    # add_fatjets_subjets(process, 'ca15CHSJets', 'hepTopTagCHS', top_tagging=True,
    #                     jetcorr_label=None, jetcorr_label_subjets=None)  # CA JEC don't exist so use nothing
    # add_fatjets_subjets(process, 'ca15CHSJets', 'ca15CHSJetsPruned',
    #                     jetcorr_label=None, jetcorr_label_subjets=None)  # we only use this to make packed collection for pruned mass

    add_fatjets_subjets(process, 'ak8CHSJetsFat', 'ak8CHSJetsSoftDrop',
                        genjets_name=lambda s: s.replace('CHS', 'Gen'))

    add_fatjets_subjets(process, 'ak8PuppiJetsFat', 'ak8PuppiJetsSoftDrop',
                        genjets_name=lambda s: s.replace('Puppi', 'Gen'),
                        jetcorr_label="AK8PFPuppi", jetcorr_label_subjets="AK4PFPuppi")


    # Adapt primary vertex collection
    adaptPVs(process, pvCollection=cms.InputTag('offlineSlimmedPrimaryVertices'))

    ###############################################
    # Add subjet variables
    #
    # For each type of jet, we normally add both ungroomed and groomed quantities

    # NJettiness
    # ----------
    from RecoJets.JetProducers.nJettinessAdder_cfi import Njettiness
    # AK8 CHS
    process.NjettinessAk8CHS = Njettiness.clone(
        src=cms.InputTag("ak8CHSJetsFat"),
        Njets=cms.vuint32(1, 2, 3, 4),  # compute 1-, 2-, 3-, 4- subjettiness
        # variables for measure definition :
        measureDefinition=cms.uint32(0),  # CMS default is normalized measure
        beta=cms.double(1.0),  # CMS default is 1
        R0=cms.double(0.8),  # CMS default is jet cone size
    )
    task.add(process.NjettinessAk8CHS)

    process.NjettinessAk8SoftDropCHS = process.NjettinessAk8CHS.clone(
        src=cms.InputTag("ak8CHSJetsSoftDropforsub")
    )
    task.add(process.NjettinessAk8SoftDropCHS)

    # CA15 CHS
    # process.NjettinessCa15CHS = Njettiness.clone(
    #     src=cms.InputTag("ca15CHSJets"),
    #     cone=cms.double(1.5),
    #     R0=cms.double(1.5)
    # )
    # task.add(process.NjettinessCa15CHS)

    # process.NjettinessCa15SoftDropCHS = Njettiness.clone(
    #     src=cms.InputTag("ca15CHSJetsSoftDropforsub"),
    #     Njets=cms.vuint32(1, 2, 3, 4),          # compute 1-, 2-, 3-, 4- subjettiness
    #     # variables for measure definition :
    #     measureDefinition=cms.uint32(0),  # CMS default is normalized measure
    #     beta=cms.double(1.0),              # CMS default is 1
    #     R0=cms.double(1.5),                  # CMS default is jet cone size
    #     Rcutoff=cms.double(999.0),       # not used by default
    #     # variables for axes
    #     # definition :
    #     axesDefinition=cms.uint32(6),    # CMS default is 1-pass KT axes
    #     # not used by default
    #     nPass=cms.int32(999),
    #     # not used by default
    #     akAxesR0=cms.double(999.0)
    # )
    # task.add(process.NjettinessCa15SoftDropCHS)

    # CA15 PUPPI
    # process.NjettinessCa15Puppi = Njettiness.clone(
    #     src=cms.InputTag("ca15PuppiJets"),
    #     cone=cms.double(1.5),
    #     R0=cms.double(1.5)
    # )
    # task.add(process.NjettinessCa15Puppi)

    # process.NjettinessCa15SoftDropPuppi = process.NjettinessCa15SoftDropCHS.clone(
    #     src=cms.InputTag("ca15PuppiJetsSoftDropforsub")
    # )
    # task.add(process.NjettinessCa15SoftDropPuppi)

    # AK8 PUPPI
    process.NjettinessAk8Puppi = process.NjettinessAk8CHS.clone(
        src=cms.InputTag("ak8PuppiJetsFat")
    )
    task.add(process.NjettinessAk8Puppi)

    process.NjettinessAk8SoftDropPuppi = process.NjettinessAk8SoftDropCHS.clone(
        src=cms.InputTag("ak8PuppiJetsSoftDropforsub")
    )
    task.add(process.NjettinessAk8SoftDropPuppi)

    # AK8 GenJets
    # process.NjettinessAk8Gen = Njettiness.clone(
    #     src=cms.InputTag("ak8GenJets"),
    #     cone=cms.double(0.8)
    # )
    # task.add(process.NjettinessAk8Gen)

    # process.NjettinessAk8SoftDropGen = Njettiness.clone(
    #     src=cms.InputTag("ak8GenJetsSoftDrop"),
    #     cone=cms.double(0.8)
    # )
    # task.add(process.NjettinessAk8SoftDropGen)

    # QJetsAdder
    # ----------
    # from RecoJets.JetProducers.qjetsadder_cfi import QJetsAdder
    # process.QJetsCa8CHS = QJetsAdder.clone(src = cms.InputTag("patJetsCa8CHSJets"), jetRad = cms.double(0.8))
    # process.QJetsCa15CHS = QJetsAdder.clone(src = cms.InputTag("patJetsCa15CHSJets"), jetRad = cms.double(1.5))

    # process.RandomNumberGeneratorService = cms.Service("RandomNumberGeneratorService",
    #    QJetsCa8CHS = cms.PSet(
    #       initialSeed = cms.untracked.uint32(123)
    #    ),
    #    QJetsCa15CHS = cms.PSet(
    #       initialSeed = cms.untracked.uint32(123)
    #    )
    # )

    # Energy Correlation Functions (ECFs)
    # -----------------------------------
    # Add in Energy Correlation Functions for groomed jets only
    # The cut is taken from PhysicsTools/PatAlgos/python/slimming/applySubstructure_cff.py
    from RecoJets.JetProducers.ECF_cff import ecfNbeta1, ecfNbeta2
    ecf_pt_min = 250
    process.ECFNbeta1Ak8SoftDropCHS = ecfNbeta1.clone(
        src=cms.InputTag("ak8CHSJetsSoftDropforsub"),
        cuts=cms.vstring('', '', 'pt > %f' % (ecf_pt_min))
    )
    task.add(process.ECFNbeta1Ak8SoftDropCHS)

    process.ECFNbeta2Ak8SoftDropCHS = ecfNbeta2.clone(
        src=cms.InputTag("ak8CHSJetsSoftDropforsub"),
        cuts=cms.vstring('', '', 'pt > %f' % (ecf_pt_min))
    )
    task.add(process.ECFNbeta2Ak8SoftDropCHS)


    process.ECFNbeta1Ak8SoftDropPuppi = ecfNbeta1.clone(
        src=cms.InputTag("ak8PuppiJetsSoftDropforsub"),
        cuts=cms.vstring('', '', 'pt > %f' % (ecf_pt_min))
    )
    task.add(process.ECFNbeta1Ak8SoftDropPuppi)

    process.ECFNbeta2Ak8SoftDropPuppi = ecfNbeta2.clone(
        src=cms.InputTag("ak8PuppiJetsSoftDropforsub"),
        cuts=cms.vstring('', '', 'pt > %f' % (ecf_pt_min))
    )
    task.add(process.ECFNbeta2Ak8SoftDropPuppi)

    # Warning, can be very slow
    # process.ECFNbeta1CA15SoftDropCHS = ecfNbeta1.clone(
    #     src=cms.InputTag("ca15CHSJetsSoftDropforsub"),
    #     cuts=cms.vstring('', '', 'pt > 250')
    # )
    # task.add(process.ECFNbeta1CA15SoftDropCHS)

    # process.ECFNbeta2CA15SoftDropCHS = ecfNbeta2.clone(
    #     src=cms.InputTag("ca15CHSJetsSoftDropforsub"),
    #     cuts=cms.vstring('', '', 'pt > 250')
    # )
    # task.add(process.ECFNbeta2CA15SoftDropCHS)

    ###############################################
    # JEC-specific jet collections
    #
    # for JEC purposes, cluster AK8 jets but with lower pt
    # (compared to higher threshold in miniAOD)
    ak8_label = "AK8PFPUPPI"
    ak8puppi_patname = 'patJets' + ak8_label
    print 'Adding', ak8puppi_patname
    ak8puppi_jet_source = 'ak8PuppiJets'
    addJetCollection(process,
                     labelName=ak8_label,
                     jetSource=cms.InputTag(ak8puppi_jet_source),
                     algo='AK',
                     rParam=getattr(process, ak8puppi_jet_source).rParam.value(),  # this is NOT used for jet cluster cone size, only for jet-X matching
                     genJetCollection=cms.InputTag('slimmedGenJetsAK8'),

                     jetCorrections=('AK8PFPuppi', ['L2Relative', 'L3Absolute'], 'None'),

                     pfCandidates=cms.InputTag('packedPFCandidates'),
                     pvSource=cms.InputTag('offlineSlimmedPrimaryVertices'),
                     svSource=cms.InputTag('slimmedSecondaryVertices'),
                     muSource=cms.InputTag('slimmedMuons'),
                     elSource=cms.InputTag('slimmedElectrons'),
                     getJetMCFlavour=not useData
                     )
    delattr(process, "selectedPatJets"+cap(ak8_label))
    # For data, turn off every gen-related part - can't do this via
    # addJetCollection annoyingly
    if useData:
        producer = getattr(process, ak8puppi_patname)
        modify_patjetproducer_for_data(process, producer)

    ak8_label = "AK8PFCHS"
    ak8chs_patname = 'patJets' + ak8_label
    print 'Adding', ak8chs_patname
    ak8chs_jet_source = "ak8CHSJets"
    addJetCollection(process,
                     labelName=ak8_label,
                     jetSource=cms.InputTag(ak8chs_jet_source),
                     algo='AK',
                     rParam=getattr(process, ak8chs_jet_source).rParam.value(),  # this is NOT used for jet cluster cone size, only for jet-X matching
                     genJetCollection=cms.InputTag('slimmedGenJetsAK8'),
                     jetCorrections=('AK8PFchs', ['L1FastJet', 'L2Relative', 'L3Absolute'], 'None'),
                     pfCandidates=cms.InputTag('packedPFCandidates'),
                     pvSource=cms.InputTag('offlineSlimmedPrimaryVertices'),
                     svSource=cms.InputTag('slimmedSecondaryVertices'),
                     muSource=cms.InputTag('slimmedMuons'),
                     elSource=cms.InputTag('slimmedElectrons'),
                     getJetMCFlavour=not useData
                     )
    delattr(process, "selectedPatJets"+cap(ak8_label))
    # For data, turn off every gen-related part - can't do this via
    # addJetCollection annoyingly
    if useData:
        producer = getattr(process, ak8chs_patname)
        modify_patjetproducer_for_data(process, producer)

    ###############################################
    # Packing substructure: fat jets + subjets
    #
    # Add subjets from groomed fat jet to its corresponding ungroomed fatjet
    process.packedPatJetsAk8CHSJets = cms.EDProducer("JetSubstructurePacker",
        jetSrc = cms.InputTag("patJetsAk8CHSJetsFat"),
        distMax = cms.double(0.8),
        algoTags = cms.VInputTag(
            cms.InputTag("patJetsAk8CHSJetsSoftDropPacked")
        ),
        algoLabels = cms.vstring(
            'SoftDropCHS'
        ),
        fixDaughters = cms.bool(False)
    )
    task.add(process.packedPatJetsAk8CHSJets)

    process.packedPatJetsAk8PuppiJets = cms.EDProducer("JetSubstructurePacker",
        jetSrc = cms.InputTag("patJetsAk8PuppiJetsFat"),
        distMax = cms.double(0.8),
        algoTags = cms.VInputTag(
            cms.InputTag("patJetsAk8PuppiJetsSoftDropPacked")
        ),
        algoLabels = cms.vstring(
            'SoftDropPuppi'
        ),
        fixDaughters = cms.bool(False)
    )
    task.add(process.packedPatJetsAk8PuppiJets)

    ###############################################
    # Do deep flavours & deep tagging
    # This MUST be run *After* JetSubstructurePacker, so that the subjets are already there,
    # otherwise the DeepBoostedJetTagInfoProducer will fail
    # Also add in PUPPI multiplicities while we're at it.
    for name in ['slimmedJets', 'slimmedJetsPuppi', 'patJetsAK8PFPUPPI', 'packedPatJetsAk8PuppiJets','packedPatJetsAk8CHSJets']:
        print "name", name
        labelName = cap(name)
        is_ak8 = "ak8" in name.lower()
        is_puppi = "puppi" in name.lower()
        is_reclustered = "slimmed" not in name.lower()
        is_topjet = "packed" in name.lower()
        # This postfix is VERY IMPORTANT for reclustered puppi, as the puppi weights
        # are already applied. If it doesn't have this postfix then it will apply
        # puppi weights.
        # See https://github.com/cms-sw/cmssw/blob/CMSSW_10_2_10/PhysicsTools/PatAlgos/python/tools/jetTools.py#L653
        # Please check in future releases if this is still the case!
        # It's needed only for pfDeepBoostedJetTagInfos
        postfix = "WithPuppiDaughters" if is_puppi and is_reclustered else "NewDFTraining"  # NewDFTraining is not special, could just be ''

        # This call to updateJetCollection adds one PATJetUpdater to only remove the JECs,
        # then uses that as the input to another PATJetUpdater, which re-applies the JECs,
        # adds in all b tag stuff, etc.
        # The 2nd PATJetsUpdater has the extra "TransientCorrected" bit in its name.
        # It also produces a final similar "selectedUpdatedPatJets"+labelName+postfix collection
        # which is a PATJetSelector
        updater_src = "updatedPatJets" + labelName + postfix  # 1st PATJetUpdater, that removes JECs, is src to updater_name
        updater_name = "updatedPatJetsTransientCorrected" + labelName + postfix  # 2nd PATJetUpdater
        selector_name = "selectedUpdatedPatJets" + labelName + postfix

        if is_ak8 and is_puppi:
            correction_tag = "AK8PFPuppi"
        elif is_ak8 and not is_puppi:
            correction_tag = "AK8PFchs"
        elif not is_ak8 and is_puppi:
            correction_tag = "AK4PFPuppi"
        elif not is_ak8 and not is_puppi:
            correction_tag = "AK4PFchs"

        else:
            raise RuntimeError("No idea which jet correction tag you need here")

        jetcorr_list = ['L1FastJet', 'L2Relative', 'L3Absolute']
        if is_puppi:
            jetcorr_list = jetcorr_list[1:]
        if useData:
            jetcorr_list.append("L2L3Residual")

        discriminators = ak4btagDiscriminators[:]
#        discriminators = bTagDiscriminators[:]
        discriminators.extend(ak4btagDiscriminators)
        if is_ak8 and is_topjet:
            discriminators.extend(ak8btagDiscriminators)

        updateJetCollection(
            process,
            labelName=labelName,
            jetSource=cms.InputTag(name),
            pvSource=cms.InputTag('offlineSlimmedPrimaryVertices'),
            svSource=cms.InputTag('slimmedSecondaryVertices'),
            jetCorrections=(correction_tag, cms.vstring(jetcorr_list), 'None'),  # Can't use None here as we are doing btagging for some reason.
            btagDiscriminators=discriminators,
            postfix=postfix
        )

        # Add puppi multiplicity producers
        # For each, we have to add a PATPuppiJetSpecificProducer,
        # then update the relevant pat::Jet collection using updateJetCollection
        # using userFloats mechanism
        # Crucially, the PATPuppiJetSpecificProducer module name MUST be the same
        # as the final jet collection, with only "patPuppiJetSpecificProducer" prepended
        # so that NtupleWriterJets can find the stored userFloat
        puppi_mult_name = "patPuppiJetSpecificProducer" + updater_name
        setattr(process,
                puppi_mult_name,
                cms.EDProducer("PATPuppiJetSpecificProducer",
                    src = cms.InputTag(updater_src)
                    )
                )
        task.add(getattr(process, puppi_mult_name))

        # We add in the userFloats to the last PATJetUpdater
        # This is because we use the jet collection name to access the userFloats in NtupleWriterJets
        getattr(process, updater_name).userData.userFloats.src = [
            '%s:puppiMultiplicity' % puppi_mult_name,
            '%s:neutralPuppiMultiplicity' % puppi_mult_name,
            '%s:neutralHadronPuppiMultiplicity' % puppi_mult_name,
            '%s:photonPuppiMultiplicity' % puppi_mult_name,
            '%s:HFHadronPuppiMultiplicity' % puppi_mult_name,
            '%s:HFEMPuppiMultiplicity' % puppi_mult_name
        ]

    def rename_module(process, task, current_name, new_name, update_userData=True):
        """Rename a module in a process, with option to also update userData.
        This clones the existing module at process.current_name into
        process.new_name. If update_userData = True, it then clones all
        the userData producers to replace any occurrences of `current_name`
        with `new_name`, and updates the new module to use these for its userData.

        This is useful for e.g. PATJetUpdater modules that always produce
        crazy long names.

        Parameters
        ----------
        process : cms.Process
        task : Task
        current_name : str
        new_name : str
        update_userData : bool
        """
        updater = getattr(process, current_name)
        new_updater = updater.clone()

        if update_userData:
            # Rejig userData. Since we expect the userData to often have same
            # collection name as this object, we need to rename that module as well
            user_data = new_updater.userData

            # Get current userData source module names
            user_data_fields = ['userCands', 'userClasses', 'userFloats', 'userInts']
            for field_name in user_data_fields:
                sources = [src.split(":")[0] for src in getattr(user_data, field_name).src]
                sources = list(l for l in set(sources) if l)  # ignore empty strings that are the default

                for old_src_name in sources:
                    # Get rid of old src module, clone, add new one
                    old_src = getattr(process, old_src_name)
                    new_src_name = old_src_name.replace(current_name, new_name)
                    new_src = old_src.clone()
                    setattr(process, new_src_name, new_src)
                    task.add(getattr(process, new_src_name))
                    task.remove(getattr(process, old_src_name))
                    delattr(process, old_src_name)

                # Update our main module with the new producer module
                new_srcs = [label.replace(current_name, new_name)
                            for label in getattr(user_data, field_name).src]
                setattr(getattr(user_data, field_name), "src", cms.VInputTag(new_srcs))  # TODO: check the original type

        # Add the new module to process & task
        print "Renaming '%s' to '%s'" % (current_name, new_name)
        setattr(process, new_name, new_updater)
        task.add(getattr(process, new_name))
        # Delete old one
        task.remove(getattr(process, current_name))
        delattr(process, current_name)


    ###############################################
    # Rename jet collections to something more user-friendly
    #
    # This must be the last step before passing to the NtupleWriter
    # Note that using rename_module() is not always the best option: an EDAlias
    # might be more suitable.
    # For something pre-made like slimmedJets, probably need to add a PATJetSelector,
    # with 0 cuts, and rename that
    #
    # Jet collections
    rename_module(process, task, "updatedPatJetsTransientCorrectedSlimmedJetsPuppiNewDFTraining", "jetsAk4Puppi")
    rename_module(process, task, "updatedPatJetsTransientCorrectedSlimmedJetsNewDFTraining", "jetsAk4CHS")
    rename_module(process, task, "updatedPatJetsTransientCorrectedPatJetsAK8PFPUPPIWithPuppiDaughters", "jetsAk8Puppi")
    rename_module(process, task, ak8chs_patname, "jetsAk8CHS")
    # TopJet collections
    rename_module(process, task, "updatedPatJetsTransientCorrectedPackedPatJetsAk8PuppiJetsWithPuppiDaughters", "jetsAk8PuppiSubstructure")
    rename_module(process, task, "updatedPatJetsTransientCorrectedPackedPatJetsAk8CHSJetsNewDFTraining", "jetsAk8CHSSubstructure", update_userData=False)
#    rename_module(process, task, "packedPatJetsAk8CHSJets", "jetsAk8CHSSubstructure", update_userData=False)  # don't update userData as JetSubstructurePacker


    # # Dummy module to allow us to rename slimmedJets to something more descriptive
    # process.jetsAk4CHS = cms.EDFilter("PATJetSelector",
    #     cut=cms.string(''),
    #     cutLoose=cms.string(''),
    #     nLoose=cms.uint32(0),
    #     src=cms.InputTag("slimmedJets")
    # )
    # task.add(process.jetsAk4CHS)


    # Higgs tagging commissioning
    process.pfBoostedDoubleSVTagInfos = cms.EDProducer("BoostedDoubleSVProducer",
        trackSelectionBlock,
        beta=cms.double(1.0),
        R0=cms.double(0.8),
        maxSVDeltaRToJet=cms.double(0.7),
        trackPairV0Filter=cms.PSet(
           k0sMassWindow=cms.double(0.03)
        ),
        svTagInfos=cms.InputTag("pfInclusiveSecondaryVertexFinderTagInfosAk8CHSJetsFat")  # This name is taken from the modules added by addJetcollection in add_fatjets_subjets
    )
    task.add(process.pfBoostedDoubleSVTagInfos)

    process.pfBoostedDoubleSVTagInfos.trackSelection.jetDeltaRMax = cms.double(0.8)

    ###############################################
    # HOTVR & XCONE
    #
    process.hotvrPuppi = cms.EDProducer("HOTVRProducer",
                                        src=cms.InputTag("puppi")
                                        )
    task.add(process.hotvrPuppi)

    process.hotvrGen = cms.EDProducer("GenHOTVRProducer",
        src=cms.InputTag("packedGenParticlesForJetsNoNu"),
        mu=cms.double(30),
        theta=cms.double(0.7),
        max_r=cms.double(1.5),
        min_r=cms.double(0.1),
        rho=cms.double(600),
        hotvr_pt_min=cms.double(30),
    )
    task.add(process.hotvrGen)

    usePseudoXCone = cms.bool(True)
    process.xconePuppi = cms.EDProducer("XConeProducer",
        src=cms.InputTag("puppi"),
        usePseudoXCone=usePseudoXCone,  # use PseudoXCone (faster) or XCone
        NJets = cms.uint32(2),          # number of fatjets
        RJets = cms.double(1.2),        # cone radius of fatjets
        BetaJets = cms.double(2.0),     # conical mesure (beta = 2.0 is XCone default)
        NSubJets = cms.uint32(3),       # number of subjets in each fatjet
        RSubJets = cms.double(0.4),     # cone radius of subjetSrc
        BetaSubJets = cms.double(2.0)   # conical mesure for subjets
    )
    task.add(process.xconePuppi)

    process.xconeCHS = cms.EDProducer("XConeProducer",
        src=cms.InputTag("chs"),
        usePseudoXCone=usePseudoXCone,  # use PseudoXCone (faster) or XCone
        NJets = cms.uint32(2),          # number of fatjets
        RJets = cms.double(1.2),        # cone radius of fatjets
        BetaJets = cms.double(2.0),     # conical mesure (beta = 2.0 is XCone default)
        NSubJets = cms.uint32(3),       # number of subjets in each fatjet
        RSubJets = cms.double(0.4),     # cone radius of subjetSrc
        BetaSubJets = cms.double(2.0)   # conical mesure for subjets
    )
    task.add(process.xconeCHS)

    process.genXCone23TopJets = cms.EDProducer("GenXConeProducer",
        src=cms.InputTag("packedGenParticlesForJetsNoNu"),
        usePseudoXCone=usePseudoXCone,  # use PseudoXCone (faster) or XCone
        NJets = cms.uint32(2),          # number of fatjets
        RJets = cms.double(1.2),        # cone radius of fatjets
        BetaJets = cms.double(2.0),     # conical mesure (beta = 2.0 is XCone default)
        NSubJets = cms.uint32(3),       # number of subjets in each fatjet
        RSubJets = cms.double(0.4),     # cone radius of subjetSrc
        BetaSubJets = cms.double(2.0),  # conical mesure for subjets
        doLeptonSpecific = cms.bool(True),  # if true, look for gen electron or muon,
        # and whichever jet it is closest do get clustered with NJets-1 instead of NJets
        DRLeptonJet = cms.double(999),  # here you can specify the maximum distance for a lepton-jet match
    )
    task.add(process.genXCone23TopJets)

    process.genXCone33TopJets = cms.EDProducer("GenXConeProducer",
        src=cms.InputTag("packedGenParticlesForJetsNoNu"),
        usePseudoXCone=usePseudoXCone,  # use PseudoXCone (faster) or XCone
        NJets = cms.uint32(2),          # number of fatjets
        RJets = cms.double(1.2),        # cone radius of fatjets
        BetaJets = cms.double(2.0),     # conical mesure (beta = 2.0 is XCone default)
        NSubJets = cms.uint32(3),       # number of subjets in each fatjet
        RSubJets = cms.double(0.4),     # cone radius of subjetSrc
        BetaSubJets = cms.double(2.0),  # conical mesure for subjets
        doLeptonSpecific = cms.bool(False),
    )
    task.add(process.genXCone33TopJets)


    process.xconeCHS4jets04 = cms.EDProducer("XConeProducer",
                                             src=cms.InputTag("chs"),
                                             usePseudoXCone=usePseudoXCone,  # use PseudoXCone (faster) or XCone
                                             NJets = cms.uint32(4),          # number of fatjets
                                             RJets = cms.double(0.4),        # cone radius of fatjets
                                             BetaJets = cms.double(2.0),     # conical mesure (beta = 2.0 is XCone default)
                                             NSubJets = cms.uint32(1),       # number of subjets in each fatjet
                                             RSubJets = cms.double(0.2),     # cone radius of subjetSrc
                                             BetaSubJets = cms.double(2.0)   # conical mesure for subjets
                                             )
    task.add(process.xconeCHS4jets04)

    process.xconeCHS3jets04 = cms.EDProducer("XConeProducer",
                                             src=cms.InputTag("chs"),
                                             usePseudoXCone=usePseudoXCone,  # use PseudoXCone (faster) or XCone
                                             NJets = cms.uint32(3),          # number of fatjets
                                             RJets = cms.double(0.4),        # cone radius of fatjets
                                             BetaJets = cms.double(2.0),     # conical mesure (beta = 2.0 is XCone default)
                                             NSubJets = cms.uint32(1),       # number of subjets in each fatjet
                                             RSubJets = cms.double(0.2),     # cone radius of subjetSrc
                                             BetaSubJets = cms.double(2.0)   # conical mesure for subjets
                                             )
    task.add(process.xconeCHS3jets04)

    process.xconeCHS2jets04 = cms.EDProducer("XConeProducer",
                                             src=cms.InputTag("chs"),
                                             usePseudoXCone=usePseudoXCone,  # use PseudoXCone (faster) or XCone
                                             NJets = cms.uint32(2),          # number of fatjets
                                             RJets = cms.double(0.4),        # cone radius of fatjets
                                             BetaJets = cms.double(2.0),     # conical mesure (beta = 2.0 is XCone default)
                                             NSubJets = cms.uint32(1),       # number of subjets in each fatjet
                                             RSubJets = cms.double(0.2),     # cone radius of subjetSrc
                                             BetaSubJets = cms.double(2.0)   # conical mesure for subjets
                                             )
    task.add(process.xconeCHS2jets04)

    process.genXCone4jets04 = cms.EDProducer("GenXConeProducer",
                                             src=cms.InputTag("packedGenParticlesForJetsNoNu"),
                                             usePseudoXCone=usePseudoXCone,  # use PseudoXCone (faster) or XCone
                                             NJets = cms.uint32(4),          # number of fatjets
                                             RJets = cms.double(0.4),        # cone radius of fatjets
                                             BetaJets = cms.double(2.0),     # conical mesure (beta = 2.0 is XCone default)
                                             NSubJets = cms.uint32(1),       # number of subjets in each fatjet
                                             RSubJets = cms.double(0.2),     # cone radius of subjetSrc
                                             BetaSubJets = cms.double(2.0),   # conical mesure for subjets
                                             doLeptonSpecific = cms.bool(False),  # if true, look for gen electron or muon,
                                             # and whichever jet it is closest do get clustered with NJets-1 instead of NJets
                                             DRLeptonJet = cms.double(999),  # here you can specify the maximum distance for a lepton-jet match
                                             )
    task.add(process.genXCone4jets04)

    process.genXCone3jets04 = cms.EDProducer("GenXConeProducer",
                                             src=cms.InputTag("packedGenParticlesForJetsNoNu"),
                                             usePseudoXCone=usePseudoXCone,  # use PseudoXCone (faster) or XCone
                                             NJets = cms.uint32(3),          # number of fatjets
                                             RJets = cms.double(0.4),        # cone radius of fatjets
                                             BetaJets = cms.double(2.0),     # conical mesure (beta = 2.0 is XCone default)
                                             NSubJets = cms.uint32(1),       # number of subjets in each fatjet
                                             RSubJets = cms.double(0.2),     # cone radius of subjetSrc
                                             BetaSubJets = cms.double(2.0),   # conical mesure for subjets
                                             doLeptonSpecific = cms.bool(False),  # if true, look for gen electron or muon,
                                             # and whichever jet it is closest do get clustered with NJets-1 instead of NJets
                                             DRLeptonJet = cms.double(999),  # here you can specify the maximum distance for a lepton-jet match
                                             )
    task.add(process.genXCone3jets04)

    process.genXCone2jets04 = cms.EDProducer("GenXConeProducer",
                                             src=cms.InputTag("packedGenParticlesForJetsNoNu"),
                                             usePseudoXCone=usePseudoXCone,  # use PseudoXCone (faster) or XCone
                                             NJets = cms.uint32(2),          # number of fatjets
                                             RJets = cms.double(0.4),        # cone radius of fatjets
                                             BetaJets = cms.double(2.0),     # conical mesure (beta = 2.0 is XCone default)
                                             NSubJets = cms.uint32(1),       # number of subjets in each fatjet
                                             RSubJets = cms.double(0.2),     # cone radius of subjetSrc
                                             BetaSubJets = cms.double(2.0),   # conical mesure for subjets
                                             doLeptonSpecific = cms.bool(False),  # if true, look for gen electron or muon,
                                             # and whichever jet it is closest do get clustered with NJets-1 instead of NJets
                                             DRLeptonJet = cms.double(999),  # here you can specify the maximum distance for a lepton-jet match
                                             )
    task.add(process.genXCone2jets04)

    #XCONE for dijet studies R=0.8
    process.xconeCHS4jets08 = cms.EDProducer("XConeProducer",
                                             src=cms.InputTag("chs"),
                                             usePseudoXCone=usePseudoXCone,  # use PseudoXCone (faster) or XCone
                                             NJets = cms.uint32(4),          # number of fatjets
                                             RJets = cms.double(0.8),        # cone radius of fatjets
                                             BetaJets = cms.double(2.0),     # conical mesure (beta = 2.0 is XCone default)
                                             NSubJets = cms.uint32(1),       # number of subjets in each fatjet
                                             RSubJets = cms.double(0.4),     # cone radius of subjetSrc
                                             BetaSubJets = cms.double(2.0)   # conical mesure for subjets
                                             )
    task.add(process.xconeCHS4jets08)

    process.xconeCHS3jets08 = cms.EDProducer("XConeProducer",
                                             src=cms.InputTag("chs"),
                                             usePseudoXCone=usePseudoXCone,  # use PseudoXCone (faster) or XCone
                                             NJets = cms.uint32(3),          # number of fatjets
                                             RJets = cms.double(0.8),        # cone radius of fatjets
                                             BetaJets = cms.double(2.0),     # conical mesure (beta = 2.0 is XCone default)
                                             NSubJets = cms.uint32(1),       # number of subjets in each fatjet
                                             RSubJets = cms.double(0.4),     # cone radius of subjetSrc
                                             BetaSubJets = cms.double(2.0)   # conical mesure for subjets
                                             )
    task.add(process.xconeCHS3jets08)

    process.xconeCHS2jets08 = cms.EDProducer("XConeProducer",
                                             src=cms.InputTag("chs"),
                                             usePseudoXCone=usePseudoXCone,  # use PseudoXCone (faster) or XCone
                                             NJets = cms.uint32(2),          # number of fatjets
                                             RJets = cms.double(0.8),        # cone radius of fatjets
                                             BetaJets = cms.double(2.0),     # conical mesure (beta = 2.0 is XCone default)
                                             NSubJets = cms.uint32(1),       # number of subjets in each fatjet
                                             RSubJets = cms.double(0.4),     # cone radius of subjetSrc
                                             BetaSubJets = cms.double(2.0)   # conical mesure for subjets
                                             )
    task.add(process.xconeCHS2jets08)

    process.genXCone4jets08 = cms.EDProducer("GenXConeProducer",
                                             src=cms.InputTag("packedGenParticlesForJetsNoNu"),
                                             usePseudoXCone=usePseudoXCone,  # use PseudoXCone (faster) or XCone
                                             NJets = cms.uint32(4),          # number of fatjets
                                             RJets = cms.double(0.8),        # cone radius of fatjets
                                             BetaJets = cms.double(2.0),     # conical mesure (beta = 2.0 is XCone default)
                                             NSubJets = cms.uint32(1),       # number of subjets in each fatjet
                                             RSubJets = cms.double(0.4),     # cone radius of subjetSrc
                                             BetaSubJets = cms.double(2.0),   # conical mesure for subjets
                                             doLeptonSpecific = cms.bool(False),  # if true, look for gen electron or muon,
                                             # and whichever jet it is closest do get clustered with NJets-1 instead of NJets
                                             DRLeptonJet = cms.double(999),  # here you can specify the maximum distance for a lepton-jet match
                                             )
    task.add(process.genXCone4jets08)

    process.genXCone3jets08 = cms.EDProducer("GenXConeProducer",
                                             src=cms.InputTag("packedGenParticlesForJetsNoNu"),
                                             usePseudoXCone=usePseudoXCone,  # use PseudoXCone (faster) or XCone
                                             NJets = cms.uint32(3),          # number of fatjets
                                             RJets = cms.double(0.8),        # cone radius of fatjets
                                             BetaJets = cms.double(2.0),     # conical mesure (beta = 2.0 is XCone default)
                                             NSubJets = cms.uint32(1),       # number of subjets in each fatjet
                                             RSubJets = cms.double(0.4),     # cone radius of subjetSrc
                                             BetaSubJets = cms.double(2.0),   # conical mesure for subjets
                                             doLeptonSpecific = cms.bool(False),  # if true, look for gen electron or muon,
                                             # and whichever jet it is closest do get clustered with NJets-1 instead of NJets
                                             DRLeptonJet = cms.double(999),  # here you can specify the maximum distance for a lepton-jet match
                                             )
    task.add(process.genXCone3jets08)

    process.genXCone2jets08 = cms.EDProducer("GenXConeProducer",
                                             src=cms.InputTag("packedGenParticlesForJetsNoNu"),
                                             usePseudoXCone=usePseudoXCone,  # use PseudoXCone (faster) or XCone
                                             NJets = cms.uint32(2),          # number of fatjets
                                             RJets = cms.double(0.8),        # cone radius of fatjets
                                             BetaJets = cms.double(2.0),     # conical mesure (beta = 2.0 is XCone default)
                                             NSubJets = cms.uint32(1),       # number of subjets in each fatjet
                                             RSubJets = cms.double(0.4),     # cone radius of subjetSrc
                                             BetaSubJets = cms.double(2.0),   # conical mesure for subjets
                                             doLeptonSpecific = cms.bool(False),  # if true, look for gen electron or muon,
                                             # and whichever jet it is closest do get clustered with NJets-1 instead of NJets
                                             DRLeptonJet = cms.double(999),  # here you can specify the maximum distance for a lepton-jet match
                                             )
    task.add(process.genXCone2jets08)

    ###############################################
    # LEPTON configuration
    #

    # collections for lepton PF-isolation deposits
    process.load('UHH2.core.pfCandidatesByType_cff')
    process.load('CommonTools.ParticleFlow.deltaBetaWeights_cff')

    ###############################################
    # MUONS
    #
    # mini-isolation
    mu_isovals = []

    load_muonPFMiniIso(process, 'muonPFMiniIsoSequenceSTAND', algo='STAND',
                       src='slimmedMuons',
                       src_charged_hadron='pfAllChargedHadrons',
                       src_neutral_hadron='pfAllNeutralHadrons',
                       src_photon='pfAllPhotons',
                       src_charged_pileup='pfPileUpAllChargedParticles',
                       isoval_list=mu_isovals
                       )

    load_muonPFMiniIso(process, 'muonPFMiniIsoSequencePFWGT', algo='PFWGT',
                       src='slimmedMuons',
                       src_neutral_hadron='pfWeightedNeutralHadrons',
                       src_photon='pfWeightedPhotons',
                       isoval_list=mu_isovals
                       )
    for m in mu_isovals:
        task.add(getattr(process, m))
        task.add(getattr(process, m.replace("Value", "Deposit")))

    process.slimmedMuonsUSER = cms.EDProducer('PATMuonUserData',
                                              src=cms.InputTag('slimmedMuons'),
                                              vmaps_double=cms.vstring(mu_isovals),
                                              )
    task.add(process.slimmedMuonsUSER)

    ###############################################
    # ELECTRONS
    #
    # mini-isolation
    # FIXME: should we use the already existing miniIsolatioin in pat::Lepton?
    el_isovals = []

    load_elecPFMiniIso(process,
        'elecPFMiniIsoSequenceSTAND',
        algo = 'STAND',
        src = 'slimmedElectrons',
        src_charged_hadron = 'pfAllChargedHadrons',
        src_neutral_hadron = 'pfAllNeutralHadrons',
        src_photon         = 'pfAllPhotons',
        src_charged_pileup = 'pfPileUpAllChargedParticles',
        isoval_list = el_isovals
    )
    needed_collections = [
        'convertedPackedPFCandidates',
        'convertedPackedPFCandidatePtrs',
        'pfPileUp',
        'pfNoPileUp',
        'pfAllChargedHadrons',
        'pfAllNeutralHadrons',
        'pfAllPhotons',
        'pfPileUpAllChargedParticles',
        'pfWeightedNeutralHadrons',
        'pfWeightedPhotons',
        'pfAllChargedParticles',
        ]
    for m in needed_collections:
        task.add(getattr(process, m))

    load_elecPFMiniIso(process,
        'elecPFMiniIsoSequencePFWGT',
        algo = 'PFWGT',
        src = 'slimmedElectrons',
        src_neutral_hadron = 'pfWeightedNeutralHadrons',
        src_photon         = 'pfWeightedPhotons',
        isoval_list = el_isovals
    )
    for m in el_isovals:
        task.add(getattr(process, m))
        task.add(getattr(process, m.replace('Value', 'Deposit')))



    # electron ID from VID
    switchOnVIDElectronIdProducer(process, DataFormat.MiniAOD)

    elecID_mod_ls = [
        # For 2016
        'RecoEgamma.ElectronIdentification.Identification.heepElectronID_HEEPV60_cff',
        'RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_Summer16_80X_V1_cff',
        'RecoEgamma.ElectronIdentification.Identification.cutBasedElectronHLTPreselecition_Summer16_V1_cff',
        'RecoEgamma.ElectronIdentification.Identification.mvaElectronID_Spring16_GeneralPurpose_V1_cff',
        'RecoEgamma.ElectronIdentification.Identification.mvaElectronID_Spring16_HZZ_V1_cff',
        # For 2017 (& 2018 for now)
        'RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_Fall17_94X_V2_cff',
        'RecoEgamma.ElectronIdentification.Identification.heepElectronID_HEEPV70_cff',
        'RecoEgamma.ElectronIdentification.Identification.mvaElectronID_Fall17_noIso_V2_cff',
        'RecoEgamma.ElectronIdentification.Identification.mvaElectronID_Fall17_iso_V2_cff',
    ]

    for mod in elecID_mod_ls:
        setupAllVIDIdsInModule(process, mod, setupVIDElectronSelection)



    from RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_Summer16_80X_V1_cff import isoInputs as ele_iso_16
    from RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_Fall17_94X_V2_cff import isoInputs as ele_iso_17
    iso_input_era_dict = {
        "2016v2": ele_iso_16,
        "2016v3": ele_iso_16,
        "2017": ele_iso_17,
        "2018": ele_iso_17,
    }

    # slimmedElectronsUSER ( = slimmedElectrons + USER variables)
    process.slimmedElectronsUSER = cms.EDProducer('PATElectronUserData',
                                                  src=cms.InputTag(
                                                      'slimmedElectrons'),

                                                  vmaps_bool=cms.PSet(
                                                      # 2016
                                                      heepElectronID_HEEPV60=cms.InputTag(
                                                          'egmGsfElectronIDs:heepElectronID-HEEPV60'),
                                                      cutBasedElectronID_Summer16_80X_V1_veto=cms.InputTag(
                                                          'egmGsfElectronIDs:cutBasedElectronID-Summer16-80X-V1-veto'),
                                                      cutBasedElectronID_Summer16_80X_V1_loose=cms.InputTag(
                                                          'egmGsfElectronIDs:cutBasedElectronID-Summer16-80X-V1-loose'),
                                                      cutBasedElectronID_Summer16_80X_V1_medium=cms.InputTag(
                                                          'egmGsfElectronIDs:cutBasedElectronID-Summer16-80X-V1-medium'),
                                                      cutBasedElectronID_Summer16_80X_V1_tight=cms.InputTag(
                                                          'egmGsfElectronIDs:cutBasedElectronID-Summer16-80X-V1-tight'),
                                                      cutBasedElectronHLTPreselection_Summer16_V1=cms.InputTag(
                                                          'egmGsfElectronIDs:cutBasedElectronHLTPreselection-Summer16-V1'),
                                                      # 2017 & 2018
                                                      cutBasedElectronID_Fall17_94X_V2_veto=cms.InputTag(
                                                          'egmGsfElectronIDs:cutBasedElectronID-Fall17-94X-V2-veto'),
                                                      cutBasedElectronID_Fall17_94X_V2_loose=cms.InputTag(
                                                          'egmGsfElectronIDs:cutBasedElectronID-Fall17-94X-V2-loose'),
                                                      cutBasedElectronID_Fall17_94X_V2_medium=cms.InputTag(
                                                          'egmGsfElectronIDs:cutBasedElectronID-Fall17-94X-V2-medium'),
                                                      cutBasedElectronID_Fall17_94X_V2_tight=cms.InputTag(
                                                          'egmGsfElectronIDs:cutBasedElectronID-Fall17-94X-V2-tight'),
                                                      heepElectronID_HEEPV70=cms.InputTag(
                                                          'egmGsfElectronIDs:heepElectronID-HEEPV70'),
                                                      mvaEleID_Fall17_noIso_V2_wp90=cms.InputTag(
                                                          'egmGsfElectronIDs:mvaEleID-Fall17-noIso-V2-wp90'),
                                                      mvaEleID_Fall17_noIso_V2_wp80=cms.InputTag(
                                                          'egmGsfElectronIDs:mvaEleID-Fall17-noIso-V2-wp80'),
                                                      mvaEleID_Fall17_noIso_V2_wpLoose=cms.InputTag(
                                                          'egmGsfElectronIDs:mvaEleID-Fall17-noIso-V2-wpLoose'),
                                                      mvaEleID_Fall17_iso_V2_wp90=cms.InputTag(
                                                          'egmGsfElectronIDs:mvaEleID-Fall17-iso-V2-wp90'),
                                                      mvaEleID_Fall17_iso_V2_wp80=cms.InputTag(
                                                          'egmGsfElectronIDs:mvaEleID-Fall17-iso-V2-wp80'),
                                                      mvaEleID_Fall17_iso_V2_wpLoose=cms.InputTag(
                                                          'egmGsfElectronIDs:mvaEleID-Fall17-iso-V2-wpLoose'),
                                                  ),

                                                  vmaps_float=cms.PSet(
                                                      mvaGeneralPurpose=cms.InputTag(
                                                          'electronMVAValueMapProducer:ElectronMVAEstimatorRun2Spring16GeneralPurposeV1Values'),
                                                      mvaHZZ=cms.InputTag(
                                                          'electronMVAValueMapProducer:ElectronMVAEstimatorRun2Spring16HZZV1Values'),
                                                      ElectronMVAEstimatorIso=cms.InputTag(
                                                          'electronMVAValueMapProducer:ElectronMVAEstimatorRun2Fall17IsoV2Values'),
                                                      ElectronMVAEstimatorNoIso=cms.InputTag(
                                                          'electronMVAValueMapProducer:ElectronMVAEstimatorRun2Fall17NoIsoV2Values')
                                                  ),

                                                  vmaps_double=cms.vstring(el_isovals),
                                                  effAreas_file=cms.FileInPath(iso_input_era_dict[year].isoEffAreas)
                                                  )
    task.add(process.egmGsfElectronIDs)
    task.add(process.slimmedElectronsUSER)


    # photon ID from VID
    switchOnVIDPhotonIdProducer(process, DataFormat.MiniAOD)
    photonID_mod_ls = [
        # For 2016
        'RecoEgamma.PhotonIdentification.Identification.cutBasedPhotonID_Spring16_V2p2_cff',
        'RecoEgamma.PhotonIdentification.Identification.mvaPhotonID_Spring16_nonTrig_V1_cff',
        # For 2017 (& 2018 for now)
        'RecoEgamma.PhotonIdentification.Identification.cutBasedPhotonID_Fall17_94X_V2_cff',
        'RecoEgamma.PhotonIdentification.Identification.mvaPhotonID_Fall17_94X_V2_cff',
    ]

    for mod in photonID_mod_ls:
        setupAllVIDIdsInModule(process, mod, setupVIDPhotonSelection)
    process.slimmedPhotonsUSER = cms.EDProducer('PATPhotonUserData',
                                                  src=cms.InputTag(
                                                      'slimmedPhotons'),

                                                  vmaps_bool=cms.PSet(
                                                      # 2016
                                                      cutBasedPhotonID_Spring16_V2p2_loose=cms.InputTag(
                                                          'egmPhotonIDs:cutBasedPhotonID-Spring16-V2p2-loose'),
                                                      cutBasedPhotonID_Spring16_V2p2_medium=cms.InputTag(
                                                          'egmPhotonIDs:cutBasedPhotonID-Spring16-V2p2-medium'),
                                                      cutBasedPhotonID_Spring16_V2p2_tight=cms.InputTag(
                                                          'egmPhotonIDs:cutBasedPhotonID-Spring16-V2p2-tight'),
                                                      mvaPhoID_Spring16_nonTrig_V1_wp90=cms.InputTag(
                                                          'egmPhotonIDs:mvaPhoID-Spring16-nonTrig-V1-wp90'),
                                                      mvaPhoID_Spring16_nonTrig_V1_wp80=cms.InputTag(
                                                          'egmPhotonIDs:mvaPhoID-Spring16-nonTrig-V1-wp80'),

                                                      # 2017 & 2018
                                                      cutBasedPhotonID_Fall17_94X_V2_loose=cms.InputTag(
                                                          'egmPhotonIDs:cutBasedPhotonID-Fall17-94X-V2-loose'),
                                                      cutBasedPhotonID_Fall17_94X_V2_medium=cms.InputTag(
                                                          'egmPhotonIDs:cutBasedPhotonID-Fall17-94X-V2-medium'),
                                                      cutBasedPhotonID_Fall17_94X_V2_tight=cms.InputTag(
                                                          'egmPhotonIDs:cutBasedPhotonID-Fall17-94X-V2-tight'),
                                                      mvaPhoID_Fall17_iso_V2_wp90=cms.InputTag(
                                                          'egmPhotonIDs:mvaPhoID-RunIIFall17-v2-wp90'),
                                                      mvaPhoID_Fall17_iso_V2_wp80=cms.InputTag(
                                                          'egmPhotonIDs:mvaPhoID-RunIIFall17-v2-wp80'),
                                                      ),
                                                )
    task.add(process.egmPhotonIDs)
    task.add(process.slimmedPhotonsUSER)

    ###############################################
    # TRIGGER, MET FILTERS
    #
    # L1 prefiring, only needed for simulation in 2016/7
    #
    prefire_era_dict = {
        '2016v2': '2016BtoH',
        '2016v3': '2016BtoH',
        '2017': '2017BtoF'
    }
    prefire_era = None if useData else prefire_era_dict.get(year, None)
    do_prefire = prefire_era is not None
    prefire_source = "prefiringweight"
    if do_prefire:
        setattr(process,
                prefire_source,
                cms.EDProducer("L1ECALPrefiringWeightProducer",
                    ThePhotons = cms.InputTag("slimmedPhotons"),
                    TheJets = cms.InputTag("slimmedJets"),
                    L1Maps = cms.string("L1PrefiringMaps.root"),
                    DataEra = cms.string(prefire_era),
                    UseJetEMPt = cms.bool(False),  # can be set to true to use jet prefiring maps parametrized vs pt(em) instead of pt
                    PrefiringRateSystematicUncty = cms.double(0.2),  # Minimum relative prefiring uncty per object
                    SkipWarnings = cms.bool(True)
                )
        )
        task.add(getattr(process, prefire_source))

    ###############################################
    # Deal with bad ECAL endcap crystals
    # https://twiki.cern.ch/twiki/bin/viewauth/CMS/MissingETOptionalFiltersRun2#How_to_run_ecal_BadCalibReducedM
    bad_ecal = year in ['2017', '2018'] and useData
    if bad_ecal:
        process.load('RecoMET.METFilters.ecalBadCalibFilter_cfi')

        baddetEcallist = cms.vuint32(
            [872439604,872422825,872420274,872423218,
             872423215,872416066,872435036,872439336,
             872420273,872436907,872420147,872439731,
             872436657,872420397,872439732,872439339,
             872439603,872439861,872437051,
             872437052,872420649,872422436,872421950,
             872437185,872422564,872421566,872421695,
             872421955,872421567,872437184,872421951,
             872421694,872437056,872437057,872437313

             # Are these supposed to be used as well?
             # 872438182,872438951,872439990,872439864,
             # 872439609,872437181,872437182,872437053,
             # 872436794,872436667,872436536,872421541,
             # 872421413,872421414,872421031,872423083,872421439
             ])


        process.ecalBadCalibReducedMINIAODFilter = cms.EDFilter("EcalBadCalibFilter",
            EcalRecHitSource = cms.InputTag("reducedEgamma:reducedEERecHits"),
            ecalMinEt = cms.double(50.),
            baddetEcal = baddetEcallist,
            taggingMode = cms.bool(True),
            debug = cms.bool(False)
        )
        task.add(process.ecalBadCalibReducedMINIAODFilter)

    # Run Bad Charged Hadron and Bad Muon filters for 2016v2, since they were
    # only introduced after samples were produced.
    # Newer samples will already have these.
    do_bad_muon_charged_filters = (year == "2016v2")
    if do_bad_muon_charged_filters:
        process.load('RecoMET.METFilters.BadPFMuonFilter_cfi')
        process.BadPFMuonFilter.muons = cms.InputTag("slimmedMuons")
        process.BadPFMuonFilter.PFCandidates = cms.InputTag("packedPFCandidates")
        process.BadPFMuonFilter.taggingMode = False  # Run in filter mode to reject events, not store them
        task.add(process.BadPFMuonFilter)

        process.load('RecoMET.METFilters.BadChargedCandidateFilter_cfi')
        process.BadChargedCandidateFilter.muons = cms.InputTag("slimmedMuons")
        process.BadChargedCandidateFilter.PFCandidates = cms.InputTag("packedPFCandidates")
        process.BadChargedCandidateFilter.taggingMode = False
        task.add(process.BadChargedCandidateFilter)

    # NtupleWriter
    #

    if useData:
        metfilterpath = "RECO"
    else:
        metfilterpath = "PAT"


    if (year == "2016v2" or year == "2016v3"):
       ## MET CHS (not available as slimmedMET collection)
       ## copied from https://github.com/cms-jet/JMEValidator/blob/CMSSW_7_6_X/python/FrameworkConfiguration.py
        def clean_met_(met):
            del met.t01Variation
            del met.t1Uncertainties
            del met.t1SmearedVarsAndUncs
            del met.tXYUncForRaw
            del met.tXYUncForT1
            del met.tXYUncForT01
            del met.tXYUncForT1Smear
            del met.tXYUncForT01Smear

        from PhysicsTools.PatAlgos.tools.metTools import addMETCollection

        ## Raw PAT METs
        process.load('RecoMET.METProducers.PFMET_cfi')
        process.pfMet.src = cms.InputTag('chs')
        task.add(process.pfMet)
        addMETCollection(process, labelName='patPFMetCHS', metSource='pfMet') # RAW MET
        addMETCollection(process, labelName='patPFMet', metSource='pfMet') # RAW MET
        process.patPFMet.addGenMET = False
        process.patPFMetCHS.addGenMET = False
        ## Slimmed METs
        from PhysicsTools.PatAlgos.slimming.slimmedMETs_cfi import slimmedMETs
        #### CaloMET is not available in MiniAOD
        del slimmedMETs.caloMET
        # ### CHS
        process.slMETsCHS = slimmedMETs.clone()
        process.slMETsCHS.src = cms.InputTag("patPFMetCHS")
        process.slMETsCHS.rawUncertainties = cms.InputTag("patPFMetCHS") # only central value
        process.slMETsCHS.chsMET = cms.InputTag("patPFMetCHS")
        process.slMETsCHS.trkMET = cms.InputTag("patPFMetCHS")
        task.add(process.slMETsCHS)
        clean_met_(process.slMETsCHS)
            

    process.MyNtuple = cms.EDFilter('NtupleWriter',
                                    # AnalysisModule = cms.PSet(
                                    #    name = cms.string("NoopAnalysisModule"),
                                    #    library = cms.string("libUHH2examples.so"),
                                    #    # Note: all other settings of type string are passed to the module, e.g.:
                                    #    TestKey = cms.string("TestValue")
                                    #),
                                    fileName=cms.string("Ntuple.root"),
                                    year=cms.string(year),
                                    doPV=cms.bool(True),
                                    pv_sources=cms.vstring("offlineSlimmedPrimaryVertices"),
                                    doRho=cms.untracked.bool(True),
                                    rho_source=cms.InputTag("fixedGridRhoFastjetAll"),

                                    save_lepton_keys=cms.bool(True),

                                    doElectrons=cms.bool(True),
                                    electron_source=cms.InputTag("slimmedElectronsUSER"),
                                    electron_IDtags=cms.vstring(
                                        # keys to be stored in UHH2 Electron class via the tag mechanism:
                                        # each string should correspond to a variable saved
                                        # via the "userInt" method in the pat::Electron collection used 'electron_source'
                                        # [the configuration of the pat::Electron::userInt variables should be done in PATElectronUserData]
                                        # 2016
                                        'heepElectronID_HEEPV60',
                                        'cutBasedElectronID_Summer16_80X_V1_veto',
                                        'cutBasedElectronID_Summer16_80X_V1_loose',
                                        'cutBasedElectronID_Summer16_80X_V1_medium',
                                        'cutBasedElectronID_Summer16_80X_V1_tight',
                                        'cutBasedElectronHLTPreselection_Summer16_V1',
                                        # 2017 & 2018
                                        'cutBasedElectronID_Fall17_94X_V2_veto',
                                        'cutBasedElectronID_Fall17_94X_V2_loose',
                                        'cutBasedElectronID_Fall17_94X_V2_medium',
                                        'cutBasedElectronID_Fall17_94X_V2_tight',
                                        'heepElectronID_HEEPV70',
                                        'mvaEleID_Fall17_noIso_V2_wp90',
                                        'mvaEleID_Fall17_noIso_V2_wp80',
                                        'mvaEleID_Fall17_noIso_V2_wpLoose',
                                        'mvaEleID_Fall17_iso_V2_wp90',
                                        'mvaEleID_Fall17_iso_V2_wp80',
                                        'mvaEleID_Fall17_iso_V2_wpLoose',
                                    ),

                                    doMuons=cms.bool(True),
                                    muon_sources=cms.vstring("slimmedMuonsUSER"),

                                    doTaus=cms.bool(True),
                                    tau_sources=cms.vstring("slimmedTaus"),
                                    tau_ptmin=cms.double(0.0),
                                    tau_etamax=cms.double(999.0),

                                    doPhotons=cms.bool(True),
                                    photon_sources = cms.InputTag("slimmedPhotonsUSER"),
                                    photon_IDtags=cms.vstring(
                                        # keys to be stored in UHH2 Electron class via the tag mechanism:
                                        # each string should correspond to a variable saved
                                        # via the "userInt" method in the pat::Electron collection used 'photon_source'
                                        # [the configuration of the pat::Electron::userInt variables should be done in PATElectronUserData]
                                        # 2016
                                        'cutBasedPhotonID_Spring16_V2p2_loose',
                                        'cutBasedPhotonID_Spring16_V2p2_medium',
                                        'cutBasedPhotonID_Spring16_V2p2_tight',
                                        'mvaPhoID_Spring16_nonTrig_V1_wp90',
                                        'mvaPhoID_Spring16_nonTrig_V1_wp80',
                                        # 2017 & 2018
                                        'cutBasedPhotonID_Fall17_94X_V2_loose',
                                        'cutBasedPhotonID_Fall17_94X_V2_medium',
                                        'cutBasedPhotonID_Fall17_94X_V2_tight',
                                        'mvaPhoID_Fall17_iso_V2_wp80',
                                        'mvaPhoID_Fall17_iso_V2_wp90',
                                    ),

                                    doJets=cms.bool(True),
                                    jet_sources=cms.vstring(
                                        "jetsAk4CHS",
                                        "jetsAk4Puppi",
                                        "jetsAk8CHS",
                                        "jetsAk8Puppi"
                                        ),
                                    jet_ptmin=cms.double(10.0),
                                    jet_etamax=cms.double(999.0),

                                    doMET=cms.bool(True),
                                    met_sources=met_sources_GL,

                                    doGenMET=cms.bool(not useData),
                                    genmet_sources=cms.vstring("slimmedMETs"),

                                    # doTopJets=cms.bool(False),
                                    doTopJets = cms.bool(True),
                                    topjet_ptmin=cms.double(150.0),
                                    topjet_etamax=cms.double(5.0),

                                    TopJets=cms.VPSet(
                                        # Each PSet outputs a TopJet collection, with name {topjet_source}_{subjet_source}
                                        # For each, we store the jets in topjet_source as TopJet objects,
                                        # with its subjets stored depending on subjet_source.
                                        # Tagging info can also be stored, as well as various substructure variables.

                                        cms.PSet(
                                            # The topjet_source can either be groomed or ungroomed,
                                            # but note that it determines the pt/eta/phi etc of the TopJet object
                                            # This can be e.g. slimmedJetsAK8, or a collection derived
                                            # from a JetSubstructurePacker: the latter makes it easier to handle subjets.
                                            # It also means we can store ungroomed quantities,
                                            # and the groomed subjets
                                            # (from which one can then recover the groomed fatjet 4-vector).
                                            topjet_source=cms.string("jetsAk8CHSSubstructure"),

                                            # One should use the algoLabels as used in the relevant JetSubstructurePacker
                                            # to access the subjets using pat::Jet::subjet(subjet_source).
                                            # Otherwise use "daughters" here to treat all jet daughters as subjets
                                            subjet_source=cms.string("SoftDropCHS"),

                                            # Specify if you want to store b-tagging taginfos for subjet collection,
                                            # make sure to have included them with .addTagInfos = True
                                            # addTagInfos = True is currently true by default, however,
                                            # only for collections produced and not read directly from miniAOD
                                            # Default is do_subjet_taginfo=False
                                            do_subjet_taginfo=cms.bool(True),

                                            # If you want to store the MVA Higgs tagger discriminator,
                                            # specify the jet collection from which to pick it up and the tagger name
                                            # currently the discriminator is trained on ungroomed jets, so
                                            # the discriminator has to be taken from ungroomed jets
                                            higgstag_source=cms.string("patJetsAk8CHSJetsFat"),
                                            higgstag_name=cms.string("pfBoostedDoubleSecondaryVertexAK8BJetTags"),
                                            higgstaginfo_source=cms.string("pfBoostedDoubleSVTagInfos"),

                                            # If empty, njettiness is directly taken from jet UserFloat,
                                            # otherwise taken from the provided source
                                            njettiness_source=cms.string("NjettinessAk8CHS"),
                                            # substructure_variables_source should be the
                                            # source as used in the module passed to njettiness_source
                                            substructure_variables_source=cms.string("ak8CHSJetsFat"),

                                            njettiness_groomed_source=cms.string("NjettinessAk8SoftDropCHS"),
                                            # substructure_groomed_variables_source should be the
                                            # source as used in the module passed to njettiness_groomed_source
                                            substructure_groomed_variables_source=cms.string("ak8CHSJetsSoftDropforsub"),

                                            softdropmass_source=cms.string("patJetsAk8CHSJetsSoftDropPacked"),

                                            # Energy correlation functions, for beta=1 and beta=2
                                            # If blank, will use the ones in the jet userFloat.
                                            # These are assumed to be calculated from the
                                            # substructure_groomed_variables_source
                                            ecf_beta1_source=cms.string("ECFNbeta1Ak8SoftDropCHS"),
                                            ecf_beta2_source=cms.string("ECFNbeta2Ak8SoftDropCHS")
                                        ),
                                        cms.PSet(
                                            # Use our reclustered AK8 PUPPI collection instead of slimmedJetsAK8,
                                            # to become uniform over years, and control the contents
                                            topjet_source=cms.string("jetsAk8PuppiSubstructure"),  # store ungroomed vars
                                            subjet_source=cms.string("SoftDropPuppi"),

                                            do_subjet_taginfo=cms.bool(True),

                                            higgstag_source=cms.string("patJetsAk8PuppiJetsFat"),
                                            higgstag_name=cms.string("pfBoostedDoubleSecondaryVertexAK8BJetTags"),
                                            higgstaginfo_source=cms.string("pfBoostedDoubleSVTagInfos"),  # FIXME Does this need replacing?

                                            njettiness_source=cms.string("NjettinessAk8Puppi"),
                                            substructure_variables_source=cms.string("ak8PuppiJetsFat"),

                                            njettiness_groomed_source=cms.string("NjettinessAk8SoftDropPuppi"),
                                            substructure_groomed_variables_source=cms.string("ak8PuppiJetsSoftDropforsub"),

                                            softdropmass_source=cms.string("patJetsAk8PuppiJetsSoftDropPacked"),

                                            ecf_beta1_source=cms.string("ECFNbeta1Ak8SoftDropPuppi"),
                                            ecf_beta2_source=cms.string("ECFNbeta2Ak8SoftDropPuppi")
                                        ),

                                        # cms.PSet(
                                        #     topjet_source=cms.string("slimmedJetsAK8"),  # puppijets in 2017 MiniAOD & later
                                        #     subjet_source=cms.string("SoftDropPuppi"),
                                        #     do_subjet_taginfo=cms.bool(False),
                                        #     higgstag_source=cms.string("patJetsAk8PuppiJetsFat"),
                                        #     higgstag_name=cms.string("pfBoostedDoubleSecondaryVertexAK8BJetTags"),
                                        #     #njettiness_source = cms.string(""),
                                        #     #substructure_variables_source = cms.string(""),
                                        #     njettiness_groomed_source = cms.string("NjettinessAk8SoftDropPuppi"),
                                        #     substructure_groomed_variables_source = cms.string("ak8PuppiJetsSoftDropforsub"),
                                        #     softdropmass_source=cms.string("ak8PFJetsPuppiSoftDropMass"),
                                        #     # switch off qjets for now, as it takes a long time:
                                        #     # qjets_source = cms.string("QJetsCa8CHS")
                                        #     # ecf_beta1_source=cms.string(""),
                                        #     # ecf_beta2_source=cms.string("")
                                        # ),
                                        # cms.PSet(
                                        #     # The fat jets that HepTopTag produces are the Top jet candidates,
                                        #     # i.e. the sum of its subjets. Therefore they will NOT have
                                        #     # the same pt/eta/phi as normal ca15 jets.
                                        #     # Unlike the other TopJet collections,
                                        #     # the pt/eta/phi here is the groomed one, since
                                        #     # we are primarily interested in the HTTTopJetTagInfo,
                                        #     # which is only stored for each groomed jet.
                                        #     topjet_source = cms.string(
                                        #         "patJetsHepTopTagCHSPacked"),
                                        #     subjet_source = cms.string("daughters"),
                                        #     do_subjet_taginfo = cms.bool(True),
                                        #     higgstag_source = cms.string(
                                        #         "patJetsCa15CHSJets"),
                                        #     higgstag_name=cms.string(
                                        #         "pfBoostedDoubleSecondaryVertexAK8BJetTags"),
                                        #     njettiness_source=cms.string(
                                        #         "NjettinessCa15CHS"),
                                        #     substructure_variables_source=cms.string(
                                        #         "ca15CHSJets"),
                                        #     njettiness_groomed_source=cms.string(
                                        #         "NjettinessCa15SoftDropCHS"),
                                        #     substructure_groomed_variables_source=cms.string(
                                        #         "ca15CHSJetsSoftDropforsub"),
                                        #     # Specify the module that makes
                                        #     # reco::HTTTopJetTagInfo
                                        #     toptagging_source=cms.string(
                                        #         "hepTopTagCHS"),
                                        #     # prunedmass_source = cms.string(
                                        #     #     "patJetsCa15CHSJetsPrunedPacked"),
                                        #     # softdropmass_source  = cms.string(""),
                                        #     # ecf_beta1_source=cms.string(
                                        #     #     "ECFNbeta1CA15SoftDropCHS"),
                                        #     # ecf_beta2_source=cms.string(
                                        #     #     "ECFNbeta2CA15SoftDropCHS")
                                        # ) ,
                                    ),

                                    doTrigger=cms.bool(True),
                                    trigger_bits=cms.InputTag("TriggerResults", "", "HLT"),
                                    # MET filters (HBHE noise, CSC, etc.) are stored as trigger Bits in
                                    # MINIAOD produced in path "PAT"/"RECO" with prefix "Flag_"
                                    metfilter_bits=cms.InputTag("TriggerResults", "", metfilterpath),
                                    # for now, save all the triggers:
                                    trigger_prefixes=cms.vstring("HLT_", "Flag_"),

                                    # Give the names of filters for that you want to store the trigger objects that have fired the respecitve trigger
                                    # filter paths for a given trigger can be found in https://cmsweb.cern.ch/confdb/
                                    # Example: for HLT_Mu45_eta2p1 the last trigger filter is hltL3fL1sMu16orMu25L1f0L2f10QL3Filtered45e2p1Q
                                    #          for HLT_Ele35_CaloIdVT_GsfTrkIdT_PFJet150_PFJet50: relevant filters are hltEle35CaloIdVTGsfTrkIdTGsfDphiFilter (last electron filter), hltEle35CaloIdVTGsfTrkIdTDiCentralPFJet50EleCleaned and hltEle35CaloIdVTGsfTrkIdTCentralPFJet150EleCleaned (for the two jets).
                                    #          The  filter hltEle35CaloIdVTGsfTrkIdTCentralPFJet150EleCleaned only included redundant objects that are already included in hltEle35CaloIdVTGsfTrkIdTCentralPFJet50EleCleaned.
                                    # for HLT_Ele45_CaloIdVT_GsfTrkIdT_PFJet200_PFJet50: relevant filters
                                    # are hltEle45CaloIdVTGsfTrkIdTGsfDphiFilter (last electron filter),
                                    # hltEle45CaloIdVTGsfTrkIdTDiCentralPFJet50EleCleaned
                                    triggerObjects_sources=(cms.vstring(
                                        # single jet trigger
                                        'hltSinglePFJet40',
                                        'hltSinglePFJet60',
                                        'hltSinglePFJet80',
                                        'hltSinglePFJet140',
                                        'hltSinglePFJet200',
                                        'hltSinglePFJet260',
                                        'hltSinglePFJet320',
                                        'hltSinglePFJet400',
                                        'hltSinglePFJet450',
                                        'hltSinglePFJet500',

                                        # single jet FWD triggers
                                        'hltSinglePFFwdJet40',
                                        'hltSinglePFFwdJet60',
                                        'hltSinglePFFwdJet80',
                                        'hltSinglePFFwdJet140',
                                        'hltSinglePFFwdJet200',
                                        'hltSinglePFFwdJet260',
                                        'hltSinglePFFwdJet320',
                                        'hltSinglePFFwdJet400',
                                        'hltSinglePFFwdJet450',
                                        'hltSinglePFFwdJet500',

                                        'hltDiPFJetAve40',
                                        'hltDiPFJetAve60',
                                        'hltDiPFJetAve80',
                                        'hltDiPFJetAve140',
                                        'hltDiPFJetAve200',
                                        'hltDiPFJetAve260',
                                        'hltDiPFJetAve320',
                                        'hltDiPFJetAve400',
                                        'hltDiPFJetAve450',
                                        'hltDiPFJetAve500',

                                        'hltDiPFJetAve15ForHFJEC',
                                        'hltDiPFJetAve25ForHFJEC',
                                        'hltDiPFJetAve35ForHFJEC',
                                        'hltDiPFJetAve60ForHFJEC',
                                        'hltDiPFJetAve80ForHFJEC',
                                        'hltDiPFJetAve100ForHFJEC',
                                        'hltDiPFJetAve160ForHFJEC',
                                        'hltDiPFJetAve220ForHFJEC',
                                        'hltDiPFJetAve300ForHFJEC',

                                        #Z'->ttbar 2017/2018
                                        'hltL3fL1sMu22Or25L1f0L2f10QL3Filtered50Q', #HLT_Mu50_v13 (merged with HLT_TkMu50_ in 2017/2018)
                                        'hltEle50CaloIdVTGsfTrkIdTCentralPFJet165EleCleaned', #HLT_Ele50_CaloIdVT_GsfTrkIdT_PFJet165_v18
                                        'hltEle115CaloIdVTGsfTrkIdTGsfDphiFilter',#HLT_Ele115_CaloIdVT_GsfTrkIdT_v14
                                        'hltEG200HEFilter',#HLT_Photon200_v13
                                        'hltPFHT450Jet30',#HLT_Ele50_IsoVVVL_PFHT450_v
                                        #Z'->ttbar 2016
                                        #'hltL3fL1sMu22Or25L1f0L2f10QL3Filtered50Q', #HLT_Mu50_v5
                                        'hltL3fL1sMu25f0TkFiltered50Q', #HLT_TkMu50_v3
                                        #'hltEle50CaloIdVTGsfTrkIdTCentralPFJet165EleCleaned', #HLT_Ele50_CaloIdVT_GsfTrkIdT_PFJet165_v7
                                        #'hltEle115CaloIdVTGsfTrkIdTGsfDphiFilter',#HLT_Ele115_CaloIdVT_GsfTrkIdT_v7
                                    ) if useData else cms.vstring()),

                                    #  'hltL3fL1sMu16orMu25L1f0L2f10QL3Filtered45e2p1Q',        # HLT_Mu45_eta2p1_v*
                                    #  'hltEle35CaloIdVTGsfTrkIdTGsfDphiFilter',                # HLT_Ele35_CaloIdVT_GsfTrkIdT_PFJet150_PFJet50_v* (electron)
                                    #  'hltEle35CaloIdVTGsfTrkIdTDiCentralPFJet50EleCleaned',   # HLT_Ele35_CaloIdVT_GsfTrkIdT_PFJet150_PFJet50_v* (jets)
                                    #  'hltEle45CaloIdVTGsfTrkIdTGsfDphiFilter',                # HLT_Ele45_CaloIdVT_GsfTrkIdT_PFJet200_PFJet50_v* (electron)
                                    #  'hltEle45CaloIdVTGsfTrkIdTDiCentralPFJet50EleCleaned',   # HLT_Ele45_CaloIdVT_GsfTrkIdT_PFJet200_PFJet50_v* (jets)
                                    #  'hltL3crIsoL1sMu25L1f0L2f10QL3f27QL3trkIsoFiltered0p09', # HLT_IsoMu27_v*
                                    #  'hltEle27WPLooseGsfTrackIsoFilter',                      # HLT_Ele27_eta2p1_WPLoose_Gsf_v*
                                    #),
                                    trigger_objects=cms.InputTag("selectedPatTrigger" if year == "2016v2" else "slimmedPatTrigger"),

                                    doEcalBadCalib=cms.bool(bad_ecal),
                                    ecalBadCalib_source=cms.InputTag("ecalBadCalibReducedMINIAODFilter"),

                                    doPrefire=cms.bool(do_prefire),
                                    prefire_source=cms.string(prefire_source),

                                    # *** gen stuff:
                                    doGenInfo=cms.bool(not useData),
                                    genparticle_source=cms.InputTag("prunedPrunedGenParticles"),
                                    stablegenparticle_source=cms.InputTag("packedGenParticlesForJetsNoNu"),
                                    # set to true if you want to store all gen particles, otherwise, only
                                    # prunedPrunedGenParticles are stored (see above)
                                    doAllGenParticles=cms.bool(False),
                                    doAllGenParticlesPythia8=cms.bool(False),
#                                    doPFJetConstituents=cms.uint32(0),
                                    doPFJetConstituents=cms.uint32(1),
                                    doGenJets=cms.bool(not useData),
                                    doGenJetConstituents=cms.uint32(0), #number of genjets with stored gen.constituents
                                    genjet_sources=cms.vstring(
                                       #"slimmedGenJets", "slimmedGenJetsAK8", "ca15GenJets"),
                                    "slimmedGenJets", "slimmedGenJetsAK8"),
                                    genjet_ptmin=cms.double(10.0),
                                    genjet_etamax=cms.double(5.0),

                                    doGenTopJets=cms.bool(not useData),
                                    gentopjet_sources=cms.VInputTag(
                                        cms.InputTag("ak8GenJetsSoftDrop")
                                    ),
                                    # gentopjet_sources =
                                    # cms.VInputTag(cms.InputTag("ak8GenJets"),cms.InputTag("ak8GenJetsSoftDrop")),
                                    # #this can be used to save N-subjettiness for ungroomed GenJets
                                    gentopjet_ptmin=cms.double(150.0),
                                    gentopjet_etamax=cms.double(5.0),
                                    gentopjet_tau1=cms.VInputTag(),
                                    gentopjet_tau2=cms.VInputTag(),
                                    gentopjet_tau3=cms.VInputTag(),
                                    # gentopjet_tau1 = cms.VInputTag(cms.InputTag("NjettinessAk8Gen","tau1"),cms.InputTag("NjettinessAk8SoftDropGen","tau1")), #this can be used to save N-subjettiness for GenJets
                                    # gentopjet_tau2 = cms.VInputTag(cms.InputTag("NjettinessAk8Gen","tau2"),cms.InputTag("NjettinessAk8SoftDropGen","tau2")), #this can be used to save N-subjettiness for GenJets
                                    # gentopjet_tau3 =
                                    # cms.VInputTag(cms.InputTag("NjettinessAk8Gen","tau3"),cms.InputTag("NjettinessAk8SoftDropGen","tau3")),
                                    # #this can be used to save N-subjettiness for GenJets

                                    doAllPFParticles=cms.bool(False),
                                    pf_collection_source=cms.InputTag("packedPFCandidates"),

                                    # *** HOTVR & XCone stuff
                                    doHOTVR=cms.bool(True),
                                    doXCone=cms.bool(True),
                                    HOTVR_sources=cms.VInputTag(
                                        cms.InputTag("hotvrPuppi")
                                    ),
                                    XCone_sources=cms.VInputTag(
                                        cms.InputTag("xconePuppi"),
                                        cms.InputTag("xconeCHS"),
                                    ),

                                    doGenHOTVR=cms.bool(not useData),
                                    doGenXCone=cms.bool(not useData),
                                    GenHOTVR_sources=cms.VInputTag(
                                        cms.InputTag("hotvrGen")
                                    ),
                                    GenXCone_sources=cms.VInputTag(
                                        cms.InputTag("genXCone23TopJets"),
                                        cms.InputTag("genXCone33TopJets"),
                                    ),
                                    doXCone_dijet=cms.bool(True), #XCone for dijet (JERC) studies, should be stored for QCD MC and JetHT DATA
                                    XCone_dijet_sources=cms.VInputTag(
                                        cms.InputTag("xconeCHS2jets04"),
                                        cms.InputTag("xconeCHS3jets04"),
                                        cms.InputTag("xconeCHS4jets04"),
                                        cms.InputTag("xconeCHS2jets08"),
                                        cms.InputTag("xconeCHS3jets08"),
                                        cms.InputTag("xconeCHS4jets08"),
                                    ),
                                    doGenXCone_dijet=cms.bool(not useData),
                                    GenXCone_dijet_sources=cms.VInputTag(
                                        cms.InputTag("genXCone2jets04"),
                                        cms.InputTag("genXCone3jets04"),
                                        cms.InputTag("genXCone4jets04"),
                                        cms.InputTag("genXCone2jets08"),
                                        cms.InputTag("genXCone3jets08"),
                                        cms.InputTag("genXCone4jets08"),
                                    )

    )

    process.content = cms.EDAnalyzer("EventContentAnalyzer")

    # Note: we run in uncsheduled mode, i.e. all modules are run as required;
    # just make sure that the electron IDs run before MyNtuple
    process.p = cms.Path(
        process.egmGsfElectronIDSequence *
        process.egmPhotonIDSequence * 
        process.MyNtuple
#        * process.content 
    )
    if do_prefire:
        process.p.insert(0, process.prefiringweight)

    if bad_ecal:
        process.p.insert(0, process.ecalBadCalibReducedMINIAODFilter)

    if do_bad_muon_charged_filters:
        process.p.insert(0, process.BadPFMuonFilter)
        process.p.insert(0, process.BadChargedCandidateFilter)

    if year == "2016v2" and (not useData):
        process.load("PhysicsTools.JetMCAlgos.HadronAndPartonSelector_cfi")
        process.selectedHadronsAndPartonsForGenJetsFlavourInfos.particles = "prunedGenParticles"
        task.add(process.selectedHadronsAndPartonsForGenJetsFlavourInfos)
        from PhysicsTools.JetMCAlgos.AK4GenJetFlavourInfos_cfi import ak4GenJetFlavourInfos
        process.slimmedGenJetsFlavourInfos = ak4GenJetFlavourInfos.clone(jets="slimmedGenJets")
        task.add(process.slimmedGenJetsFlavourInfos)

    process.p.associate(task)
    process.p.associate(process.patAlgosToolsTask)

    return process

