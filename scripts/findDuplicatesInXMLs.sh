#!/usr/bin/env bash
#
# Find duplicates ntuple files in XMLs
# Offers some advice about which one is bigger, which has event count
# Usage:
#
# ./findDuplicatesInXMLs.sh <directory with XMLs>

INDIR="$1"

if [ -z "$INDIR" ]; then
    echo "You need to specify an XML directory"
    exit 1
fi

if [[ -z $(find "$INDIR" -name '*.xml') ]]; then
    echo "No XML files found, exiting"
    exit 0
fi

OFFSET="   "

# Store filename pairs already processed so no need to do both comparisons (A vs B & B vs A)
declare -a ALREADYDONE;


function storedPair() {
    local search=$1
    for elem in "${ALREADYDONE[@]}"
    do
        if [ $search == $elem ]; then
            return 0
        fi
    done
    return 1
}


for xf in $INDIR/*.xml; do
    echo "Checking $xf"
    # Go through all pairwise matches (obviously not with itself though)
    for cf in $INDIR/*.xml; do
        if [[ "$cf" == "$xf" ]]; then
            continue
        fi

        # Speed up things by checking for already done pairs
        # Store as A:B, so check for B:A
        storedPair "$cf:$xf"
        HASPAIR=$?

        if (( $HASPAIR == 0 )); then
            continue
        fi

        ALREADYDONE+=("$xf:$cf")  # Store this pair of files

        # Get number of lines in common between the two files
        # (comm is magic, but requires sorted input files)
        NUMCOMMON=$(comm -12 <(sort "$xf") <(sort "$cf") | wc -l)

        if (( $NUMCOMMON > 0 )); then
            echo "$OFFSET**** $cf has common lines"

            # Check existence of NumberEntries
            # Note that 0 means it does have it, 1 means it does not
            grep -q "NumberEntries" "$xf"
            XFNOCOUNT=$?
            grep -q "NumberEntries" "$cf"
            CFNOCOUNT=$?

            if (( XFNOCOUNT == 0 )) && (( CFNOCOUNT == 1 )); then
                echo "$OFFSET $OFFSET $xf has NumberEntries line, the other one doesn't"
            elif (( XFNOCOUNT == 1 )) && (( CFNOCOUNT == 0 )); then
                echo "$OFFSET $OFFSET $cf has NumberEntries line, the other one doesn't"
            elif (( XFNOCOUNT == 0 )) && (( CFNOCOUNT == 0 )); then
                echo "$OFFSET $OFFSET Both files have NumberEntries count, check which is greater/correct"
            elif (( XFNOCOUNT == 1 )) && (( CFNOCOUNT == 1 )); then
                echo "$OFFSET $OFFSET Neither file has NumberEntries count!"
            fi

            # Check # ROOT files in XML
            XFNUM=$(grep -c ".root" "$xf")
            CFNUM=$(grep -c ".root" "$cf")
            if (( XFNUM > CFNUM )); then
                echo "$OFFSET $OFFSET $xf has more Ntuples in it"
            elif (( CFNUM > XFNUM )); then
                echo "$OFFSET $OFFSET $cf has more Ntuples in it"
            elif (( CFNUM == XFNUM )); then
                echo "$OFFSET $OFFSET Both files have same number of Ntuples"
            fi
        fi
    done
done