#!/usr/bin/env python


"""Check XML against CRAB log & remove files that crab thought were still transferring

This is necessary if you then use notFinishedLumis.json from crab report, 
since that will contain jobs that CRAB thought were transferring 
*even if* they look OK on T2. 
This therefore avoids duplicate events.
"""


from __future__ import print_function

import os
import re
import argparse


def get_transferring_job_numbers(crab_log):
    """Get all job numbers that were transferring at the last status check"""
    if not os.path.isfile(crab_log):
        raise IOError("Cannot find crab log!")

    # First find the last status of all jobs in the log file
    status_text = None
    with open(crab_log) as cf:
        for line in cf:
            if "Got information from status cache file" in line:
                status_text = line.split("{", 1)[1]  # divide at first {

    if status_text is None:
        raise RuntimeError("Cannot find status in crab log")

    # Now go through and identify all bad jobs
    bad_ntuple_names = []

    # Would parse this as JSON, but has some single quotes that are 
    # impossible to remove since everything in crab.log is single quotes, bah
    # Note that if they change the format of crab.log, this will need updating
    this_job = None
    for w in status_text.split():
        this_word = w.lower().strip()
        # Find job number, e.g. '123':
        match = re.search(r'.[0-9]+.:', this_word)
        if match:
            this_job = match.group().strip(":").replace('"', "").replace("'", "")
        # Figure out if job was transferring
        # Note that a job can be "finished" but still have errors,
        # so can't use for successful jobs
        # Also, a job may have "Errors" but be successful - maybe was error from
        # early retry?
        # So very difficult to get *only* successful jobs
        elif "transferring" in this_word:
            bad_ntuple_names.append(this_job)

    return bad_ntuple_names


def job_numbers_to_filenames(job_nums):
    """Translate job numbers to ntuple filenames"""
    return ["Ntuple_%s.root" % this_job for this_job in job_nums]


def create_good_xml(xml_filename, new_xml_filename, bad_ntuple_names):
    """Copy lines from original XML to new XML if they don't contain bad ntuple"""
    if not os.path.isfile(xml_filename):
        raise IOError("Cannot find xml_filename!")

    with open(xml_filename) as inf, open(new_xml_filename, "w") as of:
        for line in inf:
            if not any([r in line for r in bad_ntuple_names]):
                of.write(line)  # already has newline at end


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("crabLog", help="crab.log file for this set of crab jobs")
    parser.add_argument("inXML", help="Input XML file to be cleaned up")
    parser.add_argument("outXML", help="Output XML file to be written")
    args = parser.parse_args()

    job_nums = get_transferring_job_numbers(args.crabLog)
    bad_ntuple_names = job_numbers_to_filenames(job_nums)
    if len(bad_ntuple_names) > 0:
        print(len(bad_ntuple_names), "'bad' ntuple filenames:", bad_ntuple_names)
        create_good_xml(args.inXML, args.outXML, bad_ntuple_names)
        print("Written XML without bad ntuples to", args.outXML)
    else:
        print("No bad filenames to replace, no updated XML will be produced")
