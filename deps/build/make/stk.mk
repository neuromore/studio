
include platforms/detect-host.mk

NAME       = stk
INCDIR     = ../../include/
SRCDIR     = ../../src/$(NAME)
OBJDIR    := $(OBJDIR)/$(NAME)
DEFINES   := $(DEFINES) -Wno-deprecated-declarations 
INCLUDES  := $(INCLUDES) -I$(INCDIR) -I$(INCDIR)/$(NAME)
CXXFLAGS  := $(CXXFLAGS) -std=c++17
LINKFLAGS := $(LINKFLAGS)
LINKPATH  := $(LINKPATH)
LINKLIBS  := $(LINKLIBS)
OBJS       = ADSR.o \
             Asymp.o \
             BandedWG.o \
             BeeThree.o \
             BiQuad.o \
             Blit.o \
             BlitSaw.o \
             BlitSquare.o \
             BlowBotl.o \
             BlowHole.o \
             Bowed.o \
             Brass.o \
             Chorus.o \
             Clarinet.o \
             Delay.o \
             DelayA.o \
             DelayL.o \
             Drummer.o \
             Echo.o \
             Envelope.o \
             FileLoop.o \
             FileRead.o \
             FileWrite.o \
             FileWvIn.o \
             FileWvOut.o \
             Fir.o \
             Flute.o \
             FM.o \
             FMVoices.o \
             FormSwep.o \
             FreeVerb.o \
             Granulate.o \
             Guitar.o \
             HevyMetl.o \
             Iir.o \
             InetWvIn.o \
             InetWvOut.o \
             JCRev.o \
             LentPitShift.o \
             Mandolin.o \
             Mesh2D.o \
             Messager.o \
             MidiFileIn.o \
             Modal.o \
             ModalBar.o \
             Modulate.o \
             Moog.o \
             Mutex.o \
             Noise.o \
             NRev.o \
             OnePole.o \
             OneZero.o \
             PercFlut.o \
             Phonemes.o \
             PitShift.o \
             Plucked.o \
             PoleZero.o \
             PRCRev.o \
             Recorder.o \
             Resonate.o \
             Rhodey.o \
             RtAudio.o \
             RtMidi.o \
             RtWvIn.o \
             RtWvOut.o \
             Sampler.o \
             Saxofony.o \
             Shakers.o \
             Simple.o \
             SineWave.o \
             SingWave.o \
             Sitar.o \
             Skini.o \
             Socket.o \
             Sphere.o \
             StifKarp.o \
             Stk.o \
             TapDelay.o \
             TcpClient.o \
             TcpServer.o \
             Thread.o \
             TubeBell.o \
             Twang.o \
             TwoPole.o \
             TwoZero.o \
             UdpSocket.o \
             Voicer.o \
             VoicForm.o \
             Whistle.o \
             Wurley.o

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
DEFINES   := $(DEFINES) \
             -D_WIN32 \
             -D_USE_MATH_DEFINES \
             -D__WINDOWS_MM__ \
             -D__WINDOWS_DS__
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
DEFINES   := $(DEFINES) \
             -DMICROSOFT_WINDOWS_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS=0
endif
endif

ifeq ($(TARGET_OS),osx)
DEFINES   := $(DEFINES) \
             -D__MACOSX_CORE__
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
DEFINES   := $(DEFINES) \
             -D__LINUX_PULSE__ \
             -D__LINUX_ALSA__
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
