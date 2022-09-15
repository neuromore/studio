
include platforms/detect-host.mk

NAME       = qt-multimedia
INCDIR     = ../../include
INCDIRQT   = $(INCDIR)/qt/QtMultimedia
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
             -I$(INCDIR)/qt/QtGui \
             -I$(INCDIR)/qt/QtMultimediaWidgets \
             -I$(INCDIR)/qt/QtMultimediaWidgets/private
CXXFLAGS  := $(CXXFLAGS) -std=c++17
CFLAGS    := $(CFLAGS)
LINKFLAGS := $(LINKFLAGS)
LINKPATH  := $(LINKPATH)
LINKLIBS  := $(LINKLIBS)
MOCH       = qabstractvideofilter.cpp \
             qabstractvideosurface.cpp \
             qaudiodecoder.cpp \
             qaudiodecodercontrol.cpp \
             qaudioencodersettingscontrol.cpp \
             qaudioinput.cpp \
             qaudioinputselectorcontrol.cpp \
             qaudiooutput.cpp \
             qaudiooutputselectorcontrol.cpp \
             qaudioprobe.cpp \
             qaudiorecorder.cpp \
             qaudiorolecontrol.cpp \
             qaudiosystem.cpp \
             qaudiosystemplugin.cpp \
             qcamera.cpp \
             qcameracapturebufferformatcontrol.cpp \
             qcameracapturedestinationcontrol.cpp \
             qcameracontrol.cpp \
             qcameraexposure.cpp \
             qcameraexposurecontrol.cpp \
             qcamerafeedbackcontrol.cpp \
             qcameraflashcontrol.cpp \
             qcamerafocus.cpp \
             qcamerafocuscontrol.cpp \
             qcameraimagecapture.cpp \
             qcameraimagecapturecontrol.cpp \
             qcameraimageprocessing.cpp \
             qcameraimageprocessingcontrol.cpp \
             qcamerainfocontrol.cpp \
             qcameralockscontrol.cpp \
             qcameraviewfindersettingscontrol.cpp \
             qcamerazoomcontrol.cpp \
             qcustomaudiorolecontrol.cpp \
             qimageencodercontrol.cpp \
             qmediaaudioprobecontrol.cpp \
             qmediaavailabilitycontrol.cpp \
             qmediacontainercontrol.cpp \
             qmediacontrol.cpp \
             qmediagaplessplaybackcontrol.cpp \
             qmedianetworkaccesscontrol.cpp \
             private/qmedianetworkplaylistprovider_p.cpp \
             qmediaobject.cpp \
             qmediaplayer.cpp \
             qmediaplayercontrol.cpp \
             qmediaplaylist.cpp \
             private/qmediaplaylist_p.cpp \
             private/qmediaplaylistcontrol_p.cpp \
             private/qmediaplaylistioplugin_p.cpp \
             private/qmediaplaylistnavigator_p.cpp \
             private/qmediaplaylistprovider_p.cpp \
             private/qmediaplaylistsourcecontrol_p.cpp \
             qmediarecorder.cpp \
             qmediarecordercontrol.cpp \
             private/qmediaresourcepolicyplugin_p.cpp \
             private/qmediaresourceset_p.cpp \
             qmediaservice.cpp \
             private/qmediaserviceprovider_p.cpp \
             qmediaserviceproviderplugin.cpp \
             qmediastreamscontrol.cpp \
             qmediavideoprobecontrol.cpp \
             qmetadatareadercontrol.cpp \
             qmetadatawritercontrol.cpp \
             private/qplaylistfileparser_p.cpp \
             qradiodata.cpp \
             qradiodatacontrol.cpp \
             qradiotuner.cpp \
             qradiotunercontrol.cpp \
             private/qsamplecache_p.cpp \
             qsound.cpp \
             qsoundeffect.cpp \
             private/qsoundeffect_qaudio_p.cpp \
             qvideodeviceselectorcontrol.cpp \
             qvideoencodersettingscontrol.cpp \
             private/qvideooutputorientationhandler_p.cpp \
             qvideoprobe.cpp \
             qvideorenderercontrol.cpp \
             private/qvideosurfaceoutput_p.cpp \
             qvideowindowcontrol.cpp \
             private/qwavedecoder_p.cpp
MOCC       =
MOCO       = qabstractvideofilter.omoc \
             qaudioprobe.omoc \
             qcamerafeedbackcontrol.omoc \
             qmediagaplessplaybackcontrol.omoc \
             qmediaresourcepolicyplugin_p.omoc \
             qmediaresourceset_p.omoc \
             qplaylistfileparser_p.omoc \
             qvideooutputorientationhandler_p.omoc \
             qvideoprobe.omoc \
             qvideosurfaceoutput_p.omoc
RCCH       =
RCCO       =
UICH       =
OBJS       = audio/qaudio.o \
             audio/qaudiobuffer.o \
             audio/qaudiodecoder.o \
             audio/qaudiodevicefactory.o \
             audio/qaudiodeviceinfo.o \
             audio/qaudioformat.o \
             audio/qaudiohelpers.o \
             audio/qaudioinput.o \
             audio/qaudiooutput.o \
             audio/qaudioprobe.o \
             audio/qaudiosystem.o \
             audio/qaudiosystemplugin.o \
             audio/qsamplecache_p.o \
             audio/qsound.o \
             audio/qsoundeffect.o \
             audio/qsoundeffect_qaudio_p.o \
             audio/qwavedecoder_p.o \
             camera/qcamera.o \
             camera/qcameraexposure.o \
             camera/qcamerafocus.o \
             camera/qcameraimagecapture.o \
             camera/qcameraimageprocessing.o \
             camera/qcamerainfo.o \
             camera/qcameraviewfindersettings.o \
             controls/qaudiodecodercontrol.o \
             controls/qaudioencodersettingscontrol.o \
             controls/qaudioinputselectorcontrol.o \
             controls/qaudiooutputselectorcontrol.o \
             controls/qaudiorolecontrol.o \
             controls/qcameracapturebufferformatcontrol.o \
             controls/qcameracapturedestinationcontrol.o \
             controls/qcameracontrol.o \
             controls/qcameraexposurecontrol.o \
             controls/qcamerafeedbackcontrol.o \
             controls/qcameraflashcontrol.o \
             controls/qcamerafocuscontrol.o \
             controls/qcameraimagecapturecontrol.o \
             controls/qcameraimageprocessingcontrol.o \
             controls/qcamerainfocontrol.o \
             controls/qcameralockscontrol.o \
             controls/qcameraviewfindersettingscontrol.o \
             controls/qcamerazoomcontrol.o \
             controls/qcustomaudiorolecontrol.o \
             controls/qimageencodercontrol.o \
             controls/qmediaaudioprobecontrol.o \
             controls/qmediaavailabilitycontrol.o \
             controls/qmediacontainercontrol.o \
             controls/qmediagaplessplaybackcontrol.o \
             controls/qmedianetworkaccesscontrol.o \
             controls/qmediaplayercontrol.o \
             controls/qmediaplaylistcontrol.o \
             controls/qmediaplaylistsourcecontrol.o \
             controls/qmediarecordercontrol.o \
             controls/qmediastreamscontrol.o \
             controls/qmediavideoprobecontrol.o \
             controls/qmetadatareadercontrol.o \
             controls/qmetadatawritercontrol.o \
             controls/qradiodatacontrol.o \
             controls/qradiotunercontrol.o \
             controls/qvideodeviceselectorcontrol.o \
             controls/qvideoencodersettingscontrol.o \
             controls/qvideorenderercontrol.o \
             controls/qvideowindowcontrol.o \
             playback/qmediacontent.o \
             playback/qmedianetworkplaylistprovider.o \
             playback/qmediaplayer.o \
             playback/qmediaplaylist.o \
             playback/qmediaplaylistioplugin.o \
             playback/qmediaplaylistnavigator.o \
             playback/qmediaplaylistprovider.o \
             playback/qmediaresource.o \
             playback/qplaylistfileparser.o \
             radio/qradiodata.o \
             radio/qradiotuner.o \
             recording/qaudiorecorder.o \
             recording/qmediaencodersettings.o \
             recording/qmediarecorder.o \
             video/qabstractvideobuffer.o \
             video/qabstractvideofilter.o \
             video/qabstractvideosurface.o \
             video/qimagevideobuffer.o \
             video/qmemoryvideobuffer.o \
             video/qvideoframe.o \
             video/qvideoframeconversionhelper.o \
             video/qvideoframeconversionhelper_avx2.o \
             video/qvideoframeconversionhelper_sse2.o \
             video/qvideoframeconversionhelper_ssse3.o \
             video/qvideooutputorientationhandler.o \
             video/qvideoprobe.o \
             video/qvideosurfaceformat.o \
             video/qvideosurfaceoutput.o \
             qmediabindableinterface.o \
             qmediacontrol.o \
             qmediametadata.o \
             qmediaobject.o \
             qmediapluginloader.o \
             qmediaresourcepolicy_p.o \
             qmediaresourceset_p.o \
             qmediaservice.o \
             qmediaserviceprovider.o \
             qmediastoragelocation.o \
             qmediatimerange.o \
             qmultimedia.o \
             qmultimediautils.o

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
INCLUDES  := $(INCLUDES) \
             -I$(INCDIR)/qt/mkspecs/linux-clang \
             -I/usr/include/gstreamer-1.0 \
             -I/usr/include/glib-2.0 
