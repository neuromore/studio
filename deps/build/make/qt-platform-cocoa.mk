
include platforms/detect-host.mk
include platforms/$(DETECTED_OS)-$(DETECTED_ARCH)-$(TARGET_OS)-$(TARGET_ARCH).mk

NAME       = qt-platform-cocoa
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
             -DQT_NO_DBUS \
             -DQT_USE_QSTRINGBUILDER \
             -DHAVE_CONFIG_H \
             -DUNICODE \
             -D_ENABLE_EXTENDED_ALIGNED_STORAGE
INCLUDES  := $(INCLUDES) \
             -I../../src \
             -I$(INCDIR) \
             -I$(INCDIR)/wintab \
             -I$(INCDIR)/freetype \
             -I$(INCDIRQT) \
             -I$(INCDIRQT)/private \
             -I$(SRCDIR) \
             -I$(UICDIR) \
             -I$(MOCDIR) \
             -I$(INCDIR)/qt \
             -I$(INCDIR)/qt/QtCore \
             -I$(INCDIR)/qt/QtGui \
             -I$(INCDIR)/qt/QtWidgets \
             -I$(INCDIR)/qt/QtFontDatabaseSupport \
             -I$(INCDIR)/qt/QtPrintSupport \
             -I$(INCDIR)/qt/QtClipboardSupport \
             -I$(INCDIR)/qt/QtClipboardSupport/private \
             -I$(INCDIR)/qt/QtGlxSupport \
             -I$(INCDIR)/qt/QtGlxSupport/private \
             -I$(INCDIR)/qt/QtEdidSupport \
             -I$(INCDIR)/qt/QtEdidSupport/private \
             -I$(INCDIR)/qt/QtServiceSupport \
             -I$(INCDIR)/qt/QtServiceSupport/private \
             -I$(INCDIR)/qt/QtThemeSupport \
             -I$(INCDIR)/qt/QtThemeSupport/private
CXXFLAGS  := $(CXXFLAGS) -std=c++17
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
DEFINES   := $(DEFINES) -D_CRT_SECURE_NO_WARNINGS
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
             -DQT_QPA_DEFAULT_PLATFORM_NAME=\"cocoa\"
CXXFLAGS  := $(CXXFLAGS) \
             -Wno-deprecated-declarations \
             -Wno-enum-compare-switch
CFLAGS    := $(CFLAGS)
INCLUDES  := $(INCLUDES) -I$(INCDIR)/qt/mkspecs/macx-clang
MOCH      := $(MOCH) \
             qcocoaclipboard.cpp \
             qcocoaeventdispatcher.cpp \
             qcocoaintegration.cpp \
             qcocoainputcontext.cpp \
             qcocoamenubar.cpp \
             qcocoanativeinterface.cpp \
             qcocoawindow.cpp
MOCC      := $(MOCC) main.mocmm
MOCO      := $(MOCO) \
             qcocoaeventdispatcher.omoc \
             qcocoainputcontext.omoc \
             qcocoamenubar.omoc \
             qcocoanativeinterface.omoc
RCCH      := $(RCCH) qcocoaresources.cpp
RCCO      := $(RCCO) qcocoaresources.orcc
UICH      := $(UICH)
OBJS      := $(OBJS) \
             main.omm \
             messages.o \
             qabstractfileiconengine.o \
             qaccessiblebridgeutils.o \
             qcocoaaccessibility.omm \
             qcocoaaccessibilityelement.omm \
             qcocoaapplication.omm \
             qcocoaapplicationdelegate.omm \
             qcocoabackingstore.omm \
             qcocoaclipboard.omm \
             qcocoacolordialoghelper.omm \
             qcocoacursor.omm \
             qcocoadrag.omm \
             qcocoaeventdispatcher.omm \
             qcocoafiledialoghelper.omm \
             qcocoafontdialoghelper.omm \
             qcocoaglcontext.omm \
             qcocoahelpers.omm \
             qcocoainputcontext.omm \
             qcocoaintegration.omm \
             qcocoaintrospection.omm \
             qcocoakeymapper.omm \
             qcocoamenu.omm \
             qcocoamenubar.omm \
             qcocoamenuitem.omm \
             qcocoamenuloader.omm \
             qcocoamimetypes.omm \
             qcocoanativeinterface.omm \
             qcocoansmenu.omm \
             qcocoaprintdevice.omm \
             qcocoaprintersupport.omm \
             qcocoascreen.omm \
             qcocoaservices.omm \
             qcocoasystemsettings.omm \
             qcocoasystemtrayicon.omm \
             qcocoatheme.omm \
             qcocoawindow.omm \
             qiosurfacegraphicsbuffer.omm \
             qmacclipboard.omm \
             qmacmime.omm \
             qmultitouch_mac.omm \
             qnsview.omm \
             qnswindow.omm \
             qnswindowdelegate.omm \
             qpaintengine_mac.omm \
             qprintengine_mac.omm \
             qrasterbackingstore.o
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
