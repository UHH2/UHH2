LIBRARY := SUHH2LwtnnExample
LHAPDFINC=$(shell scram tool tag lhapdf INCLUDE)
LHAPDFLIB=$(shell scram tool tag LHAPDF LIBDIR)
LWTNNLIB= $(shell scram tool tag lwtnn LIBDIR)
LWTNNINC= $(shell scram tool tag lwtnn INCLUDE)
USERCXXFLAGS := -I${LHAPDFINC} -I${LWTNNINC}
USERLDFLAGS := -lSUHH2core -lSUHH2common -lGenVector -lSUHH2JetMETObjects -L${LHAPDFLIB} -L${LWTNNLIB} -lLHAPDF -llwtnn
# enable par creation; this is necessary for all packages containing AnalysisModules
# to be loaded from by AnalysisModuleRunner.
PAR := 1
include ../Makefile.common
