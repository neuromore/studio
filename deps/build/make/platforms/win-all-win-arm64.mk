# Generic
EXTBIN     = .exe
EXTLIB     = .lib
EXTDLL     = .dll
EXTPDB     = .pdb
OBJDIR     = obj/win-arm64-$(MODE)
LIBDIR     = lib/win-arm64
BINDIR     = bin/win-arm64
DISTDIR    = ../../dist/win-10
TARGET     = aarch64-pc-windows-msvc
CPUFLAGS   = -march=armv8-a -mtune=generic
DEFINES    = -DWIN32 -DWIN64 -D_MT
INCLUDES   = 
CXX        = clang++
CXXFLAGS   = -target $(TARGET) -static
CC         = clang
CFLAGS     = -target $(TARGET) -static
AR         = llvm-ar
ARFLAGS    = rcs
STRIP      = llvm-strip
STRIPFLAGS = --strip-all
LINK       = $(CXX)
LINKFLAGS  = -target $(TARGET) -fuse-ld=lld -Xlinker /MACHINE:ARM64
LINKPATH   =
LINKLIBS   = 

# MSVC Resource Compiler
RC         = llvm-rc
RCFLAGS    = /l 0x0409 /nologo

# Debug vs. Release
ifeq ($(MODE),release)
DEFINES   := $(DEFINES) -DNDEBUG
CXXFLAGS  := $(CXXFLAGS) -flto -O3 -g -ffunction-sections -fdata-sections -Xclang -MT
CFLAGS    := $(CFLAGS) -flto -O3 -g -ffunction-sections -fdata-sections -Xclang -MT
LINKFLAGS := $(LINKFLAGS) -flto -g -Xlinker /OPT:ref -Xlinker /OPT:icf -RELEASE
LINKLIBS  := $(LINKLIBS) -llibcmt.lib
else
DEFINES   := $(DEFINES) -D_DEBUG
CXXFLAGS  := $(CXXFLAGS) -Og -g3 -Xclang -MTd
CFLAGS    := $(CFLAGS) -Og -g3 -Xclang -MTd
LINKFLAGS := $(LINKFLAGS) -g3
LINKLIBS  := $(LINKLIBS) -llibcmtd.lib -lDbgHelp.lib
endif
