
include platforms/detect-host.mk

NAME       = qt-multimediaplugins
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
             -I$(INCDIRQT) \
             -I$(SRCDIR) \
             -I$(UICDIR) \
             -I$(MOCDIR) \
             -I$(INCDIR)/qt \
             -I$(INCDIR)/qt/QtCore \
             -I$(INCDIR)/qt/QtGui \
             -I$(INCDIR)/qt/QtOpenGL \
             -I$(INCDIR)/qt/QtWidgets \
             -I$(INCDIR)/qt/QtMultimedia \
             -I$(INCDIR)/qt/QtMultimedia/private \
             -I$(INCDIR)/qt/QtMultimediaWidgets
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
CXXFLAGS  := $(CXXFLAGS) \
             -Wno-nonportable-include-path \
             -Wno-microsoft-exception-spec \
             -Wno-microsoft-unqualified-friend \
             -Wno-enum-compare-switch \
             -Wno-switch
CFLAGS    := $(CFLAGS)
INCLUDES  := $(INCLUDES) \
             -I$(INCDIR)/qt/mkspecs/win32-clang-msvc \
             -I$(SRCDIR)/common/evr \
             -I$(SRCDIR)/directshow/camera \
             -I$(SRCDIR)/directshow/common \
             -I$(SRCDIR)/directshow/player \
             -I$(SRCDIR)/directshow \
             -I$(SRCDIR)/wmf/decoder \
             -I$(SRCDIR)/wmf/player \
             -I$(SRCDIR)/wmf
MOCH      := $(MOCH) \
             common/evr/evrvideowindowcontrol.cpp \
             wmf/decoder/mfaudiodecodercontrol.cpp \
             wmf/decoder/mfdecoderservice.cpp \
             wmf/decoder/mfdecodersourcereader.cpp \
             wmf/player/mfaudioendpointcontrol.cpp \
             wmf/player/mfaudioprobecontrol.cpp \
             wmf/player/mfmetadatacontrol.cpp \
             wmf/player/mfplayercontrol.cpp \
             wmf/player/mfplayerservice.cpp \
             wmf/player/mfplayersession.cpp \
             wmf/player/mfvideoprobecontrol.cpp \
             wmf/player/mfvideorenderercontrol.cpp \
             wmf/wmfserviceplugin.cpp \
             wmf/sourceresolver.cpp \
             wmf/mfstream.cpp \
             directshow/camera/directshowcameracapturebufferformatcontrol.cpp \
             directshow/camera/directshowcameracapturedestinationcontrol.cpp \
             directshow/camera/directshowcameraexposurecontrol.cpp \
             directshow/camera/directshowcameraimageencodercontrol.cpp \
             directshow/camera/directshowcamerazoomcontrol.cpp \
             directshow/camera/dscameracontrol.cpp \
             directshow/camera/dscameraimageprocessingcontrol.cpp \
             directshow/camera/dscameraservice.cpp \
             directshow/camera/dscamerasession.cpp \
             directshow/camera/dsimagecapturecontrol.cpp \
             directshow/camera/dsvideodevicecontrol.cpp \
             directshow/camera/dsvideorenderer.cpp \
             directshow/common/directshowaudioprobecontrol.cpp \
             directshow/common/directshoweventloop.cpp \
             directshow/common/directshowsamplegrabber.cpp \
             directshow/common/directshowvideoprobecontrol.cpp \
             directshow/player/directshowaudioendpointcontrol.cpp \
             directshow/player/directshowevrvideowindowcontrol.cpp \
             directshow/player/directshowioreader.cpp \
             directshow/player/directshowmetadatacontrol.cpp \
             directshow/player/directshowplayercontrol.cpp \
             directshow/player/directshowplayerservice.cpp \
             directshow/player/directshowvideorenderercontrol.cpp \
             directshow/player/videosurfacefilter.cpp \
             directshow/player/vmr9videowindowcontrol.cpp \
             directshow/dsserviceplugin.cpp
MOCC      := $(MOCC) \
             wmf/player/mfvideorenderercontrol.moc
