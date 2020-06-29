#!/usr/bin/env python

"""
Produce new XML file with filenames that appear in a text file commented out
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


def get_filenames_from_plain_text(txt_filename):
    with open(txt_filename) as f:
        filenames = f.readlines()

    # Convert // to /, remove \n, etc
    filenames = [os.path.normpath(rf.strip()) for rf in filenames if rf.strip()]
    return filenames


def comment_out_bad_xml(xml_filename, txt_filename, output_filename):
    reference_filenames = get_filenames_from_plain_text(txt_filename)

    with open(output_filename, 'w') as f:
        root = get_xml_tree(xml_filename)
        for child in root:
            if child.tag != "In":
                # this will print commented out lines
                f.write(ET.tostring(child))
            else:
                filename = child.attrib['FileName']
                if ".root" in filename:
                    filename = os.path.normpath(filename.strip())
                    if filename not in reference_filenames:
                        # print out line as it was before
                        f.write(ET.tostring(child))
                    else:
                        # comment it out
                        f.write("<!-- BAD %s -->\n" % ET.tostring(child).strip())


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("xml", help="Input XML file")
    parser.add_argument("txt", help="Output text file")
    args = parser.parse_args()

    stem, ext = os.path.splitext(args.xml)
    output_filename = stem + "_nobad" + ext
    comment_out_bad_xml(args.xml, args.txt, output_filename)
    print("Written updated file", output_filename)
