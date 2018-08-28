#!/usr/bin/env python
# -*- coding: utf-8 -*-

#####################
##  DOCUMENTATION  ##
#####################

# Empty Ntuples may cause sframe_main to crash under certain circumstances, e.g. if the last Ntuple in the
# list of input files within the job's xml file is an empty one. Thus, you can call this script to comment out
# empty Ntuples in the dataset xml in the first place (creates a new dataset xml, old one is not overwritten).

# This sframe_main bug should be fixed at some point within the actual sframe code but, for now, this is a nice workaround.

# After your CRAB jobs are finished and you created the XML, call this script just like you do with readaMCatNloEntries.py

# Usage:
# >>> python commentOutEmptyNtuples.py ../../common/datasets/DATA_SingleElectron_RunB.xml
# Creates: ../../common/datasets/DATA_SingleElectron_RunB_withoutEmptyNtuples.xml


import sys
import os

def read_xml(xmlFileDir):
    xmlFile = open(str(xmlFileDir))
    rootFileStore = []
    comment = False                      # Think this code through via this example:
    for line in xmlFile:                 #
        if '<!--' in line:               # asdfasdfasdfasdfasdf
            if '-->' in line:            # asdfasdfasdfasdfasdf
                continue                 # <!-- asfdasdfasdfasdfasdf -->
            else:                        # asdfasdfasdfasdfasdf
                comment = True           # <!-- asdfasdfasfdasdfasdf
                continue                 # asdfasdfasdfasdfasdf
        if comment == True:              # asdfasdfasdfasdfasdf -->
            if not '-->' in line:        # asdfasdfasdfasdfasdf
                continue                 # 
            else:                        # Not waterproof if comment starts mid-line (or violates this scheme in another
                comment = False          # way) but mid-line comments should not exist in these files in the first place
                continue
        if comment == False:
            rootFileStore.append(line.split('"')[1])
    return rootFileStore

# Empty Ntuples have a size of 36167 bytes. Ntuples with just one event are already significantly larger.
# Hence, the following method should be more effective in finding empty Ntuples than actually opening each
# Ntuple and counting the events in it (like in read_tree function of readaMCatNloEntries.py)
def findEmptyRootFilesBySize(rootFileStore):
    emptyRootFiles = []
    for rootFile in rootFileStore:
        statinfo = os.stat(rootFile)
        if statinfo.st_size < 37000:
            emptyRootFiles.append(rootFile)
    return emptyRootFiles

def commentOutEmptyRootFiles(xmlfiles):
    print 'Begin commenting out empty Ntuples ...'
    emptyRootFiles_allxmls = []
    for xml in xmlfiles:
        rootFileStore = read_xml(xml)
        emptyRootFiles = findEmptyRootFilesBySize(rootFileStore)
        emptyRootFiles_allxmls.append(emptyRootFiles)
        xmlFile_in = open(str(xml))
        xmlFile_out_path = str(xml)
        xmlFile_out_path = os.path.splitext(xmlFile_out_path)[0]+'_withoutEmptyNtuples.xml'
        xmlFile_out = open(xmlFile_out_path,"wt")
        for line in xmlFile_in:
            newline = line
            for path in emptyRootFiles:
                if path in line:
                    newline = '<!-- '+line[0:len(line)-1]+' -->'+line[len(line)-1]
            xmlFile_out.write(newline)
        xmlFile_in.close()
        xmlFile_out.close()
        print 'Wrote new file:', xmlFile_out_path
    print 'Done commenting out empty Ntuples.'
    return emptyRootFiles_allxmls

if __name__ == "__main__":
    xmllist = sys.argv[1:]
    print 'Input:', xmllist
    commentOutEmptyRootFiles(xmllist)
