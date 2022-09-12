# Generic
EXTBIN     = 
EXTLIB     = .a
EXTDLL     = .so
EXTPDB     = .pdb
OBJDIR     = obj/linux-arm-$(MODE)
LIBDIR     = lib/linux-arm
BINDIR     = bin/linux-arm
TARGET     = arm-linux-gnueabihf
CPUFLAGS   = -march=armv7-a -mfloat-abi=hard -mfpu=neon-fp16
DEFINES    = 
INCLUDES   = -I/usr/$(TARGET)/include
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
LINKPATH   = 
LINKLIBS   = 
DEBARCH    = armhf
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
