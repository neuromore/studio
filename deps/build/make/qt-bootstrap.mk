
include platforms/detect-host.mk

NAME       = qt-bootstrap
INCDIR     = ../../include
SRCDIR     = ../../src
OBJDIR    := $(OBJDIR)/$(NAME)
DEFINES   := $(DEFINES) \
             -DCHROMIUM_ZLIB_NO_CHROMECONF \
             -DQT_VERSION_MAJOR=5 \
             -DQT_VERSION_MINOR=12 \
             -DQT_VERSION_PATCH=4 \
             -DQT_VERSION_STR="\"5.12.4\"" \
             -DQT_BOOTSTRAPPED \
             -DQT_BUILD_BOOTSTRAP_LIB \
             -DQT_BUILDING_QT \
             -DQT_MOC_COMPAT \
             -DQT_STATIC \
             -DQT_USE_QSTRINGBUILDER \
             -DQT_NO_EXCEPTIONS \
             -DQT_NO_FOREACH \
             -DQT_NO_CAST_TO_ASCII \
             -DHAVE_CONFIG_H \
             -DUNICODE \
             -D_ENABLE_EXTENDED_ALIGNED_STORAGE \
             -Wno-deprecated-declarations
INCLUDES  := $(INCLUDES) \
             -I$(INCDIR) \
             -I$(SRCDIR) \
             -I$(INCDIR)/zlib \
             -I$(INCDIR)/qt \
             -I$(INCDIR)/qt/QtCore \
             -I$(INCDIR)/qt/QtCore/private \
             -I$(INCDIR)/qt/QtXml \
             -I$(INCDIR)/qt/QtXml/private 
CXXFLAGS  := $(CXXFLAGS) -std=c++17
LINKFLAGS := $(LINKFLAGS)
LINKPATH  := $(LINKPATH)
LINKLIBS  := $(LINKLIBS)
OBJS       = qt-core/codecs/qlatincodec.o \
             qt-core/codecs/qtextcodec.o \
             qt-core/codecs/qutfcodec.o \
             qt-core/global/qendian.o \
             qt-core/global/qglobal.o \
             qt-core/global/qlogging.o \
             qt-core/global/qmalloc.o \
             qt-core/global/qnumeric.o \
             qt-core/global/qoperatingsystemversion.o \
             qt-core/global/qrandom.o \
             qt-core/io/qabstractfileengine.o \
             qt-core/io/qbuffer.o \
             qt-core/io/qdebug.o \
             qt-core/io/qdir.o \
             qt-core/io/qdiriterator.o \
             qt-core/io/qfile.o \
             qt-core/io/qfiledevice.o \
             qt-core/io/qfileinfo.o \
             qt-core/io/qfilesystemengine.o \
             qt-core/io/qfilesystementry.o \
             qt-core/io/qfsfileengine.o \
             qt-core/io/qfsfileengine_iterator.o \
             qt-core/io/qiodevice.o \
             qt-core/io/qloggingcategory.o \
             qt-core/io/qloggingregistry.o \
             qt-core/io/qresource.o \
             qt-core/io/qsavefile.o \
             qt-core/io/qstandardpaths.o \
             qt-core/io/qtemporarydir.o \
             qt-core/io/qtemporaryfile.o \
             qt-core/kernel/qcoreapplication.o \
             qt-core/kernel/qcoreglobaldata.o \
             qt-core/kernel/qmetatype.o \
             qt-core/kernel/qsystemerror.o \
             qt-core/kernel/qvariant.o \
             qt-core/plugin/quuid.o \
             qt-core/serialization/qdatastream.o \
             qt-core/serialization/qjson.o \
             qt-core/serialization/qjsonarray.o \
             qt-core/serialization/qjsondocument.o \
             qt-core/serialization/qjsonobject.o \
             qt-core/serialization/qjsonparser.o \
             qt-core/serialization/qjsonvalue.o \
             qt-core/serialization/qjsonwriter.o \
             qt-core/serialization/qtextstream.o \
             qt-core/serialization/qxmlstream.o \
             qt-core/serialization/qxmlutils.o \
             qt-core/tools/qarraydata.o \
             qt-core/tools/qbitarray.o \
             qt-core/tools/qbytearray.o \
             qt-core/tools/qbytearraymatcher.o \
             qt-core/tools/qcommandlineoption.o \
             qt-core/tools/qcommandlineparser.o \
             qt-core/tools/qcryptographichash.o \
             qt-core/tools/qdatetime.o \
             qt-core/tools/qhash.o \
             qt-core/tools/qline.o \
             qt-core/tools/qlinkedlist.o \
             qt-core/tools/qlist.o \
             qt-core/tools/qlocale.o \
             qt-core/tools/qlocale_tools.o \
             qt-core/tools/qmap.o \
             qt-core/tools/qpoint.o \
             qt-core/tools/qrect.o \
             qt-core/tools/qregexp.o \
             qt-core/tools/qringbuffer.o \
             qt-core/tools/qsize.o \
             qt-core/tools/qstring.o \
             qt-core/tools/qstring_compat.o \
             qt-core/tools/qstringbuilder.o \
             qt-core/tools/qstringlist.o \
             qt-core/tools/qversionnumber.o \
             qt-core/tools/qvsnprintf.o \
             qt-xml/dom/qdom.o \
             qt-xml/sax/qxml.o

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

