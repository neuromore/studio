# Platforms directory
PLATDIR = $(dir $(lastword $(MAKEFILE_LIST)))

# Include shared for host os
include $(PLATDIR)/linux-all.mk

# Generic
EXTBIN     = 
EXTLIB     = .a
EXTDLL     = .so
EXTPDB     = .pdb
OBJDIR     = obj/linux-x64
LIBDIR     = lib/linux-x64
BINDIR     = bin/linux-x64
TARGET     = x86_64-linux-gnu
CPUFLAGS   = -msse -msse2 -msse3 -mssse3 -msse4.1 -msse4.2 -mpclmul
DEFINES    = 
INCLUDES   = 
CXX        = clang++
CXXFLAGS   = -target $(TARGET) -fPIC -static
CC         = clang
CFLAGS     = -target $(TARGET) -fPIC -static
AR         = llvm-ar
ARFLAGS    = rcs
LINK       = $(CXX)
LINKFLAGS  = -target $(TARGET) -fuse-ld=lld -static-libstdc++ -static-libgcc
LINKPATH   = -L$(LIBDIR) -L$(PLATDIR)/../../../prebuilt/linux/x64
LINKLIBS   = 

# Debug vs. Release
ifeq ($(MODE),release)
DEFINES   := $(DEFINES) -DNDEBUG
CXXFLAGS  := $(CXXFLAGS) -flto -O3 -g -ffunction-sections -fdata-sections
CFLAGS    := $(CFLAGS) -flto -O3 -g -ffunction-sections -fdata-sections
LINKFLAGS := $(LINKFLAGS) -flto -g -Wl,--gc-sections
else
DEFINES   := $(DEFINES) -D_DEBUG
CXXFLAGS  := $(CXXFLAGS) -Og -g3
CFLAGS    := $(CFLAGS) -Og -g3
LINKFLAGS := $(LINKFLAGS) -g3
endif
