#!/usr/bin/env python

# create .dbs file from filename pattern which can be used as input dataset
# for grid-control. Works for root files and for lhe files. For both local files
# and files on (desy) dcache)
#
# Usage: create-dbs.py <path to files> [filename pattern] [name]
#
# filename pattern defaults to "*.root"
# name defaults to the pattern with "*.root" removed; if this would be empty, the last component of the path is used.

import ROOT
import sys, glob, os.path, subprocess, fnmatch, signal, threading, re

ROOT.gROOT.SetBatch(True)
ROOT.gErrorIgnoreLevel = ROOT.kError

if len(sys.argv) < 2: raise RuntimeError, "expected more arguments"
if len(sys.argv) > 4: raise RuntimeError, "expected fewer arguments (unintended pattern expansion?)"

path = sys.argv[1]
pattern = "*.root"
name = None

if len(sys.argv) > 2: pattern = sys.argv[2]
if len(sys.argv) > 3: name = sys.argv[3]


def cleanup_name(s):
    s = s.replace('*', '')
    s = s.replace('/', '')
    return s

if name is None:
   full_pattern = os.path.join(path, pattern)
   # use last '/' to '.root' at the end of the pattern:
   p0 = full_pattern.rfind('/')
   p1 = full_pattern.rfind('.')
   if p0==-1 or p1==-1: raise RuntimeError, "Could not determine name from the pattern %s; please specify name explicitly" % full_pattern
   s = full_pattern[p0:p1]
   s = cleanup_name(s)
   if s == '': # use last path name:
       p1 = p0
       p0 = full_pattern.rfind('/', 0, p1)
       if p0 == -1: raise RuntimeError, "Could not determine name from the pattern %s; please specify name explicitly" % full_pattern
       s = cleanup_name(full_pattern[p0:p1])
   name = s
   print "Using name='%s'" % name


# generate a prefix and file names from a path and pattern (NOTE:
# another implementation of the same interface could be added for dcache later ...).
class files:
    def __init__(self, path, pattern):
        if path.startswith('/pnfs/desy.de/') and not os.path.exists(path):
            self.mode = 'dcap'
            self.path = path.rstrip('/') + '/'
        else:
            self.mode = 'local'
            self.path = os.path.realpath(path)
        self.pattern = pattern

    # prefix as used for CMSSW for the short filenames (as returned by list)
    def prefix(self):
        if self.mode == 'local':
            if self.path.startswith('/pnfs/desy.de'):
                return 'dcap://dcache-cms-dcap.desy.de:22125' + self.path
            else:
                return 'file:' + self.path
        else: return 'dcap://dcache-cms-dcap.desy.de:22125' + self.path

    # returns a list of tuples (short name, full name) where short name is relative to the prefix
    # and full name is the full path name, as understood by ROOT
    def list(self):
        if self.mode == 'local':
            expr = os.path.join(self.path, self.pattern)
            file_list = [f for f in glob.glob(expr)]
            n_files = len(file_list)
            file_list = list(set(file_list))
            n_files_after = len(file_list)
            if n_files != n_files_after: # yes, it happened!
                print '*** INTERNAL OS ERROR: listed filename more than once! ***'
                sys.exit(1)
            return [(os.path.basename(f), f) for f in file_list]
        elif self.mode == 'dcap':
            offset = 0
            filenames = []
            while True:
                commands = ['lcg-ls', '-c', '999', '-o', str(offset), 'srm://dcache-se-cms.desy.de:8443' + self.path]
                p = subprocess.Popen(commands, stdout=subprocess.PIPE, stderr = subprocess.PIPE)
                out, err = p.communicate()
                if p.returncode != 0:
                    print "Exit code of '%s' was %d, output:\n%s" % (' '.join(commands), p.returncode, out + err)
                    raise RuntimeError, "error from lcg-ls (output see above)"
                new_files = 0
                for line in out.split('\n'):
                    line = line.strip()
                    if line == '': continue
                    if line.endswith('/'): continue
                    if not line.startswith('/pnfs'):
                        print "WARNING: skipping line from lcg-ls output: ", line
                        continue
                    filenames.append(os.path.basename(line))
                    new_files += 1
                if new_files == 0: break
                offset += 999
            print "lcg-ls found total of %d files in that directory" % len(filenames)
            result = []
            for f in filenames:
                f = f.strip()
                if f == '': continue
                if fnmatch.fnmatch(f, self.pattern):
                    result.append((f, self.prefix() + f))
            return result

