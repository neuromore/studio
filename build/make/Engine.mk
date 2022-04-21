
include ../../deps/build/make/platforms/detect-host.mk
include ../../deps/build/make/platforms/$(DETECTED_OS)-$(DETECTED_ARCH)-$(TARGET_OS)-$(TARGET_ARCH).mk

NAME       = Engine
INCDIR     = ../../deps/include/
SRCDIR     = ../../src/$(NAME)
OBJDIR    := $(OBJDIR)/$(NAME)
DEFINES   := $(DEFINES) \
             -DUNICODE \
             -D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
INCLUDES  := $(INCLUDES) \
             -I$(INCDIR) \
             -I$(INCDIR)/brainflow/utils \
             -I$(INCDIR)/brainflow/board_controller \
             -I../../src \
             -I$(SRCDIR)
CXXFLAGS  := $(CXXFLAGS) \
             -Wno-unknown-warning-option \
             -Wno-deprecated-declarations \
             -Wno-enum-compare-switch \
             -std=c++17
LINKFLAGS := $(LINKFLAGS)
LINKPATH  := $(LINKPATH)
LINKLIBS  := $(LINKLIBS)
PCH        = Precompiled
OBJS       = Core/AABB.o \
             Core/AES.o \
             Core/AttributeFactory.o \
             Core/AttributeSet.o \
             Core/AttributeSettings.o \
             Core/ByteArray.o \
             Core/Color.o \
             Core/Counter.o \
             Core/EncryptedJSONFile.o \
             Core/EventManager.o \
             Core/FpsCounter.o \
             Core/Json.o \
             Core/LogCallbacks.o \
             Core/LogManager.o \
             Core/Math.o \
             Core/MemoryFile.o \
             Core/Mutex.o \
             Core/String.o \
             Core/StringCharacter.o \
             Core/StringIterator.o \
             Core/Test.o \
             Core/TestFacility.o \
             Core/Thread.o \
             Core/ThreadHandler.o \
             Core/Time.o \
             Core/Version.o \
             Devices/ABM/AbmDevices.o \
             Devices/Audio/AudioDevices.o \
             Devices/Audio/AudioNodes.o \
             Devices/BrainFlow/BrainFlowDevices.o \
             Devices/BrainFlow/BrainFlowNodes.o \
             Devices/BrainMaster/BrainMasterDevices.o \
             Devices/BrainProducts/ActiChampDevice.o \
             Devices/Brainquiry/BrainquiryDevice.o \
             Devices/Emotiv/EmotivEPOCDevice.o \
             Devices/Emotiv/EmotivInsightDevice.o \
             Devices/eSense/eSenseSkinResponseDevice.o \
             Devices/eemagine/eemagineDevices.o \
             Devices/EyeX/TobiiEyeXDevice.o \
             Devices/EyeX/TobiiEyeXNode.o \
             Devices/Gamepad/GamepadDevice.o \
             Devices/Mitsar/MitsarDevices.o \
             Devices/Muse/MuseDevice.o \
             Devices/Neurosity/NotionDevices.o \
             Devices/NeuroSky/NeuroSkyDevice.o \
             Devices/OpenBCI/OpenBCIDevices.o \
             Devices/Test/TestDevice.o \
             Devices/Test/TestDeviceDriver.o \
             Devices/Versus/VersusDevice.o \
             Devices/DeviceInventory.o \
             DSP/AttributeChannels.o \
             DSP/Channel.o \
             DSP/ChannelBase.o \
             DSP/ChannelFileReader.o \
             DSP/ChannelFileWriter.o \
             DSP/ChannelProcessor.o \
             DSP/ChannelReader.o \
             DSP/ClockGenerator.o \
             DSP/Epoch.o \
             DSP/FFT_FFTW.o \
             DSP/FFT_KissFFT.o \
             DSP/FFTProcessor.o \
             DSP/Filter.o \
             DSP/FilterGenerator.o \
             DSP/FrequencyBand.o \
             DSP/Histogram.o \
             DSP/HrvProcessor.o \
             DSP/HrvTimeDomain.o \
             DSP/LinearFilterProcessor.o \
             DSP/MultiChannel.o \
             DSP/MultiChannelReader.o \
             DSP/ResampleProcessor.o \
             DSP/Spectrum.o \
             DSP/SpectrumAnalyzerSettings.o \
             DSP/StatisticsProcessor.o \
             DSP/WindowFunction.o \
             Graph/Action.o \
             Graph/Actions.o \
             Graph/ActionSet.o \
             Graph/ActionState.o \
             Graph/AnnotationNode.o \
             Graph/AutoThresholdNode.o \
             Graph/AVEColorNode.o \
             Graph/BinSelectorNode.o \
             Graph/BiquadFilterNode.o \
             Graph/BodyFeedbackNode.o \
             Graph/ChannelInfoNode.o \
             Graph/ChannelMathNode.o \
             Graph/ChannelMergerNode.o \
             Graph/ChannelSelectorNode.o \
             Graph/ChannelTransposerNode.o \
             Graph/Classifier.o \
             Graph/CloudInputNode.o \
             Graph/CloudOutputNode.o \
             Graph/ColorWheelNode.o \
             Graph/CompareNode.o \
             Graph/Connection.o \
             Graph/CustomFeedbackNode.o \
             Graph/DelayNode.o \
             Graph/DeviceInputNode.o \
             Graph/DeviceOutputNode.o \
             Graph/DominantFrequencyNode.o \
             Graph/EegDeviceNode.o \
             Graph/EntryState.o \
             Graph/ExitState.o \
             Graph/FeedbackNode.o \
             Graph/FFTNode.o \
             Graph/FileReaderNode.o \
             Graph/FileWriterNode.o \
             Graph/FreezeNode.o \
             Graph/FrequencyBandNode.o \
             Graph/Graph.o \
             Graph/GraphExporter.o \
             Graph/GraphImporter.o \
             Graph/GraphManager.o \
             Graph/GraphObject.o \
             Graph/GraphObjectFactory.o \
             Graph/GraphSettings.o \
             Graph/HrvNode.o \
             Graph/InputNode.o \
             Graph/IntegralNode.o \
             Graph/LinearFilterNode.o \
             Graph/LogicNode.o \
             Graph/LoretaNode.o \
             Graph/Math1Node.o \
             Graph/Math2Node.o \
             Graph/MetaInfoNode.o \
             Graph/MultiParameterNode.o \
             Graph/Node.o \
             Graph/OscillatorNode.o \
             Graph/OscInputNode.o \
             Graph/OscOutputNode.o \
             Graph/OutputNode.o \
             Graph/PairwiseMathNode.o \
             Graph/ParameterNode.o \
             Graph/PointsNode.o \
             Graph/Port.o \
             Graph/ProcessorNode.o \
             Graph/RecolorNode.o \
             Graph/RemapNode.o \
             Graph/RenameNode.o \
             Graph/ResampleNode.o \
             Graph/SampleGateNode.o \
             Graph/SessionInfoNode.o \
             Graph/SessionTimeNode.o \
             Graph/SignalGeneratorNode.o \
             Graph/SmoothNode.o \
             Graph/SPNode.o \
             Graph/State.o \
             Graph/StateMachine.o \
             Graph/StateTransition.o \
             Graph/StateTransitionAudioCondition.o \
             Graph/StateTransitionButtonCondition.o \
             Graph/StateTransitionCondition.o \
             Graph/StateTransitionFeedbackCondition.o \
             Graph/StateTransitionTimeCondition.o \
             Graph/StateTransitionVideoCondition.o \
             Graph/StatisticsNode.o \
             Graph/SwitchNode.o \
             Graph/SwitchState.o \
             Graph/SyncState.o \
             Graph/ThresholdNode.o \
             Graph/TimerState.o \
             Graph/ViewNode.o \
             Graph/WaveformNode.o \
             Networking/OscFeedbackPacket.o \
             Networking/OscMessageParser.o \
             Networking/OscMessageQueue.o \
             Networking/OscMessageRouter.o \
             Networking/OscPacket.o \
             Networking/OscPacketParser.o \
             Networking/OscPacketPool.o \
             BciDevice.o \
             CloudParameters.o \
             ColorMapper.o \
             Creud.o \
             Device.o \
             DeviceDriver.o \
             DeviceManager.o \
             EEGElectrodes.o \
             EngineManager.o \
             Experience.o \
             License.o \
             neuromoreEngine.o \
             Sensor.o \
             SerialPortManager.o \
             Session.o \
             SessionExporter.o \
             User.o

