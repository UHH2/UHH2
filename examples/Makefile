LIBRARY := SUHH2examples
LHAPDFLIB=$(shell scram tool tag LHAPDF LIBDIR)
USERLDFLAGS := -lSUHH2core -lSUHH2common -lGenVector -lSUHH2JetMETObjects -L${LHAPDFLIB} -lLHAPDF
# enable par creation; this is necessary for all packages containing AnalysisModules
# to be loaded from by AnalysisModuleRunner.
PAR := 1
include ../Makefile.common