MOCO      := $(MOCO) \
             evrvideowindowcontrol.omoc \
             mfaudiodecodercontrol.omoc \
             mfdecoderservice.omoc \
             mfdecodersourcereader.omoc \
             mfaudioendpointcontrol.omoc \
             mfaudioprobecontrol.omoc \
             mfmetadatacontrol.omoc \
             mfplayercontrol.omoc \
             mfplayerservice.omoc \
             mfplayersession.omoc \
             mfvideoprobecontrol.omoc \
             wmfserviceplugin.omoc \
             sourceresolver.omoc \
             mfstream.omoc \
             directshowcameracapturebufferformatcontrol.omoc \
             directshowcameracapturedestinationcontrol.omoc \
             directshowcameraexposurecontrol.omoc \
             directshowcameraimageencodercontrol.omoc \
             directshowcamerazoomcontrol.omoc \
             dscameracontrol.omoc \
             dscameraimageprocessingcontrol.omoc \
             dscameraservice.omoc \
             dscamerasession.omoc \
             dsimagecapturecontrol.omoc \
             dsvideodevicecontrol.omoc \
             dsvideorenderer.omoc \
             directshowaudioprobecontrol.omoc \
             directshoweventloop.omoc \
             directshowsamplegrabber.omoc \
             directshowvideoprobecontrol.omoc \
             directshowaudioendpointcontrol.omoc \
             directshowevrvideowindowcontrol.omoc \
             directshowioreader.omoc \
             directshowmetadatacontrol.omoc \
             directshowplayercontrol.omoc \
             directshowplayerservice.omoc \
             directshowvideorenderercontrol.omoc \
             videosurfacefilter.omoc \
             vmr9videowindowcontrol.omoc \
             dsserviceplugin.omoc
RCCH      := $(RCCH)
RCCO      := $(RCCO)
UICH      := $(UICH)
OBJS      := $(OBJS) \
             common/evr/evrcustompresenter.o \
             common/evr/evrd3dpresentengine.o \
             common/evr/evrdefs.o \
             common/evr/evrhelpers.o \
             common/evr/evrvideowindowcontrol.o \
             directshow/camera/directshowcameracapturebufferformatcontrol.o \
             directshow/camera/directshowcameracapturedestinationcontrol.o \
             directshow/camera/directshowcameraexposurecontrol.o \
             directshow/camera/directshowcameraimageencodercontrol.o \
             directshow/camera/directshowcamerazoomcontrol.o \
             directshow/camera/dscameracontrol.o \
             directshow/camera/dscameraimageprocessingcontrol.o \
             directshow/camera/dscameraservice.o \
             directshow/camera/dscamerasession.o \
             directshow/camera/dscameraviewfindersettingscontrol.o \
             directshow/camera/dsimagecapturecontrol.o \
             directshow/camera/dsvideodevicecontrol.o \
             directshow/camera/dsvideorenderer.o \
             directshow/common/directshowaudioprobecontrol.o \
             directshow/common/directshowbasefilter.o \
             directshow/common/directshoweventloop.o \
             directshow/common/directshowmediatype.o \
             directshow/common/directshowmediatypeenum.o \
             directshow/common/directshowpin.o \
             directshow/common/directshowpinenum.o \
             directshow/common/directshowsamplegrabber.o \
             directshow/common/directshowutils.o \
             directshow/common/directshowvideobuffer.o \
             directshow/common/directshowvideoprobecontrol.o \
             directshow/player/directshowaudioendpointcontrol.o \
             directshow/player/directshowevrvideowindowcontrol.o \
             directshow/player/directshowioreader.o \
             directshow/player/directshowiosource.o \
             directshow/player/directshowmetadatacontrol.o \
             directshow/player/directshowplayercontrol.o \
             directshow/player/directshowplayerservice.o \
             directshow/player/directshowvideorenderercontrol.o \
             directshow/player/videosurfacefilter.o \
             directshow/player/vmr9videowindowcontrol.o \
             directshow/dsserviceplugin.o \
             wmf/decoder/mfaudiodecodercontrol.o \
             wmf/decoder/mfdecoderservice.o \
             wmf/decoder/mfdecodersourcereader.o \
             wmf/player/mfactivate.o \
             wmf/player/mfaudioendpointcontrol.o \
             wmf/player/mfaudioprobecontrol.o \
             wmf/player/mfevrvideowindowcontrol.o \
             wmf/player/mfmetadatacontrol.o \
             wmf/player/mfplayercontrol.o \
             wmf/player/mfplayerservice.o \
             wmf/player/mfplayersession.o \
             wmf/player/mftvideo.o \
             wmf/player/mfvideoprobecontrol.o \
             wmf/player/mfvideorenderercontrol.o \
             wmf/player/samplegrabber.o \
             wmf/mfstream.o \
             wmf/sourceresolver.o \
             wmf/wmfserviceplugin.o
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
CXXFLAGS  := $(CXXFLAGS) \
             -Wno-deprecated-declarations
