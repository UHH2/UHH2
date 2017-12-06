#!/bin/sh

# might be usefull to call 
#
#   export CMSSW_GIT_REFERENCE=<DIRECTORYWITHENOUGHSPACE>/cmssw.git
#
# before running this script

source /cvmfs/cms.cern.ch/cmsset_default.sh

# Get SFrame, do not compile it until we have the right ROOT etc
git clone https://github.com/UHH2/SFrame.git

export SCRAM_ARCH=slc6_amd64_gcc630
eval `cmsrel CMSSW_9_4_1`
cd CMSSW_9_4_1/src
eval `scramv1 runtime -sh`
git cms-init

#git cms-addpkg RecoJets/JetProducers

# Update FastJet and contribs for HOTVR and UniversalJetCluster
#sed -i "s|3.1.0|3.2.1|g" $CMSSW_BASE/config/toolbox/$SCRAM_ARCH/tools/selected/fastjet.xml
#sed -i "s|/cvmfs/cms.cern.ch/slc6_amd64_gcc630/external/fastjet/3.2.1|/afs/desy.de/user/a/aggleton/public/fastjet/slc6_amd64_gcc630/fastjet-install|g" $CMSSW_BASE/config/toolbox/$SCRAM_ARCH/tools/selected/fastjet.xml
#sed -i "s|/cvmfs/cms.cern.ch/slc6_amd64_gcc630/external/fastjet-contrib/1.026|/afs/desy.de/user/a/aggleton/public/fastjet/slc6_amd64_gcc630/fastjet-install|g" $CMSSW_BASE/config/toolbox/$SCRAM_ARCH/tools/selected/fastjet-contrib.xml
#sed -i "s|/cvmfs/cms.cern.ch/slc6_amd64_gcc630/external/fastjet-contrib/1.026|/afs/desy.de/user/a/aggleton/public/fastjet/slc6_amd64_gcc630/fastjet-install|g" $CMSSW_BASE/config/toolbox/$SCRAM_ARCH/tools/selected/fastjet-contrib-archive.xml
#scram setup fastjet
#scram setup fastjet-contrib
#scram setup fastjet-contrib-archive

#scram b clean
scram b -j 20
git clone -b master https://github.com/UHH2/UHH2.git
cd UHH2
git clone https://github.com/cms-jet/JECDatabase.git
