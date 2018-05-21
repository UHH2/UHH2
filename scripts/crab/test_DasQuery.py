#!/usr/bin/env python

"""Simple test to make sure DasQuery moduel works"""

from DasQuery import autocomplete_Datasets

inputDatasets = ['/DYJetsToLL_M-50_HT-*to*_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_*/MINIAODSIM']
result = autocomplete_Datasets(inputDatasets)
print result
assert(len(result) == 11)

inputDatasets = ['/QCD_Pt-15to20_MuEnrichedPt5_TuneCUETP8M1_13TeV_pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-*/MINIAODSIM']
result = autocomplete_Datasets(inputDatasets)
print result
assert(len(result) == 1)

