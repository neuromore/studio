setlocal
set MOC=%MOC% -DQ_OS_WIN -I../../include/qt -I../../include/qt/QtCore -I../../include/qt/QtWidgets -I../../include/qt/QtMultimedia
set RCC=%RCC%
set UIC=%UIC%
set DIRIN=../../include/qt/QtMultimedia
set DIRSRC=../../src/qt-multimedia
set DIROUT=../../src/qt-multimedia/.moc
set DIROUTUIC=../../src/qt-multimedia/.uic
set DIROUTRCC=../../src/qt-multimedia/.rcc

echo MOC for root
%MOC% %DIRIN%/qmediacontrol.h -o %DIROUT%/moc_qmediacontrol.cpp
%MOC% %DIRIN%/qmediaobject.h -o %DIROUT%/moc_qmediaobject.cpp
%MOC% %DIRIN%/qmediaservice.h -o %DIROUT%/moc_qmediaservice.cpp
%MOC% %DIRIN%/qmediaserviceproviderplugin.h -o %DIROUT%/moc_qmediaserviceproviderplugin.cpp
%MOC% %DIRIN%/private/qmediaserviceprovider_p.h -o %DIROUT%/moc_qmediaserviceprovider_p.cpp

echo MOC for 'audio'
%MOC% %DIRIN%/qaudiodecoder.h -o %DIROUT%/moc_qaudiodecoder.cpp
%MOC% %DIRIN%/qaudioinput.h -o %DIROUT%/moc_qaudioinput.cpp
%MOC% %DIRIN%/qaudiooutput.h -o %DIROUT%/moc_qaudiooutput.cpp
CALL :moch qaudioprobe
%MOC% %DIRIN%/qaudiosystem.h -o %DIROUT%/moc_qaudiosystem.cpp
%MOC% %DIRIN%/qaudiosystemplugin.h -o %DIROUT%/moc_qaudiosystemplugin.cpp
%MOC% %DIRIN%/qsound.h -o %DIROUT%/moc_qsound.cpp
%MOC% %DIRIN%/qsoundeffect.h -o %DIROUT%/moc_qsoundeffect.cpp
%MOC% %DIRIN%/private/qsamplecache_p.h -o %DIROUT%/moc_qsamplecache_p.cpp
%MOC% %DIRIN%/private/qsoundeffect_qaudio_p.h -o %DIROUT%/moc_qsoundeffect_qaudio_p.cpp
%MOC% %DIRIN%/private/qwavedecoder_p.h -o %DIROUT%/moc_qwavedecoder_p.cpp

echo MOC for 'camera'
%MOC% %DIRIN%/qcamera.h -o %DIROUT%/moc_qcamera.cpp
%MOC% %DIRIN%/qcameraexposure.h -o %DIROUT%/moc_qcameraexposure.cpp
%MOC% %DIRIN%/qcamerafocus.h -o %DIROUT%/moc_qcamerafocus.cpp
%MOC% %DIRIN%/qcameraimagecapture.h -o %DIROUT%/moc_qcameraimagecapture.cpp
%MOC% %DIRIN%/qcameraimageprocessing.h -o %DIROUT%/moc_qcameraimageprocessing.cpp