def nevents(filename):
    # 1. lhe files:
    if filename.endswith('.lhe'):
        n = 0
        for l in file(filename):
            if l.startswith("<event>"): n+=1
        return n
    # 2.: root files with a TTree
    # algorithm: try to find a TTree which is either unique or its names is in the hard-coded list here
    use_ttree_names = ('Events',)
    rf = ROOT.TFile.Open(filename, 'read')
    if not rf.IsOpen():
        raise RuntimeError, "error opening root file '%s'" % filename
        
    def find_tree_key(rootfile):
        n_matching_ttrees = 0 # used later to determine whether it's unique.
        ttree_key = None
        for key in rootfile.GetListOfKeys():
            #print "found key '%s', class: '%s'" % (key.GetName(), key.GetClassName())
            if key.GetClassName() != 'TTree': continue
            if key.GetName() in use_ttree_names:
                ttree_key = key
                n_matching_ttrees = 1 # declare it unique
                break
            else:
                ttree_key = key
                n_matching_ttrees += 1
        if ttree_key is None:
            raise RuntimeError, "Did not find any TTree in file '%s'" % filename
        if n_matching_ttrees > 1:
            raise RuntimeError, "Did find several TTrees in file '%s'; please modify 'use_ttree_names' in the 'nevents' method to specify which one to use" % filename
        return ttree_key
    ttree_key = find_tree_key(rf)
    n = ttree_key.ReadObj().GetEntries()
    rf.Close()
    del rf
    return n


number_pattern = re.compile('_(\d+)_')
def cmp_number(s1, s2):
    m1 = number_pattern.search(s1)
    if m1 is not None:
        m2 = number_pattern.search(s2)
        return cmp(int(m1.group(1)), int(m2.group(1)))
    return cmp(s1, s2)


#signal.signal(signal.SIGALRM, handle_alarm)
f = files(path, pattern)
filenames = f.list()
nfiles = len(filenames)
print "Found %d files matching pattern" % nfiles

filenames.sort(cmp = lambda f1, f2: cmp_number(f1[0], f2[0]))
# cross check:
nfiles = len(filenames)
m_first = number_pattern.search(filenames[0][0])
m_last = number_pattern.search(filenames[-1][0])
if m_first is not None and m_last is not None:
    nfiles_name = int(m_last.group(1)) - int(m_first.group(1)) + 1
    print "found nfiles = %d; filenames suggest nfiles = %d (first: %s, last: %s)" % (nfiles, nfiles_name, filenames[0][0], filenames[-1][0])
    if nfiles != nfiles_name:
       print "WARNING: inconsistent number of files"
       #for f in filenames: print f[0]
       #raise RuntimeError, "nfiles cross-check failed"

# open output .dbs file:
outfile = open('%s.dbs' % name, 'w')
outfile.write("[%s]\n" % name)
outfile.write("prefix = %s\n" % f.prefix())

ntot = 0
ifile = 0
for short_name, full_name in filenames:
    ifile += 1
    print "[%5d / %5d] %s ..." % (ifile, nfiles, short_name),
    sys.stdout.flush()
    n = nevents(full_name)
    print ""
    ntot += n
    outfile.write("%s = %d\n" % (short_name, n))
    outfile.flush()
outfile.close()
print "Found %d events in %d files" % (ntot, ifile)
