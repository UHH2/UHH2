LIBRARY := SUHH2core
DICT := include/AnalysisModuleRunner.h include/NtupleObjects.h include/SUHH2core_LinkDef.h

FJINC=$(shell scram tool tag FASTJET INCLUDE)
FJLIB=$(shell scram tool tag FASTJET LIBDIR)

USERLDFLAGS := -Wl,-rpath,${FJLIB} -lm -L${FJLIB} -lfastjettools -lfastjet -lHOTVR -lNsubjettiness -lRecursiveTools
USERCXXFLAGS := -I${FJINC}

TEST := 1
TESTPAR := 1
include ../Makefile.common
