# Platforms directory
PLATDIR = $(dir $(lastword $(MAKEFILE_LIST)))

# Include shared for host os
include $(PLATDIR)/osx-all.mk

# Generic
EXTBIN     = 
EXTLIB     = .a
EXTDLL     = .dylib
EXTPDB     = .pdb
OBJDIR     = obj/osx-x64-$(MODE)
LIBDIR     = lib/osx-x64
BINDIR     = bin/osx-x64
TARGET     = x86_64-apple-darwin19.6.0
MINVER     = -mmacosx-version-min=10.15
CPUFLAGS   = -march=x86-64 -mtune=generic -msse -msse2 -msse3 -mssse3 -msse4.1 -msse4.2 -mpclmul
DEFINES    = 
INCLUDES   = 
CXX        = clang++
CXXFLAGS   = $(MINVER) -target $(TARGET) -static
CC         = clang
CFLAGS     = $(MINVER) -target $(TARGET) -static
AR         = ar
ARFLAGS    = rcs
LINK       = $(CXX)
LINKFLAGS  = -target $(TARGET)
LINKPATH   = -L$(LIBDIR) -L$(PLATDIR)/../../../prebuilt/osx/x64
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
