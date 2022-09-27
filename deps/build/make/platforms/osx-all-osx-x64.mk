
# MIN OSX: 10.15
# MIN CPU: Ivy Bridge
#
# See: https://support.apple.com/en-us/HT210222
#
# | Min. Model  | Released   | CPU                | Architecture |                                                                                                               |
# |-------------|------------|--------------------|--------------|
# | MacBook     | early 2015 | Core M-5Y51        | Broadwell    |
# | MacBook Air | mid   2012 | Core i5-3427U      | Ivy Bridge   |
# | MacBook Pro | mid   2012 | Core i5-3210M      | Ivy Bridge   |
# | iMac Pro    | late  2017 | Xeon W-2140B       | Skylake      |
# | iMac        | late  2012 | Core i5-3330S      | Ivy Bridge   |
# | Mac mini    | late  2012 | Core i5-3210M      | Ivy Bridge   |
# | Mac Pro     | late  2013 | Xeon E5-1620v2     | Ivy Bridge   |
# |-------------|------------|--------------------|--------------|
OSXMINVERSION = 10.15

# For Ivy Bridge (based on table above)
# x86-64-v2 requires CLANG 12+ and is close to Nehalem
CPUFLAGS = -march=x86-64-v2 -mtune=generic

# Not or not sure if covered by x86-64-v2
CPUFLAGS := $(CPUFLAGS) \
	-mcx16 -mavx -mxsave -mpclmul \
	-mfsgsbase -mrdrnd -mf16c -maes    

# Generic
EXTBIN     = 
EXTLIB     = .a
EXTDLL     = .dylib
EXTPDB     = .pdb
OBJDIR     = obj/osx-x64-$(MODE)
LIBDIR     = lib/osx-x64
BINDIR     = bin/osx-x64
DISTDIR    = ../../dist/osx-$(OSXMINVERSION)
TARGET     = x86_64-apple-darwin19.6.0
MINVER     = -mmacosx-version-min=$(OSXMINVERSION)
DEFINES    = 
INCLUDES   = 
CXX        = clang++
CXXFLAGS   = $(MINVER) -target $(TARGET) -static
CC         = clang
CFLAGS     = $(MINVER) -target $(TARGET) -static
AR         = ar
ARFLAGS    = rcs
STRIP      = strip
STRIPFLAGS = -S
LINK       = $(CXX)
LINKFLAGS  = $(MINVER) -target $(TARGET)
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