MOCH      := $(MOCH) \
             private/qgstappsrc_p.cpp \
             private/qgstreameraudioinputselector_p.cpp \
             private/qgstreameraudioprobecontrol_p.cpp \
             private/qgstreamerbushelper_p.cpp \
             private/qgstreamermirtexturerenderer_p.cpp \
             private/qgstreamerplayercontrol_p.cpp \
             private/qgstreamerplayersession_p.cpp \
             private/qgstreamervideoinputdevicecontrol_p.cpp \
             private/qgstreamervideooverlay_p.cpp \
             private/qgstreamervideoprobecontrol_p.cpp \
             private/qgstreamervideorenderer_p.cpp \
             private/qgstreamervideowidget_p.cpp \
             private/qgstreamervideowindow_p.cpp \
             private/qgstvideorendererplugin_p.cpp \
             private/qgstvideorenderersink_p.cpp \
             private/qgstbufferpoolinterface_p.cpp \
             private/qgstvideorendererplugin_p.cpp
MOCC      := $(MOCC) \
             gsttools/qgstreamerbushelper.moc
MOCO      := $(MOCO) \
             qgstappsrc_p.omoc \
             qgstreameraudioinputselector_p.omoc \
             qgstreameraudioprobecontrol_p.omoc \
             qgstreamerbushelper_p.omoc \
             qgstreamermirtexturerenderer_p.omoc \
             qgstreamerplayercontrol_p.omoc \
             qgstreamerplayersession_p.omoc \
             qgstreamervideoinputdevicecontrol_p.omoc \
             qgstreamervideooverlay_p.omoc \
             qgstreamervideoprobecontrol_p.omoc \
             qgstreamervideorenderer_p.omoc \
             qgstreamervideowidget_p.omoc \
             qgstreamervideowindow_p.omoc \
             qgstvideorendererplugin_p.omoc \
             qgstvideorenderersink_p.omoc
RCCH      := $(RCCH)
RCCO      := $(RCCO)
UICH      := $(UICH)
OBJS      := $(OBJS) \
             gsttools/qgstappsrc.o \
             gsttools/qgstbufferpoolinterface.o \
             gsttools/qgstcodecsinfo.o \
             gsttools/qgstreameraudioinputselector.o \
             gsttools/qgstreameraudioprobecontrol.o \
             gsttools/qgstreamerbufferprobe.o \
             gsttools/qgstreamerbushelper.o \
             gsttools/qgstreamermessage.o \
             gsttools/qgstreamerplayercontrol.o \
             gsttools/qgstreamerplayersession.o \
             gsttools/qgstreamervideoinputdevicecontrol.o \
             gsttools/qgstreamervideooverlay.o \
             gsttools/qgstreamervideoprobecontrol.o \
             gsttools/qgstreamervideorenderer.o \
             gsttools/qgstreamervideorendererinterface.o \
             gsttools/qgstreamervideowidget.o \
             gsttools/qgstreamervideowindow.o \
             gsttools/qgstutils.o \
             gsttools/qgstvideobuffer.o \
             gsttools/qgstvideorendererplugin.o \
             gsttools/qgstvideorenderersink.o
ifeq ($(TARGET_ARCH),x86)
DEFINES   := $(DEFINES)
INCLUDES  := $(INCLUDES) -I/usr/lib/i386-linux-gnu/glib-2.0/include
endif
ifeq ($(TARGET_ARCH),x64)
DEFINES   := $(DEFINES)
INCLUDES  := $(INCLUDES) -I/usr/lib/x86_64-linux-gnu/glib-2.0/include
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
