
include platforms/detect-host.mk

NAME       = qt-widgets
INCDIR     = ../../include
INCDIRQT   = $(INCDIR)/qt/QtWidgets
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
             -I$(SRCDIR)/.tracegen \
             -I$(UICDIR) \
             -I$(MOCDIR) \
             -I$(INCDIR)/qt \
             -I$(INCDIR)/qt/QtCore \
             -I$(INCDIR)/qt/QtGui
CXXFLAGS  := $(CXXFLAGS) -std=c++17
CFLAGS    := $(CFLAGS)
LINKFLAGS := $(LINKFLAGS)
LINKPATH  := $(LINKPATH)
LINKLIBS  := $(LINKLIBS)
MOCH       = qabstractbutton.cpp \
             qabstractitemdelegate.cpp \
             qabstractitemview.cpp \
             qabstractscrollarea.cpp \
             private/qabstractscrollarea_p.cpp \
             qabstractslider.cpp \
             qabstractspinbox.cpp \
             qaction.cpp \
             qactiongroup.cpp \
             qapplication.cpp \
             private/qbasickeyeventtransition_p.cpp \
             private/qbasicmouseeventtransition_p.cpp \
             qboxlayout.cpp \
             qbuttongroup.cpp \
             qcalendarwidget.cpp \
             qcheckbox.cpp \
             qcolordialog.cpp \
             qcolumnview.cpp \
             private/qcolumnviewgrip_p.cpp \
             qcombobox.cpp \
             private/qcombobox_p.cpp \
             qcommandlinkbutton.cpp \
             qcommonstyle.cpp \
             qcompleter.cpp \
             private/qcompleter_p.cpp \
             qdatawidgetmapper.cpp \
             qdatetimeedit.cpp \
             private/qdatetimeedit_p.cpp \
             qdesktopwidget.cpp \
             private/qdesktopwidget_p.cpp \
             qdial.cpp \
             qdialog.cpp \
             qdialogbuttonbox.cpp \
             qdirmodel.cpp \
             qdockwidget.cpp \
             private/qdockwidget_p.cpp \
             qerrormessage.cpp \
             qfiledialog.cpp \
             private/qfileinfogatherer_p.cpp \
             qfilesystemmodel.cpp \
             private/qflickgesture_p.cpp \
             qfocusframe.cpp \
             qfontcombobox.cpp \
             qfontdialog.cpp \
             qformlayout.cpp \
             qframe.cpp \
             private/qfusionstyle_p.cpp \
             qgesture.cpp \
             private/qgesturemanager_p.cpp \
             qgraphicsanchorlayout.cpp \
             qgraphicseffect.cpp \
             private/qgraphicseffect_p.cpp \
             qgraphicsitem.cpp \
             qgraphicsitemanimation.cpp \
             qgraphicsproxywidget.cpp \
             qgraphicsscene.cpp \
             private/qgraphicsscenebsptreeindex_p.cpp \
             private/qgraphicssceneindex_p.cpp \
             private/qgraphicsscenelinearindex_p.cpp \
             qgraphicstransform.cpp \
             qgraphicsview.cpp \
             qgraphicswidget.cpp \
             qgridlayout.cpp \
             qgroupbox.cpp \
             qheaderview.cpp \
             qinputdialog.cpp \
             qitemdelegate.cpp \
             private/qitemeditorfactory_p.cpp \
             qkeyeventtransition.cpp \
             qkeysequenceedit.cpp \
             qlabel.cpp \
             qlayout.cpp \
             qlcdnumber.cpp \
             qlineedit.cpp \
             private/qlineedit_p.cpp \
             qlistview.cpp \
             qlistwidget.cpp \
             private/qlistwidget_p.cpp \
             qmainwindow.cpp \
             private/qmainwindowlayout_p.cpp \
             qmdiarea.cpp \
             qmdisubwindow.cpp \
             qmenu.cpp \
             qmenubar.cpp \
             qmessagebox.cpp \
             qmouseeventtransition.cpp \
             qopenglwidget.cpp \
             private/qpixmapfilter_p.cpp \
             qplaintextedit.cpp \
             private/qplaintextedit_p.cpp \
             qprogressbar.cpp \
             qprogressdialog.cpp \
             qproxystyle.cpp \
             qpushbutton.cpp \
             qradiobutton.cpp \
             qrubberband.cpp \
             qscrollarea.cpp \
             qscrollbar.cpp \
             qscroller.cpp \
             private/qscroller_p.cpp \
             qshortcut.cpp \
             private/qsidebar_p.cpp \
             qsizegrip.cpp \
             qsizepolicy.cpp \
             qslider.cpp \
             qspinbox.cpp \
             qsplashscreen.cpp \
             qsplitter.cpp \
             qstackedlayout.cpp \
             qstackedwidget.cpp \
             qstatusbar.cpp \
             qstyle.cpp \
             private/qstyleanimation_p.cpp \
             qstyleditemdelegate.cpp \
             qstyleplugin.cpp \
             private/qstylesheetstyle_p.cpp \
             qsystemtrayicon.cpp \
             private/qsystemtrayicon_p.cpp \
             qtabbar.cpp \
             qtableview.cpp \
             qtablewidget.cpp \
             private/qtablewidget_p.cpp \
             qtabwidget.cpp \
             qtextbrowser.cpp \
             qtextedit.cpp \
             qtoolbar.cpp \
             private/qtoolbarextension_p.cpp \
             private/qtoolbarlayout_p.cpp \
             private/qtoolbarseparator_p.cpp \
             qtoolbox.cpp \
             qtoolbutton.cpp \
             qtreeview.cpp \
             qtreewidget.cpp \
             private/qtreewidget_p.cpp \
             qundogroup.cpp \
             qundostack.cpp \
             private/qundostack_p.cpp \
             qundoview.cpp \
             qwidget.cpp \
             qwidgetaction.cpp \
             private/qwidgetanimator_p.cpp \
             private/qwidgetbackingstore_p.cpp \
             private/qwidgetlinecontrol_p.cpp \
             private/qwidgetresizehandler_p.cpp \
             private/qwidgettextcontrol_p.cpp \
             private/qwidgetwindow_p.cpp \
             private/qwindowcontainer_p.cpp \
             private/qwindowsstyle_p.cpp \
             qwizard.cpp 
