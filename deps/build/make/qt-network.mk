
include platforms/detect-host.mk

NAME       = qt-network
INCDIR     = ../../include
INCDIRQT   = $(INCDIR)/qt/QtNetwork
SRCDIR     = ../../src/$(NAME)
MOCDIR     = $(SRCDIR)/.moc
RCCDIR     = $(SRCDIR)/.rcc
UICDIR     = $(SRCDIR)/.uic
OBJDIR    := $(OBJDIR)/$(NAME)
DEFINES   := $(DEFINES) \
             -DCHROMIUM_ZLIB_NO_CHROMECONF \
             -DOPENSSL_STATIC \
             -DQT_STATIC \
             -DQT_USE_QSTRINGBUILDER \
             -DQT_LINKED_OPENSSL \
             -DHAVE_CONFIG_H \
             -DUNICODE \
             -D_ENABLE_EXTENDED_ALIGNED_STORAGE
INCLUDES  := $(INCLUDES) \
             -I../../src \
             -I$(INCDIR) \
             -I$(INCDIR)/zlib \
             -I$(INCDIRQT) \
             -I$(INCDIRQT)/private \
             -I$(SRCDIR) \
             -I$(UICDIR) \
             -I$(MOCDIR) \
             -I$(INCDIR)/qt \
             -I$(INCDIR)/qt/QtCore
CXXFLAGS  := $(CXXFLAGS) -std=c++17
CFLAGS    := $(CFLAGS)
LINKFLAGS := $(LINKFLAGS)
LINKPATH  := $(LINKPATH)
LINKLIBS  := $(LINKLIBS)
MOCH       = qabstractnetworkcache.cpp \
             qabstractsocket.cpp \
             private/qabstractsocketengine_p.cpp \
             private/qbearerengine_p.cpp \
             private/qbearerplugin_p.cpp \
             qdnslookup.cpp \
             private/qdnslookup_p.cpp \
             qdtls.cpp \
             private/qftp_p.cpp \
             private/qhostinfo_p.cpp \
             private/qhttp2protocolhandler_p.cpp \
             qhttpmultipart.cpp \
             private/qhttpnetworkconnection_p.cpp \
             private/qhttpnetworkconnectionchannel_p.cpp \
             private/qhttpnetworkreply_p.cpp \
             private/qhttpsocketengine_p.cpp \
             private/qhttpthreaddelegate_p.cpp \
             qlocalserver.cpp \
             qlocalsocket.cpp \
             private/qnativesocketengine_p.cpp \
             private/qnetworkaccessbackend_p.cpp \
             private/qnetworkaccesscache_p.cpp \
             private/qnetworkaccessdebugpipebackend_p.cpp \
             private/qnetworkaccessfilebackend_p.cpp \
             private/qnetworkaccessftpbackend_p.cpp \
             qnetworkaccessmanager.cpp \
             qnetworkconfigmanager.cpp \
             private/qnetworkconfigmanager_p.cpp \
             qnetworkcookiejar.cpp \
             qnetworkdiskcache.cpp \
             private/qnetworkfile_p.cpp \
             qnetworkinterface.cpp \
             qnetworkproxy.cpp \
             qnetworkreply.cpp \
             private/qnetworkreplydataimpl_p.cpp \
             private/qnetworkreplyfileimpl_p.cpp \
             private/qnetworkreplyhttpimpl_p.cpp \
             private/qnetworkreplyimpl_p.cpp \
             qnetworksession.cpp \
             private/qnetworksession_p.cpp \
             private/qsocks5socketengine_p.cpp \
             private/qspdyprotocolhandler_p.cpp \
             qsslsocket.cpp \
             qtcpserver.cpp \
             qtcpsocket.cpp \
             qudpsocket.cpp
MOCC       = access/qftp.moc
MOCO       = qabstractnetworkcache.omoc \
             qabstractsocketengine_p.omoc \
             qbearerplugin_p.omoc \
             qdnslookup_p.omoc \
             qdtls.omoc \
             qhostinfo_p.omoc \
             qhttp2protocolhandler_p.omoc \
             qhttpmultipart.omoc \
             qhttpnetworkreply_p.omoc \
             qhttpsocketengine_p.omoc \
             qhttpthreaddelegate_p.omoc \
             qnativesocketengine_p.omoc \
             qnetworkaccessbackend_p.omoc \
             qnetworkaccesscache_p.omoc \
             qnetworkaccessdebugpipebackend_p.omoc \
             qnetworkaccessfilebackend_p.omoc \
             qnetworkaccessftpbackend_p.omoc \
             qnetworkconfigmanager_p.omoc \
             qnetworkcookiejar.omoc \
             qnetworkdiskcache.omoc \
             qnetworkfile_p.omoc \
             qnetworkproxy.omoc \
             qnetworkreply.omoc \
             qnetworkreplyhttpimpl_p.omoc \
             qnetworksession_p.omoc \
             qsocks5socketengine_p.omoc \
             qspdyprotocolhandler_p.omoc \
             qtcpsocket.omoc \
             qudpsocket.omoc
