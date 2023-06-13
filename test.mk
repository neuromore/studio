
include ./deps/build/make/platforms/detect-host.mk

dist:
	echo test
	-$(call mkdir,test)
	dir
	-$(call deletefiles,test,*.zip)
	-$(call rmdir,test)
	dir
	
.PHONY : dist
.DEFAULT_GOAL := dist
