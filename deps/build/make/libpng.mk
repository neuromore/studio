
include platforms/detect-host.mk
include platforms/$(DETECTED_OS)-$(DETECTED_ARCH)-$(TARGET_OS)-$(TARGET_ARCH).mk

NAME       = libpng
INCDIR     = ../../include
SRCDIR     = ../../src/$(NAME)
OBJDIR    := $(OBJDIR)/$(NAME)
DEFINES   := $(DEFINES) -DCHROMIUM_ZLIB_NO_CHROMECONF
INCLUDES  := $(INCLUDES) -I$(INCDIR)/ -I$(INCDIR)/zlib -I$(INCDIR)/libpng
CFLAGS    := $(CFLAGS) \
             -Wno-tautological-constant-out-of-range-compare
LINKFLAGS := $(LINKFLAGS)
LINKPATH  := $(LINKPATH)
LINKLIBS  := $(LINKLIBS)
OBJS       = png.o pngerror.o pngget.o pngmem.o pngpread.o pngread.o pngrio.o pngrtran.o pngrutil.o \
             pngset.o pngtrans.o pngwio.o pngwrite.o pngwtran.o pngwutil.o

################################################################################################
# CPU

ifeq ($(TARGET_ARCH),x86)
DEFINES   := $(DEFINES) -DPNG_INTEL_SSE
OBJS      := $(OBJS) \
             intel/filter_sse2_intrinsics.o \
             intel/intel_init.o
endif

ifeq ($(TARGET_ARCH),x64)
DEFINES   := $(DEFINES) -DPNG_INTEL_SSE
OBJS      := $(OBJS) \
             intel/filter_sse2_intrinsics.o \
             intel/intel_init.o
endif

ifeq ($(TARGET_ARCH),arm)
DEFINES   := $(DEFINES)
OBJS      := $(OBJS) \
             arm/arm_init.o \
             arm/filter_neon_intrinsics.o
endif

ifeq ($(TARGET_ARCH),arm64)
DEFINES   := $(DEFINES)
OBJS      := $(OBJS) \
             arm/arm_init.o \
             arm/filter_neon_intrinsics.o
endif

################################################################################################
# OS

ifeq ($(TARGET_OS),win)
DEFINES   := $(DEFINES) -D_CRT_SECURE_NO_WARNINGS
CFLAGS    := $(CFLAGS)
endif

ifeq ($(TARGET_OS),osx)
DEFINES   := $(DEFINES)
CFLAGS    := $(CFLAGS)
endif

ifeq ($(TARGET_OS),linux)
DEFINES   := $(DEFINES)
CFLAGS    := $(CFLAGS)
endif

ifeq ($(TARGET_OS),android)
DEFINES   := $(DEFINES)
CFLAGS    := $(CFLAGS)
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
