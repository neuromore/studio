
include platforms/detect-host.mk
include platforms/$(DETECTED_OS)-$(DETECTED_ARCH)-$(TARGET_OS)-$(TARGET_ARCH).mk

NAME       = zlib
INCDIR     = ../../include
SRCDIR     = ../../src/$(NAME)
OBJDIR    := $(OBJDIR)/$(NAME)
DEFINES   := $(DEFINES) -DCHROMIUM_ZLIB_NO_CHROMECONF
INCLUDES  := $(INCLUDES) -I$(INCDIR)/ -I$(INCDIR)/zlib
CXXFLAGS  := $(CXXFLAGS)
CFLAGS    := $(CFLAGS) \
             -Wno-deprecated-declarations
LINKFLAGS := $(LINKFLAGS)
LINKPATH  := $(LINKPATH)
LINKLIBS  := $(LINKLIBS)
OBJS       = adler32.o adler32_simd.o compress.o cpu_features.o \
             crc_folding.o crc32.o crc32_simd.o deflate.o gzclose.o \
             fill_window_sse.o gzclose.o gzlib.o gzread.o gzwrite.o \
             infback.o inffast.o inflate.o inftrees.o trees.o \
             uncompr.o zutil.o

ifeq ($(TARGET_ARCH),x86)
DEFINES   := $(DEFINES) -DINFLATE_CHUNK_SIMD_SSE2 -DDEFLATE_FILL_WINDOW_SSE2
endif

ifeq ($(TARGET_ARCH),x64)
DEFINES   := $(DEFINES) -DINFLATE_CHUNK_SIMD_SSE2 -DDEFLATE_FILL_WINDOW_SSE2 -DADLER32_SIMD_SSSE3 -DCRC32_SIMD_SSE42_PCLMUL -DINFLATE_CHUNK_READ_64LE
endif

ifeq ($(TARGET_ARCH),arm)
DEFINES   := $(DEFINES) -DINFLATE_CHUNK_SIMD_NEON -DADLER32_SIMD_NEON
endif

ifeq ($(TARGET_ARCH),arm64)
DEFINES   := $(DEFINES) -DINFLATE_CHUNK_SIMD_NEON -DADLER32_SIMD_NEON -DCRC32_ARMV8_CRC32 -DINFLATE_CHUNK_READ_64LE
endif

ifeq ($(TARGET_OS),win)
DEFINES   := $(DEFINES) -D_CRT_SECURE_NO_WARNINGS -D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
CXXFLAGS  := $(CXXFLAGS)
CFLAGS    := $(CFLAGS)
ifeq ($(TARGET_ARCH),x86)
DEFINES   := $(DEFINES) -DX86_WINDOWS
endif
ifeq ($(TARGET_ARCH),x64)
DEFINES   := $(DEFINES) -DX86_WINDOWS 
endif
ifeq ($(TARGET_ARCH),arm)
DEFINES   := $(DEFINES)
endif
ifeq ($(TARGET_ARCH),arm64)
DEFINES   := $(DEFINES) -DARMV8_OS_WINDOWS
endif
endif

ifeq ($(TARGET_OS),osx)
DEFINES   := $(DEFINES)
CXXFLAGS  := $(CXXFLAGS)
CFLAGS    := $(CFLAGS)
ifeq ($(TARGET_ARCH),x86)
DEFINES   := $(DEFINES) -DX86_NOT_WINDOWS
endif
ifeq ($(TARGET_ARCH),x64)
DEFINES   := $(DEFINES) -DX86_NOT_WINDOWS
endif
ifeq ($(TARGET_ARCH),arm)
DEFINES   := $(DEFINES)
endif
ifeq ($(TARGET_ARCH),arm64)
DEFINES   := $(DEFINES) -DARMV8_OS_MACOS
endif
endif

ifeq ($(TARGET_OS),linux)
DEFINES   := $(DEFINES)
CXXFLAGS  := $(CXXFLAGS)
CFLAGS    := $(CFLAGS)
ifeq ($(TARGET_ARCH),x86)
DEFINES   := $(DEFINES) -DX86_NOT_WINDOWS
endif
ifeq ($(TARGET_ARCH),x64)
DEFINES   := $(DEFINES) -DX86_NOT_WINDOWS
endif
ifeq ($(TARGET_ARCH),arm)
DEFINES   := $(DEFINES)
endif
ifeq ($(TARGET_ARCH),arm64)
DEFINES   := $(DEFINES) -DARMV8_OS_LINUX
endif
endif

ifeq ($(TARGET_OS),android)
DEFINES   := $(DEFINES)
CXXFLAGS  := $(CXXFLAGS)
CFLAGS    := $(CFLAGS)
ifeq ($(TARGET_ARCH),x86)
DEFINES   := $(DEFINES) -DX86_NOT_WINDOWS
endif
ifeq ($(TARGET_ARCH),x64)
DEFINES   := $(DEFINES) -DX86_NOT_WINDOWS
endif
ifeq ($(TARGET_ARCH),arm)
DEFINES   := $(DEFINES)
endif
ifeq ($(TARGET_ARCH),arm64)
DEFINES   := $(DEFINES) -DARMV8_OS_ANDROID
endif
endif

OBJS := $(patsubst %,$(OBJDIR)/%,$(OBJS))

$(OBJDIR)/%.o:
	@echo [CC]  $@
	$(CC) $(CPUFLAGS) $(DEFINES) $(INCLUDES) $(CFLAGS) -c $(@:$(OBJDIR)%.o=$(SRCDIR)%.c) -o $@
	
.DEFAULT_GOAL := build

build: $(OBJS)
	@echo [AR]  $(LIBDIR)/$(NAME)$(SUFFIX)$(EXTLIB)
	$(AR) $(ARFLAGS) $(LIBDIR)/$(NAME)$(SUFFIX)$(EXTLIB) $(OBJS)
	
clean:
	$(call deletefiles,$(OBJDIR),*.o)
	$(call deletefiles,$(LIBDIR),$(NAME)$(SUFFIX)$(EXTLIB))