################################################################################################
# BRANDINGS

ifeq ($(BRANDING),)
BRANDING = neuromore
endif

ifeq ($(BRANDING),neuromore)
DEFINES   := $(DEFINES)
endif

ifeq ($(BRANDING),ant)
DEFINES   := $(DEFINES) \
             -DNEUROMORE_BRANDING_ANT
endif

ifeq ($(BRANDING),starrbase)
DEFINES   := $(DEFINES) \
             -DNEUROMORE_BRANDING_STARRBASE
endif

################################################################################################
# CPU

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

################################################################################################
# WINDOWS
ifeq ($(TARGET_OS),win)
DEFINES   := $(DEFINES) \
             -D_CRT_SECURE_NO_WARNINGS \
             -DNEUROMORE_PLATFORM_WINDOWS
CXXFLAGS  := $(CXXFLAGS)
OBJS      := $(OBJS) Core/TimerWindows.o
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

################################################################################################
# OSX
ifeq ($(TARGET_OS),osx)
DEFINES   := $(DEFINES) -DNEUROMORE_PLATFORM_OSX
CXXFLAGS  := $(CXXFLAGS)
OBJS      := $(OBJS) Core/TimermacOS.o
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

################################################################################################
# LINUX
ifeq ($(TARGET_OS),linux)
DEFINES   := $(DEFINES) -DNEUROMORE_PLATFORM_LINUX
CXXFLAGS  := $(CXXFLAGS)
OBJS      := $(OBJS) Core/TimerLinux.o
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

