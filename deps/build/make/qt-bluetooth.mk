
include platforms/detect-host.mk

NAME       = qt-bluetooth
INCDIR     = ../../include
INCDIRQT   = $(INCDIR)/qt/QtBluetooth
SRCDIR     = ../../src/$(NAME)
MOCDIR     = $(SRCDIR)/.moc
RCCDIR     = $(SRCDIR)/.rcc
UICDIR     = $(SRCDIR)/.uic
OBJDIR    := $(OBJDIR)/$(NAME)
DEFINES   := $(DEFINES) \
             -DCHROMIUM_ZLIB_NO_CHROMECONF \
             -DCLASSIC_APP_BUILD \
             -DQT_STATIC \
             -DQT_USE_QSTRINGBUILDER \
             -DHAVE_CONFIG_H \
             -DUNICODE \
             -D_ENABLE_EXTENDED_ALIGNED_STORAGE
INCLUDES  := $(INCLUDES) \
             -I../../src \
             -I$(INCDIR) \
             -I$(INCDIRQT) \
             -I$(INCDIRQT)/private \
             -I$(SRCDIR) \
             -I$(MOCDIR) \
             -I$(UICDIR) \
             -I$(INCDIR)/qt \
             -I$(INCDIR)/qt/QtCore \
             -I$(INCDIR)/qt/QtConcurrent \
             -I$(INCDIR)/qt/QtBluetooth \
             -I$(INCDIR)/qt/QtBluetooth/private \
             -I$(INCDIR)/qt/QtBluetooth/private/bluez \
             -I$(INCDIR)/qt/QtDBus \
             -I$(INCDIR)/qt/QtAndroidExtras
CXXFLAGS  := $(CXXFLAGS) \
             -Wno-deprecated-declarations \
             -std=c++17
LINKFLAGS := $(LINKFLAGS)
LINKPATH  := $(LINKPATH)
LINKLIBS  := $(LINKLIBS)
MOCH       = qbluetoothdevicediscoveryagent.cpp \
             qbluetoothlocaldevice.cpp \
             qbluetoothserver.cpp \
             qbluetoothsocket.cpp \
             qbluetoothtransfermanager.cpp \
             qbluetoothtransferreply.cpp \
             qbluetoothservicediscoveryagent.cpp \
             qlowenergycontroller.cpp \
             qlowenergyservice.cpp \
             private/qbluetoothdevicediscoveryagent_p.cpp \
             private/qbluetoothlocaldevice_p.cpp \
             private/qbluetoothservicediscoveryagent_p.cpp \
             private/qbluetoothserviceinfo_p.cpp \
             private/qbluetoothsocketbase_p.cpp \
             private/qleadvertiser_p.cpp \
             private/qlowenergycontrollerbase_p.cpp \
             private/qlowenergyserviceprivate_p.cpp
MOCC       =
MOCO       = qbluetoothserviceinfo_p.omoc \
             qbluetoothservicediscoveryagent_p.omoc \
             qbluetoothsocketbase_p.omoc \
             qleadvertiser_p.omoc \
             qlowenergycontroller.omoc \
             qlowenergycontrollerbase_p.omoc \
             qlowenergyservice.omoc \
             qlowenergyserviceprivate_p.omoc
RCCH       =
RCCO       =
UICH       =
OBJS       = qbluetooth.o \
             qbluetoothaddress.o \
             qbluetoothdeviceinfo.o \
             qbluetoothhostinfo.o \
             qbluetoothlocaldevice.o \
             qbluetoothtransfermanager.o \
             qbluetoothtransferreply.o \
             qbluetoothtransferrequest.o \
             qbluetoothuuid.o \
             qlowenergyadvertisingdata.o \
             qlowenergyadvertisingparameters.o \
             qlowenergycharacteristic.o \
             qlowenergycharacteristicdata.o \
             qlowenergyconnectionparameters.o \
             qlowenergydescriptor.o \
             qlowenergydescriptordata.o \
             qlowenergyservicedata.o \
             qlowenergyserviceprivate.o

################################################################################################

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

################################################################################################
# WINDOWS
ifeq ($(TARGET_OS),win)
QTMOC     := $(QTMOC) -DQ_OS_WIN
DEFINES   := $(DEFINES) \
             -D_CRT_SECURE_NO_WARNINGS \
             -DQT_WINRT_BLUETOOTH \
             -DQT_FEATURE_bluez=-1 \
             -DQT_FEATURE_dbus=-1
CXXFLAGS  := $(CXXFLAGS) -Wno-deprecated-declarations
INCLUDES  := $(INCLUDES) -I$(INCDIR)/qt/mkspecs/win32-clang-msvc
MOCH      := $(MOCH) \
             private/qbluetoothdevicediscoveryagent_p.cpp \
             private/qbluetoothservicediscoveryagent_p.cpp \
             private/qbluetoothserviceinfo_p.cpp \
             private/qbluetoothsocket_winrt_p.cpp \
             private/qbluetoothsocketbase_p.cpp \
             private/qlowenergycontroller_p.cpp \
             private/qlowenergycontroller_winrt_p.cpp \
             private/qlowenergyserviceprivate_p.cpp
MOCC      := $(MOCC) \
             qbluetoothdevicediscoveryagent_winrt.moc \
             qbluetoothservicediscoveryagent_winrt.moc \
             qbluetoothsocket_winrt.moc \
             qlowenergycontroller_winrt.moc
MOCO      := $(MOCO) \
             qbluetoothdevicediscoveryagent_p.omoc \
             qbluetoothservicediscoveryagent_p.omoc \
             qbluetoothsocket_winrt_p.omoc \
             qbluetoothsocketbase_p.omoc \
             qbluetoothserviceinfo_p.omoc \
             qlowenergycontroller_p.omoc \
             qlowenergycontroller_winrt_p.omoc \
             qlowenergyserviceprivate_p.omoc
RCCH      := $(RCCH)
RCCO      := $(RCCO)
UICH      := $(UICH)
OBJS      := $(OBJS) \
             qbluetoothdevicediscoveryagent.o \
             qbluetoothdevicediscoveryagent_winrt.o \
             qbluetoothlocaldevice_p.o \
             qbluetoothserver.o \
             qbluetoothserver_winrt.o \
             qbluetoothservicediscoveryagent.o \
             qbluetoothservicediscoveryagent_winrt.o \
             qbluetoothserviceinfo.o \
             qbluetoothserviceinfo_winrt.o \
             qbluetoothsocket.o \
             qbluetoothsocket_winrt.o \
             qbluetoothsocketbase.o \
             qbluetoothutils_win.o \
             qlowenergycontroller.o \
             qlowenergycontroller_winrt.o \
             qlowenergycontrollerbase.o \
             qlowenergyservice.o
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
QTMOC     := $(QTMOC) -DQ_OS_MAC
DEFINES   := $(DEFINES) \
             -DQT_OSX_BLUETOOTH \
             -DQT_FEATURE_bluez=-1 \
             -DQT_FEATURE_dbus=-1
CXXFLAGS  := $(CXXFLAGS)
INCLUDES  := $(INCLUDES) -I$(INCDIR)/qt/mkspecs/macx-clang
MOCH      := $(MOCH) \
             private/osxbtnotifier_p.cpp \
             private/qbluetoothtransferreply_osx_p.mm \
             private/qlowenergycontroller_osx_p.mm \
             private/osxbtcentralmanager_p.mm
MOCC      := $(MOCC) \
             private/qlowenergycontroller_osx_p.cpp
MOCO      := $(MOCO) \
             qbluetoothdevicediscoveryagent.omoc \
             osxbtnotifier_p.omoc \
             qbluetoothtransferreply_osx_p.omocmm \
             qlowenergycontroller_osx_p.omocmm
RCCH      := $(RCCH)
RCCO      := $(RCCO)
UICH      := $(UICH)
OBJS      := $(OBJS) \
             osx/osxbtcentralmanager.omm \
             osx/osxbtchanneldelegate.omm \
             osx/osxbtconnectionmonitor.omm \
             osx/osxbtdeviceinquiry.omm \
             osx/osxbtdevicepair.omm \
             osx/osxbtgcdtimer.omm \
             osx/osxbtl2capchannel.omm \
             osx/osxbtledeviceinquiry.omm \
             osx/osxbtnotifier.o \
             osx/osxbtobexsession.omm \
             osx/osxbtperipheralmanager.omm \
             osx/osxbtrfcommchannel.omm \
             osx/osxbtsdpinquiry.omm \
             osx/osxbtservicerecord.omm \
             osx/osxbtsocketlistener.omm \
             osx/osxbtutility.omm \
             osx/uistrings.o \
             qbluetoothdevicediscoveryagent_osx.omm \
             qbluetoothtransferreply_osx.omm \
             qbluetoothlocaldevice_osx.omm \
             qbluetoothserver_osx.omm \
             qbluetoothservicediscoveryagent_osx.omm \
             qbluetoothserviceinfo_osx.omm \
             qbluetoothsocket_osx.omm \
             qlowenergycontroller_osx.omm \
             qlowenergyservice_osx.omm
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
QTMOC     := $(QTMOC) -DQ_OS_LINUX
DEFINES   := $(DEFINES) \
             -DQT_FEATURE_bluez=1 \
             -DQT_FEATURE_dbus=1
CXXFLAGS  := $(CXXFLAGS)
INCLUDES  := $(INCLUDES) -I$(INCDIR)/qt/mkspecs/linux-clang
MOCH      := $(MOCH) \
             private/qbluetoothsocket_bluez_p.cpp \
             private/qbluetoothsocket_bluezdbus_p.cpp \
             private/qbluetoothtransferreply_bluez_p.cpp \
             private/qlowenergycontroller_p.cpp \
             private/qlowenergycontroller_bluez_p.cpp \
             private/qlowenergycontroller_bluezdbus_p.cpp \
             private/qlowenergyserviceprivate_p.cpp \
             private/bluez/adapter_p.cpp \
             private/bluez/adapter1_bluez5_p.cpp \
             private/bluez/agent_p.cpp \
             private/bluez/battery1_p.cpp \
             private/bluez/bluetoothmanagement_p.cpp \
             private/bluez/bluez5_helper_p.cpp \
             private/bluez/device_p.cpp \
             private/bluez/device1_bluez5_p.cpp \
             private/bluez/gattchar1_p.cpp \
             private/bluez/gattdesc1_p.cpp \
             private/bluez/gattservice1_p.cpp \
             private/bluez/hcimanager_p.cpp \
             private/bluez/manager_p.cpp \
             private/bluez/obex_agent_p.cpp \
             private/bluez/obex_client_p.cpp \
             private/bluez/obex_client1_bluez5_p.cpp \
             private/bluez/obex_manager_p.cpp \
             private/bluez/obex_objectpush1_bluez5_p.cpp \
             private/bluez/obex_transfer_p.cpp \
             private/bluez/obex_transfer1_bluez5_p.cpp \
             private/bluez/objectmanager_p.cpp \
             private/bluez/profile1_p.cpp \
             private/bluez/profile1context_p.cpp \
             private/bluez/profilemanager1_p.cpp \
             private/bluez/properties_p.cpp \
             private/bluez/remotedevicemanager_p.cpp \
             private/bluez/service_p.cpp \
             private/bluez/servicemap_p.cpp
MOCC      := $(MOCC)
MOCO      := $(MOCO) \
             adapter_p.omoc \
             adapter1_bluez5_p.omoc \
             agent_p.omoc \
             battery1_p.omoc \
             bluetoothmanagement_p.omoc \
             bluez5_helper_p.omoc \
             device_p.omoc \
             device1_bluez5_p.omoc \
             gattchar1_p.omoc \
             gattdesc1_p.omoc \
             gattservice1_p.omoc \
             hcimanager_p.omoc \
             manager_p.omoc \
             obex_agent_p.omoc \
             obex_client_p.omoc \
             obex_client1_bluez5_p.omoc \
             obex_manager_p.omoc \
             obex_objectpush1_bluez5_p.omoc \
             obex_transfer_p.omoc \
             obex_transfer1_bluez5_p.omoc \
             objectmanager_p.omoc \
             profile1_p.omoc \
             profile1context_p.omoc \
             profilemanager1_p.omoc \
             properties_p.omoc \
             qbluetoothsocket_bluez_p.omoc \
             qbluetoothsocket_bluezdbus_p.omoc \
             qbluetoothtransferreply_bluez_p.omoc \
             qlowenergycontroller_p.omoc \
             qlowenergycontroller_bluez_p.omoc \
             qlowenergycontroller_bluezdbus_p.omoc \
             qlowenergyserviceprivate_p.omoc \
             remotedevicemanager_p.omoc \
             service_p.omoc \
             servicemap_p.omoc
