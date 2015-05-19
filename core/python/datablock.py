#!/usr/bin/env python

class datablock():
    def __init__(self, blocklumi="1", filelist=[], lumilist=[], namelist=[], type="DATA", version="DATA", neventsmax="-1", cacheable="False"):
        self.blocklumi = blocklumi
        self.filelist = filelist
        self.lumilist = lumilist
        self.namelist = namelist
        self.type = type
        self.version = version
        self.neventsmax = neventsmax
        self.cacheable = cacheable

    def setblocklumi(self, blocklumi):
        self.blocklumi = blocklumi

    def setfilelist(self, filelist):
        self.filelist = filelist

    def setlumilist(self, lumilist):
        self.lumilist = lumilist

    def setnamelist(self, namelist):
        self.namelist = namelist

    def settype(self, type):
        self.type = type

    def setversion(self, version):
        self.version = versions

    def setneventsmax(self, inputmaxevents):
        self.neventsmax = inputmaxevents

    def setcacheable(self, cacheable):
        self.cacheable = cacheable