################################################################################################
# ANDROID
ifeq ($(TARGET_OS),android)
DEFINES   := $(DEFINES) -DNEUROMORE_PLATFORM_ANDROID
CXXFLAGS  := $(CXXFLAGS)
OBJS      := $(OBJS) Core/TimerAndroid.o
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


################################################################################################
# MODE

ifeq ($(MODE),debug)
DEFINES := $(DEFINES)
else
DEFINES := $(DEFINES) -DPRODUCTION_BUILD
endif

################################################################################################

pch:
	@echo [PCH] $(OBJDIR)/$(PCH).pch
	$(CXX) $(CPUFLAGS) $(DEFINES) $(INCLUDES) $(CXXFLAGS) -x c++-header -c $(SRCDIR)/$(PCH).h -o $(OBJDIR)/$(PCH).pch

OBJS := $(patsubst %,$(OBJDIR)/%,$(OBJS))

$(OBJDIR)/%.o:
	@echo [CXX] $@
	$(CXX) $(CPUFLAGS) $(DEFINES) $(INCLUDES) $(CXXFLAGS) -Xclang -include-pch -Xclang $(OBJDIR)/$(PCH).pch -c $(@:$(OBJDIR)%.o=$(SRCDIR)%.cpp) -o $@

$(OBJS) : pch

build: pch $(OBJS)
	@echo [AR]  $(LIBDIR)/$(NAME)$(SUFFIX)$(EXTLIB)
	$(AR) $(ARFLAGS) $(LIBDIR)/$(NAME)$(SUFFIX)$(EXTLIB) $(OBJS)

clean:
	$(call deletefiles,$(OBJDIR),*.o)
	$(call deletefiles,$(OBJDIR),$(PCH).pch)
	$(call deletefiles,$(LIBDIR),$(NAME)$(SUFFIX)$(EXTLIB))

.DEFAULT_GOAL := build