RCCH      := $(RCCH)
RCCO      := $(RCCO)
UICH      := $(UICH)
OBJS      := $(OBJS) \
             bluez/adapter.o \
             bluez/adapter1_bluez5.o \
             bluez/agent.o \
             bluez/battery1.o \
             bluez/bluetoothmanagement.o \
             bluez/bluez5_helper.o \
             bluez/device.o \
             bluez/device1_bluez5.o \
             bluez/gattchar1.o \
             bluez/gattdesc1.o \
             bluez/gattservice1.o \
             bluez/hcimanager.o \
             bluez/manager.o \
             bluez/obex_agent.o \
             bluez/obex_client.o \
             bluez/obex_client1_bluez5.o \
             bluez/obex_manager.o \
             bluez/obex_objectpush1_bluez5.o \
             bluez/obex_transfer.o \
             bluez/obex_transfer1_bluez5.o \
             bluez/objectmanager.o \
             bluez/profile1.o \
             bluez/profile1context.o \
             bluez/profilemanager1.o \
             bluez/properties.o \
             bluez/remotedevicemanager.o \
             bluez/service.o \
             bluez/servicemap.o \
             lecmaccalculator.o \
             qbluetoothdevicediscoveryagent.o \
             qbluetoothdevicediscoveryagent_bluez.o \
             qbluetoothlocaldevice_bluez.o \
             qbluetoothserver.o \
             qbluetoothserver_bluez.o \
             qbluetoothservicediscoveryagent.o \
             qbluetoothservicediscoveryagent_bluez.o \
             qbluetoothserviceinfo.o \
             qbluetoothserviceinfo_bluez.o \
             qbluetoothsocket.o \
             qbluetoothsocket_bluez.o \
             qbluetoothsocket_bluezdbus.o \
             qbluetoothsocketbase.o \
             qbluetoothtransferreply_bluez.o \
             qleadvertiser_bluez.o \
             qlowenergycontroller.o \
             qlowenergycontroller_bluez.o \
             qlowenergycontroller_bluezdbus.o \
             qlowenergycontrollerbase.o \
             qlowenergyservice.o
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
DEFINES   := $(DEFINES) \
             -DQT_ANDROID_BLUETOOTH \
             -DQT_FEATURE_bluez=-1 \
             -DQT_FEATURE_dbus=-1
CXXFLAGS  := $(CXXFLAGS)
INCLUDES  := $(INCLUDES) -I$(INCDIR)/qt/mkspecs/android-clang
MOCH      := $(MOCH) \
             private/qbluetoothsocket_android_p.cpp \
             private/qlowenergycontroller_p.cpp \
             private/qlowenergycontroller_android_p.cpp \
             private/qlowenergyserviceprivate_p.cpp \
             private/android/androidbroadcastreceiver_p.cpp \
             private/android/devicediscoverybroadcastreceiver_p.cpp \
             private/android/inputstreamthread_p.cpp \
             private/android/jni_android_p.cpp \
             private/android/localdevicebroadcastreceiver_p.cpp \
             private/android/lowenergynotificationhub_p.cpp \
             private/android/serveracceptancethread_p.cpp \
             private/android/servicediscoverybroadcastreceiver_p.cpp
MOCC      := $(MOCC) \
             qbluetoothsocket_android.moc
MOCO      := $(MOCO) \
             androidbroadcastreceiver_p.omoc \
             devicediscoverybroadcastreceiver_p.omoc \
             inputstreamthread_p.omoc \
             jni_android_p.omoc \
             localdevicebroadcastreceiver_p.omoc \
             lowenergynotificationhub_p.omoc \
             serveracceptancethread_p.omoc \
             servicediscoverybroadcastreceiver_p.omoc \
             qbluetoothsocket_android_p.omoc \
             qlowenergycontroller_p.omoc \
             qlowenergycontroller_android_p.omoc \
             qlowenergyserviceprivate_p.omoc
RCCH      := $(RCCH)
RCCO      := $(RCCO)
UICH      := $(UICH)
OBJS      := $(OBJS) \
             qbluetoothdevicediscoveryagent.o \
             qbluetoothdevicediscoveryagent_android.o \
             qbluetoothlocaldevice_android.o \
             qbluetoothserver.o \
             qbluetoothserver_android.o \
             qbluetoothservicediscoveryagent.o \
             qbluetoothservicediscoveryagent_android.o \
             qbluetoothserviceinfo.o \
             qbluetoothserviceinfo_android.o \
             qbluetoothsocket.o \
             qbluetoothsocket_android.o \
             qbluetoothsocketbase.o \
             qlowenergycontroller.o \
             qlowenergycontroller_android.o \
             qlowenergycontrollerbase.o \
             qlowenergyservice.o
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
# iOS
ifeq ($(TARGET_OS),ios)
QTMOC     := $(QTMOC) -DQ_OS_IOS
DEFINES   := $(DEFINES)
CXXFLAGS  := $(CXXFLAGS)
INCLUDES  := $(INCLUDES) -I$(INCDIR)/qt/mkspecs/macx-ios-clang
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