# WINDOWS
ifeq ($(TARGET_OS),win)
DEFINES   := $(DEFINES) -D_CRT_SECURE_NO_WARNINGS
CXXFLAGS  := $(CXXFLAGS)
INCLUDES  := $(INCLUDES) -I$(INCDIR)/qt/mkspecs/win32-clang-msvc
OBJS      := $(OBJS) \
             qt-core/global/qoperatingsystemversion_win.o \
             qt-core/io/qfilesystemengine_win.o \
             qt-core/io/qfilesystemiterator_win.o \
             qt-core/io/qfsfileengine_win.o \
             qt-core/io/qstandardpaths_win.o \
             qt-core/kernel/qcoreapplication_win.o \
             qt-core/plugin/qsystemlibrary.o
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

# OSX
ifeq ($(TARGET_OS),osx)
DEFINES   := $(DEFINES)
CXXFLAGS  := $(CXXFLAGS)
INCLUDES  := $(INCLUDES) -I$(INCDIR)/qt/mkspecs/macx-clang
OBJS      := $(OBJS) \
             qt-core/global/qoperatingsystemversion_darwin.omm \
             qt-core/io/qfilesystemengine_unix.o \
             qt-core/io/qfilesystemiterator_unix.o \
             qt-core/io/qfsfileengine_unix.o \
             qt-core/io/qipaddress.o \
             qt-core/io/qstandardpaths_mac.omm \
             qt-core/io/qurl.o \
             qt-core/io/qurlquery.o \
             qt-core/io/qurlrecode.o \
             qt-core/io/qurlidna.o \
             qt-core/kernel/qcore_unix.o \
             qt-core/kernel/qcore_foundation.omm \
             qt-core/kernel/qcore_mac_objc.omm \
             qt-core/kernel/qcore_mac.o \
             qt-core/kernel/qcoreapplication_mac.o
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

# LINUX
ifeq ($(TARGET_OS),linux)
DEFINES   := $(DEFINES)
CXXFLAGS  := $(CXXFLAGS)
INCLUDES  := $(INCLUDES) -I$(INCDIR)/qt/mkspecs/linux-clang
OBJS      := $(OBJS) \
             qt-core/io/qfilesystemengine_unix.o \
             qt-core/io/qfilesystemiterator_unix.o \
             qt-core/io/qfsfileengine_unix.o \
             qt-core/io/qstandardpaths_unix.o \
             qt-core/kernel/qcore_unix.o
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

# ANDROID
ifeq ($(TARGET_OS),android)
DEFINES   := $(DEFINES)
CXXFLAGS  := $(CXXFLAGS)
INCLUDES  := $(INCLUDES) -I$(INCDIR)/qt/mkspecs/android-clang
OBJS      := $(OBJS) \
             qt-core/io/qfilesystemengine_unix.o \
             qt-core/io/qfilesystemiterator_unix.o \
             qt-core/io/qfsfileengine_unix.o \
             qt-core/io/qstandardpaths_unix.o \
             qt-core/kernel/qcore_unix.o
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

# iOS
ifeq ($(TARGET_OS),ios)
DEFINES   := $(DEFINES)
CXXFLAGS  := $(CXXFLAGS)
INCLUDES  := $(INCLUDES) -I$(INCDIR)/qt/mkspecs/macx-ios-clang
OBJS      := $(OBJS) \
             qt-core/global/qoperatingsystemversion_darwin.omm \
             qt-core/io/qfilesystemengine_unix.o \
             qt-core/io/qfilesystemiterator_unix.o \
             qt-core/io/qfsfileengine_unix.o \
             qt-core/io/qipaddress.o \
             qt-core/io/qstandardpaths_mac.omm \
             qt-core/io/qurl.o \
             qt-core/io/qurlquery.o \
             qt-core/io/qurlrecode.o \
             qt-core/io/qurlidna.o \
             qt-core/kernel/qcore_unix.o \
             qt-core/kernel/qcore_foundation.omm \
             qt-core/kernel/qcore_mac_objc.omm \
             qt-core/kernel/qcore_mac.o \
             qt-core/kernel/qcoreapplication_mac.o

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

ifeq ($(MODE),debug)
DEFINES   := $(DEFINES) -DQT_DEBUG
else
DEFINES   := $(DEFINES) -DQT_NO_DEBUG
endif

OBJS := $(patsubst %,$(OBJDIR)/%,$(OBJS))

$(OBJDIR)/%.o:
	@echo [CXX] $@
	$(CXX) $(CPUFLAGS) $(DEFINES) $(INCLUDES) $(CXXFLAGS) -c $(@:$(OBJDIR)%.o=$(SRCDIR)%.cpp) -o $@

$(OBJDIR)/%.omm:
	@echo [CXX] $@
	$(CXX) $(CPUFLAGS) $(DEFINES) $(INCLUDES) $(CXXFLAGS) -c $(@:$(OBJDIR)%.omm=$(SRCDIR)%.mm) -o $@

.DEFAULT_GOAL := build

build: $(OBJS)
	@echo [AR]  $(LIBDIR)/$(NAME)$(SUFFIX)$(EXTLIB)
	$(AR) $(ARFLAGS) $(LIBDIR)/$(NAME)$(SUFFIX)$(EXTLIB) $(OBJS)

clean:
	$(call deletefiles,$(OBJDIR),*.o)
	$(call deletefiles,$(OBJDIR),*.omm)
	$(call deletefiles,$(LIBDIR),$(NAME)$(SUFFIX)$(EXTLIB))
