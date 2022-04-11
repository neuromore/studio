# Platforms directory
PLATDIR = $(dir $(lastword $(MAKEFILE_LIST)))

# Include shared for host os
include $(PLATDIR)/osx-all.mk

# Generic
EXTBIN     = 
EXTLIB     = .a
EXTDLL     = .dylib
EXTPDB     = .pdb
OBJDIR     = obj/osx-arm64-$(MODE)
LIBDIR     = lib/osx-arm64
BINDIR     = bin/osx-arm64
TARGET     = arm64-apple-darwin19.6.0
MINVER     = -mmacosx-version-min=10.15
CPUFLAGS   = -march=armv8-a
DEFINES    = 
INCLUDES   = 
CXX        = clang++
CXXFLAGS   = $(MINVER) -target $(TARGET) -isysroot $(shell xcrun --sdk macosx --show-sdk-path) -static
CC         = clang
CFLAGS     = $(MINVER) -target $(TARGET) -isysroot $(shell xcrun --sdk macosx --show-sdk-path) -static
AR         = ar
ARFLAGS    = rcs
LINK       = $(CXX)
LINKFLAGS  = -target $(TARGET) -isysroot $(shell xcrun --sdk macosx --show-sdk-path)
LINKPATH   = -L$(LIBDIR) -L$(PLATDIR)/../../../prebuilt/osx/arm64
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
