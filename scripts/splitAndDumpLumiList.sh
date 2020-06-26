#!/bin/bash -e

set -u

# Divide up txt list of files into separate files
# that are shorter, and run dump_lumilist.C on each.
# Then combine the resultant lumilists to make a grand one.

TXT="$1"
JSON="$2"


if [[ "$TXT" != *".txt" ]]; then
    echo "text file should end .txt"
    exit 1
fi

PREFIX="${TXT/.txt/_part.txt}"
# 1000 lines should take about 10 minutes
split -d --lines=1000 "$TXT" "$PREFIX"
declare -a jsons;
for f in $PREFIX*; do
    IND="${f#$PREFIX}"
    JSONPART="${JSON/.json/$IND.json}"
    jsons+=($JSONPART)
    echo $f $JSONPART
    nohup nice -n 10 root -q -b -l 'dump_lumilist.C+("'${f}'","'${JSONPART}'")' > dump_${f}.log &
    sleep 10 # to allow it to compile once, instead of parallel compilations that fail
done
echo "Once finished, run: "
echo "mergeJSON.py ${jsons[@]} --output $JSON"

