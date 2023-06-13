
include ./deps/build/make/platforms/detect-host.mk

dist:
	-rmdir /?
	-rmdir --help
	echo test
	-$(call mkdir,test)
	dir
	-$(call rmdir,test)
	dir
	
.PHONY : dist
.DEFAULT_GOAL := dist
