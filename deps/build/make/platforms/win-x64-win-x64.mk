# Platforms directory
PLATDIR = $(dir $(lastword $(MAKEFILE_LIST)))

# Include shared for host os
include $(PLATDIR)/win-all.mk

# Generic
EXTBIN     = .exe
EXTLIB     = .lib
EXTDLL     = .dll
EXTPDB     = .pdb
OBJDIR     = obj/win-x64-$(MODE)
LIBDIR     = lib/win-x64
BINDIR     = bin/win-x64
TARGET     = x86_64-pc-windows-msvc
CPUFLAGS   = -msse -msse2 -msse3 -mssse3 -msse4.1 -msse4.2 -mpclmul
DEFINES    = -DWIN32 -DWIN64
INCLUDES   = 
CXX        = clang++
CXXFLAGS   = -target $(TARGET) -static
CC         = clang
CFLAGS     = -target $(TARGET) -static
AR         = llvm-ar
ARFLAGS    = rcs
LINK       = $(CXX)
LINKFLAGS  = -target $(TARGET) -fuse-ld=lld -Xlinker /MACHINE:X64
LINKPATH   = -L$(LIBDIR) -L$(PLATDIR)/../../../prebuilt/win/x64
LINKLIBS   = 

# MSVC Resource Compiler
RC         = llvm-rc
RCFLAGS    = /l 0x0409 /nologo

# Debug vs. Release
ifeq ($(MODE),release)
DEFINES   := $(DEFINES) -DNDEBUG
CXXFLAGS  := $(CXXFLAGS) -flto -O3 -g -ffunction-sections -fdata-sections
CFLAGS    := $(CFLAGS) -flto -O3 -g -ffunction-sections -fdata-sections
LINKFLAGS := $(LINKFLAGS) -flto -g -Xlinker /OPT:ref -Xlinker /OPT:icf
LINKLIBS  := $(LINKLIBS)
else
DEFINES   := $(DEFINES) -D_DEBUG
CXXFLAGS  := $(CXXFLAGS) -Og -g3
CFLAGS    := $(CFLAGS) -Og -g3
LINKFLAGS := $(LINKFLAGS) -g3 -MTd
LINKLIBS  := $(LINKLIBS) -lDbgHelp.lib -lmsvcrtd.lib
endif
