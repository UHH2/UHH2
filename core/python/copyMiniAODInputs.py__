#!/usr/bin/env python


"""
Extract the input files in all the ntuplewriter*.py configs, and copy their
input files locally using xrootd.

This way, you can then run over the files without waiting for xrootd.
"""


import os
import sys
from glob import glob
import subprocess
import imp

import FWCore.ParameterSet.Config as cms


# standard xrootd directors, must end in /
# XROOTD = "root://cmsxrootd.fnal.gov/"  # broken?
XROOTD = "root://xrootd-cms.infn.it/"


def check_voms():
    """Checks if the user has a valid VOMS proxy, returns True if so, False otherwise"""
    cmd = "voms-proxy-info -e"
    return_code = subprocess.call(cmd.split())
    if return_code != 0:
        print "You need a valid voms proxy. Please run:"
        print ""
        print "    voms-proxy-init -voms cms"
        print ""
        print "Then retry this script"
        return False
    return True


def copy_file_over_xrootd(filename, output_dir=".", redirector=XROOTD):
    """Copy a file using xrdcp"""
    if not filename.startswith("/store"):
        return
    basename = os.path.basename(filename)
    output_filename = os.path.join(output_dir, basename)
    if os.path.isfile(output_filename):
        print "Output file already exists - skipping"
        return
    if not redirector.endswith("/"):
        redirector = "%s/" % redirector
    cmd = "xrdcp %s%s %s" % (redirector, filename, output_filename)
    print cmd
    subprocess.check_call(cmd.split())


if __name__ == "__main__":
    if not check_voms():
        sys.exit(1)
    output_dir = "."
    if not os.path.isdir(output_dir):
        os.makedirs(output_dir)
    for cfg_filename in glob("ntuplewriter*py"):
        print "-"*80
        print cfg_filename
        print "-"*80
        name = cfg_filename.replace(".py", "")
        fp, pathname, description = imp.find_module(name)
        this_module = imp.load_module(name, fp, pathname, description)
        for f in this_module.process.source.fileNames:
            copy_file_over_xrootd(f, output_dir=output_dir)
