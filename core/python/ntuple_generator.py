"""
Module to generate our whole ntuple-making chain.

Main function imported & used in other scripts - this file should not be used directly with cmsRun

e.g.:

>> import FWCore.ParameterSet.Config as cms
>> from UHH2.core.ntuple_generator import generate_process
>> process = generate_process(year="2016", useData=False)
"""

def generate_process(year, useData=True, isDebug=False, fatjet_ptmin=120.):
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

    import FWCore.ParameterSet.Config as cms

    year = str(year)  # sanitise string
    acceptable_years = ["2022"]
    if year not in acceptable_years:
        raise ValueError("year argument in generate_process() should be one of: %s. You provided: %s" % (acceptable_years, year))

    met_sources_GL = cms.vstring("slimmedMETs", "slimmedMETsPuppi")


    # define our Process object
    # eras are needed for correct b-tagging
    from Configuration.StandardSequences.Eras import eras
    if year == "2022":
        process = cms.Process("USER", eras.Run3)
    else:
        raise RuntimeError("Cannot setup process for this year, may need to add a new entry.")

    bTagDiscriminators = [
    ]

    ak4btagDiscriminators = [
    ]

    ak8btagDiscriminators = [
    ]

    # from RecoBTag.ONNXRuntime.pfParticleNet_cff import _pfParticleNetJetTagsAll as pfParticleNetJetTagsAll
    pfParticleNetJetTagsAll = [
    ]
    ak8btagDiscriminators += pfParticleNetJetTagsAll

    bTagInfos = [
        'pfImpactParameterTagInfos', 'pfSecondaryVertexTagInfos', 'pfInclusiveSecondaryVertexFinderTagInfos', 'softPFMuonsTagInfos', 'softPFElectronsTagInfos'
    ]

    task = cms.Task()

    process.load("FWCore.MessageService.MessageLogger_cfi")
    process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(250)
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
                                                oncePerEventMode=cms.untracked.bool(True),
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
    gc_filenames = '__FILE_NAMES__'

    import os
    gc_nickname = os.getenv('DATASETNICK')
    if gc_nickname is not None:
        #useData = not gc_nickname.startswith('MC_')
        process.source.fileNames = map(lambda s: s.strip(' "'), gc_filenames.split(','))
        process.source.skipEvents = int(os.getenv('SKIP_EVENTS'))
        process.maxEvents.input = int(os.getenv('MAX_EVENTS'))

    ###############################################
    # OUT
    from Configuration.EventContent.EventContent_cff import MINIAODSIMEventContent
    process.out = cms.OutputModule("PoolOutputModule",
                                   fileName=cms.untracked.string('miniaod.root'),
                                   outputCommands=MINIAODSIMEventContent.outputCommands
    )

    process.out.outputCommands.extend([
        'keep *_patJetsAk8CHS*_*_*',
        'keep *_patJetsAk8Puppi*_*_*',
        'keep *_patJetsCa15CHS*_*_*',
        #'keep *_NjettinessAk8CHS_*_*',
        #'keep *_NjettinessAk8Puppi_*_*',
        #'keep *_NjettinessCa15CHS_*_*',
        #'keep *_NjettinessCa15SoftDropCHS_*_*',
        #"keep *_patJetsCmsTopTagCHSPacked_*_*",
        #"keep *_patJetsCmsTopTagCHSSubjets_*_*",
        #"keep *_patJetsHepTopTagCHSPacked_*_*",
        #"keep *_patJetsHepTopTagCHSSubjets_*_*",
        #"keep *_patJetsAk8CHSJetsSoftDropPacked_*_*",
        #"keep *_patJetsAk8CHSJetsSoftDropSubjets_*_*",
        #"keep *_patJetsAk8PuppiJetsSoftDropPacked_*_*",
        #"keep *_patJetsAk8PuppiJetsSoftDropSubjets_*_*",
        #"keep *_patJetsCa15CHSJetsSoftDropPacked_*_*",
        #"keep *_patJetsCa15CHSJetsSoftDropSubjets_*_*",
        #"keep *_patJetsAk8CHSJetsPrunedPacked_*_*",
        #"keep *_patJetsAk8CHSJetsPrunedSubjets_*_*",
        #"keep *_prunedPrunedGenParticles_*_*",
        "keep *_egmGsfElectronIDs_*_*"
    ])

    ###############################################
    # RECO AND GEN SETUP
    process.load("Configuration.Geometry.GeometryRecoDB_cff")
    process.load("Configuration.StandardSequences.MagneticField_cff")

    # see https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideFrontierConditions
    # for latest global tags
    # But check with:
    # https://twiki.cern.ch/twiki/bin/viewauth/CMS/PdmVAnalysisSummaryTable
    # https://twiki.cern.ch/twiki/bin/view/CMS/PdmVRun2LegacyAnalysis
    process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

    # There should be 1 key:value entry per entry in acceptable_years, and each
    # should have a dictionary of "data" and "mc" with their respective global tags
    global_tags = {
        "2022": {
            "data": "124X_dataRun3_Prompt_v4",
            "mc": "124X_mcRun3_2022_realistic_v11",
        }
    }
    if set(global_tags.keys()) != set(acceptable_years):
        raise KeyError("Mismatch between acceptable_years and global_tags")

    process.GlobalTag.globaltag = global_tags[year]['data' if useData else 'mc']

    from RecoJets.Configuration.RecoPFJets_cff import fixedGridRhoFastjetAll
    process.fixedGridRhoFastjetAll = fixedGridRhoFastjetAll.clone(
        pfCandidatesTag='packedPFCandidates'
    )


    # ###############################################
    # # GEN PARTICLES
    # #
    # # The 13TeV samples mainly use Pythia8 for showering, which stores information in another way compared to Pythia6; in particular,
    # # many intermediate particles are stored such as top quarks or W bosons, which are not required for the analyses and make the code more complicated.
    # # Therefore, the 'prunedGenParticles' collection is pruned again; see UHH2/core/python/testgenparticles.py for a test for this pruning
    # # and more comments.
    # # Note that separate instances of GenParticlePruner should be avoided - each contributes to more RSS memory overhead
    # process.prunedPrunedGenParticles = cms.EDProducer("GenParticlePruner",
    #                                    src=cms.InputTag("prunedGenParticles"),
    #                                    select=cms.vstring(
    #                                        'drop *',
    #                                        'keep status == 3',
    #                                        'keep 20 <= status <= 30',
    #                                        'keep 11 <= abs(pdgId) <= 16 && numberOfMothers()==1 && abs(mother().pdgId()) >= 23 && abs(mother().pdgId()) <= 25',
    #                                        'keep 11 <= abs(pdgId) <= 16 && numberOfMothers()==1 && abs(mother().pdgId()) == 6',
    #                                        'keep 11 <= abs(pdgId) <= 16 && numberOfMothers()==1 && abs(mother().pdgId()) == 42',
    #                                        'drop 11 <= abs(pdgId) <= 16 && numberOfMothers() == 1 && abs(mother().pdgId())==6',
    #                                        'keep 11 <= abs(pdgId) <= 16 && numberOfMothers() == 1 && abs(mother().pdgId())==6 ' \
    #                                        '&& mother().numberOfDaughters() > 2 && abs(mother().daughter(0).pdgId()) != 24 ' \
    #                                        '&& abs(mother().daughter(1).pdgId()) != 24 && abs(mother().daughter(2).pdgId()) != 24'
    #                                    )
    # )
    # task.add(process.prunedPrunedGenParticles)

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
    from RecoJets.Configuration.RecoPFJets_cff import ak8PFJets
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

    ###############################################
    # PAT JETS and Gen Jets
    #
    # 'Patify' the jet collections defined above and also add the corresponding
    # genjets.

    # captitalize string; needed below to construct pat module names.
    def cap(s): return s[0].upper() + s[1:]

    # process.load('PhysicsTools.PatAlgos.slimming.unpackedTracksAndVertices_cfi')

    # common parameters for the addJetCollection function, see below.
    # common_btag_parameters = dict(
    #     #trackSource = cms.InputTag('unpackedTracksAndVertices'),
    #     pfCandidates=cms.InputTag('packedPFCandidates'),
    #     pvSource=cms.InputTag('offlineSlimmedPrimaryVertices'),
    #     svSource=cms.InputTag('slimmedSecondaryVertices'),
    #     muSource=cms.InputTag('slimmedMuons'),
    #     elSource=cms.InputTag('slimmedElectrons'),
    #     btagInfos=bTagInfos,
    #     btagDiscriminators=bTagDiscriminators
    # )

    process.packedGenParticlesForJetsNoNu = cms.EDFilter("CandPtrSelector",
        src=cms.InputTag("packedGenParticles"),
        cut=cms.string("abs(pdgId) != 12 && abs(pdgId) != 14 && abs(pdgId) != 16")
    )
    task.add(process.packedGenParticlesForJetsNoNu)


    # Create a list with only muons and photons that where radiated from muons.
    # This lists will later be used as input for jet clustering.
    # The resulting jets act as a good lepton definition on particle level when comparing to detektor level.
    # The same procedure is done for electrons.
    process.MuonPhotonGenParticles = cms.EDFilter("CandPtrSelector",
        src=cms.InputTag("packedGenParticles"),
        cut=cms.string(
            "(abs(pdgId) == 13) || (abs(pdgId) == 22 && numberOfMothers()==1 && abs(mother().pdgId()) == 13)"
        )
    )
    task.add(process.MuonPhotonGenParticles)

    process.ElectronPhotonGenParticles = cms.EDFilter("CandPtrSelector",
        src=cms.InputTag("packedGenParticles"),
        cut=cms.string(
            "(abs(pdgId) == 11) || (abs(pdgId) == 22 && numberOfMothers()==1 && abs(mother().pdgId()) == 11)"
        )
    )
    task.add(process.ElectronPhotonGenParticles)


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


    from PhysicsTools.PatAlgos.tools.jetTools import addJetCollection, updateJetCollection

    # Adapt primary vertex collection
    from PhysicsTools.PatAlgos.tools.pfTools import adaptPVs
    adaptPVs(process, pvCollection=cms.InputTag('offlineSlimmedPrimaryVertices'))


    ###############################################
    # JEC-specific jet collections
    #
    # for JEC purposes, cluster AK8 jets but with lower pt
    # (compared to higher threshold in miniAOD)
    ak8_label = "AK8PFPUPPI"
    ak8puppi_patname = 'patJets' + ak8_label
    print(f"Adding {ak8puppi_patname}")
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

    # Rekey daughters, i.e. point to the objects in packedPfCandidates
    # instead of in puppi (you then need to multiply constituents by puppiWeight)
    # Do this to save space in ntuple if storing jet constituents -
    # no duplicates across CHS & PUPPI
    process.rekeyPatJetsAK8PFPUPPI = cms.EDProducer("RekeyJets",
        jetSrc=cms.InputTag("patJetsAK8PFPUPPI"),
        candidateSrc=cms.InputTag("packedPFCandidates"),
        )
    task.add(process.rekeyPatJetsAK8PFPUPPI)

    ak8_label = "AK8PFCHS"
    ak8chs_patname = 'patJets' + ak8_label
    print(f"Adding {ak8chs_patname}")
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
        print(f"Renaming {current_name} to {new_name}")
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
#    rename_module(process, task, "updatedPatJetsTransientCorrectedSlimmedJetsPuppiNewDFTraining", "jetsAk4Puppi")
#    rename_module(process, task, "updatedPatJetsTransientCorrectedSlimmedJetsNewDFTraining", "jetsAk4CHS")
#    rename_module(process, task, "updatedPatJetsTransientCorrectedRekeyPatJetsAK8PFPUPPINewDFTraining", "jetsAk8Puppi")
#    rename_module(process, task, ak8chs_patname, "jetsAk8CHS")
    # TopJet collections