CFLAGS    := $(CFLAGS)
INCLUDES  := $(INCLUDES) -I$(INCDIR)/qt/mkspecs/macx-clang
MOCH      := $(MOCH) \
             avfoundation/camera/avfaudioinputselectorcontrol.cpp \
             avfoundation/camera/avfcameracontrol.cpp \
             avfoundation/camera/avfcameradevicecontrol.cpp \
             avfoundation/camera/avfcameraexposurecontrol.cpp \
             avfoundation/camera/avfcameraflashcontrol.mm \
             avfoundation/camera/avfcamerafocuscontrol.cpp \
             avfoundation/camera/avfcamerainfocontrol.mm \
             avfoundation/camera/avfcamerametadatacontrol.cpp \
             avfoundation/camera/avfcamerarenderercontrol.cpp \
             avfoundation/camera/avfcameraservice.cpp \
             avfoundation/camera/avfcameraserviceplugin.mm \
             avfoundation/camera/avfcamerasession.cpp \
             avfoundation/camera/avfcameraviewfindersettingscontrol.cpp \
             avfoundation/camera/avfcamerazoomcontrol.mm \
             avfoundation/camera/avfimagecapturecontrol.cpp \
             avfoundation/camera/avfimageencodercontrol.cpp \
             avfoundation/camera/avfmediarecordercontrol.cpp \
             avfoundation/camera/avfmediavideoprobecontrol.mm \
             avfoundation/camera/avfvideoencodersettingscontrol.cpp \
             avfoundation/mediaplayer/avfdisplaylink.mm \
             avfoundation/mediaplayer/avfmediaplayercontrol.mm \
             avfoundation/mediaplayer/avfmediaplayermetadatacontrol.mm \
             avfoundation/mediaplayer/avfmediaplayerserviceplugin.mm \
             avfoundation/mediaplayer/avfmediaplayersession.mm \
             avfoundation/mediaplayer/avfvideorenderercontrol.mm \
             avfoundation/mediaplayer/avfvideowidgetcontrol.cpp \
             avfoundation/mediaplayer/avfvideowindowcontrol.cpp \
             coreaudio/coreaudiooutput.cpp \
             coreaudio/coreaudioinput.cpp \
             coreaudio/coreaudiodeviceinfo.cpp \
             coreaudio/coreaudioplugin.cpp
MOCC      := $(MOCC)
MOCO      := $(MOCO) \
             avfcameraflashcontrol.omocmm \
             avfcamerainfocontrol.omocmm \
             avfcameraserviceplugin.omocmm \
             avfmediavideoprobecontrol.omocmm \
             avfdisplaylink.omocmm \
             avfmediaplayercontrol.omocmm \
             avfmediaplayermetadatacontrol.omocmm \
             avfmediaplayerserviceplugin.omocmm \
             avfmediaplayersession.omocmm \
             avfvideorenderercontrol.omocmm
