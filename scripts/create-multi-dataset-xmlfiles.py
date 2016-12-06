#!/usr/bin/env python

import os, sys, glob, ROOT

if len(sys.argv) < 2:
   print "Usage:\n" \
         "    ./%s <rootfile pattern with full path> " \
         "<optional: xml output file POSTFIX> \n" \
         "    (it's a good idea to try this script in a test directory)" \
         % os.path.basename(sys.argv[0])
   sys.exit(1)

if len(sys.argv) >= 3:
    postfix = '_' + sys.argv[2]
else:
    postfix = ''

# glob files
pattern = sys.argv[1]
files = glob.glob(pattern)
print "Found %d files matching pattern" % len(files)
files.sort()

def get_basename(filename):
    if filename.startswith('/pnfs/'):       # use crab-project name
        return filename.split('/')[-4]

    #remove jobnumber and 'Ntuple' and make a set
    filename = os.path.splitext(filename)[0]
    filename = os.path.basename(filename)
    tokens = filename.split('_')
    if tokens[-1] == 'Ntuple':              # grid control
        tokens = tokens[:-2]
    else:                                   # sframe batch
        tokens = tokens[:-1]
    basename = '_'.join(tokens)
    return basename

### find common element in filenames


basenames = sorted(set(get_basename(name) for name in files))
print "Found these basenames: %s " % basenames

if basenames == ['']:
    print 'No basenames found. Did you give the pattern in the correct way? '
    print 'E.g. "my/path/*" (with double quotes)'
    exit(-1)

# write out
entity_template = """\
<!ENTITY %s SYSTEM  "../../common/datasets/%s.xml">
"""

conf_template = """
<InputData Lumi="%d" NEventsMax="-1" Type="MC" Version="%s" Cacheable="True">
    &%s;
    <InputTree Name="AnalysisTree" />
    <OutputTree Name="AnalysisTree" />
</InputData>
"""

entity_lines = []
config_lines = []
for basename in basenames:
    n_events = 0
    with open(basename + postfix + '.xml', 'w') as out:
        for fname in files:
            if get_basename(fname) != basename:
                continue
            out.write('<In FileName="%s" Lumi="0.0"/>\n' % fname)

            f = ROOT.TFile(fname)
            n_events += f.Get('AnalysisTree').GetEntriesFast()
            f.Close()

        out.write('<!-- number of events: %d -->\n' % n_events)
        entity_lines.append(entity_template % (basename, basename))
        config_lines.append(conf_template % (n_events, basename, basename))
        print '%s: %d events' % (basename, n_events)


with open('TEMPLATE_CONFIG%s.xml' % postfix, 'w') as out:
    out.writelines(entity_lines)
    out.writelines(config_lines)

