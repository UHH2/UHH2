#!/usr/bin/env python

"""
Convert XML list of files to plain text list, ignoring comments
"""


from __future__ import print_function

import os
import argparse
import xml.etree.ElementTree as ET
import sys
sys.path.append(os.path.join(os.environ['CMSSW_BASE'], "src", "UHH2", "scripts", "crab"))
from CommentedTreeBuilder import CommentedTreeBuilder


def get_xml_tree(xml_filename):
    with open(xml_filename) as f:
        # hack to trick XML parsing - put under a root node
        xml_str = "<data>%s</data>" % f.read()
    parser = ET.XMLParser(target=CommentedTreeBuilder())
    root = ET.fromstring(xml_str, parser)
    return root


def get_root_filenames_from_xml(xml_filename):
    # skips commented out lines, no matter if it has FileName="..."
    rootFileStore = []
    root = get_xml_tree(xml_filename)
    for child in root:
        if child.tag != "In":
            # this will also skip commented out lines
            continue
        filename = child.attrib['FileName']
        if ".root" in filename:
            rootFileStore.append(filename)
    return rootFileStore


def dump_root_filenames_from_xml_to_txt(xml_filename, txt_filename):
    root_filenames = get_root_filenames_from_xml(xml_filename)
    with open(txt_filename, 'w') as f:
        for rf in root_filenames:
            f.write("%s\n" % rf)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("xml", help="Input XML file")
    parser.add_argument("txt", help="Output text file")
    args = parser.parse_args()

    dump_root_filenames_from_xml_to_txt(args.xml, args.txt)
