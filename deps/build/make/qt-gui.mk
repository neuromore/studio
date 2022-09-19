
include platforms/detect-host.mk

NAME       = qt-gui
INCDIR     = ../../include
INCDIRQT   = $(INCDIR)/qt/QtGui
SRCDIR     = ../../src/$(NAME)
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
             -I$(INCDIR)/libpng \
             -I$(INCDIR)/libjpeg \
             -I$(INCDIR)/zlib \
             -I$(INCDIRQT) \
             -I$(INCDIRQT)/private \
             -I$(INCDIRQT)/imageformats \
             -I$(INCDIRQT)/qpa \
             -I$(SRCDIR) \
             -I$(MOCDIR) \
             -I$(UICDIR) \
             -I$(INCDIR)/qt \
             -I$(INCDIR)/qt/QtCore
CXXFLAGS  := $(CXXFLAGS) \
             -Wno-deprecated-declarations \
             -Wno-bitwise-conditional-parentheses \
             -Wno-tautological-constant-compare \
             -Wno-implicit-const-int-float-conversion \
             -Wno-unknown-warning-option \
             -std=c++17
CFLAGS    := $(CFLAGS)
LINKFLAGS := $(LINKFLAGS)
LINKPATH  := $(LINKPATH)
LINKLIBS  := $(LINKLIBS)
MOCH       = qabstracttextdocumentlayout.cpp \
             qaccessible.cpp \
             qaccessiblebridge.cpp \
             private/qaccessiblecache_p.cpp \
             qaccessibleplugin.cpp \
             qbrush.cpp \
             qclipboard.cpp \
             private/qdnd_p.cpp \
             qdrag.cpp \
             qevent.cpp \
             qfont.cpp \
             qfontdatabase.cpp \
             qgenericplugin.cpp \
             qguiapplication.cpp \
             qiconengineplugin.cpp \
             qimage.cpp \
             qimageiohandler.cpp \
             private/qinputcontrol_p.cpp \
             private/qinputdevicemanager_p.cpp \
             qinputmethod.cpp \
             private/qinternalmimedata_p.cpp \
             private/qkeymapper_p.cpp \
             qkeysequence.cpp \
             qmovie.cpp \
             qoffscreensurface.cpp \
             qopenglcontext.cpp \
             qopengldebug.cpp \
             private/qopenglengineshadermanager_p.cpp \
             qopenglshaderprogram.cpp \
             qopengltexture.cpp \
             qopenglvertexarrayobject.cpp \
             qopenglwindow.cpp \
             qpaintdevicewindow.cpp \
             qpainter.cpp \
             qpalette.cpp \
             qpdfwriter.cpp \
             qpictureformatplugin.cpp \
             qpa/qplatformbackingstore.cpp \
             qpa/qplatformdialoghelper.cpp \
             qpa/qplatformgraphicsbuffer.cpp \
             qpa/qplatforminputcontext.cpp \
             qpa/qplatforminputcontextplugin_p.cpp \
             qpa/qplatformintegrationplugin.cpp \
             qpa/qplatformmenu.cpp \
             qpa/qplatformnativeinterface.cpp \
             qpa/qplatformsharedgraphicscache.cpp \
             qpa/qplatformsystemtrayicon.cpp \
             qpa/qplatformthemeplugin.cpp \
             qrasterwindow.cpp \
             qscreen.cpp \
             qsessionmanager.cpp \
             private/qshaderlanguage_p.cpp \
             private/qshapedpixmapdndwindow_p.cpp \
             qstandarditemmodel.cpp \
             qstylehints.cpp \
             qsurface.cpp \
             qsurfaceformat.cpp \
             qsyntaxhighlighter.cpp \
             qtextdocument.cpp \
             private/qtextdocumentlayout_p.cpp \
             qtextformat.cpp \
             private/qtextimagehandler_p.cpp \
             qtextlist.cpp \
             qtextobject.cpp \
             qtexttable.cpp \
             qtouchdevice.cpp \
             qvalidator.cpp \
             qwindow.cpp \
             imageformats/qgifplugin.cpp \
             imageformats/qicoplugin.cpp \
             imageformats/qjpegplugin.cpp
