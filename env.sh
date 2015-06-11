#!/bin/bash

cmsenv

# Electron MVA
mkdir -p $CMSSW_BASE/src/EgammaAnalysis/ElectronTools/data/CSA14
cd $CMSSW_BASE/src/EgammaAnalysis/ElectronTools/data/CSA14

wget https://raw.githubusercontent.com/HuguesBrun/cmssw/addTheElecIDMVAoutputCSA14/EgammaAnalysis/ElectronTools/data/CSA14/EIDmva_EB_10_25ns_BDT.weights.xml
wget https://raw.githubusercontent.com/HuguesBrun/cmssw/addTheElecIDMVAoutputCSA14/EgammaAnalysis/ElectronTools/data/CSA14/EIDmva_EB_10_50ns_BDT.weights.xml
wget https://raw.githubusercontent.com/HuguesBrun/cmssw/addTheElecIDMVAoutputCSA14/EgammaAnalysis/ElectronTools/data/CSA14/EIDmva_EB_5_25ns_BDT.weights.xml
wget https://raw.githubusercontent.com/HuguesBrun/cmssw/addTheElecIDMVAoutputCSA14/EgammaAnalysis/ElectronTools/data/CSA14/EIDmva_EB_5_50ns_BDT.weights.xml
wget https://raw.githubusercontent.com/HuguesBrun/cmssw/addTheElecIDMVAoutputCSA14/EgammaAnalysis/ElectronTools/data/CSA14/EIDmva_EE_10_25ns_BDT.weights.xml
wget https://raw.githubusercontent.com/HuguesBrun/cmssw/addTheElecIDMVAoutputCSA14/EgammaAnalysis/ElectronTools/data/CSA14/EIDmva_EE_10_50ns_BDT.weights.xml
wget https://raw.githubusercontent.com/HuguesBrun/cmssw/addTheElecIDMVAoutputCSA14/EgammaAnalysis/ElectronTools/data/CSA14/EIDmva_EE_5_25ns_BDT.weights.xml
wget https://raw.githubusercontent.com/HuguesBrun/cmssw/addTheElecIDMVAoutputCSA14/EgammaAnalysis/ElectronTools/data/CSA14/EIDmva_EE_5_50ns_BDT.weights.xml
wget https://raw.githubusercontent.com/HuguesBrun/cmssw/addTheElecIDMVAoutputCSA14/EgammaAnalysis/ElectronTools/data/CSA14/TrigIDMVA_25ns_EB_BDT.weights.xml
wget https://raw.githubusercontent.com/HuguesBrun/cmssw/addTheElecIDMVAoutputCSA14/EgammaAnalysis/ElectronTools/data/CSA14/TrigIDMVA_25ns_EE_BDT.weights.xml
wget https://raw.githubusercontent.com/HuguesBrun/cmssw/addTheElecIDMVAoutputCSA14/EgammaAnalysis/ElectronTools/data/CSA14/TrigIDMVA_50ns_EB_BDT.weights.xml
wget https://raw.githubusercontent.com/HuguesBrun/cmssw/addTheElecIDMVAoutputCSA14/EgammaAnalysis/ElectronTools/data/CSA14/TrigIDMVA_50ns_EE_BDT.weights.xml

cd $OLDPWD
