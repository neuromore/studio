
include ../../deps/build/make/platforms/detect-host.mk

NAME       = Studio
TARGET     = $(BINDIR)/$(NAME)$(SUFFIX)$(EXTBIN)
INCDIR     = ../../deps/include
SRCDIR     = ../../src/$(NAME)
INCDIRQT   = $(SRCDIR)
MOCDIR     = $(SRCDIR)/.moc
RCCDIR     = $(SRCDIR)/.rcc
UICDIR     = $(SRCDIR)/.uic
OBJDIR    := $(OBJDIR)/$(NAME)
QTMOC     := "../../deps/build/make/$(QTMOC)" --no-notes
QTRCC     := "../../deps/build/make/$(QTRCC)" 
QTUIC     := "../../deps/build/make/$(QTUIC)"
BINDIRDEP  = "../../deps/build/make/$(BINDIR)"
LIBDIRDEP  = "../../deps/build/make/$(LIBDIR)"
LIBDIRPRE  = "../../deps/prebuilt/$(TARGET_OS)/$(TARGET_ARCH)"
DEFINES   := $(DEFINES) \
             -DCHROMIUM_ZLIB_NO_CHROMECONF \
             -DQT_STATIC \
             -DQT_LINKED_OPENSSL \
             -DOPENSSL_STATIC \
             -DOPENSSL_NO_DYNAMIC_ENGINE \
             -DPCRE2_STATIC \
             -DPCRE2_CODE_UNIT_WIDTH=16 \
             -DHAVE_CONFIG_H \
             -DUNICODE \
             -D_UNICODE \
             -D_ENABLE_EXTENDED_ALIGNED_STORAGE \
             -D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
INCLUDES  := $(INCLUDES) \
             -I../../src/ \
             -I../../src/Engine \
             -I../../src/QtBase \
             -I$(INCDIR) \
             -I$(INCDIR)/brainflow/board_controller \
             -I$(INCDIR)/brainflow/data_handler \
             -I$(INCDIR)/brainflow/utils \
             -I$(INCDIR)/json \
             -I$(INCDIR)/neurosdk \
             -I$(INCDIR)/unicorn \
             -I$(SRCDIR) \
             -I$(UICDIR) \
             -I$(MOCDIR) \
             -I$(INCDIR)/qt \
             -I$(INCDIR)/qt/QtBluetooth \
             -I$(INCDIR)/qt/QtCore \
             -I$(INCDIR)/qt/QtGui \
             -I$(INCDIR)/qt/QtMultimedia \
             -I$(INCDIR)/qt/QtNetwork \
             -I$(INCDIR)/qt/QtWidgets
CXXFLAGS  := $(CXXFLAGS) \
             -Wno-deprecated-declarations \
             -Wno-unknown-warning-option \
             -std=c++17
CFLAGS    := $(CFLAGS)
LINKFLAGS := $(LINKFLAGS)
LINKPATH  := $(LINKPATH) \
             -L$(LIBDIRDEP) \
             -L$(LIBDIRPRE)