RCCH      := $(RCCH)
RCCO      := $(RCCO)
UICH      := $(UICH)
OBJS      := $(OBJS) \
             avfoundation/camera/avfaudioencodersettingscontrol.omm \
             avfoundation/camera/avfaudioinputselectorcontrol.omm \
             avfoundation/camera/avfcameracontrol.omm \
             avfoundation/camera/avfcameradevicecontrol.omm \
             avfoundation/camera/avfcameraexposurecontrol.omm \
             avfoundation/camera/avfcameraflashcontrol.omm \
             avfoundation/camera/avfcamerafocuscontrol.omm \
             avfoundation/camera/avfcamerainfocontrol.omm \
             avfoundation/camera/avfcamerametadatacontrol.omm \
             avfoundation/camera/avfcamerarenderercontrol.omm \
             avfoundation/camera/avfcameraservice.omm \
             avfoundation/camera/avfcameraserviceplugin.omm \
             avfoundation/camera/avfcamerasession.omm \
             avfoundation/camera/avfcamerautility.omm \
             avfoundation/camera/avfcameraviewfindersettingscontrol.omm \
             avfoundation/camera/avfimagecapturecontrol.omm \
             avfoundation/camera/avfimageencodercontrol.omm \
             avfoundation/camera/avfmediacontainercontrol.omm \
             avfoundation/camera/avfmediarecordercontrol.omm \
             avfoundation/camera/avfmediavideoprobecontrol.omm \
             avfoundation/camera/avfstoragelocation.omm \
             avfoundation/camera/avfvideoencodersettingscontrol.omm \
             avfoundation/mediaplayer/avfdisplaylink.omm \
             avfoundation/mediaplayer/avfmediaplayercontrol.omm \
             avfoundation/mediaplayer/avfmediaplayermetadatacontrol.omm \
             avfoundation/mediaplayer/avfmediaplayerservice.omm \
             avfoundation/mediaplayer/avfmediaplayerserviceplugin.omm \
             avfoundation/mediaplayer/avfmediaplayersession.omm \
             avfoundation/mediaplayer/avfvideoframerenderer.omm \
             avfoundation/mediaplayer/avfvideooutput.omm \
             avfoundation/mediaplayer/avfvideorenderercontrol.omm \
             avfoundation/mediaplayer/avfvideowidget.omm \
             avfoundation/mediaplayer/avfvideowidgetcontrol.omm \
             avfoundation/mediaplayer/avfvideowindowcontrol.omm \
             coreaudio/coreaudiodeviceinfo.omm \
             coreaudio/coreaudioinput.omm \
             coreaudio/coreaudiooutput.omm \
             coreaudio/coreaudioplugin.omm \
             coreaudio/coreaudioutils.omm
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
DEFINES   := $(DEFINES) -DGST_USE_UNSTABLE_API
CXXFLAGS  := $(CXXFLAGS)
CFLAGS    := $(CFLAGS)
INCLUDES  := $(INCLUDES) \
             -I$(INCDIR)/qt/mkspecs/linux-clang \
             -I /usr/include/gstreamer-1.0 \
             -I /usr/include/glib-2.0
MOCH      := $(MOCH) \
             gstreamer/audiodecoder/qgstreameraudiodecodercontrol.cpp \
             gstreamer/audiodecoder/qgstreameraudiodecoderservice.cpp \
             gstreamer/audiodecoder/qgstreameraudiodecoderserviceplugin.cpp \
             gstreamer/audiodecoder/qgstreameraudiodecodersession.cpp \
             gstreamer/camerabin/camerabinaudioencoder.cpp \
             gstreamer/camerabin/camerabincapturebufferformat.cpp \
             gstreamer/camerabin/camerabincapturedestination.cpp \
             gstreamer/camerabin/camerabincontainer.cpp \
             gstreamer/camerabin/camerabincontrol.cpp \
             gstreamer/camerabin/camerabinexposure.cpp \
             gstreamer/camerabin/camerabinflash.cpp \
             gstreamer/camerabin/camerabinfocus.cpp \
             gstreamer/camerabin/camerabinimagecapture.cpp \
             gstreamer/camerabin/camerabinimageencoder.cpp \
             gstreamer/camerabin/camerabinimageprocessing.cpp \
             gstreamer/camerabin/camerabininfocontrol.cpp \
             gstreamer/camerabin/camerabinlocks.cpp \
             gstreamer/camerabin/camerabinmetadata.cpp \
             gstreamer/camerabin/camerabinrecorder.cpp \
             gstreamer/camerabin/camerabinresourcepolicy.cpp \
             gstreamer/camerabin/camerabinservice.cpp \
             gstreamer/camerabin/camerabinserviceplugin.cpp \
             gstreamer/camerabin/camerabinsession.cpp \
             gstreamer/camerabin/camerabinv4limageprocessing.cpp \
             gstreamer/camerabin/camerabinvideoencoder.cpp \
             gstreamer/camerabin/camerabinviewfindersettings.cpp \
             gstreamer/camerabin/camerabinviewfindersettings2.cpp \
             gstreamer/camerabin/camerabinzoom.cpp \
             gstreamer/mediacapture/qgstreameraudioencode.cpp \
             gstreamer/mediacapture/qgstreamercameracontrol.cpp \
             gstreamer/mediacapture/qgstreamercapturemetadatacontrol.cpp \
             gstreamer/mediacapture/qgstreamercaptureservice.cpp \
             gstreamer/mediacapture/qgstreamercaptureserviceplugin.cpp \
             gstreamer/mediacapture/qgstreamercapturesession.cpp \
             gstreamer/mediacapture/qgstreamerimagecapturecontrol.cpp \
             gstreamer/mediacapture/qgstreamerimageencode.cpp \
             gstreamer/mediacapture/qgstreamermediacontainercontrol.cpp \
             gstreamer/mediacapture/qgstreamerrecordercontrol.cpp \
             gstreamer/mediacapture/qgstreamerv4l2input.cpp \
             gstreamer/mediacapture/qgstreamervideoencode.cpp \
             gstreamer/mediaplayer/qgstreameravailabilitycontrol.cpp \
             gstreamer/mediaplayer/qgstreamermetadataprovider.cpp \
             gstreamer/mediaplayer/qgstreamerplayerservice.cpp \
             gstreamer/mediaplayer/qgstreamerplayerserviceplugin.cpp \
             gstreamer/mediaplayer/qgstreamerstreamscontrol.cpp \
             pulseaudio/qaudiodeviceinfo_pulse.cpp \
             pulseaudio/qaudioinput_pulse.cpp \
             pulseaudio/qaudiooutput_pulse.cpp \
             pulseaudio/qpulseaudioengine.cpp \
             pulseaudio/qpulseaudioplugin.cpp
