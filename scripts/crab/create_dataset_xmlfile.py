#!/usr/bin/env python

import sys, glob

def create_dataset_xml(pattern, outfile):
   out = open(outfile, 'w')
   l = glob.glob(pattern)
   print "Found %d files matching pattern" % len(l)
   l.sort()
   for file in l:
      out.write('<In FileName="%s" Lumi="0.0"/>\n' % file)
   out.close()

if __name__ == '__main__':
   if len(sys.argv) != 3:
      print "Usage: %s <root file pattern with full path> <xml output file>"
      sys.exit(1)
   create_dataset_xml(sys.argv[1],sys.argv[2])
