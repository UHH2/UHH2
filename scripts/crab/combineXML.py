#!/usr/bin/env python

"""
Combine XML files

Doesn't keep event count, user must do it afterwards.
"""


from __future__ import print_function

import os
import sys
import argparse
import xml.etree.ElementTree as ET
import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True
ROOT.gROOT.SetBatch(1)

from readaMCatNloEntries import get_xml_tree


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("xml", nargs='+', help="XML files to merge")
    parser.add_argument("output", help="Destination XML file")
    args = parser.parse_args()

    trees = []
    for filename in args.xml:
        if not os.path.isfile(filename):
            print("Missing", filename, " - skipping")
        this_tree = get_xml_tree(filename)
        trees.append(this_tree)

    with open(args.output, 'w') as outputfile:
        for tree in trees:
            for child in tree:
                if "function Comment" not in str(child.tag):
                    outputfile.write(ET.tostring(child))
                elif "FileName" in child.text:
                    # is comment but keep empty/bad/commented out files
                    # basically just ignore the count at the end
                    outputfile.write(ET.tostring(child))

    print("Output written to", args.output)
    print("If necessary, run readaMCatNloEntries.py on the new file")