MOCC       = image/qpixmapcache.moc \
             util/qdesktopservices.moc
MOCO       = qaccessible.omoc \
             qaccessiblebridge.omoc \
             qaccessiblecache_p.omoc \
             qaccessibleplugin.omoc \
             qbrush.omoc \
             qclipboard.omoc \
             qdnd_p.omoc \
             qdrag.omoc \
             qevent.omoc \
             qfont.omoc \
             qfontdatabase.omoc \
             qgenericplugin.omoc \
             qgifplugin.omoc \
             qicoplugin.omoc \
             qjpegplugin.omoc \
             qiconengineplugin.omoc \
             qimage.omoc \
             qimageiohandler.omoc \
             qinputcontrol_p.omoc \
             qinputdevicemanager_p.omoc \
             qinternalmimedata_p.omoc \
             qkeymapper_p.omoc \
             qkeysequence.omoc \
             qoffscreensurface.omoc \
             qopenglengineshadermanager_p.omoc \
             qopenglshaderprogram.omoc \
             qopengltexture.omoc \
             qopenglwindow.omoc \
             qpaintdevicewindow.omoc \
             qpainter.omoc \
             qpalette.omoc \
             qpdfwriter.omoc \
             qpictureformatplugin.omoc \
             qplatformbackingstore.omoc \
             qplatformdialoghelper.omoc \
             qplatformgraphicsbuffer.omoc \
             qplatforminputcontext.omoc \
             qplatforminputcontextplugin_p.omoc \
             qplatformintegrationplugin.omoc \
             qplatformmenu.omoc \
             qplatformnativeinterface.omoc \
             qplatformsharedgraphicscache.omoc \
             qplatformthemeplugin.omoc \
             qrasterwindow.omoc \
             qscreen.omoc \
             qsessionmanager.omoc \
             qshaderlanguage_p.omoc \
             qshapedpixmapdndwindow_p.omoc \
             qstylehints.omoc \
             qsurface.omoc \
             qsurfaceformat.omoc \
             qtextdocument.omoc \
             qtextformat.omoc \
             qtextimagehandler_p.omoc \
             qtextlist.omoc \
             qtextobject.omoc \
             qtexttable.omoc \
             qtouchdevice.omoc \
             qvalidator.omoc
RCCH       = painting/qpdf.cpp \
             painting/qmake_webgradients.cpp
RCCO       = qpdf.orcc \
             qmake_webgradients.orcc
