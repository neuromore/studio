
include platforms/detect-host.mk

NAME       = tonic
INCDIR     = ../../include/
SRCDIR     = ../../src/$(NAME)
OBJDIR    := $(OBJDIR)/$(NAME)
DEFINES   := $(DEFINES) -Wno-deprecated-declarations -Wno-implicit-const-int-float-conversion
INCLUDES  := $(INCLUDES) -I$(INCDIR) -I$(INCDIR)/$(NAME)
CXXFLAGS  := $(CXXFLAGS) -std=c++14
LINKFLAGS := $(LINKFLAGS)
LINKPATH  := $(LINKPATH)
LINKLIBS  := $(LINKLIBS)
OBJS       = ADSR.o \
             Arithmetic.o \
             AudioFileUtils.o \
             BasicDelay.o \
             BitCrusher.o \
             BLEPOscillator.o \
             BufferFiller.o \
             BufferPlayer.o \
             CombFilter.o \
             CompressorLimiter.o \
             ControlArithmetic.o \
             ControlCallback.o \
             ControlChangeNotifier.o \
             ControlComparison.o \
             ControlConditioner.o \
             ControlCounter.o \
             ControlDbToLinear.o \
             ControlDelay.o \
             ControlFloor.o \
             ControlGenerator.o \
             ControlMetro.o \
             ControlMetroDivider.o \
             ControlMidiToFreq.o \
             ControlParameter.o \
             ControlPrinter.o \
             ControlPulse.o \
             ControlRandom.o \
             ControlRecorder.o \
             ControlSnapToScale.o \
             ControlStepper.o \
             ControlSwitcher.o \
             ControlTrigger.o \
             ControlTriggerFilter.o \
             ControlValue.o \
             ControlXYSpeed.o \
             DelayUtils.o \
             DSPUtils.o \
             Effect.o \
             Filters.o \
			 FilterUtils.o \
			 FixedValue.o \
			 Generator.o \
			 LFNoise.o \
			 Mixer.o \
			 MonoToStereoPanner.o \
			 Noise.o \
			 RampedValue.o \
			 RectWave.o \
			 Reverb.o \
			 RingBuffer.o \
			 SampleTable.o \
			 SawtoothWave.o \
			 SineWave.o \
			 StereoDelay.o \
			 Synth.o \
			 TableLookupOsc.o \
			 TonicFrames.o

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
	$(CXX) $(CPUFLAGS) $(DEFINES) $(INCLUDES) $(CXXFLAGS) -c $(@:$(OBJDIR)%.o=$(SRCDIR)%.cpp) -o $@

.DEFAULT_GOAL := build

build: $(OBJS)
	@echo [AR]  $(LIBDIR)/$(NAME)$(SUFFIX)$(EXTLIB)
	$(AR) $(ARFLAGS) $(LIBDIR)/$(NAME)$(SUFFIX)$(EXTLIB) $(OBJS)

clean:
	$(call deletefiles,$(OBJDIR),*.o)
	$(call deletefiles,$(LIBDIR),$(NAME)$(SUFFIX)$(EXTLIB))
