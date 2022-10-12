
include ../../deps/build/make/platforms/detect-host.mk

NAME       = EngineLIB
INCDIR     = ../../deps/include/
SRCDIR     = ../../src/$(NAME)
OBJDIR    := $(OBJDIR)/$(NAME)
LIBDIRDEP  = ../../deps/build/make/$(LIBDIR)
LIBDIRPRE  = ../../deps/prebuilt/$(TARGET_OS)/$(TARGET_ARCH)
DEFINES   := $(DEFINES) \
             -DNEUROMORE_ENGINE_SHARED \
             -DUNICODE \
             -D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
INCLUDES  := $(INCLUDES) \
             -I../../src/Engine \
             -I$(INCDIR) \
             -I$(SRCDIR)
CXXFLAGS  := $(CXXFLAGS) \
             -Wno-unknown-warning-option \
             -Wno-deprecated-declarations \
             -Wno-enum-compare-switch \
             -Wno-format-security \
             -Wno-ignored-attributes \
             -std=c++17
LINKFLAGS := $(LINKFLAGS) -shared
LINKPATH  := $(LINKPATH)
LINKLIBS  := $(LINKLIBS) \
             $(LIBDIRDEP)/brainflow$(SUFFIX)$(EXTLIB) \
             $(LIBDIRDEP)/brainflow-boardcontroller$(SUFFIX)$(EXTLIB) \
             $(LIBDIRDEP)/edflib$(SUFFIX)$(EXTLIB) \
             $(LIBDIRDEP)/oscpack$(SUFFIX)$(EXTLIB) \
             $(LIBDIRDEP)/kissfft$(SUFFIX)$(EXTLIB) \
             $(LIBDIRDEP)/zlib$(SUFFIX)$(EXTLIB) \
             $(LIBDIR)/Engine$(SUFFIX)$(EXTLIB)
OBJS       =

ifeq ($(TARGET_ARCH),x86)
DEFINES   := $(DEFINES) -DNEUROMORE_ARCHITECTURE_X86
endif

ifeq ($(TARGET_ARCH),x64)
DEFINES   := $(DEFINES) -DNEUROMORE_ARCHITECTURE_X86
endif

ifeq ($(TARGET_ARCH),arm)
DEFINES   := $(DEFINES)
endif

ifeq ($(TARGET_ARCH),arm64)
DEFINES   := $(DEFINES)
endif

ifeq ($(TARGET_OS),win)
DEFINES   := $(DEFINES) \
             -D_CRT_SECURE_NO_WARNINGS \
             -DNEUROMORE_PLATFORM_WINDOWS
INCLUDES  := $(INCLUDES)
CXXFLAGS  := $(CXXFLAGS)
LINKFLAGS := $(LINKFLAGS) -Xlinker /SUBSYSTEM:CONSOLE -DLL
LINKLIBS  := $(LINKLIBS)
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
DEFINES   := $(DEFINES) -DNEUROMORE_PLATFORM_OSX
INCLUDES  := $(INCLUDES)
CXXFLAGS  := $(CXXFLAGS)
LINKFLAGS := $(LINKFLAGS)
LINKLIBS  := $(LINKLIBS)
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
DEFINES   := $(DEFINES) -DNEUROMORE_PLATFORM_LINUX
INCLUDES  := $(INCLUDES)
CXXFLAGS  := $(CXXFLAGS)
LINKFLAGS := $(LINKFLAGS)
LINKLIBS  := $(LINKLIBS)
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
DEFINES   := $(DEFINES) -DNEUROMORE_PLATFORM_ANDROID
CXXFLAGS  := $(CXXFLAGS)
LINKFLAGS := $(LINKFLAGS)
LINKLIBS  := $(LINKLIBS)
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

OBJS  := $(patsubst %,$(OBJDIR)/%,$(OBJS))
JOBJS := $(patsubst %,$(OBJDIR)/%,$(JOBJS))

$(OBJDIR)/neuromoreEngine.o:
	@echo [CXX] $(OBJDIR)/neuromoreEngine.o
	$(CXX) $(CPUFLAGS) $(DEFINES) $(INCLUDES) $(CXXFLAGS) \
	  -c $(SRCDIR)/../Engine/neuromoreEngine.cpp \
	  -o $(OBJDIR)/neuromoreEngine.o

$(OBJDIR)/%.o:
	@echo [CXX] $@
	$(CXX) $(CPUFLAGS) $(DEFINES) $(INCLUDES) $(CXXFLAGS) -c $(@:$(OBJDIR)%.o=$(SRCDIR)%.cpp) -o $@

.DEFAULT_GOAL := build

build: $(OBJDIR)/neuromoreEngine.o $(OBJS)
	@echo [LNK] $(LIBDIR)/$(NAME)$(SUFFIX)$(EXTDLL)
	$(LINK) $(LINKFLAGS) $(LINKPATH) $(OBJDIR)/neuromoreEngine.o $(OBJS) $(LINKLIBS) -o $(LIBDIR)/$(NAME)$(SUFFIX)$(EXTDLL)

clean:
	$(call deletefiles,$(OBJDIR),*.o)
	$(call deletefiles,$(LIBDIR),$(NAME)$(SUFFIX)$(EXTDLL))
	$(call deletefiles,$(LIBDIR),$(NAME)$(SUFFIX)$(EXTPDB))