#    rename_module(process, task, "updatedPatJetsTransientCorrectedRekeyPackedPatJetsAk8PuppiJetsNewDFTraining", "jetsAk8PuppiSubstructure", update_userData=False)
#    rename_module(process, task, "updatedPatJetsTransientCorrectedPackedPatJetsAk8CHSJetsNewDFTraining", "jetsAk8CHSSubstructure", update_userData=False)
#    rename_module(process, task, "packedPatJetsAk8CHSJets", "jetsAk8CHSSubstructure", update_userData=False)  # don't update userData as JetSubstructurePacker



    ###############################################
    # LEPTON configuration
    #

    # collections for lepton PF-isolation deposits
    process.load('UHH2.core.pfCandidatesByType_cff')
    process.load('CommonTools.ParticleFlow.deltaBetaWeights_cff')

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

    ###############################################
    # MUONS
    #
    # mini-isolation
    mu_isovals = []

    from UHH2.core.muon_pfMiniIsolation_cff import load_muonPFMiniIso
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

    process.slimmedMuonsData = cms.EDProducer('PATMuonUserData',
                                              src=cms.InputTag('slimmedMuons'),
                                              vmaps_double=cms.vstring(mu_isovals),
                                              )
    task.add(process.slimmedMuonsData)


    #### Muons miniIso
