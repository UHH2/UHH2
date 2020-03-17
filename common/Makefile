LIBRARY := SUHH2common
DICT :=  include/ReconstructionHypothesis.h include/SUHH2common_LinkDef.h
LHAPDFINC=$(shell scram tool tag lhapdf INCLUDE)
LHAPDFLIB=$(shell scram tool tag lhapdf LIBDIR)
TFLOWLIB= $(shell scram tool tag tensorflow LIBDIR)
TFLOWINC= $(shell scram tool tag tensorflow INCLUDE)
PBUFLIB= $(shell scram tool tag protobuf LIBDIR)
PBUFINC= $(shell scram tool tag protobuf INCLUDE)
EigenINC= $(shell scram tool tag Eigen INCLUDE)
USERCXXFLAGS := -I${LHAPDFINC} -I/cvmfs/cms.cern.ch/${SCRAM_ARCH}/cms/cmssw/${CMSSW_VERSION}/src -I${TFLOWINC} -I${PBUFINC} -I${EigenINC}
TEST := 1
USERLDFLAGS := -lSUHH2core -lSUHH2JetMETObjects -lMinuit -lGenVector -L${LHAPDFLIB} -lLHAPDF -L/cvmfs/cms.cern.ch/${SCRAM_ARCH}/cms/cmssw/${CMSSW_VERSION}/lib/${SCRAM_ARCH} -lPhysicsToolsTensorFlow -L${TFLOWLIB} -ltensorflow_framework -L${PBUFLIB}
include ../Makefile.common
