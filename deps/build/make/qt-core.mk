
include platforms/detect-host.mk

NAME       = qt-core
INCDIR     = ../../include
INCDIRQT   = $(INCDIR)/qt/QtCore
SRCDIR     = ../../src/$(NAME)
MOCDIR     = $(SRCDIR)/.moc
RCCDIR     = $(SRCDIR)/.rcc
UICDIR     = $(SRCDIR)/.uic
OBJDIR    := $(OBJDIR)/$(NAME)
DEFINES   := $(DEFINES) \
             -DCHROMIUM_ZLIB_NO_CHROMECONF \
             -DQT_STATIC \
             -DQT_USE_QSTRINGBUILDER \
             -DQT_BUILD_CORE_LIB \
             -DHAVE_CONFIG_H \
             -DUNICODE \
             -D_ENABLE_EXTENDED_ALIGNED_STORAGE \
             -DPCRE2_STATIC
INCLUDES  := $(INCLUDES) \
             -I../../src \
             -I$(INCDIR) \
             -I$(INCDIRQT) \
             -I$(INCDIRQT)/private \
             -I$(SRCDIR) \
             -I$(MOCDIR) \
             -I$(UICDIR) \
             -I$(INCDIR)/zlib \
             -I$(INCDIR)/tinycbor \
             -I$(INCDIR)/md4 \
             -I$(INCDIR)/md5 \
             -I$(INCDIR)/pcre2 \
             -I$(INCDIR)/sha3 \
             -I$(INCDIR)/rfc6234 \
             -I$(INCDIR)/harfbuzz \
             -I$(INCDIR)/forkfd \
             -I$(INCDIR)/qt \
             -I$(INCDIR)/qt/testlib \
             -I$(INCDIR)/qt/QtXml \
             -I$(INCDIR)/qt/QtXml/private 
CXXFLAGS  := $(CXXFLAGS) -std=c++17
LINKFLAGS := $(LINKFLAGS)
LINKPATH  := $(LINKPATH)
LINKLIBS  := $(LINKLIBS)
MOCH       = qabstractanimation.cpp \
             private/qabstractanimation_p.cpp \
             qabstracteventdispatcher.cpp \
             qabstractitemmodel.cpp \
             qabstractproxymodel.cpp \
             qabstractstate.cpp \
             qabstracttransition.cpp \
             qanimationgroup.cpp \
             qbuffer.cpp \
             qcborcommon.cpp \
             qcborstream.cpp \
             qcborvalue.cpp \
             qcoreapplication.cpp \
             qcoreevent.cpp \
             qcryptographichash.cpp \
             qeasingcurve.cpp \
             qeventloop.cpp \
             qeventtransition.cpp \
             private/qfactoryloader_p.cpp \
             qfile.cpp \
             qfiledevice.cpp \
             qfileselector.cpp \
             qfilesystemwatcher.cpp \
             private/qfilesystemwatcher_p.cpp \
             private/qfilesystemwatcher_polling_p.cpp \
             qfinalstate.cpp \
             qfuturewatcher.cpp \
             qhistorystate.cpp \
             qidentityproxymodel.cpp \
             qiodevice.cpp \
             qitemselectionmodel.cpp \
             qlibrary.cpp \
             qlocale.cpp \
             qmimedata.cpp \
             qmimetype.cpp \
             qnamespace.cpp \
             private/qnoncontiguousbytedevice_p.cpp \
             qobject.cpp \
             qobjectcleanuphandler.cpp \
             qparallelanimationgroup.cpp \
             qpauseanimation.cpp \
             qpluginloader.cpp \
             qprocess.cpp \
             qpropertyanimation.cpp \
             qsavefile.cpp \
             qsequentialanimationgroup.cpp \
             qsettings.cpp \
             qsharedmemory.cpp \
             qsignalmapper.cpp \
             qsignaltransition.cpp \
             qsocketnotifier.cpp \
             qsortfilterproxymodel.cpp \
             qstandardpaths.cpp \
             qstate.cpp \
             qstatemachine.cpp \
             qstringlistmodel.cpp \
             qtemporaryfile.cpp \
             private/qtextstream_p.cpp \
             qthread.cpp \
             qthreadpool.cpp \
             qtimeline.cpp \
             qtimer.cpp \
             qtranslator.cpp \
             qvariantanimation.cpp