#    process.isoForMu = cms.EDProducer("MuonIsoValueMapProducer",
#                              src = cms.InputTag("slimmedMuonsData"),
#                              relative = cms.bool(False),
#                              rho_MiniIso = cms.InputTag("fixedGridRhoFastjetAll"),
#                              EAFile_MiniIso = cms.FileInPath("PhysicsTools/NanoAOD/data/effAreaMuons_cone03_pfNeuHadronsAndPhotons_94X.txt"),
#                          )
#    task.add(process.isoForMu)

#    process.ptRatioRelForMu = cms.EDProducer("MuonJetVarProducer",
#                                             srcJet = cms.InputTag("jetsAk4CHS"),
#                                             srcLep = cms.InputTag("slimmedMuonsData"),
#                                             srcVtx = cms.InputTag("offlineSlimmedPrimaryVertices"),
#                                         )
#    task.add(process.ptRatioRelForMu)


#    process.slimmedMuonsWithUserData = cms.EDProducer("PATMuonUserDataEmbedder",
#                                              src = cms.InputTag("slimmedMuonsData"),
#                                              userFloats = cms.PSet(
#                                                  miniIsoChg = cms.InputTag("isoForMu:miniIsoChg"),
#                                                  miniIsoAll = cms.InputTag("isoForMu:miniIsoAll"),
#                                                  ptRatio = cms.InputTag("ptRatioRelForMu:ptRatio"),
#                                                  ptRel = cms.InputTag("ptRatioRelForMu:ptRel"),
#                                                  jetNDauChargedMVASel = cms.InputTag("ptRatioRelForMu:jetNDauChargedMVASel")
#                                              ),
#                                              userCands = cms.PSet(
#                                                  jetForLepJetVar = cms.InputTag("ptRatioRelForMu:jetForLepJetVar") # warning: Ptr is null if no match is found
#                                              ),
#                                          )
#    task.add(process.slimmedMuonsWithUserData)

    muonMVAweightFile=""#"UHH2/common/data/UL16preVFP/TMVA_BDTG_TOP_muon_2016.weights.xml"


    # process.muonMVATOP= cms.EDProducer("MuonBaseMVAValueMapProducer",
    #                                    src = cms.InputTag("slimmedMuonsWithUserData"),
    #                                    weightFile =  cms.FileInPath(muonMVAweightFile),
    #                                    name = cms.string("muonMVATOP"),
    #                                    isClassifier = cms.bool(True),
    #                                 variablesOrder = cms.vstring(["dxylog", "miniIsoCharged", "miniIsoNeutral", "pTRel", "sip3d", "segmentCompatibility", "ptRatio", "bTagDeepJetClosestJet", "pt", "trackMultClosestJet", "etaAbs", "dzlog", "relIso"]),
    #                                    variables = cms.PSet(
    #                                        dxylog = cms.string("log(abs(dB('PV2D')))"),
    #                                        miniIsoCharged = cms.string("userFloat('miniIsoChg')/pt"),
    #                                        miniIsoNeutral = cms.string("(userFloat('miniIsoAll')-userFloat('miniIsoChg'))/pt"),
    #                                        pTRel = cms.string("?userCand('jetForLepJetVar').isNonnull()?userFloat('ptRel'):0"),
    #                                        sip3d = cms.string("abs(dB('PV3D')/edB('PV3D'))"),
    #                                        segmentCompatibility = cms.string("segmentCompatibility"),
    #                                        ptRatio = cms.string("?userCand('jetForLepJetVar').isNonnull()?min(userFloat('ptRatio'),1.5):1.0/(1.0+(pfIsolationR04().sumChargedHadronPt + max(pfIsolationR04().sumNeutralHadronEt + pfIsolationR04().sumPhotonEt - pfIsolationR04().sumPUPt/2,0.0))/pt)"),
    #                                        bTagDeepJetClosestJet = cms.string("?userCand('jetForLepJetVar').isNonnull()?max(userCand('jetForLepJetVar').bDiscriminator('pfDeepFlavourJetTags:probbb')+userCand('jetForLepJetVar').bDiscriminator('pfDeepFlavourJetTags:probb')+userCand('jetForLepJetVar').bDiscriminator('pfDeepFlavourJetTags:problepb'),0.0):0.0"),
    #                                        pt = cms.string("pt"),
    #                                        trackMultClosestJet = cms.string("?userCand('jetForLepJetVar').isNonnull()?userFloat('jetNDauChargedMVASel'):0"),
    #                                        etaAbs = cms.string("abs(eta)"),
    #                                        dzlog = cms.string("log(abs(dB('PVDZ')))"),
    #                                        relIso = cms.string("(pfIsolationR03().sumChargedHadronPt + max(pfIsolationR03().sumNeutralHadronEt + pfIsolationR03().sumPhotonEt - pfIsolationR03().sumPUPt/2,0.0))/pt"),
    #                                    )
    #                                )
    #task.add(process.muonMVATOP)

