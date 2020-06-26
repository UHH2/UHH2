#!/bin/bash -e
#
# Convert XML list of ntuples to plain text list
#
# Usage:
#   ./xmlToTxt.sh <XML input filename> <txt output filename>
#

set -u

XML="$1"
TXT="$2"

if [ -z $XML ]; then
    echo "Missing XML filename"
    exit 1
fi

if [ -z $TXT ]; then
    echo "Missing output filename"
    exit 1
fi

if [ -f $TXT ]; then
    rm $TXT
fi

while IFS= read -r line
do
    if [[ $line =~ "<!--" ]]; then
        continue
    fi
    if [[ $line == *".root"* ]]; then
        filename=${line#*/pnfs}
        filename=${filename%\" Lumi=\"0.0\"/>}
        filename="/pnfs$filename"
        echo $filename >> $TXT
    fi
done < "$XML"

echo "Written txt file $TXT"