UICH       =
OBJS       = accessible/qaccessible.o \
             accessible/qaccessiblebridge.o \
             accessible/qaccessiblecache.o \
             accessible/qaccessibleobject.o \
             accessible/qaccessibleplugin.o \
             accessible/qplatformaccessibility.o \
             animation/qguivariantanimation.o \
             image/qbitmap.o \
             image/qbmphandler.o \
             image/qicon.o \
             image/qiconengine.o \
             image/qiconengineplugin.o \
             image/qiconloader.o \
             image/qimage.o \
             image/qimage_compat.o \
             image/qimage_conversions.o \
             image/qimageiohandler.o \
             image/qimagepixmapcleanuphooks.o \
             image/qimagereader.o \
             image/qimagereaderwriterhelpers.o \
             image/qimagewriter.o \
             image/qmovie.o \
             image/qpaintengine_pic.o \
             image/qpicture.o \
             image/qpictureformatplugin.o \
             image/qpixmap.o \
             image/qpixmap_blitter.o \
             image/qpixmap_raster.o \
             image/qpixmapcache.o \
             image/qplatformpixmap.o \
             image/qpnghandler.o \
             image/qppmhandler.o \
             image/qxbmhandler.o \
             image/qxpmhandler.o \
             imageformats/gif/qgifhandler.o \
             imageformats/gif/qgifplugin.o \
             imageformats/ico/qicohandler.o \
             imageformats/ico/qicoplugin.o \
             imageformats/jpeg/qjpeghandler.o \
             imageformats/jpeg/qjpegplugin.o \
             itemmodels/qstandarditemmodel.o \
             kernel/qclipboard.o \
             kernel/qcursor.o \
             kernel/qdnd.o \
             kernel/qdrag.o \
             kernel/qevent.o \
             kernel/qgenericplugin.o \
             kernel/qgenericpluginfactory.o \
             kernel/qguiapplication.o \
             kernel/qguivariant.o \
             kernel/qhighdpiscaling.o \
             kernel/qinputdevicemanager.o \
             kernel/qinputmethod.o \
             kernel/qinternalmimedata.o \
             kernel/qkeymapper.o \
             kernel/qkeysequence.o \
             kernel/qoffscreensurface.o \
             kernel/qopenglcontext.o \
             kernel/qopenglwindow.o \
             kernel/qpaintdevicewindow.o \
             kernel/qpalette.o \
             kernel/qpixelformat.o \
             kernel/qplatformclipboard.o \
             kernel/qplatformcursor.o \
             kernel/qplatformdialoghelper.o \
             kernel/qplatformdrag.o \
             kernel/qplatformgraphicsbuffer.o \
             kernel/qplatformgraphicsbufferhelper.o \
             kernel/qplatforminputcontext.o \
             kernel/qplatforminputcontextfactory.o \
             kernel/qplatforminputcontextplugin.o \
             kernel/qplatformintegration.o \
             kernel/qplatformintegrationfactory.o \
             kernel/qplatformintegrationplugin.o \
             kernel/qplatformmenu.o \
             kernel/qplatformnativeinterface.o \
             kernel/qplatformoffscreensurface.o \
             kernel/qplatformopenglcontext.o \
             kernel/qplatformscreen.o \
             kernel/qplatformservices.o \
             kernel/qplatformsessionmanager.o \
             kernel/qplatformsurface.o \
             kernel/qplatformsystemtrayicon.o \
             kernel/qplatformtheme.o \
             kernel/qplatformthemefactory.o \
             kernel/qplatformthemeplugin.o \
             kernel/qplatformwindow.o \
             kernel/qrasterwindow.o \
             kernel/qscreen.o \
             kernel/qsessionmanager.o \
             kernel/qshapedpixmapdndwindow.o \
             kernel/qshortcutmap.o \
             kernel/qsimpledrag.o \
             kernel/qstylehints.o \
             kernel/qsurface.o \
             kernel/qsurfaceformat.o \
             kernel/qtestsupport_gui.o \
             kernel/qtouchdevice.o \
             kernel/qwindow.o \
             kernel/qwindowsysteminterface.o \
             math3d/qmatrix4x4.o \
             math3d/qquaternion.o \
             math3d/qvector2d.o \
             math3d/qvector3d.o \
             math3d/qvector4d.o \
             opengl/qopengl.o \
             opengl/qopengl2pexvertexarray.o \
             opengl/qopenglbuffer.o \
             opengl/qopenglcustomshaderstage.o \
             opengl/qopengldebug.o \
             opengl/qopenglengineshadermanager.o \
             opengl/qopenglframebufferobject.o \
             opengl/qopenglfunctions.o \
             opengl/qopenglgradientcache.o \
             opengl/qopenglpaintdevice.o \
             opengl/qopenglpaintengine.o \
             opengl/qopenglpixeltransferoptions.o \
             opengl/qopenglprogrambinarycache.o \
             opengl/qopenglshaderprogram.o \
             opengl/qopengltexture.o \
             opengl/qopengltextureblitter.o \
             opengl/qopengltexturecache.o \
             opengl/qopengltextureglyphcache.o \
             opengl/qopengltexturehelper.o \
             opengl/qopengltextureuploader.o \
             opengl/qopenglversionfunctions.o \
             opengl/qopenglversionfunctionsfactory.o \
             opengl/qopenglvertexarrayobject.o \
             painting/qbackingstore.o \
             painting/qbezier.o \
             painting/qblendfunctions.o \
             painting/qblittable.o \
             painting/qbrush.o \
             painting/qcolor.o \
             painting/qcolorprofile.o \
             painting/qcompositionfunctions.o \
             painting/qcosmeticstroker.o \
             painting/qcssutil.o \
             painting/qdrawhelper.o \
             painting/qemulationpaintengine.o \
             painting/qgrayraster.oc \
             painting/qimagescale.o \
             painting/qmatrix.o \
             painting/qmemrotate.o \
             painting/qoutlinemapper.o \
             painting/qpagedpaintdevice.o \
             painting/qpagelayout.o \
             painting/qpagesize.o \
             painting/qpaintdevice.o \
             painting/qpaintengine.o \
             painting/qpaintengine_blitter.o \
             painting/qpaintengine_raster.o \
             painting/qpaintengineex.o \
             painting/qpainter.o \
             painting/qpainterpath.o \
             painting/qpathclipper.o \
             painting/qpathsimplifier.o \
             painting/qpdf.o \
             painting/qpdfwriter.o \
             painting/qpen.o \
             painting/qplatformbackingstore.o \
             painting/qpolygon.o \
             painting/qrasterizer.o \
             painting/qregion.o \
             painting/qstroker.o \
             painting/qtextureglyphcache.o \
             painting/qtransform.o \
             painting/qtriangulatingstroker.o \
             painting/qtriangulator.o \
             text/qabstracttextdocumentlayout.o \
             text/qcssparser.o \
             text/qdistancefield.o \
             text/qfont.o \
             text/qfontdatabase.o \
             text/qfontengine.o \
             text/qfontengine_qpf2.o \
             text/qfontengineglyphcache.o \
             text/qfontmetrics.o \
             text/qfontsubset.o \
             text/qfontsubset_agl.o \
             text/qglyphrun.o \
             text/qharfbuzzng.o \
             text/qinputcontrol.o \
             text/qplatformfontdatabase.o \
             text/qrawfont.o \
             text/qstatictext.o \
             text/qsyntaxhighlighter.o \
             text/qtextcursor.o \
             text/qtextdocument.o \
             text/qtextdocument_p.o \
             text/qtextdocumentfragment.o \
             text/qtextdocumentlayout.o \
             text/qtextdocumentwriter.o \
             text/qtextengine.o \
             text/qtextformat.o \
             text/qtexthtmlparser.o \
             text/qtextimagehandler.o \
             text/qtextlayout.o \
             text/qtextlist.o \
             text/qtextobject.o \
             text/qtextodfwriter.o \
             text/qtextoption.o \
             text/qtexttable.o \
             text/qzip.o \
             util/qdesktopservices.o \
             util/qgridlayoutengine.o \
             util/qktxhandler.o \
             util/qlayoutpolicy.o \
             util/qpkmhandler.o \
             util/qshaderformat.o \
             util/qshadergenerator.o \
             util/qshadergraph.o \
             util/qshadergraphloader.o \
             util/qshaderlanguage.o \
             util/qshadernode.o \
             util/qshadernodeport.o \
             util/qshadernodesloader.o \
             util/qtexturefiledata.o \
             util/qtexturefilereader.o \
             util/qvalidator.o

################################################################################################

ifeq ($(TARGET_ARCH),x86)
DEFINES   := $(DEFINES)
OBJS      := $(OBJS) \
             image/qimage_ssse3.o \
             painting/qdrawhelper_sse2.o \
             painting/qdrawhelper_sse4.o \
             painting/qdrawhelper_ssse3.o \
             painting/qimagescale_sse4.o
endif

ifeq ($(TARGET_ARCH),x64)
DEFINES   := $(DEFINES)
OBJS      := $(OBJS) \
             image/qimage_ssse3.o \
             painting/qdrawhelper_avx2.o \
             painting/qdrawhelper_sse2.o \
             painting/qdrawhelper_sse4.o \
             painting/qdrawhelper_ssse3.o \
             painting/qimagescale_sse4.o
endif

ifeq ($(TARGET_ARCH),arm)
DEFINES   := $(DEFINES)
OBJS      := $(OBJS) \
             image/qimage_neon.o \
             painting/qdrawhelper_neon.o \
             painting/qimagescale_neon.o
endif

ifeq ($(TARGET_ARCH),arm64)
DEFINES   := $(DEFINES)
OBJS      := $(OBJS) \
             image/qimage_neon.o \
             painting/qdrawhelper_neon.o \
             painting/qimagescale_neon.o
endif

################################################################################################
# WINDOWS
ifeq ($(TARGET_OS),win)
QTMOC     := $(QTMOC) -DQ_OS_WIN
DEFINES   := $(DEFINES) \
             -D_CRT_SECURE_NO_WARNINGS \
             -DQT_QPA_DEFAULT_PLATFORM_NAME=\"windows\" \
             -DQT_NO_FONTCONFIG \
             -DQT_FEATURE_dbus=-1
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
             image/qpixmap_win.o \
             opengl/qopenglfunctions_1_0.o \
             opengl/qopenglfunctions_1_1.o \
             opengl/qopenglfunctions_1_2.o \
             opengl/qopenglfunctions_1_3.o \
             opengl/qopenglfunctions_1_4.o \
             opengl/qopenglfunctions_1_5.o \
             opengl/qopenglfunctions_2_0.o \
             opengl/qopenglfunctions_2_1.o \
             opengl/qopenglfunctions_3_0.o \
             opengl/qopenglfunctions_3_1.o \
             opengl/qopenglfunctions_3_2_compatibility.o \
             opengl/qopenglfunctions_3_2_core.o \
             opengl/qopenglfunctions_3_3_compatibility.o \
             opengl/qopenglfunctions_3_3_core.o \
             opengl/qopenglfunctions_4_0_compatibility.o \
             opengl/qopenglfunctions_4_0_core.o \
             opengl/qopenglfunctions_4_1_compatibility.o \
             opengl/qopenglfunctions_4_1_core.o \
             opengl/qopenglfunctions_4_2_compatibility.o \
             opengl/qopenglfunctions_4_2_core.o \
             opengl/qopenglfunctions_4_3_compatibility.o \
             opengl/qopenglfunctions_4_3_core.o \
             opengl/qopenglfunctions_4_4_compatibility.o \
             opengl/qopenglfunctions_4_4_core.o \
             opengl/qopenglfunctions_4_5_compatibility.o \
             opengl/qopenglfunctions_4_5_core.o
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
             -DQT_QPA_DEFAULT_PLATFORM_NAME=\"cocoa\" \
             -DQT_FEATURE_dbus=-1