#    process.slimmedMuonsUSER = cms.EDProducer("PATMuonUserDataEmbedder",
#                                              src = cms.InputTag("slimmedMuonsWithUserData"),
#                                              userFloats = cms.PSet(
#                                                  mvaTOP = cms.InputTag("muonMVATOP"),
#                                              ),
#                                          )
#    task.add(process.slimmedMuonsUSER)

    ###############################################
    # ELECTRONS and PHOTONS
    #



    # # era needs to be passed, convert from the "year" string
    # if(year == "2016v2"): egamma_era = "2016-Legacy"
    # elif(year == "2016v3"): egamma_era = "2016-Legacy"
    # elif(year == "2017v1"): egamma_era = "2017-Nov17ReReco"
    # elif(year == "2017v2"): egamma_era = "2017-Nov17ReReco"
    # elif(year == "2018"): egamma_era = "2018-Prompt"
    # elif(year == "UL16preVFP"): egamma_era = "2016preVFP-UL"
    # elif(year == "UL16postVFP"): egamma_era = "2016postVFP-UL"
    # elif(year == "UL17"): egamma_era = "2017-UL"
    # elif(year == "UL18"): egamma_era = "2018-UL"
    #
    # # setting runVID according to recommendations
    # if(year == "2016v2"): doRunVID=True
    # elif(year == "2016v3"): doRunVID=True
    # elif(year == "2017v1"): doRunVID=True
    # elif(year == "2017v2"): doRunVID=True
    # elif(year == "2018"): doRunVID=True
    # elif(year == "UL16preVFP"): doRunVID=True
    # elif(year == "UL16postVFP"): doRunVID=True
    # elif(year == "UL17"): doRunVID=True
    # elif(year == "UL18"): doRunVID=True
    #
    # # The following means that the energy corrections are calculated for each electron/photon and are thus accessible but they are not actually applied to the electron/photon written to the ntuple.
    # # In order to apply the energy corrections on analysis-level, one can use members of electron/photon class which store the calculated corrections (if they have been written to the electron/photon
    # # class, see NtupleWriterLeptons).
    # doRunEnergyCorrections=False
    # doApplyEnergyCorrections=False
    #
    # # this is the magic thing by the Egamma POG that does *everything*
    # from RecoEgamma.EgammaTools.EgammaPostRecoTools import setupEgammaPostRecoSeq
    # setupEgammaPostRecoSeq(process, era=egamma_era, runVID=doRunVID, runEnergyCorrections=doRunEnergyCorrections, applyEnergyCorrections=doApplyEnergyCorrections)

    el_isovals = []

    from UHH2.core.electron_pfMiniIsolation_cff import load_elecPFMiniIso
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

    # from RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_Summer16_80X_V1_cff import isoInputs as ele_iso_16
    # from RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_Fall17_94X_V2_cff import isoInputs as ele_iso_17
    # iso_input_era_dict = {
    #     "2016v2": ele_iso_16,
    #     "2016v3": ele_iso_16,
    #     "2017v1": ele_iso_17,
    #     "2017v2": ele_iso_17,
    #     "2018": ele_iso_17,
    #     "UL16preVFP": ele_iso_17,
    #     "UL16postVFP": ele_iso_17,
    #     "UL17": ele_iso_17,
    #     "UL18": ele_iso_17,
    # }
    #
    # # slimmedElectronsUSER ( = slimmedElectrons + USER variables)
    # process.slimmedElectronsData = cms.EDProducer('PATElectronUserData',
    #                                               src=cms.InputTag(
    #                                                   'slimmedElectrons'),
    #
    #                                               vmaps_double=cms.vstring(el_isovals),
    #                                               effAreas_file=cms.FileInPath(iso_input_era_dict[year].isoEffAreas)
    #                                               )
    # task.add(process.slimmedElectronsData)
    #
    # EAFile_MiniIso_file = "RecoEgamma/ElectronIdentification/data/Spring15/effAreaElectrons_cone03_pfNeuHadronsAndPhotons_25ns.txt"
    # EAFile_PFIso_file = "RecoEgamma/ElectronIdentification/data/Summer16/effAreaElectrons_cone03_pfNeuHadronsAndPhotons_80X.txt"
    # if(year=="UL16preVFP" or year=="UL16postVFP"):
    #         EAFile_MiniIso_file = "RecoEgamma/ElectronIdentification/data/Fall17/effAreaElectrons_cone03_pfNeuHadronsAndPhotons_94X.txt"
    #         EAFile_PFIso_file = "RecoEgamma/ElectronIdentification/data/Fall17/effAreaElectrons_cone03_pfNeuHadronsAndPhotons_94X.txt"
    #
    #
    # process.isoForEle = cms.EDProducer("EleIsoValueMapProducer",
    #                                    src = cms.InputTag("slimmedElectronsData"),
    #                                    relative = cms.bool(False),
    #                                    rho_MiniIso = cms.InputTag("fixedGridRhoFastjetAll"),
    #                                    rho_PFIso = cms.InputTag("fixedGridRhoFastjetAll"),
    #                                    EAFile_MiniIso = cms.FileInPath(EAFile_MiniIso_file),
    #                                    EAFile_PFIso = cms.FileInPath(EAFile_PFIso_file),
    #                                )
    # task.add(process.isoForEle)
    #
    # process.ptRatioRelForEle = cms.EDProducer("ElectronJetVarProducer",
    #                                   srcJet = cms.InputTag("jetsAk4CHS"),
    #                                   srcLep = cms.InputTag("slimmedElectronsData"),
    #                                   srcVtx = cms.InputTag("offlineSlimmedPrimaryVertices"),
    #                               )
    #
    # task.add(process.ptRatioRelForEle)
    #
    # from RecoEgamma.ElectronIdentification.Identification.mvaElectronID_Fall17_noIso_V2_cff import mvaEleID_Fall17_noIso_V2_producer_config
    # process.electronMVAValueMapProducerv2 = cms.EDProducer('ElectronMVAValueMapProducer',
    #                                          src = cms.InputTag('slimmedElectronsData'),
    #                                          mvaConfigurations = cms.VPSet(mvaEleID_Fall17_noIso_V2_producer_config)
    #                                          )
    # task.add(process.electronMVAValueMapProducerv2)
    #
    # process.slimmedElectronsWithUserData = cms.EDProducer("PATElectronUserDataEmbedder",
    #                                                       src = cms.InputTag("slimmedElectronsData"),
    #                                                       userFloats = cms.PSet(
    #                                                           mvaFall17V2noIso = cms.InputTag("electronMVAValueMapProducerv2:ElectronMVAEstimatorRun2Fall17NoIsoV2Values"),
    #                                                           miniIsoChg = cms.InputTag("isoForEle:miniIsoChg"),
    #                                                           miniIsoAll = cms.InputTag("isoForEle:miniIsoAll"),
    #                                                           PFIsoChg = cms.InputTag("isoForEle:PFIsoChg"),
    #                                                           PFIsoAll = cms.InputTag("isoForEle:PFIsoAll"),
    #                                                           PFIsoAll04 = cms.InputTag("isoForEle:PFIsoAll04"),
    #                                                           ptRatio = cms.InputTag("ptRatioRelForEle:ptRatio"),
    #                                                           ptRel = cms.InputTag("ptRatioRelForEle:ptRel"),
    #                                                           jetNDauChargedMVASel = cms.InputTag("ptRatioRelForEle:jetNDauChargedMVASel")
    #                                                       ),
    #                                                       userCands = cms.PSet(
    #                                                           jetForLepJetVar = cms.InputTag("ptRatioRelForEle:jetForLepJetVar") # warning: Ptr is null if no match is found
    #                                                       )
    #                                                   )
    # task.add(process.slimmedElectronsWithUserData)
    #
    # electronMVAweightFile="UHH2/common/data/UL16preVFP/TMVA_BDTG_TOP_elec_2016.weights.xml"
    # if (year=="UL17"):  electronMVAweightFile="UHH2/common/data/UL17/TMVA_BDTG_TOP_elec_2017.weights.xml"
    # elif(year=="UL18"): electronMVAweightFile="UHH2/common/data/UL18/TMVA_BDTG_TOP_elec_2018.weights.xml"
    #
    # process.electronMVATOP = cms.EDProducer("EleBaseMVAValueMapProducer",
    #                                         src = cms.InputTag("slimmedElectronsWithUserData"),
    #                                         weightFile =  cms.FileInPath(electronMVAweightFile),
    #                                         name = cms.string("electronMVATOP"),
    #                                         isClassifier = cms.bool(True),
    #                                         variablesOrder = cms.vstring(["dxylog", "miniIsoCharged", "miniIsoNeutral", "pTRel", "sip3d", "mvaIdFall17v2noIso", "ptRatio", "bTagDeepJetClosestJet", "pt", "trackMultClosestJet", "etaAbs", "dzlog", "relIso"]),
    #                                         variables = cms.PSet(
    #                                             dxylog = cms.string("log(abs(dB('PV2D')))"),
    #                                             miniIsoCharged = cms.string("userFloat('miniIsoChg')/pt"),
    #                                             miniIsoNeutral = cms.string("(userFloat('miniIsoAll')-userFloat('miniIsoChg'))/pt"),
    #                                             pTRel = cms.string("?userCand('jetForLepJetVar').isNonnull()?userFloat('ptRel'):0"),
    #                                             sip3d = cms.string("abs(dB('PV3D')/edB('PV3D'))"),
    #                                             mvaIdFall17v2noIso = cms.string("userFloat('mvaFall17V2noIso')"),
    #                                             ptRatio = cms.string("?userCand('jetForLepJetVar').isNonnull()?min(userFloat('ptRatio'),1.5):1.0/(1.0+userFloat('PFIsoAll04')/pt)"),
    #                                             bTagDeepJetClosestJet = cms.string("?userCand('jetForLepJetVar').isNonnull()?max(userCand('jetForLepJetVar').bDiscriminator('pfDeepFlavourJetTags:probbb')+userCand('jetForLepJetVar').bDiscriminator('pfDeepFlavourJetTags:probb')+userCand('jetForLepJetVar').bDiscriminator('pfDeepFlavourJetTags:problepb'),0.0):0.0"),
    #                                             pt = cms.string("pt/userFloat('ecalTrkEnergyPostCorr')*userFloat('ecalTrkEnergyPreCorr')"),
    #                                             trackMultClosestJet = cms.string("?userCand('jetForLepJetVar').isNonnull()?userFloat('jetNDauChargedMVASel'):0"),
    #                                             etaAbs = cms.string("abs(eta)"),
    #                                             dzlog = cms.string("log(abs(dB('PVDZ')))"),
    #                                             relIso = cms.string("userFloat('PFIsoAll')/pt"),
    #                                         )
    #                                     )
    # task.add(process.electronMVATOP)
    #
    # process.slimmedElectronsUSER = cms.EDProducer("PATElectronUserDataEmbedder",
    #                                           src = cms.InputTag("slimmedElectronsWithUserData"),
    #                                           userFloats = cms.PSet(
    #                                               mvaTOP = cms.InputTag("electronMVATOP"),
    #                                           ),
    #                                       )
    # task.add(process.slimmedElectronsUSER)



    #rename_module(process, task, "slimmedPhotons", "slimmedPhotonsUSER", False)

    process.MyNtuple = cms.EDFilter('NtupleWriter',
                                    # AnalysisModule = cms.PSet(
                                    #    name = cms.string("NoopAnalysisModule"),
                                    #    library = cms.string("libUHH2examples.so"),
                                    #    # Note: all other settings of type string are passed to the module, e.g.:
                                    #    TestKey = cms.string("TestValue")
                                    #),
                                    fileName=cms.string("Ntuple.root"),
                                    compressionAlgorithm=cms.string('LZMA'),
                                    compressionLevel=cms.int32(9),  # LZMA9 is the highest compressions, for a slight increase in CPU/RAM usage
                                    year=cms.string(year),
                                    doPV=cms.bool(True),
                                    pv_sources=cms.vstring("offlineSlimmedPrimaryVertices"),
                                    doRho=cms.untracked.bool(True),
                                    rho_source=cms.InputTag("fixedGridRhoFastjetAll"),

                                    save_lepton_keys=cms.bool(False),

                                    doElectrons=cms.bool(True),
                                    electron_source=cms.InputTag("slimmedElectrons"),
                                    electron_IDtags=cms.vstring(),

                                    doMuons=cms.bool(True),
                                    muon_sources=cms.vstring("slimmedMuons"),

                                    doTaus=cms.bool(False),
                                    tau_sources=cms.vstring("slimmedTaus"),
                                    tau_ptmin=cms.double(0.0),
                                    tau_etamax=cms.double(999.0),

                                    doPhotons=cms.bool(True),
                                    save_photon_keys=cms.bool(True),
                                    photon_sources = cms.InputTag("slimmedPhotons"),
                                    photon_IDtags=cms.vstring(),

                                    doJets=cms.bool(True),
                                    jet_sources=cms.vstring(
                                        "slimmedJets",
                                        #"jetsAk4CHS",
                                        #"jetsAk4Puppi",
                                        #"jetsAk8CHS",
                                        #"jetsAk8Puppi",
					"slimmedJetsPuppi",
					"slimmedJetsAK8",
                                        ),
                                    jet_ptmin=cms.double(10.0),
                                    jet_etamax=cms.double(999.0),

                                    #store PF constituents for jet_sources: doPFJetConstituentsNjets and doPFJetConstituentsMinJetPt are combined with OR
                                    doPFJetConstituentsNjets=cms.uint32(0),#store constituents for N leading jets, where N is parameter
                                    doPFJetConstituentsMinJetPt=cms.double(-1),#store constituence for all jets with pt above threshold, set to negative value if not used

                                    doMET=cms.bool(True),
                                    met_sources=met_sources_GL,

                                    doGenMET=cms.bool(not useData),
                                    genmet_sources=cms.vstring("slimmedMETs"),

                                    doTopJets=cms.bool(False),
                                    # NB fatjet_ptmin arg used for clustering should
                                    # be smaller than topjet_ptmin to account for JECs
                                    topjet_ptmin=cms.double(150.0),
                                    topjet_etamax=cms.double(5.0),
                                    #store PF constituents for TopJets: doPFJetConstituentsNjets and doPFJetConstituentsMinJetPt are combined with OR
                                    doPFTopJetConstituentsNjets=cms.uint32(0),#store constituents for N leading topjets, where N is parameter
                                    doPFTopJetConstituentsMinJetPt=cms.double(-1),#store constituence for all topjets with pt above threshold, set to negative value if not used

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

                                            # If you want to store the MVA Higgs tagger discriminator,
                                            # specify the jet collection from which to pick it up and the tagger name
                                            # currently the discriminator is trained on ungroomed jets, so
                                            # the discriminator has to be taken from ungroomed jets
                                            higgstag_source=cms.string("patJetsAk8CHSJetsFat"),
                                            higgstag_name=cms.string("pfBoostedDoubleSecondaryVertexAK8BJetTags"),
                                            higgstaginfo_source=cms.string("pfBoostedDoubleSVTagInfosCHS"),

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

                                            higgstag_source=cms.string("patJetsAk8PuppiJetsFat"),
                                            higgstag_name=cms.string("pfBoostedDoubleSecondaryVertexAK8BJetTags"),
                                            higgstaginfo_source=cms.string("pfBoostedDoubleSVTagInfosPuppi"),  # FIXME Does this need replacing?

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
                                    metfilter_bits=cms.InputTag("TriggerResults", "", "PAT"),
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
                                        'hltDiPFJetAve500',

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

                                    # Extra trigger bits to store
                                    # Each gets stored as "Extra_<name>"
                                    # In theory one could also store the EcalBadCalib bit here as well,
                                    # but that might screw up existing workflows,
                                    # and removing the branch might screw up existing trees
                                    extra_trigger_bits=cms.VInputTag(),

                                    #For 2017 data with prefiring issue it might be usefull to store L1 seeds
                                    doL1TriggerObjects=cms.bool(False),
                                    l1GtSrc = cms.InputTag("gtStage2Digis"),
                                    l1EGSrc = cms.InputTag("caloStage2Digis:EGamma"),
                                    l1JetSrc = cms.InputTag("caloStage2Digis:Jet"),
                                    l1MuonSrc = cms.InputTag("gmtStage2Digis:Muon"),
                                    l1EtSumSrc = cms.InputTag("caloStage2Digis:EtSum"),
                                    l1TauSrc = cms.InputTag("caloStage2Digis:Tau"),

                                    doEcalBadCalib=cms.bool(False),
                                    ecalBadCalib_source=cms.InputTag("ecalBadCalibReducedMINIAODFilter"),

                                    doPrefire=cms.bool(False),
                                    #prefire_source=cms.string(prefire_source),

                                    # *** gen stuff:
                                    doGenInfo=cms.bool(not useData),  # Master switch for geninfo, genparticles, etc

                                    # Source to store (pruned) GenParticles from the genparticle history
                                    # By default we try to store the Matrix Element genparticles only
                                    # Note that this doesn't work for Herwig generators due to their
                                    # weird status codes
                                    genparticle_source=cms.InputTag("prunedPrunedGenParticles"),

                                    # Flag & source to store stable (i.e. packed) GenParticles
                                    # Useful if you want to store final-state genparticles in the event
                                    # If you only want GenJet constituents,
                                    # you should use the appropriate options further down
                                    doStableGenParticles=cms.bool(False),
                                    stablegenparticle_source=cms.InputTag("packedGenParticlesForJetsNoNu"),

                                    doGenJets=cms.bool(not useData),
                                    #store GEN constituents for genjet_sources: doGenJetConstituentsNjets and doGenJetConstituentsMinJetPt are combined with OR
                                    doGenJetConstituentsNjets=cms.uint32(0),#store constituents for N leading genjets, where N is parameter
                                    doGenJetConstituentsMinJetPt=cms.double(-1),#store constituence for all genjets with pt above threshold, set to negative value if not used
                                    genjet_sources=cms.vstring(
                                        # "muonGenJets", "electronGenJets",
                                        "slimmedGenJets", "slimmedGenJetsAK8"),
                                    genjet_ptmin=cms.double(10.0),
                                    genjet_etamax=cms.double(5.0),

                                    doGenTopJets=cms.bool(False),
                                    #store GenTopJet constituents: doGenTopJetConstituentsNjets and doGenTopJetConstituentsMinJetPt are combined with OR
                                    doGenTopJetConstituentsNjets=cms.uint32(0),#store constituents for N leading genjets, where N is parameter
                                    doGenTopJetConstituentsMinJetPt=cms.double(-1),#store constituence for all genjets with pt above threshold, set to negative value if not used
                                    gentopjet_ptmin=cms.double(150.0),
                                    gentopjet_etamax=cms.double(5.0),
                                    GenTopJets=cms.VPSet(
                                        cms.PSet(
                                            # gentopjet_source can be groomed or ungroomed.
                                            # It determines the main kinematics & constituent properties
                                            # of the GenTopJet, as well as the output collection name
                                            # If groomed (like here), the FastjetJetProducer module should have
                                            # `writeCompound=False`, otherwise the fatjets will have
                                            # daughters that are subjets, which will ruin the constituent
                                            # calculations e.g. energy fractions, # daughters.
                                            gentopjet_source=cms.string("genjetsAk8SubstructureSoftDrop"),

                                            # If you specify a source here, it will assume its
                                            # daughters are the corresponding subjets for each jet
                                            # in gentopjet_source and store them as such.
                                            # Thus you should set `writeCompound=True`
                                            # in your FastjetJetProducer
                                            subjet_source=cms.string("ak8GenJetsSoftDrop"),

                                            # substructure_variables_source should be the same
                                            # source as used in the njettiness_source & ecf_beta*_sources
                                            substructure_variables_source=cms.string("ak8GenJetsSoftDropforsub"),
                                            # Njettiness, internally will look for the various tau*
                                            njettiness_source=cms.string("NjettinessAk8SoftDropGen"),
                                            # Energy correlation functions, for beta=1 and beta=2
                                            ecf_beta1_source=cms.string("ECFNbeta1Ak8SoftDropGen"),
                                            ecf_beta2_source=cms.string("ECFNbeta2Ak8SoftDropGen")
                                        ),
                                        cms.PSet(
                                            # This is ungroomed AK8 GenJets, so no subjets,
                                            # but we do want Njettiness.
                                            gentopjet_source=cms.string("genjetsAk8Substructure"),
                                            subjet_source=cms.string(""),
                                            substructure_variables_source=cms.string("ak8GenJetsFat"),
                                            njettiness_source=cms.string("NjettinessAk8Gen"),
                                            ecf_beta1_source=cms.string(""),
                                            ecf_beta2_source=cms.string(""),
                                        ),
                                    ),

                                    doAllPFParticles=cms.bool(False),
                                    pf_collection_source=cms.InputTag("packedPFCandidates"),

#                                     # *** HOTVR & XCone stuff
#                                     doXCone=cms.bool(False),
#                                     #store PF constituents for XCone_sources: doPFJetConstituentsNjets and doPFJetConstituentsMinJetPt are combined with OR
#                                     doPFxconeJetConstituentsNjets=cms.uint32(0),#store constituents for N leading topjets, where N is parameter
#                                     doPFxconeJetConstituentsMinJetPt=cms.double(-1),#store constituence for all topjets with pt above threshold, set to negative value if not used
#                                     XCone_sources=cms.VInputTag(
#                                         cms.InputTag("xconePuppi"),
#                                         cms.InputTag("xconeCHS"),
#                                     ),
#                                     doHOTVR=cms.bool(False),
#                                     #store PF constituents for HOTVR_sources: doPFJetConstituentsNjets and doPFJetConstituentsMinJetPt are combined with OR
#                                     doPFhotvrJetConstituentsNjets=cms.uint32(0),#store constituents for N leading topjets, where N is parameter
#                                     doPFhotvrJetConstituentsMinJetPt=cms.double(-1),#store constituence for all topjets with pt above threshold, set to negative value if not used
#                                     HOTVR_sources=cms.VInputTag(
#                                         cms.InputTag("hotvrPuppi")
#                                     ),

#                                     doGenHOTVR=cms.bool(False),
#                                     doGenhotvrJetConstituentsNjets=cms.uint32(0),#store constituents for N leading genjets, where N is parameter
#                                     doGenhotvrJetConstituentsMinJetPt=cms.double(-1),#store constituence for all genjets with pt above threshold, set to negative value if not used

#                                     doGenXCone=cms.bool(False),
#                                     doGenxconeJetConstituentsNjets=cms.uint32(0),#store constituents for N leading genjets, where N is parameter
#                                     doGenxconeJetConstituentsMinJetPt=cms.double(-1),#store constituence for all genjets with pt above threshold, set to negative value if not used

#                                     GenHOTVR_sources=cms.VInputTag(
#                                         cms.InputTag("hotvrGen")
#                                     ),
#                                     GenXCone_sources=cms.VInputTag(
# #                                        cms.InputTag("genXCone23TopJets"),
#                                         cms.InputTag("genXCone33TopJets"),
#                                     ),
#                                     doXCone_dijet=cms.bool(False), #XCone for dijet (JERC) studies, should be stored for QCD MC and JetHT DATA
#                                     #store PF constituents for XCone_dijet_sources: doPFJetConstituentsNjets and doPFJetConstituentsMinJetPt are combined with OR
#                                     doPFxconeDijetJetConstituentsNjets=cms.uint32(0),#store constituents for N leading topjets, where N is parameter
#                                     doPFxconeDijetJetConstituentsMinJetPt=cms.double(-1),#store constituence for all topjets with pt above threshold, set to negative value if not used

#                                     XCone_dijet_sources=cms.VInputTag(
#                                         cms.InputTag("xconeCHS2jets04"),
#                                         cms.InputTag("xconeCHS3jets04"),
#                                         cms.InputTag("xconeCHS4jets04"),
#                                         cms.InputTag("xconeCHS2jets08"),
#                                         cms.InputTag("xconeCHS3jets08"),
#                                         cms.InputTag("xconeCHS4jets08"),
#                                         cms.InputTag("xconePUPPI2jets04"),
#                                         cms.InputTag("xconePUPPI3jets04"),
#                                         cms.InputTag("xconePUPPI4jets04"),
#                                         cms.InputTag("xconePUPPI2jets08"),
#                                         cms.InputTag("xconePUPPI3jets08"),
#                                         cms.InputTag("xconePUPPI4jets08"),

#                                     ),
# #                                    doGenXCone_dijet=cms.bool(not useData),
#                                     doGenXCone_dijet=cms.bool(False),
#                                     #store GEN constituents for GenXCone_dijet_sources: doGenJetConstituentsNjets and doGenJetConstituentsMinJetPt are combined with OR
#                                     doGenxconeDijetJetConstituentsNjets=cms.uint32(0),#store constituents for N leading topjets, where N is parameter
#                                     doGenxconeDijetJetConstituentsMinJetPt=cms.double(-1),#store constituence for all topjets with pt above threshold, set to negative value if not
#                                     GenXCone_dijet_sources=cms.VInputTag(
#                                         cms.InputTag("genXCone2jets04"),
#                                         cms.InputTag("genXCone3jets04"),
#                                         cms.InputTag("genXCone4jets04"),
#                                         cms.InputTag("genXCone2jets08"),
#                                         cms.InputTag("genXCone3jets08"),
#                                         cms.InputTag("genXCone4jets08"),
#                                     )

    )

    process.content = cms.EDAnalyzer("EventContentAnalyzer")

    # Note: we run in uncsheduled mode, i.e. all modules are run as required;
    # just make sure that the electron IDs run before MyNtuple
    process.p = cms.Path(
        #process.egammaPostRecoSeq *
        process.MyNtuple
#        * process.content
    )
    #if do_prefire:
    #    process.p.insert(0, process.prefiringweight)

    #if bad_ecal:
    #    process.p.insert(0, process.ecalBadCalibReducedMINIAODFilter)

    #if do_bad_muon_charged_filters:
    #    process.p.insert(0, process.BadPFMuonFilter)
        # process.p.insert(0, process.BadChargedCandidateFilter)

    process.p.associate(task)
    process.p.associate(process.patAlgosToolsTask)

    return process