RCCH       =
RCCO       =
UICH       =
OBJS       = access/http2/bitstreams.o \
             access/http2/hpack.o \
             access/http2/hpacktable.o \
             access/http2/http2frames.o \
             access/http2/http2protocol.o \
             access/http2/http2streams.o \
             access/http2/huffman.o \
             access/qabstractnetworkcache.o \
             access/qabstractprotocolhandler.o \
             access/qftp.o \
             access/qhsts.o \
             access/qhstspolicy.o \
             access/qhstsstore.o \
             access/qhttp2protocolhandler.o \
             access/qhttpmultipart.o \
             access/qhttpnetworkconnection.o \
             access/qhttpnetworkconnectionchannel.o \
             access/qhttpnetworkheader.o \
             access/qhttpnetworkreply.o \
             access/qhttpnetworkrequest.o \
             access/qhttpprotocolhandler.o \
             access/qhttpthreaddelegate.o \
             access/qnetworkaccessauthenticationmanager.o \
             access/qnetworkaccessbackend.o \
             access/qnetworkaccesscache.o \
             access/qnetworkaccesscachebackend.o \
             access/qnetworkaccessfilebackend.o \
             access/qnetworkaccessftpbackend.o \
             access/qnetworkaccessmanager.o \
             access/qnetworkcookie.o \
             access/qnetworkcookiejar.o \
             access/qnetworkdiskcache.o \
             access/qnetworkfile.o \
             access/qnetworkreply.o \
             access/qnetworkreplydataimpl.o \
             access/qnetworkreplyfileimpl.o \
             access/qnetworkreplyhttpimpl.o \
             access/qnetworkreplyimpl.o \
             access/qnetworkrequest.o \
             access/qspdyprotocolhandler.o \
             bearer/qbearerengine.o \
             bearer/qbearerplugin.o \
             bearer/qnetworkconfigmanager.o \
             bearer/qnetworkconfigmanager_p.o \
             bearer/qnetworkconfiguration.o \
             bearer/qnetworksession.o \
             bearer/qsharednetworksession.o \
             kernel/qauthenticator.o \
             kernel/qdnslookup.o \
             kernel/qhostaddress.o \
             kernel/qhostinfo.o \
             kernel/qnetworkdatagram.o \
             kernel/qnetworkinterface.o \
             kernel/qnetworkproxy.o \
             kernel/qurlinfo.o \
             socket/qabstractsocket.o \
             socket/qabstractsocketengine.o \
             socket/qhttpsocketengine.o \
             socket/qlocalserver.o \
             socket/qlocalsocket.o \
             socket/qnativesocketengine.o \
             socket/qsocks5socketengine.o \
             socket/qtcpserver.o \
             socket/qtcpsocket.o \
             socket/qudpsocket.o \
             ssl/qasn1element.o \
             ssl/qdtls.o \
             ssl/qdtls_openssl.o \
             ssl/qpassworddigestor.o \
             ssl/qssl.o \
             ssl/qsslcertificate.o \
             ssl/qsslcertificate_openssl.o \
             ssl/qsslcertificateextension.o \
             ssl/qsslcipher.o \
             ssl/qsslconfiguration.o \
             ssl/qsslcontext_openssl.o \
             ssl/qsslcontext_openssl11.o \
             ssl/qssldiffiehellmanparameters.o \
             ssl/qssldiffiehellmanparameters_openssl.o \
             ssl/qsslellipticcurve.o \
             ssl/qsslellipticcurve_openssl.o \
             ssl/qsslerror.o \
             ssl/qsslkey_openssl.o \
             ssl/qsslkey_p.o \
             ssl/qsslpresharedkeyauthenticator.o \
             ssl/qsslsocket.o \
             ssl/qsslsocket_openssl.o \
             ssl/qsslsocket_openssl_symbols.o \
             ssl/qsslsocket_openssl11.o

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
             -DOPENSSLDIR=\".\" \
             -DENGINESDIR=\".\"
CXXFLAGS  := $(CXXFLAGS)
CFLAGS    := $(CFLAGS)
INCLUDES  := $(INCLUDES) -I$(INCDIR)/qt/mkspecs/win32-clang-msvc
MOCH      := $(MOCH) \
             private/qwindowscarootfetcher_p.cpp
MOCC      := $(MOCC)
MOCO      := $(MOCO) \
             qwindowscarootfetcher_p.omoc
RCCH      := $(RCCH)
RCCO      := $(RCCO)
UICH      := $(UICH)
OBJS      := $(OBJS) \
             kernel/qdnslookup_win.o \
             kernel/qhostinfo_win.o \
             kernel/qnetworkinterface_win.o \
             kernel/qnetworkproxy_win.o \
             socket/qlocalserver_win.o \
             socket/qlocalsocket_win.o \
             socket/qnativesocketengine_win.o \
             ssl/qwindowscarootfetcher.o
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
             -DOPENSSLDIR=\"/etc/ssl\" \
             -DENGINESDIR=\".\"
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
             kernel/qdnslookup_unix.o \
             kernel/qhostinfo_unix.o \
             kernel/qnetworkinterface_unix.o \
             kernel/qnetworkproxy_generic.o \
             socket/qlocalserver_unix.o \
             socket/qlocalsocket_unix.o \
             socket/qnativesocketengine_unix.o \
             ssl/qsslsocket_mac_shared.o
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
             -DOPENSSLDIR=\"/etc/ssl\" \
             -DENGINESDIR=\".\"
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
             kernel/qdnslookup_unix.o \
             kernel/qhostinfo_unix.o \
             kernel/qnetworkinterface_unix.o \
             kernel/qnetworkproxy_generic.o \
             socket/qlocalserver_unix.o \
             socket/qlocalsocket_unix.o \
             socket/qnativesocketengine_unix.o
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
