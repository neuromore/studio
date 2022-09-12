
include platforms/detect-host.mk

NAME       = brainflow-boardcontroller
INCDIR     = ../../include/
SRCDIR     = ../../src/$(NAME)
OBJDIR    := $(OBJDIR)/$(NAME)
DEFINES   := $(DEFINES)
INCLUDES  := $(INCLUDES) \
           -I$(INCDIR) \
           -I$(INCDIR)/neurosdk \
           -I$(INCDIR)/http \
           -I$(INCDIR)/json \
           -I$(INCDIR)/unicorn \
           -I$(INCDIR)/brainflow/board_controller \
           -I$(INCDIR)/brainflow/board_controller/freeeeg32 \
           -I$(INCDIR)/brainflow/board_controller/gtec \
           -I$(INCDIR)/brainflow/board_controller/ironbci \
           -I$(INCDIR)/brainflow/board_controller/mit \
           -I$(INCDIR)/brainflow/board_controller/neuromd \
           -I$(INCDIR)/brainflow/board_controller/neurosity \
           -I$(INCDIR)/brainflow/board_controller/openbci \
           -I$(INCDIR)/brainflow/board_controller/oymotion \
           -I$(INCDIR)/brainflow/ganglion-bglib \
           -I$(INCDIR)/brainflow/gforcesdkcxx-wrapper \
           -I$(INCDIR)/brainflow/utils
CXXFLAGS  := $(CXXFLAGS) -std=c++14
LINKFLAGS := $(LINKFLAGS)
LINKPATH  := $(LINKPATH)
LINKLIBS  := $(LINKLIBS)
OBJS       = freeeeg32/freeeeg32.o \
             gtec/unicorn_board.o \
             ironbci/ironbci.o \
             mit/fascia.o \
             neuromd/brainbit.o \
             neuromd/callibri.o \
             neuromd/callibri_ecg.o \
             neuromd/callibri_eeg.o \
             neuromd/callibri_emg.o \
             neuromd/neuromd_board.o \
             neurosity/notion_osc.o \
             openbci/cyton.o \
             openbci/cyton_daisy.o \
             openbci/cyton_daisy_wifi.o \
             openbci/cyton_wifi.o \
             openbci/galea.o \
             openbci/ganglion.o \
             openbci/ganglion_wifi.o \
             openbci/openbci_serial_board.o \
             openbci/openbci_wifi_shield_board.o \
             oymotion/gforce_pro.o \
             board.o \
             board_controller.o \
             board_info_getter.o \
             brainflow_boards.o \
             file_streamer.o \
             multicast_streamer.o \
             playback_file_board.o \
             streaming_board.o \
             synthetic_board.o

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
DEFINES   := $(DEFINES) -DNOMINMAX -D_CRT_SECURE_NO_WARNINGS -Wno-macro-redefined
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