LINKLIBS  := $(LINKLIBS) \
             $(LIBDIR)/Engine$(SUFFIX)$(EXTLIB) \
             $(LIBDIR)/QtBase$(SUFFIX)$(EXTLIB) \
             $(LIBDIRDEP)/singleapplication$(SUFFIX)$(EXTLIB) \
             $(LIBDIRDEP)/qt-bluetooth$(SUFFIX)$(EXTLIB) \
             $(LIBDIRDEP)/qt-eventdispatchers$(SUFFIX)$(EXTLIB) \
             $(LIBDIRDEP)/qt-gamepad$(SUFFIX)$(EXTLIB) \
             $(LIBDIRDEP)/qt-multimediaplugins$(SUFFIX)$(EXTLIB) \
             $(LIBDIRDEP)/qt-multimediawidgets$(SUFFIX)$(EXTLIB) \
             $(LIBDIRDEP)/qt-multimedia$(SUFFIX)$(EXTLIB) \
             $(LIBDIRDEP)/qt-network$(SUFFIX)$(EXTLIB) \
             $(LIBDIRDEP)/qt-opengl$(SUFFIX)$(EXTLIB) \
             $(LIBDIRDEP)/qt-fontdatabases$(SUFFIX)$(EXTLIB) \
             $(LIBDIRDEP)/qt-printsupport$(SUFFIX)$(EXTLIB) \
             $(LIBDIRDEP)/qt-serialport$(SUFFIX)$(EXTLIB) \
             $(LIBDIRDEP)/qt-websockets$(SUFFIX)$(EXTLIB) \
             $(LIBDIRDEP)/qt-widgets$(SUFFIX)$(EXTLIB) \
             $(LIBDIRDEP)/qt-gui$(SUFFIX)$(EXTLIB) \
             $(LIBDIRDEP)/qt-xml$(SUFFIX)$(EXTLIB) \
             $(LIBDIRDEP)/qt-concurrent$(SUFFIX)$(EXTLIB) \
             $(LIBDIRDEP)/qt-core$(SUFFIX)$(EXTLIB) \
             $(LIBDIRDEP)/double-conversion$(SUFFIX)$(EXTLIB) \
             $(LIBDIRDEP)/edflib$(SUFFIX)$(EXTLIB) \
             $(LIBDIRDEP)/freetype$(SUFFIX)$(EXTLIB) \
             $(LIBDIRDEP)/harfbuzz$(SUFFIX)$(EXTLIB) \
             $(LIBDIRDEP)/harfbuzz-ng$(SUFFIX)$(EXTLIB) \
             $(LIBDIRDEP)/kissfft$(SUFFIX)$(EXTLIB) \
             $(LIBDIRDEP)/libssl$(SUFFIX)$(EXTLIB) \
             $(LIBDIRDEP)/libcrypto$(SUFFIX)$(EXTLIB) \
             $(LIBDIRDEP)/brainflow-boardcontroller$(SUFFIX)$(EXTLIB) \
             $(LIBDIRDEP)/brainflow$(SUFFIX)$(EXTLIB) \
             $(LIBDIRDEP)/opencv-core$(SUFFIX)$(EXTLIB) \
             $(LIBDIRDEP)/opencv-imgcodecs$(SUFFIX)$(EXTLIB) \
             $(LIBDIRDEP)/opencv-imgproc$(SUFFIX)$(EXTLIB) \
             $(LIBDIRDEP)/opencv-videoio$(SUFFIX)$(EXTLIB) \
             $(LIBDIRDEP)/minizip$(SUFFIX)$(EXTLIB) \
             $(LIBDIRDEP)/dspfilters$(SUFFIX)$(EXTLIB) \
             $(LIBDIRDEP)/wavelib$(SUFFIX)$(EXTLIB) \
             $(LIBDIRDEP)/oscpack$(SUFFIX)$(EXTLIB) \
             $(LIBDIRDEP)/pcre2$(SUFFIX)$(EXTLIB) \
             $(LIBDIRDEP)/tinyaes$(SUFFIX)$(EXTLIB) \
             $(LIBDIRDEP)/tinyobjloader$(SUFFIX)$(EXTLIB) \
             $(LIBDIRDEP)/tinyxml$(SUFFIX)$(EXTLIB) \
             $(LIBDIRDEP)/libjpeg$(SUFFIX)$(EXTLIB) \
             $(LIBDIRDEP)/libpng$(SUFFIX)$(EXTLIB) \
             $(LIBDIRDEP)/libsvm$(SUFFIX)$(EXTLIB) \
             $(LIBDIRDEP)/zlib$(SUFFIX)$(EXTLIB)
