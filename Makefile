
include ./deps/build/make/platforms/detect-host.mk

##################################################################################
# ALL PLATFORMS
##################################################################################

Dependencies:
	@echo [BLD] Dependencies
	@make -s -C ./deps/ -f Makefile

Dependencies-clean:
	@echo [CLN] Dependencies
	@make -s -C ./deps/ -f Makefile clean

Engine:
	@echo [BLD] Engine
	@make -s -C ./build/make/ -f Engine.mk -j $(DETECTED_CORES)

Engine-clean:
	@echo [CLN] Engine
	@make -s -C ./build/make/ -f Engine.mk clean -j $(DETECTED_CORES)

EngineJNI:
	@echo [BLD] EngineJNI
	@make -s -C ./build/make/ -f EngineJNI.mk -j $(DETECTED_CORES)

EngineJNI-clean:
	@echo [CLN] EngineJNI
	@make -s -C ./build/make/ -f EngineJNI.mk clean -j $(DETECTED_CORES)

QtBase:
	@echo [BLD] QtBase
	@make -s -C ./build/make/ -f QtBase.mk -j $(DETECTED_CORES)

QtBase-clean:
	@echo [CLN] QtBase
	@make -s -C ./build/make/ -f QtBase.mk clean -j $(DETECTED_CORES)

Studio:
	@echo [BLD] Studio
	@make -s -C ./build/make/ -f Studio.mk -j $(DETECTED_CORES)

Studio-clean:
	@echo [CLN] Studio
	@make -s -C ./build/make/ -f Studio.mk clean -j $(DETECTED_CORES)

##################################################################################

all: Engine QtBase Studio 
clean: Engine-clean QtBase-clean Studio-clean 

##################################################################################

.PHONY : all
.DEFAULT_GOAL := all
