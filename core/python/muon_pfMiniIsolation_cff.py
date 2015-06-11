import FWCore.ParameterSet.Config as cms

from RecoMuon.MuonIsolation.muonPFIsolationDeposits_cff import *
from RecoMuon.MuonIsolation.muonPFIsolationValues_cff import *

def init_module_muPFMiniIsoValue(imod, R_max, R_min, KTfac):
    mod = cms.EDProducer('CandIsolatorFromDepositsMINIISO',
      deposits = cms.VPSet(
        cms.PSet(
          src = imod.deposits[0].src,
          weight = imod.deposits[0].weight,
          vetos = imod.deposits[0].vetos,
          skipDefaultVeto = imod.deposits[0].skipDefaultVeto,
          mode = imod.deposits[0].mode,

          dRmax = cms.double(R_max),
          dRmin = cms.double(R_min),
          kTfac = cms.double(KTfac),
        )
      )
    )

    return mod

def load_muonPFMiniIso(proc, seq_name, algo, isoval_list, src,
                       src_charged_hadron='', src_neutral_hadron='', src_photon='', src_charged_pileup='',
                       dRmax=.20, dRmin=.05, kTfac=10.):
    # deposits
    iso_seq = cms.Sequence()

    if src_charged_hadron:
        setattr(proc, 'muPFMiniIsoDepositCH'+algo, muPFIsoDepositCharged.clone())
        getattr(proc, 'muPFMiniIsoDepositCH'+algo).src = cms.InputTag(src)
        getattr(proc, 'muPFMiniIsoDepositCH'+algo).ExtractorPSet.inputCandView = cms.InputTag(src_charged_hadron)
        iso_seq += getattr(proc, 'muPFMiniIsoDepositCH'+algo)

    if src_neutral_hadron:
        setattr(proc, 'muPFMiniIsoDepositNH'+algo, muPFIsoDepositNeutral.clone())
        getattr(proc, 'muPFMiniIsoDepositNH'+algo).src = cms.InputTag(src)
        getattr(proc, 'muPFMiniIsoDepositNH'+algo).ExtractorPSet.inputCandView = cms.InputTag(src_neutral_hadron)
        iso_seq += getattr(proc, 'muPFMiniIsoDepositNH'+algo)

    if src_photon:
        setattr(proc, 'muPFMiniIsoDepositPh'+algo, muPFIsoDepositGamma.clone())
        getattr(proc, 'muPFMiniIsoDepositPh'+algo).src = cms.InputTag(src)
        getattr(proc, 'muPFMiniIsoDepositPh'+algo).ExtractorPSet.inputCandView = cms.InputTag(src_photon)
        iso_seq += getattr(proc, 'muPFMiniIsoDepositPh'+algo)

    if src_charged_pileup:
        setattr(proc, 'muPFMiniIsoDepositPU'+algo, muPFIsoDepositPU.clone())
        getattr(proc, 'muPFMiniIsoDepositPU'+algo).src = cms.InputTag(src)
        getattr(proc, 'muPFMiniIsoDepositPU'+algo).ExtractorPSet.inputCandView = cms.InputTag(src_charged_pileup)
        iso_seq += getattr(proc, 'muPFMiniIsoDepositPU'+algo)

    # values
    iso_vals_seq = cms.Sequence()

    if src_charged_hadron:
        setattr(proc, 'muPFMiniIsoValueCH'+algo, init_module_muPFMiniIsoValue(muPFIsoValueCharged03, R_max=dRmax, R_min=dRmin, KTfac=kTfac))
        getattr(proc, 'muPFMiniIsoValueCH'+algo).deposits[0].src = cms.InputTag('muPFMiniIsoDepositCH'+algo)
        iso_vals_seq += getattr(proc, 'muPFMiniIsoValueCH'+algo)
        isoval_list.append('muPFMiniIsoValueCH'+algo)

    if src_neutral_hadron:
        setattr(proc, 'muPFMiniIsoValueNH'+algo, init_module_muPFMiniIsoValue(muPFIsoValueNeutral03, R_max=dRmax, R_min=dRmin, KTfac=kTfac))
        getattr(proc, 'muPFMiniIsoValueNH'+algo).deposits[0].src = cms.InputTag('muPFMiniIsoDepositNH'+algo)
        iso_vals_seq += getattr(proc, 'muPFMiniIsoValueNH'+algo)
        isoval_list.append('muPFMiniIsoValueNH'+algo)

    if src_photon:
        setattr(proc, 'muPFMiniIsoValuePh'+algo, init_module_muPFMiniIsoValue(muPFIsoValueGamma03, R_max=dRmax, R_min=dRmin, KTfac=kTfac))
        getattr(proc, 'muPFMiniIsoValuePh'+algo).deposits[0].src = cms.InputTag('muPFMiniIsoDepositPh'+algo)
        iso_vals_seq += getattr(proc, 'muPFMiniIsoValuePh'+algo)
        isoval_list.append('muPFMiniIsoValuePh'+algo)

    if src_charged_pileup:
        setattr(proc, 'muPFMiniIsoValuePU'+algo, init_module_muPFMiniIsoValue(muPFIsoValuePU03, R_max=dRmax, R_min=dRmin, KTfac=kTfac))
        getattr(proc, 'muPFMiniIsoValuePU'+algo).deposits[0].src = cms.InputTag('muPFMiniIsoDepositPU'+algo)
        iso_vals_seq += getattr(proc, 'muPFMiniIsoValuePU'+algo)
        isoval_list.append('muPFMiniIsoValuePU'+algo)

    iso_seq *= iso_vals_seq

    setattr(proc, seq_name, iso_seq)

    return
