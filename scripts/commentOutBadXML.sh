#!/bin/bash -e
#
# Create a copy of an XML file,
# commenting out any filepath in a list of "bad" files
#
# Usage:
#
#   ./comment_out_bad_XML.sh <XML filename> <txt file with list>
#

set -u

XML="$1"
BADLIST="$2"

if [ -z $XML ]; then
    echo "Missing XML filename"
    exit 1
fi

if [ -z $BADLIST ]; then
    echo "Missing bad list filename"
    exit 1
fi

newfile=${XML/.xml/_nobad.xml}
if [ -f $newfile ]; then
    rm $newfile
fi

while IFS= read -r line
do
    filename=${line#*/pnfs}
    filename=${filename%\" Lumi=\"0.0\"/>}
    filename="/pnfs$filename"
    filename=${filename//\/\//\/}  # convert // to /, assumes BADLIST doesn't have them
    if grep -q "$filename" "$BADLIST"; then
        echo "Found bad $filename"
        echo "<!-- BAD $line -->" >> $newfile
    else
        echo $line >> $newfile
    fi
done < "$XML"

echo "Written updated file $newfile"
