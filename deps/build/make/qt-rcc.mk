
include platforms/detect-host.mk

NAME       = qt-rcc
INCDIR     = ../../include
SRCDIR     = ../../src/$(NAME)
OBJDIR    := $(OBJDIR)/$(NAME)
DEFINES   := $(DEFINES) \
             -DCHROMIUM_ZLIB_NO_CHROMECONF \
             -DQT_VERSION_MAJOR=5 \
             -DQT_VERSION_MINOR=12 \
             -DQT_VERSION_PATCH=4 \
             -DQT_VERSION_STR="\"5.12.4\"" \
             -DQT_RCC \
             -DQT_BOOTSTRAPPED \
             -DQT_STATIC \
             -DQT_USE_QSTRINGBUILDER \
             -DQT_NO_EXCEPTIONS \
             -DQT_NO_FOREACH \
             -DQT_NO_CAST_TO_ASCII \
             -DQT_NO_CAST_FROM_BYTEARRAY \
             -DQT_NO_NARROWING_CONVERSIONS_IN_CONNECT \
             -DQT_NO_COMPRESS \
             -DHAVE_CONFIG_H \
             -DUNICODE \
             -D_ENABLE_EXTENDED_ALIGNED_STORAGE
INCLUDES  := $(INCLUDES) \
             -I$(INCDIR)/tinycbor \
             -I$(INCDIR)/qt \
             -I$(INCDIR)/qt/QtCore \
             -I$(SRCDIR)
CXXFLAGS  := $(CXXFLAGS) -std=c++17
LINKFLAGS := $(LINKFLAGS)
LINKPATH  := $(LINKPATH)
LINKLIBS  := $(LINKLIBS) \
             $(LIBDIR)/zlib$(SUFFIX)$(EXTLIB) \
             $(LIBDIR)/qt-bootstrap$(SUFFIX)$(EXTLIB)
OBJS       = main.o rcc.o

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

# WINDOWS
ifeq ($(TARGET_OS),win)
DEFINES   := $(DEFINES) -D_CRT_SECURE_NO_WARNINGS
CXXFLAGS  := $(CXXFLAGS)
INCLUDES  := $(INCLUDES) -I$(INCDIR)/qt/mkspecs/win32-clang-msvc
LINKLIBS  := $(LINKLIBS) -lOle32.lib -lAdvapi32.lib -lShell32.lib -lNetapi32.lib
OBJS      := $(OBJS)
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

# OSX
ifeq ($(TARGET_OS),osx)
DEFINES   := $(DEFINES)
CXXFLAGS  := $(CXXFLAGS)
INCLUDES  := $(INCLUDES) -I$(INCDIR)/qt/mkspecs/macx-clang
LINKLIBS  := $(LINKLIBS) -framework AppKit
OBJS      := $(OBJS)
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

# LINUX
ifeq ($(TARGET_OS),linux)
DEFINES   := $(DEFINES)
CXXFLAGS  := $(CXXFLAGS)
INCLUDES  := $(INCLUDES) -I$(INCDIR)/qt/mkspecs/linux-clang
LINKLIBS  := $(LINKLIBS) -lpthread
OBJS      := $(OBJS)
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

# ANDROID
ifeq ($(TARGET_OS),android)
DEFINES   := $(DEFINES)
CXXFLAGS  := $(CXXFLAGS)
INCLUDES  := $(INCLUDES) -I$(INCDIR)/qt/mkspecs/android-clang
OBJS      := $(OBJS)
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

ifeq ($(MODE),debug)
DEFINES   := $(DEFINES) -DQT_DEBUG
else
DEFINES   := $(DEFINES) -DQT_NO_DEBUG
endif

OBJS := $(patsubst %,$(OBJDIR)/%,$(OBJS))

$(OBJDIR)/%.o:
	@echo [CXX] $@
	$(CXX) $(CPUFLAGS) $(DEFINES) $(INCLUDES) $(CXXFLAGS) -c $(@:$(OBJDIR)%.o=$(SRCDIR)%.cpp) -o $@

.DEFAULT_GOAL := build

build: $(OBJS)
	@echo [LNK] $(BINDIR)/$(NAME)$(SUFFIX)$(EXTBIN)
	$(LINK) $(LINKFLAGS) $(LINKPATH) $(OBJS) $(LINKLIBS) -o $(BINDIR)/$(NAME)$(SUFFIX)$(EXTBIN)

clean:
	$(call deletefiles,$(OBJDIR),*.o)
	$(call deletefiles,$(LIBDIR),$(NAME)$(SUFFIX)$(EXTLIB))
	$(call deletefiles,$(BINDIR),$(NAME)$(SUFFIX)$(EXTBIN))
	$(call deletefiles,$(BINDIR),$(NAME)$(SUFFIX)$(EXTPDB))