PCH        = Precompiled
MOCH       = Devices/Bluetooth/BluetoothDevice.cpp \
             Devices/Bluetooth/BluetoothDriver.cpp \
             Devices/Bluetooth/BluetoothService.cpp \
             Devices/Brainquiry/BrainquiryDriver.cpp \
             Devices/Brainquiry/BrainquirySerialHandler.cpp \
             Devices/eemagine/eemagineDriver.cpp \
             Devices/NeuroSky/NeuroSkyDriver.cpp \
             Devices/NeuroSky/NeuroSkySerialHandler.cpp \
             Devices/OpenBCI/OpenBCIDriver.cpp \
             Devices/OpenBCI/OpenBCISerialHandler.cpp \
             Devices/Versus/VersusDriver.cpp \
             Devices/Versus/VersusSerialHandler.cpp \
             Plugins/ABM/ABMPlugin.cpp \
             Plugins/ABM/ABMProgressWidget.cpp \
             Plugins/BackendFileSystem/BackendFileSystemPlugin.cpp \
             Plugins/BackendFileSystem/BackendFileSystemWidget.cpp \
             Plugins/BackendFileSystem/CreateFileWindow.cpp \
             Plugins/BackendFileSystem/CreateFolderWindow.cpp \
             Plugins/Development/EEGElectrodePlacementPlugin/EEGElectrodePlacementPlugin.cpp \
             Plugins/Development/HeatmapPlugin/HeatmapPlugin.cpp \
             Plugins/Development/HeatmapPlugin/HeatmapWidget.cpp \
             Plugins/Development/LatencyTestPlugin/LatencyTestPlugin.cpp \
             Plugins/Development/LatencyTestPlugin/LatencyTestWidget.cpp \
             Plugins/Devices/BciDeviceWidget.cpp \
             Plugins/Devices/DevicesPlugin.cpp \
             Plugins/Devices/DevicesWidget.cpp \
             Plugins/Devices/DeviceWidget.cpp \
             Plugins/Devices/NoDeviceWidget.cpp \
             Plugins/EngineStatus/EngineStatusPlugin.cpp \
             Plugins/Examples/SimpleOpenGLPlugin/ExampleOpenGLPlugin.cpp \
             Plugins/Examples/SimpleOpenGLPlugin/ExampleOpenGLWidget.cpp \
             Plugins/Experience/ExperiencePlugin.cpp \
             Plugins/Experience/ExperienceWidget.cpp \
             Plugins/ExperienceSelection/ExperienceSelectionPlugin.cpp \
             Plugins/ExperienceSelection/ExperienceSelectionWidget.cpp \
             Plugins/Feedback/FeedbackHistoryWidget.cpp \
             Plugins/Feedback/FeedbackPlugin.cpp \
             Plugins/Graph/ClassifierPlugin.cpp \
             Plugins/Graph/GraphAttributesWidget.cpp \
             Plugins/Graph/GraphInfoWidget.cpp \
             Plugins/Graph/GraphPaletteWidget.cpp \
             Plugins/Graph/GraphPlugin.cpp \
             Plugins/Graph/GraphShared.cpp \
             Plugins/Graph/GraphTextPixmapCache.cpp \
             Plugins/Graph/GraphWidget.cpp \
             Plugins/Graph/StateMachinePlugin.cpp \
             Plugins/Networking/NetworkTestClientPlugin.cpp \
             Plugins/Parameter/ParameterControlPlugin.cpp \
             Plugins/Parameter/ParametersWidget.cpp \
             Plugins/Parameter/ParameterWidgets.cpp \
             Plugins/RawWaveform/RawWaveformPlugin.cpp \
             Plugins/RawWaveform/RawWaveformWidget.cpp \
             Plugins/SessionControl/ClientInfoWidget.cpp \
             Plugins/SessionControl/PreSessionWidget.cpp \
             Plugins/SessionControl/SessionControlPlugin.cpp \
             Plugins/SessionControl/SessionInfoWidget.cpp \
             Plugins/SessionControl/StageControlWidget.cpp \
             Plugins/SessionControl/WhileSessionWidget.cpp \
             Plugins/Spectrogram/SpectrogramBandsPlugin.cpp \
             Plugins/Spectrogram/SpectrogramPlugin.cpp \
             Plugins/Spectrogram/SpectrogramPluginCollection.cpp \
             Plugins/Spectrogram/SpectrogramSurfacePlugin.cpp \
             Plugins/Spectrogram2D/Spectrogram2DPlugin.cpp \
             Plugins/Spectrogram2D/Spectrogram2DWidget.cpp \
             Plugins/SpectrumAnalyzerSettings/SpectrumAnalyzerSettingsPlugin.cpp \
             Plugins/SpectrumAnalyzerSettings/SpectrumAnalyzerSettingsWidget.cpp \
             Plugins/View/ViewPlugin.cpp \
             Plugins/View/ViewSpectrumPlugin.cpp \
             Plugins/View/ViewSpectrumWidget.cpp \
             Plugins/View/ViewWidget.cpp \
             Plugins/DataVisualizationPlugin.cpp \
             Plugins/DataVisualizationSettingsWidget.cpp \
             Rendering/OpenGLManager.cpp \
             Rendering/OpenGLWidget.cpp \
             Widgets/BatteryStatusWidget.cpp \
             Widgets/BreathingRateWidget.cpp \
             Widgets/ChannelMultiSelectionWidget.cpp \
             Widgets/ChannelSelectionWidget.cpp \
             Widgets/DeviceSelectionWidget.cpp \
             Widgets/EEGElectrodesWidget.cpp \
             Widgets/FrequencyBandCheckboxWidget.cpp \
             Widgets/HeartRateWidget.cpp \
             Widgets/HMultiCheckboxWidget.cpp \
             Widgets/ImpedanceTestWidget.cpp \
             Widgets/SensorCheckboxWidget.cpp \
             Widgets/SignalQualityWidget.cpp \
             Widgets/SkinTemperatureWidget.cpp \
             Widgets/StopwatchWidget.cpp \
             Widgets/SwitchingImage.cpp \
             Widgets/ThermometerWidget.cpp \
             Windows/AboutWindow.cpp \
             Windows/CreateUserWindow.cpp \
             Windows/EmailValidator.cpp \
             Windows/ExperienceWizardWindow.cpp \
             Windows/InviteUserWindow.cpp \
             Windows/LicenseAgreement.cpp \
             Windows/LicenseAgreementWindow.cpp \
             Windows/LoginWindow.cpp \
             Windows/ReportWindow.cpp \
             Windows/SelectUserWindow.cpp \
             Windows/SettingsWindow.cpp \
             Windows/UpgradeWindow.cpp \
             Windows/VisualizationSelectWindow.cpp \
             AppManager.cpp \
             AuthenticationCenter.cpp \
             MainWindow.cpp \
             VideoPlayer.cpp \
             VisualizationManager.cpp \
             OnboardingAction.cpp \
             TourManager.cpp
MOCC       =
MOCO       = $(patsubst %.cpp,%.omoc,$(MOCH))
RCCH       = Resources/StudioResources.cpp
RCCO       = StudioResources.orcc
UICH       =
RESO       =
OBJS       = Devices/ABM/AbmDriver.o \
             Devices/ABM/AbmThreadHandler.o \
             Devices/Audio/AudioDriver.o \
             Devices/Audio/QtAudioDevices.o \
             Devices/Bluetooth/BluetoothDevice.o \
             Devices/Bluetooth/BluetoothDriver.o \
             Devices/Bluetooth/BluetoothService.o \
             Devices/BrainFlow/BrainFlowDriver.o \
             Devices/BrainMaster/BrainMasterDriver.o \
             Devices/BrainMaster/Discovery20.o \
             Devices/BrainProducts/ActiChampDriver.o \
             Devices/Brainquiry/BrainquiryDriver.o \
             Devices/Brainquiry/BrainquirySerialHandler.o \
             Devices/eemagine/eemagineDriver.o \
             Devices/Emotiv/EmotivDriver.o \
             Devices/EyeX/TobiiEyeXDriver.o \
             Devices/Mitsar/MitsarDriver.o \
             Devices/NeuroSky/NeuroSkyDriver.o \
             Devices/NeuroSky/NeuroSkySerialHandler.o \
             Devices/OpenBCI/OpenBCIDriver.o \
             Devices/OpenBCI/OpenBCISerialHandler.o \
             Devices/Versus/VersusDriver.o \
             Devices/Versus/VersusSerialHandler.o \
             Devices/DeviceHelpers.o \
             Devices/DriverInventory.o \
             Plugins/ABM/ABMPlugin.o \
             Plugins/ABM/ABMProgressWidget.o \
             Plugins/BackendFileSystem/BackendFileSystemPlugin.o \
             Plugins/BackendFileSystem/BackendFileSystemWidget.o \
             Plugins/BackendFileSystem/CreateFileWindow.o \
             Plugins/BackendFileSystem/CreateFolderWindow.o \
             Plugins/Development/EEGElectrodePlacementPlugin/EEGElectrodePlacementPlugin.o \
             Plugins/Development/HeatmapPlugin/HeatmapPlugin.o \
             Plugins/Development/HeatmapPlugin/HeatmapWidget.o \
             Plugins/Development/LatencyTestPlugin/LatencyTestPlugin.o \
             Plugins/Development/LatencyTestPlugin/LatencyTestWidget.o \
             Plugins/Devices/BciDeviceWidget.o \
             Plugins/Devices/DevicesPlugin.o \
             Plugins/Devices/DevicesWidget.o \
             Plugins/Devices/DeviceWidget.o \
             Plugins/Devices/NoDeviceWidget.o \
             Plugins/EngineStatus/EngineStatusPlugin.o \
             Plugins/Examples/SimpleOpenGLPlugin/ExampleOpenGLPlugin.o \
             Plugins/Examples/SimpleOpenGLPlugin/ExampleOpenGLWidget.o \
             Plugins/Experience/ExperiencePlugin.o \
             Plugins/Experience/ExperienceWidget.o \
             Plugins/ExperienceSelection/ExperienceSelectionPlugin.o \
             Plugins/ExperienceSelection/ExperienceSelectionWidget.o \
             Plugins/Feedback/FeedbackHistoryWidget.o \
             Plugins/Feedback/FeedbackPlugin.o \
             Plugins/Graph/ClassifierPlugin.o \
             Plugins/Graph/GraphAttributesWidget.o \
             Plugins/Graph/GraphHelpers.o \
             Plugins/Graph/GraphInfoWidget.o \
             Plugins/Graph/GraphPaletteWidget.o \
             Plugins/Graph/GraphPlugin.o \
             Plugins/Graph/GraphRenderer.o \
             Plugins/Graph/GraphRendererState.o \
             Plugins/Graph/GraphShared.o \
             Plugins/Graph/GraphTextPixmapCache.o \
             Plugins/Graph/GraphWidget.o \
             Plugins/Graph/StateMachinePlugin.o \
             Plugins/Networking/NetworkTestClientPlugin.o \
             Plugins/Parameter/ParameterControlPlugin.o \
             Plugins/Parameter/ParametersWidget.o \
             Plugins/Parameter/ParameterWidgets.o \
             Plugins/RawWaveform/RawWaveformPlugin.o \
             Plugins/RawWaveform/RawWaveformWidget.o \
             Plugins/SessionControl/ClientInfoWidget.o \
             Plugins/SessionControl/PreSessionWidget.o \
             Plugins/SessionControl/SessionControlPlugin.o \
             Plugins/SessionControl/SessionInfoWidget.o \
             Plugins/SessionControl/StageControlWidget.o \
             Plugins/SessionControl/WhileSessionWidget.o \
             Plugins/Spectrogram/SpectrogramBandsPlugin.o \
             Plugins/Spectrogram/SpectrogramPlugin.o \
             Plugins/Spectrogram/SpectrogramSurfacePlugin.o \
             Plugins/Spectrogram2D/Spectrogram2DPlugin.o \
             Plugins/Spectrogram2D/Spectrogram2DWidget.o \
             Plugins/SpectrumAnalyzerSettings/SpectrumAnalyzerSettingsPlugin.o \
             Plugins/SpectrumAnalyzerSettings/SpectrumAnalyzerSettingsWidget.o \
             Plugins/View/ViewPlugin.o \
             Plugins/View/ViewSpectrumPlugin.o \
             Plugins/View/ViewSpectrumWidget.o \
             Plugins/View/ViewWidget.o \
             Plugins/DataVisualizationPlugin.o \
             Plugins/DataVisualizationSettingsWidget.o \
             Rendering/Mesh.o \
             Rendering/OpenGLManager.o \
             Rendering/OpenGLWidget.o \
             Rendering/OpenGLWidget2DHelpers.o \
             Rendering/OpenGLWidgetCallback.o \
             Rendering/TextureManager.o \
             UnitTests/StudioTestFacility.o \
             Widgets/BatteryStatusWidget.o \
             Widgets/BreathingRateWidget.o \
             Widgets/ChannelMultiSelectionWidget.o \
             Widgets/ChannelSelectionWidget.o \
             Widgets/DeviceSelectionWidget.o \
             Widgets/EEGElectrodesWidget.o \
             Widgets/FrequencyBandCheckboxWidget.o \
             Widgets/HeartRateWidget.o \
             Widgets/HMultiCheckboxWidget.o \
             Widgets/ImpedanceTestWidget.o \
             Widgets/SensorCheckboxWidget.o \
             Widgets/SignalQualityWidget.o \
             Widgets/SkinTemperatureWidget.o \
             Widgets/StopwatchWidget.o \
             Widgets/SwitchingImage.o \
             Widgets/ThermometerWidget.o \
             Windows/AboutWindow.o \
             Windows/CreateUserWindow.o \
             Windows/EmailValidator.o \
             Windows/ExperienceWizardWindow.o \
             Windows/InviteUserWindow.o \
             Windows/LicenseAgreement.o \
             Windows/LicenseAgreementWindow.o \
             Windows/LoginWindow.o \
             Windows/ReportWindow.o \
             Windows/SelectUserWindow.o \
             Windows/SettingsWindow.o \
             Windows/UpgradeWindow.o \
             Windows/VisualizationSelectWindow.o \
             AppManager.o \
             AuthenticationCenter.o \
             DocumentationExporter.o \
             main.o \
             MainWindow.o \
             VideoPlayer.o \
             Visualization.o \
             VisualizationManager.o \
             OnboardingAction.o \
             TourManager.o

################################################################################################
# Version

VERSIONFILE       = $(SRCDIR)/Version.h
VERSIONMACROMAJOR = NEUROMORE_STUDIO_VERSION_MAJOR
VERSIONMACROMINOR = NEUROMORE_STUDIO_VERSION_MINOR
VERSIONMACROPATCH = NEUROMORE_STUDIO_VERSION_PATCH

################################################################################################
# BRANDINGS

ifeq ($(BRANDING),)
BRANDING = neuromore
endif

ifeq ($(BRANDING),neuromore)
DEFINES   := $(DEFINES) \
             -DAPPNAME="neuromoreStudio" \
             -DAPPICON="AppIcon-neuromore.ico"
endif

ifeq ($(BRANDING),ant)
DEFINES   := $(DEFINES) \
             -DNEUROMORE_BRANDING_ANT \
             -DAPPNAME="eego perform studio" \
             -DAPPICON="AppIcon-ANT.ico"
endif

ifeq ($(BRANDING),starrbase)
DEFINES   := $(DEFINES) \
             -DNEUROMORE_BRANDING_STARRBASE \
             -DAPPNAME="Starrbase" \
             -DAPPICON="AppIcon-Starrbase.ico"
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
OUTDIST   := $(DISTDIR)/$(NAME)-$(TARGET_ARCH)/$(NAME)$(EXTBIN)
QTMOC     := $(QTMOC) -DQ_OS_WIN
DEFINES   := $(DEFINES) \
             -D_CRT_SECURE_NO_WARNINGS \
             -DUSE_WINTHREAD \
             -DNEUROMORE_PLATFORM_WINDOWS \
			 -DQT_QPA_DEFAULT_PLATFORM_NAME=\"windows\"
