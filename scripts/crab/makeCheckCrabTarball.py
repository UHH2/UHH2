#!/usr/bin/env python

"""Script to create a CRAB tarball from this CMSSW environment, and optionally check the size."""


from  __future__ import division   # make division work like in python3

import re
import os
import sys
import logging
import argparse
import subprocess

try:
    from CRABClient.JobType.UserTarball import UserTarball, FILE_SIZE_LIMIT
    from CRABClient.UserUtilities import config
except ImportError as e:
    print "ImportError:", e.message
    print ""
    print "Cannot import CRABClient library, make sure you do:\n\n    source /cvmfs/cms.cern.ch/crab3/crab.sh\n"
    sys.exit(1)


logger = logging.getLogger("CRAB_TARBALL_TEST")
logger.setLevel(logging.DEBUG)
ch = logging.StreamHandler()
ch.setLevel(logging.DEBUG)
logger.addHandler(ch)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument("--output",
                        help="Output tarball filename",
                        default="testTarball.tgz")
    parser.add_argument("--noSizeCheck",
                        help="Skip tarball size check",
                        action='store_true', default=False)
    args = parser.parse_args()

    # Use the actual config object users will use
    from crab_template import config as dummy_config

    # # Create dummy config file for testing - only needs JobType info
    # dummy_config = config()
    # dummy_config.JobType.psetName = os.path.join(os.environ['CMSSW_BASE'], 'src/UHH2/core/python/ntuplewriter_data_2018.py')
    # dummy_config.JobType.inputFiles = [os.path.join(os.environ['CMSSW_BASE'], 'src/L1Prefiring/EventWeightProducer/files/L1PrefiringMaps_new.root')]
    # dummy_config.JobType.sendExternalFolder = True # needed for fastjet setup

    # Create tarball
    tarball_name = args.output

    with UserTarball(name=tarball_name, config=dummy_config, logger=logger) as tb:
        # We ignore the pset and pickle file as they are only a few MB, account for later
        inputFiles = [re.sub(r'^file:', '', file) for file in getattr(dummy_config.JobType, 'inputFiles', [])]
        tb.addFiles(userFiles=inputFiles)

    if not os.path.isfile(tarball_name):
        raise IOError("Tarball file %s was not produced" % tarball_name)

    if args.noSizeCheck:
        sys.exit(0)

    # Check size
    size_bytes = os.path.getsize(tarball_name)
    # in python3 and python2 with __future__ division, // means integer division
    size_mb = "%3.1f MB" % (size_bytes // (1024 * 1024))

    padding_bytes = (3*1024*1024)  # padding for pickle file etc
    new_limit_bytes = (FILE_SIZE_LIMIT - padding_bytes)
    limit_mb = "%3.1f MB" % (new_limit_bytes // (1024 * 1024))

    if size_bytes >= new_limit_bytes:
        # Print out contents sorted by reverse size
        subprocess.call("tar tvzf %s | sort -k3 -n" % tarball_name, shell=True)
        logger.error("Error: tarball size %s exceeds the limit of %s" % (size_mb, limit_mb))
        sys.exit(1)
    else:
        logger.info("Success: tarball size %s is below limit of %s" % (size_mb, limit_mb))

    sys.exit(0)
