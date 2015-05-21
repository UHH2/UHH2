#!/usr/bin/env python

# prints total number of events in the given .dbs file.

import re, fileinput

r = re.compile(r"=\s+(\d+)\s*$")
start_r = re.compile(r"\[([^\]]+)]")

total = {}
dset_name = "<unspec>"
for line in fileinput.input():
        match_start = start_r.search(line)
        if match_start!=None:
                dset_name = match_start.group(1)
                total[dset_name] = 0
        match = r.search(line)
        if match==None: continue
        total[dset_name] += int(match.group(1))

for dset_name in sorted(total.keys()):
    print "%50s : %d" % (dset_name, total[dset_name])
print "%50s : %d" % ("Sum", sum([total[k] for k in total]))

