# Make targets:
# * "sframe": build cdode for use with SFrame; this executes 'make' in all subdirectories defined in the "dirs" variable below
# * "scram": execute scram for building codf for CMSSW
# * "all": both scram and sframe
dirs := core JetMETObjects common examples

# Makefile.local can be used to append more directories to be built to 'dirs'
-include Makefile.local

# by default: build both the sframe code and invoke scram to build CMSSW part
all: sframe scram

sframe:
	@for d in $(dirs); do ( $(MAKE) -C $$d ) || exit 1; done

clean:
	@for d in $(dirs); do ( $(MAKE) -C $$d clean ) || exit 1; done

scram:
	@export MAKEFLAGS="$(MAKEFLAGS)"
	@cd $(CMSSW_BASE)
	@echo "Calling 'scram b' in $(CMSSW_BASE) with MAKEFLAGS=\"$(MAKEFLAGS)\""
	@# + = treat scram as sub-make; required for passing fds of make's jobserver fd in case
	@# of building with '-j N'.
	+@scram b

.PHONY: clean scram sframe
.DELETE_ON_ERRORS:
