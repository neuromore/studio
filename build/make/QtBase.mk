
include ../../deps/build/make/platforms/detect-host.mk
include ../../deps/build/make/platforms/$(DETECTED_OS)-$(DETECTED_ARCH)-$(TARGET_OS)-$(TARGET_ARCH).mk

NAME       = QtBase
INCDIR     = ../../deps/include
SRCDIR     = ../../src/$(NAME)
INCDIRQT   = $(SRCDIR)
MOCDIR     = $(SRCDIR)/.moc
RCCDIR     = $(SRCDIR)/.rcc
UICDIR     = $(SRCDIR)/.uic
OBJDIR    := $(OBJDIR)/$(NAME)
QTMOC     := "../../deps/build/make/$(QTMOC)"
QTRCC     := "../../deps/build/make/$(QTRCC)"
QTUIC     := "../../deps/build/make/$(QTUIC)"
DEFINES   := $(DEFINES) \
             -D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS \
             -DCHROMIUM_ZLIB_NO_CHROMECONF \
             -DQT_STATIC \
             -DHAVE_CONFIG_H \
             -DUNICODE \
             -DPCRE2_STATIC \
             -D_ENABLE_EXTENDED_ALIGNED_STORAGE
INCLUDES  := $(INCLUDES) \
             -I../../src \
             -I../../src/Engine \
             -I$(INCDIR) \
             -I$(SRCDIR) \
             -I$(UICDIR) \
             -I$(MOCDIR) \
             -I$(INCDIR)/qt \
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
LINKPATH  := $(LINKPATH)
LINKLIBS  := $(LINKLIBS)
PCH        = Precompiled
MOCH       = AttributeWidgets/AttributeSetGridWidget.cpp \
             AttributeWidgets/AttributeWidgets.cpp \
             AttributeWidgets/PropertyManager.cpp \
             AttributeWidgets/PropertyTreeWidget.cpp \
             Audio/MediaContent.cpp \
             Backend/BackendFileSystem.cpp \
             Backend/BackendInterface.cpp \
             Backend/BackendParameters.cpp \
             Backend/BackendUploader.cpp \
             Backend/FileDownloader.cpp \
             Backend/NetworkAccessManager.cpp \
             Backend/WebDataCache.cpp \
             Networking/NetworkClient.cpp \
             Networking/NetworkServer.cpp \
             Networking/NetworkServerClient.cpp \
             Networking/OscServer.cpp \
             PluginSystem/Plugin.cpp \
             PluginSystem/PluginManager.cpp \
             PluginSystem/PluginMenu.cpp \
             System/SerialPort.cpp \
             Widgets/ColorMappingWidget.cpp \
             Widgets/GraphObjectViewWidget.cpp \
             Widgets/PlotWidget.cpp \
             Widgets/SearchBoxWidget.cpp \
             Widgets/WindowFunctionWidget.cpp \
             Windows/EnterLabelWindow.cpp \
             Windows/ProgressWindow.cpp \
             Windows/ProgressWindowManager.cpp \
             Windows/StatusPopupManager.cpp \
             Windows/StatusPopupWindow.cpp \
             ColorLabel.cpp \
             DockHeader.cpp \
             DockWidget.cpp \
             ExperienceAssetCache.cpp \
             FileManager.cpp \
             Gamepad.cpp \
             GamepadManager.cpp \
             ImageButton.cpp \
             Layout.cpp \
             LayoutComboBox.cpp \
             LayoutManager.cpp \
             LayoutMenu.cpp \
             LinkWidget.cpp \
             MainWindowBase.cpp \
             PainterStaticTextCache.cpp \
             QtBaseManager.cpp \
             Slider.cpp \
             Spinbox.cpp
MOCC       =
MOCO       = $(patsubst %.cpp,%.omoc,$(MOCH))
RCCH       = Resources/Assets.cpp \
             Resources/DeviceResources.cpp \
             Resources/GraphResources.cpp \
             Resources/LayoutResources.cpp \
             Resources/QtBaseResources.cpp
