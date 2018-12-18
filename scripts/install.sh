#!/bin/sh

# might be usefull to call
#
#   export CMSSW_GIT_REFERENCE=<DIRECTORYWITHENOUGHSPACE>/cmssw.git
#
# before running this script

# Exit script if command fails
set -e

# Create default make args for parallel jobs
if [ -z "$MAKEFLAGS" ]
then
	np=$(grep -c ^processor /proc/cpuinfo)
	let np+=2
	# Be nice on shared machines, don't take up all the cores
	limit="12"
	if [[ "$np" -gt "$limit" ]]
	then
		np=$limit
	fi
	export MAKEFLAGS="-j$np"
fi

getToolVersion() {
    # Get CMSSW tool version using scram
    # args: <toolname>
    local toolname="$1"
    scram tool info "$toolname" | grep -i "Version : " | sed "s/Version : //"
}

setupFastjet() {
	FJVER="$1"
	FJCONTRIBVER="$2"
	echo "Setting up fastjet $FJVER and fastjet-contrib $FJCONTRIBVER"

	FJINSTALLDIR="$(pwd)/fastjet-install"

	# Setup fastjet & fastjet-contrib
	mkdir "${FJINSTALLDIR}"

	# For normal fastjet
	# NB use curl not wget as curl available by cvmfs, wget isnt
	# curl -O http://fastjet.fr/repo/fastjet-${FJVER}.tar.gz
	# tar xzf fastjet-${FJVER}.tar.gz
	# cd fastjet-${FJVER}

	# Use the CMS version of fastjet as thread-safe
	git clone -b cms/v$FJVER https://github.com/UHH2/fastjet.git
	cd fastjet
	./configure --prefix="${FJINSTALLDIR}" --enable-allplugins --enable-allcxxplugins CXXFLAGS=-fPIC
	make $MAKEFLAGS
	# make check  # fails for siscone
	make install
	cd ..

	# Add fastjet-config to PATH
	export PATH="${FJINSTALLDIR}/bin":$PATH

	# For normal fastjet-contrib
	# curl -O http://fastjet.hepforge.org/contrib/downloads/fjcontrib-${FJCONTRIBVER}.tar.gz
	# tar xzf fjcontrib-${FJCONTRIBVER}.tar.gz
	# cd fjcontrib-${FJCONTRIBVER}

	# Use the CMS version of fastjet-contrib as thread-safe
	git clone -b cms/v$FJCONTRIBVER https://github.com/UHH2/fastjet-contrib.git
	cd fastjet-contrib
	# add HOTVR from github
	# really should do it from SVN, but currently doesn't allow anonymous access
	# do it this way until it becomes a proper contrib
	git clone https://github.com/UHH2/HOTVRContrib.git HOTVR/
	# although we add fastjet-config to path, due to a bug we need to
	# explicitly state its path to ensure the necessary fragile library gets built
	./configure --fastjet-config="${FJINSTALLDIR}/bin/fastjet-config" CXXFLAGS=-fPIC
	make $MAKEFLAGS
	make check
	make install
	# the fragile libs are necessary for CMSSW
	make fragile-shared
	make fragile-shared-install
	cd ..
}

source /cvmfs/cms.cern.ch/cmsset_default.sh

# Get SFrame, do not compile it until we have the right ROOT etc
git clone https://github.com/UHH2/SFrame.git

# Get CMSSW
export SCRAM_ARCH=slc6_amd64_gcc630
CMSREL=CMSSW_9_4_10
eval `cmsrel ${CMSREL}`
cd ${CMSREL}/src
eval `scramv1 runtime -sh`

# Install FastJet & contribs for HOTVR & XCONE
cd ../..
FJVER="3.2.1"
FJCONTRIBVER="1.033"
time setupFastjet $FJVER $FJCONTRIBVER

cd $CMSSW_BASE/src

time git cms-init -y  # not needed if not addpkg ing

# Add in both cut-based and MVA EGamma VID
git cms-merge-topic guitargeek:EgammaID_9_4_X
# Remove old IDs that inflate CRAB tarball over limit
rm -r RecoEgamma/PhotonIdentification/data/MVA/PHYS14
rm -r RecoEgamma/PhotonIdentification/data/MVA/Spring16
rm -r RecoEgamma/PhotonIdentification/data/MVA/Spring15
rm -r RecoEgamma/ElectronIdentification/data/PHYS14
rm -r RecoEgamma/ElectronIdentification/data/Spring15
rm -r RecoEgamma/ElectronIdentification/data/Spring16_GeneralPurpose_V1
rm -r RecoEgamma/ElectronIdentification/data/Spring16_HZZ_V1
rm -r RecoEgamma/ElectronIdentification/data/Summer16

# Necessary for using our FastJet
git cms-addpkg RecoJets/JetProducers
# Necessary for using Fastjet 3.2.1 to pickup new JetDefinition default arg order
rm RecoJets/JetProducers/test/BuildFile.xml
rm RecoJets/JetProducers/test/test-large-voronoi-area.cc  # old test, not used?
git cms-addpkg RecoBTag/SecondaryVertex
git cms-addpkg RecoJets/JetAlgorithms
git cms-addpkg PhysicsTools/JetMCAlgos

# EE noise mitigation in MET
git cms-merge-topic cms-met:METFixEE2017_949_v2

# Update FastJet and contribs for HOTVR and UniversalJetCluster
FJINSTALL=$(fastjet-config --prefix)
OLD_FJ_VER=$(getToolVersion fastjet)
FJ_TOOL_FILE=$CMSSW_BASE/config/toolbox/$SCRAM_ARCH/tools/selected/fastjet.xml
sed -i "s|/cvmfs/cms.cern.ch/$SCRAM_ARCH/external/fastjet/$OLD_FJ_VER|$FJINSTALL|g" "$FJ_TOOL_FILE"
sed -i "s|$OLD_FJ_VER|$FJVER|g" "$FJ_TOOL_FILE"

OLD_FJCONTRIB_VER=$(getToolVersion fastjet-contrib)
FJCONFIG_TOOL_FILE=$CMSSW_BASE/config/toolbox/$SCRAM_ARCH/tools/selected/fastjet-contrib.xml
sed -i "s|/cvmfs/cms.cern.ch/$SCRAM_ARCH/external/fastjet-contrib/$OLD_FJCONTRIB_VER|$FJINSTALL|g" "$FJCONFIG_TOOL_FILE"
sed -i "s|$OLD_FJCONTRIB_VER|$FJCONTRIBVER|g" "$FJCONFIG_TOOL_FILE"

FJCONFIG_ARCHIVE_TOOL_FILE=$CMSSW_BASE/config/toolbox/$SCRAM_ARCH/tools/selected/fastjet-contrib-archive.xml
sed -i "s|/cvmfs/cms.cern.ch/$SCRAM_ARCH/external/fastjet-contrib/$OLD_FJCONTRIB_VER|$FJINSTALL|g" "$FJCONFIG_ARCHIVE_TOOL_FILE"
sed -i "s|$OLD_FJCONTRIB_VER|$FJCONTRIBVER|g" "$FJCONFIG_ARCHIVE_TOOL_FILE"

scram setup fastjet
scram setup fastjet-contrib
scram setup fastjet-contrib-archive

scram b clean
time scram b $MAKEFLAGS

# Get the UHH2 repo & JEC files
cd $CMSSW_BASE/src
time git clone -b RunII_94X_v3 https://github.com/UHH2/UHH2.git
cd UHH2
time git clone https://github.com/cms-jet/JECDatabase.git
