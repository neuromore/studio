# Requires NDK 22b or later

# Android Specific
ANDROID_API         = 21
ANDROID_TOOLCHAIN   = $(ANDROID_NDK_HOME)/toolchains/llvm/prebuilt/windows-x86_64

# Generic
EXTBIN     =
EXTLIB     = .a
EXTDLL     = .so
EXTPDB     = .pdb
OBJDIR     = obj/android-arm-$(MODE)
LIBDIR     = lib/android-arm
BINDIR     = bin/android-arm
DISTDIR    = ../../dist/android-21
TARGET     = arm-linux-androideabi
CPUFLAGS   = -march=armv7-a -mfloat-abi=softfp -mfpu=neon-fp16
DEFINES    = -DANDROID -D__ANDROID_API__=$(ANDROID_API) -DANDROID_ARM_NEON=ON 
INCLUDES   = -I$(ANDROID_NDK_HOME)/sources/android/cpufeatures
CXX        = $(ANDROID_TOOLCHAIN)/bin/armv7a-linux-androideabi$(ANDROID_API)-clang++.cmd
CXXFLAGS   = -static -fPIC \
             -target $(TARGET) \
             -isystem $(ANDROID_TOOLCHAIN)/sysroot/usr/include/$(TARGET)
CC         = $(ANDROID_TOOLCHAIN)/bin/armv7a-linux-androideabi$(ANDROID_API)-clang.cmd
CFLAGS     = -static -fPIC \
             -target $(TARGET) \
             -isystem $(ANDROID_TOOLCHAIN)/sysroot/usr/include/$(TARGET)
AR         = $(ANDROID_TOOLCHAIN)/bin/llvm-ar.exe
ARFLAGS    = rcs
STRIP      = $(ANDROID_TOOLCHAIN)/bin/llvm-strip.exe
STRIPFLAGS = --strip-all
LINK       = $(CXX)
LINKFLAGS  = -target $(TARGET) -fPIC -fuse-ld=lld -static-libstdc++ -static-libgcc -nostartfiles \
             $(ANDROID_TOOLCHAIN)/sysroot/usr/lib/$(TARGET)/$(ANDROID_API)/crtbegin_dynamic.o \
             $(ANDROID_TOOLCHAIN)/sysroot/usr/lib/$(TARGET)/$(ANDROID_API)/crtend_android.o
LINKPATH   = -L$(ANDROID_TOOLCHAIN)/sysroot/usr/lib/$(TARGET)/$(ANDROID_API) \
             -L$(ANDROID_TOOLCHAIN)/sysroot/usr/lib/$(TARGET)
LINKLIBS   = 

# Debug vs. Release
ifeq ($(MODE),release)
DEFINES   := $(DEFINES) -DNDEBUG
CXXFLAGS  := $(CXXFLAGS) -flto -O3 -ffunction-sections -fdata-sections
CFLAGS    := $(CFLAGS) -flto -O3 -ffunction-sections -fdata-sections
LINKFLAGS := $(LINKFLAGS) -flto -Wl,--gc-sections
else
DEFINES   := $(DEFINES) -D_DEBUG
CXXFLAGS  := $(CXXFLAGS) -Og -g3
CFLAGS    := $(CFLAGS) -Og -g3
LINKFLAGS := $(LINKFLAGS) -g3
endif
