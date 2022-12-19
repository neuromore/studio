
include ./deps/build/make/platforms/detect-host.mk

##################################################################################
# ALL PLATFORMS
##################################################################################

QtTools:
	@echo [BLD] QtTools
	+@make -s -C ./deps/ -f Makefile qt-tools

Dependencies:
	@echo [BLD] Dependencies
	+@make -s -C ./deps/ -f Makefile -j24

Dependencies-engine:
	@echo [BLD] Dependencies Engine
	+@make -s -C ./deps/ -f Makefile engine-dependencies

Dependencies-clean:
	@echo [CLN] Dependencies
	+@make -s -C ./deps/ -f Makefile clean

Engine:
	@echo [BLD] Engine
	+@make -s -C ./build/make/ -f Engine.mk -j24

Engine-clean:
	@echo [CLN] Engine
	+@make -s -C ./build/make/ -f Engine.mk clean

EngineLIB:
	@echo [BLD] EngineLIB
	+@make -s -C ./build/make/ -f EngineLIB.mk

EngineLIB-clean:
	@echo [CLN] EngineLIB
	+@make -s -C ./build/make/ -f EngineLIB.mk clean

EngineJNI:
	@echo [BLD] EngineJNI
	+@make -s -C ./build/make/ -f EngineJNI.mk

EngineJNI-clean:
	@echo [CLN] EngineJNI
	+@make -s -C ./build/make/ -f EngineJNI.mk clean

QtBase:
	@echo [BLD] QtBase
	+@make -s -C ./build/make/ -f QtBase.mk -j24

QtBase-clean:
	@echo [CLN] QtBase
	+@make -s -C ./build/make/ -f QtBase.mk clean

Studio:
	@echo [BLD] Studio
	+@make -s -C ./build/make/ -f Studio.mk -j8

Studio-clean:
	@echo [CLN] Studio
	+@make -s -C ./build/make/ -f Studio.mk clean

Studio-dist:
	@echo [DST] Studio
	+@make -s -C ./build/make/ -f Studio.mk dist

##################################################################################

all: Engine EngineLIB QtBase Studio 
clean: Engine-clean EngineLIB-clean QtBase-clean Studio-clean 
dist: Studio-dist

##################################################################################

.PHONY : all
.DEFAULT_GOAL := all