CXXFLAGS  := $(CXXFLAGS)
CFLAGS    := $(CFLAGS)
INCLUDES  := $(INCLUDES) -I$(INCDIR)/qt/mkspecs/win32-clang-msvc
MOCH      := $(MOCH)
MOCC      := $(MOCC)
MOCO      := $(MOCO)
RCCH      := $(RCCH)
RCCO      := $(RCCO)
UICH      := $(UICH)
RESO      := $(RESO) Resources/NMStudio.res
OBJS      := $(OBJS)
LINKFLAGS := $(LINKFLAGS)
LINKLIBS  := $(LINKLIBS) \
             $(LIBDIRDEP)/qt-platform-windows$(SUFFIX)$(EXTLIB) \
             -lversion.lib \
             -lbthprops.lib \
             -lsetupapi.lib \
             -lvfw32.lib \
             -lrpcrt4.lib \
             -lws2_32.lib \
             -liphlpapi.lib \
             -limm32.lib \
             -lwinmm.lib \
             -lcrypt32.lib \
             -lnetapi32.lib \
             -luserenv.lib \
             -ldwmapi.lib \
             -lwtsapi32.lib \
             -lgdi32.lib \
             -lwindowsapp.lib \
             -ldxva2.lib \
             -ld3d9.lib \
             -levr.lib \
             -lmf.lib \
             -lmfplat.lib \
             -lmfplay.lib \
             -lmfreadwrite.lib \
             -lmfuuid.lib \
             -lwmcodecdspuuid.lib \
             -lstrmiids.lib \
             -ladvapi32.lib \
             -lshell32.lib \
             -lpdh.lib \
             -lopengl32.lib \
             -lglu32.lib
ifeq ($(MODE),debug)
LINKFLAGS := $(LINKFLAGS) -Xlinker /SUBSYSTEM:CONSOLE",10.00"
else
LINKFLAGS := $(LINKFLAGS) -Xlinker /SUBSYSTEM:WINDOWS",10.00"
endif
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
OUTDIST   := $(DISTDIR)/$(NAME).app/Contents/MacOS/$(NAME)$(EXTBIN)
QTMOC     := $(QTMOC) -DQ_OS_MAC
DEFINES   := $(DEFINES) \
             -DNEUROMORE_PLATFORM_OSX \
             -DQT_QPA_DEFAULT_PLATFORM_NAME=\"cocoa\" \
             -DQT_FEATURE_fontconfig=1
CXXFLAGS  := $(CXXFLAGS)
CFLAGS    := $(CFLAGS)
INCLUDES  := $(INCLUDES) -I$(INCDIR)/qt/mkspecs/macx-clang
MOCH      := $(MOCH)
MOCC      := $(MOCC)
MOCO      := $(MOCO)
RCCH      := $(RCCH)
RCCO      := $(RCCO)
UICH      := $(UICH)
OBJS      := $(OBJS)
LINKLIBS  := $(LINKLIBS) \
             $(LIBDIRDEP)/qt-platform-cocoa$(SUFFIX)$(EXTLIB) \
             -lcups \
             -framework AppKit \
             -framework AVFoundation \
             -framework Carbon \
             -framework Cocoa \
             -framework CoreBluetooth \
             -framework CoreMedia \
             -framework GLUT \
             -framework IOBluetooth \
             -framework IOKit \
             -framework IOSurface \
             -framework Metal \
             -framework OpenGL \
             -framework Quartz \
             -framework Security
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
OUTDIST   := $(DISTDIR)/$(NAME)-$(TARGET_ARCH)/usr/bin/$(NAME)$(EXTBIN)
QTMOC     := $(QTMOC) -DQ_OS_LINUX
DEFINES   := $(DEFINES) \
             -DNEUROMORE_PLATFORM_LINUX \
             -DQT_QPA_DEFAULT_PLATFORM_NAME=\"xcb\" \
             -DQT_FEATURE_fontconfig=1
CXXFLAGS  := $(CXXFLAGS)
CFLAGS    := $(CFLAGS)
INCLUDES  := $(INCLUDES) -I$(INCDIR)/qt/mkspecs/linux-clang
MOCH      := $(MOCH)
MOCC      := $(MOCC)
MOCO      := $(MOCO)
RCCH      := $(RCCH)
RCCO      := $(RCCO)
UICH      := $(UICH)
OBJS      := $(OBJS)
LINKLIBS  := $(LINKLIBS) \
             $(LIBDIRDEP)/fontconfig$(SUFFIX)$(EXTLIB) \
             $(LIBDIRDEP)/qt-dbus$(SUFFIX)$(EXTLIB) \
             $(LIBDIRDEP)/qt-platform-xcb$(SUFFIX)$(EXTLIB) \
             -lpthread \
             -ldl \
             -lgstallocators-1.0 \
             -lgstapp-1.0 \
             -lgstaudio-1.0 \
             -lgstbase-1.0 \
             -lgstbasecamerabinsrc-1.0 \
             -lgstcheck-1.0 \
             -lgstcontroller-1.0 \
             -lgstgl-1.0 \
             -lgstpbutils-1.0 \
             -lgstphotography-1.0 \
             -lgstreamer-1.0 \
             -lgstvideo-1.0 \
             -lgobject-2.0 \
             -lglib-2.0 \
             -lGL \
             -lexpat \
             -lX11 \
             -lX11-xcb \
             -lxkbcommon \
             -lxcb \
             -lxcb-glx \
             -lxcb-icccm \
             -lxcb-image \
             -lxcb-keysyms \
             -lxcb-randr \
             -lxcb-render \
             -lxcb-render-util \
             -lxcb-shape \
             -lxcb-shm \
             -lxcb-sync \
             -lxcb-xfixes \
             -lxcb-xinerama \
             -lxcb-xkb
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
QTMOC     := $(QTMOC) -DQ_OS_ANDROID
DEFINES   := $(DEFINES)
CXXFLAGS  := $(CXXFLAGS)
CFLAGS    := $(CFLAGS)
INCLUDES  := $(INCLUDES) -I$(INCDIR)/qt/mkspecs/android-clang
MOCH      := $(MOCH)
MOCC      := $(MOCC)
MOCO      := $(MOCO)
RCCH      := $(RCCH)
RCCO      := $(RCCO)
UICH      := $(UICH)
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

################################################################################################
# MODE

ifeq ($(MODE),debug)
DEFINES := $(DEFINES) -DQT_DEBUG
else
DEFINES := $(DEFINES) -DQT_NO_DEBUG -DPRODUCTION_BUILD
endif

################################################################################################
# PCH

pch:
	@echo [PCH] $(OBJDIR)/$(PCH).pch
	$(CXX) $(CPUFLAGS) $(DEFINES) $(INCLUDES) $(CXXFLAGS) -x c++-header -c $(SRCDIR)/$(PCH).h -o $(OBJDIR)/$(PCH).pch

################################################################################################
# MOC

MOCH := $(patsubst %,$(MOCDIR)/%,$(MOCH))
MOCC := $(patsubst %,$(MOCDIR)/%,$(MOCC))
MOCO := $(patsubst %,$(OBJDIR)/%,$(MOCO))

$(MOCDIR)/%.cpp:
	@echo [MOC] $@
	$(QTMOC) $(DEFINES) $(INCLUDES) $(@:$(MOCDIR)/%.cpp=$(INCDIRQT)/%.h) -b $(NAME)/$(PCH).h -o $(@:$(MOCDIR)/%.cpp=$(MOCDIR)/moc_$(@F))

$(MOCDIR)/%.mm:
	@echo [MOC] $@
	$(QTMOC) $(DEFINES) $(INCLUDES) $(@:$(MOCDIR)/%.mm=$(INCDIRQT)/%.h) -b $(NAME)/$(PCH).h -o $(@:$(MOCDIR)/%.mm=$(MOCDIR)/moc_$(@F))

$(MOCDIR)/%.moc:
	@echo [MOC] $@
	$(QTMOC) $(DEFINES) $(INCLUDES) $(@:$(MOCDIR)/%.moc=$(SRCDIR)/%.cpp) -b $(NAME)/$(PCH).h -o $(@:$(MOCDIR)/%.moc=$(MOCDIR)/$(@F))

$(MOCDIR)/%.mocmm:
	@echo [MOC] $@
	$(QTMOC) $(DEFINES) $(INCLUDES) $(@:$(MOCDIR)/%.mocmm=$(SRCDIR)/%.mm) -b $(NAME)/$(PCH).h -o $(@:$(MOCDIR)/%.mocmm=$(MOCDIR)/$(basename $(@F)).moc)

$(OBJDIR)/%.omoc:
	@echo [CXX] $@
	$(CXX) $(CPUFLAGS) $(DEFINES) $(INCLUDES) $(CXXFLAGS) -include-pch $(OBJDIR)/$(PCH).pch -c $(@:$(OBJDIR)/%.omoc=$(MOCDIR)/moc_$(@F:.omoc=.cpp)) -o $@

$(OBJDIR)/%.omocmm:
	@echo [CXX] $@
	$(CXX) $(CPUFLAGS) $(DEFINES) $(INCLUDES) $(CXXFLAGS) -include-pch $(OBJDIR)/$(PCH).pch -c $(@:$(OBJDIR)/%.omocmm=$(MOCDIR)/moc_%.mm) -o $@

################################################################################################
# RCC

RCCH := $(patsubst %,$(RCCDIR)/%,$(RCCH))
RCCO := $(patsubst %,$(OBJDIR)/%,$(RCCO))

$(RCCDIR)/%.cpp:
	@echo [RCC] $@
	$(QTRCC) --name $(basename $(@F)) $(@:$(RCCDIR)/%.cpp=$(SRCDIR)/%.qrc) --output $(@:$(RCCDIR)/%.cpp=$(RCCDIR)/qrc_$(@F))

$(OBJDIR)/%.orcc:
	@echo [CXX] $@
	$(CXX) $(CPUFLAGS) $(DEFINES) $(INCLUDES) $(CXXFLAGS) -c $(@:$(OBJDIR)/%.orcc=$(RCCDIR)/qrc_$(@F:.orcc=.cpp)) -o $@

