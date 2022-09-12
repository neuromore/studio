
include platforms/detect-host.mk

NAME       = qt-printsupport
INCDIR     = ../../include
INCDIRQT   = $(INCDIR)/qt/QtPrintSupport
SRCDIR     = ../../src/$(NAME)
MOCDIR     = $(SRCDIR)/.moc
RCCDIR     = $(SRCDIR)/.rcc
UICDIR     = $(SRCDIR)/.uic
OBJDIR    := $(OBJDIR)/$(NAME)
DEFINES   := $(DEFINES) \
             -DCHROMIUM_ZLIB_NO_CHROMECONF \
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
             -I$(INCDIR)/qt/QtPrintSupport \
             -I$(INCDIR)/qt/QtPrintSupport/private \
             -I$(INCDIR)/qt/QtPrintSupport/qpa \
             -I$(INCDIR)/qt/QtWidgets \
             -I$(INCDIR)/qt/QtGui
CXXFLAGS  := $(CXXFLAGS) -std=c++17
LINKFLAGS := $(LINKFLAGS)
LINKPATH  := $(LINKPATH)
LINKLIBS  := $(LINKLIBS)
MOCH       = qabstractprintdialog.cpp \
             qpagesetupdialog.cpp \
             qprintdialog.cpp \
             qprintpreviewdialog.cpp \
             qprintpreviewwidget.cpp
MOCC       = dialogs/qprintpreviewdialog.moc \
             widgets/qprintpreviewwidget.moc	 
MOCO       = qabstractprintdialog.omoc
RCCH       = dialogs/qprintdialog.cpp
RCCO       = dialogs/qprintdialog.orcc
UICH       = dialogs/qpagesetupwidget.h \
             dialogs/qprintpropertieswidget.h \
             dialogs/qprintsettingsoutput.h \
             dialogs/qprintwidget.h
OBJS       = dialogs/qabstractprintdialog.o \
             dialogs/qpagesetupdialog.o \
             dialogs/qprintpreviewdialog.o \
             kernel/qpaintengine_alpha.o \
             kernel/qpaintengine_preview.o \
             kernel/qplatformprintdevice.o \
             kernel/qplatformprintersupport.o \
             kernel/qplatformprintplugin.o \
             kernel/qprintdevice.o \
             kernel/qprintengine_pdf.o \
             kernel/qprinter.o \
             kernel/qprinterinfo.o \
             widgets/qprintpreviewwidget.o

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
INCLUDES  := $(INCLUDES) -I$(INCDIR)/qt/mkspecs/win32-clang-msvc
MOCH      := $(MOCH)
MOCC      := $(MOCC)
MOCO      := $(MOCO)
RCCH      := $(RCCH)
RCCO      := $(RCCO)
UICH      := $(UICH)
OBJS      := $(OBJS) \
             dialogs/qpagesetupdialog_win.o \
             dialogs/qprintdialog_win.o \
             kernel/qprintengine_win.o
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
INCLUDES  := $(INCLUDES) -I$(INCDIR)/qt/mkspecs/macx-clang
MOCH      := $(MOCH) \
             qpa/qplatformprintplugin.cpp
MOCC      := $(MOCC)
MOCO      := $(MOCO) \
             qplatformprintplugin.omoc
RCCH      := $(RCCH)
RCCO      := $(RCCO)
UICH      := $(UICH)
OBJS      := $(OBJS) \
             dialogs/qpagesetupdialog_mac.omm \
             dialogs/qprintdialog_mac.omm
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
INCLUDES  := $(INCLUDES) -I$(INCDIR)/qt/mkspecs/linux-clang
MOCH      := $(MOCH)
MOCC      := $(MOCC) \
             dialogs/qprintdialog_unix.moc
MOCO      := $(MOCO)
RCCH      := $(RCCH)
RCCO      := $(RCCO)
UICH      := $(UICH)
OBJS      := $(OBJS) \
             dialogs/qpagesetupdialog_unix.o \
             dialogs/qprintdialog_unix.o
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
