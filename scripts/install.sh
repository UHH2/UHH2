#!/usr/bin/env bash -e

# might be usefull to call
#
#   export CMSSW_GIT_REFERENCE=<DIRECTORYWITHENOUGHSPACE>/cmssw.git
#
# before running this script

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
	autoreconf -f -i  # needed to avoid 'aclocal-1.15' is missing on your system
	# Optimisation flags same as in CMSSW
	# But not -fffast-time as fails Nsubjettiness checks
	FJCXXFLAGS="-O3 -Wall -ftree-vectorize -msse3 -fPIC"
	./configure --prefix="${FJINSTALLDIR}" --enable-atlascone --enable-cmsiterativecone --enable-siscone --enable-allcxxplugins --enable-pyext --disable-auto-ptr CXXFLAGS="${FJCXXFLAGS}"
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
	./configure --fastjet-config="${FJINSTALLDIR}/bin/fastjet-config" CXXFLAGS="${FJCXXFLAGS}"
	make $MAKEFLAGS
	make check
	make install
	# the fragile libs are necessary for CMSSW
	make fragile-shared
	make fragile-shared-install
	cd ..
}

checkArch() {
	# Check if this machine is compatible, because at DESY the default is SL6,
	# whereas we need EL7
	# Note that lxplus machines have uname e.g. 3.10.0-1062.4.1.el7.x86_64
	# so just checking for el7 is OK
	# To complicate matters, we need to handle gitlab CI as well,
	# however there uname is useless, it just gives e.g. 4.19.68-coreos. 
	# Instead we must rely on the IMAGE variable that we set in .gitlab-ci.yml
	# (yes that is a horrible dependence)
	if [ -n "$IMAGE" ]
	then
		# here is on gitlab CI
		if [[ "$IMAGE" != *cc7* ]]; then
			echo "This release requires a CC7 image"
			echo "Please update .gitlab-ci.yml and/or testPR.sh and run this again"
			exit 1
		fi
	else
		# here is normal running e.g. on NAF
		KERNEL=$(uname -r)
		if [[ "$KERNEL" != *el7* ]]; then
			echo "This release requires an EL7 machine, e.g. naf-cms-el7.desy.de"
			echo "Please log into one and run this again"
			exit 1
		fi
	fi
}

source /cvmfs/cms.cern.ch/cmsset_default.sh

# Get SFrame, do not compile it until we have the right ROOT etc
time git clone https://github.com/UHH2/SFrame.git

# Get CMSSW
export SCRAM_ARCH=slc7_amd64_gcc700
checkArch
CMSREL=CMSSW_10_6_8
eval `cmsrel ${CMSREL}`
cd ${CMSREL}/src
eval `scramv1 runtime -sh`

# Install FastJet & contribs for HOTVR & XCONE
cd ../..
FJVER="3.3.0"
FJCONTRIBVER="1.033"
time setupFastjet $FJVER $FJCONTRIBVER

cd $CMSSW_BASE/src

time git cms-init -y  # not needed if not addpkg ing

# Necessary for using our FastJet
time git cms-addpkg RecoJets/JetProducers
# For JetCorrector, JetResolution objects
time git cms-addpkg CondFormats/JetMETObjects
time git cms-addpkg JetMETCorrections/Modules

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

# Get the UHH2 repo & JEC,JER files
cd $CMSSW_BASE/src
time git clone -b RunII_106X_v1 https://github.com/UHH2/UHH2.git
cd UHH2
time git clone https://github.com/cms-jet/JECDatabase.git
time git clone https://github.com/cms-jet/JRDatabase.git
