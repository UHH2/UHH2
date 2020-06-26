#!/bin/bash -e

set -u

# Split a Golden JSON into the run periods, automatically downloads it
# 
# Usage:
#
#  ./splitGoldenJSONbyRunPeriod.sh <year>
#  
#   where <year> is one of 2016, 2017, 2018

YEAR=$1

if [ -z $YEAR ]; then
    echo "Missing YEAR argument"
    exit 1
fi

URL=""
# Load up run number periods into associative array
# Fake multidimensionality by using | to separate start/end run numbers
declare -A run_periods;
if [[ $YEAR == "2016" ]]; then
    run_periods["B"]="272007|275376"
    run_periods["C"]="275657|276283"
    run_periods["D"]="276315|276811"
    run_periods["E"]="276831|277420"
    run_periods["F"]="277772|278808"
    run_periods["G"]="278820|280385"
    run_periods["H"]="280919|284044"
    URL="https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions16/13TeV/ReReco/Final/Cert_271036-284044_13TeV_ReReco_07Aug2017_Collisions16_JSON.txt"
elif [[ $YEAR == "2017" ]]; then
    # A has no lumi in Golden JSON
    run_periods["B"]="297046|299329"
    run_periods["C"]="299368|302029"
    run_periods["D"]="302030|303434"
    run_periods["E"]="303824|304797"
    run_periods["F"]="305040|306462"
    # this "v1" has an extra bad ECAL LS removed: https://hypernews.cern.ch/HyperNews/CMS/get/physics-validation/3067.html
    URL="https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions17/13TeV/ReReco/Cert_294927-306462_13TeV_EOY2017ReReco_Collisions17_JSON_v1.txt"
elif [[ $YEAR == "2018" ]]; then
    run_periods["A"]="315252|316995"
    run_periods["B"]="316998|319312"
    run_periods["C"]="319313|320393"
    run_periods["D"]="320394|325273"
    # has a few extra LS wrt prompt JSON
    URL="https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions18/13TeV/ReReco/Cert_314472-325175_13TeV_17SeptEarlyReReco2018ABC_PromptEraD_Collisions18_JSON.txt"
else
    echo "bad YEAR argument, should be one of 2016, 2017, 2018"
    exit 1
fi

# Download the golden json
JSON="Golden_$YEAR.json"
wget -O "$JSON" "$URL"
# Split it by run period
for period in "${!run_periods[@]}"; do
    startend=${run_periods[$period]}
    START=${startend%|*}
    END=${startend#*|}
    echo $period $START $END
    filterJSON.py --min $START --max $END "$JSON" --output "Golden_${YEAR}_Run${period}.json"
done
