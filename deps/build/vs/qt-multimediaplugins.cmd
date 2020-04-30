setlocal
set MOC=%MOC% -DQ_OS_WIN -I../../include/qt -I../../include/qt/QtMultimedia
set RCC=%RCC%
set UIC=%UIC%
set DIRIN=../../src/qt-multimediaplugins
set DIRSRC=../../src/qt-multimediaplugins
set DIROUT=../../src/qt-multimediaplugins/.moc
set DIROUTUIC=../../src/qt-multimediaplugins/.uic
set DIROUTRCC=../../src/qt-multimediaplugins/.rcc

echo MOC for root

%MOC% %DIRIN%/common/evr/evrvideowindowcontrol.h -o %DIROUT%/moc_evrvideowindowcontrol.cpp

%MOC% %DIRIN%/wmf/decoder/mfaudiodecodercontrol.h -o %DIROUT%/moc_mfaudiodecodercontrol.cpp
%MOC% %DIRIN%/wmf/decoder/mfdecoderservice.h -o %DIROUT%/moc_mfdecoderservice.cpp
%MOC% %DIRIN%/wmf/decoder/mfdecodersourcereader.h -o %DIROUT%/moc_mfdecodersourcereader.cpp
%MOC% %DIRIN%/wmf/player/mfaudioendpointcontrol.h -o %DIROUT%/moc_mfaudioendpointcontrol.cpp
%MOC% %DIRIN%/wmf/player/mfaudioprobecontrol.h -o %DIROUT%/moc_mfaudioprobecontrol.cpp
%MOC% %DIRIN%/wmf/player/mfmetadatacontrol.h -o %DIROUT%/moc_mfmetadatacontrol.cpp
%MOC% %DIRIN%/wmf/player/mfplayercontrol.h -o %DIROUT%/moc_mfplayercontrol.cpp
%MOC% %DIRIN%/wmf/player/mfplayerservice.h -o %DIROUT%/moc_mfplayerservice.cpp
%MOC% %DIRIN%/wmf/player/mfplayersession.h -o %DIROUT%/moc_mfplayersession.cpp
%MOC% %DIRIN%/wmf/player/mfvideoprobecontrol.h -o %DIROUT%/moc_mfvideoprobecontrol.cpp
%MOC% %DIRIN%/wmf/player/mfvideorenderercontrol.h -o %DIROUT%/moc_mfvideorenderercontrol.cpp
%MOC% %DIRIN%/wmf/player/mfvideorenderercontrol.cpp -o %DIROUT%/mfvideorenderercontrol.moc
%MOC% %DIRIN%/wmf/wmfserviceplugin.h -o %DIROUT%/moc_wmfserviceplugin.cpp
%MOC% %DIRIN%/wmf/sourceresolver.h -o %DIROUT%/moc_sourceresolver.cpp
%MOC% %DIRIN%/wmf/mfstream.h -o %DIROUT%/moc_mfstream.cpp

%MOC% %DIRIN%/directshow/camera/directshowcameracapturebufferformatcontrol.h -o %DIROUT%/moc_directshowcameracapturebufferformatcontrol.cpp
%MOC% %DIRIN%/directshow/camera/directshowcameracapturedestinationcontrol.h -o %DIROUT%/moc_directshowcameracapturedestinationcontrol.cpp
%MOC% %DIRIN%/directshow/camera/directshowcameraexposurecontrol.h -o %DIROUT%/moc_directshowcameraexposurecontrol.cpp
%MOC% %DIRIN%/directshow/camera/directshowcameraimageencodercontrol.h -o %DIROUT%/moc_directshowcameraimageencodercontrol.cpp
%MOC% %DIRIN%/directshow/camera/directshowcamerazoomcontrol.h -o %DIROUT%/moc_directshowcamerazoomcontrol.cpp
%MOC% %DIRIN%/directshow/camera/dscameracontrol.h -o %DIROUT%/moc_dscameracontrol.cpp
%MOC% %DIRIN%/directshow/camera/dscameraimageprocessingcontrol.h -o %DIROUT%/moc_dscameraimageprocessingcontrol.cpp
%MOC% %DIRIN%/directshow/camera/dscameraservice.h -o %DIROUT%/moc_dscameraservice.cpp
%MOC% %DIRIN%/directshow/camera/dscamerasession.h -o %DIROUT%/moc_dscamerasession.cpp
%MOC% %DIRIN%/directshow/camera/dsimagecapturecontrol.h -o %DIROUT%/moc_dsimagecapturecontrol.cpp
%MOC% %DIRIN%/directshow/camera/dsvideodevicecontrol.h -o %DIROUT%/moc_dsvideodevicecontrol.cpp
%MOC% %DIRIN%/directshow/camera/dsvideorenderer.h -o %DIROUT%/moc_dsvideorenderer.cpp

%MOC% %DIRIN%/directshow/common/directshowaudioprobecontrol.h -o %DIROUT%/moc_directshowaudioprobecontrol.cpp
%MOC% %DIRIN%/directshow/common/directshoweventloop.h -o %DIROUT%/moc_directshoweventloop.cpp
%MOC% %DIRIN%/directshow/common/directshowsamplegrabber.h -o %DIROUT%/moc_directshowsamplegrabber.cpp
%MOC% %DIRIN%/directshow/common/directshowvideoprobecontrol.h -o %DIROUT%/moc_directshowvideoprobecontrol.cpp
%MOC% %DIRIN%/directshow/player/directshowaudioendpointcontrol.h -o %DIROUT%/moc_directshowaudioendpointcontrol.cpp
%MOC% %DIRIN%/directshow/player/directshowevrvideowindowcontrol.h -o %DIROUT%/moc_directshowevrvideowindowcontrol.cpp
%MOC% %DIRIN%/directshow/player/directshowioreader.h -o %DIROUT%/moc_directshowioreader.cpp
%MOC% %DIRIN%/directshow/player/directshowmetadatacontrol.h -o %DIROUT%/moc_directshowmetadatacontrol.cpp
%MOC% %DIRIN%/directshow/player/directshowplayercontrol.h -o %DIROUT%/moc_directshowplayercontrol.cpp
%MOC% %DIRIN%/directshow/player/directshowplayerservice.h -o %DIROUT%/moc_directshowplayerservice.cpp
%MOC% %DIRIN%/directshow/player/directshowvideorenderercontrol.h -o %DIROUT%/moc_directshowvideorenderercontrol.cpp
%MOC% %DIRIN%/directshow/player/videosurfacefilter.h -o %DIROUT%/moc_videosurfacefilter.cpp
%MOC% %DIRIN%/directshow/player/vmr9videowindowcontrol.h -o %DIROUT%/moc_vmr9videowindowcontrol.cpp
%MOC% %DIRIN%/directshow/dsserviceplugin.h -o %DIROUT%/moc_dsserviceplugin.cpp