CXXFLAGS  := $(CXXFLAGS)
CFLAGS    := $(CFLAGS)
INCLUDES  := $(INCLUDES) -I$(INCDIR)/qt/mkspecs/macx-clang
MOCH      := $(MOCH)
MOCC      := $(MOCC)
MOCO      := $(MOCO)
RCCH      := $(RCCH)
RCCO      := $(RCCO)
UICH      := $(UICH)
OBJS      := $(OBJS) \
             accessible/qaccessiblecache_mac.omm \
             image/qimage_darwin.omm \
             opengl/qopenglfunctions_1_0.o \
             opengl/qopenglfunctions_1_1.o \
             opengl/qopenglfunctions_1_2.o \
             opengl/qopenglfunctions_1_3.o \
             opengl/qopenglfunctions_1_4.o \
             opengl/qopenglfunctions_1_5.o \
             opengl/qopenglfunctions_2_0.o \
             opengl/qopenglfunctions_2_1.o \
             opengl/qopenglfunctions_3_0.o \
             opengl/qopenglfunctions_3_1.o \
             opengl/qopenglfunctions_3_2_compatibility.o \
             opengl/qopenglfunctions_3_2_core.o \
             opengl/qopenglfunctions_3_3_compatibility.o \
             opengl/qopenglfunctions_3_3_core.o \
             opengl/qopenglfunctions_4_0_compatibility.o \
             opengl/qopenglfunctions_4_0_core.o \
             opengl/qopenglfunctions_4_1_compatibility.o \
             opengl/qopenglfunctions_4_1_core.o \
             opengl/qopenglfunctions_4_2_compatibility.o \
             opengl/qopenglfunctions_4_2_core.o \
             opengl/qopenglfunctions_4_3_compatibility.o \
             opengl/qopenglfunctions_4_3_core.o \
             opengl/qopenglfunctions_4_4_compatibility.o \
             opengl/qopenglfunctions_4_4_core.o \
             opengl/qopenglfunctions_4_5_compatibility.o \
             opengl/qopenglfunctions_4_5_core.o \
             painting/qcoregraphics.omm
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
             -DQT_QPA_DEFAULT_PLATFORM_NAME=\"xcb\" \
             -DQT_FEATURE_fontconfig=1 \
             -DQT_FEATURE_dbus=1
CXXFLAGS  := $(CXXFLAGS)
CFLAGS    := $(CFLAGS)
INCLUDES  := $(INCLUDES) -I$(INCDIR)/qt/mkspecs/linux-clang
MOCH      := $(MOCH)
MOCC      := $(MOCC)
MOCO      := $(MOCO)
RCCH      := $(RCCH)
RCCO      := $(RCCO)
UICH      := $(UICH)
OBJS      := $(OBJS) \
             opengl/qopenglfunctions_1_0.o \
             opengl/qopenglfunctions_1_1.o \
             opengl/qopenglfunctions_1_2.o \
             opengl/qopenglfunctions_1_3.o \
             opengl/qopenglfunctions_1_4.o \
             opengl/qopenglfunctions_1_5.o \
             opengl/qopenglfunctions_2_0.o \
             opengl/qopenglfunctions_2_1.o \
             opengl/qopenglfunctions_3_0.o \
             opengl/qopenglfunctions_3_1.o \
             opengl/qopenglfunctions_3_2_compatibility.o \
             opengl/qopenglfunctions_3_2_core.o \
             opengl/qopenglfunctions_3_3_compatibility.o \
             opengl/qopenglfunctions_3_3_core.o \
             opengl/qopenglfunctions_4_0_compatibility.o \
             opengl/qopenglfunctions_4_0_core.o \
             opengl/qopenglfunctions_4_1_compatibility.o \
             opengl/qopenglfunctions_4_1_core.o \
             opengl/qopenglfunctions_4_2_compatibility.o \
             opengl/qopenglfunctions_4_2_core.o \
             opengl/qopenglfunctions_4_3_compatibility.o \
             opengl/qopenglfunctions_4_3_core.o \
             opengl/qopenglfunctions_4_4_compatibility.o \
             opengl/qopenglfunctions_4_4_core.o \
             opengl/qopenglfunctions_4_5_compatibility.o \
             opengl/qopenglfunctions_4_5_core.o
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
             -DQT_QPA_DEFAULT_PLATFORM_NAME=\"android\" \
             -DQT_FEATURE_fontconfig=-1 \
             -DQT_FEATURE_dbus=-1
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
DEFINES   := $(DEFINES) \
             -DQT_FEATURE_dbus=-1
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
	@echo [AR] $(LIBDIR)/$(NAME)$(SUFFIX)$(EXTLIB)
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