RCCO       = Assets.orcc \
             DeviceResources.orcc \
             GraphResources.orcc \
             LayoutResources.orcc \
             QtBaseResources.orcc
UICH       =
OBJS       = AttributeWidgets/AttributeSetGridWidget.o \
             AttributeWidgets/AttributeWidgetCreators.o \
             AttributeWidgets/AttributeWidgetFactory.o \
             AttributeWidgets/AttributeWidgets.o \
             AttributeWidgets/Property.o \
             AttributeWidgets/PropertyManager.o \
             AttributeWidgets/PropertyTreeWidget.o \
             Audio/MediaContent.o \
             AutoUpdate/AutoUpdate.o \
             Backend/BackendFileSystem.o \
             Backend/BackendHelpers.o \
             Backend/BackendInterface.o \
             Backend/BackendParameters.o \
             Backend/BackendUploader.o \
             Backend/DataChunksCreateRequest.o \
             Backend/DataChunksCreateResponse.o \
             Backend/DataChunksUploadRequest.o \
             Backend/DataChunksUploadResponse.o \
             Backend/FileDownloader.o \
             Backend/FileHierarchyGetRequest.o \
             Backend/FileHierarchyGetResponse.o \
             Backend/FileHierarchyItem.o \
             Backend/FilesCreateRequest.o \
             Backend/FilesCreateResponse.o \
             Backend/FilesDeleteRequest.o \
             Backend/FilesDeleteResponse.o \
             Backend/FilesGetRequest.o \
             Backend/FilesGetResponse.o \
             Backend/FilesUpdateRequest.o \
             Backend/FilesUpdateResponse.o \
             Backend/FileSystemGetRequest.o \
             Backend/FileSystemGetResponse.o \
             Backend/FileSystemItem.o \
             Backend/FoldersCreateRequest.o \
             Backend/FoldersCreateResponse.o \
             Backend/FoldersDeleteRequest.o \
             Backend/FoldersDeleteResponse.o \
             Backend/FoldersUpdateRequest.o \
             Backend/FoldersUpdateResponse.o \
             Backend/LoginRequest.o \
             Backend/LoginResponse.o \
             Backend/LogsCreateRequest.o \
             Backend/LogsCreateResponse.o \
             Backend/NetworkAccessManager.o \
             Backend/ParametersFindRequest.o \
             Backend/ParametersFindResponse.o \
             Backend/ParametersSetRequest.o \
             Backend/ParametersSetResponse.o \
             Backend/Request.o \
             Backend/RequestQueue.o \
             Backend/Response.o \
             Backend/SystemsGetRequest.o \
             Backend/SystemsGetResponse.o \
             Backend/UsersAgreementRequest.o \
             Backend/UsersAgreementResponse.o \
             Backend/UsersCreateRequest.o \
             Backend/UsersCreateResponse.o \
             Backend/UsersGetRequest.o \
             Backend/UsersGetResponse.o \
             Backend/UsersInviteRequest.o \
             Backend/UsersInviteResponse.o \
             Backend/WebDataCache.o \
             Networking/NetworkClient.o \
             Networking/NetworkMessage.o \
             Networking/NetworkMessageConfig.o \
             Networking/NetworkMessageData.o \
             Networking/NetworkMessageEvent.o \
             Networking/NetworkServer.o \
             Networking/NetworkServerClient.o \
             Networking/OscServer.o \
             PluginSystem/Plugin.o \
             PluginSystem/PluginManager.o \
             PluginSystem/PluginMenu.o \
             System/BluetoothHelpers.o \
             System/SerialPort.o \
             Widgets/ColorMappingWidget.o \
             Widgets/FilterPlotWidget.o \
             Widgets/GraphObjectViewWidget.o \
             Widgets/PlotWidget.o \
             Widgets/SearchBoxWidget.o \
             Widgets/WindowFunctionWidget.o \
             Windows/EnterLabelWindow.o \
             Windows/ProgressWindow.o \
             Windows/ProgressWindowManager.o \
             Windows/StatusPopupManager.o \
             Windows/StatusPopupWindow.o \
             ColorLabel.o \
             DockHeader.o \
             DockWidget.o \
             ExperienceAssetCache.o \
             FileManager.o \
             Gamepad.o \
             GamepadManager.o \
             ImageButton.o \
             ImageManipulation.o \
             Layout.o \
             LayoutComboBox.o \
             LayoutManager.o \
             LayoutMenu.o \
             LinkWidget.o \
             MainWindowBase.o \
             PainterStaticTextCache.o \
             QtBaseManager.o \
             Slider.o \
             Spinbox.o \
             SystemInfo.o