MOCC      := $(MOCC)
MOCO      := $(MOCO) \
             qgstreameraudiodecodercontrol.omoc \
             qgstreameraudiodecoderservice.omoc \
             qgstreameraudiodecoderserviceplugin.omoc \
             qgstreameraudiodecodersession.omoc \
             camerabinaudioencoder.omoc \
             camerabincapturebufferformat.omoc \
             camerabincapturedestination.omoc \
             camerabincontainer.omoc \
             camerabincontrol.omoc \
             camerabinexposure.omoc \
             camerabinflash.omoc \
             camerabinfocus.omoc \
             camerabinimagecapture.omoc \
             camerabinimageencoder.omoc \
             camerabinimageprocessing.omoc \
             camerabininfocontrol.omoc \
             camerabinlocks.omoc \
             camerabinmetadata.omoc \
             camerabinrecorder.omoc \
             camerabinresourcepolicy.omoc \
             camerabinservice.omoc \
             camerabinserviceplugin.omoc \
             camerabinsession.omoc \
             camerabinv4limageprocessing.omoc \
             camerabinvideoencoder.omoc \
             camerabinviewfindersettings.omoc \
             camerabinviewfindersettings2.omoc \
             camerabinzoom.omoc \
             qgstreameraudioencode.omoc \
             qgstreamercameracontrol.omoc \
             qgstreamercapturemetadatacontrol.omoc \
             qgstreamercaptureservice.omoc \
             qgstreamercaptureserviceplugin.omoc \
             qgstreamercapturesession.omoc \
             qgstreamerimagecapturecontrol.omoc \
             qgstreamerimageencode.omoc \
             qgstreamermediacontainercontrol.omoc \
             qgstreamerrecordercontrol.omoc \
             qgstreamerv4l2input.omoc \
             qgstreamervideoencode.omoc \
             qgstreameravailabilitycontrol.omoc \
             qgstreamermetadataprovider.omoc \
             qgstreamerplayerservice.omoc \
             qgstreamerplayerserviceplugin.omoc \
             qgstreamerstreamscontrol.omoc \
             qaudiodeviceinfo_pulse.omoc \
             qpulseaudioengine.omoc \
             qpulseaudioplugin.omoc
