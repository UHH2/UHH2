LIBRARY := SUHH2common
DICT :=  include/ReconstructionHypothesis.h include/SUHH2common_LinkDef.h
LHAPDFINC=$(shell scram tool tag lhapdf INCLUDE)
LHAPDFLIB=$(shell scram tool tag lhapdf LIBDIR)
USERCXXFLAGS := -I${LHAPDFINC}
TEST := 1
USERLDFLAGS := -lSUHH2core -lSUHH2JetMETObjects -lMinuit -lGenVector -L${LHAPDFLIB} -lLHAPDF
include ../Makefile.common
