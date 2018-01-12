#!/bin/sh

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

	# Setup fastjet & fastjet-contrib
	# NB use curl not wget as curl available by cvmfs, wget isnt
	FJINSTALLDIR="$(pwd)/fastjet-install"
	curl -O http://fastjet.fr/repo/fastjet-${FJVER}.tar.gz
	tar xzf fastjet-${FJVER}.tar.gz
	mkdir "${FJINSTALLDIR}"
	cd fastjet-${FJVER}
	./configure --prefix="${FJINSTALLDIR}" --enable-allplugins --enable-allcxxplugins CXXFLAGS=-fPIC
	make $MAKEFLAGS
	make check
	make install
	cd ..

	# Add fastjet-config to PATH
	export PATH="${FJINSTALLDIR}/bin":$PATH

	curl -O http://fastjet.hepforge.org/contrib/downloads/fjcontrib-${FJCONTRIBVER}.tar.gz
	tar xzf fjcontrib-${FJCONTRIBVER}.tar.gz
	cd fjcontrib-${FJCONTRIBVER}
	# add HOTVR from SVN - do it this way until it becomes a proper contrib
	svn co http://fastjet.hepforge.org/svn/contrib/contribs/HOTVR/trunk HOTVR/
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
eval `cmsrel CMSSW_9_4_1`
cd CMSSW_9_4_1/src
eval `scramv1 runtime -sh`
git cms-init

# Add in preliminary EGamma VID
git cms-merge-topic lsoffi:CMSSW_9_4_0_pre3_TnP

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
# Get the UHH2 repo & JEC files
cd $CMSSW_BASE/src
git clone -b master https://github.com/UHH2/UHH2.git
cd UHH2
git clone https://github.com/cms-jet/JECDatabase.git
