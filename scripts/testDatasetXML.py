#!/usr/bin/env python


"""
Script to check the files in a dataset XML actually exist.

Returns 0 if all OK, non-0 otherwise.

"""


from __future__ import print_function
import os
import sys
import argparse


def check_xml_file(xml_filename):
    """Do the checking of filenames in the XML file.

    Note that we don't use a XML module for parsing as it doens't think it's
    "proper" XML.

    Parameters
    ----------
    xml_filename : str
        Filename of the XML file to check

    Returns
    -------
    bool
        True if all files extracted and exist, False otherwise

    """
    all_good = True
    with open(xml_filename) as f:
        for line in f:
            filename = line.strip()
            if filename.startswith("<!--") or "-->" in line:
                continue
            filename = line.replace('<In FileName="', '')
            filename = filename.replace('" Lumi="0.0"/>', '')
            filename = filename.strip()

            # sanity checks
            if not filename.startswith("/pnfs"):
                print("Bad filename: not starting with /pnfs:", filename)
                all_good = False
                continue
            if not filename.endswith(".root"):
                print("Bad filename: not end with .root:", filename)
                all_good = False
                continue

            if not os.path.isfile(filename):
                print(filename, "does not exists")
                all_good = False
    return all_good


def main(in_args=sys.argv[1:]):
    """
    Main routine, collect user args, run checks over all files specified.

    Parameters
    ----------
    in_args
        Input arguments to be parsed

    Returns
    -------
    int
        Status code:
        0 if all OK
        1 if missing ROOT file
        100 if missing XML file
        101 if not a XML file

    """
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("input", help="XML dataset file[s] to check", nargs='+')
    args = parser.parse_args(in_args)
    final_status = 0
    for xml_file in args.input:
        if not os.path.isfile(xml_file):
            print("Cannot find input", xml_file, "- skipping checks")
            final_status = 100
        elif os.path.splitext(xml_file)[1] != ".xml":
            print("Input file", xml_file, "not XML - skipping checks")
            final_status = 101
        else:
            status = check_xml_file(xml_file)
            if not status:
                final_status = 1

    return final_status


if __name__ == "__main__":
    sys.exit(main())
