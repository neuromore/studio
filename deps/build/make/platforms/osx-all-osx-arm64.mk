# Generic
EXTBIN     = 
EXTLIB     = .a
EXTDLL     = .dylib
EXTPDB     = .pdb
OBJDIR     = obj/osx-arm64-$(MODE)
LIBDIR     = lib/osx-arm64
BINDIR     = bin/osx-arm64
DISTDIR    = ../../dist/osx-10.15
TARGET     = arm64-apple-darwin19.6.0
MINVER     = -mmacosx-version-min=10.15
CPUFLAGS   = -march=armv8-a -mtune=generic
DEFINES    = 
INCLUDES   = 
CXX        = clang++
CXXFLAGS   = $(MINVER) -target $(TARGET) -isysroot $(shell xcrun --sdk macosx --show-sdk-path) -static
CC         = clang
CFLAGS     = $(MINVER) -target $(TARGET) -isysroot $(shell xcrun --sdk macosx --show-sdk-path) -static
AR         = ar
ARFLAGS    = rcs
STRIP      = strip
STRIPFLAGS = -S
LINK       = $(CXX)
LINKFLAGS  = -target $(TARGET) -isysroot $(shell xcrun --sdk macosx --show-sdk-path)
LINKPATH   =
LINKLIBS   = 

# Debug vs. Release
ifeq ($(MODE),release)
DEFINES   := $(DEFINES) -DNDEBUG
CXXFLAGS  := $(CXXFLAGS) -flto -O3 -ffunction-sections -fdata-sections
CFLAGS    := $(CFLAGS) -flto -O3 -ffunction-sections -fdata-sections
LINKFLAGS := $(LINKFLAGS) -flto -dead_strip
else
DEFINES   := $(DEFINES) -D_DEBUG
CXXFLAGS  := $(CXXFLAGS) -Og -g3
CFLAGS    := $(CFLAGS) -Og -g3
LINKFLAGS := $(LINKFLAGS) -g3
endif