echo MOC for 'controls'
%MOC% %DIRIN%/qaudiodecodercontrol.h -o %DIROUT%/moc_qaudiodecodercontrol.cpp
%MOC% %DIRIN%/qaudioencodersettingscontrol.h -o %DIROUT%/moc_qaudioencodersettingscontrol.cpp
%MOC% %DIRIN%/qaudioinputselectorcontrol.h -o %DIROUT%/moc_qaudioinputselectorcontrol.cpp
%MOC% %DIRIN%/qaudiooutputselectorcontrol.h -o %DIROUT%/moc_qaudiooutputselectorcontrol.cpp
%MOC% %DIRIN%/qaudiorolecontrol.h -o %DIROUT%/moc_qaudiorolecontrol.cpp
%MOC% %DIRIN%/qcameracapturebufferformatcontrol.h -o %DIROUT%/moc_qcameracapturebufferformatcontrol.cpp
%MOC% %DIRIN%/qcameracapturedestinationcontrol.h -o %DIROUT%/moc_qcameracapturedestinationcontrol.cpp
%MOC% %DIRIN%/qcameracontrol.h -o %DIROUT%/moc_qcameracontrol.cpp
%MOC% %DIRIN%/qcameraexposurecontrol.h -o %DIROUT%/moc_qcameraexposurecontrol.cpp
%MOC% %DIRIN%/qcamerafocuscontrol.h -o %DIROUT%/moc_qcamerafocuscontrol.cpp
%MOC% %DIRIN%/qcameraimagecapturecontrol.h -o %DIROUT%/moc_qcameraimagecapturecontrol.cpp
CALL :moch qcamerafeedbackcontrol
%MOC% %DIRIN%/qcameraflashcontrol.h -o %DIROUT%/moc_qcameraflashcontrol.cpp
%MOC% %DIRIN%/qcameraimageprocessingcontrol.h -o %DIROUT%/moc_qcameraimageprocessingcontrol.cpp
%MOC% %DIRIN%/qcamerainfocontrol.h -o %DIROUT%/moc_qcamerainfocontrol.cpp
%MOC% %DIRIN%/qcameralockscontrol.h -o %DIROUT%/moc_qcameralockscontrol.cpp
%MOC% %DIRIN%/qcameraviewfindersettingscontrol.h -o %DIROUT%/moc_qcameraviewfindersettingscontrol.cpp
%MOC% %DIRIN%/qcamerazoomcontrol.h -o %DIROUT%/moc_qcamerazoomcontrol.cpp
%MOC% %DIRIN%/qcustomaudiorolecontrol.h -o %DIROUT%/moc_qcustomaudiorolecontrol.cpp
%MOC% %DIRIN%/qimageencodercontrol.h -o %DIROUT%/moc_qimageencodercontrol.cpp
%MOC% %DIRIN%/qmediaaudioprobecontrol.h -o %DIROUT%/moc_qmediaaudioprobecontrol.cpp
%MOC% %DIRIN%/qmediaavailabilitycontrol.h -o %DIROUT%/moc_qmediaavailabilitycontrol.cpp
%MOC% %DIRIN%/qmediacontainercontrol.h -o %DIROUT%/moc_qmediacontainercontrol.cpp
CALL :moch qmediagaplessplaybackcontrol
%MOC% %DIRIN%/qmedianetworkaccesscontrol.h -o %DIROUT%/moc_qmedianetworkaccesscontrol.cpp
%MOC% %DIRIN%/qmediaplayercontrol.h -o %DIROUT%/moc_qmediaplayercontrol.cpp
%MOC% %DIRIN%/qmediarecordercontrol.h -o %DIROUT%/moc_qmediarecordercontrol.cpp
CALL :mocs qmediaresourcepolicyplugin_p private
CALL :mocs qmediaresourceset_p private
%MOC% %DIRIN%/qmediastreamscontrol.h -o %DIROUT%/moc_qmediastreamscontrol.cpp
%MOC% %DIRIN%/qmediavideoprobecontrol.h -o %DIROUT%/moc_qmediavideoprobecontrol.cpp
%MOC% %DIRIN%/qmetadatareadercontrol.h -o %DIROUT%/moc_qmetadatareadercontrol.cpp
%MOC% %DIRIN%/qmetadatawritercontrol.h -o %DIROUT%/moc_qmetadatawritercontrol.cpp
%MOC% %DIRIN%/qradiodatacontrol.h -o %DIROUT%/moc_qradiodatacontrol.cpp
%MOC% %DIRIN%/qradiotunercontrol.h -o %DIROUT%/moc_qradiotunercontrol.cpp
%MOC% %DIRIN%/qvideodeviceselectorcontrol.h -o %DIROUT%/moc_qvideodeviceselectorcontrol.cpp
%MOC% %DIRIN%/qvideoencodersettingscontrol.h -o %DIROUT%/moc_qvideoencodersettingscontrol.cpp
%MOC% %DIRIN%/qvideorenderercontrol.h -o %DIROUT%/moc_qvideorenderercontrol.cpp
%MOC% %DIRIN%/qvideowindowcontrol.h -o %DIROUT%/moc_qvideowindowcontrol.cpp
%MOC% %DIRIN%/private/qmediaplaylistcontrol_p.h -o %DIROUT%/moc_qmediaplaylistcontrol_p.cpp
%MOC% %DIRIN%/private/qmediaplaylistsourcecontrol_p.h -o %DIROUT%/moc_qmediaplaylistsourcecontrol_p.cpp

echo MOC for 'playback'
%MOC% %DIRIN%/qmediaplayer.h -o %DIROUT%/moc_qmediaplayer.cpp
%MOC% %DIRIN%/qmediaplaylist.h -o %DIROUT%/moc_qmediaplaylist.cpp
%MOC% %DIRIN%/private/qmedianetworkplaylistprovider_p.h -o %DIROUT%/moc_qmedianetworkplaylistprovider_p.cpp
%MOC% %DIRIN%/private/qmediaplaylistioplugin_p.h -o %DIROUT%/moc_qmediaplaylistioplugin_p.cpp
%MOC% %DIRIN%/private/qmediaplaylistnavigator_p.h -o %DIROUT%/moc_qmediaplaylistnavigator_p.cpp
%MOC% %DIRIN%/private/qmediaplaylistprovider_p.h -o %DIROUT%/moc_qmediaplaylistprovider_p.cpp
%MOC% %DIRIN%/private/qmediaplaylist_p.h -o %DIROUT%/moc_qmediaplaylist_p.cpp
CALL :mocs qplaylistfileparser_p private

echo MOC for 'radio'
CALL :moch qradiodata
CALL :moch qradiotuner

echo MOC for 'recording'
CALL :moch qaudiorecorder
CALL :moch qmediarecorder

echo MOC for 'video'
CALL :moch qabstractvideofilter
CALL :moch qabstractvideosurface
CALL :mocs qvideooutputorientationhandler_p private
CALL :moch qvideoprobe
CALL :mocs qvideosurfaceoutput_p private

goto :eof

:moch
%MOC% %DIRIN%/%1.h -o %DIROUT%/moc_%1.cpp
goto:eof

:mocs
%MOC% %DIRIN%/%2/%1.h -o %DIROUT%/moc_%1.cpp
goto:eof

:mocc
%MOC% %DIRSRC%/%2/%1.cpp -o %DIROUT%/%1.moc
goto:eof
