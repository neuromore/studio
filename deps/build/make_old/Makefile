#
# This Makefile detects operating system, architecture and cpu cores
# and executes the appropriate Makefile automatically
#

include Makefile.detect

.DEFAULT_GOAL := all

all:
	make all-$(DETECTED_ARCH) -f Makefile.$(DETECTED_OS) -j $(DETECTED_CORES)

clean:
	make clean-$(DETECTED_ARCH) -f Makefile.$(DETECTED_OS) -j $(DETECTED_CORES)
