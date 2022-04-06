
include platforms/detect-host.mk
include platforms/$(DETECTED_OS)-$(DETECTED_ARCH)-$(TARGET_OS)-$(TARGET_ARCH).mk

NAME       = brainflow-gforcesdkcxx-wrapper
INCDIR     = ../../include
SRCDIR     = ../../src/$(NAME)
OBJDIR    := $(OBJDIR)/$(NAME)
DEFINES   := $(DEFINES) -D_UNICODE
INCLUDES  := $(INCLUDES) \
            -I$(INCDIR) \
            -I$(INCDIR)/gForceSDKCXX \
            -I$(INCDIR)/brainflow/gforcesdkcxx-wrapper \
            -I$(INCDIR)/brainflow/utils
CXXFLAGS  := $(CXXFLAGS) \
             -Wno-varargs \
             -std=c++14
LINKFLAGS := $(LINKFLAGS) -shared
LINKPATH  := $(LINKPATH)
LINKLIBS  := $(LINKLIBS) \
             $(LIBDIR)/brainflow$(SUFFIX)$(EXTLIB)
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
DEFINES   := $(DEFINES) -D_CRT_SECURE_NO_WARNINGS
CXXFLAGS  := $(CXXFLAGS)
LINKFLAGS := $(LINKFLAGS) -Xlinker /SUBSYSTEM:CONSOLE -DLL
LINKLIBS  := $(LINKLIBS) -lUser32.lib -lGdi32.lib
OBJS      := $(OBJS) wrapper.o
ifeq ($(TARGET_ARCH),x86)
DEFINES   := $(DEFINES)
NAMEDLL    = gForceSDKWrapper32$(SUFFIX)
LINKPATH  := $(LINKPATH) -L../../prebuilt/win/x86
ifeq ($(MODE),debug)
LINKLIBS  := $(LINKLIBS) -lgforce32d.lib
else
LINKLIBS  := $(LINKLIBS) -lgforce32.lib
endif
endif
ifeq ($(TARGET_ARCH),x64)
DEFINES   := $(DEFINES)
NAMEDLL    = gForceSDKWrapper$(SUFFIX)
ifeq ($(MODE),debug)
LINKLIBS  := $(LINKLIBS) -lgforce64d.lib
else
LINKLIBS  := $(LINKLIBS) -lgforce64.lib
endif
endif
ifeq ($(TARGET_ARCH),arm)
DEFINES   := $(DEFINES)
NAMEDLL    = gForceSDKWrapper32$(SUFFIX)
endif
ifeq ($(TARGET_ARCH),arm64)
DEFINES   := $(DEFINES)
NAMEDLL    = gForceSDKWrapper$(SUFFIX)
endif
endif

ifeq ($(TARGET_OS),osx)
DEFINES   := $(DEFINES)
CXXFLAGS  := $(CXXFLAGS)
OBJS      := $(OBJS)
ifeq ($(TARGET_ARCH),x86)
DEFINES   := $(DEFINES)
NAMEDLL    = libgForceSDKWrapper$(SUFFIX)
endif
ifeq ($(TARGET_ARCH),x64)
DEFINES   := $(DEFINES)
NAMEDLL    = libgForceSDKWrapper$(SUFFIX)
endif
ifeq ($(TARGET_ARCH),arm)
DEFINES   := $(DEFINES)
NAMEDLL    = libgForceSDKWrapper$(SUFFIX)
endif
ifeq ($(TARGET_ARCH),arm64)
DEFINES   := $(DEFINES)
NAMEDLL    = libgForceSDKWrapper$(SUFFIX)
endif
endif

ifeq ($(TARGET_OS),linux)
DEFINES   := $(DEFINES)
CXXFLAGS  := $(CXXFLAGS)
OBJS      := $(OBJS)
ifeq ($(TARGET_ARCH),x86)
DEFINES   := $(DEFINES)
NAMEDLL    = libgForceSDKWrapper$(SUFFIX)
endif
ifeq ($(TARGET_ARCH),x64)
DEFINES   := $(DEFINES)
NAMEDLL    = libgForceSDKWrapper$(SUFFIX)
endif
ifeq ($(TARGET_ARCH),arm)
DEFINES   := $(DEFINES)
NAMEDLL    = libgForceSDKWrapper$(SUFFIX)
endif
ifeq ($(TARGET_ARCH),arm64)
DEFINES   := $(DEFINES)
NAMEDLL    = libgForceSDKWrapper$(SUFFIX)
endif
endif

ifeq ($(TARGET_OS),android)
DEFINES   := $(DEFINES)
CXXFLAGS  := $(CXXFLAGS)
OBJS      := $(OBJS)
ifeq ($(TARGET_ARCH),x86)
DEFINES   := $(DEFINES)
NAMEDLL    = libgForceSDKWrapper$(SUFFIX)
endif
ifeq ($(TARGET_ARCH),x64)
DEFINES   := $(DEFINES)
NAMEDLL    = libgForceSDKWrapper$(SUFFIX)
endif
ifeq ($(TARGET_ARCH),arm)
DEFINES   := $(DEFINES)
NAMEDLL    = libgForceSDKWrapper$(SUFFIX)
endif
ifeq ($(TARGET_ARCH),arm64)
DEFINES   := $(DEFINES)
NAMEDLL    = libgForceSDKWrapper$(SUFFIX)
endif
endif

OBJS := $(patsubst %,$(OBJDIR)/%,$(OBJS))

$(OBJDIR)/%.o:
	@echo [CXX] $@
	$(CXX) $(CPUFLAGS) $(DEFINES) $(INCLUDES) $(CXXFLAGS) -c $(@:$(OBJDIR)%.o=$(SRCDIR)%.cpp) -o $@

.DEFAULT_GOAL := build

build: $(OBJS)
	@echo [AR]  $(LIBDIR)/$(NAME)$(SUFFIX)$(EXTLIB)
	$(AR) $(ARFLAGS) $(LIBDIR)/$(NAME)$(SUFFIX)$(EXTLIB) $(OBJS)
	@echo [LNK] $(BINDIR)/$(NAMEDLL)$(EXTDLL)
	$(LINK) $(LINKFLAGS) $(LINKPATH) $(LIBDIR)/$(NAME)$(SUFFIX)$(EXTLIB) $(LINKLIBS) -o $(BINDIR)/$(NAMEDLL)$(EXTDLL)

clean:
	$(call deletefiles,$(OBJDIR),*.o)
	$(call deletefiles,$(LIBDIR),$(NAME)$(SUFFIX)$(EXTLIB))
	$(call deletefiles,$(BINDIR),$(NAMEDLL)$(EXTLIB))
	$(call deletefiles,$(BINDIR),$(NAMEDLL)$(EXTDLL))
	$(call deletefiles,$(BINDIR),$(NAMEDLL)$(EXTPDB))
