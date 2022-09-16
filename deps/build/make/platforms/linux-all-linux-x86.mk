# Generic
EXTBIN     = 
EXTLIB     = .a
EXTDLL     = .so
EXTPDB     = .pdb
OBJDIR     = obj/linux-x86-$(MODE)
LIBDIR     = lib/linux-x86
BINDIR     = bin/linux-x86
TARGET     = i686-linux-gnu
CPUFLAGS   = -march=i686 -mtune=generic -mmmx -msse -msse2
DEFINES    = 
SYSINCDIR  = /usr/include/i386-linux-gnu
SYSLIBDIR  = /usr/lib/i386-linux-gnu
INCLUDES   = -I$(SYSINCDIR)
CXX        = clang++
CXXFLAGS   = -target $(TARGET) -fPIC -static
CC         = clang
CFLAGS     = -target $(TARGET) -fPIC -static
AR         = llvm-ar
ARFLAGS    = rcs
STRIP      = llvm-strip
STRIPFLAGS = --strip-all
LINK       = $(CXX)
LINKFLAGS  = -target $(TARGET) -fuse-ld=lld -static-libstdc++ -static-libgcc
LINKPATH   = -L$(SYSLIBDIR) -L$(LIBDIR) -L$(PLATDIR)/../../../prebuilt/linux/x86
LINKLIBS   = 
DEBARCH    = i386
LSBREL     = $(shell lsb_release -r -s)
DISTDIR    = ../../dist/ubuntu-$(LSBREL)

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
