import FWCore.ParameterSet.Config as cms

from RecoParticleFlow.PFProducer.electronPFIsolationDeposits_cff import *
from RecoParticleFlow.PFProducer.electronPFIsolationValues_cff import *

def init_module_elPFMiniIsoValue(imod, R_max, R_min, KTfac):
    mod = cms.EDProducer('PFCandIsolatorFromDepositsMINIISO',
      deposits = cms.VPSet(
        cms.PSet(
          src = imod.deposits[0].src,
          weight = imod.deposits[0].weight,
          vetos = imod.deposits[0].vetos,
          skipDefaultVeto = imod.deposits[0].skipDefaultVeto,
          mode = imod.deposits[0].mode,
          PivotCoordinatesForEBEE = imod.deposits[0].PivotCoordinatesForEBEE,

          dRmax = cms.double(R_max),
          dRmin = cms.double(R_min),
          kTfac = cms.double(KTfac),
        )
      )
    )

    return mod

def load_elecPFMiniIso(proc, seq_name, algo, isoval_list, src,
                       src_charged_hadron='', src_neutral_hadron='', src_photon='', src_charged_pileup='',
                       dRmax=.20, dRmin=.05, kTfac=10.):

    # deposits
    iso_seq = cms.Sequence()

    if src_charged_hadron:
        setattr(proc, 'elPFMiniIsoDepositCH'+algo, elPFIsoDepositCharged.clone())
        getattr(proc, 'elPFMiniIsoDepositCH'+algo).src = cms.InputTag(src)
        getattr(proc, 'elPFMiniIsoDepositCH'+algo).ExtractorPSet.inputCandView = cms.InputTag(src_charged_hadron)
        iso_seq += getattr(proc, 'elPFMiniIsoDepositCH'+algo)

    if src_neutral_hadron:
        setattr(proc, 'elPFMiniIsoDepositNH'+algo, elPFIsoDepositNeutral.clone())
        getattr(proc, 'elPFMiniIsoDepositNH'+algo).src = cms.InputTag(src)
        getattr(proc, 'elPFMiniIsoDepositNH'+algo).ExtractorPSet.inputCandView = cms.InputTag(src_neutral_hadron)
        iso_seq += getattr(proc, 'elPFMiniIsoDepositNH'+algo)

    if src_photon:
        setattr(proc, 'elPFMiniIsoDepositPh'+algo, elPFIsoDepositGamma.clone())
        getattr(proc, 'elPFMiniIsoDepositPh'+algo).src = cms.InputTag(src)
        getattr(proc, 'elPFMiniIsoDepositPh'+algo).ExtractorPSet.inputCandView = cms.InputTag(src_photon)
        iso_seq += getattr(proc, 'elPFMiniIsoDepositPh'+algo)

    if src_charged_pileup:
        setattr(proc, 'elPFMiniIsoDepositPU'+algo, elPFIsoDepositPU.clone())
        getattr(proc, 'elPFMiniIsoDepositPU'+algo).src = cms.InputTag(src)
        getattr(proc, 'elPFMiniIsoDepositPU'+algo).ExtractorPSet.inputCandView = cms.InputTag(src_charged_pileup)
        iso_seq += getattr(proc, 'elPFMiniIsoDepositPU'+algo)

    # values
    iso_vals_seq = cms.Sequence()

    if src_charged_hadron:
        setattr(proc, 'elPFMiniIsoValueCH'+algo, init_module_elPFMiniIsoValue(elPFIsoValueCharged03PFId, R_max=dRmax, R_min=dRmin, KTfac=kTfac))
        getattr(proc, 'elPFMiniIsoValueCH'+algo).deposits[0].src = cms.InputTag('elPFMiniIsoDepositCH'+algo)
        iso_vals_seq += getattr(proc, 'elPFMiniIsoValueCH'+algo)
        isoval_list.append('elPFMiniIsoValueCH'+algo)

    if src_neutral_hadron:
        setattr(proc, 'elPFMiniIsoValueNH'+algo, init_module_elPFMiniIsoValue(elPFIsoValueNeutral03PFId, R_max=dRmax, R_min=dRmin, KTfac=kTfac))
        getattr(proc, 'elPFMiniIsoValueNH'+algo).deposits[0].src = cms.InputTag('elPFMiniIsoDepositNH'+algo)
        iso_vals_seq += getattr(proc, 'elPFMiniIsoValueNH'+algo)
        isoval_list.append('elPFMiniIsoValueNH'+algo)

    if src_photon:
        setattr(proc, 'elPFMiniIsoValuePh'+algo, init_module_elPFMiniIsoValue(elPFIsoValueGamma03PFId, R_max=dRmax, R_min=dRmin, KTfac=kTfac))
        getattr(proc, 'elPFMiniIsoValuePh'+algo).deposits[0].src = cms.InputTag('elPFMiniIsoDepositPh'+algo)
        iso_vals_seq += getattr(proc, 'elPFMiniIsoValuePh'+algo)
        isoval_list.append('elPFMiniIsoValuePh'+algo)

    if src_charged_pileup:
        setattr(proc, 'elPFMiniIsoValuePU'+algo, init_module_elPFMiniIsoValue(elPFIsoValuePU03PFId, R_max=dRmax, R_min=dRmin, KTfac=kTfac))
        getattr(proc, 'elPFMiniIsoValuePU'+algo).deposits[0].src = cms.InputTag('elPFMiniIsoDepositPU'+algo)
        iso_vals_seq += getattr(proc, 'elPFMiniIsoValuePU'+algo)
        isoval_list.append('elPFMiniIsoValuePU'+algo)

    iso_seq *= iso_vals_seq

    setattr(proc, seq_name, iso_seq)

    return