################################################################################################
# UIC

UICH := $(patsubst %,$(UICDIR)/%,$(UICH))

$(UICDIR)/%.h:
	@echo [UIC] $@
	$(QTUIC) $(@:$(UICDIR)/%.h=$(SRCDIR)/%.ui) -o $(@:$(UICDIR)/%.h=$(UICDIR)/ui_$(@F))

################################################################################################
# OBJS

OBJS := $(patsubst %,$(OBJDIR)/%,$(OBJS))

$(OBJDIR)/%.o:
	@echo [CXX] $@
	$(CXX) $(CPUFLAGS) $(DEFINES) $(INCLUDES) $(CXXFLAGS) -include-pch $(OBJDIR)/$(PCH).pch -c $(@:$(OBJDIR)%.o=$(SRCDIR)%.cpp) -o $@

$(OBJDIR)/%.oc:
	@echo [CC]  $@
	$(CC) $(CPUFLAGS) $(DEFINES) $(INCLUDES) $(CFLAGS) -include-pch $(OBJDIR)/$(PCH).pch -c $(@:$(OBJDIR)%.oc=$(SRCDIR)%.c) -o $@

$(OBJDIR)/%.omm:
	@echo [CXX] $@
	$(CXX) $(CPUFLAGS) $(DEFINES) $(INCLUDES) $(CXXFLAGS) -include-pch $(OBJDIR)/$(PCH).pch -c $(@:$(OBJDIR)%.omm=$(SRCDIR)%.mm) -o $@

################################################################################################
# RES
	
RESO := $(patsubst %,$(OBJDIR)/%,$(RESO))

$(OBJDIR)/%.res:
	@echo [RC]  $@
	echo $(RC) $(RCFLAGS) $(DEFINES) $(INCLUDES) /fo $@ $(@:$(OBJDIR)/%.res=$(SRCDIR)/%.rc)
	$(RC) $(RCFLAGS) $(DEFINES) $(INCLUDES) /fo $@ $(@:$(OBJDIR)/%.res=$(SRCDIR)/%.rc)

################################################################################################

.DEFAULT_GOAL := build

$(MOCO) : pch
$(OBJS) : pch

PRES := $(MOCH) $(MOCC) $(RCCH) $(UICH)
OBLS := $(OBJS) $(MOCO) $(RCCO)

$(OBLS) : $(PRES)
$(RESO) : $(PRES)

build: pch $(PRES) $(OBLS) $(RESO)
	@echo [AR]  $(LIBDIR)/$(NAME)$(SUFFIX)$(EXTLIB)
	$(AR) $(ARFLAGS) $(LIBDIR)/$(NAME)$(SUFFIX)$(EXTLIB) $(OBLS)
	@echo [LNK] $(TARGET)
	$(LINK) $(LINKFLAGS) $(LINKPATH) $(RESO) $(LIBDIR)/$(NAME)$(SUFFIX)$(EXTLIB) $(LINKLIBS) -o $(TARGET)
	@echo [CPY] Prebuilt Libraries
	$(call copyfiles,$(LIBDIRPRE)/*$(EXTDLL),$(BINDIR)/)
	@echo [CPY] Built Libraries
	$(call copyfiles,$(BINDIRDEP)/*$(EXTDLL),$(BINDIR)/)
ifeq ($(MODE),release)
	@echo [STR] $(TARGET)
	$(STRIP) $(STRIPFLAGS) $(TARGET)
endif

clean:
	$(call deletefiles,$(MOCDIR),*.cpp)
	$(call deletefiles,$(MOCDIR),*.moc)
	$(call deletefiles,$(MOCDIR),*.mocmm)
	$(call deletefiles,$(RCCDIR),*.cpp)
	$(call deletefiles,$(UICDIR),*.h)
	$(call deletefiles,$(OBJDIR),$(PCH).pch)
	$(call deletefiles,$(OBJDIR),*.o)
	$(call deletefiles,$(OBJDIR),*.oc)
	$(call deletefiles,$(OBJDIR),*.omm)
	$(call deletefiles,$(OBJDIR),*.omoc)
	$(call deletefiles,$(OBJDIR),*.orcc)
	$(call deletefiles,$(OBJDIR),*.res)
	$(call deletefiles,$(LIBDIR),$(NAME)$(SUFFIX)$(EXTLIB))
	$(call deletefiles,$(BINDIR),$(NAME)$(SUFFIX)$(EXTLIB))
	$(call deletefiles,$(BINDIR),$(NAME)$(SUFFIX)$(EXTBIN))
	$(call deletefiles,$(BINDIR),$(NAME)$(SUFFIX)$(EXTPDB))
	$(call deletefiles,$(BINDIR),*.$(EXTDLL))
	
################################################################################################

include ../../deps/build/make/platforms/dist.mk

