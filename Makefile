
include ./deps/build/make/platforms/detect-host.mk

# Use Detected Platform as Target by default

ifeq ($(TARGET_OS),)
TARGET_OS = $(DETECTED_OS)
endif

ifeq ($(TARGET_ARCH),)
TARGET_ARCH = $(DETECTED_ARCH)
endif

##################################################################################
# ALL PLATFORMS
##################################################################################

Dependencies:
	@make -s -C ./deps/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f Makefile

Dependencies-clean:
	@make -s -C ./deps/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f Makefile clean

Engine:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f Engine.mk -j $(DETECTED_CORES)

Engine-clean:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f Engine.mk clean -j $(DETECTED_CORES)

EngineJNI: Engine
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f EngineJNI.mk -j $(DETECTED_CORES)

EngineJNI-clean: Engine-clean
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f EngineJNI.mk clean -j $(DETECTED_CORES)

QtBase:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f QtBase.mk -j $(DETECTED_CORES)

QtBase-clean:
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f QtBase.mk clean -j $(DETECTED_CORES)

Studio: Engine QtBase
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f Studio.mk -j $(DETECTED_CORES)

Studio-clean: Engine-clean QtBase-clean
	@make -s -C ./build/make/ TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) -f Studio.mk clean -j $(DETECTED_CORES)

# combined
--base: Engine QtBase Studio 
--base-clean: Engine-clean QtBase-clean Studio-clean 

##################################################################################
# WINDOWS
##################################################################################
ifeq ($(TARGET_OS),win)

all: --base
clean: --base-clean

endif

##################################################################################
# OSX
##################################################################################
ifeq ($(TARGET_OS),osx)

all: --base
clean: --base-clean

endif

##################################################################################
# LINUX
##################################################################################
ifeq ($(TARGET_OS),linux)

all: --base
clean: --base-clean

endif

##################################################################################

.PHONY : all
.DEFAULT_GOAL := all
