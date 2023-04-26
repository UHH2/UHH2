#!/usr/bin/env tcsh -e

# Create default make args for parallel jobs
set maxcores=12
set cores=`grep processor /proc/cpuinfo | wc -l`
if ( $cores > $maxcores ) then
   set cores=$maxcores
endif
echo "Compiling with $cores threads"
set MAKEFLAGS="-j${cores}"

# Get CMSSW
source /cvmfs/cms.cern.ch/cmsset_default.csh
setenv SCRAM_ARCH slc7_amd64_gcc11
set kernel=`uname -r`
if ( "$kernel" =~ *el7* ) then
    :
else
    echo "This release requires an EL7 machine, e.g. naf-cms-el7.desy.de"
    echo "Please log into one and run this again"
    exit 1
endif
set CMSREL=CMSSW_13_0_3
eval `cmsrel ${CMSREL}`
cd ${CMSREL}/src
sed -i 's/CHECK_PRIVATE_HEADERS="1"/CHECK_PRIVATE_HEADERS="0"/g' $CMSSW_BASE/config/Self.xml
eval `scramv1 runtime -csh`
run scram setup self

# Install FastJet & contribs for HOTVR & XCONE
set FJVER="3.3.0"
set FJCONTRIBVER="1.033"
echo "Setting up fastjet ${FJVER} and fastjet-contrib ${FJCONTRIBVER}"
set FJINSTALLDIR="${CMSSW_BASE}/test/fastjet-install"
mkdir ${FJINSTALLDIR}
cd ${FJINSTALLDIR}

# Use the CMS version of fastjet as thread-safe
git clone -b "cms/v${FJVER}" https://github.com/UHH2/fastjet.git
cd fastjet
autoreconf -f -i  # needed to avoid 'aclocal-1.15' is missing on your system
# Optimisation flags same as in CMSSW
# But not -fffast-time as fails Nsubjettiness checks
./configure --prefix=${FJINSTALLDIR} --enable-atlascone --enable-cmsiterativecone --enable-siscone --enable-allcxxplugins --enable-pyext --disable-auto-ptr CXXFLAGS='-O3 -Wall -ftree-vectorize -msse3 -fPIC'
make $MAKEFLAGS
# make check  # fails for siscone
make install
cd ../..

# Add fastjet-config to PATH
setenv PATH "${FJINSTALLDIR}/bin":${PATH}

# Use the CMS version of fastjet-contrib as thread-safe
git clone -b "cms/v${FJCONTRIBVER}" https://github.com/UHH2/fastjet-contrib.git
cd fastjet-contrib
# add HOTVR from github
# really should do it from SVN, but currently doesnt allow anonymous access
# do it this way until it becomes a proper contrib
git clone https://github.com/UHH2/HOTVRContrib.git HOTVR/
# although we add fastjet-config to path, due to a bug we need to
# explicitly state its path to ensure the necessary fragile library gets built
./configure --fastjet-config="${FJINSTALLDIR}/bin/fastjet-config" CXXFLAGS='-O3 -Wall -ftree-vectorize -msse3 -fPIC'
make $MAKEFLAGS
make check
make install
# the fragile libs are necessary for CMSSW
make fragile-shared
make fragile-shared-install
cd ..

# Get SFrame and build it
git clone https://github.com/UHH2/SFrame.git
cd SFrame
source setup.csh
time make $MAKEFLAGS

cd ${CMSSW_BASE}/src

time git cms-init -y  # not needed if not addpkg ing

# Necessary for using our FastJet
time git cms-addpkg RecoJets/JetProducers
# For JetCorrector, JetResolution objects
time git cms-addpkg CondFormats/JetMETObjects
time git cms-addpkg JetMETCorrections/Modules

# Update FastJet and contribs for HOTVR and UniversalJetCluster
set FJINSTALL=`fastjet-config --prefix`
set OLD_FJ_VER=`scram tool info fastjet | grep -i "Version : " | sed "s/Version : //"`
set FJ_TOOL_FILE=${CMSSW_BASE}/config/toolbox/${SCRAM_ARCH}/tools/selected/fastjet.xml
sed -i "s|/cvmfs/cms.cern.ch/${SCRAM_ARCH}/external/fastjet/${OLD_FJ_VER}|${FJINSTALL}|g" "${FJ_TOOL_FILE}"
sed -i "s|${OLD_FJ_VER}|${FJVER}|g" "${FJ_TOOL_FILE}"

set OLD_FJCONTRIB_VER=`scram tool info fastjet-contrib | grep -i "Version : " | sed "s/Version : //"`
set FJCONFIG_TOOL_FILE=${CMSSW_BASE}/config/toolbox/${SCRAM_ARCH}/tools/selected/fastjet-contrib.xml
sed -i "s|/cvmfs/cms.cern.ch/${SCRAM_ARCH}/external/fastjet-contrib/${OLD_FJCONTRIB_VER}|${FJINSTALL}|g" "${FJCONFIG_TOOL_FILE}"
sed -i "s|${OLD_FJCONTRIB_VER}|${FJCONTRIBVER}|g" "${FJCONFIG_TOOL_FILE}"

set FJCONFIG_ARCHIVE_TOOL_FILE=${CMSSW_BASE}/config/toolbox/${SCRAM_ARCH}/tools/selected/fastjet-contrib-archive.xml
sed -i "s|/cvmfs/cms.cern.ch/${SCRAM_ARCH}/external/fastjet-contrib/${OLD_FJCONTRIB_VER}|${FJINSTALL}|g" "${FJCONFIG_ARCHIVE_TOOL_FILE}"
sed -i "s|${OLD_FJCONTRIB_VER}|${FJCONTRIBVER}|g" "${FJCONFIG_ARCHIVE_TOOL_FILE}"

scram setup fastjet
scram setup fastjet-contrib
scram setup fastjet-contrib-archive

# fetching Egamma POG postrecotools
# twiki: https://twiki.cern.ch/twiki/bin/view/CMS/EgammaUL2016To2018
time git cms-addpkg RecoEgamma/EgammaTools
time git clone https://github.com/cms-egamma/EgammaPostRecoTools.git
mv EgammaPostRecoTools/python/EgammaPostRecoTools.py RecoEgamma/EgammaTools/python/.
time git clone -b ULSSfiles_correctScaleSysMC https://github.com/jainshilpi/EgammaAnalysis-ElectronTools.git EgammaAnalysis/ElectronTools/data/
time git cms-addpkg EgammaAnalysis/ElectronTools

scram b clean
time scram b $MAKEFLAGS

# Get the UHH2 repo & JEC,JER files
cd ${CMSSW_BASE}/src
time git clone -b RunII_106X_v2 https://github.com/UHH2/UHH2.git
cd UHH2
time git clone https://github.com/cms-jet/JECDatabase.git
time git clone https://github.com/cms-jet/JRDatabase.git
cd common
time git clone https://github.com/UHH2/UHH2-datasets.git
cd UHH2-datasets
git remote rename origin UHH2
git branch -u UHH2/master
cd ../..
time make $MAKEFLAGS
