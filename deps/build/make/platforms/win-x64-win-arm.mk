# Platforms directory
PLATDIR = $(dir $(lastword $(MAKEFILE_LIST)))

# Include shared for host os
include $(PLATDIR)/win-all.mk

# Generic
EXTBIN     = .exe
EXTLIB     = .lib
EXTDLL     = .dll
EXTPDB     = .pdb
OBJDIR     = obj/win-arm
LIBDIR     = lib/win-arm
BINDIR     = bin/win-arm
TARGET     = arm-pc-windows-msvc
CPUFLAGS   = -march=armv7-a -mfloat-abi=hard -mfpu=neon-fp16
DEFINES    =
INCLUDES   = 
CXX        = clang++
CXXFLAGS   = -target $(TARGET) -static
CC         = clang
CFLAGS     = -target $(TARGET) -static
AR         = llvm-ar
ARFLAGS    = rcs
LINK       = $(CXX)
LINKFLAGS  = -target $(TARGET) -fuse-ld=lld -Xlinker /MACHINE:ARM
LINKPATH   = -L$(LIBDIR) -L$(PLATDIR)/../../../prebuilt/win/arm
LINKLIBS   = 

# MSVC Resource Compiler
RC          = llvm-rc
RCFLAGS_X86 = /l 0x0409 /nologo
RCFLAGS_X64 = /l 0x0409 /nologo

# Debug vs. Release
ifeq ($(MODE),release)
DEFINES   := $(DEFINES) -DNDEBUG
CXXFLAGS  := $(CXXFLAGS) -flto -O3 -g -ffunction-sections -fdata-sections
CFLAGS    := $(CFLAGS) -flto -O3 -g -ffunction-sections -fdata-sections
LINKFLAGS := $(LINKFLAGS) -flto -g -Xlinker -MT /OPT:ref -Xlinker /OPT:icf
else
DEFINES   := $(DEFINES) -D_DEBUG
CXXFLAGS  := $(CXXFLAGS) -Og -g3
CFLAGS    := $(CFLAGS) -Og -g3
LINKFLAGS := $(LINKFLAGS) -g3 -MTd
endif
