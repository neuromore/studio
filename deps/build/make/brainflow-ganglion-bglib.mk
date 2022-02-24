
include platforms/detect-host.mk
include platforms/$(DETECTED_OS)-$(DETECTED_ARCH)-$(TARGET_OS)-$(TARGET_ARCH).mk

NAME       = brainflow-ganglion-bglib
INCDIR     = ../../include
SRCDIR     = ../../src/$(NAME)
OBJDIR    := $(OBJDIR)/$(NAME)
DEFINES   := $(DEFINES)
INCLUDES  := $(INCLUDES) \
            -I$(INCDIR) \
            -I$(INCDIR)/brainflow/ganglion-bglib \
            -I$(INCDIR)/brainflow/utils
CXXFLAGS  := $(CXXFLAGS) \
             -Wno-varargs \
             -std=c++14
LINKFLAGS := $(LINKFLAGS) -shared
LINKPATH  := $(LINKPATH)
LINKLIBS  := $(LINKLIBS) \
             $(LIBDIR)/brainflow$(SUFFIX)$(EXTLIB)
OBJS       = callbacks.o cmd_def.o helpers.o main.o stubs.o uart.o

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
ifeq ($(TARGET_ARCH),x86)
DEFINES   := $(DEFINES)
NAMEDLL    = GanglionLib32$(SUFFIX)
endif
ifeq ($(TARGET_ARCH),x64)
DEFINES   := $(DEFINES)
NAMEDLL    = GanglionLib$(SUFFIX)
endif
ifeq ($(TARGET_ARCH),arm)
DEFINES   := $(DEFINES)
NAMEDLL    = GanglionLib32$(SUFFIX)
endif
ifeq ($(TARGET_ARCH),arm64)
DEFINES   := $(DEFINES)
NAMEDLL    = GanglionLib$(SUFFIX)
endif
endif

ifeq ($(TARGET_OS),osx)
DEFINES   := $(DEFINES)
CXXFLAGS  := $(CXXFLAGS)
ifeq ($(TARGET_ARCH),x86)
DEFINES   := $(DEFINES)
NAMEDLL    = libGanglionLib$(SUFFIX)
endif
ifeq ($(TARGET_ARCH),x64)
DEFINES   := $(DEFINES)
NAMEDLL    = libGanglionLib$(SUFFIX)
endif
ifeq ($(TARGET_ARCH),arm)
DEFINES   := $(DEFINES)
NAMEDLL    = libGanglionLib$(SUFFIX)
endif
ifeq ($(TARGET_ARCH),arm64)
DEFINES   := $(DEFINES)
NAMEDLL    = libGanglionLib$(SUFFIX)
endif
endif

ifeq ($(TARGET_OS),linux)
DEFINES   := $(DEFINES)
CXXFLAGS  := $(CXXFLAGS)
ifeq ($(TARGET_ARCH),x86)
DEFINES   := $(DEFINES)
NAMEDLL    = libGanglionLib$(SUFFIX)
endif
ifeq ($(TARGET_ARCH),x64)
DEFINES   := $(DEFINES)
NAMEDLL    = libGanglionLib$(SUFFIX)
endif
ifeq ($(TARGET_ARCH),arm)
DEFINES   := $(DEFINES)
NAMEDLL    = libGanglionLib$(SUFFIX)
endif
ifeq ($(TARGET_ARCH),arm64)
DEFINES   := $(DEFINES)
NAMEDLL    = libGanglionLib$(SUFFIX)
endif
endif

ifeq ($(TARGET_OS),android)
DEFINES   := $(DEFINES)
CXXFLAGS  := $(CXXFLAGS)
ifeq ($(TARGET_ARCH),x86)
DEFINES   := $(DEFINES)
NAMEDLL    = libGanglionLib$(SUFFIX)
endif
ifeq ($(TARGET_ARCH),x64)
DEFINES   := $(DEFINES)
NAMEDLL    = libGanglionLib$(SUFFIX)
endif
ifeq ($(TARGET_ARCH),arm)
DEFINES   := $(DEFINES)
NAMEDLL    = libGanglionLib$(SUFFIX)
endif
ifeq ($(TARGET_ARCH),arm64)
DEFINES   := $(DEFINES)
NAMEDLL    = libGanglionLib$(SUFFIX)
endif
endif

OBJS := $(patsubst %,$(OBJDIR)/%,$(OBJS))

$(OBJDIR)/%.o:
	@echo [CXX] $@
	@$(CXX) $(CPUFLAGS) $(DEFINES) $(INCLUDES) $(CXXFLAGS) -c $(@:$(OBJDIR)%.o=$(SRCDIR)%.cpp) -o $@

.DEFAULT_GOAL := build

build: $(OBJS)
	@echo [LNK] $(LIBDIR)/$(NAMEDLL)$(EXTDLL)
	$(LINK) $(LINKFLAGS) $(LINKPATH) $(OBJS) $(LINKLIBS) -o $(LIBDIR)/$(NAMEDLL)$(EXTDLL)

clean:
	$(call deletefiles,$(OBJDIR),*.o)
	$(call deletefiles,$(LIBDIR),$(NAMEDLL)$(EXTLIB))
	$(call deletefiles,$(LIBDIR),$(NAMEDLL)$(EXTDLL))
	$(call deletefiles,$(LIBDIR),$(NAMEDLL)$(EXTPDB))
