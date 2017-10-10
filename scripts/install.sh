#!/bin/sh

# might be usefull to call 
#
#   export CMSSW_GIT_REFERENCE=<DIRECTORYWITHENOUGHSPACE>/cmssw.git
#
# before running this script


source /cvmfs/cms.cern.ch/cmsset_default.sh
export SCRAM_ARCH=slc6_amd64_gcc530
eval `cmsrel CMSSW_9_2_4`
cd CMSSW_9_2_4/src
eval `scramv1 runtime -sh`
git cms-init
#git cms-merge-topic -u cms-met:fromCMSSW_8_0_20_postICHEPfilter
#git cms-merge-topic gkasieczka:test-httv2-8014
#git cms-merge-topic ikrav:egm_id_80X_v2
git cms-addpkg RecoJets/JetProducers
git cms-addpkg RecoJets/JetAlgorithms
git cms-addpkg DataFormats/JetReco
git cms-addpkg PhysicsTools/JetMCAlgos
git cms-addpkg RecoBTag
git cms-addpkg RecoEgamma

#sed -i "s|use_common_bge_for_rho_and_rhom|set_common_bge_for_rho_and_rhom|g" RecoJets/JetProducers/plugins/FastjetJetProducer.cc
#sed -i "s|1.020|1.025|g" $CMSSW_BASE/config/toolbox/slc6_amd64_gcc530/tools/selected/fastjet-contrib.xml ## FASTJET-CONTRIB IS PART OF CMSSW 91X
#sed -i "s|/cvmfs/cms.cern.ch/slc6_amd64_gcc530/external/fastjet-contrib/1.025|/afs/desy.de/user/p/peiffer/www/FastJet|g" ## FASTJET-CONTRIB IS PART OF CMSSW 91X $CMSSW_BASE/config/toolbox/slc6_amd64_gcc530/tools/selected/fastjet-contrib.xml
sed -i "s|3.1.0|3.2.1|g" $CMSSW_BASE/config/toolbox/slc6_amd64_gcc530/tools/selected/fastjet.xml
sed -i "s|/cvmfs/cms.cern.ch/slc6_amd64_gcc530/external/fastjet/3.2.1|/afs/desy.de/user/p/peiffer/www/FastJet|g" $CMSSW_BASE/config/toolbox/slc6_amd64_gcc530/tools/selected/fastjet.xml
scram b clean
scram b -j 20
#cd $CMSSW_BASE/external
#cd slc6_amd64_gcc530/
#git clone https://github.com/ikrav/RecoEgamma-ElectronIdentification.git data/RecoEgamma/ElectronIdentification/data
#cd data/RecoEgamma/ElectronIdentification/data
#git checkout egm_id_80X_v1
cd $CMSSW_BASE/src
git clone -b RunII_92X_v1 https://github.com/UHH2/UHH2.git
cd UHH2
git clone https://github.com/cms-jet/JECDatabase.git
