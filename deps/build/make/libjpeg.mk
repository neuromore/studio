
include platforms/detect-host.mk

NAME       = libjpeg
INCDIR     = ../../include
SRCDIR     = ../../src/$(NAME)
OBJDIR    := $(OBJDIR)/$(NAME)
DEFINES   := $(DEFINES)
INCLUDES  := $(INCLUDES) -I$(INCDIR)/ -I$(INCDIR)/libjpeg
CFLAGS    := $(CFLAGS)
LINKFLAGS := $(LINKFLAGS)
LINKPATH  := $(LINKPATH)
LINKLIBS  := $(LINKLIBS)
OBJS       = jaricom.o jcapimin.o jcapistd.o jcarith.o jccoefct.o jccolor.o jcdctmgr.o jchuff.o \
             jcinit.o jcmainct.o jcmarker.o jcmaster.o jcomapi.o jcparam.o jcprepct.o jcsample.o \
             jctrans.o jdapimin.o jdapistd.o jdarith.o jdatadst.o jdatasrc.o jdcoefct.o jdcolor.o \
             jddctmgr.o jdhuff.o jdinput.o jdmainct.o jdmarker.o jdmaster.o jdmerge.o jdpostct.o jdsample.o \
             jdtrans.o jerror.o jfdctflt.o jfdctfst.o jfdctint.o jidctflt.o jidctfst.o jidctint.o \
             jmemmgr.o jmemnobs.o jquant1.o jquant2.o jutils.o rdbmp.o rdcolmap.o rdgif.o rdppm.o \
             rdrle.o rdswitch.o rdtarga.o transupp.o wrbmp.o wrgif.o wrppm.o wrrle.o wrtarga.o

ifeq ($(TARGET_ARCH),x86)
DEFINES   := $(DEFINES)
OBJS      := $(OBJS)
endif

ifeq ($(TARGET_ARCH),x64)
DEFINES   := $(DEFINES)
OBJS      := $(OBJS)
endif

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
