
include ../../deps/build/make/platforms/detect-host.mk
include ../../deps/build/make/platforms/$(DETECTED_OS)-$(DETECTED_ARCH)-$(TARGET_OS)-$(TARGET_ARCH).mk

NAME       = EngineJNI
INCDIR     = ../../deps/include/
SRCDIR     = ../../src/$(NAME)
OBJDIR    := $(OBJDIR)/$(NAME)
LIBDIRDEP  = ../../deps/build/make/$(LIBDIR)
LIBDIRPRE  = ../../deps/prebuilt/$(TARGET_OS)/$(TARGET_ARCH)
DEFINES   := $(DEFINES) \
             -DUNICODE \
             -D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
INCLUDES  := $(INCLUDES) \
             -I"$(JAVA_HOME)/include" \
             -I"$(JAVA_HOME)/include/win32" \
             -I"$(JAVA_HOME)/include/linux" \
             -I"$(JAVA_HOME)/include/darwin" \
             -I$(INCDIR) \
             -I../../src/Engine \
             -I$(SRCDIR)
CXXFLAGS  := $(CXXFLAGS) \
             -Wno-unknown-warning-option \
             -Wno-deprecated-declarations \
             -Wno-enum-compare-switch \
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
OBJS       = neuromoreEngineJni.o
JOBJS      = Java/com/neuromore/engine/ICallback.class \
             Java/com/neuromore/engine/Wrapper.class \
             Java/com/neuromore/engine/enums/EAssetType.class \
             Java/com/neuromore/engine/enums/EDevice.class \
             Java/com/neuromore/engine/enums/EGenericDeviceInputs.class \
             Java/com/neuromore/engine/enums/EMuseInputs.class \
             Java/com/neuromore/engine/enums/EPowerLineFrequencyType.class \
             Java/com/neuromore/engine/enums/ESenseSkinResponseInputs.class \
             Java/com/neuromore/engine/enums/EStatus.class

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

$(OBJDIR)/%.o:
	$(CXX) $(CPUFLAGS) $(DEFINES) $(INCLUDES) $(CXXFLAGS) -c $(@:$(OBJDIR)%.o=$(SRCDIR)%.cpp) -o $@

$(OBJDIR)/%.class:
	$(JAVAC) -sourcepath $(SRCDIR)/Java -d $(OBJDIR)/Java $(@:$(OBJDIR)%.class=$(SRCDIR)%.java)

.DEFAULT_GOAL := build

build: $(OBJS) $(JOBJS)
	$(LINK) $(LINKFLAGS) $(LINKPATH) $(OBJS) $(LINKLIBS) -o $(LIBDIR)/$(NAME)$(SUFFIX)$(EXTDLL)
	$(JAR) cf $(LIBDIR)/$(NAME)$(SUFFIX).jar -C $(OBJDIR)/Java .

clean:
	$(call deletefiles,$(OBJDIR),*.o)
	$(call deletefiles,$(OBJDIR),*.class)
	$(call deletefiles,$(LIBDIR),$(NAME)$(SUFFIX)$(EXTDLL))
	$(call deletefiles,$(LIBDIR),$(NAME)$(SUFFIX)$(EXTPDB))
	$(call deletefiles,$(LIBDIR),$(NAME)$(SUFFIX).jar)
