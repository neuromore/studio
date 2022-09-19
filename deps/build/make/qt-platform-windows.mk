
include platforms/detect-host.mk

NAME       = qt-platform-windows
INCDIR     = ../../include
SRCDIR     = ../../src/$(NAME)
INCDIRQT   = $(SRCDIR)
MOCDIR     = $(SRCDIR)/.moc
RCCDIR     = $(SRCDIR)/.rcc
UICDIR     = $(SRCDIR)/.uic
OBJDIR    := $(OBJDIR)/$(NAME)
DEFINES   := $(DEFINES) \
             -DCHROMIUM_ZLIB_NO_CHROMECONF \
             -DQT_STATIC \
             -DQT_STATICPLUGIN \
             -DQT_USE_QSTRINGBUILDER \
             -DHAVE_CONFIG_H \
             -DUNICODE \
             -D_ENABLE_EXTENDED_ALIGNED_STORAGE
INCLUDES  := $(INCLUDES) \
             -I../../src \
             -I$(INCDIR) \
             -I$(INCDIR)/wintab \
             -I$(INCDIRQT) \
             -I$(INCDIRQT)/private \
             -I$(SRCDIR) \
             -I$(UICDIR) \
             -I$(MOCDIR) \
             -I$(INCDIR)/qt \
             -I$(INCDIR)/qt/QtCore \
             -I$(INCDIR)/qt/QtGui \
             -I$(INCDIR)/qt/QtWindowsUIAutomationSupport \
             -I$(INCDIR)/qt/QtWindowsUIAutomationSupport/private \
             -I$(INCDIR)/qt/QtThemeSupport \
             -I$(INCDIR)/qt/QtThemeSupport/private
CXXFLAGS  := $(CXXFLAGS) \
             -Wno-microsoft-exception-spec \
             -Wno-microsoft-cast \
             -std=c++17
CFLAGS    := $(CFLAGS)
LINKFLAGS := $(LINKFLAGS)
LINKPATH  := $(LINKPATH)
LINKLIBS  := $(LINKLIBS)
MOCH       = 
MOCC       =
MOCO       =
RCCH       =
RCCO       =
UICH       =
OBJS       = 

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
             -DQT_QPA_DEFAULT_PLATFORM_NAME=\"windows\" \
             -DQT_NO_FONTCONFIG
CXXFLAGS  := $(CXXFLAGS)
CFLAGS    := $(CFLAGS)
INCLUDES  := $(INCLUDES) -I$(INCDIR)/qt/mkspecs/win32-clang-msvc
MOCH      := $(MOCH) \
             qwindowsgdinativeinterface.cpp \
             qwindowsinputcontext.cpp \
             qwindowsmenu.cpp \
             qwindowsnativeinterface.cpp \
             uiautomation/qwindowsuiabaseprovider.cpp \
             uiautomation/qwindowsuiamainprovider.cpp \
             uiautomation/qwindowsuiaprovidercache.cpp
MOCC      := $(MOCC) \
             main.moc \
             qwindowsdialoghelpers.moc
MOCO      := $(MOCO) \
             qwindowsgdinativeinterface.omoc \
             qwindowsinputcontext.omoc \
             qwindowsmenu.omoc \
             qwindowsnativeinterface.omoc \
             qwindowsuiabaseprovider.omoc \
             qwindowsuiamainprovider.omoc \
             qwindowsuiaprovidercache.omoc
RCCH      := $(RCCH) \
             cursors.cpp \
             openglblacklists.cpp
RCCO      := $(RCCO) \
             cursors.orcc \
             openglblacklists.orcc
UICH      := $(UICH)
OBJS      := $(OBJS) \
             uiautomation/qwindowsuiaaccessibility.o \
             uiautomation/qwindowsuiabaseprovider.o \
             uiautomation/qwindowsuiagriditemprovider.o \
             uiautomation/qwindowsuiagridprovider.o \
             uiautomation/qwindowsuiainvokeprovider.o \
             uiautomation/qwindowsuiamainprovider.o \
             uiautomation/qwindowsuiaprovidercache.o \
             uiautomation/qwindowsuiarangevalueprovider.o \
             uiautomation/qwindowsuiaselectionitemprovider.o \
             uiautomation/qwindowsuiaselectionprovider.o \
             uiautomation/qwindowsuiatableitemprovider.o \
             uiautomation/qwindowsuiatableprovider.o \
             uiautomation/qwindowsuiatextprovider.o \
             uiautomation/qwindowsuiatextrangeprovider.o \
             uiautomation/qwindowsuiatoggleprovider.o \
             uiautomation/qwindowsuiautils.o \
             uiautomation/qwindowsuiavalueprovider.o \
             uiautomation/qwindowsuiawindowprovider.o \
             uiautomation/qwindowsuiawrapper.o \
             main.o \
             qabstractfileiconengine.o \
             qwin10helpers.o \
             qwindowsbackingstore.o \
             qwindowsclipboard.o \
             qwindowscontext.o \
             qwindowscursor.o \
             qwindowsdialoghelpers.o \
             qwindowsdrag.o \
             qwindowsdropdataobject.o \
             qwindowseglcontext.o \
             qwindowsgdiintegration.o \
             qwindowsgdinativeinterface.o \
             qwindowsglcontext.o \
             qwindowsinputcontext.o \
             qwindowsintegration.o \
             qwindowsinternalmimedata.o \
             qwindowskeymapper.o \
             qwindowsmenu.o \
             qwindowsmime.o \
             qwindowsmousehandler.o \
             qwindowsnativeinterface.o \
             qwindowsole.o \
             qwindowsopengltester.o \
             qwindowspointerhandler.o \
             qwindowsscreen.o \
             qwindowsservices.o \
             qwindowssessionmanager.o \
             qwindowssystemtrayicon.o \
             qwindowstabletsupport.o \
             qwindowstheme.o \
             qwindowswindow.o
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
DEFINES   := $(DEFINES)  
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
DEFINES   := $(DEFINES)
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

$(OBJDIR)/%.oc:
	@echo [CC]  $@
	$(CC) $(CPUFLAGS) $(DEFINES) $(INCLUDES) $(CFLAGS) -c $(@:$(OBJDIR)%.oc=$(SRCDIR)%.c) -o $@

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
	$(call deletefiles,$(OBJDIR),*.oc)
	$(call deletefiles,$(OBJDIR),*.omm)
	$(call deletefiles,$(OBJDIR),*.omoc)
	$(call deletefiles,$(OBJDIR),*.orcc)
	$(call deletefiles,$(LIBDIR),$(NAME)$(SUFFIX)$(EXTLIB))
