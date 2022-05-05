# Platforms directory
PLATDIR = $(dir $(lastword $(MAKEFILE_LIST)))

# Include shared for host os
include $(PLATDIR)/osx-all.mk

# Generic
EXTBIN     = 
EXTLIB     = .a
EXTDLL     = .dylib
EXTPDB     = .pdb
OBJDIR     = obj/osx-arm-$(MODE)
LIBDIR     = lib/osx-arm
BINDIR     = bin/osx-arm
TARGET     = armv7-a-apple-darwin19.6.0
MINVER     = -mmacosx-version-min=10.15
CPUFLAGS   = -march=armv7a -mfloat-abi=softfp -mfpu=neon-fp16
DEFINES    = -D__ARM_NEON
INCLUDES   = 
CXX        = clang++
CXXFLAGS   = $(MINVER) -target $(TARGET) -isysroot $(shell xcrun --sdk macosx --show-sdk-path)
CC         = clang
CFLAGS     = $(MINVER) -target $(TARGET) -isysroot $(shell xcrun --sdk macosx --show-sdk-path)
AR         = ar
ARFLAGS    = rcs
STRIP      = strip
STRIPFLAGS = -s
LINK       = $(CXX)
LINKFLAGS  = -target $(TARGET) -isysroot $(shell xcrun --sdk macosx --show-sdk-path)
LINKPATH   = -L$(LIBDIR) -L$(PLATDIR)/../../../prebuilt/osx/arm
LINKLIBS   = 

# Debug vs. Release
ifeq ($(MODE),release)
DEFINES   := $(DEFINES) -DNDEBUG
CXXFLAGS  := $(CXXFLAGS) -flto -O3 -g -ffunction-sections -fdata-sections
CFLAGS    := $(CFLAGS) -flto -O3 -g -ffunction-sections -fdata-sections
LINKFLAGS := $(LINKFLAGS) -flto -g -dead_strip
else
DEFINES   := $(DEFINES) -D_DEBUG
CXXFLAGS  := $(CXXFLAGS) -Og -g3
CFLAGS    := $(CFLAGS) -Og -g3
LINKFLAGS := $(LINKFLAGS) -g3
endif
