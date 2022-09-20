# Generic
EXTBIN     = .exe
EXTLIB     = .lib
EXTDLL     = .dll
EXTPDB     = .pdb
OBJDIR     = obj/win-x86-$(MODE)
LIBDIR     = lib/win-x86
BINDIR     = bin/win-x86
DISTDIR    = ../../dist/win-10
TARGET     = i686-pc-windows-msvc
CPUFLAGS   = -march=i686 -mtune=generic -mmmx -msse -msse2
DEFINES    = -DWIN32 -D_MT
INCLUDES   = 
CXX        = clang++
CXXFLAGS   = -target $(TARGET) -static -Werror
CC         = clang
CFLAGS     = -target $(TARGET) -static -Werror
AR         = llvm-ar
ARFLAGS    = rcs
STRIP      = llvm-strip
STRIPFLAGS = --strip-all
LINK       = $(CXX)
LINKFLAGS  = -target $(TARGET) -fuse-ld=lld -Xlinker /MACHINE:X86
LINKPATH   = -L$(LIBDIR) -L$(PLATDIR)/../../../prebuilt/win/x86
LINKLIBS   = 

# MSVC Resource Compiler
RC         = llvm-rc
RCFLAGS    = -L0x0409 -NOLOGO

# Debug vs. Release
ifeq ($(MODE),release)
DEFINES   := $(DEFINES) -DNDEBUG
CXXFLAGS  := $(CXXFLAGS) -flto -O3 -ffunction-sections -fdata-sections -Xclang -MT
CFLAGS    := $(CFLAGS) -flto -O3 -ffunction-sections -fdata-sections -Xclang -MT
LINKFLAGS := $(LINKFLAGS) -flto -Xlinker /OPT:ref -Xlinker /OPT:icf -RELEASE
LINKLIBS  := $(LINKLIBS) -llibcmt.lib
else
DEFINES   := $(DEFINES) -D_DEBUG
CXXFLAGS  := $(CXXFLAGS) -Og -g3 -Xclang -MTd
CFLAGS    := $(CFLAGS) -Og -g3 -Xclang -MTd
LINKFLAGS := $(LINKFLAGS) -g3
LINKLIBS  := $(LINKLIBS) -llibcmtd.lib -lDbgHelp.lib
endif