ifeq ($(MODE),debug)
DEFINES   := $(DEFINES) -DQT_DEBUG
else
DEFINES   := $(DEFINES) -DQT_NO_DEBUG
endif

################################################################################################
# MOC

MOCH := $(patsubst %,$(MOCDIR)/%,$(MOCH))
MOCC := $(patsubst %,$(MOCDIR)/%,$(MOCC))
MOCO := $(patsubst %,$(OBJDIR)/%,$(MOCO))

$(MOCDIR)/%.cpp:
	@echo [MOC] $@
	$(QTMOC) $(DEFINES) $(INCLUDES) $(@:$(MOCDIR)/%.cpp=$(INCDIRQT)/%.h) -o $(@:$(MOCDIR)/%.cpp=$(MOCDIR)/moc_$(@F))

$(MOCDIR)/%.mm:
	@echo [MOC] $@
	$(QTMOC) $(DEFINES) $(INCLUDES) $(@:$(MOCDIR)/%.mm=$(INCDIRQT)/%.h) -o $(@:$(MOCDIR)/%.mm=$(MOCDIR)/moc_$(@F))

$(MOCDIR)/%.moc:
	@echo [MOC] $@
	$(QTMOC) $(DEFINES) $(INCLUDES) $(@:$(MOCDIR)/%.moc=$(SRCDIR)/%.cpp) -o $(@:$(MOCDIR)/%.moc=$(MOCDIR)/$(@F))

$(MOCDIR)/%.mocmm:
	@echo [MOC] $@
	$(QTMOC) $(DEFINES) $(INCLUDES) $(@:$(MOCDIR)/%.mocmm=$(SRCDIR)/%.mm) -o $(@:$(MOCDIR)/%.mocmm=$(MOCDIR)/$(basename $(@F)).moc)

$(OBJDIR)/%.omoc:
	@echo [CXX] $@
	$(CXX) $(CPUFLAGS) $(DEFINES) $(INCLUDES) $(CXXFLAGS) -c $(@:$(OBJDIR)/%.omoc=$(MOCDIR)/moc_%.cpp) -o $@

$(OBJDIR)/%.omocmm:
	@echo [CXX] $@
	$(CXX) $(CPUFLAGS) $(DEFINES) $(INCLUDES) $(CXXFLAGS) -c $(@:$(OBJDIR)/%.omocmm=$(MOCDIR)/moc_%.mm) -o $@

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
	$(CXX) $(CPUFLAGS) $(DEFINES) $(INCLUDES) $(CXXFLAGS) -c $(@:$(OBJDIR)%.o=$(SRCDIR)%.cpp) -o $@

$(OBJDIR)/%.omm:
	@echo [CXX] $@
	$(CXX) $(CPUFLAGS) $(DEFINES) $(INCLUDES) $(CXXFLAGS) -c $(@:$(OBJDIR)%.omm=$(SRCDIR)%.mm) -o $@

################################################################################################

.DEFAULT_GOAL := build

PRES := $(MOCH) $(MOCC) $(RCCH) $(UICH)
OBLS := $(OBJS) $(MOCO) $(RCCO)

$(OBLS) : $(PRES)

build: $(PRES) $(OBLS)
	@echo [AR]  $(LIBDIR)/$(NAME)$(SUFFIX)$(EXTLIB)
	$(AR) $(ARFLAGS) $(LIBDIR)/$(NAME)$(SUFFIX)$(EXTLIB) $(MOCO) $(RCCO) $(OBJS)

clean:
	$(call deletefiles,$(MOCDIR),*.cpp)
	$(call deletefiles,$(MOCDIR),*.moc)
	$(call deletefiles,$(MOCDIR),*.mocmm)
	$(call deletefiles,$(RCCDIR),*.cpp)
	$(call deletefiles,$(UICDIR),*.h)
	$(call deletefiles,$(OBJDIR),*.o)
	$(call deletefiles,$(OBJDIR),*.omm)
	$(call deletefiles,$(OBJDIR),*.omoc)
	$(call deletefiles,$(OBJDIR),*.orcc)
	$(call deletefiles,$(LIBDIR),$(NAME)$(SUFFIX)$(EXTLIB))
