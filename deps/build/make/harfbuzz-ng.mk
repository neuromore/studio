
include platforms/detect-host.mk
include platforms/$(DETECTED_OS)-$(DETECTED_ARCH)-$(TARGET_OS)-$(TARGET_ARCH).mk

NAME       = harfbuzz-ng
INCDIR     = ../../include/
SRCDIR     = ../../src/$(NAME)
OBJDIR    := $(OBJDIR)/$(NAME)
DEFINES   := $(DEFINES) \
             -DHAVE_CONFIG_H \
             -DHAVE_OT \
             -DHB_NO_UNICODE_FUNCS \
             -DHB_DISABLE_DEPRECATED \
             -DHB_EXTERN= \
             -DHB_NO_WIN1256 \
             -DQT_NO_VERSION_TAGGING \
             -DQT_NO_DYNAMIC_CAST \
             -DQT_NO_EXCEPTIONS \
             -Wno-deprecated-declarations 
INCLUDES  := $(INCLUDES) -I$(INCDIR) -I$(INCDIR)/$(NAME) -I$(INCDIR)/qt
CXXFLAGS  := $(CXXFLAGS) -std=c++17
LINKFLAGS := $(LINKFLAGS)
LINKPATH  := $(LINKPATH)
LINKLIBS  := $(LINKLIBS)
OBJS       = hb-blob.o \
             hb-buffer.o \
             hb-buffer-serialize.o \
             hb-common.o \
             hb-face.o \
             hb-font.o \
             hb-ot-font.o \
             hb-ot-layout.o \
             hb-ot-map.o \
             hb-ot-math.o \
             hb-ot-shape.o \
             hb-ot-shape-complex-arabic.o \
             hb-ot-shape-complex-default.o \
             hb-ot-shape-complex-hangul.o \
             hb-ot-shape-complex-hebrew.o \
             hb-ot-shape-complex-indic.o \
             hb-ot-shape-complex-indic-table.o \
             hb-ot-shape-complex-myanmar.o \
             hb-ot-shape-complex-thai.o \
             hb-ot-shape-complex-tibetan.o \
             hb-ot-shape-complex-use.o \
             hb-ot-shape-complex-use-table.o \
             hb-ot-shape-fallback.o \
             hb-ot-shape-normalize.o \
             hb-ot-tag.o \
             hb-ot-var.o \
             hb-set.o \
             hb-shape.o \
             hb-shape-plan.o \
             hb-shaper.o \
             hb-unicode.o \
             hb-warning.o

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
DEFINES   := $(DEFINES) -D_CRT_SECURE_NO_WARNINGS
CXXFLAGS  := $(CXXFLAGS)
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
CXXFLAGS  := $(CXXFLAGS)
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
CXXFLAGS  := $(CXXFLAGS)
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
CXXFLAGS  := $(CXXFLAGS)
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
	@echo [CXX] $@
	$(CXX) $(CPUFLAGS) $(DEFINES) $(INCLUDES) $(CXXFLAGS) -c $(@:$(OBJDIR)%.o=$(SRCDIR)%.cc) -o $@

.DEFAULT_GOAL := build

build: $(OBJS)
	@echo [AR]  $(LIBDIR)/$(NAME)$(SUFFIX)$(EXTLIB)
	$(AR) $(ARFLAGS) $(LIBDIR)/$(NAME)$(SUFFIX)$(EXTLIB) $(OBJS)

clean:
	$(call deletefiles,$(OBJDIR),*.o)
	$(call deletefiles,$(LIBDIR),$(NAME)$(SUFFIX)$(EXTLIB))
