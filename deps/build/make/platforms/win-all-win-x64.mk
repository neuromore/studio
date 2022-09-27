# Generic
EXTBIN     = .exe
EXTLIB     = .lib
EXTDLL     = .dll
EXTPDB     = .pdb
OBJDIR     = obj/win-x64-$(MODE)
LIBDIR     = lib/win-x64
BINDIR     = bin/win-x64
DISTDIR    = ../../dist/win-10
TARGET     = x86_64-pc-windows-msvc
CPUFLAGS   = -march=x86-64-v2 -mtune=generic -mpclmul
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
LINKFLAGS  = -target $(TARGET) -fuse-ld=lld -Xlinker /MACHINE:X64
LINKPATH   = -L$(LIBDIR) -L$(PLATDIR)/../../../prebuilt/win/x64
LINKLIBS   = 

# MSVC Resource Compiler
RC         = llvm-rc
RCFLAGS    = -L0x0409 -NOLOGO

# Debug vs. Release
ifeq ($(MODE),release)
DEFINES   := $(DEFINES) -DNDEBUG
CXXFLAGS  := $(CXXFLAGS) -flto=thin -O3 -g -ffunction-sections -fdata-sections -Xclang -MT
CFLAGS    := $(CFLAGS) -flto=thin -O3 -g -ffunction-sections -fdata-sections -Xclang -MT
LINKFLAGS := $(LINKFLAGS) -flto=thin -O3 -g -Xlinker /OPT:ref -Xlinker /OPT:icf -RELEASE
LINKLIBS  := $(LINKLIBS) -llibcmt.lib
else
DEFINES   := $(DEFINES) -D_DEBUG
CXXFLAGS  := $(CXXFLAGS) -Og -g3 -Xclang -MTd
CFLAGS    := $(CFLAGS) -Og -g3 -Xclang -MTd
LINKFLAGS := $(LINKFLAGS) -g3
LINKLIBS  := $(LINKLIBS) -llibcmtd.lib -lDbgHelp.lib 
endif
