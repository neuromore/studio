# Generic
EXTBIN     = 
EXTLIB     = .a
EXTDLL     = .dylib
EXTPDB     = .pdb
OBJDIR     = obj/ios-arm64-$(MODE)
LIBDIR     = lib/ios-arm64
BINDIR     = bin/ios-arm64
DISTDIR    = ../../dist/ios-13.0
TARGET     = arm64-apple-ios13.0
MINVER     = -miphoneos-version-min=13.0
CPUFLAGS   = -march=armv8-a -mtune=generic
SDKROOT    = $(shell xcrun --sdk iphoneos --show-sdk-path)
DEFINES    = 
INCLUDES   = 
CXX        = clang++
CXXFLAGS   = $(MINVER) -target $(TARGET) -isysroot $(SDKROOT) -static
CC         = clang
CFLAGS     = $(MINVER) -target $(TARGET) -isysroot $(SDKROOT) -static
AR         = ar
ARFLAGS    = rcs
STRIP      = strip
STRIPFLAGS = -S
LINK       = $(CXX)
LINKFLAGS  = $(MINVER) -target $(TARGET) -isysroot $(SDKROOT)
LINKPATH   =
LINKLIBS   = 

# Debug vs. Release
ifeq ($(MODE),release)
DEFINES   := $(DEFINES) -DNDEBUG
CXXFLAGS  := $(CXXFLAGS) -flto=thin -O3 -g -ffunction-sections -fdata-sections
CFLAGS    := $(CFLAGS) -flto=thin -O3 -g -ffunction-sections -fdata-sections
LINKFLAGS := $(LINKFLAGS) -flto=thin -O3 -g -dead_strip
else
DEFINES   := $(DEFINES) -D_DEBUG
CXXFLAGS  := $(CXXFLAGS) -Og -g3
CFLAGS    := $(CFLAGS) -Og -g3
LINKFLAGS := $(LINKFLAGS) -g3
endif
