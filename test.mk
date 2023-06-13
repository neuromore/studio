
include ./deps/build/make/platforms/detect-host.mk

dist:
	echo test

.PHONY : dist
.DEFAULT_GOAL := dist
