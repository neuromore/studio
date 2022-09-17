
include ./deps/build/make/platforms/detect-host.mk

##################################################################################
# ALL PLATFORMS
##################################################################################

QtTools:
	@echo [BLD] QtTools
	+@make -s -C ./deps/ -f Makefile qt-tools

Dependencies:
	@echo [BLD] Dependencies
	+@make -s -C ./deps/ -f Makefile

Dependencies-clean:
	@echo [CLN] Dependencies
	+@make -s -C ./deps/ -f Makefile clean

Engine:
	@echo [BLD] Engine
	+@make -s -C ./build/make/ -f Engine.mk

Engine-clean:
	@echo [CLN] Engine
	+@make -s -C ./build/make/ -f Engine.mk clean

EngineJNI:
	@echo [BLD] EngineJNI
	+@make -s -C ./build/make/ -f EngineJNI.mk

EngineJNI-clean:
	@echo [CLN] EngineJNI
	+@make -s -C ./build/make/ -f EngineJNI.mk clean

QtBase:
	@echo [BLD] QtBase
	+@make -s -C ./build/make/ -f QtBase.mk

QtBase-clean:
	@echo [CLN] QtBase
	+@make -s -C ./build/make/ -f QtBase.mk clean

Studio: Engine QtBase
	@echo [BLD] Studio
	+@make -s -C ./build/make/ -f Studio.mk

Studio-clean:
	@echo [CLN] Studio
	+@make -s -C ./build/make/ -f Studio.mk clean

Studio-dist:
	@echo [DST] Studio
	+@make -s -C ./build/make/ -f Studio.mk dist

##################################################################################

all: Engine QtBase Studio 
clean: Engine-clean QtBase-clean Studio-clean 
dist: Studio-dist

##################################################################################

.PHONY : all
.DEFAULT_GOAL := all
