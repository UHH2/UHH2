# This just executes 'make' in all the subdirectories. Note the dependencies:
# core comes first, then NtupleWriter, then common, then all other packages.
dirs := core common examples


# Makefile.local can be used to append more directories to be built to 'dirs'
-include Makefile.local

all: subdirs

subdirs:
	@for d in $(dirs); do ( $(MAKE) -C $$d ) || break; done

clean:
	@for d in $(dirs); do ( $(MAKE) -C $$d clean ) || break; done