################################################################################################

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
QTMOC     := $(QTMOC) -DQ_OS_WIN
DEFINES   := $(DEFINES) \
             -D_CRT_SECURE_NO_WARNINGS \
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
# OSX
ifeq ($(TARGET_OS),osx)
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
DEFINES   := $(DEFINES) -DNEUROMORE_PLATFORM_ANDROID
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

ifeq ($(MODE),release)
DEFINES   := $(DEFINES) -DQT_NO_DEBUG
else
DEFINES   := $(DEFINES)
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
	$(CXX) $(CPUFLAGS) $(DEFINES) $(INCLUDES) $(CXXFLAGS) -Xclang -include-pch -Xclang $(OBJDIR)/$(PCH).pch -c $(@:$(OBJDIR)/%.omoc=$(MOCDIR)/moc_$(@F:.omoc=.cpp)) -o $@

$(OBJDIR)/%.omocmm:
	@echo [CXX] $@
	$(CXX) $(CPUFLAGS) $(DEFINES) $(INCLUDES) $(CXXFLAGS) -Xclang -include-pch -Xclang $(OBJDIR)/$(PCH).pch -c $(@:$(OBJDIR)/%.omocmm=$(MOCDIR)/moc_%.mm) -o $@

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
	$(CXX) $(CPUFLAGS) $(DEFINES) $(INCLUDES) $(CXXFLAGS) -Xclang -include-pch -Xclang $(OBJDIR)/$(PCH).pch -c $(@:$(OBJDIR)%.o=$(SRCDIR)%.cpp) -o $@

$(OBJDIR)/%.oc:
	@echo [CC]  $@
	$(CC) $(CPUFLAGS) $(DEFINES) $(INCLUDES) $(CFLAGS) -Xclang -include-pch -Xclang $(OBJDIR)/$(PCH).pch -c $(@:$(OBJDIR)%.oc=$(SRCDIR)%.c) -o $@

$(OBJDIR)/%.omm:
	@echo [CXX] $@
	$(CXX) $(CPUFLAGS) $(DEFINES) $(INCLUDES) $(CXXFLAGS) -Xclang -include-pch -Xclang $(OBJDIR)/$(PCH).pch -c $(@:$(OBJDIR)%.omm=$(SRCDIR)%.mm) -o $@

################################################################################################

.DEFAULT_GOAL := build

$(MOCO) : pch
$(OBJS) : pch

PRES := $(MOCH) $(MOCC) $(RCCH) $(UICH)
OBLS := $(OBJS) $(MOCO) $(RCCO)

$(OBLS) : $(PRES)

build: pch $(PRES) $(OBLS)
	@echo [AR]  $(LIBDIR)/$(NAME)$(SUFFIX)$(EXTLIB)
	$(AR) $(ARFLAGS) $(LIBDIR)/$(NAME)$(SUFFIX)$(EXTLIB) $(MOCO) $(RCCO) $(OBJS)

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
	$(call deletefiles,$(LIBDIR),$(NAME)$(SUFFIX)$(EXTLIB))
