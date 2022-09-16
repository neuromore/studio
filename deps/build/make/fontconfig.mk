
include platforms/detect-host.mk

NAME       = fontconfig
INCDIR     = ../../include
SRCDIR     = ../../src/$(NAME)
OBJDIR    := $(OBJDIR)/$(NAME)
DEFINES   := $(DEFINES) -DHAVE_CONFIG_H
INCLUDES  := $(INCLUDES) \
             -I$(INCDIR)/ \
             -I$(INCDIR)/$(NAME) \
             -I$(INCDIR)/freetype
CFLAGS    := $(CFLAGS) \
             -Wno-non-literal-null-conversion \
             -Wno-pointer-bool-conversion
LINKFLAGS := $(LINKFLAGS)
LINKPATH  := $(LINKPATH)
LINKLIBS  := $(LINKLIBS)
OBJS       = 

ifeq ($(TARGET_ARCH),x86)
DEFINES   := $(DEFINES)
endif

ifeq ($(TARGET_ARCH),x64)
DEFINES   := $(DEFINES)
endif

ifeq ($(TARGET_ARCH),arm)
DEFINES   := $(DEFINES)
endif

ifeq ($(TARGET_ARCH),arm64)
DEFINES   := $(DEFINES)
endif

ifeq ($(TARGET_OS),win)
DEFINES   := $(DEFINES)
CFLAGS    := $(CFLAGS)
ifeq ($(TARGET_ARCH),x86)
DEFINES   := $(DEFINES)
endif
ifeq ($(TARGET_ARCH),x64)
DEFINES   := $(DEFINES)
endif
ifeq ($(TARGET_ARCH),arm)
DEFINES   := $(DEFINES)
endif
ifeq ($(TARGET_ARCH),arm64)
DEFINES   := $(DEFINES)
endif
endif

ifeq ($(TARGET_OS),osx)
DEFINES   := $(DEFINES)
CFLAGS    := $(CFLAGS)
ifeq ($(TARGET_ARCH),x86)
DEFINES   := $(DEFINES)
endif
ifeq ($(TARGET_ARCH),x64)
DEFINES   := $(DEFINES)
endif
ifeq ($(TARGET_ARCH),arm)
DEFINES   := $(DEFINES)
endif
ifeq ($(TARGET_ARCH),arm64)
DEFINES   := $(DEFINES)
endif
endif

ifeq ($(TARGET_OS),linux)
DEFINES   := $(DEFINES) \
             -DFONTCONFIG_PATH=\"/etc/fonts\" \
             -DFC_CACHEDIR=\"~/.cache/fontconfig\" \
             -DFC_TEMPLATEDIR=\"/usr/share/fontconfig/conf.avail\"
CFLAGS    := $(CFLAGS)
OBJS      := $(OBJS) \
             fcarch.o fcatomic.o fccache.o fccfg.o fccharset.o fccompat.o \
             fcdbg.o fcdefault.o fcdir.o fcformat.o fcfreetype.o fcfs.o \
             fchash.o fcinit.o fclang.o fclist.o fcmatch.o fcmatrix.o \
             fcname.o fcobjs.o fcpat.o fcptrlist.o fcrange.o fcserialize.o \
             fcstat.o fcstr.o fcweight.o fcxml.o ftglue.o
ifeq ($(TARGET_ARCH),x86)
DEFINES   := $(DEFINES) -DSIZEOF_VOID_P=4 -DALIGNOF_DOUBLE=4 -DALIGNOF_VOID_P=4
endif
ifeq ($(TARGET_ARCH),x64)
DEFINES   := $(DEFINES) -DSIZEOF_VOID_P=8 -DALIGNOF_DOUBLE=8 -DALIGNOF_VOID_P=8
endif
ifeq ($(TARGET_ARCH),arm)
DEFINES   := $(DEFINES) -DSIZEOF_VOID_P=4 -DALIGNOF_DOUBLE=8 -DALIGNOF_VOID_P=4
endif
ifeq ($(TARGET_ARCH),arm64)
DEFINES   := $(DEFINES) -DSIZEOF_VOID_P=8 -DALIGNOF_DOUBLE=8 -DALIGNOF_VOID_P=8
endif
endif

ifeq ($(TARGET_OS),android)
DEFINES   := $(DEFINES)
CFLAGS    := $(CFLAGS)
ifeq ($(TARGET_ARCH),x86)
DEFINES   := $(DEFINES)
endif
ifeq ($(TARGET_ARCH),x64)
DEFINES   := $(DEFINES)
endif
ifeq ($(TARGET_ARCH),arm)
DEFINES   := $(DEFINES)
endif
ifeq ($(TARGET_ARCH),arm64)
DEFINES   := $(DEFINES)
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
