# Platforms directory
PLATDIR = $(dir $(lastword $(MAKEFILE_LIST)))

# Include shared for host os
include $(PLATDIR)/win-all.mk

# Android Specific
ANDROID_API         = 21
ANDROID_SYSROOT     = $(ANDROID_NDK_HOME)/platforms/android-$(ANDROID_API)/arch-arm
ANDROID_TOOLCHAIN   = $(ANDROID_NDK_HOME)/toolchains/llvm/prebuilt/windows-x86_64

# Generic
EXTBIN     =
EXTLIB     = .a
EXTDLL     = .so
EXTPDB     = .pdb
OBJDIR     = obj/android-arm
LIBDIR     = lib/android-arm
BINDIR     = bin/android-arm
TARGET     = arm-linux-androideabi
CPUFLAGS   = -march=armv7-a -mfloat-abi=softfp -mfpu=neon-fp16
DEFINES    = -DANDROID -D__ANDROID_API__=$(ANDROID_API) -DANDROID_ARM_NEON=ON 
INCLUDES   = -I$(ANDROID_NDK_HOME)/sources/android/cpufeatures
CXX        = $(ANDROID_TOOLCHAIN)/bin/armv7a-linux-androideabi$(ANDROID_API)-clang++.cmd
CXXFLAGS   = -static \
             -target $(TARGET) \
             -isystem $(ANDROID_NDK_HOME)/sysroot/usr/include/$(TARGET)
CC         = $(ANDROID_TOOLCHAIN)/bin/armv7a-linux-androideabi$(ANDROID_API)-clang.cmd
CFLAGS     = -static \
             -target $(TARGET) \
             -isystem $(ANDROID_NDK_HOME)/sysroot/usr/include/$(TARGET)
AR         = $(ANDROID_TOOLCHAIN)/bin/$(TARGET)-ar.exe
ARFLAGS    = rcs
LINK       = $(CXX)
LINKFLAGS  = -target $(TARGET) -fuse-ld=lld -static-libstdc++ -static-libgcc --sysroot=$(ANDROID_SYSROOT)
LINKPATH   = -L$(LIBDIR) \
             -L$(ANDROID_SYSROOT)/usr/lib \
             -L$(ANDROID_TOOLCHAIN)/sysroot/usr/lib/$(TARGET)/$(ANDROID_API) \
             -L$(ANDROID_TOOLCHAIN)/sysroot/usr/lib/$(TARGET)
LINKLIBS   = 

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