MOCC       = kernel/qtimer.moc \
             statemachine/qhistorystate.moc \
             statemachine/qstatemachine.moc		 
MOCO       = qmimetype.omoc
RCCH       = mimetypes/mimetypes.cpp
RCCO       = mimetypes.orcc
UICH       =
OBJS       = animation/qabstractanimation.o \
             animation/qanimationgroup.o \
             animation/qparallelanimationgroup.o \
             animation/qpauseanimation.o \
             animation/qpropertyanimation.o \
             animation/qsequentialanimationgroup.o \
             animation/qvariantanimation.o \
             codecs/qbig5codec.o \
             codecs/qeucjpcodec.o \
             codecs/qeuckrcodec.o \
             codecs/qgb18030codec.o \
             codecs/qisciicodec.o \
             codecs/qjiscodec.o \
             codecs/qjpunicode.o \
             codecs/qlatincodec.o \
             codecs/qsimplecodec.o \
             codecs/qsjiscodec.o \
             codecs/qtextcodec.o \
             codecs/qtsciicodec.o \
             codecs/qutfcodec.o \
             global/archdetect.o \
             global/qconfig.o \
             global/qendian.o \
             global/qfloat16.o \
             global/qfloat16tables.o \
             global/qglobal.o \
             global/qhooks.o \
             global/qlibraryinfo.o \
             global/qlogging.o \
             global/qmalloc.o \
             global/qnumeric.o \
             global/qoperatingsystemversion.o \
             global/qrandom.o \
             global/qversiontagging.o \
             io/qabstractfileengine.o \
             io/qbuffer.o \
             io/qdataurl.o \
             io/qdebug.o \
             io/qdir.o \
             io/qdiriterator.o \
             io/qfile.o \
             io/qfiledevice.o \
             io/qfileinfo.o \
             io/qfileselector.o \
             io/qfilesystemengine.o \
             io/qfilesystementry.o \
             io/qfilesystemwatcher.o \
             io/qfilesystemwatcher_polling.o \
             io/qfsfileengine.o \
             io/qfsfileengine_iterator.o \
             io/qiodevice.o \
             io/qipaddress.o \
             io/qlockfile.o \
             io/qloggingcategory.o \
             io/qloggingregistry.o \
             io/qnoncontiguousbytedevice.o \
             io/qprocess.o \
             io/qresource.o \
             io/qresource_iterator.o \
             io/qsavefile.o \
             io/qsettings.o \
             io/qstandardpaths.o \
             io/qstorageinfo.o \
             io/qtemporarydir.o \
             io/qtemporaryfile.o \
             io/qtldurl.o \
             io/qurl.o \
             io/qurlidna.o \
             io/qurlquery.o \
             io/qurlrecode.o \
             itemmodels/qabstractitemmodel.o \
             itemmodels/qabstractproxymodel.o \
             itemmodels/qidentityproxymodel.o \
             itemmodels/qitemselectionmodel.o \
             itemmodels/qsortfilterproxymodel.o \
             itemmodels/qstringlistmodel.o \
             kernel/qabstracteventdispatcher.o \
             kernel/qabstractnativeeventfilter.o \
             kernel/qbasictimer.o \
             kernel/qcoreapplication.o \
             kernel/qcoreevent.o \
             kernel/qcoreglobaldata.o \
             kernel/qdeadlinetimer.o \
             kernel/qelapsedtimer.o \
             kernel/qeventloop.o \
             kernel/qmath.o \
             kernel/qmetaobject.o \
             kernel/qmetaobjectbuilder.o \
             kernel/qmetatype.o \
             kernel/qmimedata.o \
             kernel/qobject.o \
             kernel/qobjectcleanuphandler.o \
             kernel/qpointer.o \
             kernel/qppsattribute.o \
             kernel/qsharedmemory.o \
             kernel/qsignalmapper.o \
             kernel/qsocketnotifier.o \
             kernel/qsystemerror.o \
             kernel/qsystemsemaphore.o \
             kernel/qtestsupport_core.o \
             kernel/qtimer.o \
             kernel/qtranslator.o \
             kernel/qvariant.o \
             mimetypes/qmimedatabase.o \
             mimetypes/qmimeglobpattern.o \
             mimetypes/qmimemagicrule.o \
             mimetypes/qmimemagicrulematcher.o \
             mimetypes/qmimeprovider.o \
             mimetypes/qmimetype.o \
             mimetypes/qmimetypeparser.o \
             plugin/qfactoryinterface.o \
             plugin/qfactoryloader.o \
             plugin/qlibrary.o \
             plugin/qpluginloader.o \
             plugin/quuid.o \
             serialization/qcbordiagnostic.o \
             serialization/qcborstream.o \
             serialization/qcborvalue.o \
             serialization/qdatastream.o \
             serialization/qjson.o \
             serialization/qjsonarray.o \
             serialization/qjsoncbor.o \
             serialization/qjsondocument.o \
             serialization/qjsonobject.o \
             serialization/qjsonparser.o \
             serialization/qjsonvalue.o \
             serialization/qjsonwriter.o \
             serialization/qtextstream.o \
             serialization/qxmlstream.o \
             serialization/qxmlutils.o \
             statemachine/qabstractstate.o \
             statemachine/qabstracttransition.o \
             statemachine/qeventtransition.o \
             statemachine/qfinalstate.o \
             statemachine/qhistorystate.o \
             statemachine/qsignaltransition.o \
             statemachine/qstate.o \
             statemachine/qstatemachine.o \
             thread/qatomic.o \
             thread/qexception.o \
             thread/qfutureinterface.o \
             thread/qfuturewatcher.o \
             thread/qmutex.o \
             thread/qmutexpool.o \
             thread/qreadwritelock.o \
             thread/qresultstore.o \
             thread/qrunnable.o \
             thread/qsemaphore.o \
             thread/qthread.o \
             thread/qthreadpool.o \
             thread/qthreadstorage.o \
             tools/qarraydata.o \
             tools/qbitarray.o \
             tools/qbytearray.o \
             tools/qbytearraylist.o \
             tools/qbytearraymatcher.o \
             tools/qcollator.o \
             tools/qcommandlineoption.o \
             tools/qcommandlineparser.o \
             tools/qcontiguouscache.o \
             tools/qcryptographichash.o \
             tools/qdatetime.o \
             tools/qdatetimeparser.o \
             tools/qeasingcurve.o \
             tools/qfreelist.o \
             tools/qharfbuzz.o \
             tools/qhash.o \
             tools/qline.o \
             tools/qlinkedlist.o \
             tools/qlist.o \
             tools/qlocale.o \
             tools/qlocale_tools.o \
             tools/qmap.o \
             tools/qmargins.o \
             tools/qmessageauthenticationcode.o \
             tools/qpoint.o \
             tools/qqueue.o \
             tools/qrect.o \
             tools/qrefcount.o \
             tools/qregexp.o \
             tools/qregularexpression.o \
             tools/qringbuffer.o \
             tools/qscopedpointer.o \
             tools/qscopedvaluerollback.o \
             tools/qsharedpointer.o \
             tools/qsimd.o \
             tools/qsize.o \
             tools/qstack.o \
             tools/qstring.o \
             tools/qstring_compat.o \
             tools/qstringbuilder.o \
             tools/qstringlist.o \
             tools/qtextboundaryfinder.o \
             tools/qtimeline.o \
             tools/qtimezone.o \
             tools/qtimezoneprivate.o \
             tools/qunicodetools.o \
             tools/qvector_msvc.o \
             tools/qversionnumber.o \
             tools/qvsnprintf.o

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
CXXFLAGS  := $(CXXFLAGS) -Wno-microsoft-cast
INCLUDES  := $(INCLUDES) -I$(INCDIR)/qt/mkspecs/win32-clang-msvc
MOCH      := $(MOCH) \
             private/qeventdispatcher_win_p.cpp \
             private/qfilesystemwatcher_win_p.cpp \
             private/qwindowspipereader_p.cpp \
             private/qwindowspipewriter_p.cpp \
             qwineventnotifier.cpp
MOCC      := $(MOCC) \
             io/qfilesystemwatcher_win.moc
MOCO      := $(MOCO) \
             qeventdispatcher_win_p.omoc \
             qfilesystemwatcher_win_p.omoc \
             qwindowspipereader_p.omoc \
             qwindowspipewriter_p.omoc \
             qwineventnotifier.omoc
RCCH      := $(RCCH)
RCCO      := $(RCCO)
UICH      := $(UICH)
OBJS      := $(OBJS) \
             codecs/qwindowscodec.o \
             global/qoperatingsystemversion_win.o \
             io/qfilesystemengine_win.o \
             io/qfilesystemiterator_win.o \
             io/qfilesystemwatcher_win.o \
             io/qfsfileengine_win.o \
             io/qlockfile_win.o \
             io/qprocess_win.o \
             io/qsettings_win.o \
             io/qstandardpaths_win.o \
             io/qstorageinfo_win.o \
             io/qwindowspipereader.o \
             io/qwindowspipewriter.o \
             kernel/qcoreapplication_win.o \
             kernel/qelapsedtimer_win.o \
             kernel/qeventdispatcher_win.o \
             kernel/qsharedmemory_win.o \
             kernel/qsystemsemaphore_win.o \
             kernel/qwineventnotifier.o \
             plugin/qlibrary_win.o \
             plugin/qsystemlibrary.o \
             thread/qthread_win.o \
             thread/qwaitcondition_win.o \
             tools/qcollator_win.o \
             tools/qlocale_win.o \
             tools/qtimezoneprivate_win.o
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
             private/qeventdispatcher_cf_p.cpp \
             private/qeventdispatcher_unix_p.cpp \
             private/qfilesystemwatcher_kqueue_p.cpp \
             private/qfilesystemwatcher_inotify_p.cpp \
             private/qfilesystemwatcher_fsevents_p.cpp   
MOCC      := $(MOCC) \
             kernel/qeventdispatcher_cf.mocmm
MOCO      := $(MOCO) \
             qfilesystemwatcher_fsevents_p.omoc
RCCH      := $(RCCH)
RCCO      := $(RCCO)
UICH      := $(UICH)
OBJS      := $(OBJS) \
             global/qoperatingsystemversion_darwin.omm \
             io/forkfd_qt.o \
             io/qfilesystemengine_unix.o \
             io/qfilesystemiterator_unix.o \
             io/qfilesystemwatcher_fsevents.omm \
             io/qfsfileengine_unix.o \
             io/qlockfile_unix.o \
             io/qprocess_unix.o \
             io/qprocess_darwin.omm \
             io/qsettings_mac.o \
             io/qstandardpaths_mac.omm \
             io/qstorageinfo_mac.o \
             kernel/qcfsocketnotifier.o \
             kernel/qcore_foundation.omm \
             kernel/qcore_mac.o \
             kernel/qcore_mac_objc.omm \
             kernel/qcore_unix.o \
             kernel/qcoreapplication_mac.o \
             kernel/qelapsedtimer_mac.o \
             kernel/qeventdispatcher_cf.omm \
             kernel/qeventdispatcher_unix.o \
             kernel/qpoll.o \
             kernel/qsharedmemory_systemv.o \
             kernel/qsharedmemory_unix.o \
             kernel/qsystemsemaphore_systemv.o \
             kernel/qsystemsemaphore_unix.o \
             kernel/qtimerinfo_unix.o \
             plugin/qelfparser_p.o \
             plugin/qlibrary_unix.o \
             plugin/qmachparser.o \
             thread/qmutex_mac.o \
             thread/qthread_unix.o \
             thread/qwaitcondition_unix.o \
             tools/qcollator_macx.o \
             tools/qlocale_mac.omm \
             tools/qtimezoneprivate_mac.omm
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
             -DQT_UNIX_SEMAPHORE \
             -DQT_FEATURE_dbus=1
CXXFLAGS  := $(CXXFLAGS)
INCLUDES  := $(INCLUDES) -I$(INCDIR)/qt/mkspecs/linux-clang
MOCH      := $(MOCH) \
             private/qeventdispatcher_unix_p.cpp \
             private/qfilesystemwatcher_kqueue_p.cpp \
             private/qfilesystemwatcher_inotify_p.cpp
MOCC      := $(MOCC)
MOCO      := $(MOCO)
RCCH      := $(RCCH)
RCCO      := $(RCCO)
UICH      := $(UICH)
OBJS      := $(OBJS) \
             io/forkfd_qt.o \
             io/qfilesystemengine_unix.o \
             io/qfilesystemiterator_unix.o \
             io/qfilesystemwatcher_inotify.o \
             io/qfsfileengine_unix.o \
             io/qlockfile_unix.o \
             io/qprocess_unix.o \
             io/qstandardpaths_unix.o \
             io/qstorageinfo_unix.o \
             kernel/qcore_unix.o \
             kernel/qelapsedtimer_unix.o \
             kernel/qeventdispatcher_unix.o \
             kernel/qpoll.o \
             kernel/qsharedmemory_systemv.o \
             kernel/qsharedmemory_unix.o \
             kernel/qsystemsemaphore_systemv.o \
             kernel/qsystemsemaphore_unix.o \
             kernel/qtimerinfo_unix.o \
             plugin/qelfparser_p.o \
             plugin/qlibrary_unix.o \
             thread/qthread_unix.o \
             thread/qwaitcondition_unix.o \
             tools/qcollator_posix.o \
             tools/qlocale_unix.o \
             tools/qtimezoneprivate_tz.o
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
MOCH      := $(MOCH) \
             private/qeventdispatcher_unix_p.cpp \
             private/qfilesystemwatcher_kqueue_p.cpp \
             private/qfilesystemwatcher_inotify_p.cpp
MOCC      := $(MOCC)
MOCO      := $(MOCO)
RCCH      := $(RCCH)
RCCO      := $(RCCO)
UICH      := $(UICH)
OBJS      := $(OBJS) \
             io/forkfd_qt.o \
             io/qfilesystemengine_unix.o \
             io/qfilesystemiterator_unix.o \
             io/qfilesystemwatcher_inotify.o \
             io/qfsfileengine_unix.o \
             io/qlockfile_unix.o \
             io/qprocess_unix.o \
             io/qstandardpaths_unix.o \
             io/qstorageinfo_unix.o \
             kernel/qcore_unix.o \
             kernel/qelapsedtimer_unix.o \
             kernel/qeventdispatcher_unix.o \
             kernel/qpoll.o \
             kernel/qsharedmemory_android.o \
             kernel/qsharedmemory_unix.o \
             kernel/qsystemsemaphore_android.o \
             kernel/qsystemsemaphore_unix.o \
             kernel/qtimerinfo_unix.o \
             plugin/qelfparser_p.o \
             plugin/qlibrary_unix.o \
             thread/qthread_unix.o \
             thread/qwaitcondition_unix.o \
             tools/qcollator_posix.o \
             tools/qlocale_unix.o \
             tools/qtimezoneprivate_android.o
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