MOCC       = dialogs/qcolordialog.moc \
             dialogs/qfontdialog.moc \
             dialogs/qinputdialog.moc \
             dialogs/qmessagebox.moc \
             itemviews/qitemeditorfactory.moc \
             itemviews/qlistwidget.moc \
             itemviews/qtableview.moc \
             kernel/qtooltip.moc \
             kernel/qwhatsthis.moc \
             util/qsystemtrayicon_x11.moc \
             util/qundoview.moc \
             widgets/qcalendarwidget.moc \
             widgets/qdockwidget.moc \
             widgets/qeffects.moc \
             widgets/qfontcombobox.moc \
             widgets/qmdisubwindow.moc \
             widgets/qmenu.moc \
             widgets/qtabbar.moc \
             widgets/qtoolbox.moc
MOCO       =
RCCH       = styles/qstyle.cpp dialogs/qmessagebox.cpp
RCCO       = qstyle.orcc qmessagebox.orcc
UICH       = dialogs/qfiledialog.h
OBJS       = accessible/complexwidgets.o \
             accessible/itemviews.o \
             accessible/qaccessiblemenu.o \
             accessible/qaccessiblewidget.o \
             accessible/qaccessiblewidgetfactory.o \
             accessible/qaccessiblewidgets.o \
             accessible/rangecontrols.o \
             accessible/simplewidgets.o \
             dialogs/qcolordialog.o \
             dialogs/qdialog.o \
             dialogs/qerrormessage.o \
             dialogs/qfiledialog.o \
             dialogs/qfileinfogatherer.o \
             dialogs/qfilesystemmodel.o \
             dialogs/qfontdialog.o \
             dialogs/qinputdialog.o \
             dialogs/qmessagebox.o \
             dialogs/qprogressdialog.o \
             dialogs/qsidebar.o \
             effects/qgraphicseffect.o \
             effects/qpixmapfilter.o \
             graphicsview/qgraphicsanchorlayout.o \
             graphicsview/qgraphicsanchorlayout_p.o \
             graphicsview/qgraphicsgridlayout.o \
             graphicsview/qgraphicsgridlayoutengine.o \
             graphicsview/qgraphicsitem.o \
             graphicsview/qgraphicsitemanimation.o \
             graphicsview/qgraphicslayout.o \
             graphicsview/qgraphicslayout_p.o \
             graphicsview/qgraphicslayoutitem.o \
             graphicsview/qgraphicslayoutstyleinfo.o \
             graphicsview/qgraphicslinearlayout.o \
             graphicsview/qgraphicsproxywidget.o \
             graphicsview/qgraphicsscene.o \
             graphicsview/qgraphicsscene_bsp.o \
             graphicsview/qgraphicsscenebsptreeindex.o \
             graphicsview/qgraphicssceneevent.o \
             graphicsview/qgraphicssceneindex.o \
             graphicsview/qgraphicsscenelinearindex.o \
             graphicsview/qgraphicstransform.o \
             graphicsview/qgraphicsview.o \
             graphicsview/qgraphicswidget.o \
             graphicsview/qgraphicswidget_p.o \
             graphicsview/qsimplex_p.o \
             itemviews/qabstractitemdelegate.o \
             itemviews/qabstractitemview.o \
             itemviews/qbsptree.o \
             itemviews/qcolumnview.o \
             itemviews/qcolumnviewgrip.o \
             itemviews/qdatawidgetmapper.o \
             itemviews/qdirmodel.o \
             itemviews/qfileiconprovider.o \
             itemviews/qheaderview.o \
             itemviews/qitemdelegate.o \
             itemviews/qitemeditorfactory.o \
             itemviews/qlistview.o \
             itemviews/qlistwidget.o \
             itemviews/qstyleditemdelegate.o \
             itemviews/qtableview.o \
             itemviews/qtablewidget.o \
             itemviews/qtreeview.o \
             itemviews/qtreewidget.o \
             itemviews/qtreewidgetitemiterator.o \
             kernel/qaction.o \
             kernel/qactiongroup.o \
             kernel/qapplication.o \
             kernel/qboxlayout.o \
             kernel/qdesktopwidget.o \
             kernel/qformlayout.o \
             kernel/qgesture.o \
             kernel/qgesturemanager.o \
             kernel/qgesturerecognizer.o \
             kernel/qgridlayout.o \
             kernel/qlayout.o \
             kernel/qlayoutengine.o \
             kernel/qlayoutitem.o \
             kernel/qmacgesturerecognizer.o \
             kernel/qopenglwidget.o \
             kernel/qshortcut.o \
             kernel/qsizepolicy.o \
             kernel/qstackedlayout.o \
             kernel/qstandardgestures.o \
             kernel/qtestsupport_widgets.o \
             kernel/qtooltip.o \
             kernel/qwhatsthis.o \
             kernel/qwidget.o \
             kernel/qwidgetaction.o \
             kernel/qwidgetbackingstore.o \
             kernel/qwidgetsvariant.o \
             kernel/qwidgetwindow.o \
             kernel/qwindowcontainer.o \
             statemachine/qbasickeyeventtransition.o \
             statemachine/qbasicmouseeventtransition.o \
             statemachine/qguistatemachine.o \
             statemachine/qkeyeventtransition.o \
             statemachine/qmouseeventtransition.o \
             styles/qcommonstyle.o \
             styles/qdrawutil.o \
             styles/qfusionstyle.o \
             styles/qpixmapstyle.o \
             styles/qproxystyle.o \
             styles/qstyle.o \
             styles/qstyleanimation.o \
             styles/qstylefactory.o \
             styles/qstylehelper.o \
             styles/qstyleoption.o \
             styles/qstyleplugin.o \
             styles/qstylesheetstyle.o \
             styles/qstylesheetstyle_default.o \
             styles/qwindowsstyle.o \
             util/qcolormap.o \
             util/qcompleter.o \
             util/qflickgesture.o \
             util/qscroller.o \
             util/qscrollerproperties.o \
             util/qsystemtrayicon.o \
             util/qsystemtrayicon_qpa.o \
             util/qundogroup.o \
             util/qundostack.o \
             util/qundoview.o \
             widgets/qabstractbutton.o \
             widgets/qabstractscrollarea.o \
             widgets/qabstractslider.o \
             widgets/qabstractspinbox.o \
             widgets/qbuttongroup.o \
             widgets/qcalendarwidget.o \
             widgets/qcheckbox.o \
             widgets/qcombobox.o \
             widgets/qcommandlinkbutton.o \
             widgets/qdatetimeedit.o \
             widgets/qdial.o \
             widgets/qdialogbuttonbox.o \
             widgets/qdockarealayout.o \
             widgets/qdockwidget.o \
             widgets/qeffects.o \
             widgets/qfocusframe.o \
             widgets/qfontcombobox.o \
             widgets/qframe.o \
             widgets/qgroupbox.o \
             widgets/qkeysequenceedit.o \
             widgets/qlabel.o \
             widgets/qlcdnumber.o \
             widgets/qlineedit.o \
             widgets/qlineedit_p.o \
             widgets/qmainwindow.o \
             widgets/qmainwindowlayout.o \
             widgets/qmdiarea.o \
             widgets/qmdisubwindow.o \
             widgets/qmenu.o \
             widgets/qmenubar.o \
             widgets/qplaintextedit.o \
             widgets/qprogressbar.o \
             widgets/qpushbutton.o \
             widgets/qradiobutton.o \
             widgets/qrubberband.o \
             widgets/qscrollarea.o \
             widgets/qscrollbar.o \
             widgets/qsizegrip.o \
             widgets/qslider.o \
             widgets/qspinbox.o \
             widgets/qsplashscreen.o \
             widgets/qsplitter.o \
             widgets/qstackedwidget.o \
             widgets/qstatusbar.o \
             widgets/qtabbar.o \
             widgets/qtabwidget.o \
             widgets/qtextbrowser.o \
             widgets/qtextedit.o \
             widgets/qtoolbar.o \
             widgets/qtoolbararealayout.o \
             widgets/qtoolbarextension.o \
             widgets/qtoolbarlayout.o \
             widgets/qtoolbarseparator.o \
             widgets/qtoolbox.o \
             widgets/qtoolbutton.o \
             widgets/qwidgetanimator.o \
             widgets/qwidgetlinecontrol.o \
             widgets/qwidgetresizehandler.o \
             widgets/qwidgettextcontrol.o

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
OBJS      := $(OBJS) \
             dialogs/qwizard.o \
             dialogs/qwizard_win.o
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
MOCH      := $(MOCH) \
             qmaccocoaviewcontainer_mac.cpp \
             qmacnativewidget_mac.cpp
MOCC      := $(MOCC)
MOCO      := $(MOCO) \
             qmaccocoaviewcontainer_mac.omoc \
             qmacnativewidget_mac.omoc
RCCH      := $(RCCH)
RCCO      := $(RCCO)
UICH      := $(UICH)
OBJS      := $(OBJS) \
             util/qscroller_mac.omm \
             widgets/qmaccocoaviewcontainer_mac.omm \
             widgets/qmacnativewidget_mac.omm \
             widgets/qmenu_mac.omm
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
OBJS      := $(OBJS) util/qsystemtrayicon_x11.o
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
