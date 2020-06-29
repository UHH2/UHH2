#!/usr/bin/env python


"""
Script to create lumilist of lumisections for MC or data dataset

It is designed to only work on *one* sample (& its ext, if it exists),
not multiple:

e.g.:
    OK:     /QCD_Pt_300to470_TuneCP5_13TeV_pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic*/MINIAODSIM
    Not OK: /QCD_Pt_*to*_TuneCP5_13TeV_pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic*/MINIAODSIM

Requires you to have a valid voms proxy, as it calls DAS.
"""


from __future__ import print_function

import sys,os
import argparse
import FWCore.PythonUtilities.LumiList as LumiList
from Utilities.General.cmssw_das_client import get_data
sys.path.append(os.environ["CMSSW_BASE"]+"/src/UHH2/scripts/crab")
from DasQuery import autocomplete_Datasets,check_voms


def get_mc_lumi_list(inputDataset="/QCD_Pt_300to470_TuneCP5_13TeV_pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic*/MINIAODSIM"):
    """Get the LumiList object(s) for dataset(s) matching `inputDataset`

    inputDataset:
        if a str, will ask DAS to autocomplate (can contain wildcards)
        if a list/tuple/set[str], will iterate over each entry in the list, without asking DAS to autocomplete.
        This is because the user might have cached the dataset names before calling this function, and we don't want to call DAS more than necessary.

    returns: a dict with an entry for each dataset user inputs with das string as key and LumiList as value

    raises RuntimeError if no valid voms proxy
    raises TypeError if inputDataset incorrect type
    """
    if not check_voms():
        raise RuntimeError("Missing voms proxy")

    if isinstance(inputDataset, str):
        inputDatasets = autocomplete_Datasets([inputDataset])
    elif not isinstance(inputDataset, (list, set, tuple)):
        raise TypeError('get_mc_lumi_list: `inputDataset` expects str or list/tuple/set[str]')

    result = {}
    for dataset in inputDatasets:
        print(dataset)
        json_dict = get_data(host='https://cmsweb.cern.ch', query="run lumi file dataset="+dataset, idx=0, limit=0, threshold=300)
        lumi_list = LumiList.LumiList()
        try:
            n_files = len(json_dict['data'])
            printout = round(n_files / 10)
            for i, file_info in enumerate(json_dict['data']):
                if (i>n_files):
                    break
                if i % printout == 0:
                    print("{}% done...".format(100 * i / n_files))
                ls = file_info['lumi'][0]['number']
                run = file_info['run'][0]['run_number']
                lumi_list += LumiList.LumiList(runsAndLumis={run: ls})
        except Exception as e:
            print('Did not find lumis for', dataset)
            print(e)
        result.update({dataset:lumi_list})
    return result


def write_lumi_list(inputDataset="/QCD_Pt_1000to1400_TuneCP5_13TeV_pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14*/MINIAODSIM", filename="test.json"):
    """Get lumilist for datasets matching `inputDataset`, and write result to `filename`

    If another matching dataset, writes to `filename` but with an extra '_ext' before the file extension.

    raises ValueError if inputDataset matches more than 2 samples (we allow 2 incase we have nominal+ext sample),
    or matches 0 samples
    """
    inputDatasets = autocomplete_Datasets([inputDataset])
    if len(inputDatasets) > 2:
        for d in inputDatasets:
            print(d)
        raise ValueError("The given inputDataset DAS string corresponds to more than two samples. This is a bit unusual. "
                         "Since this script can only handle up to 2 datasets, please use a more specific dataset pattern.")
    elif len(inputDatasets) == 0:
        raise ValueError("No matching datasets for the dataset pattern")

    results = get_mc_lumi_list(inputDataset)

    results_keys = list(results.keys())
    if len(results) == 1:
        results[results_keys[0]].writeJSON(fileName=filename)
    elif len(results) > 1:
        # if there are two results assume its nominal+ext sample:
        # but it might sort the "ext" sample first, so we should
        # check which is which.
        def _print_save(key, fname):
            print("Saved", key, "to", fname)
            results[key].writeJSON(fileName=fname)

        stem, ext = os.path.splitext(filename)
        ext_filename = stem + "_ext" + ext
        if '_ext' in results_keys[0]:
            _print_save(results_keys[0], ext_filename)
            _print_save(results_keys[1], filename)
        else:
            _print_save(results_keys[0], filename)
            _print_save(results_keys[1], ext_filename)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("dataset", help="Dataset name to get lumilist")
    parser.add_argument("output", help="Output JSON filename")
    args = parser.parse_args()

    write_lumi_list(inputDataset=args.dataset, filename=args.output)
