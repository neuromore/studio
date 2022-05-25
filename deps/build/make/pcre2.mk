
include platforms/detect-host.mk
include platforms/$(DETECTED_OS)-$(DETECTED_ARCH)-$(TARGET_OS)-$(TARGET_ARCH).mk

NAME       = pcre2
INCDIR     = ../../include
SRCDIR     = ../../src/$(NAME)
OBJDIR    := $(OBJDIR)/$(NAME)
DEFINES   := $(DEFINES) \
             -DPCRE2_STATIC \
             -DPCRE2_CODE_UNIT_WIDTH=16 \
             -DHAVE_CONFIG_H
INCLUDES  := $(INCLUDES) -I$(INCDIR)/ -I$(INCDIR)/$(NAME)
CFLAGS    := $(CFLAGS)
LINKFLAGS := $(LINKFLAGS)
LINKPATH  := $(LINKPATH)
LINKLIBS  := $(LINKLIBS)
OBJS       = pcre2_auto_possess.o \
             pcre2_chartables.o \
             pcre2_compile.o pcre2_compile.o pcre2_config.o \
             pcre2_context.o \
             pcre2_dfa_match.o \
             pcre2_error.o \
             pcre2_extuni.o \
             pcre2_find_bracket.o \
             pcre2_jit_compile.o \
             pcre2_maketables.o \
             pcre2_match.o \
             pcre2_match_data.o \
             pcre2_newline.o \
             pcre2_ord2utf.o \
             pcre2_pattern_info.o \
             pcre2_script_run.o \
             pcre2_serialize.o \
             pcre2_string_utils.o \
             pcre2_study.o \
             pcre2_substitute.o \
             pcre2_substring.o \
             pcre2_tables.o \
             pcre2_ucd.o \
             pcre2_valid_utf.o \
             pcre2_xclass.o

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
