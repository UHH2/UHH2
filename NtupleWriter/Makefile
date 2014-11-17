LIBRARY = SUHH2NtupleWriter
USERLDFLAGS = -lSUHH2core -lSFrameCintex -lCintex
DICT := $(wildcard include/*.h)
# move LinkDef to the very end:
DICT := $(filter-out include/SUHH2NtupleWriter_LinkDef.h,$(DICT)) include/SUHH2NtupleWriter_LinkDef.h
include ../Makefile.common
