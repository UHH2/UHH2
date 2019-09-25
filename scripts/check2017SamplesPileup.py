#!/usr/bin/env python


"""
Check 2017 MC samples to see if they are buggy PU ones or not.
Can read either list of dataset names from file,
or pass comma-separated list on command line.

Dataset names can contain wildcards.
In that case, prints out status for each entry,
then overall status (all buggy/all fixed/mixture of buggy and fixed)
"""

# This is necessary because they decided on a stupid dataset naming scheme,
# and I got bored copying and pasting into das.


from __future__ import print_function

import os
import sys
import argparse
from subprocess import check_output, call
from time import sleep


def check_voms():
    """Checks if the user has a valid VOMS proxy, returns True if so, False otherwise"""
    cmd = "voms-proxy-info -e"
    return_code = call(cmd.split())
    if return_code != 0:
        print("You need a valid voms proxy. Please run:")
        print("")
        print("    voms-proxy-init -voms cms")
        print("")
        print("Then retry this script")
        return False
    return True


def get_all_datasets(dataset_str):
    """Get all datasets from DAS matching pattern"""
    cmd = 'dasgoclient -query="dataset dataset=%s"' % dataset_str
    output = check_output(cmd, shell=True)
    if output:
        return output.splitlines()
    else:
        return None


def get_parent_dataset(dataset_str):
    """Get parent of dataset from DAS"""
    cmd = 'dasgoclient -query="parent dataset=%s"' % dataset_str
    output = check_output(cmd, shell=True)
    if output:
        return output.splitlines()[0]
    else:
        return None


def is_buggy_PU_aod_sample(aod_dataset):
    """Determines from AOD dataset name if buggy PU sample or not."""
    if "/AODSIM" not in aod_dataset:
        raise RuntimeError("%s not recognised as AODSIM" % aod_dataset)

    if "PU2017" in aod_dataset:
        return False
    else:
        return True


def is_buggy_PU_miniaod_sample(dataset):
    """Determines from MINIAOD dataset name if buggy PU sample or not."""
    if "/MINIAODSIM" not in dataset:
        raise RuntimeError("%s not recognised as MINIAODSIM" % dataset)

    if "new_pmx" in dataset: # new_pmx in MINIAOD name means for sure it is fixed PU. saves querying DAS
        return False
    elif "PU2017" not in dataset: # if PU2017 missing in MINIAOD name, for sure it is buggy PU saves querying DAS
        return True
    else:
        # otherwise we have to look at parent AOD
        parent_dataset = get_parent_dataset(dataset)
        return is_buggy_PU_aod_sample(parent_dataset)


def print_result(dataset, result_dict):
    """
    Print out result for this dataset name.
    `result_dict` is the dict of {dataset:bool} for each dataset
    matching the dataset name, which may include wildcards
    """
    is_buggy = any(result_dict.values())
    result_str = ""
    if not is_buggy:
        if len(result_dict) > 1:
            result_str = "All Fixed PU"
        else:
            result_str = "Fixed PU"
    else:
        if len(result_dict) > 1:
            result_str = "1+ buggy PU samples"
        else:
            result_str = "Buggy PU"

    print_dict = dict(dataset=dataset, result_str=result_str)
    print("{dataset}: {result_str}".format(**print_dict))

    # If wildcard, print out results of component datasets as well
    if len(result_dict) > 1:
        for detail_name, detail_status in result_dict.items():
            result_str = 'Buggy PU' if detail_status else 'Fixed PU'
            print_dict = dict(dataset=detail_name, result_str=result_str)
            print(">>>> {dataset}: {result_str}".format(**print_dict))


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--datasetList",
                        help="File with list of datasets. " \
                             "Each entry should be on a separate line.",
                        default=None)
    parser.add_argument("--datasets",
                        help="List of datasets. " \
                             "Each entry separated by a comma.",
                        default=None)
    args = parser.parse_args()

    if not args.datasetList and not args.datasets:
        raise RuntimeError("Must use either --datasetList or --datasets")
    if args.datasetList and args.datasets:
        raise RuntimeError("Must use either --datasetList or --datasets, not both")

    list_of_datasets = []
    if args.datasetList:
        if not os.path.isfile(args.datasetList):
            raise IOError("Cannot open %s" % args.datasetList)
        with open(args.datasetList) as f:
            list_of_datasets = [x.strip() for x in f.readlines()]

    else:
        list_of_datasets = [x.strip() for x in args.datasets.split(",")]

    # Tidy up entries
    list_of_datasets = [x.strip('"').strip("'") for x in list_of_datasets if x]

    print("Processing:", list_of_datasets)
    print("-"*100)

    if not check_voms():
        sys.exit(1)

    # Go through datasets, get info, figure out if buggy
    for ind, orig_dataset in enumerate(list_of_datasets, 1):
        these_results = {}
        expanded_datasets = [orig_dataset]

        if "*" in orig_dataset:
            expanded_datasets = get_all_datasets(orig_dataset)

        for dataset in expanded_datasets:
            these_results[dataset] = is_buggy_PU_miniaod_sample(dataset)

        print_result(orig_dataset, these_results)

        if ind < len(list_of_datasets):
            sleep(5)  # to be nice to DAS