RCCH      := $(RCCH)
RCCO      := $(RCCO)
UICH      := $(UICH)
OBJS      := $(OBJS) \
             gstreamer/audiodecoder/qgstreameraudiodecodercontrol.o \
             gstreamer/audiodecoder/qgstreameraudiodecoderservice.o \
             gstreamer/audiodecoder/qgstreameraudiodecoderserviceplugin.o \
             gstreamer/audiodecoder/qgstreameraudiodecodersession.o \
             gstreamer/camerabin/camerabinaudioencoder.o \
             gstreamer/camerabin/camerabincapturebufferformat.o \
             gstreamer/camerabin/camerabincapturedestination.o \
             gstreamer/camerabin/camerabincontainer.o \
             gstreamer/camerabin/camerabincontrol.o \
             gstreamer/camerabin/camerabinexposure.o \
             gstreamer/camerabin/camerabinflash.o \
             gstreamer/camerabin/camerabinfocus.o \
             gstreamer/camerabin/camerabinimagecapture.o \
             gstreamer/camerabin/camerabinimageencoder.o \
             gstreamer/camerabin/camerabinimageprocessing.o \
             gstreamer/camerabin/camerabininfocontrol.o \
             gstreamer/camerabin/camerabinlocks.o \
             gstreamer/camerabin/camerabinmetadata.o \
             gstreamer/camerabin/camerabinrecorder.o \
             gstreamer/camerabin/camerabinresourcepolicy.o \
             gstreamer/camerabin/camerabinservice.o \
             gstreamer/camerabin/camerabinserviceplugin.o \
             gstreamer/camerabin/camerabinsession.o \
             gstreamer/camerabin/camerabinv4limageprocessing.o \
             gstreamer/camerabin/camerabinvideoencoder.o \
             gstreamer/camerabin/camerabinviewfindersettings.o \
             gstreamer/camerabin/camerabinviewfindersettings2.o \
             gstreamer/camerabin/camerabinzoom.o \
             gstreamer/mediacapture/qgstreameraudioencode.o \
             gstreamer/mediacapture/qgstreamercameracontrol.o \
             gstreamer/mediacapture/qgstreamercapturemetadatacontrol.o \
             gstreamer/mediacapture/qgstreamercaptureservice.o \
             gstreamer/mediacapture/qgstreamercaptureserviceplugin.o \
             gstreamer/mediacapture/qgstreamercapturesession.o \
             gstreamer/mediacapture/qgstreamerimagecapturecontrol.o \
             gstreamer/mediacapture/qgstreamerimageencode.o \
             gstreamer/mediacapture/qgstreamermediacontainercontrol.o \
             gstreamer/mediacapture/qgstreamerrecordercontrol.o \
             gstreamer/mediacapture/qgstreamerv4l2input.o \
             gstreamer/mediacapture/qgstreamervideoencode.o \
             gstreamer/mediaplayer/qgstreameravailabilitycontrol.o \
             gstreamer/mediaplayer/qgstreamermetadataprovider.o \
             gstreamer/mediaplayer/qgstreamerplayerservice.o \
             gstreamer/mediaplayer/qgstreamerplayerserviceplugin.o \
             gstreamer/mediaplayer/qgstreamerstreamscontrol.o \
             pulseaudio/qaudiodeviceinfo_pulse.o \
             pulseaudio/qaudioinput_pulse.o \
             pulseaudio/qaudiooutput_pulse.o \
             pulseaudio/qpulseaudioengine.o \
             pulseaudio/qpulseaudioplugin.o \
             pulseaudio/qpulsehelpers.o
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
INCLUDES  := $(INCLUDES) -I/usr/lib/arm-linux-gnueabihf/glib-2.0/include
endif
ifeq ($(TARGET_ARCH),arm64)
DEFINES   := $(DEFINES)
INCLUDES  := $(INCLUDES) -I/usr/lib/aarch64-linux-gnu/glib-2.0/include
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
OBJS      := $(OBJS) \
             avfoundation/camera/avfaudioencodersettingscontrol.omm
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
	$(QTRCC) --name $(basename $(@F)) $(@:$(RCCDIR)/%.cpp=$(SRCDIR)/%.qrc) --output $(@:$(RCCDIR)/%.cpp=$(RCCDIR)/qrc_$(@F))

$(OBJDIR)/%.orcc:
	@echo [CXX] $@
	$(CXX) $(CPUFLAGS) $(DEFINES) $(INCLUDES) $(CXXFLAGS) -c $(@:$(OBJDIR)/%.orcc=$(RCCDIR)/qrc_$(@F:.orcc=.cpp)) -o $@

################################################################################################
# UIC

UICH := $(patsubst %,$(UICDIR)/%,$(UICH))

$(UICDIR)/%.h:
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
